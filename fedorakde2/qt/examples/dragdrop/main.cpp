/****************************************************************************
** $Id: qt/examples/dragdrop/main.cpp   2.3.2   edited 2001-06-12 $
**
** Ritual main() for Qt applications
**
** Copyright (C) 1996 by Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qapplication.h>
#include "dropsite.h"
#include "secret.h"
#include <qlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qpixmap.h>

static void addStuff( QWidget * parent, bool image, bool secret = FALSE )
{
    QVBoxLayout * tll = new QVBoxLayout( parent, 10 );
    DropSite * d = new DropSite( parent );
    d->setFrameStyle( QFrame::Sunken + QFrame::WinPanel );
    tll->addWidget( d );
    if ( image ) {
	QPixmap stuff;
	if ( !stuff.load( "trolltech.bmp" ) ) {
	    stuff = QPixmap(20,20);
	    stuff.fill(Qt::green);
	}
	d->setPixmap( stuff );
    } else {
	d->setText("Drag and Drop");
    }
    d->setFont(QFont("Helvetica",18));
    if ( secret ) {
	SecretSource *s = new SecretSource( 42, parent );
	tll->addWidget( s );
    }

    QLabel * format = new QLabel( "\n\n\n\nNone\n\n\n\n", parent );
    tll->addWidget( format );
    tll->activate();
    parent->resize( parent->sizeHint() );

    QObject::connect( d, SIGNAL(message(const QString&)),
		      format, SLOT(setText(const QString&)) );
}


int main( int argc, char ** argv )
{
    QApplication a( argc, argv );

    QWidget mw;
    addStuff( &mw, TRUE );
    mw.setCaption( "Qt Example - Drag and Drop" );
    mw.show();

    QWidget mw2;
    addStuff( &mw2, FALSE );
    mw2.setCaption( "Qt Example - Drag and Drop" );
    mw2.show();

    QWidget mw3;
    addStuff( &mw3, TRUE, TRUE );
    mw3.setCaption( "Qt Example - Drag and Drop" );
    mw3.show();

    QObject::connect(qApp,SIGNAL(lastWindowClosed()),qApp,SLOT(quit()));
    return a.exec();
}
