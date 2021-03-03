/***************************************************************************
                          CXdmcp.cpp  -  description
                             -------------------
    begin                : Tue Nov 9 1999
    copyright            : (C) 1999 by Harald Hoyer
    email                : Harald.Hoyer@RedHat.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include    <config.h>
#endif

#include    <sys/types.h>
#include    <stdio.h>
#include    <ctype.h>
#include    <stdlib.h>

#ifdef HAVE_SYS_SOCKET_H
#include    <sys/socket.h>
#endif

#ifdef HAVE_SYS_SOCKIO_H
#include    <sys/sockio.h>
#endif

#ifdef HAVE_SYS_STROPTS_H
#include    <sys/stropts.h>
#endif

#ifdef HAVE_SYS_STREAM_H
#include    <sys/stream.h>
#endif

#ifdef ISC
#include    <sys/sioctl.h>
#include    <sys/stropts.h>
#endif

#ifndef MINIX
extern "C" {
#include <arpa/inet.h>
#include <netinet/in.h>
}
#else				/* MINIX */
#include <net/hton.h>
#include <net/netlib.h>
#include <net/gen/in.h>
#include <net/gen/netdb.h>
#include <net/gen/tcp.h>
#include <net/gen/tcp_io.h>
#include <net/gen/udp.h>
#include <net/gen/udp_io.h>
#include <sys/nbio.h>
#endif				/* !MINIX */

#include    <sys/ioctl.h>
#ifdef STREAMSCONN
#ifdef WINTCP			/* NCR with Wollongong TCP */
#include    <netinet/ip.h>
#endif
#include    <stropts.h>
#include    <tiuser.h>
#include    <netconfig.h>
#include    <netdir.h>
#endif

#ifdef CSRG_BASED
#include <sys/param.h>
#if (BSD >= 199103)
#define VARIABLE_IFREQ
#endif
#endif

#ifdef XKB
#include <X11/extensions/XKBbells.h>
#endif

#define BROADCAST_HOSTNAME  "BROADCAST"

#ifndef ishexdigit
#define ishexdigit(c)	(isdigit(c) || ('a' <= (c) && (c) <= 'f'))
#endif

#ifdef hpux
# include <sys/utsname.h>
# ifdef HAS_IFREQ
#  include <net/if.h>
# endif
#else
#ifdef __convex__
# include <sync/queue.h>
# include <sync/sema.h>
#endif
#ifndef MINIX
#ifndef __GNU__
# include <net/if.h>
#endif				/* __GNU__ */
#endif
#endif				/* hpux */

#ifndef MINIX
# include    <netdb.h>
#endif

#include "CXdmcp.h"
#include <kcmdlineargs.h>
#include <kapp.h>

ARRAY8Ptr xdmAddress;
ARRAY8Ptr clientAddress;
int connectionType;


/* Converts the hex string s of length len into the byte array d.
   Returns 0 if s was a legal hex string, 1 otherwise.
   */
static int FromHex(const char *s, char *d, int len)
{
    int t;
    int ret = len & 1;		/* odd-length hex strings are illegal */
    while (len >= 2) {
#define HexChar(c)  ('0' <= (c) && (c) <= '9' ? (c) - '0' : (c) - 'a' + 10)

	if (!ishexdigit(*s))
	    ret = 1;
	t = HexChar(*s) << 4;
	s++;
	if (!ishexdigit(*s))
	    ret = 1;
	t += HexChar(*s);
	s++;
	*d++ = t;
	len -= 2;
    }
    return ret;
}


#if ((defined(SVR4) && !defined(sun) && !defined(NCR)) || defined(ISC)) && defined(SIOCGIFCONF)

/* Deal with different SIOCGIFCONF ioctl semantics on these OSs */

int CXdmcp::ifioctl(int fd, int cmd, char *arg)
{
    struct strioctl ioc;
    int ret;

    bzero((char *) &ioc, sizeof(ioc));
    ioc.ic_cmd = cmd;
    ioc.ic_timout = 0;
    if (cmd == SIOCGIFCONF) {
	ioc.ic_len = ((struct ifconf *) arg)->ifc_len;
	ioc.ic_dp = ((struct ifconf *) arg)->ifc_buf;
#ifdef ISC
	/* SIOCGIFCONF is somewhat brain damaged on ISC. The argument
	 * buffer must contain the ifconf structure as header. Ifc_req
	 * is also not a pointer but a one element array of ifreq
	 * structures. On return this array is extended by enough
	 * ifreq fields to hold all interfaces. The return buffer length
	 * is placed in the buffer header.
	 */
	((struct ifconf *) ioc.ic_dp)->ifc_len =
	    ioc.ic_len - sizeof(struct ifconf);
#endif
    } else {
	ioc.ic_len = sizeof(struct ifreq);
	ioc.ic_dp = arg;
    }
    ret = ioctl(fd, I_STR, (char *) &ioc);
    if (ret >= 0 && cmd == SIOCGIFCONF)
#ifdef SVR4
	((struct ifconf *) arg)->ifc_len = ioc.ic_len;
#endif
#ifdef ISC
    {
	((struct ifconf *) arg)->ifc_len =
	    ((struct ifconf *) ioc.ic_dp)->ifc_len;
	((struct ifconf *) arg)->ifc_buf =
	    (caddr_t) ((struct ifconf *) ioc.ic_dp)->ifc_req;
    }
#endif
    return (ret);
}
#else				/* ((SVR4 && !sun && !NCR) || ISC) && SIOCGIFCONF */
#define ifioctl ioctl
#endif				/* ((SVR4 && !sun) || ISC) && SIOCGIFCONF */


void CXdmcp::pingHosts()
{
    pingTry = 0;
    doPingHosts();
}

void CXdmcp::doPingHosts()
{
    HostAddr *hosts;
    for (hosts = hostAddrdb; hosts; hosts = hosts->next) {
	if (hosts->type == QUERY)
#ifdef XIMStringConversionRetrival
	    _XdmcpFlush(socketFD, &directBuffer, hosts->addr,
			hosts->addrlen);
#else
	    _XdmcpFlush(socketFD, &directBuffer, (char *) hosts->addr,
			hosts->addrlen);
#endif
	else
#ifdef XIMStringConversionRetrival
	    _XdmcpFlush(socketFD, &broadcastBuffer, hosts->addr,
			hosts->addrlen);
#else
	    _XdmcpFlush(socketFD, &broadcastBuffer, (char *) hosts->addr,
			hosts->addrlen);
#endif
    }
    if (++pingTry < TRIES)
	t->start(PING_INTERVAL, true);
}

int CXdmcp::addHostname(ARRAY8Ptr hostname, ARRAY8Ptr status,
			struct sockaddr *addr, int willing)
{
    HostName *newname, **names, *name;
    ARRAY8 hostAddr;
    CARD16 connectionType;
    int fulllen;

    char *oldname = 0;

    switch (addr->sa_family) {
	case AF_INET:
	    hostAddr.data =
		(CARD8 *) & ((struct sockaddr_in *) addr)->sin_addr;
	    hostAddr.length = 4;
	    connectionType = FamilyInternet;
	    break;
	default:
	    hostAddr.data = (CARD8 *) "";
	    hostAddr.length = 0;
	    connectionType = FamilyLocal;
	    break;
    }
    for (names = &hostNamedb; *names; names = &(*names)->next) {
	name = *names;
	if (connectionType == name->connectionType &&
	    _XdmcpARRAY8Equal(&hostAddr, &name->hostaddr)) {
	    if (_XdmcpARRAY8Equal(status, &name->status)) {
		return 0;
	    }
	    break;
	}
    }
    if (!*names) {
	newname = (HostName *) malloc(sizeof(HostName));
	if (!newname)
	    return 0;
	if (hostname->length) {
	    switch (addr->sa_family) {
		case AF_INET:
		    {
			struct hostent *hostent;
			char *host;

			hostent =
			    gethostbyaddr((char *) hostAddr.data,
					  hostAddr.length, AF_INET);
			if (hostent) {
			    _XdmcpDisposeARRAY8(hostname);
			    host = hostent->h_name;
			    _XdmcpAllocARRAY8(hostname, strlen(host));
			    memmove(hostname->data, host,
				    hostname->length);
			}
		    }
	    }
	}
	if (!_XdmcpAllocARRAY8(&newname->hostaddr, hostAddr.length)) {
	    free((char *) newname->fullname);
	    free((char *) newname);
	    return 0;
	}
	memmove(newname->hostaddr.data, hostAddr.data, hostAddr.length);
	newname->connectionType = connectionType;
	newname->hostname = *hostname;

	*names = newname;
	newname->next = 0;

      /******************************
       * Add Host
      ******************************/
    } else {
      /******************************
       * change Host
       ******************************/
	newname = *names;
	oldname = newname->fullname;
	_XdmcpDisposeARRAY8(&newname->status);
	_XdmcpDisposeARRAY8(hostname);
    }


    newname->willing = willing;
    newname->status = *status;

    hostname = &newname->hostname;
    fulllen = hostname->length;
    if (fulllen < 30)
	fulllen = 30;
    newname->fullname = (char *) malloc(fulllen + status->length + 10);

    if (!newname->fullname) {
	newname->fullname = const_cast < char *>("Unknown");
    } else {
/*
	  sprintf (newname->fullname, "%-30.*s %*.*s",
	  hostname->length, hostname->data,
	  status->length, status->length, status->data);
*/
	sprintf(newname->fullname, "%*.*s", hostname->length,
		hostname->length, hostname->data);
    }

    if (oldname) {
	emit changeHost(QString::fromLatin1(oldname), newname);
	free(oldname);
    } else {
	emit addHost(newname);
    }

    return 1;
}

void CXdmcp::disposeHostname(HostName * host)
{
    _XdmcpDisposeARRAY8(&host->hostname);
    _XdmcpDisposeARRAY8(&host->hostaddr);
    _XdmcpDisposeARRAY8(&host->status);
    free((char *) host->fullname);
    free((char *) host);
}

void CXdmcp::removeHostname(HostName * host)
{
    HostName **prev, *hosts;

    prev = &hostNamedb;;
    for (hosts = hostNamedb; hosts; hosts = hosts->next) {
	if (hosts == host)
	    break;
	prev = &hosts->next;
    }
    if (!hosts)
	return;
    *prev = host->next;

    disposeHostname(host);

    emit deleteHost(QString::fromLatin1(host->fullname));

}

void CXdmcp::emptyHostnames(void)
{
    HostName *hosts, *next;

    for (hosts = hostNamedb; hosts; hosts = next) {
	next = hosts->next;
	disposeHostname(hosts);
    }

    emit deleteAllHosts();

    hostNamedb = 0;
}

void CXdmcp::slotReceivePacket(int socketFD)
{
    XdmcpHeader header;
    ARRAY8 authenticationName;
    ARRAY8 hostname;
    ARRAY8 status;
    int saveHostname = 0;
    struct sockaddr addr;
    int addrlen;
#ifdef MINIX
    int r;
#endif

#ifdef MINIX
    if (read_inprogress)
	abort();
    if (read_size == 0) {
	r = read(socketFD, read_buffer, sizeof(read_buffer));
	if (r == -1 && errno == EINPROGRESS) {
	    read_inprogress = 1;
	    nbio_inprogress(socketFD, ASIO_READ, 1 /* read */ ,
			    0 /* write */ , 0 /* exception */ );
	} else if (r <= 0) {
	    fprintf(stderr, "chooser: read error: %s\n", r == 0 ?
		    "EOF" : strerror(errno));
	    return;
	}
    }
#endif

    addrlen = sizeof(addr);
#ifdef MINIX
    if (!MNX_XdmcpFill(socketFD, &buffer, &addr, &addrlen,
		       read_buffer, read_size)) {
	return;
    }
    read_size = 0;
#else
    if (!_XdmcpFill(socketFD, &buffer, (XdmcpNetaddr) & addr, &addrlen))
	return;
#endif
    if (!_XdmcpReadHeader(&buffer, &header))
	return;
    if (header.version != XDM_PROTOCOL_VERSION)
	return;
    hostname.data = 0;
    status.data = 0;
    authenticationName.data = 0;
    switch (header.opcode) {
	case WILLING:
	    if (_XdmcpReadARRAY8(&buffer, &authenticationName) &&
		_XdmcpReadARRAY8(&buffer, &hostname) &&
		_XdmcpReadARRAY8(&buffer, &status)) {
		if (header.length == 6 + authenticationName.length +
		    hostname.length + status.length) {
		    if (addHostname
			(&hostname, &status, &addr,
			 header.opcode == (int) WILLING))
			saveHostname = 1;
		}
	    }
	    _XdmcpDisposeARRAY8(&authenticationName);
	    break;
	case UNWILLING:
	    if (_XdmcpReadARRAY8(&buffer, &hostname) &&
		_XdmcpReadARRAY8(&buffer, &status)) {
		if (header.length == 4 + hostname.length + status.length) {
		    if (addHostname
			(&hostname, &status, &addr,
			 header.opcode == (int) WILLING))
			saveHostname = 1;
		}
	    }
	    break;
	default:
	    break;
    }
    if (!saveHostname) {
	_XdmcpDisposeARRAY8(&hostname);
	_XdmcpDisposeARRAY8(&status);
    }
}

void CXdmcp::registerHostaddr(struct sockaddr *addr, int len,
			      xdmOpCode type)
{
    HostAddr *host, **prev;

    host = (HostAddr *) malloc(sizeof(HostAddr));
    if (!host)
	return;
    host->addr = (struct sockaddr *) malloc(len);
    if (!host->addr) {
	free((char *) host);
	return;
    }
    memmove((char *) host->addr, (char *) addr, len);
    host->addrlen = len;
    host->type = type;
    for (prev = &hostAddrdb; *prev; prev = &(*prev)->next);
    *prev = host;
    host->next = NULL;
}

/*
 * Register the address for this host.
 * Called with each of the names on the command line.
 * The special name "BROADCAST" looks up all the broadcast
 *  addresses on the local host.
 */

#if !defined(MINIX) && !defined(__GNU__)

/* Handle variable length ifreq in BNR2 and later */
#ifdef VARIABLE_IFREQ
#define ifr_size(p) (sizeof (struct ifreq) + \
		     (p->ifr_addr.sa_len > sizeof (p->ifr_addr) ? \
		      p->ifr_addr.sa_len - sizeof (p->ifr_addr) : 0))
#else
#define ifr_size(p) (sizeof (struct ifreq))
#endif

void CXdmcp::registerHostname(const char *name)
{
    struct hostent *hostent;
    struct sockaddr_in in_addr;
    struct ifconf ifc;
    register struct ifreq *ifr;
    struct sockaddr broad_addr;
    char buf[2048], *cp, *cplim;

    if (!strcmp(name, BROADCAST_HOSTNAME)) {
#ifdef WINTCP			/* NCR with Wollongong TCP */
	int ipfd;
	struct ifconf *ifcp;
	struct strioctl ioc;
	int n;

	ifcp = (struct ifconf *) buf;
	ifcp->ifc_buf = buf + 4;
	ifcp->ifc_len = sizeof(buf) - 4;

	if ((ipfd = open("/dev/ip", O_RDONLY)) < 0) {
	    t_error("RegisterHostname() t_open(/dev/ip) failed");
	    return;
	}

	ioc.ic_cmd = IPIOC_GETIFCONF;
	ioc.ic_timout = 60;
	ioc.ic_len = sizeof(buf);
	ioc.ic_dp = (char *) ifcp;

	if (ioctl(ipfd, (int) I_STR, (char *) &ioc) < 0) {
	    perror
		("RegisterHostname() ioctl(I_STR(IPIOC_GETIFCONF)) failed");
	    close(ipfd);
	    return;
	}

	for (ifr = ifcp->ifc_req, n = ifcp->ifc_len / sizeof(struct ifreq);
	     --n >= 0; ifr++)
#else				/* WINTCP */
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;
	if (ifioctl(socketFD, (int) SIOCGIFCONF, (char *) &ifc) < 0)
	    return;

#ifdef ISC
#define IFC_IFC_REQ (struct ifreq *) ifc.ifc_buf
#else
#define IFC_IFC_REQ ifc.ifc_req
#endif

	cplim = (char *) IFC_IFC_REQ + ifc.ifc_len;

	for (cp = (char *) IFC_IFC_REQ; cp < cplim; cp += ifr_size(ifr))
#endif				/* WINTCP */
	{
#ifndef WINTCP
	    ifr = (struct ifreq *) cp;
#endif
	    if (ifr->ifr_addr.sa_family != AF_INET)
		continue;

	    broad_addr = ifr->ifr_addr;
	    ((struct sockaddr_in *) &broad_addr)->sin_addr.s_addr =
		htonl(INADDR_BROADCAST);
#ifdef SIOCGIFBRDADDR
	    {
		struct ifreq broad_req;

		broad_req = *ifr;
#ifdef WINTCP			/* NCR with Wollongong TCP */
		ioc.ic_cmd = IPIOC_GETIFFLAGS;
		ioc.ic_timout = 0;
		ioc.ic_len = sizeof(broad_req);
		ioc.ic_dp = (char *) &broad_req;

		if (ioctl(ipfd, I_STR, (char *) &ioc) != -1 &&
#else				/* WINTCP */
		if (ifioctl(socketFD, SIOCGIFFLAGS, (char *) &broad_req) !=
		    -1 &&
#endif				/* WINTCP */
		    (broad_req.ifr_flags & IFF_BROADCAST) &&
		    (broad_req.ifr_flags & IFF_UP)
		    ) {
		    broad_req = *ifr;
#ifdef WINTCP			/* NCR with Wollongong TCP */
		    ioc.ic_cmd = IPIOC_GETIFBRDADDR;
		    ioc.ic_timout = 0;
		    ioc.ic_len = sizeof(broad_req);
		    ioc.ic_dp = (char *) &broad_req;

		    if (ioctl(ipfd, I_STR, (char *) &ioc) != -1)
#else				/* WINTCP */
		    if (ifioctl(socketFD, SIOCGIFBRDADDR, (char *) &broad_req) 
			!= -1)
#endif				/* WINTCP */
			broad_addr = broad_req.ifr_addr;
		    else
			continue;
		} else
		    continue;
	    }
#endif
	    in_addr = *((struct sockaddr_in *) &broad_addr);
	    in_addr.sin_port = htons(XDM_UDP_PORT);
#ifdef BSD44SOCKETS
	    in_addr.sin_len = sizeof(in_addr);
#endif
	    registerHostaddr((struct sockaddr *) &in_addr, sizeof(in_addr),
			     BROADCAST_QUERY);
	}
    } else {
	/* address as hex string, e.g., "12180022" (depreciated) */
	if (strlen(name) == 8 &&
	    FromHex(name, (char *) &in_addr.sin_addr, strlen(name)) == 0) {
	    in_addr.sin_family = AF_INET;
	}
	/* Per RFC 1123, check first for IP address in dotted-decimal form */
	else if ((in_addr.sin_addr.s_addr =
		  inet_addr(name)) != (unsigned) -1)
	    in_addr.sin_family = AF_INET;
	else {
	    hostent = gethostbyname(name);
	    if (!hostent)
		return;
	    if (hostent->h_addrtype != AF_INET || hostent->h_length != 4)
		return;
	    in_addr.sin_family = hostent->h_addrtype;
	    memmove(&in_addr.sin_addr, hostent->h_addr, 4);
	}
	in_addr.sin_port = htons(XDM_UDP_PORT);
#ifdef BSD44SOCKETS
	in_addr.sin_len = sizeof(in_addr);
#endif
	registerHostaddr((struct sockaddr *) &in_addr, sizeof(in_addr),
			 QUERY);
    }
}

#else				/* MINIX */

void CXdmcp::registerHostname(const char *name)
{
    struct hostent *hostent;
    struct sockaddr_in in_addr;

    if (!strcmp(name, BROADCAST_HOSTNAME)) {
	in_addr.sin_addr.s_addr = htonl(0xFFFFFFFF);
	in_addr.sin_port = htons(XDM_UDP_PORT);
	RegisterHostaddr((struct sockaddr *) &in_addr, sizeof(in_addr),
			 BROADCAST_QUERY);
    } else {
	/* address as hex string, e.g., "12180022" (depreciated) */
	if (strlen(name) == 8 &&
	    FromHex(name, (char *) &in_addr.sin_addr, strlen(name)) == 0) {
	    in_addr.sin_family = AF_INET;
	}
	/* Per RFC 1123, check first for IP address in dotted-decimal form */
	else if ((in_addr.sin_addr.s_addr = inet_addr(name)) != -1)
	    in_addr.sin_family = AF_INET;
	else {
	    hostent = gethostbyname(name);
	    if (!hostent)
		return;
	    if (hostent->h_addrtype != AF_INET || hostent->h_length != 4)
		return;
	    in_addr.sin_family = hostent->h_addrtype;
	    memmove(&in_addr.sin_addr, hostent->h_addr, 4);
	}
	in_addr.sin_port = htons(XDM_UDP_PORT);
	RegisterHostaddr((struct sockaddr *) &in_addr, sizeof(in_addr),
			 QUERY);
    }
}
#endif				/* !MINIX */

#if 0
static void RegisterAuthenticationName(char *name, int namelen)
{
    ARRAY8Ptr authName;
    if (!_XdmcpReallocARRAYofARRAY8(&AuthenticationNames,
				    AuthenticationNames.length + 1))
	return;
    authName = &AuthenticationNames.data[AuthenticationNames.length - 1];
    if (!_XdmcpAllocARRAY8(authName, namelen))
	return;
    memmove(authName->data, name, namelen);
}
#endif

int CXdmcp::initXDMCP()
{
    int soopts = 1;
    XdmcpHeader header;
    int i;
#ifdef MINIX
    char *udp_device;
    nwio_udpopt_t udpopt;
    int flags;
    nbio_ref_t ref;
#endif

    header.version = XDM_PROTOCOL_VERSION;
    header.opcode = (CARD16) BROADCAST_QUERY;
    header.length = 1;
    for (i = 0; i < (int) AuthenticationNames.length; i++)
	header.length += 2 + AuthenticationNames.data[i].length;
    _XdmcpWriteHeader(&broadcastBuffer, &header);
    _XdmcpWriteARRAYofARRAY8(&broadcastBuffer, &AuthenticationNames);

    header.version = XDM_PROTOCOL_VERSION;
    header.opcode = (CARD16) QUERY;
    header.length = 1;
    for (i = 0; i < (int) AuthenticationNames.length; i++)
	header.length += 2 + AuthenticationNames.data[i].length;
    _XdmcpWriteHeader(&directBuffer, &header);
    _XdmcpWriteARRAYofARRAY8(&directBuffer, &AuthenticationNames);
#if defined(STREAMSCONN)
    if ((socketFD = t_open("/dev/udp", O_RDWR, 0)) < 0)
	return 0;

    if (t_bind(socketFD, NULL, NULL) < 0) {
	t_close(socketFD);
	return 0;
    }

    /*
     * This part of the code looks contrived. It will actually fit in nicely
     * when the CLTS part of Xtrans is implemented.
     */
    {
	struct netconfig *nconf;

	if ((nconf = getnetconfigent("udp")) == NULL) {
	    t_unbind(socketFD);
	    t_close(socketFD);
	    return 0;
	}

	if (netdir_options(nconf, ND_SET_BROADCAST, socketFD, NULL)) {
	    freenetconfigent(nconf);
	    t_unbind(socketFD);
	    t_close(socketFD);
	    return 0;
	}

	freenetconfigent(nconf);
    }
#else
#ifdef MINIX
    udp_device = getenv("UDP_DEVICE");
    if (udp_device == NULL)
	udp_device = UDP_DEVICE;
    if ((socketFD = open(udp_device, O_RDWR)) == -1)
	return 0;
    udpopt.nwuo_flags = NWUO_SHARED | NWUO_LP_SEL | NWUO_EN_LOC |
	NWUO_EN_BROAD | NWUO_RP_ANY | NWUO_RA_ANY | NWUO_RWDATALL |
	NWUO_DI_IPOPT;
    if (ioctl(socketFD, NWIOSUDPOPT, &udpopt) == -1) {
	close(socketFD);
	return 0;
    }
    if ((flags = fcntl(socketFD, F_GETFD)) == -1) {
	close(socketFD);
	return 0;
    }
    if (fcntl(socketFD, F_SETFD, flags | FD_ASYNCHIO) == -1) {
	close(socketFD);
	return 0;
    }
    nbio_register(socketFD);
    ref.ref_int = socketFD;
    nbio_setcallback(socketFD, ASIO_READ, read_cb, ref);
#else				/* !MINIX */
    if ((socketFD = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	return 0;
#endif				/* MINIX */
#endif
#ifndef STREAMSCONN
#ifdef SO_BROADCAST
    soopts = 1;
    if (setsockopt
	(socketFD, SOL_SOCKET, SO_BROADCAST, (char *) &soopts,
	 sizeof(soopts)) < 0)
	perror("setsockopt");
#endif
#endif

    pingTry = 0;

    return 1;
}

void CXdmcp::chooseHost(const char *r)
{
    HostName *h;
    for (h = hostNamedb; h; h = h->next) {
	if (!strcmp(r, h->fullname))
	    break;
    }
    if (!h)
	return;

    if (xdmAddress) {
	struct sockaddr_in in_addr;
	struct sockaddr *addr = NULL;
	int family;
	int len = 0;
	int fd;
	char buf[1024];
	XdmcpBuffer buffer;
	char *xdm;
#if defined(STREAMSCONN)
	struct t_call call, rcv;
#endif
#ifdef MINIX
	char *tcp_device;
	nwio_tcpconf_t tcpconf;
	nwio_tcpcl_t tcpcl;
#endif

	xdm = (char *) xdmAddress->data;
	family = (xdm[0] << 8) + xdm[1];
	switch (family) {
	    case AF_INET:
#ifdef BSD44SOCKETS
		in_addr.sin_len = sizeof(in_addr);
#endif
		in_addr.sin_family = family;
		memmove(&in_addr.sin_port, xdm + 2, 2);
		memmove(&in_addr.sin_addr, xdm + 4, 4);
		addr = (struct sockaddr *) &in_addr;
		len = sizeof(in_addr);
		break;
	}
#if defined(STREAMSCONN)
	if ((fd = t_open("/dev/tcp", O_RDWR, NULL)) == -1) {
	    fprintf(stderr, "Cannot create response endpoint\n");
	    fflush(stderr);
	    exit(EX_REMANAGE_DPY);
	}
	if (t_bind(fd, NULL, NULL) == -1) {
	    fprintf(stderr, "Cannot bind response endpoint\n");
	    fflush(stderr);
	    t_close(fd);
	    exit(EX_REMANAGE_DPY);
	}
	call.addr.buf = (char *) addr;
	call.addr.len = len;
	call.addr.maxlen = len;
	call.opt.len = 0;
	call.opt.maxlen = 0;
	call.udata.len = 0;
	call.udata.maxlen = 0;
	if (t_connect(fd, &call, NULL) == -1) {
	    t_error("Cannot connect to xdm\n");
	    fflush(stderr);
	    t_unbind(fd);
	    t_close(fd);
	    exit(EX_REMANAGE_DPY);
	}
#else
#ifdef MINIX
	tcp_device = getenv("TCP_DEVICE");
	if (tcp_device == NULL)
	    tcp_device = TCP_DEVICE;
	if ((fd = open(tcp_device, O_RDWR)) == -1) {
	    fprintf(stderr, "Cannot open '%s': %s\n", tcp_device,
		    strerror(errno));
	    exit(EX_REMANAGE_DPY);
	}
	tcpconf.nwtc_flags =
	    NWTC_EXCL | NWTC_LP_SEL | NWTC_SET_RA | NWTC_SET_RP;
	tcpconf.nwtc_remport = in_addr.sin_port;
	tcpconf.nwtc_remaddr = in_addr.sin_addr.s_addr;
	if (ioctl(fd, NWIOSTCPCONF, &tcpconf) == -1) {
	    fprintf(stderr, "NWIOSTCPCONF failed: %s\n", strerror(errno));
	    exit(EX_REMANAGE_DPY);
	}
	tcpcl.nwtcl_flags = 0;
	if (ioctl(fd, NWIOTCPCONN, &tcpcl) == -1) {
	    fprintf(stderr, "NWIOTCPCONN failed: %s\n", strerror(errno));
	    exit(EX_REMANAGE_DPY);
	}
#else				/* !MINIX */
	if ((fd = socket(family, SOCK_STREAM, 0)) == -1) {
	    fprintf(stderr, "Cannot create response socket\n");
	    exit(EX_REMANAGE_DPY);
	}
	if (::connect(fd, addr, len) == -1) {
	    fprintf(stderr, "Cannot connect to xdm\n");
	    exit(EX_REMANAGE_DPY);
	}
#endif				/* MINIX */
#endif
	buffer.data = (BYTE *) buf;
	buffer.size = sizeof(buf);
	buffer.pointer = 0;
	buffer.count = 0;
	_XdmcpWriteARRAY8(&buffer, clientAddress);
	_XdmcpWriteCARD16(&buffer, (CARD16) connectionType);
	_XdmcpWriteARRAY8(&buffer, &h->hostaddr);
#if defined(STREAMSCONN)
	if (t_snd(fd, (char *) buffer.data, buffer.pointer, 0) < 0) {
	    fprintf(stderr, "Cannot send to xdm\n");
	    fflush(stderr);
	    t_unbind(fd);
	    t_close(fd);
	    exit(EX_REMANAGE_DPY);
	}
	sleep(5);		/* Hack because sometimes the connection gets
				   closed before the data arrives on the other end. */
	t_snddis(fd, NULL);
	t_unbind(fd);
	t_close(fd);
#else
	write(fd, (char *) buffer.data, buffer.pointer);
	close(fd);
#endif
    } else {
	int i;

	printf("%u\n", h->connectionType);
	for (i = 0; i < (int) h->hostaddr.length; i++)
	    printf("%u%s", h->hostaddr.data[i],
		   i == h->hostaddr.length - 1 ? "\n" : " ");
    }
}


#ifdef MINIX
void CXdmcp::read_cb(nbio_ref_t ref, int res, int err)
{
    if (!read_inprogress)
	abort();
    if (res > 0) {
	read_size = res;
    } else {
	fprintf(stderr, "chooser: read error: %s\n", res == 0 ?
		"EOF" : strerror(err));
	read_size = 0;
    }
    read_inprogress = 0;
}
#endif

CXdmcp::~CXdmcp()
{
    emptyHostnames();
}

static ARRAY8Ptr CvtStringToARRAY8(QCString fromVal)
{
    ARRAY8Ptr dest;
    char *s;
    int len;

    dest = (ARRAY8Ptr) XtMalloc(sizeof(ARRAY8));
    len = fromVal.length();
    s = (char *) fromVal.data();
    if (_XdmcpAllocARRAY8(dest, len >> 1)) {
	if (!FromHex(s, (char *) dest->data, len))
	    return dest;
	qWarning("chooser: Cannot convert hex string '%s'.", s);
    }
    XtFree((char *)dest);
    return 0;
}

CXdmcp::CXdmcp()
{
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    xdmAddress = CvtStringToARRAY8(args->getOption("xdmaddress"));
    clientAddress = CvtStringToARRAY8(args->getOption("clientaddress"));
    connectionType = args->getOption("connectionType").toInt();

    initXDMCP();

    for (int i = 0, n = args->count(); i < n; i++)
	registerHostname(args->arg(i));

    t = new QTimer(this);
    sn = new QSocketNotifier(socketFD, QSocketNotifier::Read);

    connect(t, SIGNAL(timeout()), this, SLOT(doPingHosts()));

    // connections
    connect(sn, SIGNAL(activated(int)), this,
	    SLOT(slotReceivePacket(int)));
}

#include "CXdmcp.moc"
