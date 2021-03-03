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

#ifndef __indexwidget_h__
#define __indexwidget_h__

#include <qwidgetstack.h>

#include "global.h"

class ConfigModuleList;
class ConfigModule;
class ModuleTreeView;
class ModuleIconView;

class IndexWidget : public QWidgetStack
{
  Q_OBJECT
  
public:   
  IndexWidget(ConfigModuleList *list, QWidget *parent, const char *name=0);
  virtual ~IndexWidget();

public slots:
  void makeVisible(ConfigModule *module);
  void makeSelected(ConfigModule *module);
  void activateView(IndexViewMode);
  void reload();

protected slots:
  void moduleSelected(ConfigModule *);

signals:
  void moduleActivated(ConfigModule *module);
  void categorySelected(QListViewItem *);

protected:
  void resizeEvent(QResizeEvent *e);

private:
  ModuleTreeView   *_tree;
  ModuleIconView   *_icon;
  ConfigModuleList *_modules;
  IndexViewMode    viewMode;
};

#endif
