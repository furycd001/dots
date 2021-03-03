/*
 * bell.cpp
 *
 * Copyright (c) 1997 Patrick Dowler dowler@morgul.fsh.uvic.ca
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qlayout.h>
#include <qtabbar.h>
#include <qtabwidget.h>

#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>

#include <kcmkdb.h>
#include <pluginconfig.h>
#include <connectionconfig.h>

#include <kdb/dbengine.h>

#include <kcmkdb.moc>

KDBModule::KDBModule( QWidget* parent, const char* name )
  : KCModule( parent, name )
{
  QHBoxLayout* layout = new QHBoxLayout( this );
  
  m_tab = new QTabWidget( this );
  layout->addWidget( m_tab );
 
  m_plugins = new PluginConfig( m_tab );
  m_tab->addTab( m_plugins, i18n( "Plugins" ) );
  connect( m_plugins, SIGNAL( changed() ), SLOT( slotChanged() ) );
 
  m_connections = new ConnectionConfig( m_tab );
  m_tab->addTab( m_connections, i18n( "Connections" ) );
  connect( m_connections, SIGNAL( changed() ), SLOT( slotChanged() ) );

  load();
}

KDBModule::~KDBModule()
{
}

void KDBModule::load()
{
  m_plugins->load();
  m_connections->load();
}

void KDBModule::save()
{
  m_plugins->save();
  m_connections->save();
  DBENGINE->config()->sync();
}

void KDBModule::defaults()
{
  m_plugins->defaults();
  m_connections->defaults();
}

void KDBModule::slotChanged()
{
  emit changed( true );
}

extern "C"
{
  KCModule* create_kdb( QWidget* parent, const char* name ) 
  {
    KGlobal::locale()->insertCatalogue( "kcmkdb" );
    return new KDBModule( parent, name );
  }
}


