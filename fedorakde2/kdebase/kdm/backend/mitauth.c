/* $TOG: mitauth.c /main/13 1998/02/09 13:55:37 kaleb $ */
/* $Id: mitauth.c,v 1.5 2001/03/26 21:26:15 ossi Exp $ */
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
/* $XFree86: xc/programs/xdm/mitauth.c,v 1.2 1998/10/10 15:25:36 dawes Exp $ */

/*
 * xdm - display manager daemon
 * Author:  Keith Packard, MIT X Consortium
 *
 * mitauth
 *
 * generate authorization keys
 * for MIT-MAGIC-COOKIE-1 type authorization
 */

#include "dm.h"
#include "dm_auth.h"

#include <X11/Xos.h>

#define AUTH_DATA_LEN	16	/* bytes of authorization data */
static char	auth_name[256];
static int	auth_name_len;

void
MitInitAuth (unsigned short name_len, char *name)
{
    if (name_len > 256)
	name_len = 256;
    auth_name_len = name_len;
    memmove( auth_name, name, name_len);
}

Xauth *
MitGetAuth (unsigned short namelen, char *name)
{
    Xauth   *new;
    new = (Xauth *) malloc (sizeof (Xauth));

    if (!new)
	return (Xauth *) 0;
    new->family = FamilyWild;
    new->address_length = 0;
    new->address = 0;
    new->number_length = 0;
    new->number = 0;

    new->data = (char *) malloc (AUTH_DATA_LEN);
    if (!new->data)
    {
	free ((char *) new);
	return (Xauth *) 0;
    }
    new->name = (char *) malloc (namelen);
    if (!new->name)
    {
	free ((char *) new->data);
	free ((char *) new);
	return (Xauth *) 0;
    }
    memmove( (char *)new->name, name, namelen);
    new->name_length = namelen;
    GenerateAuthData (new->data, AUTH_DATA_LEN);
    new->data_length = AUTH_DATA_LEN;
    return new;
}
