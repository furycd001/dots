/****************************************************************************

 $Id$

 Copyright (C) 2001 Lubos Lunak        <l.lunak@kde.org>

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

****************************************************************************/

#ifndef __KXMESSAGES_H
#define __KXMESSAGES_H

#include <qwidget.h>
#include <qcstring.h>
#include <qstring.h>
#include <qmap.h>
#include <X11/X.h>

// TODO docs
/**
 * Sending string messages to other applications using the X Client Messages.
 *
 * Used internally by KStartupInfo. You usually don't want to use this, use DCOP
 * instead.
 *
 * @author Lubos Lunak <l.lunak@kde.org>
 * @version $Id$
 */
class KXMessages
    : public QWidget
    {
    Q_OBJECT
    public:
	/**
	 * Creates an instance which will receive X messages.
	 *
	 * If accept_broadcast is non-NULL,  all broadcast messages with this
	 * message type will be received.
	 */
	// CHECKME accept_broadcast == NULL is useless now
        KXMessages( const char* accept_broadcast = NULL, QWidget* parent = NULL );
        virtual ~KXMessages();
        void sendMessage( WId w, const char* msg_type, const QString& message );
	/**
	 * Broadcasts the given message with the given message type.
	 */
        void broadcastMessage( const char* msg_type, const QString& message );
        static bool sendMessageX( Display* disp, WId w, const char* msg_type,
            const QString& message );
	/**
	 * Broadcasts the given message with the given message type.
	 *
	 * @param disp X11 connection which will be used instead of qt_x11display()
	 */
        static bool broadcastMessageX( Display* disp, const char* msg_type,
            const QString& message );
    signals:
	/**
	 * Emitted when a message was received.
	 */
        void gotMessage( const QString& message );
    protected:
	/**
	 * @internal
	 */
        virtual bool x11Event( XEvent* ev );
	/**
	 * @internal
	 */
        static void send_message_internal( WId w_P, const QString& msg_P, long mask_P,
            Display* disp, Atom atom_P, Window handle_P );
	/**
	 * @internal
	 */
        QWidget* handle;
	/**
	 * @internal
	 */
        Atom cached_atom;
	/**
	 * @internal
	 */
        QCString cached_atom_name;
	/**
	 * @internal
	 */
        Atom accept_atom;
	/**
	 * @internal
	 */
        QMap< WId, QCString > incoming_messages;
    private:
        class Private;
        Private* d;
    };

#endif
