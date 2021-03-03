/****************************************************************************
** $Id: qt/examples/texture/main.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
//
// Qt OpenGL example: Texture
//
// File: main.cpp
//
// The main() function 
// 

#include "globjwin.h"
#include <qapplication.h>
#include <qgl.h>

/*
  The main program is here. 
*/

int main( int argc, char **argv )
{
    QApplication::setColorSpec( QApplication::CustomColor );
    QApplication a(argc,argv);

    if ( !QGLFormat::hasOpenGL() ) {
	qWarning( "This system has no OpenGL support. Exiting." );
	return -1;
    }
    
    GLObjectWindow* w = new GLObjectWindow;
    w->resize( 400, 350 );
    a.setMainWidget( w );
    w->setCaption("Qt Example - OpenGL - Texture");
    w->show();
    int result = a.exec();
    delete w;
    return result;
}
