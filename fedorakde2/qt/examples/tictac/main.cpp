/****************************************************************************
** $Id: qt/examples/tictac/main.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qapplication.h>
#include <stdlib.h>
#include "tictac.h"


int main( int argc, char **argv )
{
    QApplication a( argc, argv );
    int n = 3;
    if ( argc == 2 )				// get board size n
        n = atoi(argv[1]);
    if ( n < 3 || n > 10 ) {			// out of range
        qWarning( "%s: Board size must be from 3x3 to 10x10", argv[0] );
        return 1;
    }
    TicTacToe ttt( n );				// create game
    a.setMainWidget( &ttt );
    ttt.setCaption("Qt Example - TicTac");
    ttt.show();					// show widget
    return a.exec();				// go
}
