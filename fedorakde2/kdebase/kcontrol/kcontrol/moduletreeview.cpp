/*
  Copyright (c) 2000 Matthias Elter <elter@kde.org>
  Copyright (c) 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>

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

#include <qheader.h>
#include <qstring.h>
#include <qlist.h>
#include <qpoint.h>
#include <qcursor.h>


#include <klocale.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kiconloader.h>
#include <kservicegroup.h>
#include <kdebug.h>
#include <qwhatsthis.h>

#include "moduletreeview.h"
#include "moduletreeview.moc"
#include "modules.h"
#include "global.h"


class ModuleTreeWhatsThis : public QWhatsThis
{
public:
    ModuleTreeWhatsThis( ModuleTreeView* tree)
        : QWhatsThis( tree ), treeView( tree ) {}
    ~ModuleTreeWhatsThis(){};


    QString text( const QPoint & p) {
        ModuleTreeItem* i = (ModuleTreeItem*)  treeView->itemAt( p );
        if ( i && i->module() )  {
            return i->module()->comment();
        } else if ( i ) {
            return i18n("The %1 configuration group. Click to open it.").arg( i->text(0) );
        }
        return i18n("This treeview displays all available control modules. Click on one of the modules to receive more detailed information.");
    }

private:
    ModuleTreeView* treeView;
};

ModuleTreeView::ModuleTreeView(ConfigModuleList *list, QWidget * parent, const char * name)
  : KListView(parent, name)
  , _modules(list)
{
  setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  addColumn("");
  setAllColumnsShowFocus(true);
  setRootIsDecorated(true);
  header()->hide();

  new ModuleTreeWhatsThis( this );

  connect(this, SIGNAL(executed(QListViewItem*)),
                  this, SLOT(slotItemSelected(QListViewItem*)));
}

void ModuleTreeView::fill()
{
  clear();

  ConfigModule *module;
  for (module=_modules->first(); module != 0; module=_modules->next())
    {
      ModuleTreeItem *parent = 0;
      parent = getGroupItem(parent, module->groups());
      if (parent)
        new ModuleTreeItem(parent, module);
      else
        new ModuleTreeItem(this, module);
    }

  setMinimumWidth(columnWidth(0));
}

void ModuleTreeView::makeSelected(ConfigModule *module)
{
  ModuleTreeItem *item = static_cast<ModuleTreeItem*>(firstChild());

  updateItem(item, module);
}

void ModuleTreeView::updateItem(ModuleTreeItem *item, ConfigModule *module)
{
  while (item)
    {
          if (item->childCount() != 0)
                updateItem(static_cast<ModuleTreeItem*>(item->firstChild()), module);
          if (item->module() == module)
                {
                  setSelected(item, true);
                  break;
                }
          item = static_cast<ModuleTreeItem*>(item->nextSibling());
    }
}

void ModuleTreeView::expandItem(QListViewItem *item, QList<QListViewItem> *parentList)
{
  while (item)
    {
      setOpen(item, parentList->contains(item));

          if (item->childCount() != 0)
                expandItem(item->firstChild(), parentList);
      item = item->nextSibling();
    }
}

void ModuleTreeView::makeVisible(ConfigModule *module)
{
  ModuleTreeItem *item;

  item = static_cast<ModuleTreeItem*>(firstChild());

  // collapse all
  //QList<QListViewItem> parents;
  //expandItem(firstChild(), &parents);

  QStringList::ConstIterator it;
  item =static_cast<ModuleTreeItem*>( firstChild());
  for (it=module->groups().begin(); it != module->groups().end(); it++)
    {
      while (item)
                {
                  if (item->tag() == *it)
                        {
                          setOpen(item, true);
                          break;
                        }

                  item = static_cast<ModuleTreeItem*>(item->nextSibling());
                }
    }

  // make the item visible
  if (item)
    ensureItemVisible(item);
}


static QString menuPath(const QStringList& groups)
{
  QString path;

  QStringList::ConstIterator it;
  for (it=groups.begin(); it != groups.end(); ++it)
    path += *it + "/";

  return path;
}

ModuleTreeItem *ModuleTreeView::getGroupItem(ModuleTreeItem *parent, const QStringList& groups)
{
  // break recursion if path is empty
  if (groups.count() == 0)
    return parent;

  // calculate path
  QString path = menuPath(groups);
  //kdDebug() << "Path " << path << endl;

  // look if menu already exists
  if (_menuDict[path])
    return _menuDict[path];

  // find parent menu
  QStringList parGroup;
  for (unsigned int i=0; i<groups.count()-1; i++)
    parGroup.append(groups[i]);
  ModuleTreeItem *mparent = getGroupItem(parent, parGroup);

  // create new menu
  ModuleTreeItem *menu;
  if (mparent)
    menu = new ModuleTreeItem(mparent);
  else
    menu = new ModuleTreeItem(this);

  KServiceGroup::Ptr group = KServiceGroup::group(KCGlobal::baseGroup()+path);
  QString defName = path.left(path.length()-1);
  int pos = defName.findRev('/');
  if (pos >= 0)
    defName = defName.mid(pos+1);
  if (group && group->isValid())
  {
     menu->setPixmap(0, SmallIcon(group->icon()));
     menu->setText(0, " " + group->caption());
     menu->setTag(defName);
     menu->setCaption(group->caption());
  }
  else
  {
     // Should not happen: Installation problem
     // Let's try to fail softly.
     menu->setText(0, " " + defName);
     menu->setTag(defName);
  }

  _menuDict.insert(path, menu);

  return menu;
}


void ModuleTreeView::slotItemSelected(QListViewItem* item)
{
  if (!item) return;

  if (static_cast<ModuleTreeItem*>(item)->module())
    {
      emit moduleSelected(static_cast<ModuleTreeItem*>(item)->module());
      return;
    }
  else
    {
      emit categorySelected(item);
    }

  setOpen(item, !item->isOpen());

  /*
  else
    {
      QList<QListViewItem> parents;

      QListViewItem* i = item;
      while(i)
        {
          parents.append(i);
          i = i->parent();
        }

      //int oy1 = item->itemPos();
      //int oy2 = mapFromGlobal(QCursor::pos()).y();
      //int offset = oy2 - oy1;

      expandItem(firstChild(), &parents);

      //int x =mapFromGlobal(QCursor::pos()).x();
      //int y = item->itemPos() + offset;
      //QCursor::setPos(mapToGlobal(QPoint(x, y)));
    }
  */
}

void ModuleTreeView::keyPressEvent(QKeyEvent *e)
{
  if (!currentItem()) return;

  if(e->key() == Key_Return
     || e->key() == Key_Enter
        || e->key() == Key_Space)
    {
      //QCursor::setPos(mapToGlobal(QPoint(10, currentItem()->itemPos()+5)));
      slotItemSelected(currentItem());
    }
  else
    KListView::keyPressEvent(e);
}

ModuleTreeItem::ModuleTreeItem(QListViewItem *parent, ConfigModule *module)
  : QListViewItem(parent)
  , _module(module)
  , _tag(QString::null)
{
  if (_module)
        {
          setText(0, " " + module->name());
          setPixmap(0, SmallIcon(module->icon()));
        }
}

ModuleTreeItem::ModuleTreeItem(QListView *parent, ConfigModule *module)
  : QListViewItem(parent)
  , _module(module)
  , _tag(QString::null)
{
  if (_module)
        {
          setText(0, " " + module->name());
          setPixmap(0, SmallIcon(module->icon()));
        }
}

ModuleTreeItem::ModuleTreeItem(QListViewItem *parent, const QString& text)
  : QListViewItem(parent, " " + text)
  , _module(0)
  , _tag(QString::null) {}

ModuleTreeItem::ModuleTreeItem(QListView *parent, const QString& text)
  : QListViewItem(parent, " " + text)
  , _module(0)
  , _tag(QString::null) {}
