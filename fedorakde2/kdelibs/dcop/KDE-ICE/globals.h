/* $XConsortium: globals.h,v 1.14 94/04/17 20:15:33 mor Exp $ */
/******************************************************************************


Copyright (c) 1993  X Consortium

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
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.

Author: Ralph Mor, X Consortium
******************************************************************************/
/* $XFree86: xc/lib/ICE/globals.h,v 1.1.1.1.12.2 1998/10/19 20:57:04 hohndel Exp $ */

#include "KDE-ICE/ICElib.h"
#include "KDE-ICE/ICElibint.h"
#include "KDE-ICE/ICEutil.h"

#define _IceDefaultErrorHandler _KDE_IceDefaultErrorHandler

extern void _IceDefaultErrorHandler (
#if NeedFunctionPrototypes
    IceConn         /* iceConn */,
    Bool            /* swap */,
    int             /* offendingMinorOpcode */,
    unsigned long   /* offendingSequence */,
    int             /* errorClass */,
    int             /* severity */,
    IcePointer      /* values */
#endif
);

#define _IceDefaultIOErrorHandler _KDE_IceDefaultIOErrorHandler

extern void _IceDefaultIOErrorHandler (
#if NeedFunctionPrototypes
    IceConn 		/* iceConn */
#endif
);

#define _IcePoMagicCookie1Proc _KDE_IcePoMagicCookie1Proc

extern IcePoAuthStatus _IcePoMagicCookie1Proc (
#if NeedFunctionPrototypes
    IceConn         /* iceConn */,
    IcePointer *    /* authStatePtr */,
    Bool            /* cleanUp */,
    Bool            /* swap */,
    int             /* authDataLen */,
    IcePointer      /* authData */,
    int *           /* replyDataLenRet */,
    IcePointer *    /* replyDataRet */,
    char **         /* errorStringRet */
#endif
);

#define _IcePaMagicCookie1Proc _KDE_IcePaMagicCookie1Proc

extern IcePaAuthStatus _IcePaMagicCookie1Proc (
#if NeedFunctionPrototypes
    IceConn         /* iceConn */,
    IcePointer *    /* authStatePtr */,
    Bool            /* swap */,
    int             /* authDataLen */,
    IcePointer      /* authData */,
    int *           /* replyDataLenRet */,
    IcePointer *    /* replyDataRet */,
    char **         /* errorStringRet */
#endif
);

#define _IceProcessCoreMessage _KDE_IceProcessCoreMessage

extern void _IceProcessCoreMessage (
#if NeedFunctionPrototypes
    IceConn          /* iceConn */,
    int              /* opcode */,
    unsigned long    /* length */,
    Bool             /* swap */,
    IceReplyWaitInfo * /* replyWait */,
    Bool *           /* replyReadyRet */,
    Bool *           /* connectionClosedRet */
#endif
);

#define _IceConnectionObjs	_KDE_IceConnectionObjs
#define _IceConnectionStrings	_KDE_IceConnectionStrings
#define _IceConnectionCount	_KDE_IceConnectionCount

extern IceConn     	_IceConnectionObjs[256];
extern char	    	*_IceConnectionStrings[256];
extern int     		_IceConnectionCount;

#define _IceProtocols _KDE_IceProtocols
#define _IceLastMajorOpcode _KDE_IceLastMajorOpcode

extern _IceProtocol 	_IceProtocols[255];
extern int         	_IceLastMajorOpcode;

#define _IceAuthCount		_KDE_IceAuthCount
#define _IceAuthNames		_KDE_IceAuthNames
#define _IcePoAuthProcs 	_KDE_IcePoAuthProcs
#define _IcePaAuthProcs 	_KDE_IcePaAuthProcs

extern int		_IceAuthCount;
extern const char	*_IceAuthNames[];
extern IcePoAuthProc	_IcePoAuthProcs[];
extern IcePaAuthProc	_IcePaAuthProcs[];

#define _IceVersionCount	_KDE_IceVersionCount
#define _IceVersions		_KDE_IceVersions
#define _IceWatchProcs		_KDE_IceWatchProcs

extern int		_IceVersionCount;
extern _IceVersion	_IceVersions[];

extern _IceWatchProc	*_IceWatchProcs;

#define _IceErrorHandler 	_KDE_IceErrorHandler
#define _IceIOErrorHandler 	_KDE_IceIOErrorHandler

extern IceErrorHandler   _IceErrorHandler;
extern IceIOErrorHandler _IceIOErrorHandler;

#define _IcePaAuthDataEntryCount _KDE_IcePaAuthDataEntryCount
#define _IcePaAuthDataEntries 	_KDE_IcePaAuthDataEntries

extern int            _IcePaAuthDataEntryCount;
extern IceAuthDataEntry _IcePaAuthDataEntries[];
