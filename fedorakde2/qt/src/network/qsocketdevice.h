/****************************************************************************
** $Id: qt/src/network/qsocketdevice.h   2.3.2   edited 2001-01-26 $
**
** Definition of QSocketDevice class.
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

#ifndef QSOCKETDEVICE_H
#define QSOCKETDEVICE_H

#ifndef QT_H
#include "qiodevice.h"
#include "qstring.h"
#include "qhostaddress.h"
#endif // QT_H

#ifndef QT_NO_NETWORK
class QSocketDevicePrivate;


class  Q_EXPORT QSocketDevice: public QIODevice
{
public:
    enum Type { Stream, Datagram };

    QSocketDevice( Type type = Stream );
    QSocketDevice( int socket, Type type );
    virtual ~QSocketDevice();

    bool	 isValid() const;
    Type	 type() const;
    int		 socket() const;
    virtual void setSocket( int socket, Type type );

    bool	 open( int mode );
    void	 close();
    void	 flush();

    // Implementation of QIODevice abstract virtual functions
    uint	 size() const;
    int		 at() const;
    bool	 at( int );
    bool	 atEnd() const;

    bool	 blocking() const;
    virtual void setBlocking( bool );

    bool	 addressReusable() const;
    virtual void setAddressReusable( bool );

    int		 receiveBufferSize() const;
    virtual void setReceiveBufferSize( uint );
    int		 sendBufferSize() const;
    virtual void setSendBufferSize( uint );

    virtual bool connect( const QHostAddress &, Q_UINT16 );

    virtual bool bind( const QHostAddress &, Q_UINT16 );
    virtual bool listen( int backlog );
    virtual int	 accept();

    int		 bytesAvailable() const;
    int		 waitForMore( int msecs ) const;
    int		 readBlock( char *data, uint maxlen );
    int		 writeBlock( const char *data, uint len );
    virtual int  writeBlock( const char *data, uint len,
			    const QHostAddress & host, Q_UINT16 port );

    int		 getch();
    int		 putch( int );
    int		 ungetch(int);

    Q_UINT16	 port() const;
    Q_UINT16	 peerPort() const;
    QHostAddress address() const;
    QHostAddress peerAddress() const;

    enum Error { NoError, AlreadyBound, Inaccessible, NoResources,
		 Bug, Impossible, NoFiles, ConnectionRefused,
		 NetworkFailure, UnknownError };
    Error 	 error() const;

protected:
    void setError( Error err );

private:
    int fd;
    Type t;
    Q_UINT16 p;
    QHostAddress a;
    Q_UINT16 pp;
    QHostAddress pa;
    QSocketDevice::Error e;
    QSocketDevicePrivate * d;

    enum Option { Broadcast, ReceiveBuffer, ReuseAddress, SendBuffer };

    int		 option( Option ) const;
    virtual void setOption( Option, int );

    void	 fetchConnectionParameters();

    static void init();

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    QSocketDevice( const QSocketDevice & );
    QSocketDevice &operator=( const QSocketDevice & );
#endif
};

#endif // QT_NO_NETWORK
#endif // QSOCKETDEVICE_H
