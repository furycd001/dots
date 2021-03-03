/****************************************************************************
** $Id: qt/src/network/qftp.cpp   2.3.2   edited 2001-07-26 $
**
** Implementation of QFtp class.
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

#include "qftp.h"

#ifndef QT_NO_NETWORKPROTOCOL_FTP

#include "qurlinfo.h"
#include "qurloperator.h"
#include "qstringlist.h"
#include "qregexp.h"
#include "qtimer.h"

//#define QFTP_DEBUG
//#define QFTP_COMMANDSOCKET_DEBUG

/*!
  \class QFtp qftp.h
  \brief The QFtp class implements the FTP protocol

  \module network

  The QFtp class implements the FTP protocol. This class
  is derived from QNetworkProtocol and can be
  used with QUrlOperator. In fact, you normally will not
  use the QFtp class directly, but rather use it through
  the QUrlOperator like

  \code
  QUrlOperator op( "ftp://ftp.trolltech.com" );
  op.listChildren();
  \endcode

  If you really need to use QFtp directly, don't forget
  to set the QUrlOperator on which it works using
  setUrl().

  See also the <a href="network.html">Qt Network Documentation</a>

  \sa QNetworkProtocol, QUrlOperator
*/


/*!
  Constructs a QFtp object.
*/

QFtp::QFtp()
    : QNetworkProtocol(), connectionReady( FALSE ),
      passiveMode( FALSE ), startGetOnFail( FALSE ),
      errorInListChildren( FALSE )
{
#if defined(QFTP_DEBUG)
    qDebug( "QFtp::QFtp" );
#endif
    commandSocket = new QSocket( this, "command socket" );
    dataSocket = new QSocket( this, "data socket" );

    connect( commandSocket, SIGNAL( hostFound() ),
	     this, SLOT( hostFound() ) );
    connect( commandSocket, SIGNAL( connected() ),
	     this, SLOT( connected() ) );
    connect( commandSocket, SIGNAL( connectionClosed() ),
	     this, SLOT( closed() ) );
    connect( commandSocket, SIGNAL( readyRead() ),
	     this, SLOT( readyRead() ) );
    connect( commandSocket, SIGNAL( error( int ) ),
	     this, SLOT( error( int ) ) );
    connect( dataSocket, SIGNAL( hostFound() ),
	     this, SLOT( dataHostFound() ) );
    connect( dataSocket, SIGNAL( connected() ),
	     this, SLOT( dataConnected() ) );
    connect( dataSocket, SIGNAL( connectionClosed() ),
	     this, SLOT( dataClosed() ) );
    connect( dataSocket, SIGNAL( readyRead() ),
	     this, SLOT( dataReadyRead() ) );
    connect( dataSocket, SIGNAL( bytesWritten( int ) ),
	     this, SLOT( dataBytesWritten( int ) ) );
}

/*!
  Destructor
*/

QFtp::~QFtp()
{
#if defined(QFTP_DEBUG)
    qDebug( "QFtp::~QFtp" );
#endif

    close();
    delete commandSocket;
    delete dataSocket;
}

/*!
  \reimp
*/

void QFtp::operationListChildren( QNetworkOperation *op )
{
    op->setState( StInProgress );
    errorInListChildren = FALSE;
#if defined(QFTP_DEBUG)
    qDebug( "QFtp: switch command socket to passive mode!" );
#endif
#if defined(QFTP_COMMANDSOCKET_DEBUG)
    qDebug( "QFtp S: PASV" );
#endif
    commandSocket->writeBlock( "PASV\r\n", strlen( "PASV\r\n") );
}

/*!
  \reimp
*/

void QFtp::operationMkDir( QNetworkOperation *op )
{
    op->setState( StInProgress );
    QString cmd( "MKD " + op->arg( 0 ) + "\r\n" );
    if ( QUrl::isRelativeUrl( op->arg( 0 ) ) )
	cmd = "MKD " + QUrl( *url(), op->arg( 0 ) ).path() + "\r\n";
#if defined(QFTP_COMMANDSOCKET_DEBUG)
    qDebug( "QFtp S: %s", cmd.latin1() );
#endif
    commandSocket->writeBlock( cmd, cmd.length() );
}

/*!
  \reimp
*/

void QFtp::operationRemove( QNetworkOperation *op )
{
    // put the operation in StWaiting state first until the CWD command
    // succeeds
    op->setState( StWaiting );
    QString path = url()->path().isEmpty() ? QString( "/" ) : url()->path();
    QString cmd = "CWD " + path + "\r\n";
#if defined(QFTP_COMMANDSOCKET_DEBUG)
    qDebug( "QFtp S: %s", cmd.latin1() );
#endif
    commandSocket->writeBlock( cmd.latin1(), cmd.length() );
}

/*!
  \reimp
*/

void QFtp::operationRename( QNetworkOperation *op )
{
    // put the operation in StWaiting state first until the CWD command
    // succeeds
    op->setState( StWaiting );
    QString path = url()->path().isEmpty() ? QString( "/" ) : url()->path();
    QString cmd = "CWD " + path + "\r\n";
#if defined(QFTP_COMMANDSOCKET_DEBUG)
    qDebug( "QFtp S: %s", cmd.latin1() );
#endif
    commandSocket->writeBlock( cmd.latin1(), cmd.length() );
}

/*!
  \reimp
*/

void QFtp::operationGet( QNetworkOperation *op )
{
    op->setState( StInProgress );
#if defined(QFTP_COMMANDSOCKET_DEBUG)
    qDebug( "QFtp S: TYPE I" );
#endif
    commandSocket->writeBlock( "TYPE I\r\n", 8 );
}

/*!
  \reimp
*/

void QFtp::operationPut( QNetworkOperation *op )
{
    op->setState( StInProgress );
#if defined(QFTP_COMMANDSOCKET_DEBUG)
    qDebug( "QFtp S: TYPE I" );
#endif
    commandSocket->writeBlock( "TYPE I\r\n", 8 );
}

/*!
  \reimp
*/

bool QFtp::checkConnection( QNetworkOperation * )
{
#if defined(QFTP_DEBUG)
    qDebug( "QFtp (" + url()->toString() + "): checkConnection" );
#endif
    if ( commandSocket->isOpen() && connectionReady && !passiveMode ) {
#if defined(QFTP_DEBUG)
	qDebug( "QFtp: connection ok!" );
#endif
	return TRUE;
    }
    if ( commandSocket->isOpen() ) {
// #if defined(QFTP_DEBUG)
// 	qDebug( "QFtp: command socket open but connection not ok!" );
// #endif
	return FALSE;
    }
    if ( commandSocket->state() == QSocket::Connecting ) {
#if defined(QFTP_DEBUG)
	qDebug( "QFtp::checkConnection(): already trying to connect" );
#endif
	return FALSE;
    }

    connectionReady = FALSE;
    commandSocket->connectToHost( url()->host(),
				  url()->port() != -1 ? url()->port() : 21 );

#if defined(QFTP_DEBUG)
	qDebug( "QFtp: try connecting!" );
#endif
    return FALSE;
}

/*!
  Closes the command and data connection to the FTP server
*/

void QFtp::close()
{
#if defined(QFTP_DEBUG)
    qDebug( "QFtp:: close and quit" );
#endif

    if ( dataSocket->isOpen() ) {
	dataSocket->close();
    }
    if ( commandSocket->isOpen() ) {
#if defined(QFTP_COMMANDSOCKET_DEBUG)
	qDebug( "QFtp S: quit" );
#endif
 	commandSocket->writeBlock( "quit\r\n", strlen( "quit\r\n" ) );
 	commandSocket->close();
    }
}

/*!
  \reimp
*/

int QFtp::supportedOperations() const
{
    return OpListChildren | OpMkdir | OpRemove | OpRename | OpGet | OpPut;
}

/*!
  Parses \a buffer, which is one line of a directory list
  which came from the FTP server, and sets the
  values which have been parsed to \a info.
*/

void QFtp::parseDir( const QString &buffer, QUrlInfo &info )
{
    QStringList lst = QStringList::split( " ", buffer );

    if ( lst.count() < 9 )
	return;

    QString tmp;

    // permissions
    tmp = lst[ 0 ];

    if ( tmp[ 0 ] == QChar( 'd' ) ) {
	info.setDir( TRUE );
	info.setFile( FALSE );
	info.setSymLink( FALSE );
    } else if ( tmp[ 0 ] == QChar( '-' ) ) {
	info.setDir( FALSE );
	info.setFile( TRUE );
	info.setSymLink( FALSE );
    } else if ( tmp[ 0 ] == QChar( 'l' ) ) {
	info.setDir( TRUE ); // #### todo
	info.setFile( FALSE );
	info.setSymLink( TRUE );
    } else {
	return;
    }

    static int user = 0;
    static int group = 1;
    static int other = 2;
    static int readable = 0;
    static int writable = 1;
    static int executable = 2;

    bool perms[ 3 ][ 3 ]; 
    perms[0][0] = (tmp[ 1 ] == 'r');
    perms[0][1] = (tmp[ 2 ] == 'w');
    perms[0][2] = (tmp[ 3 ] == 'x');
    perms[1][0] = (tmp[ 4 ] == 'r');
    perms[1][1] = (tmp[ 5 ] == 'w');
    perms[1][2] = (tmp[ 6 ] == 'x');
    perms[2][0] = (tmp[ 7 ] == 'r');
    perms[2][1] = (tmp[ 8 ] == 'w');
    perms[2][2] = (tmp[ 9 ] == 'x');

    // owner
    tmp = lst[ 2 ];
    info.setOwner( tmp );

    // group
    tmp = lst[ 3 ];
    info.setGroup( tmp );

    // ### not correct
    info.setWritable( ( url()->user() == info.owner() && perms[ user ][ writable ] ) ||
	perms[ other ][ writable ] );
    info.setReadable( ( url()->user() == info.owner() && perms[ user ][ readable ] ) ||
	perms[ other ][ readable ] );

    int p = 0;
    if ( perms[ user ][ readable ] )
	p |= QFileInfo::ReadUser;
    if ( perms[ user ][ writable ] )
	p |= QFileInfo::WriteUser;
    if ( perms[ user ][ executable ] )
	p |= QFileInfo::ExeUser;
    if ( perms[ group ][ readable ] )
	p |= QFileInfo::ReadGroup;
    if ( perms[ group ][ writable ] )
	p |= QFileInfo::WriteGroup;
    if ( perms[ group ][ executable ] )
	p |= QFileInfo::ExeGroup;
    if ( perms[ other ][ readable ] )
	p |= QFileInfo::ReadOther;
    if ( perms[ other ][ writable ] )
	p |= QFileInfo::WriteOther;
    if ( perms[ other ][ executable ] )
	p |= QFileInfo::ExeOther;
    info.setPermissions( p );

    // size
    tmp = lst[ 4 ];
    info.setSize( tmp.toInt() );

    // date, time #### todo
    QDate date;
    int month = 1, day;
    for ( uint i = 1; i <= 12; ++i ) {
	if ( date.monthName( i ) == lst[ 5 ] ) {
	    month = i;
	    break;
	}
    }
    day = lst[ 6 ].toInt();

    if ( lst[ 7 ].contains( ":" ) ) {
	QTime time( lst[ 7 ].left( 2 ).toInt(), lst[ 7 ].right( 2 ).toInt() );
	date = QDate( QDate::currentDate().year(), month, day );
	info.setLastModified( QDateTime( date, time ) );
    } else {
	int year = lst[ 7 ].toInt();
	date = QDate( year, month, day );
	info.setLastModified( QDateTime( date, QTime() ) );
    }

    // name
    if ( info.isSymLink() )
	info.setName( lst[ 8 ].stripWhiteSpace() );
    else {
	QString n;
	for ( uint i = 8; i < lst.count(); ++i )
	    n += lst[ i ] + " ";
	n = n.stripWhiteSpace();
	info.setName( n );
    }
}

/*!
 \internal
*/

void QFtp::hostFound()
{
    if ( url() )
	emit connectionStateChanged( ConHostFound, tr( "Host %1 found" ).arg( url()->host() ) );
    else
	emit connectionStateChanged( ConHostFound, tr( "Host found" ) );
}

/*!
 \internal
*/

void QFtp::connected()
{
    if ( url() )
	emit connectionStateChanged( ConConnected, tr( "Connected to host %1" ).arg( url()->host() ) );
    else
	emit connectionStateChanged( ConConnected, tr( "Connected to host" ) );
}

/*!
 \internal
*/

void QFtp::closed()
{
    if ( url() )
	emit connectionStateChanged( ConClosed, tr( "Connection to %1 closed" ).arg( url()->host() ) );
    else
	emit connectionStateChanged( ConClosed, tr( "Connection closed" ) );
}

/*!
  If data arrived on the command socket, this slot is called. It looks at
  the data and passes it to the correct method which can handle it
*/

void QFtp::readyRead()
{
    QCString s;
    int bytesAvailable = commandSocket->bytesAvailable();
    s.resize( bytesAvailable + 1 );
    commandSocket->readBlock( s.data(), bytesAvailable );
    s[ bytesAvailable ] = '\0';

    if ( !url() )
	return;

    bool ok = FALSE;
    int code = s.left( 3 ).toInt( &ok );
    if ( !ok )
	return;

#if defined(QFTP_DEBUG)
    if ( s.size() < 400 )
	qDebug( "QFtp: readyRead; %s", s.data() );
#endif
#if defined(QFTP_COMMANDSOCKET_DEBUG)
    if ( s.size() < 400 )
	qDebug( "QFtp R: %s", s.data() );
    else
	qDebug( "QFtp R: More than 400 bytes received. Not printing." );
#endif

    if ( s.left( 1 ) == "1" )
	okButTryLater( code, s );
    else if ( s.left( 1 ) == "2" )
	okGoOn( code, s );
    else if ( s.left( 1 ) == "3" )
	okButNeedMoreInfo( code, s );
    else if ( s.left( 1 ) == "4" )
	errorForNow( code, s );
    else if ( s.left( 1 ) == "5" )
	errorForgetIt( code, s );
    // else strange things happen...
}

/*!
  Handles responses from the server which say that
  currently something couldn't be done and it should be tried later again.
*/

void QFtp::okButTryLater( int, const QCString & )
{
    if ( operationInProgress() && operationInProgress()->operation() == OpPut &&
	    dataSocket && dataSocket->isOpen() ) {
	putToWrite = operationInProgress()->rawArg(1).size();
	putWritten = 0;
	dataSocket->writeBlock( operationInProgress()->rawArg(1), putToWrite );
    }
}

/*!
  Handles responses from the server which are the result of a success
*/

void QFtp::okGoOn( int code, const QCString &data )
{
    switch ( code ) {
    case 213: { // state of a file (size and so on)
	if ( operationInProgress() ) {
	    if ( operationInProgress()->operation() == OpGet ) {
		// cut off the "213 "
		QString s( data );
		s.remove( 0, 4 );
		s = s.simplifyWhiteSpace();
		getTotalSize = s.toInt();
		operationInProgress()->setState( StInProgress );
#if defined(QFTP_COMMANDSOCKET_DEBUG)
		qDebug( "QFtp S: PASV" );
#endif
		commandSocket->writeBlock( "PASV\r\n", strlen( "PASV\r\n") );
	    }
	}
    } break;
    case 200: { // last command ok
	if ( operationInProgress() ) {
	    if ( operationInProgress()->operation() == OpPut ) {
		operationInProgress()->setState( StInProgress );
#if defined(QFTP_COMMANDSOCKET_DEBUG)
		qDebug( "QFtp S: PASV" );
#endif
		commandSocket->writeBlock( "PASV\r\n", strlen( "PASV\r\n") );
	    } else if ( operationInProgress()->operation() == OpGet ) {
		startGetOnFail = TRUE;
		getTotalSize = -1;
		getDoneSize = 0;
		QString cmd = "SIZE "+ QUrl( operationInProgress()->arg( 0 ) ).path() + "\r\n";
#if defined(QFTP_COMMANDSOCKET_DEBUG)
		qDebug( "QFtp S: %s", cmd.latin1() );
#endif
		commandSocket->writeBlock( cmd.latin1(), cmd.length() );
	    }
	}
    } break;
    case 220: { // expect USERNAME
#if defined(QFTP_DEBUG)
	qDebug( "QFtp: start login porcess" );
#endif
	QString user = url()->user().isEmpty() ? QString( "anonymous" ) : url()->user();
	QString cmd = "USER " + user + "\r\n";
#if defined(QFTP_DEBUG)
	qDebug( "QFtp: write to command socket: \"%s\"", cmd.latin1() );
#endif

#if defined(QFTP_COMMANDSOCKET_DEBUG)
	qDebug( "QFtp S: %s", cmd.latin1() );
#endif
	commandSocket->writeBlock( cmd, cmd.length() );
	connectionReady = FALSE;
    } break;
    case 230: // succesfully logged in
#if defined(QFTP_DEBUG)
	qDebug( "QFtp: connection is ready, successful logged int!" );
#endif
	connectionReady = TRUE;
	break;
    case 227: { // open the data connection (passive mode)
#if defined(QFTP_DEBUG)
	qDebug( "QFtp: command socket switched to passive mode, open data connection" );
#endif
	QCString s = data;
	int i = s.find( "(" );
	int i2 = s.find( ")" );
	s = s.mid( i + 1, i2 - i - 1 );
	if ( dataSocket->isOpen() )
	    dataSocket->close();
	QStringList lst = QStringList::split( ',', s );
	int port = ( lst[ 4 ].toInt() << 8 ) + lst[ 5 ].toInt();
	dataSocket->connectToHost( lst[ 0 ] + "." + lst[ 1 ] + "." + lst[ 2 ] + "." + lst[ 3 ], port );
    } break;
    case 250: { // file operation succesfully
	if ( operationInProgress() && !passiveMode &&
	     operationInProgress()->operation() == OpListChildren ) { // list dir
	    if ( !errorInListChildren ) {
		operationInProgress()->setState( StInProgress );
#if defined(QFTP_DEBUG)
		qDebug( "QFtp: list children (command socket is passive!" );
#endif
#if defined(QFTP_COMMANDSOCKET_DEBUG)
		qDebug( "QFtp S: LIST" );
#endif
		commandSocket->writeBlock( "LIST\r\n", strlen( "LIST\r\n" ) );
		emit start( operationInProgress() );
		passiveMode = TRUE;
	    }
	} else if ( operationInProgress() &&
		    operationInProgress()->operation() == OpRename ) { // rename successfull
	    if ( operationInProgress()->state() == StWaiting ) {
		operationInProgress()->setState( StInProgress );
		QString oldname = operationInProgress()->arg( 0 );
		QString newname = operationInProgress()->arg( 1 );
		QString cmd( "RNFR " + oldname + "\r\n" );
#if defined(QFTP_COMMANDSOCKET_DEBUG)
		qDebug( "QFtp S: %s", cmd.latin1() );
#endif
		commandSocket->writeBlock( cmd, cmd.length() );
		cmd = "RNTO " + newname + "\r\n";
#if defined(QFTP_COMMANDSOCKET_DEBUG)
		qDebug( "QFtp S: %s", cmd.latin1() );
#endif
		commandSocket->writeBlock( cmd, cmd.length() );
	    } else {
		operationInProgress()->setState( StDone );
		emit itemChanged( operationInProgress() );
		emit finished( operationInProgress() );
	    }
	} else if ( operationInProgress() &&
		    operationInProgress()->operation() == OpRemove ) { // remove or cwd successful
	    if ( operationInProgress()->state() == StWaiting ) {
		operationInProgress()->setState( StInProgress );
		QString name = QUrl( operationInProgress()->arg( 0 ) ).path();
		QString cmd( "DELE " + name + "\r\n" );
#if defined(QFTP_COMMANDSOCKET_DEBUG)
		qDebug( "QFtp S: %s", cmd.latin1() );
#endif
		commandSocket->writeBlock( cmd, cmd.length() );
	    } else {
		operationInProgress()->setState( StDone );
		emit removed( operationInProgress() );
		emit finished( operationInProgress() );
	    }
	}
    } break;
    case 226: // listing directory (in passive mode) finished and data socket closing
	break;
    case 257: { // mkdir worked
	if ( operationInProgress() && operationInProgress()->operation() == OpMkdir ) {
	    operationInProgress()->setState( StDone );
	    // ######## todo get correct info
	    QUrlInfo inf( operationInProgress()->arg( 0 ), 0, "", "", 0, QDateTime(),
			  QDateTime(), TRUE, FALSE, FALSE, TRUE, TRUE, TRUE );
	    emit newChild( inf, operationInProgress() );
	    emit createdDirectory( inf, operationInProgress() );
	    reinitCommandSocket();
	    emit finished( operationInProgress() );
	}
    } break;
    }
}

/*!
  Handles responses from the server which needs more information about something
*/

void QFtp::okButNeedMoreInfo( int code, const QCString & )
{
    switch ( code ) {
    case 331: {		// expect PASSWORD
#if defined(QFTP_DEBUG)
	qDebug( "QFtp: write password" );
#endif
	QString pass = url()->password().isEmpty() ? QString( "info@trolltech.com" ) : url()->password();
	QString cmd = "PASS " + pass + "\r\n";
#if defined(QFTP_DEBUG)
	qDebug( "QFtp: write to command socket: \"%s\"", cmd.latin1() );
#endif
#if defined(QFTP_COMMANDSOCKET_DEBUG)
	qDebug( "QFtp S: %s", cmd.latin1() );
#endif
	commandSocket->writeBlock( cmd, cmd.length() );
	connectionReady = FALSE;
    } break;
    }
}

/*!
  Handles error messages from the server
*/

void QFtp::errorForNow( int, const QCString & )
{
}

/*!
  Handles fatal error messages from the server (after this nothing more can't be done)
*/

void QFtp::errorForgetIt( int code, const QCString &data )
{
    if ( startGetOnFail ) {
	operationInProgress()->setState( StInProgress );
#if defined(QFTP_COMMANDSOCKET_DEBUG)
	qDebug( "QFtp S: PASV" );
#endif
	commandSocket->writeBlock( "PASV\r\n", strlen( "PASV\r\n") );
	startGetOnFail = FALSE;
	return;
    }

    switch ( code ) {
    case 530: { // Login incorrect
	close();
	QString msg( tr( "Login Incorrect" ) );
	QNetworkOperation *op = operationInProgress();
	if ( op ) {
	    op->setProtocolDetail( msg );
	    op->setState( StFailed );
	    op->setErrorCode( (int)ErrLoginIncorrect );
	}
	reinitCommandSocket();
	clearOperationQueue();
	emit finished( op );
    } break;
    case 550: { // no such file or directory
	QString msg( data.mid( 4 ) );
	msg = msg.simplifyWhiteSpace();
	QNetworkOperation *op = operationInProgress();
	if ( op ) {
	    op->setProtocolDetail( msg );
	    op->setState( StFailed );
	    op->setErrorCode( (int)ErrFileNotExisting );
	}
	errorInListChildren = TRUE;
	reinitCommandSocket();
	emit finished( op );
    } break;
    case 553: { // permission denied
	QString msg( data.mid( 4 ) );
	msg = msg.simplifyWhiteSpace();
	QNetworkOperation *op = operationInProgress();
	if ( op ) {
	    op->setProtocolDetail( msg );
	    op->setState( StFailed );
	    op->setErrorCode( (int)ErrPermissionDenied );
	}
	reinitCommandSocket();
	emit finished( op );
    } break;
    }
}

/*!
  \internal
*/

void QFtp::dataHostFound()
{
}

/*!
  Some operations require a data connection to the server. If this connection
  could be opened, this method handles the rest.
*/

void QFtp::dataConnected()
{
#if defined(QFTP_DEBUG)
    qDebug( "QFtp: data socket connected" );
#endif
    if ( !operationInProgress() )
	return;
    switch ( operationInProgress()->operation() ) {
    case OpListChildren: { // change dir first
	QString path = url()->path().isEmpty() ? QString( "/" ) : url()->path();
	QString cmd = "CWD " + path + "\r\n";
#if defined(QFTP_DEBUG)
	qDebug( "QFtp: list children (data socket), to command socket write \"%s\"", cmd.latin1() );
#endif
#if defined(QFTP_COMMANDSOCKET_DEBUG)
	qDebug( "QFtp S: %s", cmd.latin1() );
#endif
	commandSocket->writeBlock( cmd.latin1(), cmd.length() );
    } break;
    case OpGet: { // retrieve file
	if ( !operationInProgress() || operationInProgress()->arg( 0 ).isEmpty() ) {
	    qWarning( "no filename" );
	    break;
	}
	QString cmd = "RETR " + QUrl( operationInProgress()->arg( 0 ) ).path() + "\r\n";
#if defined(QFTP_DEBUG)
	qDebug( "QFtp: get (data socket), to command socket write \"%s\"", cmd.latin1() );
#endif
#if defined(QFTP_COMMANDSOCKET_DEBUG)
	qDebug( "QFtp S: %s", cmd.latin1() );
#endif
	commandSocket->writeBlock( cmd.latin1(), cmd.length() );
	emit dataTransferProgress( 0, getTotalSize, operationInProgress() );
    } break;
    case OpPut: { // upload file
	if ( !operationInProgress() || operationInProgress()->arg( 0 ).isEmpty() ) {
	    qWarning( "no filename" );
	    break;
	}
	QString cmd = "STOR " + QUrl( operationInProgress()->arg( 0 ) ).path() + "\r\n";
#if defined(QFTP_DEBUG)
	qDebug( "QFtp: put (data socket), to command socket write \"%s\"", cmd.latin1() );
#endif
#if defined(QFTP_COMMANDSOCKET_DEBUG)
	qDebug( "QFtp S: %s", cmd.latin1() );
#endif
	commandSocket->writeBlock( cmd.latin1(), cmd.length() );
    } break;
    case OpMkdir: {
    } break;
    case OpRemove: {
    } break;
    case OpRename: {
    } break;
    }
}

/*!
  Called when the data connection has been closed
*/

void QFtp::dataClosed()
{
    // switch back to ASCII mode
#if defined(QFTP_COMMANDSOCKET_DEBUG)
	qDebug( "QFtp S: TYPE A" );
#endif
    commandSocket->writeBlock( "TYPE A\r\n", 8 );

    passiveMode = FALSE;

    reinitCommandSocket();

    if ( !errorInListChildren && operationInProgress() ) {
	operationInProgress()->setState( StDone );
	emit finished( operationInProgress() );
    }
}

/*!
  This method is called when new data arrived on the data socket.
*/

void QFtp::dataReadyRead()
{
#if defined(QFTP_DEBUG)
    qDebug( "QFtp: read on data socket" );
#endif
    if ( !operationInProgress() )
	return;

    switch ( operationInProgress()->operation() ) {
    case OpListChildren: { // parse directory entry
	if ( !dataSocket->canReadLine() )
	    break;
	while ( dataSocket->canReadLine() ) {
	    QString ss = dataSocket->readLine();
	    ss = ss.stripWhiteSpace();
	    QUrlInfo inf;
	    parseDir( ss, inf );
	    if ( !inf.name().isEmpty() ) {
		if ( url() ) {
		    QRegExp filt( url()->nameFilter(), FALSE, TRUE );
		    if ( inf.isDir() || filt.match( inf.name() ) != -1 ) {
			emit newChild( inf, operationInProgress() );
		    }
		}
	    }
	}
    } break;
    case OpGet: {
	QByteArray s;
	int bytesAvailable = dataSocket->bytesAvailable();
	s.resize( bytesAvailable );
	int bytesRead = dataSocket->readBlock( s.data(), bytesAvailable );
	if ( bytesRead <= 0 )
	    break; // error
	if ( bytesRead != bytesAvailable )
	    s.resize( bytesRead );
	emit data( s, operationInProgress() );
	getDoneSize += bytesRead;
	emit dataTransferProgress( getDoneSize, getTotalSize, operationInProgress() );
	// qDebug( "%s", s.data() );
    } break;
    case OpMkdir: {
    } break;
    case OpRemove: {
    } break;
    case OpRename: {
    } break;
    case OpPut: {
    } break;
    }
}

/*!
  This method is called, when \a nbytes have been successfully written to the
  data socket.
*/

void QFtp::dataBytesWritten( int nbytes )
{
    putWritten += nbytes;
    emit dataTransferProgress( putWritten, putToWrite, operationInProgress() );
    if ( putWritten >= putToWrite ) {
	dataSocket->close();
	QTimer::singleShot( 1, this, SLOT( dataClosed() ) );
    }
}

/*!
  \internal
  Reinitializes the command socket
*/

void QFtp::reinitCommandSocket()
{
}

/*!
  \reimp
*/

void QFtp::error( int code )
{
    if ( code == QSocket::ErrHostNotFound ||
	 code == QSocket::ErrConnectionRefused ) {
	if ( dataSocket->isOpen() )
	    dataSocket->close();
	QNetworkOperation *op = operationInProgress();
	if ( op ) {
	    QString msg = tr( "Host not found or couldn't connect to: \n" + url()->host() );
	    op->setState( StFailed );
	    op->setProtocolDetail( msg );
	    op->setErrorCode( (int)ErrHostNotFound );
	    clearOperationQueue();
	    emit finished( op );
	}
    }
}

#endif // QT_NO_NETWORKPROTOCOL_FTP
