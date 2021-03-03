/****************************************************************************
** $Id: qt/examples/tetrix/qtetrix.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "qtetrix.h"
#include <qapplication.h>
#include <qlabel.h>
#include <qdatetime.h>


void drawTetrixButton( QPainter *p, int x, int y, int w, int h,
		       const QColor *color )
{
    QColor fc;
    if ( color ) {
        QPointArray a;
	a.setPoints( 3,  x,y+h-1, x,y, x+w-1,y );
	p->setPen( color->light() );
	p->drawPolyline( a );
	a.setPoints( 3, x+1,y+h-1, x+w-1,y+h-1, x+w-1,y+1 );
	p->setPen( color->dark() );
	p->drawPolyline( a );
	x++;
	y++;
	w -= 2;
	h -= 2;
	fc = *color;
    }
    else
	fc = p->backgroundColor();
    p->fillRect( x, y, w, h, fc );
}


ShowNextPiece::ShowNextPiece( QWidget *parent, const char *name )
    : QFrame( parent, name )
{
    setFrameStyle( QFrame::Panel | QFrame::Sunken );
    xOffset = -1;     // -1 until first resizeEvent.
}

void ShowNextPiece::resizeEvent( QResizeEvent *e )
{
    QSize sz = e->size();
    blockWidth  = (sz.width()  - 3)/5;
    blockHeight = (sz.height() - 3)/6;
    xOffset     = (sz.width()  - 3)/5;
    yOffset     = (sz.height() - 3)/6;
}


void ShowNextPiece::paintEvent( QPaintEvent * )
{
    QPainter p( this );
    drawFrame( &p );
    p.end();			// explicit end() so any slots can paint too
    emit update();
}


void ShowNextPiece::drawNextSquare(int x, int y,QColor *color)
{
    if (xOffset == -1)		// Before first resizeEvent?
        return;

    QPainter paint;
    paint.begin(this);
    drawTetrixButton( &paint, xOffset+x*blockWidth, yOffset+y*blockHeight,
		      blockWidth, blockHeight, color );
    paint.end();
}


QTetrix::QTetrix( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    QTime t = QTime::currentTime();
    TetrixPiece::setRandomSeed( (((double)t.hour())+t.minute()+t.second())/
                                 (24+60+60) );

#define ADD_LABEL( str, x, y, w, h )			\
    { QLabel *label = new QLabel(str,this); 		\
      label->setGeometry(x,y,w,h); 			\
      label->setAlignment(AlignCenter|AlignVCenter); }

    ADD_LABEL( "NEXT", 50, 10, 78, 30 );
    ADD_LABEL( "SCORE", 330, 10, 178, 30 );
    ADD_LABEL( "LEVEL", 50, 130, 78, 30 );
    ADD_LABEL( "LINES REMOVED", 330, 130, 178, 30 );

    board       = new QTetrixBoard(this);
    showNext    = new ShowNextPiece(this);
#ifndef QT_NO_LCDNUMBER
    showScore   = new QLCDNumber(5,this);
    showLevel   = new QLCDNumber(2,this);
    showLines   = new QLCDNumber(5,this);
#else
    showScore   = new QLabel("0",this);
    showLevel   = new QLabel("0",this);
    showLines   = new QLabel("0",this);
    showScore->setAlignment(AlignCenter);
    showLines->setAlignment(AlignCenter);
    showLevel->setAlignment(AlignCenter);
    showScore->setFrameStyle(QFrame::Sunken|QFrame::Box);
    showLines->setFrameStyle(QFrame::Sunken|QFrame::Box);
    showLevel->setFrameStyle(QFrame::Sunken|QFrame::Box);
#endif    
    quitButton  = new QPushButton("&Quit",this);
    startButton = new QPushButton("&New Game",this);
    pauseButton = new QPushButton("&Pause",this);

    // Don't let the buttons get keyboard focus
    quitButton->setFocusPolicy( QWidget::NoFocus );
    startButton->setFocusPolicy( QWidget::NoFocus );
    pauseButton->setFocusPolicy( QWidget::NoFocus );

    connect( board, SIGNAL(gameOverSignal()), SLOT(gameOver()) );
    connect( board, SIGNAL(drawNextSquareSignal(int,int,QColor*)), showNext,
	     SLOT(drawNextSquare(int,int,QColor*)) );
    connect( showNext, SIGNAL(update()), board, SLOT(updateNext()) );
#ifndef QT_NO_LCDNUMBER
    connect( board, SIGNAL(updateScoreSignal(int)), showScore,
	     SLOT(display(int)) );
    connect( board, SIGNAL(updateLevelSignal(int)), showLevel,
	     SLOT(display(int)));
    connect( board, SIGNAL(updateRemovedSignal(int)), showLines,
	     SLOT(display(int)));
#else
    connect( board, SIGNAL(updateScoreSignal(int)), showScore,
	     SLOT(setNum(int)) );
    connect( board, SIGNAL(updateLevelSignal(int)), showLevel,
	     SLOT(setNum(int)));
    connect( board, SIGNAL(updateRemovedSignal(int)), showLines,
	     SLOT(setNum(int)));
#endif
    connect( startButton, SIGNAL(clicked()), board, SLOT(start()) );
    connect( quitButton , SIGNAL(clicked()), SLOT(quit()));
    connect( pauseButton, SIGNAL(clicked()), board, SLOT(pause()) );

    board->setGeometry( 150, 20, 153, 333 );
    showNext->setGeometry( 50, 40, 78, 94 );
    showScore->setGeometry( 330, 40, 178, 93 );
    showLevel->setGeometry( 50, 160, 78, 93 );
    showLines->setGeometry( 330, 160, 178, 93 );
#ifndef QT_NO_LCDNUMBER
    showScore->display( 0 );
    showLevel->display( 0 );
    showLines->display( 0 );
#else
    showScore->setNum( 0 );
    showLevel->setNum( 0 );
    showLines->setNum( 0 );
#endif    
    startButton->setGeometry( 50, 288, 80, 30 );
    quitButton->setGeometry( 375, 265, 80, 30 );
    pauseButton->setGeometry( 375, 310, 80, 30 );
    board->revealNextPiece(TRUE);

    resize( 550, 370 );
}

void QTetrix::gameOver()
{
}


void QTetrix::quit()
{
    qApp->quit();
}
