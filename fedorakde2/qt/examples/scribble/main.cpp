/****************************************************************************
** $Id: qt/examples/scribble/main.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright ( C ) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "scribble.h"
#include <qapplication.h>


int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    Scribble scribble;

    scribble.resize( 500, 350 );
    scribble.setCaption("Qt Example - Scribble");
    a.setMainWidget( &scribble );
    if ( QApplication::desktop()->width() > 550
	 && QApplication::desktop()->height() > 366 )
	scribble.show();
    else
	scribble.showMaximized();
    return a.exec();
}
