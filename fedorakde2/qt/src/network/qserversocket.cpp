/****************************************************************************
** $Id: qt/src/network/qserversocket.cpp   2.3.2   edited 2001-07-10 $
**
** Implementation of QServerSocket class.
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

#include "qserversocket.h"

#ifndef QT_NO_NETWORK
class QServerSocketPrivate {
public:
    QServerSocketPrivate(): s(0), n(0) {}
    ~QServerSocketPrivate() { delete n; delete s; }
    QSocketDevice *s;
    QSocketNotifier *n;
};


/*!
  \class QServerSocket qserversocket.h
  \brief The QServerSocket class provides a TCP-based server.

  \module network

  This class is a convenience class for accepting incoming TCP
  connections.  You can specify port or have QSocketServer pick one,
  and listen on just one address or on all the addresses of the
  machine.

  The API is very simple: Subclass it, call the constructor of your
  choice, and implement newConnection() to handle new incoming
  connections.  There is nothing more to do.

  (Note that due to lack of support in the underlying APIs,
  QServerSocket cannot accept or reject connections conditionally.)

  \sa QSocket, QSocketDevice, QHostAddress, QSocketNotifier
*/


/*!
  Creates a server socket object, that will serve the given \a port on
  all the addresses of this host.  If \a port is 0, QServerSocket
  picks a suitable port in in a system-dependent manner. With \a backlog you
  can specify how many pending connections a server can have.

  The \a parent and \a name arguments are passed on as usual
  to the QObject constructor.

  \warning On Tru64 Unix systems a value of 0 for \a backlog means that you
  don't accept any connections at all; you should specify a value larger than
  0.
*/

QServerSocket::QServerSocket( Q_UINT16 port, int backlog,
			      QObject *parent, const char *name )
    : QObject( parent, name )
{
    d = new QServerSocketPrivate;
    init( QHostAddress(), port, backlog );
}


/*!
  Creates a server socket object, that will serve the given \a port
  on just \a address. With \a backlog you can specify how many pending
  connections a server can have.

  The \a parent and \a name arguments are passed on as usual
  to the QObject constructor.

  \warning On Tru64 Unix systems a value of 0 for \a backlog means that you
  don't accept any connections at all; you should specify a value larger than
  0.
*/

QServerSocket::QServerSocket( const QHostAddress & address, Q_UINT16 port,
			      int backlog,
			      QObject *parent, const char *name )
    : QObject( parent, name )
{
    d = new QServerSocketPrivate;
    init( address, port, backlog );
}


/*!
  Construct an empty server socket.

  This constructor in combination with setSocket() allows one to use the
  QServerSocket class as a wrapper for other socket types (e.g. Unix Domain
  Sockets under Unix).

  \sa setSocket()
*/

QServerSocket::QServerSocket( QObject *parent, const char *name )
    : QObject( parent, name )
{
    d = new QServerSocketPrivate;
}


/*!
  Tests that the construction succeeded.
*/
bool QServerSocket::ok() const
{
    return !!d->s;
}

/*!  The common bit of the constructors. */

void QServerSocket::init( const QHostAddress & address, Q_UINT16 port, int backlog )
{
    d->s = new QSocketDevice;
    if ( d->s->bind( address, port )
      && d->s->listen( backlog ) )
    {
	d->n = new QSocketNotifier( d->s->socket(), QSocketNotifier::Read,
				    this, "accepting new connections" );
	connect( d->n, SIGNAL(activated(int)),
		 this, SLOT(incomingConnection(int)) );
    } else {
	qWarning( "QServerSocket: failed to bind or listen to the socket" );
	delete d->s;
	d->s = 0;
    }
}


/*!
  Destructs the socket.

  This brutally severes any backlogged connections (connections that
  have reached the host, but not yet been completely set up by calling
  QSocketDevice::accept()).

  Existing connections continue to exist; this only affects acceptance
  of new connections.
*/

QServerSocket::~QServerSocket()
{
    delete d;
}


/*!
  \fn void QServerSocket::newConnection( int socket )

  This pure virtual function is responsible for setting up a new
  incoming connection.  \a socket is the fd of the newly accepted
  connection.
*/


void QServerSocket::incomingConnection( int )
{
    int fd = d->s->accept();
    if ( fd >= 0 )
	newConnection( fd );
}


/*!
  Returns the port number on which this object listens.  This is
  always non-zero; if you specify 0 in the constructor, QServerSocket
  picks a port itself and port() returns its number. ok() must be TRUE before
  calling this function.

  \sa address() QSocketDevice::port()
*/

Q_UINT16 QServerSocket::port() const
{
    if ( !d || !d->s )
	return 0;
    return d->s->port();
}


/*!
  Returns the operating system socket.
*/

int QServerSocket::socket() const
{
    if ( !d || !d->s )
	return -1;
    
    return d->s->socket();
}

/*!
  Returns the address on which this object listens, or 0.0.0.0 if
  this object listens on more than one address. ok() must be TRUE before
  calling this function.

  \sa port() QSocketDevice::address()
*/

QHostAddress QServerSocket::address() const
{
    if ( !d || !d->s )
	return QHostAddress();
    
    return d->s->address();
}


/*!
  Returns a pointer to the internal socket device. The returned pointer is
  null if there is no connection or pending connection.

  There is normally no need to manipulate the socket device directly since this
  class does all the necessary setup for most client or server socket
  applications.
*/

QSocketDevice *QServerSocket::socketDevice()
{
    if ( !d )
	return 0;
    
    return d->s;
}


/*!
  Sets the socket to use \a socket. bind() and listen() should already be
  called For this socket.

  This allows one to use the QServerSocket class as a wrapper for other socket
  types (e.g. Unix Domain Sockets under Unix).
*/

void QServerSocket::setSocket( int socket )
{
    delete d;
    d = new QServerSocketPrivate;
    d->s = new QSocketDevice( socket, QSocketDevice::Stream );
    d->n = new QSocketNotifier( d->s->socket(), QSocketNotifier::Read,
	       this, "accepting new connections" );
    connect( d->n, SIGNAL(activated(int)),
	     this, SLOT(incomingConnection(int)) );
}

#endif //QT_NO_NETWORK
