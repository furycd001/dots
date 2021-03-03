/****************************************************************************
** $Id: qt/examples/qwerty/main.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qapplication.h>
#include "qwerty.h"


int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    
    bool small =  qApp->desktop()->size().width() < 450 
		  || qApp->desktop()->size().height() < 450;
    
    int i;
    for ( i= argc <= 1 ? 0 : 1; i<argc; i++ ) {
	Editor *e = new Editor;
	e->setCaption("Qt Example - QWERTY");
	if ( i > 0 )
	    e->load( argv[i] );
	if ( small ) {
	    e->showMaximized();
	} else {
	    e->resize( 400, 400 );
	    e->show();
	}
    }
    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
    return a.exec();
}
