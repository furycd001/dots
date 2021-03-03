/****************************************************************************
** $Id: qt/examples/life/life.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "life.h"

#include <qpainter.h>
#include <qdrawutil.h>
#include <qcheckbox.h>
#include <qevent.h>
#include <qapplication.h>

// The main game of life widget

LifeWidget::LifeWidget( int s, QWidget *parent, const char *name )
    : QFrame( parent, name )
{
    SCALE = s;

    maxi = maxj = 50;
    setMinimumSize( MINSIZE * SCALE + 2 * BORDER,
		   MINSIZE * SCALE + 2 * BORDER );
    setMaximumSize( MAXSIZE * SCALE + 2 * BORDER,
		   MAXSIZE * SCALE + 2 * BORDER );
    setSizeIncrement( SCALE, SCALE);

    clear();
    resize( maxi * SCALE + 2 * BORDER , maxj * SCALE + 2 * BORDER );

}


void LifeWidget::clear()
{
    current = 0;
    for ( int t = 0; t < 2; t++ )
	for ( int i = 0; i < MAXSIZE + 2; i++ )
	    for ( int j = 0; j < MAXSIZE + 2; j++ )
		cells[t][i][j] = FALSE;

    repaint();
}


// We assume that the size will never be beyond the maximum size set
// this is not in general TRUE, but in practice it's good enough for
// this program

void LifeWidget::resizeEvent( QResizeEvent * e )
{
    maxi = (e->size().width()  - 2 * BORDER) / SCALE;
    maxj = (e->size().height() - 2 * BORDER) / SCALE;
}


void LifeWidget::setPoint( int i, int j )
{
    if ( i < 1 || i > maxi || j < 1 || j > maxi )
	return;
    cells[current][i][j] = TRUE;
    repaint( index2pos(i), index2pos(j), SCALE, SCALE, FALSE );
}


void LifeWidget::mouseHandle( const QPoint &pos )
{
    int i = pos2index( pos.x() );
    int j = pos2index( pos.y() );
    setPoint( i, j );
}


void LifeWidget::mouseMoveEvent( QMouseEvent *e )
{
    mouseHandle( e->pos() );
}


void LifeWidget::mousePressEvent( QMouseEvent *e )
{
    if ( e->button() == QMouseEvent::LeftButton )
	mouseHandle( e->pos() );
}


void LifeWidget::nextGeneration()
{
    for ( int i = 1; i <= MAXSIZE; i++ ) {
	for ( int j = 1; j <= MAXSIZE; j++ ) {
	    int t = cells[current][i - 1][j - 1]
	    + cells[current][i - 1][j]
	    + cells[current][i - 1][j + 1]
	    + cells[current][i][j - 1]
	    + cells[current][i][j + 1]
	    + cells[current][i + 1][j - 1]
	    + cells[current][i + 1][j]
	    + cells[current][i + 1][j + 1];

	    cells[!current][i][j] = ( t == 3 ||
				      t == 2 && cells[current][i][j] );
	}
    }
    current = !current;
    repaint( FALSE );		// repaint without erase
}


void LifeWidget::paintEvent( QPaintEvent * e )
{
    int starti = pos2index( e->rect().left() );
    int stopi  = pos2index( e->rect().right() );
    int startj = pos2index( e->rect().top() );
    int stopj  = pos2index( e->rect().bottom() );

    if (stopi > maxi)
	stopi = maxi;
    if (stopj > maxj)
	stopj = maxj;

    QPainter paint( this );
    for ( int i = starti; i <= stopi; i++ ) {
	for ( int j = startj; j <= stopj; j++ ) {
	    if ( cells[current][i][j] )
		qDrawShadePanel( &paint, index2pos( i ), index2pos( j ),
				 SCALE - 1, SCALE - 1, colorGroup() );
	    else if ( cells[!current][i][j] )
		paint.eraseRect( index2pos( i ), index2pos( j ),
				 SCALE - 1, SCALE - 1);
	}
    }
    drawFrame( &paint );
}
