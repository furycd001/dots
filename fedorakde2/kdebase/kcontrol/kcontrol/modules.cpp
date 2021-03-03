/*
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

#include <unistd.h>
#include <sys/types.h>


#include <qwidget.h>
#include <qlabel.h>
#include <qlayout.h>


#include <kapp.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kservicegroup.h>
#include <kcmodule.h>
#include <krun.h>
#include <kprocess.h>
#include <qxembed.h>
#include <klocale.h>
#include <kstddirs.h>


#include "modules.h"
#include "modules.moc"
#include "global.h"
#include "utils.h"
#include "proxywidget.h"
#include "modloader.h"
#include "kcrootonly.h"

#include <X11/Xlib.h>


template class QList<ConfigModule>;


ConfigModule::ConfigModule(QString desktopFile)
  : ModuleInfo(desktopFile), _changed(false), _module(0), _embedWidget(0),
    _rootProcess(0), _embedLayout(0)
{
}

ConfigModule::~ConfigModule()
{
  deleteClient();
}

ProxyWidget *ConfigModule::module()
{
  if (_module)
    return _module;

  bool run_as_root = needsRootPrivileges() && (getuid() != 0);

  KCModule *modWidget = 0;
  
  if (run_as_root && !hasReadOnlyMode())
     modWidget = new KCRootOnly(0, "root_only");
  else
     modWidget = ModuleLoader::loadModule(*this);

  if (modWidget)
    {

      _module = new ProxyWidget(modWidget, name(), "", run_as_root);
      connect(_module, SIGNAL(changed(bool)), this, SLOT(clientChanged(bool)));
      connect(_module, SIGNAL(closed()), this, SLOT(clientClosed()));
      connect(_module, SIGNAL(helpRequest()), this, SIGNAL(helpRequest()));
      connect(_module, SIGNAL(runAsRoot()), this, SLOT(runAsRoot()));

      return _module;
    }

  return 0;
}

void ConfigModule::deleteClient()
{
  if (_embedWidget)
    XKillClient(qt_xdisplay(), _embedWidget->embeddedWinId());

  delete _rootProcess;
  _rootProcess = 0;

  delete _embedWidget;
  _embedWidget = 0;
  kapp->syncX();

  delete _module;
  _module = 0;

  delete _embedLayout;
  _embedLayout = 0;

  ModuleLoader::unloadModule(*this);
  _changed = false;
}

void ConfigModule::clientClosed()
{
  deleteClient();

  emit changed(this);
  emit childClosed();
}


void ConfigModule::clientChanged(bool state)
{
  setChanged(state);
  emit changed(this);
}


void ConfigModule::runAsRoot()
{
  if (!_module)
    return;

  delete _rootProcess;
  delete _embedWidget;
  delete _embedLayout;

  // create an embed widget that will embed the
  // kcmshell running as root
  _embedLayout = new QVBoxLayout(_module->parentWidget());
  _embedWidget = new QXEmbed(_module->parentWidget());
  _embedLayout->addWidget(_embedWidget,1);
  _module->hide();
  _embedWidget->show();
  QLabel *_busy = new QLabel(i18n("<big>Loading ...</big>"), _embedWidget);
  _busy->setAlignment(AlignCenter);
  _busy->setTextFormat(RichText);
  _busy->setGeometry(0,0, _module->width(), _module->height());
  _busy->show();

  // prepare the process to run the kcmshell
  QString cmd = service()->exec().stripWhiteSpace();
  bool kdeshell = false;
  if (cmd.left(5) == "kdesu2")
    cmd = cmd.remove(0,5).stripWhiteSpace();
  if (cmd.left(8) == "kcmshell")
    {
      cmd = cmd.remove(0,8).stripWhiteSpace();
      kdeshell = true;
    }

  // run the process
  QString kdesu = KStandardDirs::findExe("kdesu2");
  if (!kdesu.isEmpty())
    {
      _rootProcess = new KProcess;
      *_rootProcess << kdesu;
      // We have to disable the keep-password feature because
      // in that case the modules is started through kdesud and kdesu 
      // returns before the module is running and that doesn't work. 
      // We also don't have a way to close the module in that case.
      *_rootProcess << "--n"; // Don't keep password.
      if (kdeshell)
        *_rootProcess << "kcmshell";
      *_rootProcess << QString("%1 --embed %2").arg(cmd).arg(_embedWidget->winId());

      connect(_rootProcess, SIGNAL(processExited(KProcess*)), this, SLOT(rootExited(KProcess*)));

      _rootProcess->start(KProcess::NotifyOnExit);

      return;
    }

  // clean up in case of failure
  delete _embedWidget;
  _embedWidget = 0;
  delete _embedLayout;
  _embedLayout = 0;
  _module->show();
}


void ConfigModule::rootExited(KProcess *)
{
  if (_embedWidget->embeddedWinId())
    XDestroyWindow(qt_xdisplay(), _embedWidget->embeddedWinId());

  delete _embedWidget;
  _embedWidget = 0;

  delete _rootProcess;
  _rootProcess = 0;

  delete _embedLayout;
  _embedLayout = 0;

  delete _module;
  _module=0;

  _changed = false;
  emit changed(this);
  emit childClosed();
}


ConfigModuleList::ConfigModuleList()
{
  setAutoDelete(true);
}

const KAboutData *ConfigModule::aboutData() const
{
  if (!_module) return 0;
  return _module->aboutData();
}

void ConfigModuleList::readDesktopEntries()
{
  readDesktopEntriesRecursive( KCGlobal::baseGroup() );
}

void ConfigModuleList::readDesktopEntriesRecursive(const QString &path)
{
  KServiceGroup::Ptr group = KServiceGroup::group(path);

  if (!group || !group->isValid()) return;

  KServiceGroup::List list = group->entries(true);

  for( KServiceGroup::List::ConstIterator it = list.begin();
       it != list.end(); it++)
  {
     KSycocaEntry *p = (*it);
     if (p->isType(KST_KService))
     {
        ConfigModule *module = new ConfigModule(p->entryPath());
        append(module);
     }
     else if (p->isType(KST_KServiceGroup))
     {
        readDesktopEntriesRecursive(p->entryPath());
     }
  }
}
