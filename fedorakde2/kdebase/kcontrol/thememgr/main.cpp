/*
 * main.cpp
 *
 * Copyright (c) 1998 Stefan Taferner <taferner@kde.org>
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

#include <kmessagebox.h>

#include "themecreator.h"
#include "installer.h"
#include "global.h"
#include "options.h"
#include "about.h"

#include <qlayout.h>
#include <qtabwidget.h>

#include <klocale.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kapp.h>

ThemeCreator* theme = NULL;

//-----------------------------------------------------------------------------
class KThemeMgr : public KCModule
{
public:

  KThemeMgr(QWidget *parent, const char *name);
  ~KThemeMgr();

  virtual void init();
  virtual void save();
  virtual void load();
  virtual void defaults();

private:
  Installer* mInstaller;
  Options* mOptions;
  About* mAbout;
};



//-----------------------------------------------------------------------------
KThemeMgr::KThemeMgr(QWidget *parent, const char *name)
  : KCModule(parent, name)
{
  init();

  mInstaller = NULL;
  theme = new ThemeCreator;

  QVBoxLayout *layout = new QVBoxLayout(this);
  QTabWidget *tab = new QTabWidget(this);
  layout->addWidget(tab);

  mInstaller = new Installer(this);
  tab->addTab(mInstaller, i18n("&Installer"));
  connect(mInstaller, SIGNAL(changed(bool)), this, SIGNAL(changed(bool)));

  mOptions = new Options(this);
  tab->addTab(mOptions, i18n("&Contents"));
  connect(mOptions, SIGNAL(changed(bool)), this, SIGNAL(changed(bool)));

  mAbout = new About(this);
  tab->addTab(mAbout, i18n("&About"));
}


//-----------------------------------------------------------------------------
KThemeMgr::~KThemeMgr()
{
  delete theme;
}

//-----------------------------------------------------------------------------
void KThemeMgr::init()
{
  //kdDebug() << "No init necessary" << endl;
    KGlobal::dirs()->addResourceType("themes", KStandardDirs::kde_default("data") + "kthememgr/Themes/");
}


//-----------------------------------------------------------------------------
void KThemeMgr::defaults()
{
  mInstaller->defaults();
}


//-----------------------------------------------------------------------------
void KThemeMgr::save()
{
  mAbout->save();
  mOptions->save();
  mInstaller->save();
  theme->install();
}

//-----------------------------------------------------------------------------
void KThemeMgr::load()
{
  mInstaller->load();
  mOptions->load();
  mAbout->load();
}

extern "C"
{
  KCModule *create_themes(QWidget *parent, const char *name)
  {
    KGlobal::locale()->insertCatalogue("kcmthemes");
    return new KThemeMgr(parent, name);
  }

}


