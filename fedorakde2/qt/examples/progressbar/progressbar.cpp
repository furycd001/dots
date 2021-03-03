/****************************************************************************
** $Id: qt/examples/progressbar/progressbar.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "progressbar.h"

#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qprogressbar.h>
#include <qlayout.h>

/*
 * Constructor
 *
 * Creates child widgets of the ProgressBar widget
 */

ProgressBar::ProgressBar( QWidget *parent, const char *name )
    : QButtonGroup( 0, Horizontal, "Progress Bar", parent, name ), timer()
{
    setMargin( 10 );

    QGridLayout* toplayout = new QGridLayout( layout(), 2, 2, 5);
 
    setRadioButtonExclusive( TRUE );

    // insert three radiobuttons which the user can use
    // to set the speed of the progress and two pushbuttons
    // to start/pause/continue and reset the progress 
    slow = new QRadioButton( "&Slow", this );
    normal = new QRadioButton( "&Normal", this );
    fast = new QRadioButton( "&Fast", this );
    QVBoxLayout* vb1 = new QVBoxLayout;
    toplayout->addLayout( vb1, 0, 0 );
    vb1->addWidget( slow );
    vb1->addWidget( normal );
    vb1->addWidget( fast );

    // two push buttons, one for start, for for reset.
    start = new QPushButton( "&Start", this );
    reset = new QPushButton( "&Reset", this );
    QVBoxLayout* vb2 = new QVBoxLayout;
    toplayout->addLayout( vb2, 0, 1 );
    vb2->addWidget( start );
    vb2->addWidget( reset );
    
    // Create the progressbar
    progress = new QProgressBar( 100, this );
    toplayout->addMultiCellWidget( progress, 1, 1, 0, 1 );

    // connect the clicked() SIGNALs of the pushbuttons to SLOTs
    connect( start, SIGNAL( clicked() ), this, SLOT( slotStart() ) );
    connect( reset, SIGNAL( clicked() ), this, SLOT( slotReset() ) );

    // connect the timeout() SIGNAL of the progress-timer to a SLOT
    connect( &timer, SIGNAL( timeout() ), this, SLOT( slotTimeout() ) );

    // Let's start with normal speed...
    normal->setChecked( TRUE );
    
    
    // some contraints
    start->setFixedWidth( 80 );
    setMinimumWidth( 300 );
}

/*
 * SLOT slotStart
 *
 * This SLOT is called if the user clicks start/pause/continue
 * button
 */

void ProgressBar::slotStart()
{
    // If the progress bar is at the beginning...
    if ( progress->progress() == -1 ) {
        // ...set according to the checked speed-radiobutton
        // the number of steps which are needed to complete the process
        if ( slow->isChecked() )
            progress->setTotalSteps( 10000 );
        else if ( normal->isChecked() )
            progress->setTotalSteps( 1000 );
        else
            progress->setTotalSteps( 50 );

        // disable the speed-radiobuttons
        slow->setEnabled( FALSE );
        normal->setEnabled( FALSE );
        fast->setEnabled( FALSE );
    }

    // If the progress is not running...
    if ( !timer.isActive() ) {
        // ...start the timer (and so the progress) with a interval of 1 ms...
        timer.start( 1 );
        // ...and rename the start/pause/continue button to Pause
        start->setText( "&Pause" );
    } else { // if the prgress is running...
        // ...stop the timer (and so the prgress)...
        timer.stop();
        // ...and rename the start/pause/continue button to Continue
        start->setText( "&Continue" );
    }
}

/*
 * SLOT slotReset
 *
 * This SLOT is called when the user clicks the reset button
 */

void ProgressBar::slotReset()
{
    // stop the timer and progress
    timer.stop();

    // rename the start/pause/continue button to Start...
    start->setText( "&Start" );
    // ...and enable this button
    start->setEnabled( TRUE );

    // enable the speed-radiobuttons
    slow->setEnabled( TRUE );
    normal->setEnabled( TRUE );
    fast->setEnabled( TRUE );

    // reset the progressbar
    progress->reset();
}

/*
 * SLOT slotTimeout
 *
 * This SLOT is called each ms when the timer is 
 * active (== progress is running)
 */

void ProgressBar::slotTimeout()
{
    int p = progress->progress();

    // If the progress is complete...
    if ( p == progress->totalSteps() )  {
        // ...rename the start/pause/continue button to Start...
        start->setText( "&Start" );
        // ...and disable it...
        start->setEnabled( FALSE );
        // ...and return
        return;
    }
    
    // If the process is not complete increase it
    progress->setProgress( ++p );
}
