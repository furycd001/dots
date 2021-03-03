/* $TOG: dpylist.c /main/30 1998/02/09 13:55:07 kaleb $ */
/* $Id: dpylist.c,v 1.11.2.1 2001/09/24 02:55:19 ossi Exp $ */
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
/* $XFree86: xc/programs/xdm/dpylist.c,v 1.3 2000/04/27 16:26:50 eich Exp $ */

/*
 * xdm - display manager daemon
 * Author:  Keith Packard, MIT X Consortium
 *
 * a simple linked list of known displays
 */

#include "dm.h"
#include "dm_error.h"

static struct display	*displays;
static struct disphist	*disphist;

int
AnyDisplaysLeft (void)
{
    return displays != (struct display *) 0;
}

void
ForEachDisplay (void (*f)(struct display *))
{
    struct display *d, *next;

    for (d = displays; d; d = next) {
	next = d->next;
	(*f) (d);
    }
}

struct display *
FindDisplayByName (char *name)
{
    struct display *d;

    for (d = displays; d; d = d->next)
	if (!strcmp (name, d->name))
	    return d;
    return 0;
}

struct display *
FindDisplayByPid (int pid)
{
    struct display *d;

    for (d = displays; d; d = d->next)
	if (pid == d->pid)
	    return d;
    return 0;
}

struct display *
FindDisplayByServerPid (int serverPid)
{
    struct display *d;

    for (d = displays; d; d = d->next)
	if (serverPid == d->serverPid)
	    return d;
    return 0;
}

#ifdef XDMCP

struct display *
FindDisplayBySessionID (CARD32 sessionID)
{
    struct display	*d;

    for (d = displays; d; d = d->next)
	if (sessionID == d->sessionID)
	    return d;
    return 0;
}

struct display *
FindDisplayByAddress (XdmcpNetaddr addr, int addrlen, CARD16 displayNumber)
{
    struct display  *d;

    for (d = displays; d; d = d->next)
	if ((d->displayType & d_origin) == FromXDMCP &&
	    d->displayNumber == displayNumber &&
	    addressEqual ((XdmcpNetaddr)d->from.data, d->from.length, 
			  addr, addrlen))
	    return d;
    return 0;
}

#endif /* XDMCP */

#define IfFree(x)  if (x) free ((char *) x)
    
void
RemoveDisplay (struct display *old)
{
    struct display	*d, **dp;
    int			i;

    for (dp = &displays; (d = *dp); dp = &(*dp)->next) {
	if (d == old) {
	    *dp = d->next;
	    IfFree (d->class2);
	    IfFree (d->cfg.data);
	    delStr (d->cfg.dep.name);
	    freeStrArr (d->serverArgv);
	    if (d->authorizations)
	    {
		for (i = 0; i < d->authNum; i++)
		    XauDisposeAuth (d->authorizations[i]);
		free ((char *) d->authorizations);
	    }
	    if (d->authFile) {
		(void) unlink (d->authFile);
		free (d->authFile);
	    }
	    IfFree (d->authNameLens);
#ifdef XDMCP
	    XdmcpDisposeARRAY8 (&d->peer);
	    XdmcpDisposeARRAY8 (&d->from);
	    XdmcpDisposeARRAY8 (&d->clientAddr);
#endif
	    free ((char *) d);
	    break;
	}
    }
}

static struct disphist *
FindHist (char *name)
{
    struct disphist *hstent;

    for (hstent = disphist; hstent; hstent = hstent->next)
	if (!strcmp (hstent->name, name))
	    return hstent;
    return 0;
}

struct display *
NewDisplay (char *name, char *class2)
{
    struct display	*d;
    struct disphist	*hstent;

    if (!(hstent = FindHist (name))) {
	if (!(hstent = calloc (1, sizeof (*hstent)))) {
	    LogOutOfMem ("NewDisplay");
	    return 0;
	}
	if (!StrDup (&hstent->name, name)) {
	    free (hstent);
	    LogOutOfMem ("NewDisplay");
	    return 0;
	}
	hstent->next = disphist; disphist = hstent;
    }

    if (!(d = (struct display *) calloc (1, sizeof (*d)))) {
	LogOutOfMem ("NewDisplay");
	return 0;
    }
    d->next = displays;
    d->hstent = hstent;
    d->name = hstent->name;
    if (!StrDup (&d->class2, class2)) {
	LogOutOfMem ("NewDisplay");
	free ((char *) d);
	return 0;
    }
    /* initialize every field to avoid possible problems (others are 0) */
    d->status = notRunning;
    d->pid = -1;
    d->serverPid = -1;
    d->stillThere = 1;
    d->autoLogin1st = 1;
    d->fifoOwner = -1;
    d->fifoGroup = -1;
    d->fifofd = -1;
    d->pipefd[0] = -1;
    d->pipefd[1] = -1;
    displays = d;
    return d;
}
