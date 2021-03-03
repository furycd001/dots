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
 
#ifndef __modulemenu_h__
#define __modulemenu_h__


#include <qlist.h>
#include <qstringlist.h>
#include <qintdict.h>
#include <qstring.h>
#include <qwidget.h>
#include <qdict.h>

#include <kpopupmenu.h>


class ConfigModule;
class ConfigModuleList;


class ModuleMenu : public KPopupMenu
{
  Q_OBJECT

public:

  ModuleMenu(ConfigModuleList *list, QWidget * parent = 0, const char * name = 0);


signals:

  void moduleActivated(ConfigModule*);


private slots:

  void moduleSelected(int id);


protected:

  KPopupMenu *getGroupMenu(const QStringList &groups);


private:
  
  int id;

  ConfigModuleList       *_modules;
  QIntDict<ConfigModule> _moduleDict;
  QDict<KPopupMenu>      _menuDict;

};


#endif
