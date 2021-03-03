/*
 *  main.cpp
 *
 *  Copyright (C) 1998 Luca Montecchiani <m.luca@usa.net>
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
 *
 */
#include <unistd.h>

#include <qtabwidget.h>
#include <qlabel.h>
#include <qlayout.h>

#include <dcopclient.h>
#include <kapp.h>
#include <kglobal.h>
#include <klocale.h>
#include <kcmodule.h>

#include "main.h"
#include "main.moc"

#include "tzone.h"
#include "dtime.h"

KclockModule::KclockModule(QWidget *parent, const char *name)
  : KCModule(parent, name)
{
  QVBoxLayout *layout = new QVBoxLayout(this);

  dtime = new Dtime(this);
  layout->addWidget(dtime);
  connect(dtime, SIGNAL(timeChanged(bool)), this, SLOT(moduleChanged(bool)));

  tzone = new Tzone(this);
  layout->addWidget(tzone);
  connect(tzone, SIGNAL(zoneChanged(bool)), this, SLOT(moduleChanged(bool)));

  layout->addStretch();

  if(getuid() == 0)
    setButtons(Help|Apply);
  else
    setButtons(Help);


}

void KclockModule::save()
{
  tzone->save();
  dtime->save();

    // restart kicker to sync up the time
    if (!kapp->dcopClient()->isAttached())
    {
        kapp->dcopClient()->attach();
    }

    QByteArray data;
    kapp->dcopClient()->send( "kicker", "Panel", "restart()", data );

}

void KclockModule::load()
{
  dtime->load();
  tzone->load();
}

QString KclockModule::quickHelp() const
{
  return i18n("<h1>Date & Time</h1> This control module can be used to set the system date and"
    " time. As these settings do not only affect you as a user, but rather the whole system, you"
    " can only change these settings when you start the Control Center as root. If you don't have"
    " the root password, but feel the system time should be corrected, please contact your system"
    " administrator.");
}

void KclockModule::moduleChanged(bool state)
{
  emit changed(state);
}

extern "C"
{
  KCModule *create_clock(QWidget *parent, const char *name)
  {
    KGlobal::locale()->insertCatalogue("kcmkclock");
    return new KclockModule(parent, name);
  }
}


