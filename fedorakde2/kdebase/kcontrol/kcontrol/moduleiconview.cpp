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


#include <qevent.h>

#include <kapp.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kdebug.h>

#include <kservicegroup.h>
#include <kiconloader.h>

#include "moduleiconview.h"
#include "moduleiconview.moc"
#include "modules.h"
#include "global.h"


ModuleIconView::ModuleIconView(ConfigModuleList *list, QWidget * parent, const char * name)
  : KIconView(parent, name)
  , _path(QString::null)
  , _modules(list)
{
  setArrangement(LeftToRight);
  setSelectionMode(Single);
  setItemsMovable(false);
  setSorting(false);
  setWordWrapIconText(true);
  setItemTextPos(Right);
  setResizeMode(Adjust);

  connect(this, SIGNAL(executed(QIconViewItem*)),
                  this, SLOT(slotItemSelected(QIconViewItem*)));
}

void ModuleIconView::makeSelected(ConfigModule *m)
{
  if (!m) return;

  for (QIconViewItem *i = firstItem(); i; i = i->nextItem())
        {
      if(static_cast<ModuleIconItem*>(i)->module() == m)
                {
                  setSelected(i, true);
                  break;
                }
        }

}

void ModuleIconView::makeVisible(ConfigModule *m)
{
  if (!m) return;
  _path = m->groups().join("/");
  fill();
}

void ModuleIconView::fill()
{
  clear();

  QStringList subdirs;

  // build a list of subdirs
  ConfigModule *module;
  for (module=_modules->first(); module != 0; module=_modules->next())
  {
    QString path = module->groups().join("/");
    // get the top level subdirs
    if (_path == QString::null)
    {
      QString top_level(module->groups()[0]);
      if (!subdirs.contains(top_level))
        subdirs.append(top_level);
    }
    else
    {
      if ((path != _path) && (path.left(_path.length()) == _path) &&
           !subdirs.contains(path))
        subdirs.append(path);
    }
  }

  // some defines for code readibility
  #define LoadSmall(x) KGlobal::iconLoader()->loadIcon(x, KIcon::Desktop, KIcon::SizeSmall)
  #define LoadLarge(x) KGlobal::iconLoader()->loadIcon(x, KIcon::Desktop, KIcon::SizeLarge)
  #define LoadMedium(x) KGlobal::iconLoader()->loadIcon(x, KIcon::Desktop, KIcon::SizeMedium)

  QPixmap icon;
  // add our "up" icon if we aren't top level
  if (_path != QString::null)
  {
    if (KCGlobal::iconSize() == Small)
    {
      icon = LoadSmall("up");
      if(icon.isNull())
        icon = LoadSmall("folder");
    }
    else if (KCGlobal::iconSize() == Large)
    {
      icon = LoadLarge("up");
      if(icon.isNull())
        icon = LoadLarge("folder");
    }
    else
    {
      icon = LoadMedium("up");
      if(icon.isNull())
        icon = LoadMedium("folder");
    }

    // go-up node
    ModuleIconItem *i = new ModuleIconItem(this, i18n("Go up"), icon);
    int last_slash = _path.findRev('/');
    if (last_slash == -1)
      i->setTag(QString::null);
    else
      i->setTag(_path.left(last_slash));
  }

  for (QStringList::Iterator it = subdirs.begin(); it != subdirs.end(); ++it )
  {
    QString subdir = (*it);

    KServiceGroup::Ptr group = KServiceGroup::group(KCGlobal::baseGroup()+subdir+'/');
    if (!group || !group->isValid())
       continue;

    if (KCGlobal::iconSize() == Small)
    {
      icon = LoadSmall(group->icon());
      if(icon.isNull())
        icon = LoadSmall("folder");
    }
    else if (KCGlobal::iconSize() == Large)
    {
      icon = LoadLarge(group->icon());
      if(icon.isNull())
        icon = LoadLarge("folder");
    }
    else
    {
      icon = LoadMedium(group->icon());
      if(icon.isNull())
        icon = LoadMedium("folder");
    }

    ModuleIconItem *i = new ModuleIconItem(this, group->caption(), icon);
    i->setTag(subdir);
  }

  // we don't ever have any modules on the top level
  if (_path == QString::null)
    return;

  for (module=_modules->first(); module != 0; module=_modules->next())
  {
    if (module->library().isEmpty())
      continue;

    QString path = module->groups().join("/");
    if (path != _path)
      continue;

    if (KCGlobal::iconSize() == Small)
      icon = LoadSmall(module->icon());
    else if (KCGlobal::iconSize() == Large)
      icon = LoadLarge(module->icon());
    else
      icon = LoadMedium(module->icon());

    (void) new ModuleIconItem(this, module->name(), icon, module);
  }
}

void ModuleIconView::slotItemSelected(QIconViewItem* item)
{
  QApplication::restoreOverrideCursor();
  if (!item) return;

  if (static_cast<ModuleIconItem*>(item)->module())
        emit moduleSelected(static_cast<ModuleIconItem*>(item)->module());
  else
        {
          _path = static_cast<ModuleIconItem*>(item)->tag();
          fill();
      setCurrentItem(firstItem());
        }
}

QDragObject *ModuleIconView::dragObject()
{
  QDragObject *icondrag = QIconView::dragObject();
  QUriDrag *drag = new QUriDrag(this);

  QPixmap pm = icondrag->pixmap();
  drag->setPixmap(pm, QPoint(pm.width()/2,pm.height()/2));


  QPoint orig = viewportToContents(viewport()->mapFromGlobal(QCursor::pos()));

  QStringList l;
  ModuleIconItem *item = (ModuleIconItem*) findItem(orig);
  if (item)
    {
      if (item->module())
        l.append(item->module()->fileName());
      else
        if (!item->tag().isEmpty())
          {
            QString dir = _path + "/" + item->tag();
            dir = locate("apps", KCGlobal::baseGroup()+dir+"/.directory");
            int pos = dir.findRev("/.directory");
            if (pos > 0)
              {
                dir = dir.left(pos);
                l.append(dir);
              }
          }

      drag->setFilenames(l);
    }

  delete icondrag;

  if (l.count() == 0)
    return 0;

  return drag;
}

void ModuleIconView::keyPressEvent(QKeyEvent *e)
{
  if(   e->key() == Key_Return
     || e->key() == Key_Enter
     || e->key() == Key_Space)
    {
      if (currentItem())
        slotItemSelected(currentItem());
    }
  // Workaround for strange QIconView behaviour.
  else if(e->key() == Key_Up)
    {
      QKeyEvent ev(e->type(), Key_Left, e->ascii(), e->state(), e->text(),
                   e->isAutoRepeat(), e->count());
      KIconView::keyPressEvent(&ev);
    }
  // Workaround for strange QIconView behaviour.
  else if(e->key() == Key_Down)
    {
      QKeyEvent ev(e->type(), Key_Right, e->ascii(), e->state(), e->text(),
                   e->isAutoRepeat(), e->count());
      KIconView::keyPressEvent(&ev);
    }
  else
    KIconView::keyPressEvent(e);
}
