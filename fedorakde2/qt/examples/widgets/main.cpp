/****************************************************************************
** $Id: qt/examples/widgets/main.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qapplication.h>
#include "widgets.h"
#include <qmotifstyle.h>
#include <qcdestyle.h>
#include <qwindowsstyle.h>
#include <qplatinumstyle.h>
#include <qsgistyle.h>

class MyWidgetView : public WidgetView
{
    int s;
public:
    MyWidgetView( QWidget *parent=0, const char *name=0 )
	:WidgetView(parent, name), s(0)
	{
	}

    void button1Clicked() {
	s++;
	switch (s%5){
	case 0:
	    qApp->setStyle(new QMotifStyle);
	    break;
	case 1:
	    qApp->setStyle(new QCDEStyle);
	    break;
	case 2:
	    qApp->setStyle(new QWindowsStyle);
	    break;
	case 3:
	    qApp->setStyle(new QPlatinumStyle);
	    break;
	case 4:
	    qApp->setStyle(new QSGIStyle);
	    break;
	}
	WidgetView::button1Clicked();
    }
};


//
// Create and display our WidgetView.
//

int main( int argc, char **argv )
{
    QApplication::setColorSpec( QApplication::CustomColor );
    QApplication a( argc, argv );

    MyWidgetView* w = new MyWidgetView;
    a.setMainWidget( w );

    w->show();
    int res = a.exec();
    delete w;
    return res;
}
