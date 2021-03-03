/*
  Copyright (c) 2000 Matthias Elter <elter@kde.org>

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

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <kservicegroup.h>
#include <ksycoca.h>
#include <kdebug.h>
#include <kapp.h>

#include <qobjectlist.h>
#include <qaccel.h>
#include <qwidget.h>

#include "config.h"
#include "utils.h"
#include "global.h"

bool KCGlobal::_root = false;
QStringList KCGlobal::_types;
QString KCGlobal::_uname = "";
QString KCGlobal::_hname = "";
QString KCGlobal::_kdeversion = "";
QString KCGlobal::_isystem = "";
QString KCGlobal::_irelease = "";
QString KCGlobal::_iversion = "";
QString KCGlobal::_imachine = "";
IndexViewMode KCGlobal::_viewmode = Icon;
IndexIconSize KCGlobal::_iconsize = Medium;
QString KCGlobal::_baseGroup = "";

void KCGlobal::init()
{
  QString hostname, username;
  char buf[128];
  char *user = getlogin();
  struct utsname info;

  gethostname(buf, 128);
  if (strlen(buf)) hostname = QString("%1").arg(buf); else hostname = "";
  if (!user) user = getenv("LOGNAME");
  if (!user) username = ""; else username = QString("%1").arg(user);

  setHostName(hostname);
  setUserName(username);
  setRoot(getuid() == 0);

  setKDEVersion(KDE_VERSION_STRING);

  uname(&info);

  setSystemName(info.sysname);
  setSystemRelease(info.release);
  setSystemVersion(info.version);
  setSystemMachine(info.machine);
}

void KCGlobal::setType(const QCString& s)
{
  QString string = s.lower();
  splitString(string, ',', _types);
}

QString KCGlobal::baseGroup()
{
  if ( _baseGroup.isEmpty() )
  {
    KServiceGroup::Ptr group = KServiceGroup::baseGroup( "settings" );
    if (group)
    {
      _baseGroup = group->relPath();
      kdDebug() << "Found basegroup = " << _baseGroup << endl;
      return _baseGroup;
    }
    // Compatibility with old behaviour, in case of missing .directory files.
    if (_baseGroup.isEmpty())
    {
      kdWarning() << "No K menu group with X-KDE-BaseGroup=settings found ! Defaulting to Settings/" << endl;
      _baseGroup = QString::fromLatin1("Settings");
    }
  }
  return _baseGroup;
}

void KCGlobal::repairAccels( QWidget * tw )
{
    QObjectList * l = tw->queryList( "QAccel" );
    QObjectListIt it( *l );             // iterate over the buttons
    QObject * obj;
    while ( (obj=it.current()) != 0 ) { // for each found object...
        ++it;
        ((QAccel*)obj)->repairEventFilter();
    }
    delete l;                           // delete the list, not the objects
}
