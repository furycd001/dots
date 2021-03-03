/****************************************************************************
** $Id: qt/examples/mail/smtp.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "smtp.h"

#include <qtextstream.h>
#include <qsocket.h>
#include <qdns.h>
#include <qtimer.h>
#include <qapplication.h>
#include <qmessagebox.h>


void replace( QString& str, const QString& before, const QString& after )
{
    if ( before.length() == 0 )
        return;

    int maxsize;
    if ( before.length() > after.length() )
        maxsize = str.length();
    else
        maxsize = ( str.length() / before.length() ) * after.length();

    QChar *buf = new QChar[maxsize + 1]; // + 1 in case maxsize is 0
    const QChar *strBuf = str.unicode();

    int prev = 0;
    int cur = 0;
    int i = 0;
    bool changed = FALSE;

    while ( (uint) (cur = str.find(before, prev)) < str.length() ) {
        if ( cur > prev ) {
            memcpy( buf + i, strBuf + prev, sizeof(QChar) * (cur - prev) );
            i += cur - prev;
        }
        if ( after.length() > 0 ) {
            memcpy( buf + i, after.unicode(), sizeof(QChar) * after.length() );
            i += after.length();
        }
        prev = cur + before.length();
        changed = TRUE;
    }

    if ( changed ) {
        memcpy( buf + i, strBuf + prev,
		sizeof(QChar) * (str.length() - prev) );
        i += str.length() - prev;
        str = QString( buf, i );
    }
    delete[] buf;
}


Smtp::Smtp( const QString &from, const QString &to,
	    const QString &subject,
	    const QString &body )
{
    socket = new QSocket( this );
    connect ( socket, SIGNAL( readyRead() ),
	      this, SLOT( readyRead() ) );
    connect ( socket, SIGNAL( connected() ),
	      this, SLOT( connected() ) );

    mxLookup = new QDns( to.mid( to.find( '@' )+1 ), QDns::Mx );
    connect( mxLookup, SIGNAL(resultsReady()),
	     this, SLOT(dnsLookupHelper()) );

    message = QString::fromLatin1( "From: " ) + from +
	      QString::fromLatin1( "\nTo: " ) + to +
	      QString::fromLatin1( "\nSubject: " ) + subject +
	      QString::fromLatin1( "\n\n" ) + body + "\n";
    replace( message, QString::fromLatin1( "\n" ),
	     QString::fromLatin1( "\r\n" ) );
    replace( message, QString::fromLatin1( "\r\n.\r\n" ),
	     QString::fromLatin1( "\r\n..\r\n" ) );

    this->from = from;
    rcpt = to;

    state = Init;
}


Smtp::~Smtp()
{
    delete t;
    delete socket;
}


void Smtp::connected()
{
    emit status( tr( "Connected to %1" ).arg( socket->peerName() ) );
}


void Smtp::readyRead()
{
    // SMTP is line-oriented
    if ( !socket->canReadLine() )
	return;

    QString responseLine;
    do {
	responseLine = socket->readLine();
	response += responseLine;
    } while( socket->canReadLine() && responseLine[3] != ' ' );
    responseLine.truncate( 3 );

    if ( state == Init && responseLine[0] == '2' ) {
	// banner was okay, let's go on
	*t << "HELO there\r\n";
	state = Mail;
    } else if ( state == Mail && responseLine[0] == '2' ) {
	// HELO response was okay (well, it has to be)
	*t << "MAIL FROM: <" << from << ">\r\n";
	state = Rcpt;
    } else if ( state == Rcpt && responseLine[0] == '2' ) {
	*t << "RCPT TO: <" << rcpt << ">\r\n";
	state = Data;
    } else if ( state == Data && responseLine[0] == '2' ) {
	*t << "DATA\r\n";
	state = Body;
    } else if ( state == Body && responseLine[0] == '3' ) {
	*t << message << ".\r\n";
	state = Quit;
    } else if ( state == Quit && responseLine[0] == '2' ) {
	*t << "QUIT\r\n";
	// here, we just close.
	state = Close;
	emit status( tr( "Message sent" ) );
    } else if ( state == Close ) {
	// we ignore it
    } else {
	// something broke.
	QMessageBox::warning( qApp->activeWindow(),
			      tr( "Qt Mail Example" ),
			      tr( "Unexpected reply from SMTP server:\n\n" ) +
			      response );
	state = Close;
    }

    response = "";

    if ( state == Close ) {
	QTimer::singleShot( 0, this, SLOT(deleteMe()) );
    }
}


void Smtp::deleteMe()
{
    delete this;
}


void Smtp::dnsLookupHelper()
{
    QValueList<QDns::MailServer> s = mxLookup->mailServers();
    if ( s.isEmpty() && mxLookup->isWorking() )
	return;

    emit status( tr( "Connecting to %1" ).arg( s.first().name ) );
    
    socket->connectToHost( s.first().name, 25 );
    t = new QTextStream( socket );
}
