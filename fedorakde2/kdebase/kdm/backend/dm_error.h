/*
 * $XFree86: xc/programs/xdm/dm_error.h,v 1.1 1998/10/10 15:25:33 dawes Exp $
 */

/************************************************************

Copyright 1998 by Thomas E. Dickey <dickey@clark.net>

                        All Rights Reserved

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE ABOVE LISTED COPYRIGHT HOLDER(S) BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name(s) of the above copyright
holders shall not be used in advertising or otherwise to promote the
sale, use or other dealings in this Software without prior written
authorization.

********************************************************/


#ifndef _DM_ERROR_H_
#define _DM_ERROR_H_ 1

#include "greet.h"

#include <stdarg.h>

extern void GDebug	(char *fmt, ...);
extern void Debug	(char *fmt, ...);
extern void LogInfo	(char *fmt, ...);
extern void LogError	(char *fmt, ...);
extern void LogPanic	(char *fmt, ...) ATTR_NORETURN;
extern void LogOutOfMem	(char *fkt);
extern void Panic	(char *mesg) ATTR_NORETURN;
extern void InitErrorLog(char *errorLogFile);
#ifdef USE_SYSLOG
extern void ReInitErrorLog(void);
#else
# define ReInitErrorLog() while(0)
#endif

#endif /* _DM_ERROR_H_ */
