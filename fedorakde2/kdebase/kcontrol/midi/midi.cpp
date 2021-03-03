/**
 *  midi.cpp
 *
 *  Copyright (c) 2000 Antonio Larrosa <larrosa@kde.org>
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
#include <qvbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <klocale.h>
#include <libkmid/deviceman.h>
#include <kurlrequester.h>

#include "midi.h"

KMidConfig::KMidConfig(QWidget *parent, const char *name)
  : KCModule(parent, name)
{
  QVBoxLayout *topLayout = new QVBoxLayout(this,5);

  label=new QLabel(i18n("Select the midi device you want to use :"),this);
//  label->adjustSize();
  mididevices=new QListBox(this,"midideviceslist");
  connect(mididevices,SIGNAL(highlighted(int)),SLOT(deviceSelected(int)));
  devman=new DeviceManager();
  int r=devman->initManager();
  
  QString s;
  for (int i=0;i<devman->midiPorts()+devman->synthDevices();i++)
  {
    if (strcmp(devman->type(i),"")!=0)
      s.sprintf("%s - %s",devman->name(i),devman->type(i));
    else
      s.sprintf("%s",devman->name(i));

    mididevices->insertItem(s,i);
  };
  
  usemap=new QCheckBox(i18n("Use Midi Mapper"),this,"usemidimapper");
  
  connect(usemap,SIGNAL(toggled(bool)),SLOT(useMap(bool)));

  maprequester=new KURLRequester(this,"maprequester");

  topLayout->addWidget(label);
  topLayout->addWidget(mididevices);
  topLayout->addWidget(usemap);
  topLayout->addWidget(maprequester);

  load();
  
  mididevices->setFocus();
}

KMidConfig::~KMidConfig() 
{
}

void KMidConfig::configChanged()
{
  emit changed(true);
}

void KMidConfig::useMap(bool i)
{
  maprequester->setEnabled(i); 
  
  emit changed(true);
}

void KMidConfig::deviceSelected(int)
{
  emit changed(true);
}

void KMidConfig::load()
{
  KConfig *config = new KConfig("kcmmidirc", true);
  
  config->setGroup("Configuration");
  mididevices->setCurrentItem(config->readNumEntry("midiDevice",0));
  QString mapurl(config->readEntry("mapFilename",""));
//  KURL::encode(mapurl);
  maprequester->setURL(mapurl);
  usemap->setChecked(config->readBoolEntry("useMidiMapper", false));
  maprequester->setEnabled(usemap->isChecked());
  delete config;

  emit changed(false);
}

void KMidConfig::save()
{

  KConfig *config= new KConfig("kcmmidirc", false);

  config->setGroup("Configuration");

  config->writeEntry("midiDevice", mididevices->currentItem());
  config->writeEntry("useMidiMapper", usemap->isChecked());
  config->writeEntry("mapFilename", maprequester->url());
  
  config->sync();
  delete config;

  emit changed(false);
}

void KMidConfig::defaults()
{
  usemap->setChecked(false);
  
  emit changed(true);
}


extern "C"
{
  KCModule *create_midi(QWidget *parent, const char *name)
  {
    KGlobal::locale()->insertCatalogue("kcmmidi");
    return new KMidConfig(parent, name);
  };
}
#include "midi.moc"
