/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qapplication.h>
#include "mydialogimpl.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    MyDialogImpl * mw = new MyDialogImpl;
    a.setMainWidget( mw );
    mw->show();
    return a.exec();
}
