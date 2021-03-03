/****************************************************************************
** $Id: qt/examples/mail/main.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qapplication.h>

#include "composer.h"

int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    Composer c;
    a.setMainWidget( &c );
    c.resize( 400, 500 );
    c.setCaption("Qt Example - Mail");
    c.show();
    return a.exec();
}
