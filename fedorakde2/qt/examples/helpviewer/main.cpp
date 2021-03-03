/****************************************************************************
** $Id: qt/examples/helpviewer/main.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "helpwindow.h"
#include <qapplication.h>
#include <qwindowsstyle.h>
#include <qstylesheet.h>
#include <stdlib.h>


int main( int argc, char ** argv )
{
    QApplication::setColorSpec( QApplication::ManyColor );
    QApplication a(argc, argv);

    QString home;
    if (argc > 1)
        home = argv[1];
    else
        home = QString(getenv("QTDIR")) + "/doc/html/index.html";

    
    HelpWindow *help = new HelpWindow(home, ".", 0, "help viewer");
    help->setCaption("Qt Example - Helpviewer");
    if ( QApplication::desktop()->width() > 400
	 && QApplication::desktop()->height() > 500 )
	help->show();
    else
	help->showMaximized();

    QObject::connect( &a, SIGNAL(lastWindowClosed()),
                      &a, SLOT(quit()) );

    return a.exec();
}
