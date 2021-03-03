/* This file is part of the KDE libraries

   Copyright (c) 2000 Dawit Alemayehu <adawit@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License (LGPL) as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qobject.h>
#include <qpopupmenu.h>

#include <kstdaccel.h>
#include <kcompletion.h>
#include <klocale.h>

KCompletionBase::KCompletionBase()
{
    // Assign the default completion type to use.
    m_iCompletionMode = KGlobalSettings::completionMode();

    // Initialize all key-bindings to 0 by default so that
    // the event filter will use the global settings.
    useGlobalKeyBindings();

    // By default we initialize everything to false.
    // All the variables would be setup properly when
    // the appropriate member functions are called.
    setup( false, false, false );
}

KCompletionBase::~KCompletionBase()
{
    if( m_bAutoDelCompObj && m_pCompObj )
    {
        delete m_pCompObj;
    }
}

KCompletion* KCompletionBase::completionObject( bool hsig )
{
    if ( !m_pCompObj )
    {
        setCompletionObject( new KCompletion(), hsig );
	m_bAutoDelCompObj = true;
    }
    return m_pCompObj;
}

KCompletion* KCompletionBase::completionObject( bool create, bool hsig )
{
    if ( !create )
	return m_pCompObj;

    return completionObject( hsig );
}

void KCompletionBase::setCompletionObject( KCompletion* compObj, bool hsig )
{
    if ( m_bAutoDelCompObj && compObj != m_pCompObj )
        delete m_pCompObj;

    m_pCompObj = compObj;

    // We emit rotation and completion signals
    // if completion object is not NULL.
    setup( false, hsig, !m_pCompObj.isNull() );
}

// BC: Inline this function and possibly rename it to setHandleEvents??? (DA)
void KCompletionBase::setHandleSignals( bool handle )
{
    m_bHandleSignals = handle;
}

void KCompletionBase::setCompletionMode( KGlobalSettings::Completion mode )
{
    m_iCompletionMode = mode;
    // Always sync up KCompletion mode with ours as long as we
    // are performing completions.
    if( m_pCompObj &&
        m_iCompletionMode != KGlobalSettings::CompletionNone )
    {
        m_pCompObj->setCompletionMode( m_iCompletionMode );
    }
}

bool KCompletionBase::setKeyBinding( KeyBindingType item, int key )
{
    if( key >= 0 )
    {
        if( key > 0 )
        {
            for( KeyBindingMap::Iterator it = m_keyMap.begin(); it != m_keyMap.end(); ++it )
                if( it.data() == key )  return false;
        }
        m_keyMap.replace( item, key );
        return true;
    }
    return false;
}

void KCompletionBase::useGlobalKeyBindings()
{
    m_keyMap.clear();
    m_keyMap.insert( TextCompletion, 0 );
    m_keyMap.insert( PrevCompletionMatch, 0 );
    m_keyMap.insert( NextCompletionMatch, 0 );
}

void KCompletionBase::setup( bool autodel, bool hsig, bool esig )
{
    m_bAutoDelCompObj = autodel;
    m_bHandleSignals = hsig;
    m_bEmitSignals = esig;
}
