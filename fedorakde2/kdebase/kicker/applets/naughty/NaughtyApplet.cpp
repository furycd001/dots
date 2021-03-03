/*
    Naughty applet - Runaway process monitor for the KDE panel

    Copyright 2000 Rik Hemsley (rikkus) <rik@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "NaughtyApplet.h"
#include "NaughtyProcessMonitor.h"
#include "NaughtyConfigDialog.h"

#include <qmessagebox.h>
#include <qtoolbutton.h>
#include <qlayout.h>

#include <kiconloader.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kaboutapplication.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kmessagebox.h>

extern "C"
{
    KPanelApplet *
  init(QWidget * parent, const QString & configFile)
  {
    KGlobal::locale()->insertCatalogue("naughtyapplet");

    return new NaughtyApplet
      (
       configFile,
       KPanelApplet::Normal,
       KPanelApplet::About | KPanelApplet::Preferences,
       parent,
       "naughtyapplet"
      );
  }
}

NaughtyApplet::NaughtyApplet
(
 const QString & configFile,
 Type t,
 int actions,
 QWidget * parent,
 const char * name
)
  : KPanelApplet(configFile, t, actions, parent, name)
{
  KGlobal::iconLoader()->addAppDir("naughtyapplet");
  setBackgroundMode(X11ParentRelative);

  button_ = new QPushButton(this);
  button_->setFixedSize(20, 20);

  QVBoxLayout * layout = new QVBoxLayout(this);
  layout->addWidget(button_);

  monitor_ = new NaughtyProcessMonitor(2, 20, this);

  connect
    (
     button_,   SIGNAL(clicked()),
     this,      SLOT(slotPreferences())
    );

  connect
    (
     monitor_,  SIGNAL(runawayProcess(ulong, const QString &)),
     this,      SLOT(slotWarn(ulong, const QString &))
    );

  connect
    (
     monitor_,  SIGNAL(load(uint)),
     this,      SLOT(slotLoad(uint))
    );

  loadSettings();

  monitor_->start();
}

NaughtyApplet::~NaughtyApplet()
{
}

  void
NaughtyApplet::slotWarn(ulong pid, const QString & name)
{
  if (ignoreList_.contains(name))
    return;

  QString s = i18n("A program called '%1' is slowing down the others\n"
                   "on your machine. It may have a bug that is causing\n"
                   "this, or it may just be busy.\n\n"
                   "Would you like to try to stop the program ?");

  int retval = KMessageBox::warningYesNo(this, s.arg(name));

  if (KMessageBox::Yes == retval)
    monitor_->kill(pid);
  else
  {
    s = i18n("In future, should busy programs called '%1'\n"
             "be ignored?");

    retval = KMessageBox::questionYesNo(this, s.arg(name));

    if (KMessageBox::Yes == retval)
    {
      ignoreList_.append(name);
      config()->writeEntry("IgnoreList", ignoreList_);
      config()->sync();
    }
  }
}

  int
NaughtyApplet::widthForHeight(int) const
{
  return 20;
}

  int
NaughtyApplet::heightForWidth(int) const
{
  return 20;
}

  void
NaughtyApplet::slotLoad(uint l)
{
  if (l > monitor_->triggerLevel())
    button_->setPixmap(BarIcon("naughty-sad"));
  else
    button_->setPixmap(BarIcon("naughty-happy"));
}

  void
NaughtyApplet::about()
{
  KAboutData about
    (
     "naughtyapplet",
     I18N_NOOP("Naughty applet"), 
     "1.0",
     I18N_NOOP("Runaway process catcher"),
     KAboutData::License_GPL_V2,
     "(C) 2000 Rik Hemsley (rikkus) <rik@kde.org>"
   );
     
  KAboutApplication a(&about, this);
  a.exec();
}

  void
NaughtyApplet::slotPreferences()
{
  preferences();
}

  void
NaughtyApplet::preferences()
{
  NaughtyConfigDialog d
    (
     ignoreList_,
     monitor_->interval(),
     monitor_->triggerLevel(),
     this
    );

  QDialog::DialogCode retval = QDialog::DialogCode(d.exec());

  if (QDialog::Accepted == retval)
  {
    ignoreList_ = d.ignoreList();
    monitor_->setInterval(d.updateInterval());
    monitor_->setTriggerLevel(d.threshold());
    saveSettings();
  }
}

  void
NaughtyApplet::loadSettings()
{
  ignoreList_ = config()->readListEntry("IgnoreList");
  monitor_->setInterval(config()->readUnsignedNumEntry("UpdateInterval", 2));
  monitor_->setTriggerLevel(config()->readUnsignedNumEntry("Threshold", 20));

  // Add 'X' as a default.
  if (ignoreList_.isEmpty() && !config()->hasKey("IgnoreList"))
    ignoreList_.append("X");
}

  void
NaughtyApplet::saveSettings()
{
  config()->writeEntry("IgnoreList",      ignoreList_);
  config()->writeEntry("UpdateInterval",  monitor_->interval());
  config()->writeEntry("Threshold",       monitor_->triggerLevel());
  config()->sync();
}

#include "NaughtyApplet.moc"

