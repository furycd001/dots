/****************************************************************************
** $Id: qt/examples/rot13/rot13.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "rot13.h"

#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qapplication.h>
#include <qlayout.h>

Rot13::Rot13()
{
    left = new QMultiLineEdit( this, "left" );
    right = new QMultiLineEdit( this, "right" );
    connect( left, SIGNAL(textChanged()), this, SLOT(changeRight()) );
    connect( right, SIGNAL(textChanged()), this, SLOT(changeLeft()) );

    QPushButton * quit = new QPushButton( "&Quit", this );
    quit->setFocusPolicy( NoFocus );
    connect( quit, SIGNAL(clicked()), qApp, SLOT(quit()) );

    QGridLayout * l = new QGridLayout( this, 2, 2, 5 );
    l->addWidget( left, 0, 0 );
    l->addWidget( right, 0, 1 );
    l->addWidget( quit, 1, 1, AlignRight );

    left->setFocus();
}


void Rot13::changeLeft()
{
    left->blockSignals( TRUE );
    left->setText( rot13( right->text() ) );
    left->blockSignals( FALSE );
}


void Rot13::changeRight()
{
    right->blockSignals( TRUE );
    right->setText( rot13( left->text() ) );
    right->blockSignals( FALSE );
}


QString Rot13::rot13( const QString & input ) const
{
    QString r = input;
    int i = r.length();
    while( i-- ) {
	if ( r[i] >= QChar('A') && r[i] <= QChar('M') ||
	     r[i] >= QChar('a') && r[i] <= QChar('m') )
	    r[i] = (char)((int)QChar(r[i]) + 13);
	else if  ( r[i] >= QChar('N') && r[i] <= QChar('Z') ||
		   r[i] >= QChar('n') && r[i] <= QChar('z') )
	    r[i] = (char)((int)QChar(r[i]) - 13);
    }
    return r;
}


int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    Rot13 r;
    r.resize( 400, 400 );
    a.setMainWidget( &r );
    r.setCaption("Qt Example - ROT13");
    r.show();
    return a.exec();
}
