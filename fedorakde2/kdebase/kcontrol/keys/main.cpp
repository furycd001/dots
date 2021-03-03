/*
 * main.cpp
 *
 * Copyright (c) 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
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

#include <klocale.h>
#include <kglobal.h>
#include <kdialog.h>

#include "keyconfig.h"


#include "main.h"


KeyModule::KeyModule(QWidget *parent, const char *name)
  : KCModule(parent, name)
{
  QVBoxLayout *layout = new QVBoxLayout(this);
  tab = new QTabWidget(this);
  layout->addWidget(tab);
  connect(tab, SIGNAL(currentChanged(QWidget*)), SLOT(tabChanged(QWidget*)));

  global = new KKeyModule(this, true, false, true);
  tab->addTab(global, i18n("&Global Shortcuts"));
  connect(global, SIGNAL(changed(bool)), this, SLOT(moduleChanged(bool)));

  series = new KKeyModule(this, true, true, false);
  tab->addTab(series, i18n("Shortcut Se&quences"));
  connect(series, SIGNAL(changed(bool)), this, SLOT(moduleChanged(bool)));

  standard = new KKeyModule(this, false);
  tab->addTab(standard, i18n("&Application Shortcuts"));
  connect(standard, SIGNAL(changed(bool)), this, SLOT(moduleChanged(bool)));

  connect(standard, SIGNAL(keysChanged( const KKeyEntryMap* )),
      global, SLOT( updateKeys( const KKeyEntryMap* )));
  connect(global, SIGNAL(keysChanged( const KKeyEntryMap* )),
      standard, SLOT( updateKeys( const KKeyEntryMap* )));
}


void KeyModule::load()
{
  global->load();
  series->load();
  standard->load();
}


void KeyModule::save()
{
  global->save();
  series->save();
  standard->save();
}


void KeyModule::defaults()
{
  global->defaults();
  series->defaults();
  standard->defaults();
}


void KeyModule::moduleChanged(bool state)
{
  emit changed(state);
}

// Keep the global and sequence shortcut scheme lists in sync
void KeyModule::tabChanged(QWidget*)
{
  global->readSchemeNames();
  series->readSchemeNames();
}

void KeyModule::resizeEvent(QResizeEvent *)
{
  tab->setGeometry(0,0,width(),height());
}

QString KeyModule::quickHelp() const
{
  return i18n("<h1>Key Bindings</h1> Using key bindings you can configure certain actions to be"
    " triggered when you press a key or a combination of keys, e.g. CTRL-C is normally bound to"
    " 'Copy'. KDE allows you to store more than one 'scheme' of key bindings, so you might want"
    " to experiment a little setting up your own scheme while you can still change back to the"
    " KDE defaults.<p> In the tab 'Global shortcuts' you can configure non-application specific"
    " bindings like how to switch desktops or maximize a window. In the tab 'Application shortcuts'"
    " you'll find bindings typically used in applications, such as copy and paste.");
}


extern "C"
{
  KCModule *create_keys(QWidget *parent, const char *name)
  {
    KGlobal::locale()->insertCatalogue("kcmkeys");
    KGlobal::locale()->insertCatalogue("kwin");
    KGlobal::locale()->insertCatalogue("kdesktop");
    KGlobal::locale()->insertCatalogue("kicker");
    return new KeyModule(parent, name);
  }
  void init_keys()
  {
    KKeyModule::init();
  }
}


#include "main.moc"
