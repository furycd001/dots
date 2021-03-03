/*****************************************************************
ksmserver - the KDE session management server

Copyright (C) 2000 Matthias Ettrich <ettrich@kde.org>

relatively small extensions by Oswald Buddenhagen <ob6@inf.tu-dresden.de>

some code taken from the dcopserver (part of the KDE libraries), which is
Copyright (c) 1999 Matthias Ettrich <ettrich@kde.org>
Copyright (c) 1999 Preston Brown <pbrown@kde.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#ifdef HAVE_VFORK_H
#include <vfork.h>
#endif

#include <qfile.h>
#include <qtextstream.h>
#include <qdatastream.h>
#include <qstack.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qguardedptr.h>
#include <qtimer.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kstddirs.h>
#include <unistd.h>
#include <kapp.h>
#include <knotifyclient.h>
#include <kstaticdeleter.h>
#include <dcopclient.h>

#include "server.h"
#include "global.h"
#include "shutdown.h"

#include "server.moc"

#include <kdebug.h>

extern "C" {
    /* int umask(...); */

static void the_reaper(int /*sig*/)
{
    int status;
#ifdef HAVE_WAITPID
    while(waitpid(-1, &status, WNOHANG) > 0) ;
#else
    wait(&status);
#endif
    signal(SIGCHLD, the_reaper);
}

}
KSMServer* the_server = 0;

KSMClient::KSMClient( SmsConn conn)
{
    smsConn = conn;
    clientId = 0;
    resetState();
}

KSMClient::~KSMClient()
{
    for ( SmProp* prop = properties.first(); prop; prop = properties.next() )
	SmFreeProperty( prop );
    if (clientId) free((void*)clientId);
}

SmProp* KSMClient::property( const char* name ) const
{
    for ( QListIterator<SmProp> it( properties ); it.current(); ++it ) {
	if ( !qstrcmp( it.current()->name, name ) )
	    return it.current();
    }
    return 0;
}

void KSMClient::resetState()
{
    saveYourselfDone = FALSE;
    pendingInteraction = FALSE;
    waitForPhase2 = FALSE;
    wasPhase2 = FALSE;
}

/*
 * This fakes SmsGenerateClientID() in case we can't read our own hostname.
 * In this case SmsGenerateClientID() returns NULL, but we really want a
 * client ID, so we fake one.
 */
static KStaticDeleter<QString> smy_addr;
char * safeSmsGenerateClientID( SmsConn c )
{
    char *ret = SmsGenerateClientID(c);
    if (!ret) {
        static QString *my_addr = 0;
       if (!my_addr) {
           qDebug("Can't get own host name. Your system is severely misconfigured\n");
           my_addr = smy_addr.setObject(new QString);

           /* Faking our IP address, the 0 below is "unknown" address format
	      (1 would be IP, 2 would be DEC-NET format) */
           my_addr->sprintf("0%.8x", KApplication::random());
       }
       /* Needs to be malloc(), to look the same as libSM */
       ret = (char *)malloc(1+9+13+10+4+1 + /*safeness*/ 10);
       static int sequence = 0;
       sprintf(ret, "1%s%.13ld%.10d%.4d", my_addr->latin1(), (long)time(NULL),
           getpid(), sequence);
       sequence = (sequence + 1) % 10000;
    }
    return ret;
}

void KSMClient::registerClient( const char* previousId )
{
    clientId = previousId;
    if ( !clientId )
	clientId = safeSmsGenerateClientID( smsConn );
    SmsRegisterClientReply( smsConn, (char*) clientId );
    SmsSaveYourself( smsConn, SmSaveLocal, FALSE, SmInteractStyleNone, FALSE );
    SmsSaveComplete( smsConn );
}


QString KSMClient::program() const
{
    SmProp* p = property( SmProgram );
    if ( !p || qstrcmp( p->type, SmARRAY8) || p->num_vals < 1)
	return QString::null;
    return QString::fromLatin1( (const char*) p->vals[0].value );
}

QStringList KSMClient::restartCommand() const
{
    QStringList result;
    SmProp* p = property( SmRestartCommand );
    if ( !p || qstrcmp( p->type, SmLISTofARRAY8) || p->num_vals < 1)
	return result;
    for ( int i = 0; i < p->num_vals; i++ )
	result +=QString::fromLatin1( (const char*) p->vals[i].value );
    return result;
}

QStringList KSMClient::discardCommand() const
{
    QStringList result;
    SmProp* p = property( SmDiscardCommand );
    if ( !p || qstrcmp( p->type, SmLISTofARRAY8) || p->num_vals < 1)
	return result;
    for ( int i = 0; i < p->num_vals; i++ )
	result +=QString::fromLatin1( (const char*) p->vals[i].value );
    return result;
}

int KSMClient::restartStyleHint() const
{
    SmProp* p = property( SmRestartStyleHint );
    if ( !p || qstrcmp( p->type, SmCARD8) || p->num_vals < 1)
	return SmRestartIfRunning;
    return *((int*)p->vals[0].value);
}

QString KSMClient::userId() const
{
    SmProp* p = property( SmUserID );
    if ( !p || qstrcmp( p->type, SmARRAY8) || p->num_vals < 1)
	return QString::null;
    return QString::fromLatin1( (const char*) p->vals[0].value );
}


/*! Utility function to execute a command on the local machine. Used
 * to restart applications.
 */
void KSMServer::startApplication( const QStringList& command )
{
    if ( command.isEmpty() )
	return;
    int n = command.count();
    QCString app = command[0].latin1();
    QValueList<QCString> argList;
    for ( int i=1; i < n; i++)
       argList.append( QCString(command[i].latin1()));

      QByteArray params;
      QDataStream stream(params, IO_WriteOnly);
      stream << app << argList;
      kapp->dcopClient()->send(launcher, launcher, "exec_blind(QCString,QValueList<QCString>)", params);
}

/*! Utility function to execute a command on the local machine. Used
 * to discard session data
 */
void KSMServer::executeCommand( const QStringList& command )
{
    if ( command.isEmpty() )
	return;
    int n = command.count();
    QCString app = command[0].latin1();
    char ** argList = new char *[n+1];

    for ( int i=0; i < n; i++)
       argList[i] = (char *) command[i].latin1();
    argList[n] = 0;

    int pid = fork();
    if (pid == -1)
       return;
    if (pid == 0) {
       execvp(app.data(), argList);
       exit(127);
    }
    int status;
    waitpid(pid, &status, 0);
    delete [] argList;
}

IceAuthDataEntry *authDataEntries = 0;
static char *addAuthFile = 0;
static char *remAuthFile = 0;

static IceListenObj *listenObjs = 0;
int numTransports = 0;
static bool only_local = 0;

static Bool HostBasedAuthProc ( char* /*hostname*/)
{
    if (only_local)
	return TRUE;
    else
	return FALSE;
}


Status KSMRegisterClientProc (
    SmsConn 		/* smsConn */,
    SmPointer		managerData,
    char *		previousId
)
{
    KSMClient* client = (KSMClient*) managerData;
    client->registerClient( previousId );
    return 1;
}

void KSMInteractRequestProc (
    SmsConn		/* smsConn */,
    SmPointer		managerData,
    int			dialogType
)
{
    the_server->interactRequest( (KSMClient*) managerData, dialogType );
}

void KSMInteractDoneProc (
    SmsConn		/* smsConn */,
    SmPointer		managerData,
    Bool			cancelShutdown
)
{
    the_server->interactDone( (KSMClient*) managerData, cancelShutdown );
}

void KSMSaveYourselfRequestProc (
    SmsConn		/* smsConn */,
    SmPointer		/* managerData */,
    int  		/* saveType */,
    Bool		/* shutdown */,
    int			/* interactStyle */,
    Bool		fast,
    Bool		/* global */
)
{
    the_server->shutdown( fast );
}

void KSMSaveYourselfPhase2RequestProc (
    SmsConn		/* smsConn */,
    SmPointer		managerData
)
{
    the_server->phase2Request( (KSMClient*) managerData );
}

void KSMSaveYourselfDoneProc (
    SmsConn		/* smsConn */,
    SmPointer		managerData,
    Bool		success
)
{
    the_server->saveYourselfDone( (KSMClient*) managerData, success );
}

void KSMCloseConnectionProc (
    SmsConn		smsConn,
    SmPointer		managerData,
    int			count,
    char **		reasonMsgs
)
{
    the_server->deleteClient( ( KSMClient* ) managerData );
    if ( count )
	SmFreeReasons( count, reasonMsgs );
    IceConn iceConn = SmsGetIceConnection( smsConn );
    SmsCleanUp( smsConn );
    IceSetShutdownNegotiation (iceConn, False);
    IceCloseConnection( iceConn );
}

void KSMSetPropertiesProc (
    SmsConn		/* smsConn */,
    SmPointer		managerData,
    int			numProps,
    SmProp **		props
)
{
    KSMClient* client = ( KSMClient* ) managerData;
    for ( int i = 0; i < numProps; i++ ) {
	SmProp *p = client->property( props[i]->name );
	if ( p )
	    client->properties.removeRef( p );
	client->properties.append( props[i] );
	if ( !qstrcmp( props[i]->name, SmProgram ) )
	    the_server->clientSetProgram( client );
    }

    if ( numProps )
	free( props );

}

void KSMDeletePropertiesProc (
    SmsConn		/* smsConn */,
    SmPointer		managerData,
    int			numProps,
    char **		propNames
)
{
    KSMClient* client = ( KSMClient* ) managerData;
    for ( int i = 0; i < numProps; i++ ) {
	SmProp *p = client->property( propNames[i] );
	if ( p )
	    client->properties.removeRef( p );
    }
}

void KSMGetPropertiesProc (
    SmsConn		smsConn,
    SmPointer		managerData
)
{
    KSMClient* client = ( KSMClient* ) managerData;
    SmProp** props = new SmProp*[client->properties.count()];
    int i = 0;
    for ( SmProp* prop = client->properties.first(); prop; prop = client->properties.next() )
	props[i++] = prop;

    SmsReturnProperties( smsConn, i, props );
    delete [] props;
}


class KSMListener : public QSocketNotifier
{
public:
    KSMListener( IceListenObj obj )
	: QSocketNotifier( IceGetListenConnectionNumber( obj ),
			   QSocketNotifier::Read, 0, 0)
{
    listenObj = obj;
}

    IceListenObj listenObj;
};

class KSMConnection : public QSocketNotifier
{
 public:
  KSMConnection( IceConn conn )
    : QSocketNotifier( IceConnectionNumber( conn ),
		       QSocketNotifier::Read, 0, 0 )
    {
	iceConn = conn;
    }

    IceConn iceConn;
};


/* for printing hex digits */
static void fprintfhex (FILE *fp, unsigned int len, char *cp)
{
    static char hexchars[] = "0123456789abcdef";

    for (; len > 0; len--, cp++) {
	unsigned char s = *cp;
	putc(hexchars[s >> 4], fp);
	putc(hexchars[s & 0x0f], fp);
    }
}

/*
 * We use temporary files which contain commands to add/remove entries from
 * the .ICEauthority file.
 */
static void write_iceauth (FILE *addfp, FILE *removefp, IceAuthDataEntry *entry)
{
    fprintf (addfp,
	     "add %s \"\" %s %s ",
	     entry->protocol_name,
	     entry->network_id,
	     entry->auth_name);
    fprintfhex (addfp, entry->auth_data_length, entry->auth_data);
    fprintf (addfp, "\n");

    fprintf (removefp,
	     "remove protoname=%s protodata=\"\" netid=%s authname=%s\n",
	     entry->protocol_name,
	     entry->network_id,
	     entry->auth_name);
}


#ifndef HAVE_MKSTEMP
static char *unique_filename (const char *path, const char *prefix)
#else
static char *unique_filename (const char *path, const char *prefix, int *pFd)
#endif
{
#ifndef HAVE_MKSTEMP
#ifndef X_NOT_POSIX
    return ((char *) tempnam (path, prefix));
#else
    char tempFile[PATH_MAX];
    char *tmp;

    sprintf (tempFile, "%s/%sXXXXXX", path, prefix);
    tmp = (char *) mktemp (tempFile);
    if (tmp)
	{
	    char *ptr = (char *) malloc (strlen (tmp) + 1);
	    strcpy (ptr, tmp);
	    return (ptr);
	}
    else
	return (NULL);
#endif
#else
    char tempFile[PATH_MAX];
    char *ptr;

    sprintf (tempFile, "%s/%sXXXXXX", path, prefix);
    ptr = (char *)malloc(strlen(tempFile) + 1);
    if (ptr != NULL)
	{
	    strcpy(ptr, tempFile);
	    *pFd =  mkstemp(ptr);
	}
    return ptr;
#endif
}

#define MAGIC_COOKIE_LEN 16

Status SetAuthentication_local (int count, IceListenObj *listenObjs)
{
    int i;
    for (i = 0; i < count; i ++) {
	char *prot = IceGetListenConnectionString(listenObjs[i]);
	if (!prot) continue;
	char *host = strchr(prot, '/');
	char *sock = 0;
	if (host) {
	    *host=0;
	    host++;
	    sock = strchr(host, ':');
	    if (sock) {
	        *sock = 0;
		sock++;
	    }
	}
	kdDebug() << "KSMServer: SetAProc_loc: conn " << (unsigned)i << ", prot=" << prot << ", file=" << sock << endl;
	if (sock && !strcmp(prot, "local")) {
	    chmod(sock, 0700);
	}
	IceSetHostBasedAuthProc (listenObjs[i], HostBasedAuthProc);
	free(prot);
    }
    return 1;
}

Status SetAuthentication (int count, IceListenObj *listenObjs,
			  IceAuthDataEntry **authDataEntries)
{
    FILE        *addfp = NULL;
    FILE        *removefp = NULL;
    const char  *path;
    int         original_umask;
    char        command[256];
    int         i;
#ifdef HAVE_MKSTEMP
    int         fd;
#endif

    original_umask = ::umask (0077);      /* disallow non-owner access */

    path = getenv ("KSM_SAVE_DIR");
    if (!path)
	path = "/tmp";
#ifndef HAVE_MKSTEMP
    if ((addAuthFile = unique_filename (path, "ksm")) == NULL)
	goto bad;

    if (!(addfp = fopen (addAuthFile, "w")))
	goto bad;

    if ((remAuthFile = unique_filename (path, "ksm")) == NULL)
	goto bad;

    if (!(removefp = fopen (remAuthFile, "w")))
	goto bad;
#else
    if ((addAuthFile = unique_filename (path, "ksm", &fd)) == NULL)
	goto bad;

    if (!(addfp = fdopen(fd, "wb")))
	goto bad;

    if ((remAuthFile = unique_filename (path, "ksm", &fd)) == NULL)
	goto bad;

    if (!(removefp = fdopen(fd, "wb")))
	goto bad;
#endif

    if ((*authDataEntries = (IceAuthDataEntry *) malloc (
			 count * 2 * sizeof (IceAuthDataEntry))) == NULL)
	goto bad;

    for (i = 0; i < numTransports * 2; i += 2) {
	(*authDataEntries)[i].network_id =
	    IceGetListenConnectionString (listenObjs[i/2]);
	(*authDataEntries)[i].protocol_name = (char *) "ICE";
	(*authDataEntries)[i].auth_name = (char *) "MIT-MAGIC-COOKIE-1";

	(*authDataEntries)[i].auth_data =
	    IceGenerateMagicCookie (MAGIC_COOKIE_LEN);
	(*authDataEntries)[i].auth_data_length = MAGIC_COOKIE_LEN;

	(*authDataEntries)[i+1].network_id =
	    IceGetListenConnectionString (listenObjs[i/2]);
	(*authDataEntries)[i+1].protocol_name = (char *) "XSMP";
	(*authDataEntries)[i+1].auth_name = (char *) "MIT-MAGIC-COOKIE-1";

	(*authDataEntries)[i+1].auth_data =
	    IceGenerateMagicCookie (MAGIC_COOKIE_LEN);
	(*authDataEntries)[i+1].auth_data_length = MAGIC_COOKIE_LEN;

	write_iceauth (addfp, removefp, &(*authDataEntries)[i]);
	write_iceauth (addfp, removefp, &(*authDataEntries)[i+1]);

	IceSetPaAuthData (2, &(*authDataEntries)[i]);

	IceSetHostBasedAuthProc (listenObjs[i/2], HostBasedAuthProc);
    }

    fclose (addfp);
    fclose (removefp);

    umask (original_umask);

    sprintf (command, "iceauth source %s", addAuthFile);
    system (command);

    unlink (addAuthFile);

    return (1);

 bad:

    if (addfp)
	fclose (addfp);

    if (removefp)
	fclose (removefp);

    if (addAuthFile) {
	unlink(addAuthFile);
	free(addAuthFile);
    }
    if (remAuthFile) {
	unlink(remAuthFile);
	free(remAuthFile);
    }

    return (0);
}

/*
 * Free up authentication data.
 */
void FreeAuthenticationData(int count, IceAuthDataEntry *authDataEntries)
{
    /* Each transport has entries for ICE and XSMP */

    char command[256];
    int i;

    if (only_local)
	return;

    for (i = 0; i < count * 2; i++) {
	free (authDataEntries[i].network_id);
	free (authDataEntries[i].auth_data);
    }

    free (authDataEntries);

    sprintf (command, "iceauth source %s", remAuthFile);
    system(command);

    unlink(remAuthFile);

    free(addAuthFile);
    free(remAuthFile);
}

static void sighandler(int sig)
{
    if (sig == SIGHUP) {
	signal(SIGHUP, sighandler);
	return;
    }

    if (the_server)
    {
       the_server->cleanUp();
       delete the_server;
       the_server = 0;
    }
    kapp->quit();
    //::exit(0);
}


void KSMWatchProc ( IceConn iceConn, IcePointer client_data, Bool opening, IcePointer* watch_data)
{
    KSMServer* ds = ( KSMServer*) client_data;

    if (opening) {
	*watch_data = (IcePointer) ds->watchConnection( iceConn );
    }
    else  {
	ds->removeConnection( (KSMConnection*) *watch_data );
    }
}

static Status KSMNewClientProc ( SmsConn conn, SmPointer manager_data,
				 unsigned long* mask_ret, SmsCallbacks* cb, char** failure_reason_ret)
{
    *failure_reason_ret = 0;

    void* client =  ((KSMServer*) manager_data )->newClient( conn );

    cb->register_client.callback = KSMRegisterClientProc;
    cb->register_client.manager_data = client;
    cb->interact_request.callback = KSMInteractRequestProc;
    cb->interact_request.manager_data = client;
    cb->interact_done.callback = KSMInteractDoneProc;
    cb->interact_done.manager_data = client;
    cb->save_yourself_request.callback = KSMSaveYourselfRequestProc;
    cb->save_yourself_request.manager_data = client;
    cb->save_yourself_phase2_request.callback = KSMSaveYourselfPhase2RequestProc;
    cb->save_yourself_phase2_request.manager_data = client;
    cb->save_yourself_done.callback = KSMSaveYourselfDoneProc;
    cb->save_yourself_done.manager_data = client;
    cb->close_connection.callback = KSMCloseConnectionProc;
    cb->close_connection.manager_data = client;
    cb->set_properties.callback = KSMSetPropertiesProc;
    cb->set_properties.manager_data = client;
    cb->delete_properties.callback = KSMDeletePropertiesProc;
    cb->delete_properties.manager_data = client;
    cb->get_properties.callback = KSMGetPropertiesProc;
    cb->get_properties.manager_data = client;

    *mask_ret = SmsRegisterClientProcMask |
		SmsInteractRequestProcMask |
		SmsInteractDoneProcMask |
		SmsSaveYourselfRequestProcMask |
		SmsSaveYourselfP2RequestProcMask |
		SmsSaveYourselfDoneProcMask |
		SmsCloseConnectionProcMask |
		SmsSetPropertiesProcMask |
		SmsDeletePropertiesProcMask |
		SmsGetPropertiesProcMask;
    return 1;
};


#ifdef HAVE__ICETRANSNOLISTEN
extern "C" int _IceTransNoListen(const char * protocol);
#endif

KSMServer::KSMServer( const QString& windowManager, bool _only_local )
  : DCOPObject("ksmserver2")
{
    the_server = this;
    clean = false;
    wm = windowManager;

    progress = 0;

    state = Idle;
    dialogActive = false;
    KConfig* config = KGlobal::config();
    config->setGroup("General" );
    clientInteracting = 0;

    only_local = _only_local;
#ifdef HAVE__ICETRANSNOLISTEN
    if (only_local)
	_IceTransNoListen("tcp");
#else
    only_local = false;
#endif

    launcher = KApplication::launcher();

    char 	errormsg[256];
    if (!SmsInitialize ( (char*) KSMVendorString, (char*) KSMReleaseString,
			 KSMNewClientProc,
			 (SmPointer) this,
			 HostBasedAuthProc, 256, errormsg ) ) {

	qWarning("KSMServer: could not register XSM protocol");
    }

    if (!IceListenForConnections (&numTransports, &listenObjs,
				  256, errormsg))
    {
	qWarning("KSMServer: Error listening for connections: %s", errormsg);
        qWarning("KSMServer: Aborting.");
        exit(1);
    }

    {
	// publish available transports.
	QCString fName = QFile::encodeName(locateLocal("socket", "KSMserver"));
        QCString display = ::getenv("DISPLAY");
	// strip the screen number from the display
	display.replace(QRegExp("\\.[0-9]+$"), "");

        fName += "-"+display;
	FILE *f;
	f = ::fopen(fName.data(), "w+");
	if (!f)
        {
            qWarning("KSMServer: can't open %s: %s", fName.data(), strerror(errno));
            qWarning("KSMServer: Aborting.");
            exit(1);
        }
	char* session_manager = IceComposeNetworkIdList(numTransports, listenObjs);
	fprintf(f, session_manager);
	fprintf(f, "\n%i\n", getpid());
	fclose(f);
	setenv( "SESSION_MANAGER", session_manager, TRUE  );
       // Pass env. var to kdeinit.
       QCString name = "SESSION_MANAGER";
       QCString value = session_manager;
       QByteArray params;
       QDataStream stream(params, IO_WriteOnly);
       stream << name << value;
       kapp->dcopClient()->send(launcher, launcher, "setLaunchEnv(QCString,QCString)", params);
    }

    if (only_local) {
	if (!SetAuthentication_local(numTransports, listenObjs))
	    qFatal("KSMSERVER: authentication setup failed.");
    } else {
	if (!SetAuthentication(numTransports, listenObjs, &authDataEntries))
	    qFatal("KSMSERVER: authentication setup failed.");
    }

    IceAddConnectionWatch (KSMWatchProc, (IcePointer) this);

    listener.setAutoDelete( TRUE );
    KSMListener* con;
    for ( int i = 0; i < numTransports; i++) {
	con = new KSMListener( listenObjs[i] );
	listener.append( con );
	connect( con, SIGNAL( activated(int) ), this, SLOT( newConnection(int) ) );
    }

    signal(SIGHUP, sighandler);
    signal(SIGTERM, sighandler);
    signal(SIGINT, sighandler);
    signal(SIGCHLD, the_reaper);
    signal(SIGPIPE, SIG_IGN);

    connect( &protection, SIGNAL( timeout() ), this, SLOT( protectionTimeout() ) );
    connect( kapp, SIGNAL( shutDown() ), this, SLOT( cleanUp() ) );

    KNotifyClient::event( "startkde" ); // this is the time KDE is up
}

KSMServer::~KSMServer()
{
    the_server = 0;
    cleanUp();
}

void KSMServer::cleanUp()
{
    if (clean) return;
    clean = true;
    IceFreeListenObjs (numTransports, listenObjs);

    QCString fName = QFile::encodeName(locateLocal("socket", "KSMserver"));
    QCString display = ::getenv("DISPLAY");
    // strip the screen number from the display
    display.replace(QRegExp("\\.[0-9]+$"), "");

    fName += "-"+display;
    ::unlink(fName.data());

    FreeAuthenticationData(numTransports, authDataEntries);
    signal(SIGTERM, SIG_DFL);
    signal(SIGINT, SIG_DFL);
    signal(SIGCHLD, SIG_DFL);
}



void* KSMServer::watchConnection( IceConn iceConn )
{
    KSMConnection* conn = new KSMConnection( iceConn );
    connect( conn, SIGNAL( activated(int) ), this, SLOT( processData(int) ) );
    return (void*) conn;
}

void KSMServer::removeConnection( KSMConnection* conn )
{
    delete conn;
}


/*!
  Called from our IceIoErrorHandler
 */
void KSMServer::ioError( IceConn /* iceConn */ )
{
}

void KSMServer::processData( int /*socket*/ )
{
    IceConn iceConn = ((KSMConnection*)sender())->iceConn;
    IceProcessMessagesStatus status = IceProcessMessages( iceConn, 0, 0 );
    if ( status == IceProcessMessagesIOError ) {
	IceSetShutdownNegotiation( iceConn, False );
	QListIterator<KSMClient> it ( clients );
	while ( it.current() &&SmsGetIceConnection( it.current()->connection() ) != iceConn )
	    ++it;

	if ( it.current() ) {
	    SmsConn smsConn = it.current()->connection();
	    deleteClient( it.current() );
	    SmsCleanUp( smsConn );
	}
        // TODO: trinity believes this is a double free, and moved it into the above if block.
        // Should probably NULL it out after freeing and add a NULL check everywhere.
	(void) IceCloseConnection( iceConn );
    }
}

KSMClient* KSMServer::newClient( SmsConn conn )
{
    KSMClient* client = new KSMClient( conn );
    clients.append( client );
    if ( progress ) {
	progress--;
	publishProgress( progress );
	if ( progress == 0 )
	    upAndRunning( "session ready" );
    }
    return client;
}

void KSMServer::deleteClient( KSMClient* client )
{
    if ( clients.findRef( client ) == -1 ) // paranoia
	return;
    clients.removeRef( client );
    if ( client == clientInteracting ) {
	clientInteracting = 0;
	handlePendingInteractions();
    }
    delete client;
    if ( state == Shutdown )
	completeShutdown();
    if ( state == Killing )
	completeKilling();
}

void KSMServer::newConnection( int /*socket*/ )
{
    IceAcceptStatus status;
    IceConn iceConn = IceAcceptConnection( ((KSMListener*)sender())->listenObj, &status);
    IceSetShutdownNegotiation( iceConn, False );
    IceConnectStatus cstatus;
    while ((cstatus = IceConnectionStatus (iceConn))==IceConnectPending) {
	(void) IceProcessMessages( iceConn, 0, 0 );
    }

    if (cstatus != IceConnectAccepted) {
	if (cstatus == IceConnectIOError)
	    qWarning ("IO error opening ICE Connection!\n");
	else
	    qWarning ("ICE Connection rejected!\n");
	(void )IceCloseConnection (iceConn);
    }
}


void KSMServer::shutdown()
    { shutdown( false ); }

void KSMServer::shutdown( bool bFast )
{
    if ( state != Idle )
	return;
    if ( dialogActive )
        return;
    dialogActive = true;

    // don't use KGlobal::config here! config may have changed!
    KConfig *cfg = new KConfig("ksmserverrc", false, false);
    cfg->setGroup("General" );
    bool old_saveSession = saveSession =
	cfg->readBoolEntry( "saveSession", FALSE );
    bool confirmLogout = !bFast && cfg->readBoolEntry( "confirmLogout", TRUE );
    delete cfg;
 
    if ( confirmLogout ) {
        KSMShutdownFeedback::start(); // make the screen gray
        connect( KSMShutdownFeedback::self(), SIGNAL( aborted() ), SLOT( cancelShutdown() ) );
    }

    if ( !confirmLogout || KSMShutdownDlg::confirmShutdown( saveSession ) ) {
	// Set the real desktop background to black so that exit looks
	// clean regardless of what was on "our" desktop.
	kapp->desktop()->setBackgroundColor( Qt::black );
	KNotifyClient::event( "exitkde" ); // KDE says good bye
	if (saveSession != old_saveSession) {
	    KConfig* config = KGlobal::config();
	    config->setGroup("General" );
	    config->writeEntry( "saveSession", saveSession);
	}
	if ( saveSession )
	    discardSession();
	state = Shutdown;
	startProtection();
	for ( KSMClient* c = clients.first(); c; c = clients.next() ) {
	    c->resetState();
	    SmsSaveYourself( c->connection(), saveSession?SmSaveBoth: SmSaveGlobal,
			     TRUE, SmInteractStyleAny, FALSE );
	}
	if ( clients.isEmpty() )
	    completeShutdown();
    }
    // ###### We can't make the screen remain gray while talking to the apps,
    // because this prevents interaction ("do you want to save", etc.)
    // TODO: turn the feedback widget into a list of apps to be closed,
    // with an indicator of the current status for each.
    //else
        KSMShutdownFeedback::stop(); // so that the screen becomes normal again
    dialogActive = false;
}


// callbacks
void KSMServer::saveYourselfDone( KSMClient* client, bool success )
{
    if ( state == Idle )
	return;
    if ( success ) {
	client->saveYourselfDone = TRUE;
	completeShutdown();
    } else {
	// fake success to make KDE's logout not block with broken
	// apps. A perfect ksmserver would display a warning box at
	// the very end.
	client->saveYourselfDone = TRUE;
	completeShutdown();
    }
    startProtection();
}

void KSMServer::interactRequest( KSMClient* client, int /*dialogType*/ )
{
    if ( state == Shutdown )
	client->pendingInteraction = TRUE;
    else
	SmsInteract( client->connection() );

    handlePendingInteractions();

}

void KSMServer::interactDone( KSMClient* client, bool cancelShutdown_ )
{
    if ( client != clientInteracting )
	return; // should not happen
    clientInteracting = 0;
    if ( cancelShutdown_ )
	cancelShutdown();
    else
	handlePendingInteractions();
}


void KSMServer::phase2Request( KSMClient* client )
{
    client->waitForPhase2 = TRUE;
    client->wasPhase2 = TRUE;
    completeShutdown();
}

void KSMServer::handlePendingInteractions()
{
    if ( clientInteracting )
	return;

    for ( KSMClient* c = clients.first(); c; c = clients.next() ) {
	if ( c->pendingInteraction ) {
	    clientInteracting = c;
	    c->pendingInteraction = FALSE;
	    break;
	}
    }
    if ( clientInteracting ) {
	endProtection();
	SmsInteract( clientInteracting->connection() );
    } else {
	startProtection();
    }
}


void KSMServer::cancelShutdown()
{
    kdDebug() << "cancelShutdown!" << endl;
    clientInteracting = 0;
    for ( KSMClient* c = clients.first(); c; c = clients.next() )
 	SmsShutdownCancelled( c->connection() );
    state = Idle;
    KSMShutdownFeedback::stop(); // so that the screen becomes normal again
}

/*
   Internal protection slot, invoked when clients do not react during
  shutdown.
 */
void KSMServer::protectionTimeout()
{
    endProtection();
    if ( state != Shutdown || clientInteracting )
	return;

    for ( KSMClient* c = clients.first(); c; c = clients.next() ) {
	if ( !c->saveYourselfDone && !c->waitForPhase2 )
	    c->saveYourselfDone = TRUE;
    }
    completeShutdown();
    startProtection();
}

void KSMServer::startProtection()
{
    protection.start( 8000 );
}

void KSMServer::endProtection()
{
    protection.stop();
}


void KSMServer::completeShutdown()
{
    if ( state != Shutdown )
	return;

    for ( KSMClient* c = clients.first(); c; c = clients.next() ) {
	if ( !c->saveYourselfDone && !c->waitForPhase2 )
	    return; // not done yet
    }

    // do phase 2
    bool waitForPhase2 = FALSE;
    for ( KSMClient* c = clients.first(); c; c = clients.next() ) {
	if ( !c->saveYourselfDone && c->waitForPhase2 ) {
	    c->waitForPhase2 = FALSE;
	    SmsSaveYourselfPhase2( c->connection() );
	    waitForPhase2 = TRUE;
	}
    }
    if ( waitForPhase2 )
	return;

    if ( saveSession )
	storeSesssion();

    // kill all clients
    state = Killing;
    for ( KSMClient* c = clients.first(); c; c = clients.next() ) {
        kdDebug() << "completeShutdown: client " << c->program() << endl;
	if (c->wasPhase2)
	    continue;
       	SmsDie( c->connection() );
    }
    kdDebug() << " We killed all clients. We have now clients.count()=" <<
	clients.count() << endl;

    completeKilling();
    QTimer::singleShot( 4000, this, SLOT( timeoutQuit() ) );
}

void KSMServer::completeKilling()
{
    kdDebug(0) << "KSMServer::completeKilling clients.count()=" <<
	clients.count() << endl;
    if ( state != Killing ) {
// 	kdWarning() << "Not Killing !!! state=" << state << endl;
	return;
    }

    if ( clients.isEmpty() ) {
	kdDebug(0) << "Calling qApp->quit()" << endl;
	qApp->quit();
    } else {
	for (KSMClient *c = clients.first(); c; c = clients.next()) {
	    if (! c->wasPhase2)
		return;
	}
	// the wm was not killed yet, do it
	for (KSMClient *c = clients.first(); c; c = clients.next())
	    SmsDie( c->connection() );
    }
}

void KSMServer::timeoutQuit()
{
    qApp->quit();
}

void KSMServer::discardSession()
{
    KConfig* config = KGlobal::config();
    config->setGroup("Session" );
    int count =  config->readNumEntry( "count" );
    for ( int i = 1; i <= count; i++ ) {
	QString n = QString::number(i);
	executeCommand( config->readListEntry( QString("discardCommand")+n ) );
    }
    config->writeEntry( "count", 0 );
}

void KSMServer::storeSesssion()
{
    KConfig* config = KGlobal::config();
    config->setGroup("Session" );
    int count =  0;
    for ( KSMClient* c = clients.first(); c; c = clients.next() ) {
        int restartHint = c->restartStyleHint();
        if (restartHint == SmRestartNever)
           continue;
        QString program = c->program();
        QStringList restartCommand = c->restartCommand();
        if (program.isEmpty() && restartCommand.isEmpty())
           continue;

        count++;
        QString n = QString::number(count);
        config->writeEntry( QString("program")+n, program );
        config->writeEntry( QString("restartCommand")+n, restartCommand );
        config->writeEntry( QString("discardCommand")+n, c->discardCommand() );
        config->writeEntry( QString("restartStyleHint")+n, restartHint );
        config->writeEntry( QString("userId")+n, c->userId() );
    }
    config->writeEntry( "count", count );

    config->setGroup("General");
    config->writeEntry( "screenCount", ScreenCount(qt_xdisplay()));

    config->sync();
}


/*!  Restores the previous session. Ensures the window manager is
  running (if specified).
 */
void KSMServer::restoreSession()
{
    kdDebug(0) << "KSMServer::restoreSession" << endl;
    upAndRunning( "restore session");
    KConfig* config = KGlobal::config();
    config->setGroup("Session" );
    int count =  config->readNumEntry( "count" );
    appsToStart = count;

    QStringList wmCommand;
    if ( !wm.isEmpty() ) {
	for ( int i = 1; i <= count; i++ ) {
	    QString n = QString::number(i);
	    if ( wm == config->readEntry( QString("program")+n ) ) {
		appsToStart--;
		wmCommand << config->readEntry( QString("restartCommand")+n );
	    }
	}
    }
    if ( wmCommand.isEmpty() )
	wmCommand << wm;

    publishProgress( appsToStart, true );

    connectDCOPSignal( "klauncher", "klauncher", "autoStartDone()",
                       "restoreSessionInternal()", true);

    if ( !wmCommand.isEmpty() ) {
	// when we have a window manager, we start it first and give
	// it some time before launching other processes. Results in a
	// visually more appealing startup.
        for (uint i = 0; i < wmCommand.count(); i++)
	    startApplication( QStringList::split(',', wmCommand[i]) );
	QTimer::singleShot( 4000, this, SLOT( autoStart() ) );
    } else {
	autoStart();
    }
}

/*!
  Starts the default session.

  Currently, that's the window manager only (if specified).
 */
void KSMServer::startDefaultSession()
{
    upAndRunning( "start session" );
    progress = 1;
    publishProgress( progress, true );
    startApplication( wm );
    QTimer::singleShot( 4000, this, SLOT( autoStart() ) );
}

bool KSMServer::process(const QCString &fun, const QByteArray &data,
                        QCString& replyType, QByteArray &replyData)
{
    if (fun == "restoreSessionInternal()")
    {
       restoreSessionInternal();
       replyType = "void";
       return true;
    }
    return DCOPObject::process(fun, data, replyType, replyData);
}

void KSMServer::autoStart()
{
    static bool beenThereDoneThat = FALSE;
    if ( beenThereDoneThat )
	return;
    beenThereDoneThat = TRUE;

    kapp->dcopClient()->send("klauncher", "klauncher", "autoStart()", QByteArray());
}


void KSMServer::clientSetProgram( KSMClient* client )
{
    if ( !wm.isEmpty() && client->program() == wm )
	autoStart();
}


void KSMServer::restoreSessionInternal()
{
    disconnectDCOPSignal( "klauncher", "klauncher", "autoStartDone()",
                          "restoreSessionInternal()");
    progress = appsToStart;
    KConfig* config = KGlobal::config();
    config->setGroup("Session" );
    int count =  config->readNumEntry( "count" );
    for ( int i = 1; i <= count; i++ ) {
	QString n = QString::number(i);
	QStringList restartCommand = config->readListEntry( QString("restartCommand")+n );
	if ( restartCommand.isEmpty() ||
	     (config->readNumEntry( QString("restartStyleHint")+n ) == SmRestartNever)) {
	    progress--;
	    continue;
	}
	if ( wm == config->readEntry( QString("program")+n ) )
	    continue;

	startApplication( restartCommand );
    }
    if (progress == 0) {
	publishProgress( progress );
	upAndRunning( "session ready" );
    }
}

void KSMServer::publishProgress( int progress, bool max  )
{
    QByteArray data;
    QDataStream arg(data, IO_WriteOnly);
    arg << progress;
    kapp->dcopClient()->send("ksplash", "", max ? "setMaxProgress(int)" : "setProgress(int)", data );
}


void KSMServer::upAndRunning( const QString& msg )
{
    kapp->dcopClient()->send( "ksplash", "", "upAndRunning(QString)", msg );
}
