/* $TOG: error.c /main/17 1998/02/09 13:55:13 kaleb $ */
/* $Id: error.c,v 1.22 2001/07/25 18:36:43 ossi Exp $ */
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
/* $XFree86: xc/programs/xdm/error.c,v 1.2 1998/10/10 15:25:34 dawes Exp $ */

/*
 * xdm - display manager daemon
 * Author:  Keith Packard, MIT X Consortium
 *
 * error.c
 *
 * Log display manager errors to a file as
 * we generally do not have a terminal to talk to
 * or use syslog if it exists
 */

#include "dm.h"
#include "dm_error.h"

#include <stdio.h>

#define PRINT_QUOTES
#define PRINT_ARRAYS
#define LOG_DEBUG_MASK DEBUG_CORE
#define LOG_PANIC_EXIT 1
#include "printf.c"

void
GDebug (char *fmt, ...)
{
    va_list args;

    if (debugLevel & DEBUG_HLPCON)
    {
	va_start(args, fmt);
	Logger (DM_DEBUG, fmt, args);
	va_end(args);
    }
}

void
Panic (char *mesg)
{
#ifdef USE_SYSLOG
    syslog(LOG_ALERT, mesg);
#else
    int i = creat ("/dev/console", 0666);
    write (i, prog, strlen(prog));
    write (i, " panic: ", 8);
    write (i, mesg, strlen (mesg));
#endif
    exit (1);
}

#if defined(USE_SYSLOG) && defined(USE_PAM)
void
ReInitErrorLog ()
{
    InitLog ();
}
#endif

void
InitErrorLog (char *errorLogFile)
{
#ifdef USE_SYSLOG
# ifdef USE_PAM
    ReInitErrorLog ();
# else
    InitLog ();
# endif
#endif
    /* We do this independently of using syslog, as we cannot redirect
     * the output of external programs to syslog.
     */
    if (isatty (2)) {
	char buf[100];
	if (!errorLogFile) {
	    sprintf (buf, "/var/log/%s.log", prog);
	    errorLogFile = buf;
	}
	if (!freopen (errorLogFile, "a", stderr))
	    LogError ("Cannot open log file %s\n", errorLogFile);
    }
    dup2 (2, 1);
}

