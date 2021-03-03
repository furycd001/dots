/***************************************************************************
                          katepluginmanager.h  -  description
                             -------------------
    begin                : FRE Feb 23 2001
    copyright            : (C) 2001 by Joseph Wenninger
    email                : jowenn@bigfoot.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef _kate_pluginmanager_h_
#define _kate_pluginmanager_h_

#include "../main/katemain.h"
#include "../interfaces/plugin.h"

#include <qobject.h>
#include <qvaluelist.h>
#include <qlist.h>

struct PluginListItem
{
  bool load;
  QString libname;
  QString name;
  QString description;
  QString author;
  Kate::Plugin *plugin;
};

typedef QList<PluginListItem> PluginList;

class KatePluginManager : public QObject
{
  Q_OBJECT

  friend class KateConfigPluginPage;
  friend class KateConfigDialog;
  friend class KateMainWindow;
  friend class KateApp;

  public:
    KatePluginManager(QObject *parent);
    ~KatePluginManager();

    void loadAllEnabledPlugins ();
    void enableAllPluginsGUI (KateMainWindow *win);

  private:
    void setupPluginList ();
    void loadConfig ();
    void writeConfig ();

    void loadPlugin (PluginListItem *item);
    void unloadPlugin (PluginListItem *item);
    void enablePluginGUI (PluginListItem *item, KateMainWindow *win);
    void enablePluginGUI (PluginListItem *item);
    void disablePluginGUI (PluginListItem *item);

    PluginList myPluginList;
};

#endif
