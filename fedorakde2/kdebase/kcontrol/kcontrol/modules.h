/*
  Copyright (c) 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>

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

#ifndef __modules_h__
#define __modules_h__

#include <ltdl.h>

#include <qobject.h>
#include <qstringlist.h>
#include <qlist.h>
#include <qpixmap.h>

#include "moduleinfo.h"


class QWidget;
class KAboutData;
class KCModule;
class ProxyWidget;
class KProcess;
class QXEmbed;
class QVBoxLayout;


class ConfigModule : public ModuleInfo
{
  Q_OBJECT

public:

  ConfigModule(QString desktopFile);
  ~ConfigModule();

  bool isChanged() { return _changed; };
  void setChanged(bool changed) { _changed = changed; };

  bool isActive() { return _module != 0; };
  ProxyWidget *module();
  const KAboutData *aboutData() const;


public slots:

  void deleteClient();


private slots:

  void clientClosed();
  void clientChanged(bool state);
  void runAsRoot();
  void rootExited(KProcess *proc);


signals:

  void changed(ConfigModule *module);
  void childClosed();
  void helpRequest();


private:

  bool         _changed;
  ProxyWidget *_module;
  QXEmbed     *_embedWidget;
  KProcess    *_rootProcess;
  QVBoxLayout *_embedLayout;

};

class ConfigModuleList : public QList<ConfigModule>
{
public:

  ConfigModuleList();

  void readDesktopEntries();
  void readDesktopEntriesRecursive(const QString &path);

};

#endif
