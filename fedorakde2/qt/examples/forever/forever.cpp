/****************************************************************************
** $Id: qt/examples/forever/forever.cpp   2.3.2   edited 2001-10-18 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qtimer.h>
#include <qpainter.h>
#include <qapplication.h>
#include <stdlib.h>				// defines rand() function

#include "forever.h"


//
// Forever - a widget that draws rectangles forever.
//

//
// Constructs a Forever widget.
//

Forever::Forever( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    for (int a=0; a<numColors; a++) {
	colors[a] = QColor( rand()&255,
			    rand()&255,
			    rand()&255 );
    }
    rectangles = 0;
    startTimer( 0 );				// run continuous timer
    QTimer * counter = new QTimer( this );
    connect( counter, SIGNAL(timeout()),
	     this, SLOT(updateCaption()) );
    counter->start( 1000 );
}


void Forever::updateCaption()
{
    QString s;
    s.sprintf( "Qt Example - Forever - %d rectangles/second", rectangles );
    rectangles = 0;
    setCaption( s );
}


//
// Handles timer events for the Forever widget.
//

void Forever::timerEvent( QTimerEvent * )
{
    QPainter paint( this );			// painter object
    paint.setPen( NoPen );			// do not draw outline
    int w = width();
    int h = height();
    if( w <= 0 || h <= 0 ) 
	return;
    for ( int i=0; i<100; i++ ) {
	paint.setBrush( colors[i % numColors]); // set random brush color
	QPoint p1( rand()%w, rand()%h );	// p1 = top left
	QPoint p2( rand()%w, rand()%h );	// p2 = bottom right
	QRect r( p1, p2 );
	paint.drawRect( r );			// draw filled rectangle
	rectangles++;
    }
}


//
// Create and display Forever widget.
//

int main( int argc, char **argv )
{
    QApplication a( argc, argv );		// create application object
    Forever always;				// create widget
    a.setMainWidget( &always );			// set as main widget
    always.setCaption("Qt Example - Forever");
    always.show();				// show widget
    return a.exec();				// run event loop
}
