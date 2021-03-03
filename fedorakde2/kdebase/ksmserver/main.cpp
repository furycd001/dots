/*****************************************************************
ksmserver - the KDE session management server

Copyright (C) 2000 Matthias Ettrich <ettrich@kde.org>
******************************************************************/

#include <config.h>

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#undef Unsorted // for --enable-final

#include <qmessagebox.h>
#include <qdir.h>

#include <dcopclient.h>

#include <kapp.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include "server.h"

static const char *version = "0.4";
static const char *description = I18N_NOOP( "The reliable KDE session manager that talks the standard X11R6 \nsession management protocol (XSMP)." );

static const KCmdLineOptions options[] =
{
   { "r", 0, 0 },
   { "restore", I18N_NOOP("Restores the previous session if available"), 0},
   { "w", 0, 0 },
   { "windowmanager <wm>", I18N_NOOP("Starts 'wm' in case no other window manager is \nparticipating in the session. Default is 'kwin'"), 0},
   { "nolocal", I18N_NOOP("Also allow remote connections."), 0},
   { 0, 0, 0 }
};

extern KSMServer* the_server;

void IoErrorHandler ( IceConn iceConn)
{
    the_server->ioError( iceConn );
}

bool writeTest(QCString path)
{
   path += "/XXXXXX";
   int fd = mkstemp(path.data());
   if (fd == -1)
      return false;
   if (write(fd, "Hello World\n", 12) == -1)
   {
      int save_errno = errno;
      close(fd);
      unlink(path.data());
      errno = save_errno;
      return false;
   }
   close(fd);
   unlink(path.data());
   return true;
}

void sanity_check( int argc, char* argv[] )
{
  QCString msg;
  QCString path = getenv("HOME");
  if (path.isEmpty())
  {
     msg = "$HOME not set!";
  }
  if (msg.isEmpty() && access(path.data(), W_OK))
  {
     if (errno == ENOENT)
        msg = "$HOME directory (%s) does not exist.";
     else
        msg = "No write access to $HOME directory (%s).";
  }
  if (msg.isEmpty() && access(path.data(), R_OK))
  {
     if (errno == ENOENT)
        msg = "$HOME directory (%s) does not exist.";
     else
        msg = "No read access to $HOME directory (%s).";
  }
  if (msg.isEmpty() && !writeTest(path))
  {
     if (errno == ENOSPC)
        msg = "$HOME directory (%s) is out of disk space.";
     else
        msg = "Writing to the $HOME directory (%s) failed with\n    "
              "the error '"+QCString(strerror(errno))+"'";
  }
  if (msg.isEmpty())
  {
     path = IceAuthFileName();
 
     if (access(path.data(), W_OK) && (errno != ENOENT))
        msg = "No write access to '%s'.";
     else if (access(path.data(), R_OK) && (errno != ENOENT))
        msg = "No read access to '%s'.";
  }
  if (msg.isEmpty())
  {
     path = getenv("KDETMP");
     if (path.isEmpty()) 
        path = "/tmp";
     if (!writeTest(path))
     {
        if (errno == ENOSPC)
           msg = "Temp directory (%s) is out of disk space.";
        else
           msg = "Writing to the temp directory (%s) failed with\n    "
                 "the error '"+QCString(strerror(errno))+"'";
     }
  }
  if (msg.isEmpty() && (path != "/tmp"))
  {
     path = "/tmp";
     if (!writeTest(path))
     {
        if (errno == ENOSPC)
           msg = "Temp directory (%s) is out of disk space.";
        else
           msg = "Writing to the temp directory (%s) failed with\n    "
                 "the error '"+QCString(strerror(errno))+"'";
     }
  }
  if (msg.isEmpty())
  {
     path += ".ICE-unix";
     if (access(path.data(), W_OK) && (errno != ENOENT))
        msg = "No write access to '%s'.";
     else if (access(path.data(), R_OK) && (errno != ENOENT))
        msg = "No read access to '%s'.";
  }
  if (!msg.isEmpty())
  {
    const char *msg_pre = 
             "The following installation problem was detected\n"
             "while trying to start KDE:"
             "\n\n    ";
    const char *msg_post = "\n\nKDE is unable to start.\n";
    fprintf(stderr, msg_pre);
    fprintf(stderr, msg.data(), path.data());
    fprintf(stderr, msg_post);
 
    QApplication a(argc, argv);
    QCString qmsg(256+path.length());
    qmsg.sprintf(msg.data(), path.data());
    qmsg = msg_pre+qmsg+msg_post; 
    QMessageBox::critical(0, "KDE Installation Problem!",
        QString::fromLatin1(qmsg.data()));
    exit(255);
  }
}

int main( int argc, char* argv[] )
{
    sanity_check(argc, argv);

    KAboutData aboutData( "ksmserver2", I18N_NOOP("The KDE Session Manager"),
       version, description, KAboutData::License_BSD,
       "(C) 2000, The KDE Developers");
    aboutData.addAuthor("Matthias Ettrich",0, "ettrich@kde.org");

    KCmdLineArgs::init(argc, argv, &aboutData);
    KCmdLineArgs::addCmdLineOptions( options );

    putenv((char*)"SESSION_MANAGER=");
    KApplication a(false, true); // Disable styles until we need them.
    fcntl(ConnectionNumber(qt_xdisplay()), F_SETFD, 1);

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    kapp->dcopClient()->registerAs("ksmserver2", false);
    if (!kapp->dcopClient()->isRegistered())
    {
       qWarning("Could not register with DCOPServer. Aborting.");
       return 1;
    }

    QCString wm = args->getOption("windowmanager");
    if ( wm.isEmpty() )
	wm = "kwin";

    bool only_local = args->isSet("local");
#ifndef HAVE__ICETRANSNOLISTEN
    /* this seems strange, but the default is only_local, so if !only_local
     * the option --nolocal was given, and we warn (the option --nolocal
     * does nothing on this platform, as here the default is reversed)
     */
    if (!only_local) {
        qWarning("--[no]local is not supported on your platform. Sorry.");
    }
    only_local = false;
#endif

    KSMServer *server = new KSMServer( QString::fromLatin1(wm), only_local);
    IceSetIOErrorHandler (IoErrorHandler );

    KConfig *config = KGlobal::config();

    bool screenCountChanged =
         (config->readNumEntry("screenCount") != ScreenCount(qt_xdisplay()));

    if ( args->isSet("restore") && ! screenCountChanged )
	server->restoreSession();
    else
	server->startDefaultSession();

    return a.exec();
}

