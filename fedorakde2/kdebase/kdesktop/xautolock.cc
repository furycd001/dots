//----------------------------------------------------------------------------
//
// This file is part of the KDE project
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//
// KDE screensaver engine
//
// This module is a heavily modified xautolock.
// In fact as of KDE 2.0 this code is practically unrecognisable as xautolock.
//
// The orignal copyright notice follows
//

/*****************************************************************************
 *
 * xautolock
 * =========
 *
 * Authors   :  S. De Troch (SDT) + M. Eyckmans (MCE)
 *
 * Date      :  22/07/90
 *
 * ---------------------------------------------------------------------------
 *
 * Copyright 1990, 1992-1995 by S. De Troch and MCE.
 *
 * Permission to use, copy, modify and distribute this software and the
 * supporting documentation without fee is hereby granted, provided that
 *
 *  1 : Both the above copyright notice and this permission notice
 *      appear in all copies of both the software and the supporting
 *      documentation.
 *  2 : No financial profit is made out of it.
 *
 * THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL THEY BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
 * OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 *****************************************************************************/

/*
 * A short word about how this works:
 *
 * XAutoLock is a class that detects user inactivity. User inactivity is
 * defined as not touching the mouse or keyboard. It is detected like this:
 *
 * Mouse: We periodically (5 sec) query the mouse position and see if it has
 * changed. The possibility that the mouse is moved and then moved back to 
 * its original position is neglected.
 *
 * Keyboard: This is not so simple. We query the window tree, and select for
 * KeyPress events on every window. Because new windows can be created, we
 * have to select for SubstructureNotify too, so that we can watch future
 * windows.
 *
 * If the timeout period expires without a key or mouse event taking place,
 * the signal timeout() is raised.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <qapplication.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>
// The following include is to make --enable-final work
#include <X11/Xutil.h>

#undef Bool
#undef Unsorted

#include "xautolock.h"
#include "xautolock.moc"

#define DEFAULT_TIMEOUT           600
#define CREATION_DELAY             30      /* should be > 10 and
                                              < min (45,(MIN_MINUTES*30))  */
#define TIME_CHANGE_LIMIT         120      /* if the time changes by more
                                              than x secs then we will
                                              assume someone has changed
                                              date or machine has suspended */

int catchFalseAlarms(Display *, XErrorEvent *)
{
	return 0;
}

template class QQueue<WatchedWindow>;

//===========================================================================
//
// Detect user inactivity.
// Named XAutoLock after the program that it is based on.
//
XAutoLock::XAutoLock()
{
    mWindows.setAutoDelete(true);

    int (*oldHandler)(Display *, XErrorEvent *);
    oldHandler = XSetErrorHandler(catchFalseAlarms);

    XSync(qt_xdisplay(), 0);

    for (int s = 0; s < ScreenCount(qt_xdisplay()); s++)
    {
        Window r = RootWindowOfScreen(ScreenOfDisplay(qt_xdisplay(), s));
        mWindows.enqueue(new WatchedWindow(r));
        selectEvents(r, true);
    }

    XSetErrorHandler(oldHandler);

    mTimeout = DEFAULT_TIMEOUT;
    resetTrigger();

    time(&mLastTimeout);
    mActive = false;

    mTimerId = startTimer(5000);
}

//---------------------------------------------------------------------------
//
// Destructor.
//
XAutoLock::~XAutoLock()
{
  int (*oldHandler)(Display *, XErrorEvent *);
  oldHandler = XSetErrorHandler(catchFalseAlarms);
  XSetErrorHandler(oldHandler);
}

//---------------------------------------------------------------------------
//
// The time in seconds of continuous inactivity.
//
void XAutoLock::setTimeout(int t)
{
    mTimeout = t;
    resetTrigger();
}

//---------------------------------------------------------------------------
//
// Start watching Activity
//
void XAutoLock::start()
{
    resetTrigger();
    time(&mLastTimeout);
    mActive = true;
}

//---------------------------------------------------------------------------
//
// Stop watching Activity
//
void XAutoLock::stop()
{
    mActive = false;
}

//---------------------------------------------------------------------------
//
// Reset the trigger time.
//
void XAutoLock::resetTrigger()
{
    mTrigger = time(0) + mTimeout;
}

//---------------------------------------------------------------------------
//
// Select events on newly added windows.
//
void XAutoLock::processWatched(time_t delay)
{
    time_t now = time(0);

    while (mWindows.current() && mWindows.current()->created() + delay < now )
    {
        selectEvents(mWindows.current()->window(), false);
        mWindows.remove();
    }
}

//---------------------------------------------------------------------------
//
// Select the events we want to watch on each window.
// This code is the most intact xautolock code remaining.
//
void XAutoLock::selectEvents(Window window, bool substructure_only)
{
    Window             root;              /* root window of this window */
    Window             parent;            /* parent of this window      */
    Window*            children;          /* children of this window    */
    unsigned           nof_children = 0;  /* number of children         */
    XWindowAttributes  attribs;           /* attributes of the window   */

    Display *d = qt_xdisplay();

    /*
     * Don't mess with our own widgets. We already get events for them.
     */

    if (QWidget::find(window) && (window != qt_xrootwin()))
	return;

    /*
     *  Start by querying the server about parent and child windows.
     */
    if (!XQueryTree (d, window, &root, &parent, &children, &nof_children))
    {
        return;
    }

    /*
     *  Build the appropriate event mask. The basic idea is that we don't
     *  want to interfere with the normal event propagation mechanism if
     *  we don't have to.
     */
    if (substructure_only)
    {
        XSelectInput(d, window, SubstructureNotifyMask);
    }
    else
    {
	if (XGetWindowAttributes(d, window, &attribs) == 0)
	{
	    if (nof_children)
	    {
		XFree(children);
	    }
	    return;
	}

	XSelectInput(d, window, SubstructureNotifyMask |
		     ((attribs.all_event_masks | attribs.do_not_propagate_mask)
		      & KeyPressMask) | attribs.your_event_mask);
    }

    /*
     *  Now do the same thing for all children.
     */
    for (unsigned i = 0; i < nof_children; i++)
    {
        selectEvents(children[i], substructure_only);
    }

    if (nof_children)
    {
        XFree(children);
    }
}

//---------------------------------------------------------------------------
//
// Query the position of the pointer to see if it has moved.
//
void XAutoLock::queryPointer()
{
    Display *d = qt_xdisplay();
    Window           dummy_w;
    int              dummy_c;
    unsigned         mask;               /* modifier mask                 */
    int              root_x;
    int              root_y;
    static Window    root;               /* root window the pointer is on */
    static Screen*   screen;             /* screen the pointer is on      */
    static unsigned  prev_mask = 0;
    static int       prev_root_x = -1;
    static int       prev_root_y = -1;
    static bool      first_call = true;

    if (first_call)
    {
        first_call = false;
        root = DefaultRootWindow (d);
        screen = ScreenOfDisplay (d, DefaultScreen (d));
    }

    /*
     *  Find out whether the pointer has moved. Using XQueryPointer for this
     *  is gross, but it also is the only way never to mess up propagation
     *  of pointer events.
     *
     *  Remark : Unlike XNextEvent(), XPending () doesn't notice if the
     *           connection to the server is lost. For this reason, earlier
     *           versions of xautolock periodically called XNoOp (). But
     *           why not let XQueryPointer () do the job for us, since
     *           we now call that periodically anyway?
     */
    if (!XQueryPointer (d, root, &root, &dummy_w, &root_x, &root_y,
                      &dummy_c, &dummy_c, &mask))
    {
        /*
        *  Pointer has moved to another screen, so let's find out which one.
        */
        for (int i = 0; i < ScreenCount(d); i++) 
        {
            if (root == RootWindow(d, i)) 
            {
                screen = ScreenOfDisplay (d, i);
                break;
            }
        }
    }

    if (root_x != prev_root_x || root_y != prev_root_y || mask != prev_mask)
    {
        prev_root_x = root_x;
        prev_root_y = root_y;
        prev_mask = mask;
        resetTrigger();
    }
}

//---------------------------------------------------------------------------
//
// Process new windows and check the mouse.
//
void XAutoLock::timerEvent(QTimerEvent *ev)
{
    if (ev->timerId() != mTimerId)
    {
        return;
    }

    int (*oldHandler)(Display *, XErrorEvent *);
    oldHandler = XSetErrorHandler(catchFalseAlarms);

    processWatched((time_t) CREATION_DELAY);

    time_t now = time(0);
    if ((now > mLastTimeout && now - mLastTimeout > TIME_CHANGE_LIMIT) ||
        (mLastTimeout > now && mLastTimeout - now > TIME_CHANGE_LIMIT+1))
    {
        /* the time has changed in one large jump.  This could be because
           the date was changed, or the machine was suspended.  We'll just
           reset the triger. */
        resetTrigger();
    }

    mLastTimeout = now;
          
    queryPointer();

    XSetErrorHandler(oldHandler);

    if (now >= mTrigger)
    {
        resetTrigger();
        if (mActive)
        {
            emit timeout();
        }
    }
}

//---------------------------------------------------------------------------
//
// A new window has been created.
//
void XAutoLock::windowCreated(Window w)
{
    mWindows.enqueue(new WatchedWindow(w));
}

//---------------------------------------------------------------------------
//
// The user pressed a key.
//
void XAutoLock::keyPressed()
{
    resetTrigger();
}

