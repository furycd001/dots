/****************************************************************************

 KHotKeys -  (C) 1999 Lubos Lunak <l.lunak@email.cz>

 khkglobalaccel.cpp  - Slightly modified KGlobalAccel from kdelibs
 
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

 $Id: khkglobalaccel.cpp,v 1.4 2001/06/21 20:28:42 lunakl Exp $

****************************************************************************/

#include <qwidget.h>
#include <kdebug.h>
#include <X11/Xlib.h>

#include "khkglobalaccel.h"


KHKGlobalAccel::KHKGlobalAccel()
    : KGlobalAccel( true ) // HACK this avoids creating a QWidget
                           // in KGlobalAccel, because
                           // KGlobalAccel::x11EventFilter() is not virtual
    {
    do_not_grab = false; // HACK but we want grabbing
    }

// from 20010620 snapshot
#ifdef KeyPress
const int XKeyPress = KeyPress;
#undef KeyPress
#endif

static uint g_keyModMaskXAccel = 0;
static uint g_keyModMaskXAlwaysOff = 0;
static uint g_keyModMaskXOnOrOff = 0;

static void calculateGrabMasks()
{
	KAccel::readModifierMapping();
	g_keyModMaskXAccel = KAccel::accelModMaskX();
	g_keyModMaskXAlwaysOff = ~(
			KAccel::keyModXShift() |
			KAccel::keyModXLock() |
			KAccel::keyModXCtrl() |
			KAccel::keyModXAlt() |
			KAccel::keyModXNumLock() |
			KAccel::keyModXModeSwitch() |
			KAccel::keyModXMeta() |
			KAccel::keyModXScrollLock() );
	g_keyModMaskXOnOrOff =
			KAccel::keyModXLock() |
			KAccel::keyModXNumLock() |
			KAccel::keyModXScrollLock();

	// X11 seems to treat the ModeSwitch bit differently than the others --
	//  namely, it won't grab anything if it's set, but both switched and
	//  unswiched keys if it's not.
	//  So we always need to XGrabKey with the bit set to 0.
	g_keyModMaskXAlwaysOff |= KAccel::keyModXModeSwitch();
}


bool KHKGlobalAccel::x11EventFilter( const XEvent *event_ ) {
    uint keyModX, keyModX2;
    uint keySymX, keySymX2;

    if ( event_->type == MappingNotify ) {
	kdDebug(125) << "Caught MappingNotify" << endl;
	// Do XUngrabKey()s.
	setEnabled( false );
	// Maybe the X modifier map has been changed.
	calculateGrabMasks();
	// Do new XGrabKey()s.
	setEnabled( true );
	return true;
    }

    if ( aKeyMap.isEmpty() ) return false;
    if ( event_->type != XKeyPress ) return false;
#if 0
    if ( !KGlobalAccelPrivate::g_bKeyEventsEnabled ) return false;
#else
    if ( !areKeyEventsEnabled() ) return false;
#endif

    KAccel::keyEventXToKeyX( event_, 0, &keySymX, &keyModX );
    keyModX &= g_keyModMaskXAccel;

    kdDebug(125) << "x11EventFilter: seek " << KAccel::keySymXToString( keySymX, keyModX, false )
    	<< QString( " keyCodeX: %1 state: %2 keySym: %3 keyMod: %4\n" )
    		.arg( event_->xkey.keycode, 0, 16 ).arg( event_->xkey.state, 0, 16 ).arg( keySymX, 0, 16 ).arg( keyModX, 0, 16 );

    // Search for which accelerator activated this event:
    KKeyEntry entry;
    QString sConfigKey;
    for (KKeyEntryMap::ConstIterator it = aKeyMap.begin(); it != aKeyMap.end(); ++it) {
	KAccel::keyQtToKeyX( (*it).aCurrentKeyCode, 0, &keySymX2, &keyModX2 );
	//kdDebug() << "x11EventFilter: inspecting " << KAccel::keyToString( (*it).aCurrentKeyCode )
	//	<< QString( " keySym: %1 keyMod: %2\n" ).arg( keySymX2, 0, 16 ).arg( keyModX2, 0, 16 );
	if ( keySymX == keySymX2 && keyModX == (keyModX2 & g_keyModMaskXAccel) ) {
	    entry = *it;
	    sConfigKey = it.key();
	    break;
	}
    }

    if ( !QWidget::keyboardGrabber() ) {
	kdDebug(125) << "received action " << sConfigKey << endl;
#if 0
	if ( !d->rawModeList || !d->rawModeList->contains( sConfigKey ) ) {
#endif
	    XUngrabKeyboard(qt_xdisplay(), event_->xkey.time );
#if 0
	} else {
	    kdDebug(125) << "in raw mode !" << endl;
	}
#endif
	if ( !entry.receiver || !entry.bEnabled ) {
		kdDebug(125) << "KGlobalAccel::x11EventFilter(): Key has been grabbed (" << KAccel::keySymXToString( keySymX, keyModX, false ) << ") which doesn't have an associated action or was disabled.\n";
		return false;
	} else {
#if 0
                // what the hell is this supposed to do ?
		QRegExp r1( "([ ]*int[ ]*)" ), r2( " [0-9]+$" );
		if( r1.match( entry.member ) >= 0 && r2.match( sConfigKey ) >= 0 ) {
			int n = sConfigKey.mid( sConfigKey.findRev(' ')+1 ).toInt();
			kdDebug(125) << "Calling " << entry.member << " int = " << n << endl;
			connect( this, SIGNAL( activated( int ) ),
				entry.receiver, entry.member);
			emit activated( n );
			disconnect( this, SIGNAL( activated( int ) ), entry.receiver,
				entry.member );
		} else {
			connect( this, SIGNAL( activated() ),
				entry.receiver, entry.member);
			emit activated();
			disconnect( this, SIGNAL( activated() ), entry.receiver,
				entry.member );
#else
// this is actually the only important change
	    connect( this, SIGNAL( activated( const QString&, const QString&,
	        int) ), entry.receiver, entry.member);
	    emit activated( sConfigKey, entry.descr, entry.aCurrentKeyCode );
	    disconnect( this, SIGNAL( activated( const QString&,
                const QString&, int ) ), entry.receiver, entry.member );
#endif
#if 0
		}
#endif
	}
    }

    return true;
}

#include "khkglobalaccel.moc"
