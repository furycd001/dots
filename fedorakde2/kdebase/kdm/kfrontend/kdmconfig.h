    /*

    Configuration for kdm. Class KDMConfig
    $Id: kdmconfig.h,v 1.13 2001/07/14 16:29:53 ossi Exp $

    Copyright (C) 1997, 1998, 2000 Steffen Hansen <hansen@kde.org>
    Copyright (C) 2000, 2001 Oswald Buddenhagen <ossi@kde.org>


    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    */
 

#ifndef KDMCONFIG_H
#define KDMCONFIG_H

#include <sys/param.h>	// for BSD
#include <unistd.h>

#include <qstring.h>
#include <qstrlist.h>
#include <qfont.h>

#include <ksimpleconfig.h>

#include <qnamespace.h>

QString GetCfgQStr (int id);
QStringList GetCfgQStrList (int id);

class KDMConfig {

private:
    QFont Str2Font (QString aValue);
//    QColor Str2Color (QString aValue);

public:
    KDMConfig();
//    ~KDMConfig();

    QFont	_normalFont;
    QFont	_failFont;
    QFont	_greetFont;

    int		_logoArea;
    QString	_logo;
    QString	_greetString;
    int		_greeterPosX, _greeterPosY;

    int		_showUsers;
    int		_preselUser;
    QString	_defaultUser;
    bool	_focusPasswd;
    bool	_sortUsers;
    QStringList	_users;
    QStringList	_noUsers;
    int		_lowUserId, _highUserId;
    int		_echoMode;
     
    QStringList	_sessionTypes;

    int		_allowShutdown;

#ifdef __linux__
    bool	_useLilo;
    QString	_liloCmd;
    QString	_liloMap;
#endif
};

extern KDMConfig *kdmcfg;

#endif /* KDMCONFIG_H */
