/***************************************************************************
                          katepluginmanager.cpp  -  description
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

#include "katepluginmanager.h"
#include "katepluginmanager.moc"

#include "../app/kateapp.h"
#include "../mainwindow/katemainwindow.h"

#include <kparts/factory.h>

#include <kconfig.h>
#include <ksimpleconfig.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <qstringlist.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <ksimpleconfig.h>

KatePluginManager::KatePluginManager(QObject *parent) : QObject(parent)
{
  setupPluginList ();
  loadConfig ();
}

KatePluginManager::~KatePluginManager()
{
}

void KatePluginManager::setupPluginList ()
{
  KStandardDirs *dirs = KGlobal::dirs();

  QStringList list=dirs->findAllResources("appdata","plugins/*.desktop",false,true);

  KSimpleConfig *confFile;
  for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
  {
    confFile=new KSimpleConfig(*it,true);

    PluginListItem *info=new PluginListItem;

    info->load = false;
    info->libname = confFile->readEntry("libname","");
    info->name = confFile->readEntry("name","");
    info->description = confFile->readEntry("description","");
    info->author = confFile->readEntry("author","");
    info->plugin = 0L;

    myPluginList.append(info);

    delete confFile;
  }
}

void KatePluginManager::loadConfig ()
{
  KSimpleConfig *config = new KSimpleConfig("katepluginrc", false);
  config->setGroup("Plugins");

  for (uint i=0; i<myPluginList.count(); i++)
  {
    if  (config->readBoolEntry(myPluginList.at(i)->libname, false))
      myPluginList.at(i)->load = true;
  }
}

void KatePluginManager::writeConfig ()
{
  KSimpleConfig *config = new KSimpleConfig("katepluginrc", false);
  config->setGroup("Plugins");

  for (uint i=0; i<myPluginList.count(); i++)
  {
    config->writeEntry(myPluginList.at(i)->libname, myPluginList.at(i)->load);
  }

  config->sync();
}


void KatePluginManager::loadAllEnabledPlugins ()
{
  for (uint i=0; i<myPluginList.count(); i++)
  {
    if  (myPluginList.at(i)->load)
      loadPlugin (myPluginList.at(i));
  }
}

void KatePluginManager::enableAllPluginsGUI (KateMainWindow *win)
{
  for (uint i=0; i<myPluginList.count(); i++)
  {
    if  (myPluginList.at(i)->load)
      enablePluginGUI (myPluginList.at(i), win);
  }
}

void KatePluginManager::loadPlugin (PluginListItem *item)
{
  KLibFactory *factory = KLibLoader::self()->factory( item->libname.latin1() );
  item->plugin = (Kate::Plugin *)factory->create( (Kate::Application *)parent(), "", "Kate::Plugin" );
  item->load = true;
}

void KatePluginManager::unloadPlugin (PluginListItem *item)
{
  disablePluginGUI (item);

  delete item->plugin;
  item->plugin = 0L;
  item->load = false;
}

void KatePluginManager::enablePluginGUI (PluginListItem *item, KateMainWindow *win)
{
  if (!item->plugin->hasView()) return;

  win->guiFactory()->addClient( item->plugin->createView(win) );
}

void KatePluginManager::enablePluginGUI (PluginListItem *item)
{
  if (!item->plugin->hasView()) return;

  for (uint i=0; i< ((KateApp*)parent())->mainWindows.count(); i++)
  {
    ((KateApp*)parent())->mainWindows.at(i)->guiFactory()->addClient( item->plugin->createView(((KateApp*)parent())->mainWindows.at(i)) );
  }
}

void KatePluginManager::disablePluginGUI (PluginListItem *item)
{
  for (uint i=0; i< ((KateApp*)parent())->mainWindows.count(); i++)
  {
    for (uint z=0; z< item->plugin->viewList.count(); z++)
    {
      ((KateApp*)parent())->mainWindows.at(i)->guiFactory()->removeClient( item->plugin->viewList.at (z) );
    }
  }

  item->plugin->viewList.setAutoDelete (true);
  item->plugin->viewList.clear ();
}
