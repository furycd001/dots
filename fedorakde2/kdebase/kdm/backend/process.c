/* $TOG: session.c /main/79 1998/02/09 13:56:17 kaleb $ */
/* $Id: process.c,v 2.8.2.2 2001/10/03 10:47:11 ossi Exp $ */
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

#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

static int opipe[2], ipipe[2], gpid;
Jmp_buf GErrJmp;


SIGVAL (*Signal (int sig, SIGFUNC handler))(int)
{
#if !defined(X_NOT_POSIX) && !defined(__EMX__)
    struct sigaction sigact, osigact;
    sigact.sa_handler = handler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigaction(sig, &sigact, &osigact);
    return osigact.sa_handler;
#else
    return signal(sig, handler);
#endif
}


void
TerminateProcess (int pid, int sig)
{
    kill (pid, sig);
#ifdef SIGCONT
    kill (pid, SIGCONT);
#endif
}


static FD_TYPE	CloseMask;
static int	max;

void
RegisterCloseOnFork (int fd)
{
    FD_SET (fd, &CloseMask);
    if (fd > max)
	max = fd;
}

void
ClearCloseOnFork (int fd)
{
    FD_CLR (fd, &CloseMask);
}

static void
CloseOnFork (void)
{
    int	fd;

    for (fd = 0; fd <= max; fd++)
	if (FD_ISSET (fd, &CloseMask))
	{
#ifdef MINIX
	    nbio_unregister(fd);
#endif
	    close (fd);
        }
    FD_ZERO (&CloseMask);
#ifdef MINIX
    { extern int chooserFd; nbio_unregister(chooserFd); }
#endif
}

int
Fork ()
{
    int pid;

#ifndef X_NOT_POSIX
    sigset_t ss, oss; 
    sigemptyset(&ss);
    sigfillset(&ss);
    sigprocmask(SIG_SETMASK, &ss, &oss);
#else
    int omask = sigsetmask (-1);
#endif

    if (!(pid = fork())) {
#ifdef SIGCHLD
	(void) Signal (SIGCHLD, SIG_DFL);
#endif
	(void) Signal (SIGTERM, SIG_DFL);
	(void) Signal (SIGINT, SIG_IGN);	/* for -nodaemon */
	(void) Signal (SIGPIPE, SIG_DFL);
	(void) Signal (SIGALRM, SIG_DFL);
	(void) Signal (SIGHUP, SIG_DFL);
#ifndef X_NOT_POSIX
	sigemptyset(&ss);
	sigprocmask(SIG_SETMASK, &ss, NULL);
#else
	sigsetmask (0);
#endif
	CloseOnFork ();
	gpid = 0;
	return 0;
    }

#ifndef X_NOT_POSIX
    sigprocmask(SIG_SETMASK, &oss, 0);
#else
    sigsetmask (omask);
#endif

    return pid;
}

int
Wait4 (int pid)
{
    waitType	result;

#ifndef X_NOT_POSIX
    if (waitpid (pid, &result, 0) < 0)
#else
    if (wait4 (pid, &result, 0, (struct rusage *)0) < 0)
#endif
    {
	Debug ("Wait4(%d) failed\n", pid);
	return 0;
    }
    return waitVal (result);
}


void
execute (char **argv, char **environ)
{
    Debug ("execute: %[s ; %[s\n", argv, environ);
    execve (argv[0], argv, environ);
    /*
     * In case this is a shell script which hasn't been
     * made executable (or this is a SYSV box), do
     * a reasonable thing
     */
    if (errno != ENOENT) {
	char	program[1024], *e, **newargv;
	FILE	*f;

	/*
	 * emulate BSD kernel behaviour -- read
	 * the first line; check if it starts
	 * with "#!", in which case it uses
	 * the rest of the line as the name of
	 * program to run.  Else use "/bin/sh".
	 */
	if (!(f = fopen (argv[0], "r")))
	    return;
	if (!fgets (program, sizeof(program), f))
 	{
	    fclose (f);
	    return;
	}
	fclose (f);
	e = program + strlen (program) - 1;
	if (*e == '\n')
	    *e = '\0';
	if (!strncmp (program, "#!", 2))
	    newargv = parseArgs (0, program + 2);
	else
	    newargv = addStrArr (0, "/bin/sh", 7);
	mergeStrArrs (&newargv, argv);
	Debug ("Shell script execution: %[s\n", newargv);
	execve (newargv[0], newargv, environ);
    }
}

int
runAndWait (char **args, char **environ)
{
    int	pid, ret;

    switch (pid = Fork ()) {
    case 0:
	execute (args, environ);
	LogError ("can't execute \"%s\" (err %d)\n", args[0], errno);
	exit (1);
    case -1:
	LogError ("can't fork to execute \"%s\" (err %d)\n", args[0], errno);
	return 1;
    }
    ret = Wait4 (pid);
    return waitVal (ret);
}


static void
GClosen ()
{
    ClearCloseOnFork (opipe[1]);
    ClearCloseOnFork (ipipe[0]);
    close (opipe[1]);
    close (ipipe[0]);
}

static void
GCloseAll ()
{
    GClosen ();
    close (opipe[0]);
    close (ipipe[1]);
}

char *
GOpen (char **argv, char *what, char **env)
{
    char **margv, coninfo[20];

    if (gpid)
	return "another helper program is already running";
    if (pipe (opipe))
	return "pipe() failed";
    if (pipe (ipipe)) {
	close (opipe[0]);
	close (opipe[1]);
	return "pipe() failed";
    }
    RegisterCloseOnFork (opipe[1]);
    RegisterCloseOnFork (ipipe[0]);
    if (!(margv = xCopyStrArr (1, argv))) {
	GCloseAll ();
	return "out of memory";
    }
    if (!StrApp (margv, progpath, what, (char *)0)) {
	free (margv);
	GCloseAll ();
	return "out of memory";
    }
    switch (gpid = Fork()) {
    case -1:
	free (margv[0]);
	free (margv);
	GCloseAll ();
	return "fork() failed";
    case 0:
	(void) Signal (SIGPIPE, SIG_IGN);
	sprintf (coninfo, "CONINFO=%d %d", opipe[0], ipipe[1]);
	env = putEnv (coninfo, env);
	execute (margv, env);
	LogPanic ("Cannot execute '%s'\n", margv[0]);
    default:
	Debug ("Forked helper %s, pid %d\n", margv[0], gpid);
	free (margv[0]);
	free (margv);
	close (opipe[0]);
	close (ipipe[1]);
	(void) Signal (SIGPIPE, SIG_IGN);
	GSendInt (debugLevel);
	return (char *)0;
    }
}

int
GClose ()
{
    int	ret;

    if (!gpid) {
	Debug ("Whoops, GClose while no helper is running\n");
	return 0;
    }
    GClosen ();
/*    TerminateProcess (gpid, SIGTERM);*/
    ret = Wait4 (gpid);
    gpid = 0;
    if (ret)
	LogError ("Abnormal helper termination, code %d, signal %d\n", 
		  WaitCode(ret), WaitSig(ret));
    return ret;
}

static void GError (void) ATTR_NORETURN;

static void
GError ()
{
    (void) GClose ();
    Longjmp (GErrJmp, 1);
}

static void
GRead (void *buf, int len)
{
    if (Reader (ipipe[0], buf, len) != len) {
	LogError ("Cannot read from helper\n");
	GError ();
    }
}

static void
GWrite (void *buf, int len)
{
#if 0
    int ret;
    do {
	ret = write (opipe[1], buf, len);
    } while (ret < 0 && errno == EINTR);
    if (ret != len) {
#else
    if (write (opipe[1], buf, len) != len) {
#endif
	LogError ("Cannot write to helper\n");
	GError ();
    }
}

void
GSendInt (int val)
{
    GDebug ("Sending int %d (0x%x) to helper\n", val, val);
    GWrite (&val, sizeof(val));
}

int
GRecvInt ()
{
    int val;

    GDebug ("Receiving int from helper ...\n");
    GRead (&val, sizeof(val));
    GDebug (" -> %d (0x%x)\n", val, val);
    return val;
}

int
GRecvCmd (int *cmd)
{
    GDebug ("Receiving command from helper ...\n");
    if (Reader (ipipe[0], cmd, sizeof(*cmd)) == sizeof(*cmd)) {
	GDebug (" -> %d\n", *cmd);
	return 1;
    }
    GDebug (" -> no data\n");
    return 0;
}

#ifdef XDMCP
void
GSendArr (int len, char *data)
{
    GDebug ("Sending array[%d] %02[*{hhx to helper\n", len, len, data);
    GWrite (&len, sizeof(len));
    GWrite (data, len);
}
#endif

static char *
iGRecvArr (int *rlen)
{
    int len;
    char *buf;

    GRead (&len, sizeof (len));
    *rlen = len;
    GDebug (" -> %d bytes\n", len);
    if (!len)
	return (char *)0;
    if (!(buf = malloc (len)))
    {
	LogOutOfMem ("GRecvArr");
	GError ();
    }
    GRead (buf, len);
    return buf;
}

/*
char *
GRecvArr (int *rlen)
{
    char *buf;

    GDebug ("Receiving array from helper ...\n");
    buf = iGRecvArr (rlen);
    GDebug (" -> %02[*{hhx\n", *rlen, buf);
    return buf;
}
*/

static int
iGRecvArrBuf (char *buf)
{
    int len;

    GRead (&len, sizeof (len));
    GDebug (" -> %d bytes\n", len);
    if (len)
	GRead (buf, len);
    return len;
}

/*
int
GRecvArrBuf (char *buf)
{
    int len;

    GDebug ("Receiving already allocated array from helper ...\n");
    len = iGRecvArrBuf (buf);
    GDebug (" -> %02[*{hhx\n", len, buf);
    return len;
}
*/

int
GRecvStrBuf (char *buf)
{
    int len;

    GDebug ("Receiving already allocated string from helper ...\n");
    len = iGRecvArrBuf (buf);
    GDebug (" -> %'.*s\n", len, buf);
    return len;
}

void
GSendStr (char *buf)
{
    int len;

    GDebug ("Sending string %'s to helper\n", buf);
    if (buf) {
	len = strlen (buf) + 1;
	GWrite (&len, sizeof(len));
	GWrite (buf, len);
    } else
	GWrite (&buf, sizeof(int));
}

char *
GRecvStr ()
{
    int len;
    char *buf;

    GDebug ("Receiving string from helper ...\n");
    buf = iGRecvArr (&len);
    GDebug (" -> %'.*s\n", len, buf);
    return buf;
}

static void
iGSendStrArr (int num, char **data)
{
    char **cdata;

    GWrite (&num, sizeof(num));
    for (cdata = data; --num >= 0; cdata++)
	GSendStr (*cdata);
}

/*
void
GSendStrArr (int num, char **data)
{
    GDebug ("Sending string array[%d] to helper\n", num);
    iGSendStrArr (num, data);
}
*/

char **
GRecvStrArr (int *rnum)
{
    int num;
    char **argv, **cargv;

    GDebug ("Receiving string array from helper ...\n");
    GRead (&num, sizeof(num));
    GDebug (" -> %d strings\n", num);
    *rnum = num;
    if (!num)
	return (char **)0;
    if (!(argv = malloc (num * sizeof(char *))))
    {
	LogOutOfMem ("GRecvStrArr");
	GError ();
    }
    for (cargv = argv; --num >= 0; cargv++)
	*cargv = GRecvStr ();
    return argv;
}

void
GSendArgv (char **argv)
{
    int num;

    if (argv) {
	for (num = 0; argv[num]; num++);
	GDebug ("Sending argv[%d] to helper ...\n", num);
	iGSendStrArr (num + 1, argv);
    } else {
	GDebug ("Sending NULL argv to helper\n");
	GWrite (&argv, sizeof(int));
    }
}

char **
GRecvArgv ()
{
    int num;

    return GRecvStrArr (&num);
}

