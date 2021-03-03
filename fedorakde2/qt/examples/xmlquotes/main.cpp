/****************************************************************************
** $Id: qt/examples/xmlquotes/main.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "richtext.h"
#include "quoteparser.h"
#include <qapplication.h>
#include <qfile.h>
#include <qmessagebox.h>

int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    // parse xml file
    QuoteHandler handler;
    QFile file( "quotes.xml" );
    QXmlInputSource source( file );
    QXmlSimpleReader reader;
    reader.setContentHandler( &handler );
    reader.setErrorHandler( &handler );
    bool ok = reader.parse( source );
    file.close();
    if ( !ok ) {
	QMessageBox::critical( 0,
		a.tr( "Parse Error" ),
		a.tr( handler.errorProtocol() ) );
	return -1;
    }

    MyRichText richtext( handler.quotes() );
    richtext.resize( 450, 350 );
    richtext.setCaption( "Qt Example - XML Quotes" );
    a.setMainWidget( &richtext );
    richtext.show();

    return a.exec();
}
