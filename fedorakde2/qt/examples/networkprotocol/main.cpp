/****************************************************************************
** $Id: qt/examples/networkprotocol/main.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qapplication.h>
#include <qnetwork.h>

#include "http.h"
#include "nntp.h"
#include "view.h"

int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    qInitNetworkProtocols();
    QNetworkProtocol::registerNetworkProtocol( "http", new QNetworkProtocolFactory<Http> );
    QNetworkProtocol::registerNetworkProtocol( "nntp", new QNetworkProtocolFactory<Nntp> );
    
    View v;
    v.resize( 600, 600 );
    v.setCaption("Qt Example - Network Protocols");
    v.show();
    a.setMainWidget( &v );
    
    return a.exec();
}
