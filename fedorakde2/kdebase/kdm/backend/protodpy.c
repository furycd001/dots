/*
 * $TOG: protodpy.c /main/16 1998/02/09 13:55:55 kaleb $
 * $Id: protodpy.c,v 1.8 2001/07/25 18:36:43 ossi Exp $
 *
Copyright 1989, 1998  The Open Group

All Rights Reserved.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.
 *
 * Author:  Keith Packard, MIT X Consortium
 */
/* $XFree86: xc/programs/xdm/protodpy.c,v 1.2 1998/10/10 15:25:37 dawes Exp $ */

/*
 * protodpy.c
 *
 * manage a collection of proto-displays.  These are displays for
 * which sessionID's have been generated, but no session has been
 * started.
 */

#include "dm.h"

#ifdef XDMCP

#include "dm_error.h"

#include <sys/types.h>
#ifdef X_NOT_STDC_ENV
#define Time_t long
extern Time_t time ();
#else
#include <time.h>
#define Time_t time_t
#endif

static struct protoDisplay	*protoDisplays;

#ifdef DEBUG
static
PrintProtoDisplay (pdpy)
    struct protoDisplay	*pdpy;
{
    Debug ("ProtoDisplay %p\n", pdpy);
    Debug ("\taddress: ");
    PrintSockAddr (pdpy->address, pdpy->addrlen);
    Debug ("\tdate %d (%d from now)\n", pdpy->date, time(0) - pdpy->date);
    Debug ("\tdisplay Number %d\n", pdpy->displayNumber);
    Debug ("\tsessionID %d\n", pdpy->sessionID);
}
#endif

struct protoDisplay *
FindProtoDisplay (
    XdmcpNetaddr    address,
    int		    addrlen,
    CARD16	    displayNumber)
{
    struct protoDisplay	*pdpy;

    Debug ("FindProtoDisplay\n");
    for (pdpy = protoDisplays; pdpy; pdpy=pdpy->next)
    {
	if (pdpy->displayNumber == displayNumber &&
	    addressEqual (address, addrlen, pdpy->address, pdpy->addrlen))
	{
	    return pdpy;
	}
    }
    return (struct protoDisplay *) 0;
}

static void
TimeoutProtoDisplays (Time_t now)
{
    struct protoDisplay	*pdpy, *next;

    for (pdpy = protoDisplays; pdpy; pdpy = next)
    {
	next = pdpy->next;
	if (pdpy->date < (unsigned long)(now - PROTO_TIMEOUT))
	    DisposeProtoDisplay (pdpy);
    }
}

struct protoDisplay *
NewProtoDisplay (
    XdmcpNetaddr    address,
    int		    addrlen,
    CARD16	    displayNumber,
    CARD16	    connectionType,
    ARRAY8Ptr	    connectionAddress,
    CARD32	    sessionID)
{
    struct protoDisplay	*pdpy;
    Time_t date;

    Debug ("NewProtoDisplay\n");
    time (&date);
    TimeoutProtoDisplays (date);
    pdpy = (struct protoDisplay *) malloc (sizeof *pdpy);
    if (!pdpy)
	return NULL;
    pdpy->address = (XdmcpNetaddr) malloc (addrlen);
    if (!pdpy->address)
    {
	free ((char *) pdpy);
	return NULL;
    }
    pdpy->addrlen = addrlen;
    memmove( pdpy->address, address, addrlen);
    pdpy->displayNumber = displayNumber;
    pdpy->connectionType = connectionType;
    pdpy->date = date;
    if (!XdmcpCopyARRAY8 (connectionAddress, &pdpy->connectionAddress))
    {
	free ((char *) pdpy->address);
	free ((char *) pdpy);
	return NULL;
    }
    pdpy->sessionID = sessionID;
    pdpy->fileAuthorization = (Xauth *) NULL;
    pdpy->xdmcpAuthorization = (Xauth *) NULL;
    pdpy->next = protoDisplays;
    protoDisplays = pdpy;
    return pdpy;
}

void
DisposeProtoDisplay (pdpy)
    struct protoDisplay	*pdpy;
{
    struct protoDisplay	*p, *prev;

    prev = 0;
    for (p = protoDisplays; p; p=p->next)
    {
	if (p == pdpy)
	    break;
	prev = p;
    }
    if (!p)
	return;
    if (prev)
	prev->next = pdpy->next;
    else
	protoDisplays = pdpy->next;
    bzero(&pdpy->key, sizeof(pdpy->key));
    if (pdpy->fileAuthorization)
	XauDisposeAuth (pdpy->fileAuthorization);
    if (pdpy->xdmcpAuthorization)
	XauDisposeAuth (pdpy->xdmcpAuthorization);
    XdmcpDisposeARRAY8 (&pdpy->connectionAddress);
    free ((char *) pdpy->address);
    free ((char *) pdpy);
}

#endif /* XDMCP */
