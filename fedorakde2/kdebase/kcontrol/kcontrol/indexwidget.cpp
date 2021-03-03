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

#include <qlayout.h>
#include <qlistview.h>

#include <kglobal.h>
#include <klocale.h>
#include <kconfig.h>

#include "indexwidget.h"
#include "indexwidget.moc"
#include "moduletreeview.h"
#include "moduleiconview.h"
#include "modules.h"

IndexWidget::IndexWidget(ConfigModuleList *modules, QWidget *parent ,const char *name)
  : QWidgetStack(parent, name)
  , _modules(modules)
  , viewMode(Icon)
{
  // treeview
  _tree = new ModuleTreeView(_modules, this);
  _tree->fill();
  connect(_tree, SIGNAL(moduleSelected(ConfigModule*)), 
		  this, SLOT(moduleSelected(ConfigModule*)));
  connect(_tree, SIGNAL(categorySelected(QListViewItem*)), 
		  this, SIGNAL(categorySelected(QListViewItem*)));

  // iconview
  _icon = new ModuleIconView(_modules, this);
  _icon->fill();
  connect(_icon, SIGNAL(moduleSelected(ConfigModule*)), 
		  this, SLOT(moduleSelected(ConfigModule*)));
  activateView(Icon);
}

IndexWidget::~IndexWidget() {}

void IndexWidget::reload()
{
  _icon->fill();
}

void IndexWidget::resizeEvent(QResizeEvent *e)
{
  QWidgetStack::resizeEvent( e );
  _icon->setGridX(width()-26);
  _icon->fill();
}

void IndexWidget::moduleSelected(ConfigModule *m)
{
  if(!m) return;

  emit moduleActivated(m);

  if (sender()->inherits("ModuleIconView"))
	{
	  _tree->makeVisible(m);

	  _tree->disconnect(SIGNAL(moduleSelected(ConfigModule*)));
	  _tree->makeSelected(m);
	  connect(_tree, SIGNAL(moduleSelected(ConfigModule*)), 
			  this, SLOT(moduleSelected(ConfigModule*)));
	}
  else if (sender()->inherits("ModuleTreeView"))
	{
	  _icon->makeVisible(m);

	  _icon->disconnect(SIGNAL(moduleSelected(ConfigModule*)));
	  _icon->makeSelected(m);
	  connect(_icon, SIGNAL(moduleSelected(ConfigModule*)), 
			 this, SLOT(moduleSelected(ConfigModule*)));
	}
}

void IndexWidget::makeSelected(ConfigModule *module)
{
  _icon->disconnect(SIGNAL(moduleSelected(ConfigModule*)));
  _tree->disconnect(SIGNAL(moduleSelected(ConfigModule*)));

  _icon->makeSelected(module);
  _tree->makeSelected(module);

  connect(_icon, SIGNAL(moduleSelected(ConfigModule*)), 
		  this, SLOT(moduleSelected(ConfigModule*)));

  connect(_tree, SIGNAL(moduleSelected(ConfigModule*)), 
		  this, SLOT(moduleSelected(ConfigModule*)));
}

void IndexWidget::makeVisible(ConfigModule *module)
{
  _icon->makeVisible(module);
  _tree->makeVisible(module);
}

void IndexWidget::activateView(IndexViewMode mode)
{
  viewMode = mode;

  if (mode == Icon)
        raiseWidget( _icon );
  else
        raiseWidget( _tree );
}
