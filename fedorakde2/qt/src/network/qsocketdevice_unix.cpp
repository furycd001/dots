/****************************************************************************
** $Id: qt/src/network/qsocketdevice_unix.cpp   2.3.2   edited 2001-09-27 $
**
** Implementation of QSocketDevice class.
**
** Created : 970521
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the network module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition licenses for Unix/X11 or
** for Qt/Embedded may use this file in accordance with the Qt Commercial
** License Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qsocketdevice.h"
#ifndef QT_NO_NETWORK

#include <unistd.h>
#include "qwindowdefs.h"
#include <string.h>

#if defined(_OS_AIX_)
// Please add comments! Which version of AIX? Why?
// Adding #defines specifying BSD compatibility ought to be enough.
#  include <strings.h>
#  include <sys/select.h>
#endif

#include <sys/param.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#if defined(_OS_SOLARIS_) || defined(_OS_UNIXWARE7_) || defined(_OS_RELIANT_)
// FIONREAD is #defined in <sys/filio.h> not <sys/ioctl.h>.
#include <sys/filio.h>
#endif
#include <sys/file.h>
#include <sys/time.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <netdb.h>
#include <errno.h>

#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX    108
#endif

#ifdef __MIPSEL__
#ifndef SOCK_DGRAM
#define SOCK_DGRAM 1
#endif
#ifndef SOCK_STREAM
#define SOCK_STREAM 2
#endif
#endif

#if defined(_OS_SOLARIS_) || defined(_OS_UNIXWARE7_) || defined(_OS_RELIANT_) || defined(_OS_OS2EMX_) || defined(_OS_QNX_) || defined(_OS_QNX6_)
// And this then?  Unixware?
// Also why is this defined on Solaris? Seems to be in <sys/file.h>, unguarded,
// in both Solaris 2.5.1 and Solaris 8!
#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif
#endif


// no includes after this point


#if defined(TIOCINQ) && !defined(FIONREAD)
#define FIONREAD TIOCINQ
#endif

// This mess (it's not yet a mess but I'm sure it'll be one before it's
// done) defines SOCKLEN_T to socklen_t or whatever else, for this system.
// Single Unix 1998 says it's to be socklen_t, classically (XNS4) it's int,
// who knows what it might be on different modern unixes.  size_t seems to
// have been a short-lived non-LP64 compatible error on most decent systems.
//
// Short answer: Single Unix 1995 with XNS4 seems to be the default on most
// modern unixes and you have to explicitly _ask_ for Single Unix 1998 by
// setting for example _XOPEN_SOURCE=500 and we don't do that.

#if defined(SOCKLEN_T)
#undef SOCKLEN_T
#endif

#if defined(_OS_LINUX_) && defined(__GLIBC__) && ( __GLIBC__ >= 2 )
// new linux is Single Unix 1998, not old linux
#  define SOCKLEN_T socklen_t
#elif defined(BSD4_4)
// BSD 4.4
#  if defined(_OS_FREEBSD_) && __FreeBSD_version < 400000
// FreeBSD 4.0 and higher
#    define SOCKLEN_T int
#  else
#    define SOCKLEN_T socklen_t
#  endif
#elif defined(_OS_UNIXWARE7_) || defined(_OS_RELIANT_)
#  define SOCKLEN_T size_t
#elif defined(_OS_AIX_)
#  ifdef _AIX43
// AIX 4.3
// The AIX 4.3 online documentation says 'size_t'. But a user asked IBM
// and they are reported to have told him the documentation is wrong.
// In addition 'socklen_t' is needed by many users, so...
#    define SOCKLEN_T socklen_t
#  elif _AIX42
// AIX 4.2
#    define SOCKLEN_T size_t
#  else
// AIX 4.1
#    define SOCKLEN_T int
#  endif
#elif defined(_OS_OSF_)
#  if defined(_POSIX_PII_SOCKET)
#    define SOCKLEN_T socklen_t
#  elif defined(_XOPEN_SOURCE_EXTENDED)
#    define SOCKLEN_T size_t
#  else
#    define SOCKLEN_T int
#  endif
#elif defined(_OS_SOLARIS_)
#  if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE-0 >= 500)
// on Solaris 7 and better with specific feature test macros
#    define SOCKLEN_T socklen_t
#  elif defined(_XOPEN_SOURCE_EXTENDED)
// on Solaris 2.6 and better with specific feature test macros
#    define SOCKLEN_T size_t
#  else
// always this case in practice
#    define SOCKLEN_T int
#  endif
#elif defined(_OS_QNX_) || defined(_OS_QNX6_)
#  define SOCKLEN_T size_t
#else
// Most unixes are Single Unix 1995 compliant (at least by default) including
// irix, osf1/tru64, solaris, hp-ux and old linux. We do not specify Single
// Unix 1998 even when it is available.
#  define SOCKLEN_T int
#endif


// Tru64 redefines accept() to _accept() when _XOPEN_SOURCE_EXTENDED is
// defined.  This breaks our sources.
static inline int qt_socket_accept(int s, struct sockaddr *addr, SOCKLEN_T *addrlen)
{ return ::accept(s, addr, addrlen); }
#if defined(accept)
# undef accept
#endif

// Solaris redefines connect to __xnet_connect when _XOPEN_SOURCE_EXTENDED
// is defined.  This breaks our sources.
static inline int qt_socket_connect(int s, struct sockaddr *addr, SOCKLEN_T addrlen)
{ return ::connect(s, addr, addrlen); }
#if defined(connect)
# undef connect
#endif

// Solaris redefines bind to __xnet_bind when _XOPEN_SOURCE_EXTENDED is
// defined.  This breaks our sources.
static inline int qt_socket_bind(int s, struct sockaddr *addr, SOCKLEN_T addrlen)
{ return ::bind(s, addr, addrlen); }
#if defined(bind)
# undef bind
#endif

// UnixWare 7 redefines listen() to _listen().  This breaks our sources.
static inline int qt_socket_listen(int s, int backlog)
{ return ::listen(s, backlog); }
#if defined(listen)
# undef listen
#endif


// the next mess deals with EAGAIN and/or EWOULDBLOCK

#if !defined(EAGAIN) && !defined(EWOULDBLOCK)
#error "requires EAGAIN or EWOULDBLOCK"
#endif

// if one is there, define the other one similarly, so we can switch() easily
#if defined(EAGAIN) && !defined(EWOULDBLOCK)
#define EWOULDBLOCK EAGAIN
#endif
#if defined(EWOULDBLOCK) && !defined(EAGAIN)
#define EAGAIN EWOULDBLOCK
#endif


// internal
void QSocketDevice::init()
{
}


/*!
  Creates a QSocketDevice object for a stream or datagram socket.

  The \a type argument must be either \c QSocketDevice::Stream for a
  reliable, connection-oriented TCP socket, or \c
  QSocketDevice::Datagram for an unreliable UDP socket.

  \sa blocking()
*/
QSocketDevice::QSocketDevice( Type type )
    : fd( -1 ), t( Stream ), p( 0 ), pp( 0 ), e( NoError ), d( 0 )
{
#if defined(QSOCKETDEVICE_DEBUG)
    qDebug( "QSocketDevice: Created QSocketDevice object %p, type %d",
	    this, type );
#endif
    init();
    int s = ::socket( AF_INET, type==Datagram?SOCK_DGRAM:SOCK_STREAM, 0 );
    if ( s < 0 ) {
	// leave fd at -1 but set the type
	t = type;
	switch( errno ) {
	case EPROTONOSUPPORT:
	    e = Bug; // 0 is supposed to work for both types
	    break;
	case ENFILE:
	    e = NoFiles; // special case for this
	    break;
	case EACCES:
	    e = Inaccessible;
	    break;
	case ENOBUFS:
	case ENOMEM:
	    e = NoResources;
	    break;
	case EINVAL:
	    e = Impossible;
	    break;
	default:
	    e = UnknownError;
	    break;
	}
    } else {
	setSocket( s, type );
    }
}


/*! \reimp

  Closes the socket and sets the socket identifier to -1 (invalid).

  (This function ignores errors; if there are any then a file
  descriptor leakage might result.  As far as we know, the only error
  that can arise is EBADF, and that would of course not cause leakage.
  There may be OS-specfic errors that we haven't come across,
  however.)

  \sa open()
*/
void QSocketDevice::close()
{
    if ( fd == -1 || !isOpen() )		// already closed
	return;
    setFlags( IO_Sequential );
    setStatus( IO_Ok );
    ::close( fd );
#if defined(QSOCKETDEVICE_DEBUG)
    qDebug( "QSocketDevice::close: Closed socket %x", fd );
#endif
    fd = -1;
    fetchConnectionParameters();
}


/*!
  Returns TRUE if the socket is in blocking mode, or FALSE if it
  is in nonblocking mode or if the socket is invalid.

  Note that this function does not set error().

  \warning On Windows, this function always returns TRUE since the
  ioctlsocket() function is broken.

  \sa setBlocking(), isValid()
*/
bool QSocketDevice::blocking() const
{
    if ( !isValid() )
	return TRUE;
    int s = fcntl(fd, F_GETFL, 0);
    return !(s >= 0 && ((s & FNDELAY) != 0));
}


/*!
  Makes the socket blocking if \a enable is TRUE or nonblocking if
  \a enable is FALSE.

  Sockets are blocking by default, but we recommend using
  nonblocking socket operations, especially for GUI programs that need
  to be responsive.

  \warning On Windows, this function does nothing since the
  ioctlsocket() function is broken.

  Whenever you use a QSocketNotifier on Windows, the socket is immediately
  made nonblocking.

  \sa blocking(), isValid()
*/
void QSocketDevice::setBlocking( bool enable )
{
#if defined(QSOCKETDEVICE_DEBUG)
    qDebug( "QSocketDevice::setBlocking( %d )", enable );
#endif
    if ( !isValid() )
	return;
    int tmp = ::fcntl(fd, F_GETFL, 0);
    if ( tmp >= 0 )
	tmp = fcntl( fd, F_SETFL, enable ? (tmp&~FNDELAY) : (tmp|FNDELAY) );
    if ( tmp >= 0 )
	return;
    if ( e )
	return;
    switch( errno ) {
    case EACCES:
    case EBADF:
	e = Impossible;
	break;
    case EFAULT:
    case EAGAIN:
#if EAGAIN != EWOULDBLOCK
    case EWOULDBLOCK:
#endif
    case EDEADLK:
    case EINTR:
    case EINVAL:
    case EMFILE:
    case ENOLCK:
    case EPERM:
    default:
	e = UnknownError;
    }
}


/*!
  Returns a socket option.
*/
int QSocketDevice::option( Option opt ) const
{
    if ( !isValid() )
	return -1;
    int n = -1;
    int v = -1;
    switch ( opt ) {
    case Broadcast:
	n = SO_BROADCAST;
	break;
    case ReceiveBuffer:
	n = SO_RCVBUF;
	break;
    case ReuseAddress:
	n = SO_REUSEADDR;
	break;
    case SendBuffer:
	n = SO_SNDBUF;
	break;
    }
    if ( n != -1 ) {
	SOCKLEN_T len;
	len = sizeof(v);
	int r = ::getsockopt( fd, SOL_SOCKET, n, (char*)&v, &len );
	if ( r >= 0 )
	    return v;
	if ( !e ) {
	    QSocketDevice *that = (QSocketDevice*)this; // mutable function
	    switch( errno ) {
	    case EBADF:
	    case ENOTSOCK:
		that->e = Impossible;
		break;
	    case EFAULT:
		that->e = Bug;
		break;
	    default:
		that->e = UnknownError;
		break;
	    }
	}
	return -1;
    }
    return v;
}


/*!
  Sets a socket option.
*/
void QSocketDevice::setOption( Option opt, int v )
{
    if ( !isValid() )
	return;
    int n = -1; // for really, really bad compilers
    switch ( opt ) {
    case Broadcast:
	n = SO_BROADCAST;
	break;
    case ReceiveBuffer:
	n = SO_RCVBUF;
	break;
    case ReuseAddress:
	n = SO_REUSEADDR;
	break;
    case SendBuffer:
	n = SO_SNDBUF;
	break;
    default:
	return;
    }
    if ( ::setsockopt( fd, SOL_SOCKET, n, (char*)&v, sizeof(v)) < 0 &&
	 e == NoError ) {
	switch( errno ) {
	case EBADF:
	case ENOTSOCK:
	    e = Impossible;
	    break;
	case EFAULT:
	    e = Bug;
	    break;
	default:
	    e = UnknownError;
	    break;
	}
    }
}


/*!
  Connects to the IP address and port specified by \a addr.  Returns
  TRUE if it establishes a connection, and FALSE if not.  error()
  explains why.

  Note that error() commonly returns NoError for non-blocking sockets;
  this just means that you can call connect() again in a little while
  and it'll probably succeed.
*/
bool QSocketDevice::connect( const QHostAddress &addr, Q_UINT16 port )
{
    if ( !isValid() )
	return FALSE;

    if ( !addr.isIp4Addr() ) {
	qWarning( "QSocketDevice: IPv6 is not supported by this version" );
	e = Impossible;
	return FALSE;
    }
    struct sockaddr_in aa;
    memset( &aa, 0, sizeof(aa) );
    aa.sin_family = AF_INET;
    aa.sin_port = htons( port );
    aa.sin_addr.s_addr = htonl( addr.ip4Addr() );

    int r = qt_socket_connect( fd, (struct sockaddr*)&aa,
		       sizeof(struct sockaddr_in) );
    if ( r == 0 ) {
	fetchConnectionParameters();
	return TRUE;
    }
    if ( errno == EISCONN || errno == EALREADY || errno == EINPROGRESS )
	return TRUE;
    if ( e != NoError || errno == EAGAIN || errno == EWOULDBLOCK )
	return FALSE;
    switch( errno ) {
    case EBADF:
    case ENOTSOCK:
	e = Impossible;
	break;
    case EFAULT:
    case EAFNOSUPPORT:
	e = Bug;
	break;
    case ECONNREFUSED:
	e = ConnectionRefused;
	break;
    case ETIMEDOUT:
    case ENETUNREACH:
	e = NetworkFailure;
	break;
    case EADDRINUSE:
	e = NoResources;
	break;
    case EACCES:
    case EPERM:
	e = Inaccessible;
	break;
    default:
	e = UnknownError;
	break;
    }
    return FALSE;
}


/*!
  Assigns a name to an unnamed socket.  If the operation succeeds,
  bind() returns TRUE.  Otherwise, it returns FALSE without changing
  what port() and address() return.

  bind() is used by servers for setting up incoming connections.
  Call bind() before listen().
*/
bool QSocketDevice::bind( const QHostAddress &address, Q_UINT16 port )
{
    if ( !isValid() )
	return FALSE;

    if ( !address.isIp4Addr() ) {
	qWarning( "QSocketDevice: IPv6 is not supported by this version" );
	e = Impossible;
	return FALSE;
    }
    struct sockaddr_in aa;
    memset( &aa, 0, sizeof(aa) );
    aa.sin_family = AF_INET;
    aa.sin_port = htons( port );
    aa.sin_addr.s_addr = htonl( address.ip4Addr() );

    int r = qt_socket_bind( fd, (struct sockaddr*)&aa,sizeof(struct sockaddr_in) );
    if ( r < 0 ) {
	switch( errno ) {
	case EINVAL:
	    e = AlreadyBound;
	    break;
	case EACCES:
	    e = Inaccessible;
	    break;
	case ENOMEM:
	    e = NoResources;
	    break;
	case EFAULT: // a was illegal
	case ENAMETOOLONG: // sz was wrong
	    e = Bug;
	    break;
	case EBADF: // AF_UNIX only
	case ENOTSOCK: // AF_UNIX only
	case EROFS: // AF_UNIX only
	case ENOENT: // AF_UNIX only
	case ENOTDIR: // AF_UNIX only
	case ELOOP: // AF_UNIX only
	    e = Impossible;
	    break;
	default:
	    e = UnknownError;
	    break;
	}
	return FALSE;
    }
    fetchConnectionParameters();
    return TRUE;
}


/*!
  Specifies how many pending connections a server socket can have.
  Returns TRUE if the operation was successful, otherwise FALSE.

  The listen() call only applies to sockets where type() is \c Stream,
  not \c Datagram sockets.  listen() must not be called before bind()
  or after accept().  It is common to use a \a backlog value of 50 on
  most Unix systems.

  \sa bind(), accept()
*/
bool QSocketDevice::listen( int backlog )
{
    if ( !isValid() )
	return FALSE;
    if ( qt_socket_listen( fd, backlog ) >= 0 )
	return TRUE;
    if ( !e )
	e = Impossible;
    return FALSE;
}


/*!
  Extracts the first connection from the queue of pending connections
  for this socket and returns a new socket identifier.  Returns -1
  if the operation failed.

  \sa bind(), listen()
*/
int QSocketDevice::accept()
{
    if ( !isValid() )
	return FALSE;
    struct sockaddr aa;
    SOCKLEN_T l = sizeof(struct sockaddr);
    int s = qt_socket_accept( fd, (struct sockaddr*)&aa, &l );
    // we'll blithely throw away the stuff accept() wrote to aa
    if ( s < 0 && e == NoError ) {
	switch( errno ) {
#if defined(EPROTO)
	case EPROTO:
#endif
#if defined(ENONET)
	case ENONET:
#endif
	case ENOPROTOOPT:
	case EHOSTDOWN:
	case EOPNOTSUPP:
	case EHOSTUNREACH:
	case ENETDOWN:
	case ENETUNREACH:
	case ETIMEDOUT:
	    // in all these cases, an error happened during connection
	    // setup.  we're not interested in what happened, so we
	    // just treat it like the client-closed-quickly case.
	case EPERM:
	    // firewalling wouldn't let us accept.  we treat it like
	    // the client-closed-quickly case.
	case EAGAIN:
#if EAGAIN != EWOULDBLOCK
	case EWOULDBLOCK:
#endif
	    // the client closed the connection before we got around
	    // to accept()ing it.
	    break;
	case EBADF:
	case ENOTSOCK:
	    e = Impossible;
	    break;
	case EFAULT:
	    e = Bug;
	    break;
	case ENOMEM:
	case ENOBUFS:
	    e = NoResources;
	    break;
	default:
	    e = UnknownError;
	    break;
	}
    }
    return s;
}


/*!
  Returns the number of bytes available for reading, or -1 if an
  error occurred.

  \warning On Microsoft Windows, we use the ioctlsocket() function
  to determine the number of bytes queued on the socket.
  According to Microsoft (KB Q125486), ioctlsocket() sometimes
  return an incorrect number.  The only safe way to determine the
  amount of data on the socket is to read it using readBlock().
  QSocket has workarounds to deal with this problem.
*/
int QSocketDevice::bytesAvailable() const
{
    if ( !isValid() )
	return -1;
    // gives shorter than true amounts on Unix domain sockets.
    int nbytes = 0;
    if ( ::ioctl(fd, FIONREAD, (char*)&nbytes) < 0 )
	return -1;
    return nbytes;
}


/*!
  Wait up to \a msecs milliseconds for more data to be available.
  If \a msecs is -1 the call will block indefinitely.

  This is a blocking call and should be avoided in event driven
  applications.

  Returns the number of bytes available for reading, or -1 if an
  error occurred.

  \sa bytesAvailable()
*/
int QSocketDevice::waitForMore( int msecs ) const
{
    if ( !isValid() )
	return -1;

    fd_set fds;
    struct timeval tv;

    FD_ZERO( &fds );
    FD_SET( fd, &fds );

    tv.tv_sec = msecs / 1000;
    tv.tv_usec = (msecs % 1000) * 1000;

    int rv = select( fd+1, &fds, 0, 0, msecs < 0 ? 0 : &tv );

    if ( rv < 0 )
	return -1;

    return bytesAvailable();
}


/*!
  Reads max \a maxlen bytes from the socket into \a data and returns
  the number of bytes read.  Returns -1 if an error occurred.
*/
int QSocketDevice::readBlock( char *data, uint maxlen )
{
#if defined(CHECK_NULL)
    if ( data == 0 && maxlen != 0 ) {
	qWarning( "QSocketDevice::readBlock: Null pointer error" );
    }
#endif
#if defined(CHECK_STATE)
    if ( !isValid() ) {
	qWarning( "QSocketDevice::readBlock: Invalid socket" );
	return -1;
    }
    if ( !isOpen() ) {
	qWarning( "QSocketDevice::readBlock: Device is not open" );
	return -1;
    }
    if ( !isReadable() ) {
	qWarning( "QSocketDevice::readBlock: Read operation not permitted" );
	return -1;
    }
#endif
    bool done = FALSE;
    int r = 0;
    while ( done == FALSE ) {
	if ( t == Datagram ) {
	    struct sockaddr_in aa;
	    memset( &aa, 0, sizeof(aa) );
	    SOCKLEN_T sz;
	    sz = sizeof( aa );
	    r = ::recvfrom( fd, data, maxlen, 0,
			    (struct sockaddr *)&aa, &sz );
	    pp = ntohs( aa.sin_port );
	    pa = QHostAddress( ntohl( aa.sin_addr.s_addr ) );
	} else {
	    r = ::read( fd, data, maxlen );
	}
	done = TRUE;
	if ( r >= 0 || errno == EAGAIN || errno == EWOULDBLOCK ) {
	    // nothing
	} else if ( errno == EINTR ) {
	    done = FALSE;
	} else if ( e == NoError ) {
	    switch( errno ) {
	    case EIO:
	    case EISDIR:
	    case EBADF:
	    case EINVAL:
	    case EFAULT:
	    case ENOTCONN:
	    case ENOTSOCK:
		e = Impossible;
		break;
#if defined(ENONET)
	    case ENONET:
#endif
	    case EHOSTUNREACH:
	    case ENETDOWN:
	    case ENETUNREACH:
	    case ETIMEDOUT:
		e = NetworkFailure;
		break;
	    default:
		e = UnknownError;
		break;
	    }
	}
    }
    return r;
}


/*!
  Writes \a len bytes to the socket from \a data and returns
  the number of bytes written.  Returns -1 if an error occurred.

  This is used for \c QSocketDevice::Stream sockets.
*/
int QSocketDevice::writeBlock( const char *data, uint len )
{
    if ( data == 0 && len != 0 ) {
#if defined(CHECK_NULL) || defined(QSOCKETDEVICE_DEBUG)
	qWarning( "QSocketDevice::writeBlock: Null pointer error" );
#endif
	return -1;
    }
    if ( !isValid() ) {
#if defined(CHECK_STATE) || defined(QSOCKETDEVICE_DEBUG)
	qWarning( "QSocketDevice::writeBlock: Invalid socket" );
#endif
	return -1;
    }
    if ( !isOpen() ) {
#if defined(CHECK_STATE) || defined(QSOCKETDEVICE_DEBUG)
	qWarning( "QSocketDevice::writeBlock: Device is not open" );
#endif
	return -1;
    }
    if ( !isWritable() ) {
#if defined(CHECK_STATE) || defined(QSOCKETDEVICE_DEBUG)
	qWarning( "QSocketDevice::writeBlock: Write operation not permitted" );
#endif
	return -1;
    }
    bool done = FALSE;
    int r = 0;
    while ( !done ) {
	r = ::write( fd, data, len );
	done = TRUE;
	if ( r < 0 && e == NoError &&
	     errno != EAGAIN && errno != EWOULDBLOCK ) {
	    switch( errno ) {
	    case EINTR: // signal - call read() or whatever again
		done = FALSE;
		break;
	    case ENOSPC:
	    case EPIPE:
	    case EIO:
	    case EISDIR:
	    case EBADF:
	    case EINVAL:
	    case EFAULT:
	    case ENOTCONN:
	    case ENOTSOCK:
		e = Impossible;
		break;
#if defined(ENONET)
	    case ENONET:
#endif
	    case EHOSTUNREACH:
	    case ENETDOWN:
	    case ENETUNREACH:
	    case ETIMEDOUT:
		e = NetworkFailure;
		break;
	    default:
		e = UnknownError;
		break;
	    }
	}
    }
    return r;
}


/*!
  Writes \a len bytes to the socket from \a data and returns
  the number of bytes written.  Returns -1 if an error occurred.

  This is used for \c QSocketDevice::Datagram sockets. You have to specify the
  \a host and \a port of the destination of the data.
*/
int QSocketDevice::writeBlock( const char * data, uint len,
			       const QHostAddress & host, Q_UINT16 port )
{
    if ( t != Datagram ) {
#if defined(CHECK_STATE) || defined(QSOCKETDEVICE_DEBUG)
	qWarning( "QSocketDevice::sendBlock: Not datagram" );
#endif
	return -1; // for now - later we can do t/tcp
    }

    if ( data == 0 && len != 0 ) {
#if defined(CHECK_NULL) || defined(QSOCKETDEVICE_DEBUG)
	qWarning( "QSocketDevice::sendBlock: Null pointer error" );
#endif
	return -1;
    }
    if ( !isValid() ) {
#if defined(CHECK_STATE) || defined(QSOCKETDEVICE_DEBUG)
	qWarning( "QSocketDevice::sendBlock: Invalid socket" );
#endif
	return -1;
    }
    if ( !isOpen() ) {
#if defined(CHECK_STATE) || defined(QSOCKETDEVICE_DEBUG)
	qWarning( "QSocketDevice::sendBlock: Device is not open" );
#endif
	return -1;
    }
    if ( !isWritable() ) {
#if defined(CHECK_STATE) || defined(QSOCKETDEVICE_DEBUG)
	qWarning( "QSocketDevice::sendBlock: Write operation not permitted" );
#endif
	return -1;
    }
    if ( !host.isIp4Addr() ) {
	qWarning( "QSocketDevice: IPv6 is not supported by this version" );
	e = Impossible;
	return FALSE;
    }
    struct sockaddr_in aa;
    memset( &aa, 0, sizeof(aa) );
    aa.sin_family = AF_INET;
    aa.sin_port = htons( port );
    aa.sin_addr.s_addr = htonl( host.ip4Addr() );

    // we'd use MSG_DONTWAIT + MSG_NOSIGNAL if Stevens were right.
    // but apparently Stevens and most implementors disagree
    bool done = FALSE;
    int r = 0;
    while ( !done ) {
	r = ::sendto( fd, data, len, 0,
		      (struct sockaddr *)(&aa), sizeof(sockaddr_in) );
	done = TRUE;
	if ( r < 0 && e == NoError &&
	     errno != EAGAIN && errno != EWOULDBLOCK ) {
	    switch( errno ) {
	    case EINTR: // signal - call read() or whatever again
		done = FALSE;
		break;
	    case ENOSPC:
	    case EPIPE:
	    case EIO:
	    case EISDIR:
	    case EBADF:
	    case EINVAL:
	    case EFAULT:
	    case ENOTCONN:
	    case ENOTSOCK:
		e = Impossible;
		break;
#if defined(ENONET)
	    case ENONET:
#endif
	    case EHOSTUNREACH:
	    case ENETDOWN:
	    case ENETUNREACH:
	    case ETIMEDOUT:
		e = NetworkFailure;
		break;
	    default:
		e = UnknownError;
		break;
	    }
	}
    }
    return r;
}


/*!
  Fetches information about both ends of the connection - whatever
  is available.
*/
void QSocketDevice::fetchConnectionParameters()
{
    if ( !isValid() ) {
	p = 0;
	a = QHostAddress();
	pp = 0;
	pa = QHostAddress();
	return;
    }
    struct sockaddr_in sa;
    memset( &sa, 0, sizeof(sa) );
    SOCKLEN_T sz;
    sz = sizeof( sa );
    if ( !::getsockname( fd, (struct sockaddr *)(&sa), &sz ) ) {
	p = ntohs( sa.sin_port );
	a = QHostAddress( ntohl( sa.sin_addr.s_addr ) );
    }
    if ( !::getpeername( fd, (struct sockaddr *)(&sa), &sz ) ) {
	pp = ntohs( sa.sin_port );
	pa = QHostAddress( ntohl( sa.sin_addr.s_addr ) );
    }
}


/*!
  Returns the port number of the port this socket device is connected to. This
  may be 0 for a while, but is set to something sensible when there is a
  sensible value it can have.

  Note that for Datagram sockets, this is the source port of the last packet
  received.
*/
Q_UINT16 QSocketDevice::peerPort() const
{
    return pp;
}


/*!
  Returns the address of the port this socket device is connected to. This may
  be 0.0.0.0 for a while, but is set to something sensible when there is a
  sensible value it can have.

  Note that for Datagram sockets, this is the source address of the last packet
  received.
*/
QHostAddress QSocketDevice::peerAddress() const
{
    return pa;
}

#endif //QT_NO_NETWORK
