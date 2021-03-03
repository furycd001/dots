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

#include <kapp.h>
#include <klocale.h>
#include <kprogress.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kdebug.h>
#include <kdesktopfile.h>
#include <kiconloader.h>


#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qstringlist.h>
#include <qtimer.h>


#include "scanner.h"
#include "scanner.moc"
#include "checker.h"


Scanner::Scanner(QWidget *parent, const char *name)
  : KDialog(parent,name,true)
{
  setCaption(i18n("KDE Application Finder"));

  QGridLayout *grid = new QGridLayout(this, 2,2, marginHint());

  QLabel *l = new QLabel(i18n("The application finder looks for legacy "
			      "applications on your system and adds "
			      "them to the KDE menu system. "
			      "Click 'Scan' to begin."), this);
  l->setAlignment(WordBreak);

  grid->addMultiCellWidget(l, 0,0, 0,1);
  grid->addRowSpacing(1, marginHint());

  l = new QLabel(i18n("Looking for:"), this);
  grid->addMultiCellWidget(l, 2,2, 0,1);

  _appIcon = new QLabel(this);
  _appIcon->setFixedSize(40,42);
  _appIcon->setFrameStyle(QFrame::Panel|QFrame::Sunken);
  grid->addWidget(_appIcon, 3,0);
  _appIcon->setPixmap(KGlobal::iconLoader()->loadIcon("kappfinder", KIcon::Desktop));

  _appName = new QLabel(i18n("Legacy applications"), this);
  grid->addWidget(_appName, 3,1);

  grid->addRowSpacing(4, marginHint());

  l = new QLabel(i18n("Progress:"), this);
  grid->addMultiCellWidget(l, 5,5, 0,1);

  _progress = new KProgress(KProgress::Horizontal, this);
  grid->addMultiCellWidget(_progress, 6,6, 0,1);
  _progress->hide();

  _summary = new QLabel(i18n("Summary:"), this);
  grid->addMultiCellWidget(_summary, 7,7, 0,1);

  grid->setRowStretch(20,1);

  _cancel1 = new QPushButton(i18n("&Cancel"), this);
  grid->addWidget(_cancel1, 21,0, AlignLeft);

  _scan = new QPushButton(i18n("&Scan"), this);
  grid->addWidget(_scan, 21,1, AlignRight);
  connect(_scan, SIGNAL(clicked()), this, SLOT(startScan()));
  _scan->setDefault(true);

  connect(kapp, SIGNAL(lastWindowClosed()), kapp, SLOT(quit()));
  connect(_cancel1, SIGNAL(clicked()), kapp, SLOT(quit()));

  resize(450,250);
}


Scanner::~Scanner()
{
}


void Scanner::startScan()
{
  _templates = KGlobal::dirs()->findAllResources("data", "kappfinder/apps/*.desktop", true);

  found = 0;
  count = _templates.count();

  _progress->setRange(0, count);
  _progress->setValue(0);
  _progress->show();

  _scan->setEnabled(false);

  QTimer::singleShot(0, this, SLOT(scanOneFile()));
}


void Scanner::scanOneFile()
{
  KIconLoader *loader = KGlobal::iconLoader();

  if (_templates.count() > 0)
    {
      QStringList::Iterator first = _templates.begin();

      KDesktopFile desktop(*first, true);

      // eye candy
      _appIcon->setPixmap(loader->loadIcon(desktop.readIcon(), KIcon::Desktop));
      _appName->setText(desktop.readName());
      _progress->setValue(_progress->value()+1);

      // copy over the desktop file, if exists
      if (checkDesktopFile(*first))
	found++;

      // update summary
      QString sum(i18n("Summary: found %1 applications"));
      _summary->setText(sum.arg(found));

      // remove entry
      _templates.remove(first);

      // come back later
      QTimer::singleShot(0, this, SLOT(scanOneFile()));
      return;
    }

  // decorate directories
  decorateDirs();

  // stop scanning
  _scan->disconnect(this, SLOT(startScan()));
  _scan->setText(i18n("&Quit"));
  connect(_scan, SIGNAL(clicked()), kapp, SLOT(quit()));
  _scan->setEnabled(true);
}
