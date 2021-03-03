/****************************************************************************
** $Id: qt/examples/rangecontrols/main.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "rangecontrols.h"
#include <qapplication.h>

int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    RangeControls rangecontrols;
    rangecontrols.resize( 500, 300 );
    rangecontrols.setCaption( "Qt Example - Range Control Widgets" );
    a.setMainWidget( &rangecontrols );
    rangecontrols.show();

    return a.exec();
}
