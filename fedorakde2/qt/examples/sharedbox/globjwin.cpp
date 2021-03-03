/****************************************************************************
** $Id: qt/examples/sharedbox/globjwin.cpp   2.3.2   edited 2001-01-26 $
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
    file->insertItem( "Delete Left QGLWidget", this, 
		      SLOT(deleteFirstWidget()) );
    file->insertItem( "Exit",  qApp, SLOT(quit()), CTRL+Key_Q );

    // Create a menu bar
    QMenuBar *m = new QMenuBar( this );
    m->setSeparator( QMenuBar::InWindowsStyle );
    m->insertItem("&File", file );

    // Create nice frames to put around the OpenGL widgets
    QFrame* f1 = new QFrame( this, "frame1" );
    f1->setFrameStyle( QFrame::Sunken | QFrame::Panel );
    f1->setLineWidth( 2 );
    QFrame* f2 = new QFrame( this, "frame2" );
    f2->setFrameStyle( QFrame::Sunken | QFrame::Panel );
    f2->setLineWidth( 2 );

    // Create an OpenGL widget
    c1 = new GLBox( f1, "glbox1" );
    
    // Create another OpenGL widget that shares display lists with the first
    c2 = new GLBox( f2, "glbox2", c1 );

    // Create the three sliders; one for each rotation axis
    // Make them spin the boxes, but not in synch
    QSlider* x = new QSlider ( 0, 360, 60, 0, QSlider::Vertical, this, "xsl" );
    x->setTickmarks( QSlider::Left );
    connect( x, SIGNAL(valueChanged(int)), c1, SLOT(setXRotation(int)) );
    connect( x, SIGNAL(valueChanged(int)), c2, SLOT(setZRotation(int)) );

    QSlider* y = new QSlider ( 0, 360, 60, 0, QSlider::Vertical, this, "ysl" );
    y->setTickmarks( QSlider::Left );
    connect( y, SIGNAL(valueChanged(int)), c1, SLOT(setYRotation(int)) );
    connect( y, SIGNAL(valueChanged(int)), c2, SLOT(setXRotation(int)) );

    QSlider* z = new QSlider ( 0, 360, 60, 0, QSlider::Vertical, this, "zsl" );
    z->setTickmarks( QSlider::Left );
    connect( z, SIGNAL(valueChanged(int)), c1, SLOT(setZRotation(int)) );
    connect( z, SIGNAL(valueChanged(int)), c2, SLOT(setYRotation(int)) );


    // Now that we have all the widgets, put them into a nice layout

    // Put the sliders on top of each other
    QVBoxLayout* vlayout = new QVBoxLayout( 20, "vlayout");
    vlayout->addWidget( x );
    vlayout->addWidget( y );
    vlayout->addWidget( z );

    // Put the GL widgets inside the frames
    QHBoxLayout* flayout1 = new QHBoxLayout( f1, 2, 2, "flayout1");
    flayout1->addWidget( c1, 1 );
    QHBoxLayout* flayout2 = new QHBoxLayout( f2, 2, 2, "flayout2");
    flayout2->addWidget( c2, 1 );

    // Top level layout, puts the sliders to the left of the frame/GL widget
    QHBoxLayout* hlayout = new QHBoxLayout( this, 20, 20, "hlayout");
    hlayout->setMenuBar( m );
    hlayout->addLayout( vlayout );
    hlayout->addWidget( f1, 1 );
    hlayout->addWidget( f2, 1 );

}


void GLObjectWindow::deleteFirstWidget()
{
    // Delete only c1; c2 will keep working and use the shared display list
    if ( c1 ) {
	delete c1;
	c1 = 0;
    }
}
