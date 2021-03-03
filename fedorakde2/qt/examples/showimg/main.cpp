/****************************************************************************
** $Id: qt/examples/showimg/main.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "showimg.h"
#include "imagefip.h"
#include <qapplication.h>
#include <qimage.h>

int main( int argc, char **argv )
{
    if ( argc > 1 && QString(argv[1]) == "-m" ) {
	QApplication::setColorSpec( QApplication::ManyColor );
	argc--;
	argv++;
    } 
    else if ( argc > 1 && QString(argv[1]) == "-n" ) {
	QApplication::setColorSpec( QApplication::NormalColor );
	argc--;
	argv++;
    } 
    else {
	QApplication::setColorSpec( QApplication::CustomColor );
    }

    QApplication::setFont( QFont("Helvetica", 12) );
    QApplication a( argc, argv );

    QFileDialog::setIconProvider(new ImageIconProvider);

    if ( argc <= 1 ) {
	// Create a window which looks after its own existence.
	ImageViewer *w =
	    new ImageViewer(0, "new window", Qt::WDestructiveClose | Qt::WResizeNoErase );
	w->setCaption("Qt Example - Image Viewer");
	w->show();
    } else {
	for ( int i=1; i<argc; i++ ) {
	    // Create a window which looks after its own existence.
	    ImageViewer *w =
		new ImageViewer(0, argv[i], Qt::WDestructiveClose | Qt::WResizeNoErase );
	    w->setCaption("Qt Example - Image Viewer");
	    w->loadImage( argv[i] );
	    w->show();
	}
    }

    QObject::connect(qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()));

    return a.exec();
}
