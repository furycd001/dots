/****************************************************************************
** $Id: qt/examples/tabdialog/main.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "tabdialog.h"
#include <qapplication.h>
#include <qstring.h>

int main( int argc, char **argv )
{

    QApplication a( argc, argv );

    TabDialog tabdialog( 0, "tabdialog", QString( argc < 2 ? "." : argv[1] ) );
    tabdialog.resize( 450, 350 );
    tabdialog.setCaption( "Qt Example - Tabbed Dialog" );
    a.setMainWidget( &tabdialog );
    tabdialog.show();

    return a.exec();
}
