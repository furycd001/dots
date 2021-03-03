/****************************************************************************
** $Id: qt/examples/tooltip/tooltip.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "tooltip.h"
#include <qapplication.h>
#include <qpainter.h>
#include <stdlib.h>


DynamicTip::DynamicTip( QWidget * parent )
    : QToolTip( parent )
{
    // no explicit initialization needed
}


void DynamicTip::maybeTip( const QPoint &pos )
{
    if ( !parentWidget()->inherits( "TellMe" ) )
	return;

    QRect r( ((TellMe*)parentWidget())->tip(pos) );
    if ( !r.isValid() )
	return;

    QString s;
    s.sprintf( "position: %d,%d", r.center().x(), r.center().y() );
    tip( r, s );
}


TellMe::TellMe( QWidget * parent , const char * name  )
    : QWidget( parent, name )
{
    setMinimumSize( 30, 30 );
    r1 = randomRect();
    r2 = randomRect();
    r3 = randomRect();

    t = new DynamicTip( this );

    QToolTip::add( this, r3, "this color is called red" ); // <- helpful
}


TellMe::~TellMe()
{
    delete t;
    t = 0;
}


void TellMe::paintEvent( QPaintEvent * e )
{
    QPainter p( this );

    // I try to be efficient here, and repaint only what's needed

    if ( e->rect().intersects( r1 ) ) {
	p.setBrush( blue );
	p.drawRect( r1 );
    }

    if ( e->rect().intersects( r2 ) ) {
	p.setBrush( blue );
	p.drawRect( r2 );
    }

    if ( e->rect().intersects( r3 ) ) {
	p.setBrush( red );
	p.drawRect( r3 );
    }
}


void TellMe::mousePressEvent( QMouseEvent * e )
{
    if ( r1.contains( e->pos() ) )
	r1 = randomRect();
    if ( r2.contains( e->pos() ) )
	r2 = randomRect();
    repaint();
}


void TellMe::resizeEvent( QResizeEvent * )
{
    if ( !rect().contains( r1 ) )
	 r1 = randomRect();
    if ( !rect().contains( r2 ) )
	 r2 = randomRect();
}


QRect TellMe::randomRect()
{
    return QRect( ::rand() % (width() - 20), ::rand() % (height() - 20),
		  20, 20 );
}


QRect TellMe::tip( const QPoint & p )
{
    if ( r1.contains( p ) )
	return r1;
    else if ( r2.contains( p ) )
	return r2;
    else
	return QRect( 0,0, -1,-1 );
}
