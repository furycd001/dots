/*
 * $TOG: choose.c /main/18 1998/02/09 13:54:39 kaleb $
 * $Id: choose.c,v 1.11.2.1 2001/09/20 13:27:55 ossi Exp $
 *
Copyright 1990, 1998  The Open Group

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

/* $XFree86: xc/programs/xdm/choose.c,v 3.9 2000/05/31 07:15:11 eich Exp $ */

/*
 * choose.c
 *
 * xdm interface to chooser program
 */

#include "dm.h"

#ifdef XDMCP

#include "dm_error.h"
#include "dm_socket.h"

#include <X11/X.h>

#ifndef MINIX
# ifndef X_NO_SYS_UN
#  ifndef Lynx
#   include <sys/un.h>
#  else
#   include <un.h>
#  endif
# endif
#else /* MINIX */
# include <sys/ioctl.h>
# include <net/netlib.h>
# include <net/gen/in.h>
# include <net/gen/tcp.h>
# include <net/gen/tcp_io.h>
#endif /* !MINIX */

#include <ctype.h>
#include <errno.h>

#if defined(STREAMSCONN)
# include <tiuser.h>
#endif

#ifdef X_NOT_STDC_ENV
extern int errno;
# define Time_t long
extern Time_t time ();
#else
# include <time.h>
# define Time_t time_t
#endif

#ifdef MINIX
int listen_inprogress;
int listen_completed;
#endif

/* XXX use SNPrintf for this? */
static int
FormatBytes (
    unsigned char *data,
    int	    length,
    char    *buf,
    int	    buflen)
{
    int	    i;
    static char	    HexChars[] = "0123456789abcdef";

    if (buflen < length * 2 + 1)
	return 0;
    for (i = 0; i < length; i++)
    {
	*buf++ = HexChars[(data[i] >> 4) & 0xf];
	*buf++ = HexChars[(data[i]) & 0xf];
    }
    *buf++ = '\0';
    return 1;
}

static int
FormatARRAY8 (
    ARRAY8Ptr	a,
    char	*buf,
    int		buflen)
{
    return FormatBytes (a->data, a->length, buf, buflen);
}

/* Converts an Internet address in ARRAY8 format to a string in
   familiar dotted address notation, e.g., "18.24.0.11"
   Returns 1 if successful, 0 if not.
   */
static int
ARRAY8ToDottedDecimal (
    ARRAY8Ptr	a,
    char	*buf,
    int		buflen)
{
    if (a->length != 4  ||  buflen < 20)
	return 0;
    sprintf(buf, "%d.%d.%d.%d",
	    a->data[0], a->data[1], a->data[2], a->data[3]);
    return 1;
}

typedef struct _IndirectUsers {
    struct _IndirectUsers   *next;
    ARRAY8	client;
    CARD16	connectionType;
} IndirectUsersRec, *IndirectUsersPtr;

static IndirectUsersPtr	indirectUsers;

int
RememberIndirectClient (
    ARRAY8Ptr	clientAddress,
    CARD16	connectionType)
{
    IndirectUsersPtr	i;

    for (i = indirectUsers; i; i = i->next)
	if (XdmcpARRAY8Equal (clientAddress, &i->client) &&
	    connectionType == i->connectionType)
	    return 1;
    i = (IndirectUsersPtr) malloc (sizeof (IndirectUsersRec));
    if (!XdmcpCopyARRAY8 (clientAddress, &i->client))
    {
	free ((char *) i);
	return 0;
    }
    i->connectionType = connectionType;
    i->next = indirectUsers;
    indirectUsers = i;
    return 1;
}

void
ForgetIndirectClient (
    ARRAY8Ptr	clientAddress,
    CARD16	connectionType)
{
    IndirectUsersPtr	i, prev;

    prev = 0;
    for (i = indirectUsers; i; i = i->next)
    {
	if (XdmcpARRAY8Equal (clientAddress, &i->client) &&
	    connectionType == i->connectionType)
	{
	    if (prev)
		prev->next = i->next;
	    else
		indirectUsers = i->next;
	    XdmcpDisposeARRAY8 (&i->client);
	    free ((char *) i);
	    break;
	}
	prev = i;
    }
}

int
IsIndirectClient (
    ARRAY8Ptr	clientAddress,
    CARD16	connectionType)
{
    IndirectUsersPtr	i;

    for (i = indirectUsers; i; i = i->next)
	if (XdmcpARRAY8Equal (clientAddress, &i->client) &&
	    connectionType == i->connectionType)
	    return 1;
    return 0;
}

static int
FormatChooserArgument (char *buf, int len)
{
    unsigned char   addr_buf[1024];
    int		    addr_len = sizeof (addr_buf);
    unsigned char   result_buf[1024];
    int		    result_len = 0;
    int		    netfamily;

    if (GetChooserAddr ((char *)addr_buf, &addr_len) == -1)
    {
	LogError ("Cannot get return address for chooser socket\n");
	Debug ("Cannot get chooser socket address\n");
	return 0;
    }
    netfamily = NetaddrFamily((XdmcpNetaddr)addr_buf);
    switch (netfamily) {
    case AF_INET:
	{
	    char *port;
	    int portlen;
	    ARRAY8Ptr localAddress;

	    port = NetaddrPort((XdmcpNetaddr)addr_buf, &portlen);
	    result_buf[0] = netfamily >> 8;
	    result_buf[1] = netfamily & 0xFF;
	    result_buf[2] = port[0];
	    result_buf[3] = port[1];
	    localAddress = getLocalAddress ();
	    memmove( (char *)result_buf+4, (char *)localAddress->data, 4);
	    result_len = 8;
	}
	break;
#ifdef AF_DECnet
    case AF_DECnet:
	break;
#endif
    default:
	Debug ("Chooser family %d isn't known\n", netfamily);
	return 0;
    }

    return FormatBytes (result_buf, result_len, buf, len);
}

typedef struct _Choices {
    struct _Choices *next;
    ARRAY8	    client;
    CARD16	    connectionType;
    ARRAY8	    choice;
    Time_t	    time;
} ChoiceRec, *ChoicePtr;

static ChoicePtr   choices;

ARRAY8Ptr
IndirectChoice (
    ARRAY8Ptr	clientAddress,
    CARD16	connectionType)
{
    ChoicePtr	c, next, prev;
    Time_t	now;

    now = time ((Time_t*)0);
    prev = 0;
    for (c = choices; c; c = next)
    {
	next = c->next;
	Debug ("Choice checking timeout: %ld >? %d\n",
	    (long)(now - c->time), choiceTimeout);
	if (now - c->time > (Time_t)choiceTimeout)
	{
	    Debug ("Timeout choice %ld > %d\n",
		(long)(now - c->time), choiceTimeout);
	    if (prev)
		prev->next = next;
	    else
		choices = next;
	    XdmcpDisposeARRAY8 (&c->client);
	    XdmcpDisposeARRAY8 (&c->choice);
	    free ((char *) c);
	}
	else
	{
	    if (XdmcpARRAY8Equal (clientAddress, &c->client) &&
	    	connectionType == c->connectionType)
	    	return &c->choice;
	    prev = c;
	}
    }
    return 0;
}

static int
RegisterIndirectChoice (
    ARRAY8Ptr	clientAddress,
    CARD16      connectionType,
    ARRAY8Ptr	choice)
{
    ChoicePtr	c;
    int		insert;
    int		found = 0;

    Debug ("Got indirect choice back\n");
    for (c = choices; c; c = c->next) {
	if (XdmcpARRAY8Equal (clientAddress, &c->client) &&
	    connectionType == c->connectionType) {
	    found = 1;
	    break;
	}
    }
#if 0
    if (!found)
	return 0;
#endif

    insert = 0;
    if (!c)
    {
	insert = 1;
	c = (ChoicePtr) malloc (sizeof (ChoiceRec));
	if (!c)
	    return 0;
	c->connectionType = connectionType;
    	if (!XdmcpCopyARRAY8 (clientAddress, &c->client))
    	{
	    free ((char *) c);
	    return 0;
    	}
    }
    else
    {
	XdmcpDisposeARRAY8 (&c->choice);
    }
    if (!XdmcpCopyARRAY8 (choice, &c->choice))
    {
	XdmcpDisposeARRAY8 (&c->client);
	free ((char *) c);
	return 0;
    }
    if (insert)
    {
	c->next = choices;
	choices = c;
    }
    c->time = time (0);
    return 1;
}

#ifdef notdef
static
RemoveIndirectChoice (
    ARRAY8Ptr	clientAddress,
    CARD16	connectionType)
{
    ChoicePtr	c, prev;

    prev = 0;
    for (c = choices; c; c = c->next)
    {
	if (XdmcpARRAY8Equal (clientAddress, &c->client) &&
	    connectionType == c->connectionType)
	{
	    if (prev)
		prev->next = c->next;
	    else
		choices = c->next;
	    XdmcpDisposeARRAY8 (&c->client);
	    XdmcpDisposeARRAY8 (&c->choice);
	    free ((char *) c);
	    return;
	}
	prev = c;
    }
}
#endif

/*ARGSUSED*/
static void
AddChooserHost (
    CARD16	connectionType ATTR_UNUSED,
    ARRAY8Ptr	addr,
    char	*closure)
{
    char	***argp;
    char	hostbuf[1024];

    argp = (char ***) closure;
    if (addr->length == 9 &&
	!memcmp ((char *)addr->data, "BROADCAST", 9))
    {
	*argp = addStrArr (*argp, "BROADCAST", 9);
    }
    else if (ARRAY8ToDottedDecimal (addr, hostbuf, sizeof (hostbuf)))
    {
	*argp = addStrArr (*argp, hostbuf, -1);
    }
}

void
ProcessChooserSocket (int fd)
{
    int		client_fd;
    char	buf[1024];
    int		len;
    XdmcpBuffer	buffer;
    ARRAY8	clientAddress;
    CARD16	connectionType;
    ARRAY8	choice;
#if defined(STREAMSCONN)
    struct t_call *call;
    int		flags = 0;
#endif
#ifdef MINIX
    nwio_tcpconf_t tcpconf;
    nwio_tcpcl_t tcpcl;
    char *tcp_device;
    int new_fd, flags, r;
#endif /* MINIX */

    Debug ("Process chooser socket\n");
    len = sizeof (buf);
#if defined(STREAMSCONN)
    call = (struct t_call *)t_alloc( fd, T_CALL, T_ALL );
    if( call == NULL )
    {
	t_error( "ProcessChooserSocket: t_alloc failed" );
	LogError ("Cannot setup to listen on chooser connection\n");
	return;
    }
    if( t_listen( fd, call ) < 0 )
    {
	t_error( "ProcessChooserSocket: t_listen failed" );
	t_free( (char *)call, T_CALL );
	LogError ("Cannot listen on chooser connection\n");
	return;
    }
    client_fd = t_open ("/dev/tcp", O_RDWR, NULL);
    if (client_fd == -1)
    {
	t_error( "ProcessChooserSocket: t_open failed" );
	t_free( (char *)call, T_CALL );
	LogError ("Cannot open new chooser connection\n");
	return;
    }
    if( t_bind( client_fd, NULL, NULL ) < 0 )
    {
	t_error( "ProcessChooserSocket: t_bind failed" );
	t_free( (char *)call, T_CALL );
	LogError ("Cannot bind new chooser connection\n");
        t_close (client_fd);
	return;
    }
    if( t_accept (fd, client_fd, call) < 0 )
    {
	t_error( "ProcessChooserSocket: t_accept failed" );
	LogError ("Cannot accept chooser connection\n");
	t_free( (char *)call, T_CALL );
        t_unbind (client_fd);
        t_close (client_fd);
	return;
    }
#else
#ifdef MINIX
    if (listen_inprogress) abort();
    /* If the listen succeeded save the filedescriptor */
    if (listen_completed)
    {
    	client_fd = dup(fd);
    	if (client_fd == -1)
    	{
		LogError ("Dup failed: %s\n", strerror(errno));
		return;
    	}
    }
    else
    	client_fd = -1;

    /* Try to setup a new tcp device at the same filedescriptor as the old
     * one.
     */
    if (ioctl(fd, NWIOGTCPCONF, &tcpconf) == -1)
    {
	LogError ("NWIOGTCPCONF failed: %s\n", strerror(errno));
	return;
    }
    close(fd);
    tcp_device = getenv("TCP_DEVICE");
    if (tcp_device == NULL)
    	tcp_device = TCP_DEVICE;
    new_fd = open(tcp_device, O_RDWR);
    if (new_fd == -1)
    {
	LogError ("open '%s' failed: %s\n", tcp_device, strerror(errno));
	return;
    }
    if (new_fd != fd)
    {
    	dup2(new_fd, fd);
    	close(new_fd);
    }
    if ((flags = fcntl(fd, F_GETFD)) == -1)
    {
	LogError ("F_GETFD failed: %s\n", strerror(errno));
	return;
    }
    if (fcntl(fd, F_SETFD, flags | FD_ASYNCHIO) == -1)
    {
	LogError ("F_SETFD failed: %s\n", strerror(errno));
	return;
    }
    tcpconf.nwtc_flags = NWTC_EXCL | NWTC_LP_SET | NWTC_UNSET_RA | NWTC_UNSET_RP;
    if (ioctl(fd, NWIOSTCPCONF, &tcpconf) == -1)
    {
	LogError ("NWIOSTCPCONF failed: %s\n", strerror(errno));
	return;
    }
    listen_inprogress = 0;
    listen_completed = 0;

    tcpcl.nwtcl_flags = 0;
    r = ioctl(fd, NWIOTCPLISTEN, &tcpcl);
    if (r == -1 && errno == EINPROGRESS)
    {
    	listen_inprogress = 1;
    	nbio_inprogress(fd, ASIO_IOCTL, 1 /* read */, 1 /* write */,
    		0 /* except */);
    }
    else if (r == -1)
    {
	LogError ("NWIOTCPLISTEN failed: %s\n", strerror(errno));
	return;
    }
    else
    	listen_completed = 1;
    if (client_fd == -1)
    	return;
#else /* !MINIX */
    client_fd = accept (fd, (struct sockaddr *)buf, (void *)&len);
#endif /* MINIX */
    if (client_fd == -1)
    {
	LogError ("Cannot accept chooser connection\n");
	return;
    }
#endif
    Debug ("Accepted %d\n", client_fd);
    
#if defined(STREAMSCONN)
    len = t_rcv (client_fd, buf, sizeof (buf), &flags);
#else
    len = read (client_fd, buf, sizeof (buf));
#endif
    Debug ("Read returns %d\n", len);
    if (len > 0)
    {
    	buffer.data = (BYTE *) buf;
    	buffer.size = sizeof (buf);
    	buffer.count = len;
    	buffer.pointer = 0;
	clientAddress.data = 0;
	clientAddress.length = 0;
	choice.data = 0;
	choice.length = 0;
	if (XdmcpReadARRAY8 (&buffer, &clientAddress)) {
	    if (XdmcpReadCARD16 (&buffer, &connectionType)) {
		if (XdmcpReadARRAY8 (&buffer, &choice)) {
		    Debug ("Read from chooser succesfully\n");
		    RegisterIndirectChoice (&clientAddress, connectionType, &choice);
		    XdmcpDisposeARRAY8 (&choice);
		} else {
		    LogError ("Invalid choice response length %d\n", len);
		}
	    } else {
		LogError ("Invalid choice response length %d\n", len);
	    }
	    XdmcpDisposeARRAY8 (&clientAddress);
	} else {
	    LogError ("Invalid choice response length %d\n", len);
	}
    }
    else
    {
	LogError ("Choice response read error: %s\n", strerror(errno));
    }

#if defined(STREAMSCONN)
    t_unbind (client_fd);
    t_free( (char *)call, T_CALL );
    t_close (client_fd);
#else
    close (client_fd);
#endif
}

void
RunChooser (struct display *d)
{
    char **args;
    char buf[1024];
    char **env;

    Debug ("RunChooser %s\n", d->name);
    SetTitle (d->name, "chooser", (char *) 0);
    LoadXloginResources (d);
    args = parseArgs ((char **) 0, d->chooser);
    strcpy (buf, "-xdmaddress ");
    if (FormatChooserArgument (buf + strlen (buf), sizeof (buf) - strlen (buf)))
	args = parseArgs (args, buf);
    if (d->useChooser)
    {
	strcpy (buf, "-clientaddress ");
	if (FormatARRAY8 (&d->clientAddr, buf + strlen (buf), sizeof (buf) - strlen (buf)))
	    args = parseArgs (args, buf);
	sprintf (buf, "-connectionType %d", d->connectionType);
	args = parseArgs (args, buf);
	ForEachChooserHost (&d->clientAddr, d->connectionType, AddChooserHost,
			    (char *) &args);
    }
    else
	args = addStrArr (args, "BROADCAST", 9);
    env = systemEnv (d, (char *) 0, (char *) 0);
    Debug ("Running %s\n", args[0]);
    execute (args, env);
    Debug ("Couldn't run %s\n", args[0]);
    LogError ("Cannot execute %s\n", args[0]);
    exit (EX_REMANAGE_DPY);
}

# ifdef MINIX
void tcp_listen_cb(nbio_ref_t ref, int res, int err)
{
	if (!listen_inprogress)
		abort();
	if (res == 0)
		listen_completed= 1;
	else
    		LogError("listen error: %s\n", strerror(err));
	listen_inprogress= 0;
}
# endif

#endif /* XDMCP */
