/* $TOG: policy.c /main/13 1998/02/09 13:55:49 kaleb $ */
/* $Id: policy.c,v 1.15.2.3 2001/09/24 02:55:19 ossi Exp $ */
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

/*
 * xdm - display manager daemon
 * Author:  Keith Packard, MIT X Consortium
 *
 * policy.c.  Implement site-dependent policy for XDMCP connections
 */
/* $XFree86: xc/programs/xdm/policy.c,v 3.4 1998/12/06 06:08:48 dawes Exp $ */

#include "dm.h"

#ifdef XDMCP

#include "dm_auth.h"
#include "dm_socket.h"

#include <X11/X.h>

#include <errno.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

static ARRAY8 noAuthentication = { (CARD16) 0, (CARD8Ptr) 0 };

typedef struct _XdmAuth {
    ARRAY8  authentication;
    ARRAY8  authorization;
} XdmAuthRec, *XdmAuthPtr;

static XdmAuthRec auth[] = {
#ifdef HASXDMAUTH
{ {(CARD16) 20, (CARD8 *) "XDM-AUTHENTICATION-1"},
  {(CARD16) 19, (CARD8 *) "XDM-AUTHORIZATION-1"},
},
#endif
{ {(CARD16) 0, (CARD8 *) 0},
  {(CARD16) 0, (CARD8 *) 0},
}
};

#define NumAuth	as(auth)

ARRAY8Ptr
ChooseAuthentication (ARRAYofARRAY8Ptr authenticationNames)
{
    int	i, j;

    for (i = 0; i < (int)authenticationNames->length; i++)
	for (j = 0; j < NumAuth; j++)
	    if (XdmcpARRAY8Equal (&authenticationNames->data[i],
				  &auth[j].authentication))
		return &authenticationNames->data[i];
    return &noAuthentication;
}

int
CheckAuthentication (
    struct protoDisplay	*pdpy,
    ARRAY8Ptr		displayID,
    ARRAY8Ptr		name,
    ARRAY8Ptr		data)
{
#ifdef HASXDMAUTH
    if (name->length && !strncmp ((char *)name->data, "XDM-AUTHENTICATION-1", 20))
	return XdmCheckAuthentication (pdpy, displayID, name, data);
#endif
    return TRUE;
}

int
SelectAuthorizationTypeIndex (
    ARRAY8Ptr		authenticationName,
    ARRAYofARRAY8Ptr	authorizationNames)
{
    int	i, j;

    for (j = 0; j < NumAuth; j++)
	if (XdmcpARRAY8Equal (authenticationName,
			      &auth[j].authentication))
	    break;
    if (j < NumAuth)
    {
    	for (i = 0; i < (int)authorizationNames->length; i++)
	    if (XdmcpARRAY8Equal (&authorizationNames->data[i],
				  &auth[j].authorization))
	    	return i;
    }
    for (i = 0; i < (int)authorizationNames->length; i++)
	if (ValidAuthorization (authorizationNames->data[i].length,
				(char *) authorizationNames->data[i].data))
	    return i;
    return -1;
}


/*#define WILLING_INTERNAL*/

#ifdef WILLING_INTERNAL
/* Report the loadavg to chooser. Nice feature ...
 *
 * Wed Mar 10 1999 -- Steffen Hansen
 */
static void
Willing_msg (char *mbuf)
{
#ifdef linux
    int fd;
    char *fail_msg = "Willing to manage";
    char buf[1024];
    FILE *f;
    float load[3];
    float mhz = 0.0;
    int numcpu = 0;

    fd = open ("/proc/loadavg", O_RDONLY);
    if (fd == -1) {
	sprintf (mbuf, fail_msg);
	return;
    } else if (read (fd, buf, 100) < 4) {
	close (fd);
	sprintf (mbuf, fail_msg);
	return;
    }
    close (fd);

    sscanf (buf, "%f %f %f", &load[0], &load[1], &load[2]);
    sprintf (mbuf, "Available (load: %0.2f, %0.2f, %0.2f)",
	     load[0], load[1], load[2]);

    numcpu = 0;

    if (!(f = fopen ("/proc/cpuinfo", "r")))
	return;

    while (!feof (f)) {
	float m;
	fgets (buf, sizeof(buf), f);
	buf[sizeof (buf) - 1] = 0;

	if (sscanf (buf, "cpu MHz : %f", &m)) {
	    numcpu++;
	    mhz = m;
	}
    }

    fclose (f);

    if (numcpu) {
	if (numcpu > 1)
	    sprintf (buf, " %d*%0.0f MHz", numcpu, mhz);
	else
	    sprintf (buf, " %0.0f MHz", mhz);

	strncat (mbuf, buf, 256);

	mbuf[255] = 0;
    }
#elif HAVE_GETLOADAVG	/* !linux */
#ifdef __GNUC__
# warning This code is untested...
#endif
    double load[3];
    getloadavg (load, 3);
    sprintf (mbuf, "Available (load: %0.2f, %0.2f, %0.2f)", load[0],
	     load[1], load[2]);
#else	/* !linux && !GETLOADAVG */
    strcpy (mbuf, "Willing to manage");
#endif
}
#endif

/*ARGSUSED*/
int
Willing (
    ARRAY8Ptr	    addr,
    CARD16	    connectionType,
    ARRAY8Ptr	    authenticationName ATTR_UNUSED,
    ARRAY8Ptr	    status,
    xdmOpCode	    type)
{
    char	statusBuf[256];
    int		ret;
    
    ret = AcceptableDisplayAddress (addr, connectionType, type);
    if (!ret)
	sprintf (statusBuf, "Display not authorized to connect");
    else
    {
        if (*willing)
	{
	    FILE *fd;
	    int ok = 0;
	    if ((fd = popen(willing, "r"))) {
		for (;;) {
		    if (fgets(statusBuf, sizeof(statusBuf), fd)) {
			int len = strlen(statusBuf);
			if (len && statusBuf[len - 1] == '\n')
			    statusBuf[--len] = 0; /* chop newline */
			if (len) {
			    ok = 1;
			    break;
			}
		    }
		    if (feof (fd) || errno != EINTR)
			break;
		}
		pclose(fd);
	    }
	    if (!ok)
		sprintf (statusBuf, "Willing, but %s failed", willing);
	}
	else
#ifdef WILLING_INTERNAL
	    Willing_msg (statusBuf);
#else
	    strcpy (statusBuf, "Willing to manage");
#endif
    }
    status->length = strlen (statusBuf);
    status->data = (CARD8Ptr) malloc (status->length);
    if (!status->data)
	status->length = 0;
    else
	memmove( status->data, statusBuf, status->length);
    return ret;
}

/*ARGSUSED*/
ARRAY8Ptr
Accept (
    struct sockaddr *from ATTR_UNUSED,
    int		    fromlen ATTR_UNUSED,
    CARD16	    displayNumber ATTR_UNUSED)
{
    return 0;
}

/*ARGSUSED*/
int
SelectConnectionTypeIndex (
    ARRAY16Ptr	     connectionTypes ATTR_UNUSED,
    ARRAYofARRAY8Ptr connectionAddresses ATTR_UNUSED)
{
    return 0;
}

#endif /* XDMCP */
