/****************************************************************************
** $Id: qt/examples/dclock/dclock.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "dclock.h"
#include <qdatetime.h>


//
// Constructs a DigitalClock widget with a parent and a name.
//

DigitalClock::DigitalClock( QWidget *parent, const char *name )
    : QLCDNumber( parent, name )
{
    showingColon = FALSE;
    setFrameStyle( QFrame::Panel | QFrame::Raised );
    setLineWidth( 2 );				// set frame line width
    showTime();					// display the current time
    normalTimer = startTimer( 500 );		// 1/2 second timer events
    showDateTimer = -1;				// not showing date
}


//
// Handles timer events for the digital clock widget.
// There are two different timers; one timer for updating the clock
// and another one for switching back from date mode to time mode.
//

void DigitalClock::timerEvent( QTimerEvent *e )
{
    if ( e->timerId() == showDateTimer )	// stop showing date
	stopDate();
    else {					// normal timer
	if ( showDateTimer == -1 )		// not showing date
	    showTime();
    }
}

//
// Enters date mode when the left mouse button is pressed.
//

void DigitalClock::mousePressEvent( QMouseEvent *e )
{
    if ( e->button() == QMouseEvent::LeftButton )		// left button pressed
	showDate();
}


//
// Shows the current date in the internal lcd widget.
// Fires a timer to stop showing the date.
//

void DigitalClock::showDate()
{
    if ( showDateTimer != -1 )			// already showing date
	return;
    QDate date = QDate::currentDate();
    QString s;
    s.sprintf( "%2d %2d", date.month(), date.day() );
    display( s );				// sets the LCD number/text
    showDateTimer = startTimer( 2000 );		// keep this state for 2 secs
}

//
// Stops showing the date.
//

void DigitalClock::stopDate()
{
    killTimer( showDateTimer );
    showDateTimer = -1;
    showTime();
}

//
// Shows the current time in the internal lcd widget.
//

void DigitalClock::showTime()
{
    showingColon = !showingColon;		// toggle/blink colon
    QString s = QTime::currentTime().toString().left(5);
    if ( !showingColon )
	s[2] = ' ';
    if ( s[0] == '0' )
	s[0] = ' ';
    display( s );				// set LCD number/text
}
