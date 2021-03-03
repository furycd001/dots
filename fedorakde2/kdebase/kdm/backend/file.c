/* $TOG: file.c /main/18 1998/02/09 13:55:19 kaleb $ */
/* $Id: file.c,v 1.10 2001/07/30 00:54:23 ossi Exp $ */
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
/* $XFree86: xc/programs/xdm/file.c,v 1.4 1998/10/10 15:25:34 dawes Exp $ */

/*
 * xdm - display manager daemon
 * Author:  Keith Packard, MIT X Consortium
 *
 * file.c
 */

#include "dm.h"
#include "dm_error.h"

#include <ctype.h>

int parseDisplayType (char *string, int *usedDefault, char **atPos);

static char **
copyArgs (char **args)
{
    char    **a, **new, **n;

    for (a = args; *a; a++)
	/* SUPPRESS 530 */
	;
    new = (char **) malloc ((a - args + 1) * sizeof (char *));
    if (!new)
	return NULL;
    n = new;
    a = args;
    /* SUPPRESS 560 */
    while ((*n++ = *a++))
	/* SUPPRESS 530 */
	;
    return new;
}

static void
freeSomeArgs (char **args, int n)
{
    char    **a;

    a = args;
    while (n--)
	free (*a++);
    free ((char *) args);
}

void
ParseDisplay (char *src)
{
    char		**args, **argv, *dtx, *atPos;
    char		*name, *class2, *type;
    struct display	*d;
    int			usedDefault;
    int			displayType;

    if (!(args = parseArgs (0, src)))
	return;
    if (!args[0])
    {
	freeStrArr (args);
	return;
    }
    name = args[0];
    if (!args[1])
    {
	LogError ("Missing display type for %s\n", args[0]);
	freeStrArr (args);
	return;
    }
    displayType = parseDisplayType (args[1], &usedDefault, &atPos);
    class2 = NULL;
    type = args[1];
    argv = args + 2;
    /*
     * extended syntax; if the second argument doesn't
     * exactly match a legal display type and the third
     * argument does, use the second argument as the
     * display class string
     */
    if (usedDefault && args[2])
    {
	displayType = parseDisplayType (args[2], &usedDefault, &atPos);
	if (!usedDefault)
	{
	    class2 = args[1];
	    type = args[2];
	    argv = args + 3;
	}
    }
    if ((d = FindDisplayByName (name)))
    {
	d->stillThere = 1;
	ReStr (&d->class2, class2);
	ReStr (&d->console, atPos);
	freeStrArr (d->serverArgv);
	dtx = "existing";
    }
    else
    {
	d = NewDisplay (name, class2);
	StrDup (&d->console, atPos);
	dtx = "new";
    }
    Debug ("Found %s display: %s %s %s %[s\n",
	   dtx, d->name, d->class2 ? d->class2 : "", type, argv);
    d->hstent->startTries = 0;
    d->displayType = displayType;
    d->serverArgv = copyArgs (argv);
    freeSomeArgs (args, argv - args);
}

static struct displayMatch {
	char		*name;
	int		len;
	int		type;
} displayTypes[] = {
	{ "local", 5,		Local | Permanent | FromFile },
	{ "foreign", 7,		Foreign | Permanent | FromFile },
	{ 0, 0,			Local | Permanent | FromFile },
};

int
parseDisplayType (char *string, int *usedDefault, char **atPos)
{
    struct displayMatch *d;

    *atPos = 0;
    for (d = displayTypes; d->name; d++) {
	if (!memcmp (d->name, string, d->len) &&
	    (!string[d->len] || string[d->len] == '@')) {
	    if (string[d->len] == '@')
		*atPos = string + d->len + 1;
	    *usedDefault = 0;
	    return d->type;
	}
    }
    *usedDefault = 1;
    return d->type;
}
