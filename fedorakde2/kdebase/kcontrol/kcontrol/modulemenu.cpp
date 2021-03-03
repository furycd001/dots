this file is currently not used.
this message breaks compilation.
that is intentional :-]

/*
  Copyright (c) 2000 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>

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


#include <qheader.h>
#include <qstring.h>
#include <qlist.h>
#include <qpoint.h>
#include <qcursor.h>

#include <klocale.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kiconloader.h>
#include <kservicegroup.h>
#include <kdebug.h>

#include "modulemenu.h"
#include "modulemenu.moc"
#include "modules.h"
#include "global.h"


ModuleMenu::ModuleMenu(ConfigModuleList *list, QWidget * parent, const char * name)
  : KPopupMenu(parent, name)
  , _modules(list)
{
  // use large id's to start with...
  id = 10000;

  ConfigModule *module;
  for (module=_modules->first(); module != 0; module=_modules->next())
    {
      KPopupMenu *parent = getGroupMenu(module->groups());
  
      // Item names may contain ampersands. To avoid them being converted to 
      // accelators, replace them with two ampersands.
      QString name = module->name();
      name.replace(QRegExp("&"), "&&");
      
      int realid = parent->insertItem(KGlobal::iconLoader()->loadIcon(module->icon(), KIcon::Desktop, KIcon::SizeSmall)
                                          , name, id);
      _moduleDict.insert(realid, module);

      id++;
    }

  connect(this, SIGNAL(activated(int)), this, SLOT(moduleSelected(int)));
}


QString menuPath(const QStringList& groups)
{
  QString path;

  QStringList::ConstIterator it;
  for (it=groups.begin(); it != groups.end(); ++it)
    path += *it + "/";

  return path;
}


KPopupMenu *ModuleMenu::getGroupMenu(const QStringList &groups)
{
  // break recursion if path is empty
  if (groups.count() == 0)
    return this;

  // calculate path
  QString path = menuPath(groups);
  //kdDebug() << "Path " << path << endl;

  // look if menu already exists
  if (_menuDict[path])
    return _menuDict[path];

  // find parent menu
  QStringList parGroup;
  for (unsigned int i=0; i<groups.count()-1; i++)
    parGroup.append(groups[i]);
  KPopupMenu *parent = getGroupMenu(parGroup);

  KServiceGroup::Ptr group = KServiceGroup::group(KCGlobal::baseGroup()+path);
  if (!group)
  {
     kdWarning() << KCGlobal::baseGroup()+path << " not found!\n";
     return this;
  }
 
  // create new menu
  KPopupMenu *menu = new KPopupMenu(parent);
  connect(menu, SIGNAL(activated(int)), this, SLOT(moduleSelected(int)));

  // Item names may contain ampersands. To avoid them being converted to 
  // accelators, replace them with two ampersands.
  QString name = group->caption();
  name.replace(QRegExp("&"), "&&");
  
  parent->insertItem(KGlobal::iconLoader()->loadIcon(group->icon(), KIcon::Desktop, KIcon::SizeSmall)
                                         , name, menu);

  _menuDict.insert(path, menu);

  return menu;
}


void ModuleMenu::moduleSelected(int id)
{
  kdDebug() << "Item " << id << " selected" << endl;
  ConfigModule *module = _moduleDict[id];
  if (module)
    emit moduleActivated(module);
}
