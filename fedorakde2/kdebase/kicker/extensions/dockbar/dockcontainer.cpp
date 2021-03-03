/*****************************************************************

Copyright (c) 2000 Matthias Elter <elter@kde.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include <qwidget.h>
#include <kwin.h>

#include "dockcontainer.h"
#include "dockcontainer.moc"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>


DockContainer::DockContainer( QString command, QWidget *parent, const char* name )
    : QFrame( parent, name ), _embeddedWinId(0), _command(command)
{
    XSelectInput( qt_xdisplay(), winId(),
		  KeyPressMask | KeyReleaseMask |
		  ButtonPressMask | ButtonReleaseMask |
		  KeymapStateMask |
		  ButtonMotionMask |
		  PointerMotionMask |
		  EnterWindowMask | LeaveWindowMask |
		  FocusChangeMask |
		  ExposureMask |
		  StructureNotifyMask |
		  SubstructureRedirectMask |
		  SubstructureNotifyMask );
    
    setFrameStyle(StyledPanel | Raised);
    setLineWidth(1);
}

void DockContainer::embed( WId id )
{
    if( id == _embeddedWinId )
        return;

    QRect geom = KWin::info(id).geometry;

    // does the same as KWM::prepareForSwallowing()
    XWithdrawWindow( qt_xdisplay(), id, qt_xscreen() );
    while( KWin::info(id).state != WithdrawnState );

    // place application in the middle of the app frame
    /* if( (geom.width() < width()) ||
        (geom.height() < height()) )
        XReparentWindow( qt_xdisplay(), id, winId(),
    			 (width()-geom.width())/2,
    			 (height()-geom.height())/2 );
    else
    */
        XReparentWindow( qt_xdisplay(), id, winId(), 0, 0 );

    // resize if window is bigger than frame
    if( (geom.width() > width()) ||
        (geom.height() > height()) )
        XResizeWindow( qt_xdisplay(), id, width(), height() );

    XMapWindow( qt_xdisplay(), id );
    XUngrabButton( qt_xdisplay(), AnyButton, AnyModifier, winId() );

    _embeddedWinId = id;
}

void DockContainer::unembed()
{
    if( _embeddedWinId )
        XReparentWindow( qt_xdisplay(), _embeddedWinId, qt_xrootwin(), 0, 0 );
}

WId DockContainer::embeddedWinId()
{
    return _embeddedWinId;
}

bool DockContainer::x11Event( XEvent *e )
{
    switch( e->type ) {
    case DestroyNotify:
	if( e->xdestroywindow.window == _embeddedWinId ) {
	    _embeddedWinId = 0;
	    emit embededWindowDestroyed(this);
	}
	break;
    case ReparentNotify:
	if( _embeddedWinId &&
	    (e->xreparent.window == _embeddedWinId) &&
	    (e->xreparent.parent != winId()) ) {
	    _embeddedWinId = 0;
	}
	else if( e->xreparent.parent == winId() ) {
	    _embeddedWinId = e->xreparent.window;
	    embed( _embeddedWinId );
	}
	break;
    }

    return false;
}
