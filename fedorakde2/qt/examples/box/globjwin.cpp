/****************************************************************************
** $Id: qt/examples/box/globjwin.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qpushbutton.h>
#include <qslider.h>
#include <qlayout.h>
#include <qframe.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qapplication.h>
#include <qkeycode.h>
#include "globjwin.h"
#include "glbox.h"


GLObjectWindow::GLObjectWindow( QWidget* parent, const char* name )
    : QWidget( parent, name )
{

    // Create a menu
    QPopupMenu *file = new QPopupMenu( this );
    file->insertItem( "Exit",  qApp, SLOT(quit()), CTRL+Key_Q );

    // Create a menu bar
    QMenuBar *m = new QMenuBar( this );
    m->setSeparator( QMenuBar::InWindowsStyle );
    m->insertItem("&File", file );

    // Create a nice frame to put around the OpenGL widget
    QFrame* f = new QFrame( this, "frame" );
    f->setFrameStyle( QFrame::Sunken | QFrame::Panel );
    f->setLineWidth( 2 );

    // Create our OpenGL widget
    GLBox* c = new GLBox( f, "glbox");

    // Create the three sliders; one for each rotation axis
    QSlider* x = new QSlider ( 0, 360, 60, 0, QSlider::Vertical, this, "xsl" );
    x->setTickmarks( QSlider::Left );
    QObject::connect( x, SIGNAL(valueChanged(int)),c,SLOT(setXRotation(int)) );

    QSlider* y = new QSlider ( 0, 360, 60, 0, QSlider::Vertical, this, "ysl" );
    y->setTickmarks( QSlider::Left );
    QObject::connect( y, SIGNAL(valueChanged(int)),c,SLOT(setYRotation(int)) );

    QSlider* z = new QSlider ( 0, 360, 60, 0, QSlider::Vertical, this, "zsl" );
    z->setTickmarks( QSlider::Left );
    QObject::connect( z, SIGNAL(valueChanged(int)),c,SLOT(setZRotation(int)) );


    // Now that we have all the widgets, put them into a nice layout

    // Put the sliders on top of each other
    QVBoxLayout* vlayout = new QVBoxLayout( 20, "vlayout");
    vlayout->addWidget( x );
    vlayout->addWidget( y );
    vlayout->addWidget( z );

    // Put the GL widget inside the frame
    QHBoxLayout* flayout = new QHBoxLayout( f, 2, 2, "flayout");
    flayout->addWidget( c, 1 );

    // Top level layout, puts the sliders to the left of the frame/GL widget
    QHBoxLayout* hlayout = new QHBoxLayout( this, 20, 20, "hlayout");
    hlayout->setMenuBar( m );
    hlayout->addLayout( vlayout );
    hlayout->addWidget( f, 1 );
}
