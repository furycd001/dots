/* This file is part of the KDE project
   Copyright (C) 2001 Waldo Bastian <bastian@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qmap.h>
#include <qimage.h>

#include <kconfig.h>
#include <kiconloader.h>
#include <kapp.h>
#include <kglobal.h>

#include "kcustommenu.h"

class KCustomMenu::KCustomMenuPrivate
{
public:
   QMap<int,KService::Ptr> entryMap;
};

KCustomMenu::KCustomMenu(const QString &configfile, QWidget *parent)
   : QPopupMenu(parent, "kcustom_menu")
{
  d = new KCustomMenuPrivate; 
  
  KConfig cfg(configfile, true, false);
  int count = cfg.readNumEntry("NrOfItems");
  for(int i = 0; i < count; i++)
  {
     QString entry = cfg.readEntry(QString("Item%1").arg(i+1));
     if (entry.isEmpty())
        continue;

     // Try KSycoca first.
     KService::Ptr menuItem = KService::serviceByDesktopPath( entry );
     if (!menuItem)
        menuItem = KService::serviceByDesktopName( entry );
     if (!menuItem)
        menuItem = new KService( entry );

     if (!menuItem->isValid())
        continue;
 
     insertMenuItem( menuItem, -1 );
  }
  connect(this, SIGNAL(activated(int)), this, SLOT(slotActivated(int)));
}

KCustomMenu::~KCustomMenu()
{
  delete d;
}

void
KCustomMenu::slotActivated(int id)
{
  KService::Ptr s = d->entryMap[id];
  if (!s)
     return;
  kapp->startServiceByDesktopPath(s->desktopEntryPath());
}

// The following is copied from kicker's PanelServiceMenu
void 
KCustomMenu::insertMenuItem(KService::Ptr & s, int nId, int nIndex/*= -1*/)
{
    QString serviceName = s->name();

    // item names may contain ampersands. To avoid them being converted
    // to accelators, replace them with two ampersands.
    serviceName.replace(QRegExp("&"), "&&");

    QPixmap normal = KGlobal::instance()->iconLoader()->loadIcon(s->icon(), KIcon::Small,
                                                                 0, KIcon::DefaultState, 0L, true);
    QPixmap active = KGlobal::instance()->iconLoader()->loadIcon(s->icon(), KIcon::Small,
                                                                 0, KIcon::ActiveState, 0L, true);
    // make sure they are not larger than 16x16
    if (normal.width() > 16 || normal.height() > 16) {
        QImage tmp = normal.convertToImage();
        tmp = tmp.smoothScale(16, 16);
        normal.convertFromImage(tmp);
    }
    if (active.width() > 16 || active.height() > 16) {
        QImage tmp = active.convertToImage();
        tmp = tmp.smoothScale(16, 16);
        active.convertFromImage(tmp);
    }

    QIconSet iconset;
    iconset.setPixmap(normal, QIconSet::Small, QIconSet::Normal);
    iconset.setPixmap(active, QIconSet::Small, QIconSet::Active);

    int newId = insertItem(iconset, serviceName, nId, nIndex);
    d->entryMap.insert(newId, s);
}

#include "kcustommenu.moc"
