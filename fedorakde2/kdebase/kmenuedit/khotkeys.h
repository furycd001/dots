/*
 *   Copyright (C) 2000 Matthias Elter <elter@kde.org>
 *                      Lubos Lunak    <l.lunak@email.cz>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __khotkeys_public_h__
#define __khotkeys_public_h__

#include <qstring.h>

// see kdebase/khotkeys/kcontrol for info on these

class KHotKeys
{
public:
    static bool init();
    static bool present();
    static QString getMenuEntryShortcut( const QString& entry_P );
    static QString changeMenuEntryShortcut( const QString& entry_P,
                                            const QString shortcut_P );
    static QString editMenuEntryShortcut( const QString& entry_P,
                                          const QString shortcut_P, bool save_if_edited_P );
    static bool menuEntryMoved( const QString& new_P, const QString& old_P );
    static void menuEntryDeleted( const QString& entry_P );
};

#endif
