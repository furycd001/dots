/****************************************************************************
** $Id: qt/examples/tooltip/main.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qapplication.h>
#include "tooltip.h"

int main( int argc, char ** argv ) 
{
    QApplication a( argc, argv );

    TellMe mw;
    mw.setCaption( "Qt Example - Dynamic Tool Tips" );
    a.setMainWidget( &mw );
    mw.show();
    
    return a.exec();
}
