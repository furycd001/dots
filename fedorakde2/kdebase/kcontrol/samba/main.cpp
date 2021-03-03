/*
 * main.cpp for the samba kcontrol module
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

#include "ksmbstatus.h"
#include "kcmsambaimports.h"
#include "kcmsambalog.h"
#include "kcmsambastatistics.h"
#include <klocale.h>
#include <qtabwidget.h>
#include <kglobal.h>
#include <qlayout.h>
#include <kconfig.h>

#include <kcmodule.h>

class SambaContainer:public KCModule
{
   public:
      SambaContainer(QWidget *parent=0, const char * name=0);
      virtual ~SambaContainer();
      virtual void loadSettings();
      virtual void saveSettings();
      virtual void defaults() {};
      QString quickHelp() const;

   private:
      QVBoxLayout layout;
      KConfig config;
      QTabWidget tabs;
      NetMon status;
      ImportsView imports;
      LogView logView;
      StatisticsView statisticsView;
};

SambaContainer::SambaContainer(QWidget *parent, const char* name)
:KCModule(parent,name)
,layout(this)
,config("kcmsambarc",false,true)
,tabs(this)
,status(&tabs,&config)
,imports(&tabs,&config)
,logView(&tabs,&config)
,statisticsView(&tabs,&config)
{
   layout.addWidget(&tabs);
   tabs.addTab(&status,i18n("&Exports"));
   tabs.addTab(&imports,i18n("&Imports"));
   tabs.addTab(&logView,i18n("&Log"));
   tabs.addTab(&statisticsView,i18n("&Statistics"));
   connect(&logView,SIGNAL(contentsChanged(QListView* , int, int)),&statisticsView,SLOT(setListInfo(QListView *, int, int)));
   setButtons(Help);
   loadSettings();
};

SambaContainer::~SambaContainer()
{
   saveSettings();
}

#include <iostream>

void SambaContainer::loadSettings()
{
   status.loadSettings();
   imports.loadSettings();
   logView.loadSettings();
   statisticsView.loadSettings();
}

void SambaContainer::saveSettings()
{
   status.saveSettings();
   imports.saveSettings();
   logView.saveSettings();
   statisticsView.saveSettings();
   config.sync();
}


QString SambaContainer::quickHelp() const
{
   return i18n("The Samba and NFS Status Monitor is a front end to the programs"
     " <em>smbstatus</em> and <em>showmount</em>. Smbstatus reports on current"
     " Samba connections, and is part of the suite of Samba tools, which"
     " implements the SMB (Session Message Block) protocol, also called the"
     " NetBIOS or LanManager protocol. This protocol can be used to provide"
     " printer sharing or drive sharing services on a network including"
     " machines running the various flavors of Microsoft Windows.<p>"
     " Showmount is part of the NFS software package. NFS stands for Network"
     " File System and is the traditional UNIX way to share directories over"
     " the network. In this case the output of <em>showmount -a localhost</em>"
     " is parsed. On some systems showmount is in /usr/sbin, check if you have"
     " showmount in your PATH.");
}


extern "C"
{

  KCModule *create_samba(QWidget *parent, const char *name)
  {
    KGlobal::locale()->insertCatalogue("kcmsamba");
    return new SambaContainer(parent, name);
  }
}
