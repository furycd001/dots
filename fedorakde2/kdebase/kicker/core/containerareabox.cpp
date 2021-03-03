/*****************************************************************

Copyright (c) 1996-2000 the kicker authors. See file AUTHORS.

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

#include <qapplication.h>
#include <qrect.h>

#include <kdebug.h>

// X11/Qt conflict
#undef Unsorted

#include "containerareabox.h"
#include "containerareabox.moc"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

ContainerAreaBox::ContainerAreaBox( QWidget* parent, const char* name, WFlags f, bool allowLines )
    : QVBox( parent, name, f, allowLines )
{
  _activeWidget = 0;
  _x11EventFilterEnabled = false;
}

bool ContainerAreaBox::x11Event( XEvent* e )
{
    if (!_x11EventFilterEnabled)
	return false;

    // "Catch" XEvents which occur on the panel's edges, and redirect them
    // to the apropriate applets in the appletarea. This way the panel obeys
    // Fitt's law.
    switch ( e->type ) {

    case ButtonPress:
	{
	    // Only ButtonPress events which occur on the widget's frame need to
	    // be handled.
	    if (contentsRect().contains( QPoint(e->xbutton.x, e->xbutton.y) ) ||
		!rect().contains( QPoint(e->xbutton.x, e->xbutton.y) ))
		return false;

	    // Determine the difference between the catched event's position and
	    // the position of the new event that we will contruct. The new
	    // position may not be on the frame, but has to be in
	    // contentsRect(). Using a difference is easier because it can be
	    // applied it to both the local and global positions.
	    int dx, dy;
	    dx = QMAX( 0, contentsRect().left() - e->xbutton.x ) ;
	    dy = QMAX( 0, contentsRect().top() - e->xbutton.y );
	    if (dx == 0)
		dx = QMIN( 0, contentsRect().right() - e->xbutton.x );
	    if (dy == 0)
		dy = QMIN( 0, contentsRect().bottom() - e->xbutton.y );

	    // The widget which will be the destination of the new event.
	    QWidget* destWidget = QApplication::widgetAt(e->xbutton.x_root + dx,
						e->xbutton.y_root + dy, true);

	    // If there is no such widget, we leave the event to Qt's event
	    // handler. If destWidget is equal to this widget pass it to Qt's
	    // event handler too to avoid nasty loops.
	    if (!destWidget || destWidget == this)
		return false;

	    // Now construct the new event.
	    XEvent ne;
	    memset(&ne, 0, sizeof(ne));
	    ne = *e;
	    ne.xbutton.window = destWidget->winId();
	    Window child; // Not used
	    XTranslateCoordinates(qt_xdisplay(), winId(), destWidget->winId(),
				  e->xbutton.x + dx, e->xbutton.y + dy,
				  &ne.xbutton.x, &ne.xbutton.y, &child);
	    ne.xbutton.x_root = e->xbutton.x_root + dx;
	    ne.xbutton.y_root = e->xbutton.y_root + dy;

	    // Pretty obvious... Send the event.
	    XSendEvent(qt_xdisplay(), destWidget->winId(), false, NoEventMask, &ne);

	    // Make the receiver our active widget. It will receive all events
	    // until the mouse button is released.
	    _activeWidget = destWidget;

	    // We're done with this event.
	    return true;
	}

	// The rest of the cases are more or less a duplication of the first
	// one with off course some minor differences. ButtonRelease is almost
	// the same as MotionNotify, but there's xbutton and xmotion.
    case ButtonRelease:
	{
	    // Handle events outside the widget's rectangle too, since the mouse
	    // can be grabbed.
	    if (contentsRect().contains( QPoint(e->xbutton.x, e->xbutton.y) ))
		return false;

	    int dx, dy;
	    dx = QMAX( 0, contentsRect().left() - e->xbutton.x ) ;
	    dy = QMAX( 0, contentsRect().top() - e->xbutton.y );
	    if (dx == 0)
		dx = QMIN( 0, contentsRect().right() - e->xbutton.x );
	    if (dy == 0)
		dy = QMIN( 0, contentsRect().bottom() - e->xbutton.y );

	    // If there is a widget active it should receive the new event.
	    QWidget* destWidget;
	    if (_activeWidget)
		destWidget = _activeWidget;
	    else
		destWidget = QApplication::widgetAt(e->xbutton.x_root + dx,
						e->xbutton.y_root + dy, true);
		
	    if (!destWidget || destWidget == this)
		return false;
	
	    // The event's position can be outside the widget as well, so
	    // there's no need to adjust the position.
	    if (!rect().contains( QPoint(e->xbutton.x, e->xbutton.y)) ) {
		dx = 0;
		dy = 0;
	    }

	    XEvent ne;
	    memset(&ne, 0, sizeof(ne));
	    ne = *e;
	    ne.xbutton.window = destWidget->winId();
	    Window child;
	    XTranslateCoordinates(qt_xdisplay(), winId(), destWidget->winId(),
				  e->xbutton.x + dx, e->xbutton.y + dy,
				  &ne.xbutton.x, &ne.xbutton.y, &child);
	    ne.xbutton.x_root = e->xbutton.x_root + dx;
	    ne.xbutton.y_root = e->xbutton.y_root + dy;

	    XSendEvent(qt_xdisplay(), destWidget->winId(), false, NoEventMask, &ne);

	    // Turn off the active widget.
	    _activeWidget = 0;

	    return true;
	}

    case MotionNotify:
	{
	    if (contentsRect().contains( QPoint(e->xmotion.x, e->xmotion.y) ))
		return false;

	    int dx, dy;
	    dx = QMAX( 0, contentsRect().left() - e->xmotion.x ) ;
	    dy = QMAX( 0, contentsRect().top() - e->xmotion.y );
	    if (dx == 0)
		dx = QMIN( 0, contentsRect().right() - e->xmotion.x );
	    if (dy == 0)
		dy = QMIN( 0, contentsRect().bottom() - e->xmotion.y );

	    QWidget* destWidget;
	    if (_activeWidget)
		destWidget = _activeWidget;
	    else
		destWidget = QApplication::widgetAt(e->xmotion.x_root + dx,
						e->xmotion.y_root + dy, true);
		
	    if (!destWidget || destWidget == this)
		return false;

	    if (!rect().contains( QPoint(e->xmotion.x, e->xmotion.y)) ) {
		dx = 0;
		dy = 0;
	    }

	    XEvent ne;
	    memset(&ne, 0, sizeof(ne));
	    ne = *e;
	    ne.xmotion.window = destWidget->winId();
	    Window child;
	    XTranslateCoordinates(qt_xdisplay(), winId(), destWidget->winId(),
				  e->xmotion.x + dx, e->xmotion.y + dy,
				  &ne.xmotion.x, &ne.xmotion.y, &child);
	    ne.xmotion.x_root = e->xmotion.x_root + dx;
	    ne.xmotion.y_root = e->xmotion.y_root + dy;

	    XSendEvent(qt_xdisplay(), destWidget->winId(), false, NoEventMask, &ne);

	    return true;
	}

    case EnterNotify:
    case LeaveNotify:
	{
	    if (contentsRect().contains(
		QPoint(e->xcrossing.x, e->xcrossing.y) ) ||
		!rect().contains( QPoint(e->xcrossing.x, e->xcrossing.y) ))
		return false;

	    int dx, dy;
	    dx = QMAX( 0, contentsRect().left() - e->xcrossing.x ) ;
	    dy = QMAX( 0, contentsRect().top() - e->xcrossing.y );
	    if (dx == 0)
		dx = QMIN( 0, contentsRect().right() - e->xcrossing.x );
	    if (dy == 0)
		dy = QMIN( 0, contentsRect().bottom() - e->xcrossing.y );

	    QWidget* destWidget;
	    if (_activeWidget)
		destWidget = _activeWidget;
	    else
		destWidget = QApplication::widgetAt(e->xcrossing.x_root + dx,
						e->xcrossing.y_root + dy, true);
		
	    if (!destWidget || destWidget == this)
		return false;

	    if (!rect().contains( QPoint(e->xcrossing.x, e->xcrossing.y)) ) {
		dx = 0;
		dy = 0;
	    }

	    XEvent ne;
	    memset(&ne, 0, sizeof(ne));
	    ne = *e;
	    ne.xcrossing.window = destWidget->winId();
	    Window child;
	    XTranslateCoordinates(qt_xdisplay(), winId(), destWidget->winId(),
				  e->xcrossing.x + dx, e->xcrossing.y + dy,
				  &ne.xcrossing.x, &ne.xcrossing.y, &child);
	    ne.xcrossing.x_root = e->xcrossing.x_root + dx;
	    ne.xcrossing.y_root = e->xcrossing.y_root + dy;

	    XSendEvent(qt_xdisplay(), destWidget->winId(), false, NoEventMask, &ne);

	    return true;
	}
    default:
	break;
    }

    return false;
}
