/* $TOG: daemon.c /main/17 1998/02/09 13:54:47 kaleb $ */
/* $Id: daemon.c,v 1.18 2001/07/30 00:54:23 ossi Exp $ */
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
/* $XFree86: xc/programs/xdm/daemon.c,v 3.11 2000/08/10 17:40:41 dawes Exp $ */

/*
 * xdm - display manager daemon
 * Author:  Keith Packard, MIT X Consortium
 */

#include <X11/Xos.h>

#if defined(SVR4) || defined(USG)
# include <termios.h>
#else
# include <sys/ioctl.h>
#endif
#if defined(__osf__) || defined(linux) || defined(MINIX) || defined(__GNU__) || defined(__CYGWIN__)
# define setpgrp setpgid
#endif
#ifdef hpux
# include <sys/ptyio.h>
# ifndef TIOCNOTTY
#  define TIOCNOTTY  _IO('t', 113)           /* void tty association */
# endif
#endif
#include <errno.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif
#include <sys/types.h>
#ifdef X_NOT_POSIX
# define Pid_t int
#else
# define Pid_t pid_t
#endif

#ifndef X_NOT_STDC_ENV
# include <stdlib.h>
#else
extern void exit (int);
#endif


#include "dm.h"
#include "dm_error.h"

void
BecomeDaemon (void)
{
    Pid_t child_id;
#ifdef CSRG_BASED
    int fd;
#else
    int sts;
    register int i;
#endif

    /*
     * fork so that the process goes into the background automatically. Also
     * has a nice side effect of having the child process get inherited by
     * init (pid 1).
     * Separate the child into its own process group before the parent
     * exits.  This eliminates the possibility that the child might get
     * killed when the init script that's running xdm exits.
     */

    child_id = fork();
    switch (child_id) {
    case 0:
	/* child */
	break;
    case -1:
	/* error */
	LogError("daemon fork failed, errno = %d\n", errno);
	break;

    default:
	/* parent */

#ifndef CSRG_BASED
# if defined(SVR4) || defined(__QNXNTO__)
	sts = setpgid(child_id, child_id);
	/* This gets error EPERM.  Why? */
# else
#  if defined(SYSV)
	sts = 0;	/* don't know how to set child's process group */
#  else
	sts = setpgrp(child_id, child_id);
#   ifndef MINIX
	if (sts)
	    LogError("setting process grp for daemon failed, errno = %d\n",
		     errno);
#   endif /* MINIX */
#  endif
# endif
#endif /* !CSRG_BASED */
	exit (0);
    }

#ifndef CSRG_BASED
    /*
     * Close standard file descriptors and get rid of controlling tty
     */

# if defined(SYSV) || defined(SVR4) || defined(__QNXNTO__)
    setpgrp ();
# else
    setpgrp (0, getpid());
# endif

    chdir("/");

# ifndef __EMX__
#  ifdef MINIX
#   if 0
    /* Use setsid() to get rid of our controlling tty, this requires an extra
     * fork though.
     */
    setsid();
    if (fork() > 0)
    	_exit(0);
#   endif
#  else /* !MINIX */
#   if !((defined(SYSV) || defined(SVR4)) && defined(i386)) && !defined(__CYGWIN__)
    if ((i = open ("/dev/tty", O_RDWR)) >= 0) {	/* did open succeed? */
#    if defined(USG) && defined(TCCLRCTTY)
	int zero = 0;
	(void) ioctl (i, TCCLRCTTY, &zero);
#    else
#     if (defined(SYSV) || defined(SVR4)) && defined(TIOCTTY)
	int zero = 0;
	(void) ioctl (i, TIOCTTY, &zero);
#     else
	(void) ioctl (i, TIOCNOTTY, (char *) 0);    /* detach, BSD style */
#     endif
#    endif
	(void) close (i);
    }
#   endif /* !((SYSV || SVR4) && i386) */
#  endif /* MINIX */
# endif /* !__EMX__ */

#else
    daemon (0, 1);
#endif /* CSRG_BASED */
}
