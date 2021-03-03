/****************************************************************************
** $Id: qt/examples/drawlines/connect.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qwidget.h>
#include <qpainter.h>
#include <qapplication.h>
#include <stdlib.h>


const int MAXPOINTS = 2000;			// maximum number of points
const int MAXCOLORS = 40;


//
// ConnectWidget - draws connected lines
//

class ConnectWidget : public QWidget
{
public:
    ConnectWidget( QWidget *parent=0, const char *name=0 );
   ~ConnectWidget();
protected:
    void	paintEvent( QPaintEvent * );
    void	mousePressEvent( QMouseEvent *);
    void	mouseReleaseEvent( QMouseEvent *);
    void	mouseMoveEvent( QMouseEvent *);
private:
    QPoint     *points;				// point array
    QColor     *colors;				// color array
    int		count;				// count = number of points
    bool	down;				// TRUE if mouse down
};


//
// Constructs a ConnectWidget.
//

ConnectWidget::ConnectWidget( QWidget *parent, const char *name )
    : QWidget( parent, name, WNorthWestGravity )
{
    setBackgroundColor( white );		// white background
    count = 0;
    down = FALSE;
    points = new QPoint[MAXPOINTS];
    colors = new QColor[MAXCOLORS];
    for ( int i=0; i<MAXCOLORS; i++ )		// init color array
	colors[i] = QColor( rand()&255, rand()&255, rand()&255 );
}

ConnectWidget::~ConnectWidget()
{
    delete[] points;				// cleanup
    delete[] colors;
}


//
// Handles paint events for the connect widget.
//

void ConnectWidget::paintEvent( QPaintEvent * )
{
    QPainter paint( this );
    for ( int i=0; i<count-1; i++ ) {		// connect all points
	for ( int j=i+1; j<count; j++ ) {
	    paint.setPen( colors[rand()%MAXCOLORS] ); // set random pen color
	    paint.drawLine( points[i], points[j] ); // draw line
	}
    }
}


//
// Handles mouse press events for the connect widget.
//

void ConnectWidget::mousePressEvent( QMouseEvent * )
{
    down = TRUE;
    count = 0;					// start recording points
    erase();					// erase widget contents
}


//
// Handles mouse release events for the connect widget.
//

void ConnectWidget::mouseReleaseEvent( QMouseEvent * )
{
    down = FALSE;				// done recording points
    update();					// draw the lines
}


//
// Handles mouse move events for the connect widget.
//

void ConnectWidget::mouseMoveEvent( QMouseEvent *e )
{
    if ( down && count < MAXPOINTS ) {
	QPainter paint( this );
	points[count++] = e->pos();		// add point
	paint.drawPoint( e->pos() );		// plot point
    }
}


//
// Create and display a ConnectWidget.
//

int main( int argc, char **argv )
{
    QApplication a( argc, argv );
    ConnectWidget connect;
    a.setMainWidget( &connect );
    connect.setCaption("Qt Example - Draw lines");
    connect.show();
    return a.exec();
}
