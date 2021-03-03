/*
  Copyright (c) 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
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

#include <qimage.h>

#include <kiconloader.h>
#include <kdesktopfile.h>
#include <kservice.h>
#include <kdebug.h>
#include <assert.h>

#include "moduleinfo.h"
#include "moduleinfo.moc"
#include "utils.h"
#include "global.h"

ModuleInfo::ModuleInfo(QString desktopFile)
  : _fileName(desktopFile)
{
  _allLoaded = false;

  //kdDebug() << "desktopFile = " << desktopFile << endl;
  _service = KService::serviceByDesktopPath(desktopFile);
  assert(_service != 0L);

  // set the modules simple attributes
  setName(_service->name());
  setComment(_service->comment());
  setIcon(_service->icon());

  // library and factory
  setLibrary(_service->library());

  // get the keyword list
  setKeywords(_service->keywords());

  // try to find out the modules groups
  QString group = desktopFile;
  int pos = group.find(KCGlobal::baseGroup());
  if (pos >= 0)
     group = group.mid(pos+KCGlobal::baseGroup().length());
  pos = group.findRev('/');
  if (pos >= 0)
     group = group.left(pos);
  else
     group = QString::null;

  QStringList groups = QStringList::split('/', group);
  setGroups(groups);
}

ModuleInfo::~ModuleInfo()
{
  _name = QString::null;
  _comment = QString::null;
}

void
ModuleInfo::loadAll() const
{
  ModuleInfo *non_const_this = const_cast<ModuleInfo *>(this);
  non_const_this->_allLoaded = true;

  KDesktopFile desktop(_fileName);

  // library and factory
  non_const_this->setHandle(desktop.readEntry("X-KDE-FactoryName"));

  // does the module need super user privileges?
  non_const_this->setNeedsRootPrivileges(desktop.readBoolEntry("X-KDE-RootOnly", false));

  // does the module implement a read-only mode?
  non_const_this->setHasReadOnlyMode(desktop.readBoolEntry("X-KDE-HasReadOnlyMode", false));

  // get the documentation path
  non_const_this->setDocPath(desktop.readEntry("DocPath"));
}

QCString ModuleInfo::moduleId() const
{
  if (!_allLoaded) loadAll();

  QString res;

  QStringList::ConstIterator it;
  for (it = _groups.begin(); it != _groups.end(); ++it)
    res.append(*it+"-");
  res.append(name());

  return res.ascii();
}

const QString
ModuleInfo::fileName() const
{
   return _fileName;
};

const QStringList &
ModuleInfo::groups() const
{
   return _groups;
};


KService::Ptr ModuleInfo::service() const
{
  return _service;
}


const QStringList &
ModuleInfo::keywords() const
{
   return _keywords;
};

QString
ModuleInfo::name() const
{
   return _name;
};

QString
ModuleInfo::comment() const
{
   return _comment;
};

QString
ModuleInfo::icon() const
{
   return _icon;
};

QString
ModuleInfo::docPath() const
{
  if (!_allLoaded) loadAll();

  return _doc;
};

QString
ModuleInfo::library() const
{
  return _lib;
};

QString
ModuleInfo::handle() const
{
  if (!_allLoaded) loadAll();

  if (_handle.isEmpty())
     return _lib;

  return _handle;
};

bool
ModuleInfo::isDirectory() const
{
   return _directory;
};

bool
ModuleInfo::needsRootPrivileges() const
{
  if (!_allLoaded) loadAll();

  return _needsRootPrivileges;
};

bool
ModuleInfo::hasReadOnlyMode() const
{
  if (!_allLoaded) loadAll();

  return _hasReadOnlyMode;
};

