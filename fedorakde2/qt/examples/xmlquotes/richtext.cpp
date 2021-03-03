/****************************************************************************
** $Id: qt/examples/xmlquotes/richtext.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "richtext.h"

#include <qhbox.h>
#include <qhbox.h>
#include <qpushbutton.h>
#include <qtextview.h>
#include <qbrush.h>
#include <qapplication.h>


MyRichText::MyRichText( const QStringList &s, QWidget *parent, const char *name )
    : QVBox( parent, name ), sayings( s )
{
    setMargin( 5 );

    view = new QTextView( this );
    view->setText( "This is a <b>Test</b> with <i>italic</i> <u>stuff</u>" );
    QBrush paper;
    paper.setPixmap( QPixmap( "../richtext/marble.png" ) );
    view->setPaper( paper );

    view->setText( sayings[0] );
    view->setMinimumSize( 450, 250 );

    QHBox *buttons = new QHBox( this );
    buttons->setMargin( 5 );

    bClose = new QPushButton( "&Close", buttons );
    bPrev = new QPushButton( "<< &Prev", buttons );
    bNext = new QPushButton( "&Next >>", buttons );

    bPrev->setEnabled( FALSE );

    connect( bClose, SIGNAL( clicked() ), qApp, SLOT( quit() ) );
    connect( bPrev, SIGNAL( clicked() ), this, SLOT( prev() ) );
    connect( bNext, SIGNAL( clicked() ), this, SLOT( next() ) );

    num = 0;
}

void MyRichText::prev()
{
    if ( num <= 0 )
        return;

    num--;

    view->setText( sayings[num] );

    if ( num == 0 )
        bPrev->setEnabled( FALSE );

    bNext->setEnabled( TRUE );
}

void MyRichText::next()
{
    if ( num >= (int)sayings.count()-1 )
        return;

    num++;

    view->setText( sayings[num] );

    if ( num >= (int)sayings.count()-1 )
        bNext->setEnabled( FALSE );

    bPrev->setEnabled( TRUE );
}
