/* $TOG: session.c /main/79 1998/02/09 13:56:17 kaleb $ */
/* $Id: session.c,v 1.58.2.4 2001/10/25 09:51:15 ossi Exp $ */
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
/* $XFree86: xc/programs/xdm/session.c,v 3.23 2000/06/17 00:27:34 dawes Exp $ */

/*
 * xdm - display manager daemon
 * Author:  Keith Packard, MIT X Consortium
 *
 * session.c
 */

#include "dm.h"
#include "dm_error.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

static char *
s_copy (char **dst, char *src, int spc)
{
    int idx;

    while (*src == ' ')
	src++;
    for (idx = 0; src[idx] >= ' ' && (spc || src[idx] != ' '); idx++);
    (void) StrNDup (dst, src, idx);
    return src + idx;
}

static int
AutoLogon (struct display *d, char **namer, char **passr, char ***sessargs)
{
    char *str;
    Time_t tdiff;

    if (!autoLogin)
	return 0;
    tdiff = time (0) - d->hstent->lastExit - d->openDelay;
Debug ("autoLogon, tdiff = %d, nlogpipe%s empty, goodexit = %d\n", 
	tdiff, d->hstent->nLogPipe ? " not":"", d->hstent->goodExit);
    if (d->hstent->nLogPipe && tdiff <= 0) {
	char *cp;
	if (d->hstent->nLogPipe[0] == '\n')
	    return 0;
	cp = s_copy(&str, d->hstent->nLogPipe, 0);
	cp = s_copy(namer, cp, 0);
	(void) s_copy(passr, cp, 1);
	*sessargs = addStrArr (0, str, -1);
    } else if (d->autoUser[0] != '\0') {
	if (tdiff <= 0) {
	    if (d->hstent->goodExit)
		return 0;
	} else {
	    if (!d->autoLogin1st)
		return 0;
	}
	StrDup (namer, d->autoUser);
	StrDup (passr, d->autoPass);
	RdUsrData (d, d->autoUser, sessargs);
	if (!*sessargs || !*sessargs[0]) {
	    if (d->autoString[0])
		*sessargs = parseArgs (*sessargs, d->autoString);
	    else
		*sessargs = addStrArr (*sessargs, "default", 7);
	}
    } else
	return 0;

    if (Verify (d, *namer, *passr) == V_OK)
	return 1;
    free (*namer);
    WipeStr (*passr);
    freeStrArr (*sessargs);
    return 0;
}



static Jmp_buf	abortSession;

/* ARGSUSED */
static SIGVAL
catchTerm (int n ATTR_UNUSED)
{
    Longjmp (abortSession, 1);
}

static Jmp_buf	pingTime;

/* ARGSUSED */
static SIGVAL
catchAlrm (int n ATTR_UNUSED)
{
    Longjmp (pingTime, 1);
}

static Jmp_buf	tenaciousClient;

/* ARGSUSED */
static SIGVAL
waitAbort (int n ATTR_UNUSED)
{
    Longjmp (tenaciousClient, 1);
}

#if defined(_POSIX_SOURCE) || defined(SYSV) || defined(SVR4)
# define killpg(pgrp, sig) kill(-(pgrp), sig)
#endif

static void
AbortClient (int pid)
{
    int	sig = SIGTERM;
#ifdef __STDC__
    volatile int	i;
#else
    int	i;
#endif
    int	retId;
    for (i = 0; i < 4; i++) {
	if (killpg (pid, sig) == -1) {
	    switch (errno) {
	    case EPERM:
		LogError ("Can't kill client\n");
	    case EINVAL:
	    case ESRCH:
		return;
	    }
	}
	if (!Setjmp (tenaciousClient)) {
	    (void) Signal (SIGALRM, waitAbort);
	    (void) alarm ((unsigned) 10);
	    retId = wait ((waitType *) 0);
	    (void) alarm ((unsigned) 0);
	    (void) Signal (SIGALRM, SIG_DFL);
	    if (retId == pid)
		break;
	} else
	    (void) Signal (SIGALRM, SIG_DFL);
	sig = SIGKILL;
    }
}


/*
 * We need our own error handlers because we can't be sure what exit code Xlib
 * will use, and our Xlib does exit(1) which matches EX_REMANAGE_DPY, which
 * can cause a race condition leaving the display wedged.  We need to use
 * EX_RESERVER_DPY for IO errors, to ensure that the manager waits for the
 * server to terminate.  For other X errors, we should give up.
 */

/*ARGSUSED*/
static int
IOErrorHandler (Display *dpy ATTR_UNUSED)
{
    LogError("fatal IO error %d (%s)\n", errno, _SysErrorMsg(errno));
    exit(EX_AL_RESERVER_DPY);	/* XXX */
    /*NOTREACHED*/
    return 0;
}

/*ARGSUSED*/
static int
ErrorHandler(Display *dpy ATTR_UNUSED, XErrorEvent *event)
{
    LogError("X error\n");
    if (event->error_code == BadImplementation)
	exit(EX_UNMANAGE_DPY); /* XXX */
    return 0;
}


static void
GreetUser (struct display *d, char **namer, char **passr, char ***sessargs)
{
    int		i, cmd, type, exitCode;
    char	*ptr, *name, *pass, **avptr, **args;
#ifdef XDMCP
    ARRAY8Ptr	aptr;
#endif

    /*
     * Load system default Resources (if any)
     */
    LoadXloginResources (d);

    if (Setjmp (GErrJmp))
	SessionExit (d, EX_RESERVER_DPY);
    ptr = GOpen ((char **)0, "_greet", systemEnv (d, 0, 0));
    if (ptr) {
	LogError ("Cannot run greeter: %s\n", ptr);
	goto bail;
    }
    exitCode = -1;
    *namer = (char *)0;
    while (GRecvCmd (&cmd)) {
	switch (cmd)
	{
	case G_GetCfg:
	    Debug ("G_GetCfg\n");
	    type = GRecvInt ();
	    Debug (" index 0x%x\n", type);
	    if (!(avptr = FindCfgEnt (d, type))) {
		Debug (" -> not found\n");
		GSendInt (GE_NoEnt);
		break;
	    }
	    switch (type & C_TYPE_MASK) {
	    default:
		Debug (" -> unknown type\n");
		GSendInt (GE_BadType);
		break;
	    case C_TYPE_INT:
	    case C_TYPE_STR:
	    case C_TYPE_ARGV:
#ifdef XDMCP
	    case C_TYPE_ARR:
#endif
		GSendInt (GE_Ok);
		switch (type & C_TYPE_MASK) {
		case C_TYPE_INT:
		    Debug (" -> int %#x (%d)\n", *(int *)avptr, *(int *)avptr);
		    GSendInt (*(int *)avptr);
		    break;
		case C_TYPE_STR:
		    Debug (" -> string %'s\n", *avptr);
		    GSendStr (*avptr);
		    break;
		case C_TYPE_ARGV:
		    Debug (" -> sending argv %'[{s\n", *(char ***)avptr);
		    GSendArgv (*(char ***)avptr);
		    break;
#ifdef XDMCP
		case C_TYPE_ARR:
		    aptr = *(ARRAY8Ptr *)avptr;
		    Debug (" -> sending array %02[*:hhx\n", 
			   aptr->length, aptr->data);
		    GSendArr (aptr->length, aptr->data);
		    break;
#endif
		}
		break;
	    }
	    break;
	case G_GetSessArg:
	    Debug ("G_GetSessArg\n");
	    name = GRecvStr ();
	    Debug (" user '%s'\n", name);
	    RdUsrData (d, name, &args);
	    Debug (" -> %'[{s\n", args);
	    GSendArgv (args);
	    freeStrArr (args);
	    free (name);
	    break;
	case G_SessionExit:
	    Debug ("G_SessionExit\n");
	    exitCode = GRecvInt ();
	    Debug (" code %d\n", exitCode);
	    break;
	case G_Verify:
	    Debug ("G_Verify\n");
	    name = GRecvStr ();
	    Debug (" user '%s'\n", name);
	    pass = GRecvStr ();
	    Debug (pass[0] ? " password\n" : " no password\n");
	    GSendInt (i = Verify (d, name, pass));
	    Debug (" -> return %d\n", i);
	    free (name);
	    WipeStr (pass);
	    break;
	case G_Restrict:
	    Debug ("G_Restrict(...)\n");
	    Restrict (d);
	    break;
	case G_Login:
	    Debug ("G_Login\n");
	    *namer = GRecvStr ();
	    Debug (" user '%s'\n", *namer);
	    *passr = GRecvStr ();
	    Debug ((*passr)[0] ? " password\n" : " no password\n");
	    *sessargs = GRecvArgv ();
	    Debug (" arguments: %'[{s\n", *sessargs);
	    break;
	case G_SetupDpy:
	    Debug ("G_SetupDpy\n");
	    SetupDisplay (d);
	    break;
	default:
	    LogError ("Received unknown command 0x%x from greeter\n", cmd);
	    (void) GClose ();
	    goto bail;
	}
    }
    if (GClose ())
	exitCode = EX_UNMANAGE_DPY;
    DeleteXloginResources (d);
    if (exitCode >= 0)
	SessionExit (d, exitCode);
    if (!*namer) {
	LogError ("Greeter exited unexpectedly\n");
	SessionExit (d, EX_RESERVER_DPY);
    }
    return;

  bail:
    Debug ("bailing from GreetUser\n");
    SessionExit (d, EX_RESERVER_DPY);
}


void
ManageSession (struct display *d)
{
    volatile int	exitCode, clientPid;
    int		pid;
    char	*name, *pass, **sessargs;

    Debug ("ManageSession %s\n", d->name);
    (void)XSetIOErrorHandler(IOErrorHandler);
    (void)XSetErrorHandler(ErrorHandler);
    SetTitle(d->name, (char *) 0);

    if (!AutoLogon(d, &name, &pass, &sessargs))
	GreetUser(d, &name, &pass, &sessargs);

    exitCode = EX_NORMAL;
    clientPid = 0;
    if (!Setjmp (abortSession)) {
	(void) Signal (SIGTERM, catchTerm);
	/*
	 * Start the clients, changing uid/groups
	 *	   setting up environment and running the session
	 */
	clientPid = StartClient (d, name, pass, sessargs);

	/* XXX perfect crap */
	if (clientPid && d->pipefd[1] >= 0) {
	    char *buf;
	    if (d->autoReLogin &&
		(buf = malloc (strlen(sessargs[0]) + strlen(name) + strlen(pass) + 4))) {
		write (d->pipefd[1], buf, 
		       sprintf (buf, "%s %s %s\n", sessargs[0], name, pass));
		free (buf);
	    } else
		write (d->pipefd[1], "\n", 1);
	}

	free (name);
	WipeStr (pass);
	freeStrArr (sessargs);

	if (clientPid) {
	    Debug ("Client Started\n");

	    /*
	     * Wait for session to end,
	     */
	    for (;;) {
		if (!Setjmp (pingTime))
		{
		    (void) Signal (SIGALRM, catchAlrm);
		    (void) alarm (d->pingInterval * 60); /* may be 0 */
		    pid = wait ((waitType *) 0);
		    (void) alarm (0);
		    if (pid == clientPid)
			break;
		}
		else
		{
		    (void) alarm (0);
		    if (!PingServer (d))
			catchTerm (SIGTERM);
		}
	    }
	    /* 
	     * Sometimes the Xsession somehow manages to exit before
	     * a server crash is noticed - so we sleep a bit and wait
	     * for being killed.
	     */
	    if (!PingServer (d)) {
		Debug("X-Server dead upon session exit.\n");
		if ((d->displayType & d_origin) == Local)
		    sleep (10);
		exitCode = EX_AL_RESERVER_DPY;
	    }
	} else {
	    LogError ("session start failed\n");
	}
    } else {
	/*
	 * when terminating the session, nuke
	 * the child and then run the reset script
	 */
	if (clientPid)
	    AbortClient (clientPid);
	exitCode = EX_AL_RESERVER_DPY;
    }
    SessionExit (d, exitCode);
}

void
LoadXloginResources (struct display *d)
{
    char	**args;
    char	**env = 0;

    if (d->resources[0] && access (d->resources, 4) == 0) {
	env = systemEnv (d, (char *) 0, (char *) 0);
	args = parseArgs ((char **) 0, d->xrdb);
	args = parseArgs (args, d->resources);
	Debug ("Loading resource file: %s\n", d->resources);
	(void) runAndWait (args, env);
	freeStrArr (args);
	freeStrArr (env);
    }
}

void
SetupDisplay (struct display *d)
{
    char	**env = 0;

    if (d->setup && d->setup[0])
    {
    	env = systemEnv (d, (char *) 0, (char *) 0);
    	(void) source (env, d->setup);
    	freeStrArr (env);
    }
}

void
DeleteXloginResources (struct display *d)
{
    int i;
    Atom prop = XInternAtom(d->dpy, "SCREEN_RESOURCES", True);

    XDeleteProperty(d->dpy, RootWindow (d->dpy, 0), XA_RESOURCE_MANAGER);
    if (prop) {
	for (i = ScreenCount(d->dpy); --i >= 0; )
	    XDeleteProperty(d->dpy, RootWindow (d->dpy, i), prop);
    }
}


int
source (char **environ, char *file)
{
    char	**args, *args_safe[2];
    int		ret;

    if (file && file[0]) {
	Debug ("source %s\n", file);
	args = parseArgs ((char **) 0, file);
	if (!args)
	{
	    args = args_safe;
	    args[0] = file;
	    args[1] = NULL;
	    return runAndWait (args, environ);
	}
	ret = runAndWait (args, environ);
	freeStrArr (args);
	return ret;
    }
    return 0;
}

char **
inheritEnv (char **env, char **what)
{
    char	*value;

    for (; *what; ++what)
	if ((value = getenv (*what)))
	    env = setEnv (env, *what, value);
    return env;
}

char **
defaultEnv (char *user)
{
    char	**env;

    env = 0;

#ifdef AIXV3
    /* we need the tags SYSENVIRON: and USRENVIRON: in the call to setpenv() */
    env = setEnv(env, "SYSENVIRON:", "");
#endif

    if (user) {
	env = setEnv (env, "USER", user);
#ifdef AIXV3
	env = setEnv (env, "LOGIN", user);
#endif
	env = setEnv (env, "LOGNAME", user);
    }

#ifdef AIXV3
    env = setEnv(env, "USRENVIRON:", "");
#endif

    if (exportList)
	env = inheritEnv (env, exportList);

    return env;
}

char **
systemEnv (struct display *d, char *user, char *home)
{
    char	**env;

    env = defaultEnv (user);
    if (home)
	env = setEnv (env, "HOME", home);
    env = setEnv (env, "DISPLAY", d->name);
    env = setEnv (env, "PATH", d->systemPath);
    env = setEnv (env, "SHELL", d->systemShell);
    if (d->authFile)
	env = setEnv (env, "XAUTHORITY", d->authFile);
    return env;
}
