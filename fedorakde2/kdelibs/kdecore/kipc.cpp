/* This file is part of the KDE libraries

   Copyright (C) 1999 Mattias Ettrich (ettrich@kde.org)
   Copyright (C) 1999,2000 Geert Jansen <jansen@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


/*
 * kipc.cpp: Send a message to one/all KDE apps.
 *
 * $Id$
 */

#include <qwindowdefs.h>

#include <X11/X.h>
#include <X11/Xlib.h>

#include <kipc.h>


static int dropError(Display *, XErrorEvent *)
{
    return 0;
}


static long getSimpleProperty(Window w, Atom a)
{
    Atom real_type;
    int format;
    unsigned long n, extra, *p = 0, res;
    int status;

    res = 0;
    status = XGetWindowProperty(qt_xdisplay(), w, a, 0L, 1L, False, a,
            &real_type, &format, &n, &extra, (unsigned char **) &p);
    if ((status == Success) && (n == 1) && (format == 32))
	res = p[0];
    if (p) XFree((char *) p);
    return res;
}


void KIPC::sendMessage(Message msg, WId w, int data)
{
    static Atom a = 0;
    if (a == 0)
	a = XInternAtom(qt_xdisplay(), "KIPC_COMM_ATOM", False);
    XEvent ev;
    ev.xclient.type = ClientMessage;
    ev.xclient.display = qt_xdisplay();
    ev.xclient.window = (Window) w;
    ev.xclient.message_type = a;
    ev.xclient.format = 32;
    ev.xclient.data.l[0] = msg;
    ev.xclient.data.l[1] = data;
    XSendEvent(qt_xdisplay(), (Window) w, False, 0L, &ev);

    // KDE 1 support
    static Atom kde1 = 0;
    if ( msg == PaletteChanged || msg == FontChanged ) {
	if ( kde1 == 0 )
	    kde1 = XInternAtom(qt_xdisplay(), "KDEChangeGeneral", False );
	ev.xclient.message_type = kde1;
	XSendEvent(qt_xdisplay(), (Window) w, False, 0L, &ev);
    }

}


void KIPC::sendMessageAll(Message msg, int data)
{
    unsigned int i, nrootwins;
    Window dw1, dw2, *rootwins = 0;
    int (*defaultHandler)(Display *, XErrorEvent *);
    Display *dpy = qt_xdisplay();
    int screen_count = ScreenCount(dpy);

    defaultHandler = XSetErrorHandler(&dropError);

    for (int s = 0; s < screen_count; s++) {
	Window root = RootWindow(dpy, s);

	XQueryTree(dpy, root, &dw1, &dw2, &rootwins, &nrootwins);
	Atom a = XInternAtom(qt_xdisplay(), "KDE_DESKTOP_WINDOW", False);
	for (i = 0; i < nrootwins; i++)
	    {
		if (getSimpleProperty(rootwins[i], a) != 0L)
		    sendMessage(msg, rootwins[i], data);
	    }
    }

    XFlush(dpy);
    XSetErrorHandler(defaultHandler);
    XFree((char *) rootwins);
}
