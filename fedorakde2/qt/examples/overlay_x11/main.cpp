/****************************************************************************
** $Id: qt/examples/overlay_x11/main.cpp   2.3.2   edited 2001-01-26 $
**
** Example application showing how to use Qt and Qt OpenGL Extension on an 
** X11 overlay visual 
**
** Copyright (C) 1999 by Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qapplication.h>
#include "gearwidget.h"
#include "rubberbandwidget.h"

#if defined(_WS_X11_)
#include <X11/Xlib.h>
#endif

QColor findOverlayTransparentColor()
{
    QColor invalidColor;

#if defined(_WS_X11_)

    Display* appDisplay;
    Visual* appVisual;

    // The static methods are called 'App' in Qt 2.x
#if QT_VERSION < 200
    appDisplay = QPaintDevice::x__Display();
    appVisual = (Visual*)QPaintDevice::x11Visual();
#else
    appDisplay = QPaintDevice::x11AppDisplay();
    appVisual = (Visual*)QPaintDevice::x11AppVisual();
#endif

    debug( "Default Visual ID: 0x%x", (int)XVisualIDFromVisual(appVisual) );

    typedef struct OverlayProp {
	long  visual;
	long  type;
	long  value;
	long  layer;
    } OverlayProp;

    QWidget* rootWin = QApplication::desktop();
    if ( !rootWin )
	return invalidColor; // Should not happen

    Atom overlayVisualsAtom = XInternAtom( appDisplay, 
					   "SERVER_OVERLAY_VISUALS", True );
    if ( overlayVisualsAtom == None ) {
	warning( "Server has no overlay visuals" );
	return invalidColor;
    }

    Atom actualType;
    int actualFormat;
    ulong nItems;
    ulong bytesAfter;
    OverlayProp* overlayProp;
    int res = XGetWindowProperty( appDisplay, QApplication::desktop()->winId(),
				  overlayVisualsAtom, 0, 10000, False, 
				  overlayVisualsAtom, &actualType, 
				  &actualFormat, &nItems, &bytesAfter,
				  (uchar**)&overlayProp );

    if ( res != Success || actualType != overlayVisualsAtom 
	 || actualFormat != 32 || nItems < 4 ) {
	warning( "Failed to get overlay visual property from server" );
	return invalidColor;
    }


    for ( uint i = 0; i < nItems/4; i++ ) {
	if ( (VisualID)overlayProp[i].visual == XVisualIDFromVisual(appVisual)
	     && overlayProp[i].type == 1 )
	    return QColor( qRgb( 1, 2, 3 ), overlayProp[i].value );
    }

    warning( "Default visual is not in overlay plane" );
    return invalidColor;

#else // defined(_WS_X11_)
    warning( "Wrong window system - Only X11 has overlay support." );
    return invalidColor;
#endif
}


int main( int argc, char **argv )
{
    QApplication::setColorSpec( QApplication::CustomColor );
    QApplication a( argc, argv );

    if ( !QGLFormat::hasOpenGL() ) {
	warning( "This system has no OpenGL support. Exiting." );
	return -1;
    }

    QColor transparentColor = findOverlayTransparentColor();
    if ( !transparentColor.isValid() ) {
	warning( "Failed to get transparent color for overlay. Exiting." );
	return -1;
    }

    QWidget top;
    a.setMainWidget( &top );
    top.setGeometry( 50, 50, 600, 400 );

    // Make an OpenGL widget. It will use the deepest visual available
    // (typically a TrueColor visual), which typically is in the normal layer.
    GearWidget g( &top );
    g.setGeometry( 20, 20, 560, 360 );

    // Put the rubberband widget (which uses the default, i.e. overlay visual)
    // on top of the OpenGL widget:
    RubberbandWidget r( transparentColor, &top );
    r.setGeometry( 20, 20, 560, 360 );

    top.show();
    return a.exec();
}
