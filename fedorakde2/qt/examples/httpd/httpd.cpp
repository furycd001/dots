/****************************************************************************
** $Id: qt/examples/httpd/httpd.cpp   2.3.2   edited 2001-07-10 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qserversocket.h>
#include <qapplication.h>
#include <qmainwindow.h>
#include <qtextstream.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qtextview.h>
#include <qpushbutton.h>
#include <stdlib.h>


class HttpDaemon : public QServerSocket
{
    Q_OBJECT
public:
    HttpDaemon( QObject* parent=0 ) :
	QServerSocket(8080,1,parent)
    {
	if ( !ok() ) {
	    qWarning("Failed to bind to port 8080");
	    exit(1);
	}
    }

    void newConnection( int socket )
    {
	QSocket* s = new QSocket(this);
	connect(s,SIGNAL(readyRead()),this,SLOT(readClient()));
	connect(s,SIGNAL(delayedCloseFinished()),this,SLOT(discardClient()));
	s->setSocket(socket);
	emit newConnect();
    }

signals:
    void newConnect();
    void endConnect();
    void wroteToClient();

private slots:
    void readClient()
    {
	QSocket* socket = (QSocket*)sender();
	if (socket->canReadLine()) {
	    QStringList tokens = QStringList::split(QRegExp("[ \n\r][ \n\r]*"),socket->readLine());
	    if ( tokens[0] == "GET" ) {
		QTextStream os(socket);
		os << "<h1>Nothing to see here</h1>\n";
		socket->close();
		emit wroteToClient();
	    }
	}
    }
    void discardClient()
    {
	QSocket* socket = (QSocket*)sender();
	delete socket;
	emit endConnect();
    }
};


class HttpInfo : public QVBox
{
    Q_OBJECT
public:
    HttpInfo()
    {
	HttpDaemon *httpd = new HttpDaemon( this );

	QString itext = QString(
		"This is a small httpd example.\n"
		"You can connect with your\n"
		"web browser to port %1"
	    ).arg( httpd->port() );
	QLabel *lb = new QLabel( itext, this );
	lb->setAlignment( AlignHCenter );
	infoText = new QTextView( this );
	QPushButton *quit = new QPushButton( "quit" , this );

	connect( httpd, SIGNAL(newConnect()), SLOT(newConnect()) );
	connect( httpd, SIGNAL(endConnect()), SLOT(endConnect()) );
	connect( httpd, SIGNAL(wroteToClient()), SLOT(wroteToClient()) );
	connect( quit, SIGNAL(pressed()), qApp, SLOT(quit()) );
    }

    ~HttpInfo()
    {
    }

private slots:
    void newConnect()
    {
	infoText->append( "New connection" );
    }
    void endConnect()
    {
	infoText->append( "Connection closed" );
    }
    void wroteToClient()
    {
	infoText->append( "Wrote to client" );
    }

private:
    QTextView *infoText;
};


int main( int argc, char** argv )
{
    QApplication app( argc, argv );
    HttpInfo info;
    app.setMainWidget( &info );
    info.setCaption("Qt Example - httpd");
    info.show();
    return app.exec();
}

#include "httpd.moc"
