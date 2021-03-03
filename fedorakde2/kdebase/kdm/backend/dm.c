/* $TOG: dm.c /main/73 1998/04/09 15:12:03 barstow $ */
/* $Id: dm.c,v 1.26.2.2 2001/10/03 10:47:11 ossi Exp $ */
/*

Copyright 1988, 1998  The Open Group

All Rights Reserved.

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from The Open Group.

*/
/* $XFree86: xc/programs/xdm/dm.c,v 3.10 2000/04/27 16:26:50 eich Exp $ */

/*
 * xdm - display manager daemon
 * Author:  Keith Packard, MIT X Consortium
 *
 * display manager
 */

#include "dm.h"
#include "dm_auth.h"
#include "dm_error.h"

#include <stdio.h>
#include <string.h>
#ifdef X_POSIX_C_SOURCE
# define _POSIX_C_SOURCE X_POSIX_C_SOURCE
# include <signal.h>
# undef _POSIX_C_SOURCE
#else
# if defined(X_NOT_POSIX) || defined(_POSIX_SOURCE)
#  include <signal.h>
# else
#  define _POSIX_SOURCE
#  include <signal.h>
#  undef _POSIX_SOURCE
# endif
#endif
#ifdef __NetBSD__
# include <sys/param.h>
#endif

#ifndef sigmask
# define sigmask(m)  (1 << ((m - 1)))
#endif

#include <sys/stat.h>
#include <errno.h>
#include <X11/Xfuncproto.h>
#include <stdarg.h>

#ifndef X_NOT_POSIX
# include <unistd.h>
#endif

#ifndef PATH_MAX
# ifdef WIN32
#  define PATH_MAX 512
# else
#  include <sys/param.h>
# endif
# ifndef PATH_MAX
#  ifdef MAXPATHLEN
#   define PATH_MAX MAXPATHLEN
#  else
#   define PATH_MAX 1024
#  endif
# endif
#endif

#if defined(CSRG_BASED) || !defined(sun)
# include <utmp.h>
# ifndef UTMP_FILE
#  define UTMP_FILE _PATH_UTMP
# endif
# define LOGSTAT_FILE UTMP_FILE
#else
# include <utmpx.h>
# ifndef ut_time
#  define ut_time ut_tv.tv_sec
# endif
# ifndef UTMPX_FILE
#  define UTMPX_FILE _PATH_UTMPX
# endif
# define LOGSTAT_FILE UTMPX_FILE
#endif
#ifdef linux
# include <sys/ioctl.h>
# include <linux/vt.h>
#endif

#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

#if defined(SVR4) && !defined(SCO)
extern FILE    *fdopen();
#endif

static SIGVAL	StopAll (int n), UtmpNotify (int n), RescanNotify (int n);
static void	RescanConfigs (void);
static void	RestartDisplay (struct display *d, int forceReserver);
static void	ScanServers (void);
static void	SetAccessFileTime (void);	/* XXX kill! */
static void	StartDisplays (void);
static void	ExitDisplay (struct display *d, int doRestart, int forceReserver, int goodExit);
static int	CheckUtmp (void);
static void	SwitchToTty (struct display *d);

int	Rescan;
int	ChkUtmp;
long	ServersModTime, AccessFileModTime;	/* XXX kill! */

#define nofork_session (debugLevel & DEBUG_NOFORK)

#ifndef NOXDMTITLE
static char *Title;
static int TitleLen;
#endif

#ifndef UNRELIABLE_SIGNALS
static SIGVAL ChildNotify (int n);
#endif

static int StorePid (void);


#define A_NONE	 0
#define A_HALT	 1
#define A_REBOOT 2
static int action;

char *prog, *progpath;

int
main (int argc, char **argv)
{
    int	oldpid, oldumask;
    char *pt, *errorLogFile;

    /* make sure at least world write access is disabled */
    if (((oldumask = umask(022)) & 002) == 002)
	(void) umask (oldumask);

    /* give /dev/null as stdin */
    (void) close (0);
    open ("/dev/null", O_RDONLY);

    if (argv[0][0] == '/')
	StrDup (&progpath, argv[0]);
    else
#if 0
	Panic ("Must be invoked with full path specification\n");
#else
    {
	char directory[PATH_MAX+1];
# if !defined(X_NOT_POSIX) || defined(SYSV) || defined(WIN32)
        if (!getcwd(directory, PATH_MAX + 1))
	    Panic ("Can't find myself (getcwd failed)\n");
# else
        if (!getwd(directory))
	    Panic ("Can't find myself (getwd failed)\n");
# endif
	if (strchr(argv[0], '/'))
	    StrApp (&progpath, directory, "/", argv[0], (char *)0);
	else
	{
	    int len;
	    char *path, *pathe, *name, *thenam, nambuf[PATH_MAX+1];

	    if (!(path = getenv ("PATH")))
		Panic ("Can't find myself (no PATH)\n");
	    len = strlen (argv[0]);
	    name = nambuf + PATH_MAX - len;
	    memcpy (name, argv[0], len + 1);
	    *--name = '/';
	    do {
		if (!(pathe = strchr (path, ':')))
		    pathe = path + strlen (path);
		len = pathe - path;
		if (!len || (len == 1 && *path == '.'))
		{
		    len = strlen (directory);
		    path = directory;
		}
		thenam = name - len;
		if (thenam >= nambuf)
		{
		    memcpy (thenam, path, len);
		    if (!access (thenam, X_OK))
			goto found;
		}
		path = pathe;
	    } while (*path++ != '\0');
	    Panic ("Can't find myself (not in PATH)\n");
	  found:
	    if (!StrDup (&progpath, thenam))
		Panic ("Out of memory\n");
	}
    }
#endif
    prog = strrchr(progpath, '/') + 1;

#ifndef NOXDMTITLE
    Title = argv[0];
    TitleLen = (argv[argc - 1] + strlen(argv[argc - 1])) - Title;
#endif

    /*
     * Parse basic command line options
     */
    for (argv++, errorLogFile = 0; argv[0] && argv[1]; argv += 2) {
	if (*argv[0] != '-')
	    break;
	pt = argv[0] + 1;
	if (*pt == '-')
	    pt++;
	if (!strcmp (pt, "debug"))
	    sscanf (argv[1], "%i", &debugLevel);
	else if (!strcmp (pt, "error") || !strcmp (pt, "logfile"))
	    errorLogFile = argv[1];
	else
	    break;
    }
    InitErrorLog (errorLogFile);

    /*
     * Only allow root to run in non-debug mode to avoid problems
     */
    if (!debugLevel && getuid())
    {
	fprintf (stderr, "Only root wants to run %s\n", prog);
	exit (1);
    }

    /*
     * Step 1 - load configuration parameters
     */
    if (!InitResources (argv) || !LoadDMResources (TRUE))
	LogPanic ("Config reader failed. Aborting ...\n");

    if (daemonMode)
	BecomeDaemon ();

    /* SUPPRESS 560 */
    if ((oldpid = StorePid ()))
    {
	if (oldpid == -1)
	    LogError ("Can't create/lock pid file %s\n", pidFile);
	else
	    LogError ("Can't lock pid file %s, another xdm is running (pid %d)\n",
		 pidFile, oldpid);
	exit (1);
    }

    if (!nofork_session) {
	/* Clean up any old Authorization files */
	char cmdbuf[1024];
#ifndef MINIX
	sprintf(cmdbuf, "/bin/rm -f %s/authfiles/A*", authDir);
#else
	sprintf(cmdbuf, "/usr/bin/rm -f %s/authfiles/A*", authDir);
#endif
	system(cmdbuf);
    }

#ifdef XDMCP
    init_session_id ();
    CreateWellKnownSockets ();
#else
    Debug ("not compiled for XDMCP\n");
#endif
    (void) Signal (SIGTERM, StopAll);
    (void) Signal (SIGINT, StopAll);

    /*
     * Step 2 - run a sub-daemon for each entry
     */
    SetAccessFileTime ();
#ifdef XDMCP
    ScanAccessDatabase ();
#endif
    ScanServers ();
    StartDisplays ();
    (void) Signal (SIGHUP, RescanNotify);
#ifndef UNRELIABLE_SIGNALS
    (void) Signal (SIGCHLD, ChildNotify);
#endif
    while (
#ifdef XDMCP
	   AnyWellKnownSockets() ||
#endif
	   AnyDisplaysLeft ())
    {
	if (Rescan)
	{
	    RescanConfigs ();
	    Rescan = 0;
	}
	if (ChkUtmp)
	{
	    CheckUtmp ();
	    ChkUtmp = 0;
	}
#if defined(UNRELIABLE_SIGNALS) || !defined(XDMCP)
	WaitForChild ();
#else
	WaitForSomething ();
#endif
    }
    if (action != A_NONE)
    {
	if (Fork() <= 0)
	{
	    char *cmd = action == A_HALT ? cmdHalt : cmdReboot;
	    execute (parseArgs ((char **)0, cmd), (char **)0);
	    LogError ("Failed to execute shutdown command '%s'\n", cmd);
	    exit (1);
	} else {
#ifndef X_NOT_POSIX
	    sigset_t mask;
	    sigemptyset(&mask);
	    sigaddset(&mask, SIGCHLD);
	    sigaddset(&mask, SIGHUP);
	    sigaddset(&mask, SIGALRM);
	    sigsuspend(&mask);
#else
	    sigpause (sigmask (SIGCHLD) | sigmask (SIGHUP) | sigmask (SIGALRM));
#endif
	}
    }
    Debug ("Nothing left to do, exiting\n");
    return 0;
}


/* ARGSUSED */
static SIGVAL
UtmpNotify (int n ATTR_UNUSED)
{
    Debug ("Caught SIGALRM\n");
    ChkUtmp = 1;
}

enum utState { UtWait, UtActive };

#ifndef UT_LINESIZE
#define UT_LINESIZE 32
#endif

struct utmps {
    struct utmps *next;
    struct display *d;
    char line[UT_LINESIZE];
    time_t time;
    enum utState state;
    int hadSess;
#ifdef CSRG_BASED
    int checked;
#endif
};

#define TIME_LOG 40
#define TIME_RELOG 10

static struct utmps *utmpList;

static int
CheckUtmp (void)
{
    static time_t modtim;
    int nck, nextChk;
    time_t now;
    struct utmps *utp, **utpp;
    struct stat st;
#ifdef CSRG_BASED
    struct utmp ut[1];
#elif defined(sun)
    struct utmpx *ut;
#else
    struct utmp *ut;
#endif

    if (stat(LOGSTAT_FILE, &st))
    {
	LogError (LOGSTAT_FILE " not found - cannot use console mode\n");
	return 0;
    }
    if (!utmpList)
	return 1;
    time(&now);
    if (modtim != st.st_mtime)
    {
#ifdef CSRG_BASED
	int fd;
#endif

	Debug ("Rescanning " LOGSTAT_FILE "\n");
#ifdef CSRG_BASED
	for (utp = utmpList; utp; utp = utp->next)
	    utp->checked = 0;
	if ((fd = open (UTMP_FILE, O_RDONLY)) < 0)
	{
	    LogError ("Cannot open " UTMP_FILE " - cannot use console mode\n");
	    return 0;
	}
	while (Reader (fd, ut, sizeof(ut[0])) == sizeof(ut[0]))
#elif defined(sun)
	setutxent();
	while ((ut = getutxent()))
#else
	setutent();
	while ((ut = getutent()))
#endif
	{
	    for (utp = utmpList; utp; utp = utp->next)
		if (!strncmp(utp->line, ut->ut_line, UT_LINESIZE))
		{
#ifdef CSRG_BASED
		    utp->checked = 1;
#else
		    if (ut->ut_type == LOGIN_PROCESS)
		    {
			Debug ("utmp entry for %s marked waiting\n", utp->line);
			utp->state = UtWait;
		    }
		    else if (ut->ut_type != USER_PROCESS)
			break;
		    else
#endif
		    {
			utp->hadSess = 1;
			Debug ("utmp entry for %s marked active\n", utp->line);
			utp->state = UtActive;
		    }
		    if (utp->time < ut->ut_time)
			utp->time = ut->ut_time;
		    break;
		}
	}
#ifdef CSRG_BASED
	close (fd);
	for (utp = utmpList; utp; utp = utp->next)
	    if (!utp->checked && utp->state == UtActive)
	    {
		utp->state = UtWait;
		utp->time = now;
		Debug ("utmp entry for %s marked waiting\n", utp->line);
	    }
#elif defined(sun)
	endutxent();
#else
	endutent();
#endif
	modtim = st.st_mtime;
    }
    nextChk = 1000;
    for (utpp = &utmpList; (utp = *utpp); )
    {
	if (utp->state == UtWait)
	{
	    time_t remains = utp->time + (utp->hadSess ? TIME_RELOG : TIME_LOG) 
			     - now;
	    if (remains <= 0)
	    {
		struct display *d = utp->d;
		Debug ("console login for %s at %s timed out\n", 
		       utp->d->name, utp->line);
		*utpp = utp->next;
		free (utp);
		ExitDisplay (d, TRUE, TRUE, TRUE);
		StartDisplays ();
		continue;
	    }
	    else
		nck = remains;
	}
	else
#ifdef CSRG_BASED
	    nck = (TIME_RELOG + 5) / 3;
#else
	    nck = TIME_RELOG;
#endif
	if (nck < nextChk)
	    nextChk = nck;
	utpp = &(*utpp)->next;
    }
    if (nextChk < 1000)
    {
	Signal (SIGALRM, UtmpNotify);
	alarm (nextChk);
    }
    return 1;
}

static void
SwitchToTty (struct display *d)
{
    struct utmps *utp;

    if (!d->console || !d->console[0])
    {
	LogError("No console for %s specified - cannot use console mode\n", d->name);
	d->status = notRunning;
	return;
    }
    if (!(utp = malloc (sizeof(*utp))))
    {
	LogOutOfMem("SwitchToTty");
	d->status = notRunning;
	return;
    }
    strncpy (utp->line, d->console, UT_LINESIZE);
    utp->d = d;
    utp->time = time(0);
    utp->hadSess = 0;
    utp->next = utmpList;
    utmpList = utp;
#ifdef linux	/* chvt */
    if (!memcmp(d->console, "tty", 3))
    {
	int con = open ("/dev/console", O_RDONLY);
	if (con >= 0)
	{
	    ioctl (con, VT_ACTIVATE, atoi (d->console + 3));
	    close (con);
	}
    }
#endif
    if (!CheckUtmp ())
    {
	utmpList = utp->next;
	free (utmpList);
	d->status = notRunning;
	return;
    }
}

/* ARGSUSED */
static SIGVAL
RescanNotify (int n ATTR_UNUSED)
{
    Debug ("Caught SIGHUP\n");
    Rescan = 1;
#ifdef SIGNALS_RESET_WHEN_CAUGHT
    (void) Signal (SIGHUP, RescanNotify);
#endif
}

static void
ScanServers (void)
{
    char	lineBuf[10240];
    FILE	*serversFile;
    struct stat	statb;

    if (servers[0] == '/')
    {
	serversFile = fopen (servers, "r");
	if (serversFile == NULL)
 	{
	    LogError ("cannot access servers file %s\n", servers);
	    return;
	}
	if (ServersModTime == 0)
	{
	    fstat (fileno (serversFile), &statb);
	    ServersModTime = statb.st_mtime;
	}
	while (fgets (lineBuf, sizeof (lineBuf)-1, serversFile))
	    ParseDisplay (lineBuf);
	fclose (serversFile);
    }
    else
    {
	ParseDisplay (servers);
    }
}

static void
MarkDisplay (struct display *d)
{
    d->stillThere = 0;
}

static void
RescanConfigs (void)
{
    Debug ("rescanning servers\n");
    LogInfo ("Rescanning both config and servers files\n");
    LoadDMResources (TRUE);
    ForEachDisplay (MarkDisplay);
    ScanServers ();
    SetAccessFileTime ();
#ifdef XDMCP
    ScanAccessDatabase ();
#endif
    StartDisplays ();
}

static void
SetAccessFileTime (void)
{
    struct stat	statb;

    if (stat (accessFile, &statb) != -1)
	AccessFileModTime = statb.st_mtime;
}

static void
RescanIfMod (void)
{
    struct stat	statb;

    LoadDMResources (FALSE);
    if (servers[0] == '/' && stat(servers, &statb) != -1)
    {
	if (statb.st_mtime != ServersModTime)
	{
	    Debug ("Servers file %s has changed, rescanning\n", servers);
	    LogInfo ("Rereading servers file %s\n", servers);
	    ServersModTime = statb.st_mtime;
	    ForEachDisplay (MarkDisplay);
	    ScanServers ();
	}
    }
#ifdef XDMCP
    if (accessFile && accessFile[0] && stat (accessFile, &statb) != -1)
    {
	if (statb.st_mtime != AccessFileModTime)
	{
	    Debug ("Access file %s has changed, rereading\n", accessFile);
	    LogInfo ("Rereading access file %s\n", accessFile);
	    AccessFileModTime = statb.st_mtime;
	    ScanAccessDatabase ();
	}
    }
#endif
}

/*
 * catch a SIGTERM, kill all displays and exit
 */

/* ARGSUSED */
static SIGVAL
StopAll (int n ATTR_UNUSED)
{
    Debug ("Shutting down entire manager\n");
#ifdef XDMCP
    DestroyWellKnownSockets ();
#endif
    ForEachDisplay (StopDisplay);
#ifdef SIGNALS_RESET_WHEN_CAUGHT
    /* to avoid another one from killing us unceremoniously */
    (void) Signal (SIGTERM, StopAll);
    (void) Signal (SIGINT, StopAll);
#endif
}

/*
 * notice that a child has died and may need another
 * sub-daemon started
 */

int	ChildReady;

#ifndef UNRELIABLE_SIGNALS
/* ARGSUSED */
static SIGVAL
ChildNotify (int n ATTR_UNUSED)
{
    ChildReady = 1;
#ifdef ISC
    (void) Signal (SIGCHLD, ChildNotify);
#endif
}
#endif

void
WaitForChild (void)
{
    int		pid;
    struct display	*d;
    waitType	status;
#ifndef X_NOT_POSIX
    sigset_t mask, omask;
#else
    int		omask;
#endif

#ifdef UNRELIABLE_SIGNALS
    /* XXX classic System V signal race condition here with RescanNotify */
    if ((pid = wait (&status)) != -1)
#else
# ifndef X_NOT_POSIX
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigaddset(&mask, SIGHUP);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_BLOCK, &mask, &omask);
    Debug ("signals blocked\n");
# else
    omask = sigblock (sigmask (SIGCHLD) | sigmask (SIGHUP) | sigmask (SIGALRM));
    Debug ("signals blocked, mask was 0x%x\n", omask);
# endif
    if (!ChildReady && !Rescan)
# ifndef X_NOT_POSIX
	sigsuspend(&omask);
# else
	sigpause (omask);
# endif
    ChildReady = 0;
# ifndef X_NOT_POSIX
    sigprocmask(SIG_SETMASK, &omask, (sigset_t *)NULL);
    while ((pid = waitpid (-1, &status, WNOHANG)) > 0)
# else
    sigsetmask (omask);
    while ((pid = wait3 (&status, WNOHANG, (struct rusage *) 0)) > 0)
# endif
#endif
    {
	Debug ("Manager wait returns  pid %d  sig %d  core %d  code %d\n",
	       pid, waitSig(status), waitCore(status), waitCode(status));
	if (autoRescan)
	    RescanIfMod ();
	/* SUPPRESS 560 */
	if ((d = FindDisplayByPid (pid))) {
	    d->pid = -1;
	    switch (waitVal (status)) {
	    case EX_REBOOT:
		Debug ("Display exited with EX_REBOOT\n");
		action = A_REBOOT;
		ExitDisplay (d, FALSE, FALSE, FALSE);
		StopAll (SIGTERM);
		break;
	    case EX_HALT:
		Debug ("Display exited with EX_HALT\n");
		action = A_HALT;
		ExitDisplay (d, FALSE, FALSE, FALSE);
		StopAll (SIGTERM);
		break;
	    case EX_TEXTLOGIN:
		Debug ("Display exited with EX_TEXTLOGIN\n");
		if (d->serverPid != -1) {
		    d->status = suspended;
		    TerminateProcess (d->serverPid, d->termSignal);
		} else	/* Something is _really_ wrong if we get here */
		    ExitDisplay (d, FALSE, FALSE, FALSE);
		break;
	    case EX_UNMANAGE_DPY:
		Debug ("Display exited with EX_UNMANAGE_DPY\n");
		ExitDisplay (d, FALSE, FALSE, FALSE);
		break;
	    case EX_NORMAL:
		Debug ("Display exited with EX_NORMAL\n");
		ExitDisplay (d, TRUE, FALSE, TRUE);
		break;
	    default:
		LogError ("Unknown session exit code from manager process\n");
		ExitDisplay (d, FALSE, FALSE, TRUE);
		break;
	    case EX_OPENFAILED_DPY:
		LogError ("Display %s cannot be opened\n", d->name);
		/*
		 * no display connection was ever made, tell the
		 * terminal that the open attempt failed
 		 */
#ifdef XDMCP
		if ((d->displayType & d_origin) == FromXDMCP)
		    SendFailed (d, "cannot open display");
#endif
		ExitDisplay (d, TRUE, TRUE, FALSE);
		break;
	    case EX_RESERVER_DPY:
		Debug ("Display exited with EX_RESERVER_DPY\n");
		ExitDisplay (d, TRUE, TRUE, TRUE);
		break;
	    case EX_AL_RESERVER_DPY:
		Debug ("Display exited with EX_AL_RESERVER_DPY\n");
		ExitDisplay (d, TRUE, TRUE, FALSE);
		break;
	    case waitCompose (SIGTERM,0,0):
		Debug ("Display exited on SIGTERM\n");
		ExitDisplay (d, TRUE, TRUE, FALSE);
		break;
	    case EX_REMANAGE_DPY:
		Debug ("Display exited with EX_REMANAGE_DPY\n");
		/*
 		 * XDMCP will restart the session if the display
		 * requests it
		 */
		ExitDisplay (d, TRUE, FALSE, TRUE);
		break;
	    }
	}
	/* SUPPRESS 560 */
	else if ((d = FindDisplayByServerPid (pid)))
	{
	    d->serverPid = -1;
	    switch (d->status)
	    {
	    case suspended:
		Debug ("Server for suspended display %s exited\n", d->name);
		SwitchToTty (d);
		break;
	    case zombie:
		Debug ("Zombie server reaped, removing display %s\n", d->name);
		RemoveDisplay (d);
		break;
	    case phoenix:
		Debug ("Phoenix server arises, restarting display %s\n", d->name);
		d->status = notRunning;
		break;
	    case running:
		LogError ("Server for display %s terminated unexpectedly\n", 
			  d->name);
		if (d->pid != -1)
		{
		    Debug ("Terminating session pid %d\n", d->pid);
		    TerminateProcess (d->pid, SIGTERM);
		}
		break;
	    case notRunning:
		Debug ("Server exited for notRunning session on display %s\n", 
		       d->name);
		break;
	    }
	}
	else
	{
	    Debug ("Unknown child termination\n");
	}
    }
    StartDisplays ();
}

static void
CheckDisplayStatus (struct display *d)
{
    if ((d->displayType & d_origin) == FromFile)
    {
	if (d->stillThere) {
	    if (d->status == notRunning)
		StartDisplay (d);
	} else
	    StopDisplay (d);
    }
}

static void
StartDisplays (void)
{
    ForEachDisplay (CheckDisplayStatus);
    CloseGetter ();
}

void
StartDisplay (struct display *d)
{
    char	buf[100];
    int		pid;
    Time_t	curtime;

    Debug ("StartDisplay %s, try %d\n", d->name, d->hstent->startTries + 1);

    if (!LoadDisplayResources (d))
    {
	LogError ("Unable to read configuration for display %s; stopping it.\n", 
		  d->name);
	StopDisplay (d);
	return;
    }

    time (&curtime);
    if (d->hstent->lastStart + d->startInterval < curtime)
	d->hstent->startTries = 0;
    else if (d->hstent->startTries > d->startAttempts)
    {
	Debug ("Ignoring disabled display %s\n", d->name);
	StopDisplay (d);
	return;
    } 
    if (++d->hstent->startTries > d->startAttempts)
    {
	LogError ("Display %s is being disabled (restarting too fast)\n",
		  d->name);
	StopDisplay (d);
	return;
    }
    d->hstent->lastStart = curtime;

    if ((d->displayType & d_location) == Local)
    {
	/* don't bother pinging local displays; we'll
	 * certainly notice when they exit
	 */
	d->pingInterval = 0;
	if (d->authorize)
	{
	    Debug ("SetLocalAuthorization %s, auth %s\n",
		    d->name, d->authNames[0]);
	    SetLocalAuthorization (d);
	    /*
	     * reset the server after writing the authorization information
	     * to make it read the file (for compatibility with old
	     * servers which read auth file only on reset instead of
	     * at first connection)
	     */
	    if (d->serverPid != -1 && d->resetForAuth && d->resetSignal)
		kill (d->serverPid, d->resetSignal);
	}
	if (d->serverPid == -1 && !StartServer (d))
	{
	    LogError ("Server for display %s can't be started, session disabled\n", d->name);
	    RemoveDisplay (d);
	    return;
	}
    }
    else
    {
	/* this will only happen when using XDMCP */
	if (d->authorizations)
	    SaveServerAuthorizations (d, d->authorizations, d->authNum);
    }
    if (d->fifoCreate && d->fifofd < 0) {
	sprintf (buf, "/tmp/xlogin-%s", d->name);
	unlink (buf);
	if (mkfifo (buf, 0) < 0)
	    LogError ("cannot create login data fifo %s\n", buf);
	else {
	    chown (buf, d->fifoOwner, d->fifoGroup);
	    if (d->fifoOwner >= 0)
		chmod (buf, 0600);
	    else if (d->fifoGroup >= 0)
		chmod (buf, 0620);
	    else
		chmod (buf, 0622);
	    if ((d->fifofd = open (buf, O_RDONLY | O_NONBLOCK)) < 0)
		unlink (buf);
	    else
		RegisterCloseOnFork (d->fifofd);
	}
    }
    if (d->pipefd[0] < 0) {
	if (!pipe (d->pipefd)) {
	    (void) fcntl (d->pipefd[0], F_SETFL, O_NONBLOCK);
	    RegisterCloseOnFork (d->pipefd[0]);
	}
    }
    if (!nofork_session) {
	Debug ("forking session\n");
	pid = Fork ();
    } else {
	Debug ("not forking session\n");
	CloseGetter ();
	pid = -2;
    }
    switch (pid)
    {
    case 0:
	if (debugLevel & DEBUG_WSESS)
	    sleep (100);
    case -2:
	(void) Signal (SIGPIPE, SIG_IGN);
	if (d->pipefd[0] >= 0)
	    RegisterCloseOnFork (d->pipefd[1]);
	SetAuthorization (d);
	if (!WaitForServer (d))
	    exit (EX_OPENFAILED_DPY);
#ifdef XDMCP
	if (d->useChooser)
	    RunChooser (d);
	else
#endif
	    ManageSession (d);
	exit (EX_REMANAGE_DPY);
    case -1:
	break;
    default:
	Debug ("forked session, pid %d\n", pid);
	d->pid = pid;
	d->status = running;
	break;
    }
}

static void
ClrnLog (struct display *d)
{
    WipeStr (d->hstent->nLogPipe);
    d->hstent->nLogPipe = NULL;
}

static void
savenLog (char *buf, int len, void *ptr)
{
    ReStrN (&((struct display *)ptr)->hstent->nLogPipe, buf, len);
}

static void
ReadnLog (struct display *d, int fd)
{
    FdGetsCall (fd, savenLog, d);
}

static void
ExitDisplay (
    struct display	*d, 
    int			doRestart,
    int			forceReserver,
    int			goodExit)
{
    Debug ("Recording exit of %s (GoodExit=%d)\n", d->name, goodExit);

    time (&d->hstent->lastExit);
    d->hstent->goodExit = goodExit;
    ClrnLog (d);
    if (d->pipefd[0] >= 0)
	ReadnLog (d, d->pipefd[0]);
    if (goodExit)
	ClrnLog (d);
    if (d->fifofd >= 0)
	ReadnLog (d, d->fifofd);

    if (!doRestart ||
	(d->displayType & d_lifetime) != Permanent ||
	d->status == zombie)
	StopDisplay (d);
    else
	RestartDisplay (d, forceReserver);
}

/*
 * transition from running to zombie or deleted
 */

void
StopDisplay (struct display *d)
{
    if (d->fifofd >= 0) {
	char buf[100];
	close (d->fifofd);
	ClearCloseOnFork (d->fifofd);
	d->fifofd = -1;
	sprintf (buf, "/tmp/xlogin-%s", d->name);
	unlink (buf);
    }
    if (d->pipefd[0] >= 0) {
	int i;
	for (i = 0; i < 2; i++) {
	    ClearCloseOnFork(d->pipefd[i]); 
	    close (d->pipefd[i]); 
	    d->pipefd[i] = -1;
	}
    }
    if (d->serverPid != -1)
	d->status = zombie; /* be careful about race conditions */
    if (d->pid != -1)
	TerminateProcess (d->pid, SIGTERM);
    if (d->serverPid != -1)
	TerminateProcess (d->serverPid, d->termSignal);
    else
	RemoveDisplay (d);
}

/*
 * transition from running to phoenix or notRunning
 */

static void
RestartDisplay (struct display *d, int forceReserver)
{
    if (d->serverPid != -1 && (forceReserver || d->terminateServer))
    {
	TerminateProcess (d->serverPid, d->termSignal);
	d->status = phoenix;
    }
    else
    {
	d->status = notRunning;
    }
}

static int  pidFd;
static FILE *pidFilePtr;

static int
StorePid (void)
{
    int		oldpid;

    if (pidFile[0] != '\0') {
	pidFd = open (pidFile, O_RDWR);
	if (pidFd == -1 && errno == ENOENT)
	    pidFd = open (pidFile, O_RDWR|O_CREAT, 0666);
	if (pidFd == -1 || !(pidFilePtr = fdopen (pidFd, "r+")))
	{
	    LogError ("process-id file %s cannot be opened\n",
		      pidFile);
	    return -1;
	}
	if (fscanf (pidFilePtr, "%d\n", &oldpid) != 1)
	    oldpid = -1;
	fseek (pidFilePtr, 0l, 0);
	if (lockPidFile)
	{
#ifdef F_SETLK
# ifndef SEEK_SET
#  define SEEK_SET 0
# endif
	    struct flock lock_data;
	    lock_data.l_type = F_WRLCK;
	    lock_data.l_whence = SEEK_SET;
	    lock_data.l_start = lock_data.l_len = 0;
	    if (fcntl(pidFd, F_SETLK, &lock_data) == -1)
	    {
		if (errno == EAGAIN)
		    return oldpid;
		else
		    return -1;
	    }
#else
# ifdef LOCK_EX
	    if (flock (pidFd, LOCK_EX|LOCK_NB) == -1)
	    {
		if (errno == EWOULDBLOCK)
		    return oldpid;
		else
		    return -1;
	    }
# else
	    if (lockf (pidFd, F_TLOCK, 0) == -1)
	    {
		if (errno == EACCES)
		    return oldpid;
		else
		    return -1;
	    }
# endif
#endif
	}
	fprintf (pidFilePtr, "%d\n", getpid ());
	(void) fflush (pidFilePtr);
	RegisterCloseOnFork (pidFd);
    }
    return 0;
}

#if 0
void
UnlockPidFile (void)
{
    if (lockPidFile)
# ifdef F_SETLK
    {
	struct flock lock_data;
	lock_data.l_type = F_UNLCK;
	lock_data.l_whence = SEEK_SET;
	lock_data.l_start = lock_data.l_len = 0;
	(void) fcntl(pidFd, F_SETLK, &lock_data);
    }
# else
#  ifdef F_ULOCK
	lockf (pidFd, F_ULOCK, 0);
#  else
	flock (pidFd, LOCK_UN);
#  endif
# endif
    close (pidFd);
    fclose (pidFilePtr);
}
#endif

void SetTitle (char *name, ...)
{
#ifndef NOXDMTITLE
    char	*p = Title;
    int	left = TitleLen;
    char	*s;
    va_list	args;

    va_start(args, name);
    *p++ = '-';
    --left;
    s = name;
    while (s)
    {
	while (*s && left > 0)
	{
	    *p++ = *s++;
	    left--;
	}
	s = va_arg (args, char *);
    }
    while (left > 0)
    {
	*p++ = '\0';
	--left;
    }
    va_end(args);
#endif	
}
