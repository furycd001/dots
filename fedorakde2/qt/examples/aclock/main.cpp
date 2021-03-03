/****************************************************************************
** $Id: qt/examples/aclock/main.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "aclock.h"
#include <qapplication.h>


int main( int argc, char **argv )
{
    QApplication a( argc, argv );
    AnalogClock *clock = new AnalogClock;
    if ( argc == 2 && strcmp( argv[1], "-transparent" ) == 0 )
	clock->setAutoMask( TRUE );
    clock->resize( 100, 100 );
    a.setMainWidget( clock );
    clock->setCaption("Qt Example - Analog Clock");
    clock->show();
    int result = a.exec();
    delete clock;
    return result;
}
