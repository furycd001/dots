/****************************************************************************
** $Id: qt/examples/tetrix/qtetrixb.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-1998 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "qtetrixb.h"
#include "qtetrix.h"
#include <qtimer.h>
#include <qkeycode.h>
#include <qpainter.h>

const int waitAfterLineTime = 500;

QTetrixBoard::QTetrixBoard( QWidget *p, const char *name )
    : QFrame( p, name )
{
    setFrameStyle( QFrame::Panel | QFrame::Sunken );
    paint = 0;
    timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), SLOT(timeout()) );

    colors[0].setRgb(200,100,100);
    colors[1].setRgb(100,200,100);
    colors[2].setRgb(100,100,200);
    colors[3].setRgb(200,200,100);
    colors[4].setRgb(200,100,200);
    colors[5].setRgb(100,200,200);
    colors[6].setRgb(218,170,  0);

    xOffset          = -1;      // -1 until a resizeEvent is received.
    blockWidth       = 20;
    yOffset          = 30;
    blockHeight      = 20;
    noGame           = TRUE;
    isPaused         = FALSE;
    waitingAfterLine = FALSE;
    updateTimeoutTime();   // Sets timeoutTime
}

void QTetrixBoard::startGame(int gameType,int fillRandomLines)
{
    if ( isPaused )
        return;		// ignore if game is paused
    noGame = FALSE;
    GenericTetrix::startGame( gameType, fillRandomLines );
    // Note that the timer is started by updateLevel!
}


void QTetrixBoard::pause()
{
    if ( noGame )			// game not active
        return;
    isPaused = !isPaused;
    if ( isPaused ) {
	timer->stop();
        hideBoard();
    }
    else
	timer->start(timeoutTime);
    update();
}


void QTetrixBoard::drawSquare(int x,int y,int value)
{
    if (xOffset == -1)    // Before first resizeEvent?
        return;

    const int X = xOffset  + x*blockWidth;
    const int Y = yOffset  + (y - 1)*blockHeight;

    bool localPainter = paint == 0;
    QPainter *p;
    if ( localPainter )
	p = new QPainter( this );    
    else
	p = paint;
    drawTetrixButton( p, X, Y, blockWidth, blockHeight,
		      value == 0 ? 0 : &colors[value-1] );
    /*
    if ( value != 0 ) {
	QColor tc, bc;
	tc = colors[value-1].light();
	bc = colors[value-1].dark();
	p->drawShadePanel( X, Y, blockWidth, blockHeight,
			   tc, bc, 1, colors[value-1], TRUE );
    }
    else
	p->fillRect( X, Y, blockWidth, blockHeight, backgroundColor() );
	*/
    if ( localPainter )
	delete p;
}

void QTetrixBoard::drawNextSquare( int x, int y, int value )
{
    if ( value == 0 )
        emit drawNextSquareSignal (x, y, 0 );
    else
        emit drawNextSquareSignal( x, y, &colors[value-1] );
}

void QTetrixBoard::updateRemoved( int noOfLines )
{
    if ( noOfLines > 0 ) {
        timer->stop();
        timer->start( waitAfterLineTime );
        waitingAfterLine = TRUE;
    }
    emit updateRemovedSignal( noOfLines );
}

void QTetrixBoard::updateScore( int newScore )
{
    emit updateScoreSignal( newScore );
}

void QTetrixBoard::updateLevel( int newLevel )
{
    timer->stop();
    updateTimeoutTime();
    timer->start( timeoutTime );
    emit updateLevelSignal( newLevel );
}

void QTetrixBoard::pieceDropped(int)
{
    if ( waitingAfterLine ) // give player a break if a line has been removed
        return;
    newPiece();
}

void QTetrixBoard::gameOver()
{
    timer->stop();
    noGame = TRUE;
    emit gameOverSignal();
}

void QTetrixBoard::timeout()
{
    if ( waitingAfterLine ) {
	timer->stop();
	waitingAfterLine = FALSE;
	newPiece();
	timer->start( timeoutTime );
    } else {
        oneLineDown();
    }
}

void QTetrixBoard::drawContents( QPainter *p )
{
    const char *text = "Press \"Pause\"";
    QRect r = contentsRect();
    paint = p;				// set widget painter
    if ( isPaused ) {
	p->drawText( r, AlignCenter | AlignVCenter, text );
        return;
    }
    int x1,y1,x2,y2;
    x1 = (r.left() - xOffset) / blockWidth;
    if (x1 < 0)
        x1 = 0;
    if (x1 >= boardWidth())
        x1 = boardWidth() - 1;

    x2 = (r.right() - xOffset) / blockWidth;
    if (x2 < 0)
        x2 = 0;
    if (x2 >= boardWidth())
        x2 = boardWidth() - 1;

    y1 = (r.top() - yOffset) / blockHeight;
    if (y1 < 0)
        y1 = 0;
    if (y1 >= boardHeight())
        y1 = boardHeight() - 1;

    y2 = (r.bottom() - yOffset) / blockHeight;
    if (y2 < 0)
        y2 = 0;
    if (y2 >= boardHeight())
        y2 = boardHeight() - 1;

    updateBoard( x1, y1, x2, y2, TRUE );
    paint = 0;				// reset widget painter
    return;
}

void QTetrixBoard::resizeEvent(QResizeEvent *e)
{
    QSize sz = e->size();
    blockWidth  = (sz.width() - 3)/10;
    blockHeight = (sz.height() - 3)/22;
    xOffset     = 1;
    yOffset     = 1;
}

void QTetrixBoard::keyPressEvent( QKeyEvent *e )
{
    if ( noGame || isPaused || waitingAfterLine )
        return;
    switch( e->key() ) {
	case Key_Left :
	    moveLeft();
	    break;
	case Key_Right :
	    moveRight();
	    break;
	case Key_Down :
	    rotateRight();
	    break;
	case Key_Up :
	    rotateLeft();
	    break;
	case Key_Space :
	    dropDown();
	    break;
	case Key_D :
	    oneLineDown();
	    break;
        default:
	    return;
    }
    e->accept();
}

void QTetrixBoard::updateTimeoutTime()
{
    timeoutTime = 1000/(1 + getLevel());
}
