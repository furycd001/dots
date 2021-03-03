/****************************************************************************
** $Id: qt/examples/life/lifedlg.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "lifedlg.h"
#include <qapplication.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qslider.h>
#include <qcombobox.h>
#include <qdatetime.h>
#include <stdlib.h>

#include "patterns.cpp"


// A simple timer which has a pause and a setSpeed slot

LifeTimer::LifeTimer( QWidget *parent ) : QTimer( parent ), interval( 500 )
{
    start( interval );
}


void LifeTimer::pause( bool stopIt )
{
    if ( stopIt )
	stop();
    else
	start( interval );
}


void LifeTimer::setSpeed( int speed )
{
    interval = MAXSPEED - speed; 
    if ( isActive() )
	changeInterval( interval );
}


// A top-level container widget to organize the others

LifeDialog::LifeDialog( int scale, QWidget * parent, const char * name )
    : QWidget( parent, name )
{
    qb = new QPushButton( "Quit!", this );
    cb = new QComboBox( this, "comboBox" );
    life = new LifeWidget(scale, this);
    life->move( SIDEBORDER, TOPBORDER );


    connect( qb, SIGNAL(clicked()), qApp, SLOT(quit()) );
    qb->setGeometry( SIDEBORDER, SIDEBORDER, qb->sizeHint().width(), 25 );
    timer = new LifeTimer( this );

    connect( timer, SIGNAL(timeout()), life, SLOT(nextGeneration()) );
    pb = new QPushButton( "Pause", this );
    pb->setToggleButton( TRUE );
    connect( pb, SIGNAL(toggled(bool)), timer, SLOT(pause(bool)) );
    pb->resize( pb->sizeHint().width(), 25 );
    pb->move( width() - SIDEBORDER - pb->width(), SIDEBORDER );

    sp = new QLabel( "Speed:", this );
    sp->adjustSize();
    sp->move( SIDEBORDER, 45 );
    scroll = new QSlider( 0, LifeTimer::MAXSPEED, 50,
			     LifeTimer::MAXSPEED / 2,
			     QSlider::Horizontal, this );
    connect( scroll, SIGNAL(valueChanged(int)),
	     timer,  SLOT(setSpeed(int)) );

    scroll->move( sp->width() + 2 * SIDEBORDER, 45 );
    scroll->resize( 200, 15 );

    life->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    life->show();

    srand( QTime(0,0,0).msecsTo(QTime::currentTime()) );
    int sel =  rand() % NPATS;
    getPattern( sel );

    cb->move( 2*SIDEBORDER + qb->width(), SIDEBORDER);
    cb->insertItem( "Glider Gun " );
    cb->insertItem( "Figure Eight " );
    cb->insertItem( "Pulsar " );
    cb->insertItem( "Barber Pole P2 " );
    cb->insertItem( "Achim P5 " );
    cb->insertItem( "Hertz P4 " );
    cb->insertItem( "Tumbler " );
    cb->insertItem( "Pulse1 P4" );
    cb->insertItem( "Shining Flower P5 " );
    cb->insertItem( "Pulse2 P6 " );
    cb->insertItem( "Pinwheel, Clock P4 " );
    cb->insertItem( "Pentadecatholon " );
    cb->insertItem( "Piston " );
    cb->insertItem( "Piston2 " );
    cb->insertItem( "Switch Engine " );
    cb->insertItem( "Gears (Gear, Flywheel, Blinker) " );
    cb->insertItem( "Turbine8 " );
    cb->insertItem( "P16 " );
    cb->insertItem( "Puffer " );
    cb->insertItem( "Escort " );
    cb->insertItem( "Dart Speed 1/3 " );
    cb->insertItem( "Period 4 Speed 1/2 " );
    cb->insertItem( "Another Period 4 Speed 1/2 " );
    cb->insertItem( "Smallest Known Period 3 Spaceship Speed 1/3 " );
    cb->insertItem( "Turtle Speed 1/3 " );
    cb->insertItem( "Smallest Known Period 5 Speed 2/5 " );
    cb->insertItem( "Sym Puffer " );
    cb->insertItem( "], Near Ship, Pi Heptomino " );
    cb->insertItem( "R Pentomino " );
    cb->setAutoResize( FALSE );
    cb->setCurrentItem( sel );
    cb->show();
    connect( cb, SIGNAL(activated(int)), SLOT(getPattern(int)) );

    QSize s;
    s = life->minimumSize();
    setMinimumSize( s.width() + 2 * SIDEBORDER, 
		    s.height() + TOPBORDER + SIDEBORDER );
    s = life->maximumSize();
    setMaximumSize( s.width() + 2 * SIDEBORDER, 
		    s.height() + TOPBORDER + SIDEBORDER );
    s = life->sizeIncrement();
    setSizeIncrement( s.width(), s.height() );

    resize( QMIN(512, qApp->desktop()->width()),
	    QMIN(480, qApp->desktop()->height()) );
}


void LifeDialog::resizeEvent( QResizeEvent * e )
{
    life->resize( e->size() - QSize( 2 * SIDEBORDER, TOPBORDER + SIDEBORDER ));
    pb->move( e->size().width() - SIDEBORDER - pb->width(), SIDEBORDER );
    scroll->resize( e->size().width() - sp->width() - 3 * SIDEBORDER,
		    scroll->height() );
    cb->resize( width() - 4*SIDEBORDER - qb->width() - pb->width()  , 25 );
}


// Adapted from xlock, see pattern.cpp for copyright info.

void LifeDialog::getPattern( int pat )
{
    life->clear();
    int i = pat % NPATS;
    int col;
    int * patptr = &patterns[i][0];
    while ( (col = *patptr++) != 127 ) {
	int row = *patptr++;
	col += life->maxCol() / 2;
	row += life->maxRow() / 2;
	life->setPoint( col, row );
    }
}
