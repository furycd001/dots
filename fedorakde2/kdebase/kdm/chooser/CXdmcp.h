/***************************************************************************
                          CXdmcp.h  -  description
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
#ifndef CXDMCP_H
#define CXDMCP_H

#ifndef QT_CLEAN_NAMESPACE
#define QT_CLEAN_NAMESPACE
#endif
#include <qapplication.h>
#include <qsocketnotifier.h>

#include <qtimer.h>

extern "C" {
#include <X11/Xos.h>
#ifdef index
#undef index
#undef rindex
#endif
#include <X11/Xfuncs.h>
#include <X11/Xmd.h>
#include <X11/Xauth.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
// this macro is only defined in XFree < 4.0
#ifdef XIMStringConversionRetrival
# include "dm.h"
#endif
#ifndef XIMStringConversionRetrival
# include "dm.h"
#endif
#include "CXdmcp_c.h"
}
#include    <sys/types.h>
#include    <stdio.h>
#include    <ctype.h>
#if defined(SVR4) && !defined(SCO325)
#include    <sys/sockio.h>
#endif
#if defined(SVR4) && defined(PowerMAX_OS)
#include    <sys/stropts.h>
#endif
#if defined(SYSV) && defined(i386)
#include    <sys/stream.h>
#ifdef ISC
#include    <sys/sioctl.h>
#include    <sys/stropts.h>
#endif
#endif
#define XtRARRAY8   "ARRAY8"
#define PING_INTERVAL	2000
#define TRIES		3
struct _app_resources {
    ARRAY8Ptr xdmAddress;
    ARRAY8Ptr clientAddress;
    int connectionType;
};


class CXdmcp:public QObject {
    Q_OBJECT 

  public:

    typedef struct _hostName {
	struct _hostName *next;
	char *fullname;
	int willing;
	ARRAY8 hostname, status;
	CARD16 connectionType;
	ARRAY8 hostaddr;
    } HostName;

    /* Constructor with command line arguments.
     */
     CXdmcp();
    ~CXdmcp();

    /* Add hostname to ping.
     * "BROADCAST" is special.
     */
    void registerHostname(const char *name);

    /* Empty Hostname list.
     */
    void emptyHostnames(void);

    /* Select Host.
     */
    void chooseHost(const char *h);

    /* Ping all specified hosts.
     */
    void pingHosts();

  signals:
    /* No more hosts to display.
     */
    void deleteAllHosts();

    /* Remove host from list.
     */
    void deleteHost(const QString & hn);

    /* Add host to list.
     */
    void addHost(CXdmcp::HostName * newname);

    /* Change hosts name in list.
     */
    void changeHost(const QString & hn, CXdmcp::HostName * newname);

  public slots:
    /* To call when socket is ready.
     */
    void slotReceivePacket(int);

  private slots:
    void doPingHosts();

  private:
     QSocketNotifier * sn;

    ARRAYofARRAY8 AuthenticationNames;
    int socketFD;
    QTimer *t;
    int pingTry;

    int ifioctl(int fd, int cmd, char *arg);
    void rebuildTable(int size);
    int addHostname(ARRAY8Ptr hostname, ARRAY8Ptr status,
		    struct sockaddr *addr, int willing);

    void disposeHostname(HostName * host);
    void removeHostname(HostName * host);
    void registerHostaddr(struct sockaddr *addr, int len, xdmOpCode type);
    int initXDMCP();

#ifdef MINIX
    char read_buffer[XDM_MAX_MSGLEN + sizeof(udp_io_hdr_t)];
    int read_inprogress;
    int read_size;
    void read_cb(nbio_ref_t ref, int res, int err);
#endif

    int fromHex(char *s, char *d, int len);


    struct _app_resources app_resources;


    typedef struct _hostAddr {
	struct _hostAddr *next;
	struct sockaddr *addr;
	int addrlen;
	xdmOpCode type;
    } HostAddr;

    HostAddr *hostAddrdb;
    HostName *hostNamedb;

    XdmcpBuffer directBuffer, broadcastBuffer;
    XdmcpBuffer buffer;
};

#endif
