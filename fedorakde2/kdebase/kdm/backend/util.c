/* $TOG: util.c /main/19 1998/02/09 13:56:40 kaleb $ */
/* $Id: util.c,v 1.18.2.2 2001/10/03 10:47:11 ossi Exp $ */
/*

Copyright 1989, 1998  The Open Group

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
/* $XFree86: xc/programs/xdm/util.c,v 3.14 2000/08/10 17:40:41 dawes Exp $ */

/*
 * xdm - display manager daemon
 * Author:  Keith Packard, MIT X Consortium
 *
 * util.c
 *
 * various utility routines
 */

#include "dm.h"
#include "dm_error.h"

#include <X11/Xosdefs.h>
#ifndef X_NOT_STDC_ENV
# include <string.h>
# include <unistd.h>
#endif

#include <errno.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

#ifdef USG
# define NEED_UTSNAME
#endif

#ifdef NEED_UTSNAME
# include <sys/utsname.h>
#endif


void
WipeStr (char *str)
{
    if (str) {
	bzero (str, strlen (str));
	free (str);
    }
}

/* duplicate src; wipe & free old dst string */
int
ReStrN (char **dst, char *src, int len)
{
    char *ndst = 0;

    if (src) {
	if (len < 0)
	    len = strlen (src);
	if (*dst && !memcmp (*dst, src, len) && !(*dst)[len])
	    return 1;
	if (!(ndst = malloc (len + 1)))
	    return 0;
	memcpy (ndst, src, len);
	ndst[len] = 0;
    }
    WipeStr (*dst);	/* make an option, if we should become heavily used */
    *dst = ndst;
    return 1;
}

int
ReStr (char **dst, char *src)
{
    return ReStrN (dst, src, -1);
}

/* duplicate src */
int
StrNDup (char **dst, char *src, int len)
{
    if (src) {
	if (len < 0)
	    len = strlen (src);
	if (!(*dst = malloc (len + 1)))
	    return 0;
	memcpy (*dst, src, len);
	(*dst)[len] = 0;
    } else
	*dst = 0;
    return 1;
}

int
StrDup (char **dst, char *src)
{
    return StrNDup (dst, src, -1);
}

/* append any number of strings to dst */
int
StrApp (char **dst, ...)
{
    int len;
    char *bk, *pt, *dp;
    va_list va;
    
    len = 1;
    if (*dst)
	len += strlen(*dst);
    va_start (va, dst);
    for (;;) {
	pt = va_arg (va, char *);
	if (!pt)
	    break;
	len += strlen (pt);
    }
    va_end (va);
    if (!(bk = malloc (len)))
	return 0;
    dp = bk;
    if (*dst) {
	len = strlen(*dst);
	memcpy (dp, *dst, len);
	dp += len;
	free(*dst);
    }
    va_start (va, dst);
    for (;;) {
	pt = va_arg (va, char *);
	if (!pt)
	    break;
	len = strlen(pt);
	memcpy (dp, pt, len);
	dp += len;
    }
    va_end (va);
    *dp = '\0';
    *dst = bk;
    return 1;
}


char **
initStrArr (char **arr)
{
    if (!arr)
	if ((arr = malloc (sizeof(char *))))
	    arr[0] = 0;
    return arr;
}

static int
arrLen (char **arr)
{
    int nu = 0;
    if (arr)
	for (; arr[nu]; nu++);
    return nu;
}

char **
extStrArr (char ***arr)
{
    char **rarr;
    int nu;

    nu = arrLen (*arr);
    if ((rarr = realloc (*arr, sizeof (char *) * (nu + 2)))) {
	*arr = rarr;
	rarr[nu + 1] = 0;
	return rarr + nu;
    }
    return 0;
}

char **
addStrArr (char **arr, char *str, int len)
{
    char **dst;

    if ((dst = extStrArr (&arr)))
	(void) StrNDup (dst, str, len);
    return arr;    
}

char **
xCopyStrArr (int rn, char **arr)
{
    char **rarr;
    int nnu, nu, i;

    nu = arrLen (arr);
    nnu = nu + rn;
    if ((rarr = calloc (sizeof (char *), nnu + 1)))
	for (i = 0; i < nu; i++)
	    rarr[rn + i] = arr[i];
    return rarr;
}

void
mergeStrArrs (char ***darr, char **arr)
{
    char **rarr;
    int nu, i;

    nu = arrLen (*darr);
    if ((rarr = xCopyStrArr (nu, arr))) {
	for (i = 0; i < nu; i++)
	    rarr[i] = (*darr)[i];
	free (*darr);
	*darr = rarr;
    }
}

void
freeStrArr (char **arr)
{
    char **a;

    if (arr) {
    	for (a = arr; *a; a++)
	    free (*a);
    	free (arr);
    }
}


char **
parseArgs (char **argv, char *string)
{
    char *word, ch;

    argv = initStrArr (argv);
    for (word = string; ; ++string) {
	ch = *string;
	if (!ch || ch == '\n' || ch == '#' || ch == ' ' || ch == '\t') {
	    if (word != string)
		argv = addStrArr (argv, word, string - word);
	    if (!ch || ch == '\n' || ch == '#')
		return argv;
	    word = string + 1;
	}
    }
}


static char *
makeEnv (char *name, char *value)
{
    char *result;

    result = malloc ((unsigned) (strlen (name) + strlen (value) + 2));
    if (!result) {
	LogOutOfMem ("makeEnv");
	return 0;
    }
#ifdef AIXV3
    /* setpenv() depends on "SYSENVIRON:", not "SYSENVIRON:=" */
    if (!(value && *value))
	sprintf (result, "%s", name);
    else
#endif
	sprintf (result, "%s=%s", name, value);
    return result;
}

char *
getEnv (char **e, char *name)
{
    int l = strlen (name);

    if (!e)
	return 0;

    while (*e) {
	if ((int) strlen (*e) > l && !strncmp (*e, name, l) &&
	    (*e)[l] == '=')
	    return (*e) + l + 1;
	++e;
    }
    return 0;
}

char **
setEnv (char **e, char *name, char *value)
{
    char **new, **old;
    char *newe;
    int envsize;
    int l;

    l = strlen (name);
    newe = makeEnv (name, value);
    if (!newe) {
	LogOutOfMem ("setEnv");
	return e;
    }
    if (e) {
	for (old = e; *old; old++)
	    if ((int) strlen (*old) > l && !strncmp (*old, name, l)
		&& (*old)[l] == '=')
		break;
	if (*old) {
	    free (*old);
	    *old = newe;
	    return e;
	}
	envsize = old - e;
	new = (char **) realloc ((char *) e,
				 (unsigned) ((envsize + 2) * 
					     sizeof (char *)));
    } else {
	envsize = 0;
	new = (char **) malloc (2 * sizeof (char *));
    }
    if (!new) {
	LogOutOfMem ("setEnv");
	free (newe);
	return e;
    }
    new[envsize] = newe;
    new[envsize + 1] = 0;
    return new;
}

char **
putEnv(char *string, char **env)
{
    char *v, *b, *n;
    int nl;
  
    if ((b = strchr(string, '=')) == NULL)
	return NULL;
    v = b + 1;
  
    nl = b - string;
    if ((n = malloc(nl + 1)) == NULL)
    {
	LogOutOfMem ("putEnv");
	return NULL;
    }
  
    memcpy(n, string, nl);
    n[nl] = 0;
  
    env = setEnv(env, n, v);
    free(n);
    return env;
}

static int
GetHostname(char *buf, int maxlen)
{
    int len;

#ifdef NEED_UTSNAME
    /*
     * same host name crock as in server and xinit.
     */
    struct utsname name;

    uname (&name);
    len = strlen (name.nodename);
    if (len >= maxlen) len = maxlen - 1;
    memcpy (buf, name.nodename, len);
    buf[len] = '\0';
#else
    buf[0] = '\0';
    (void) gethostname (buf, maxlen);
    buf [maxlen - 1] = '\0';
    len = strlen(buf);
#endif /* hpux */
    return len;
}

static char localHostbuf[256];
static int  gotLocalHostname;

char *
localHostname (void)
{
    if (!gotLocalHostname)
    {
	GetHostname (localHostbuf, sizeof (localHostbuf) - 1);
	gotLocalHostname = 1;
    }
    return localHostbuf;
}

int
Reader (int fd, void *buf, int count)
{
    int ret, rlen;

    for (rlen = 0; rlen < count; ) {
      dord:
	ret = read (fd, (void *)((char *)buf + rlen), count - rlen);
	if (ret < 0) {
	    if (errno == EINTR)
		goto dord;
	    if (errno == EAGAIN)
		break;
	    return -1;
	}
	if (!ret)
	    break;
	rlen += ret;
    }
    return rlen;
}

void
FdGetsCall (int fd, void (*func)(char *, int, void *), void *ptr)
{
    char	*p;
    int		bpos, bend, llen, ll, rt, ign;
    char	buf[200];

    for (bpos = bend = ign = 0;;) {
	for (;;) {
	    if ((p = memchr(buf + bpos, '\n', bend - bpos)) != 0) {
		llen = (ll = (p - buf) - bpos) + 1;
		break;
	    }
	    if (bpos == 0 && bend == sizeof (buf)) {
		ign = 1;
		bend = 0;
	    } else {
		memcpy (buf, buf + bpos, bend - bpos);
		bend -= bpos;
	    }
	    bpos = 0;
	    rt = Reader (fd, buf + bend, sizeof (buf) - bend);
	    if (rt < 0) {
		bzero (buf, sizeof(buf));
		return;
	    }
	    if (!rt) {
		if (bend) {
		    llen = ll = bend;
		    break;
		} else {
		    bzero (buf, sizeof(buf));
		    return;
		}
	    }
	    bend += rt;
	}
	if (ign)
	    ign = 0;
	else
	    func (buf + bpos, ll, ptr);
	bpos += llen;
    }
}

