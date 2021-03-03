/****************************************************************************
** $Id: qt/src/network/qsocketdevice.cpp   2.3.2   edited 2001-08-21 $
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
** Licensees holding valid Qt Enterprise Edition licenses may use this
** file in accordance with the Qt Commercial License Agreement provided
** with the Software.
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

#include "qwindowdefs.h"
#include <string.h>


//#define QSOCKETDEVICE_DEBUG


class QSocketDevicePrivate
{
public:
};



/*!
  \class QSocketDevice qsocketdevice.h
  \brief The QSocketDevice class provides a platform-independent low-level socket API.

  \module network

  This class is not really meant for use outside Qt.  It can be used
  to achieve some things that QSocket does not provide, but it's
  not particularly easy to understand or use.

  The basic purpose of the class is to provide a QIODevice that works
  on sockets, wrapped in a platform-independent API.

  \sa QSocket, QSocketNotifier, QHostAddress
*/

/*! \enum QSocketDevice::Error

  This enum type describes the error states of QSocketDevice.  At present these
  errors are defined:

  <ul>
  <li> \c NoError - all is fine.

  <li> \c AlreadyBound - bind() said so.

  <li> \c Inaccessible - the operating system or firewall prohibits something.

  <li> \c NoResources - the operating system ran out of something.

  <li> \c Bug - there seems to be a bug in QSocketDevice.

  <li> \c Impossible - the impossible happened, usually because you confused
  QSocketDevice horribly.  Simple example:
  \code
  ::close( sd->socket() );
  sd->writeBlock( someData, 42 );
  \endcode
  The libc ::close() closes the socket, but QSocketDevice is not aware
  of that.  So when you call writeBlock(), the impossible happens.

  <li> \c NoFiles - the operating system will not let QSocketDevice open
  another file.

  <li> \c ConnectionRefused - a connection attempt was rejected by the
  peer.

  <li> \c NetworkFailure - there is a network failure between this host
  and... and whatever.

  <li> \c UnknownError - the operating system reacted in a way that the
  Qt developers did not foresee.
  </ul>
*/

/*! \enum QSocketDevice::Type

  This enum type describes the type of the socket:
  <ul>
  <li> \c Stream - a stream socket
  <li> \c Datagram - a datagram socket
  </ul>
*/


/*!
  Creates a QSocketDevice object for an existing socket.

  The \a type argument must match the actual socket type;
  \c QSocketDevice::Stream for a reliable, connection-oriented TCP socket, or
  \c QSocketDevice::Datagram for an unreliable, connectionless UDP socket.
*/
QSocketDevice::QSocketDevice( int socket, Type type )
    : fd( -1 ), t( Stream ), p( 0 ), pp( 0 ), e( NoError ),
      d( 0 )
{
#if defined(QSOCKETDEVICE_DEBUG)
    qDebug( "QSocketDevice: Created QSocketDevice %p (socket %x, type %d)",
	   this, socket, type );
#endif
    init();
    setSocket( socket, type );
}


/*!
  Destroys the socket device and closes the socket if it is open.
*/
QSocketDevice::~QSocketDevice()
{
    close();
    delete d;
    d = 0;
#if defined(QSOCKETDEVICE_DEBUG)
    qDebug( "QSocketDevice: Destroyed QSocketDevice %p", this );
#endif
}


/*!
  Returns TRUE if this is a valid socket or FALSE if it is an invalid
  socket.  This is actually a shortcut for socket() == -1.

  \sa socket()
*/
bool QSocketDevice::isValid() const
{
    return socket() != -1;
}


/*!
  \fn Type QSocketDevice::type() const

  Returns the socket type; \c QSocketDevice::Stream for a reliable,
  connection-oriented TCP socket, or \c QSocketDevice::Datagram for an
  unreliable UDP socket.

  \sa socket()
*/
QSocketDevice::Type QSocketDevice::type() const
{
    return t;
}


/*!
  Returns the socket number, or -1 if it is an invalid socket.

  \sa isValid(), type()
*/
int QSocketDevice::socket() const
{
    return fd;
}


/*!
  Sets an existing socket.

  The \a type argument must match the actual socket type;
  \c QSocketDevice::Stream for a reliable, connection-oriented TCP socket, or
  \c QSocketDevice::Datagram for an unreliable, connectionless UDP socket.

  Any existing socket is closed.

  \sa isValid(), close()
*/
void QSocketDevice::setSocket( int socket, Type type )
{
    if ( fd != -1 )			// close any open socket
	close();
#if defined(QSOCKETDEVICE_DEBUG)
    qDebug( "QSocketDevice::setSocket: socket %x, type %d", socket, type );
#endif
    t = type;
    fd = socket;
    e = NoError;
    setFlags( IO_Sequential );
    setStatus( IO_Ok );
    open( IO_ReadWrite );
    fetchConnectionParameters();
}


/*! \reimp

  Opens the socket using the specified QIODevice file \a mode.  This function
  is called from the QSocketDevice constructors and from the setSocket()
  function.  You should not call it yourself.

  \sa close().
*/
bool QSocketDevice::open( int mode )
{
    if ( isOpen() || !isValid() )
	return FALSE;
#if defined(QSOCKETDEVICE_DEBUG)
    qDebug( "QSocketDevice::open: mode %x", mode );
#endif
    setMode( mode & IO_ReadWrite );
    setState( IO_Open );
    return TRUE;
}


/*! \reimp

  QSocketDevice does not buffer at all, so this is a no-op.
*/
void QSocketDevice::flush()
{
}


/*! \reimp

  The size is meaningless for a socket, therefore this function returns 0.
*/
uint QSocketDevice::size() const
{
    return 0;
}


/*! \reimp

  The read/write index is meaningless for a socket, therefore
  this function returns 0.
*/
int QSocketDevice::at() const
{
    return 0;
}


/*! \reimp

  The read/write index is meaningless for a socket, therefore
  this function does nothing and returns TRUE.
*/
bool QSocketDevice::at( int )
{
    return TRUE;
}


/*! \reimp

  The read/write index is meaningless for a socket, therefore
  this always returns FALSE.
*/
bool QSocketDevice::atEnd() const
{
    return FALSE;
}


/*! \reimp

  \warning getch() is implemented as a one-byte readBlock(), so it
  may be very slow if you call it more than a few times.

  \sa putch() readBlock()
*/
int QSocketDevice::getch()
{
    char buf[2];
    return  readBlock(buf,1) == 1 ? buf[0] : -1;
}


/*! \reimp

  \warning putch() is implemented as a one-byte writeBlock(), so it
  may be very slow if you call it more than a few times.

  \sa getch()
*/
int QSocketDevice::putch( int ch )
{
    char buf[2];
    buf[0] = ch;
    return writeBlock(buf, 1) == 1 ? ch : -1;
}


/*! \reimp

  This implementation of ungetch returns -1 (error).  A socket is a sequential
  device and does not allow any ungetch operation.
*/
int QSocketDevice::ungetch( int )
{
    return -1;
}


/*!
  Returns TRUE if the address of this socket can be used by other
  sockets at the same time, and FALSE if this socket claims exclusive
  ownership.

  \sa setAddressReusable()
*/
bool QSocketDevice::addressReusable() const
{
    return option( ReuseAddress );
}


/*!
  Sets the address of this socket to be usable by other sockets too
  if \a enable is TRUE, and to be used exclusively by this socket if
  \a enable is FALSE.

  When a socket is reusable, other sockets can use the same port
  number (and IP address), which is in general good.  Of course other
  sockets cannot use the same (address,port,peer-address,peer-port)
  4-tuple as this socket, so there is no risk of confusing the two TCP
  connections.

  \sa addressReusable()
*/
void QSocketDevice::setAddressReusable( bool enable )
{
    setOption( ReuseAddress, enable );
}


/*!
  Returns the size of the OS receive buffer.

  \sa setReceiveBufferSize()
*/
int QSocketDevice::receiveBufferSize() const
{
    return option( ReceiveBuffer );
}


/*!
  Sets the size of the OS receive buffer to \a size.

  The OS receive buffer size effectively limits two things: How much
  data can be in transit at any one moment, and how much data can be
  received in one iteration of the main event loop.

  The default is OS-dependent.  A socket that receives large amounts
  of data is probably best off with a buffer size of 49152.
*/
void QSocketDevice::setReceiveBufferSize( uint size )
{
    setOption( ReceiveBuffer, size );
}


/*!
  Returns the size of the OS send buffer.

  \sa setSendBufferSize()
*/
int QSocketDevice::sendBufferSize() const
{
    return option( SendBuffer );
}


/*!
  Sets the size of the OS send buffer to \a size.

  The OS send buffer size effectively limits how much data can be in
  transit at any one moment.

  The default is OS-dependent.  A socket that sends large amounts of
  data is probably best off with a buffer size of 49152.
*/
void QSocketDevice::setSendBufferSize( uint size )
{
    setOption( SendBuffer, size );
}


/*!
  Returns the port number of this socket device. This may be 0 for a while,
  but is set to something sensible when there is a sensible value it can have.
*/
Q_UINT16 QSocketDevice::port() const
{
    return p;
}


/*!
  Returns the address of this socket device.  This may be 0.0.0.0 for a while,
  but is set to something sensible when there is a sensible value it can have.
*/
QHostAddress QSocketDevice::address() const
{
    return a;
}


/*!
  Returns the first error seen.
*/
QSocketDevice::Error QSocketDevice::error() const
{
    return e;
}


/*!
  Allows subclasses to set the error state.
*/
void QSocketDevice::setError( Error err )
{
    e = err;
}
#endif //QT_NO_NETWORK
