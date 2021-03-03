/*
 *  applettab.cpp
 *
 *  Copyright (c) 2000 Matthias Elter <elter@kde.org>
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
 */

#include <qlayout.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qwhatsthis.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qvbox.h>
#include <qfileinfo.h>

#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kdialog.h>
#include <kstddirs.h>
#include <klistview.h>
#include <kdebug.h>

#include "applettab.h"
#include "applettab.moc"


extern int kickerconfig_screen_number;


AppletTab::AppletTab( QWidget *parent, const char* name )
  : QWidget (parent, name)
{
  layout = new QGridLayout(this, 2, 1, 6, 11);

  // security level group
  level_group = new QButtonGroup(i18n("Security Level"), this);
  level_group->setRadioButtonExclusive(true);
  connect(level_group, SIGNAL(clicked(int)), SLOT(level_changed(int)));

  level_group->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5,(QSizePolicy::SizeType)0,level_group->sizePolicy().hasHeightForWidth() ) );   level_group->setColumnLayout(0, Qt::Vertical );
  level_group->layout()->setSpacing( 0 );
  level_group->layout()->setMargin( 0 );
  QVBoxLayout *vbox = new QVBoxLayout( level_group->layout() );
  vbox->setAlignment( Qt::AlignTop );
  vbox->setSpacing(KDialog::marginHint() );
  vbox->setMargin(  KDialog::spacingHint() );

  trusted_rb = new QRadioButton(i18n("Load only trusted applets internal"), level_group);
  vbox->addWidget(trusted_rb);

  new_rb = new QRadioButton(i18n("Load startup config applets internal"), level_group);
  vbox->addWidget(new_rb);

  all_rb = new QRadioButton(i18n("Load all applets internal"), level_group);
  vbox->addWidget(all_rb);
  QWhatsThis::add( level_group, i18n("Panel applets can be started in two different ways:"
    " internal or external. While 'internal' is the preferred way to load applets, this can"
    " raise stability or security problems when you are using poorly programmed third-party applets."
    " To address these problems, applets can be marked 'trusted'. You might want to configure"
    " kicker to treat trusted applets different from untrusted ones. Your options are:"
    " <ul><li><em>Load only trusted applets internal:</em> All applets but the ones marked 'trusted'"
    " will be loaded using an external wrapper application.</li>"
    " <li><em>Load startup config applets internal:</em> The applets shown on KDE startup"
    " will be loaded internally, others will be loaded using an external wrapper application.</li>"
    " <li><em>Load all applets internal</em></li></ul>") );

  layout->addWidget(level_group,0,0);

  // trusted list group
  list_group = new QGroupBox(i18n("List of Trusted Applets"), this);

  QVBoxLayout *vbox1 = new QVBoxLayout(list_group, KDialog::marginHint(),
                                       KDialog::spacingHint());
  vbox1->addSpacing(fontMetrics().lineSpacing());

  QHBoxLayout *hbox = new QHBoxLayout(vbox1, KDialog::spacingHint());

  lb_trusted = new KListView(list_group);
  lb_trusted->addColumn(i18n("Trusted Applets"));
  connect(lb_trusted, SIGNAL(selectionChanged(QListViewItem*)),
          SLOT(trusted_selection_changed(QListViewItem*)));
  hbox->addWidget(lb_trusted);
  QWhatsThis::add( lb_trusted, i18n("Here you can see a list of applets that are marked"
    " 'trusted', i.e. will be loaded internally by kicker in any case. To move an applet"
    " from the list of available applets to the trusted ones or vice versa, select it and"
    " press the left or right buttons.") );

  QVBox *vbox2 = new QVBox(list_group);
  pb_add = new QPushButton(i18n("<<"), vbox2);
  QWhatsThis::add( pb_add, i18n("Click here to add the selected applet from the list of available,"
    " untrusted applets to the list of trusted applets.") );
  pb_remove = new QPushButton(i18n(">>"), vbox2);
  QWhatsThis::add( pb_remove, i18n("Click here to remove the selected applet from the list of trusted"
    " applets to the list of available, untrusted applets.") );
  pb_add->setEnabled(false);
  pb_remove->setEnabled(false);
  connect(pb_add, SIGNAL(clicked()), SLOT(add_clicked()));
  connect(pb_remove, SIGNAL(clicked()), SLOT(remove_clicked()));
  hbox->addWidget(vbox2);

  lb_available = new KListView(list_group);
  lb_available->addColumn(i18n("Available Applets"));
  connect(lb_available, SIGNAL(selectionChanged(QListViewItem*)),
          SLOT(available_selection_changed(QListViewItem*)));
  hbox->addWidget(lb_available);
  QWhatsThis::add( lb_available, i18n("Here you can see a list of available applets that you"
    " currently don't trust. This doesn't mean you can't use those applets, but rather that"
    " the panel's policy using them depends on your applet security level. To move an applet"
    " from the list of available applets to the trusted ones or vice versa, select it and"
    " press the left or right buttons.") );

  layout->addWidget(list_group,1,0);

  layout->setRowStretch(0, 1);
  layout->setRowStretch(1, 2);

  load();
}

void AppletTab::load()
{
  QCString configname;
  if (kickerconfig_screen_number == 0)
      configname = "kickerrc";
  else
      configname.sprintf("kicker-screen-%drc", kickerconfig_screen_number);
  KConfig *c = new KConfig(configname, false, false);

  c->setGroup("General");

  available.clear();
  l_available.clear();
  l_trusted.clear();

  int level = c->readNumEntry("SecurityLevel", 1);

  switch(level)
    {
    case 0:
    default:
      trusted_rb->setChecked(true);
      break;
    case 1:
      new_rb->setChecked(true);
      break;
    case 2:
      all_rb->setChecked(true);
      break;
    }

  list_group->setEnabled(trusted_rb->isChecked());

  QStringList list = KGlobal::dirs()->findAllResources("applets", "*.desktop");
  for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
    {
      QFileInfo fi(*it);
      available << fi.baseName();
    }

  if(c->hasKey("TrustedApplets"))
    {
      QStringList list = c->readListEntry("TrustedApplets");
      for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
        {
          if(available.contains(*it))
            l_trusted << (*it);
        }
    }
  else
      l_trusted << "clockapplet" << "ksystemtrayapplet" << "krunapplet" << "quicklauncher"
                << "kminipagerapplet" << "ktaskbarapplet" << "eyesapplet" << "kmixapplet";

  for ( QStringList::Iterator it = available.begin(); it != available.end(); ++it )
    {
      if(!l_trusted.contains(*it))
        l_available << (*it);
    }

  updateTrusted();
  updateAvailable();

  delete c;
}

void AppletTab::save()
{
  QCString configname;
  if (kickerconfig_screen_number == 0)
      configname = "kickerrc";
  else
      configname.sprintf("kicker-screen-%drc", kickerconfig_screen_number);
  KConfig *c = new KConfig(configname, false, false);

  c->setGroup("General");

  int level = 0;
  if(new_rb->isChecked()) level = 1;
  else if (all_rb->isChecked()) level = 2;

  c->writeEntry("SecurityLevel", level);
  c->writeEntry("TrustedApplets", l_trusted);
  c->sync();

  delete c;
}

void AppletTab::defaults()
{
  new_rb->setChecked(true);
  list_group->setEnabled(false);
}

QString AppletTab::quickHelp() const
{
  return i18n("");
}

void AppletTab::level_changed(int)
{
  list_group->setEnabled(trusted_rb->isChecked());
  emit changed();
}

void AppletTab::updateTrusted()
{
  lb_trusted->clear();
  for ( QStringList::Iterator it = l_trusted.begin(); it != l_trusted.end(); ++it )
    (void) new QListViewItem(lb_trusted, (*it));
}

void AppletTab::updateAvailable()
{
  lb_available->clear();
  for ( QStringList::Iterator it = l_available.begin(); it != l_available.end(); ++it )
    (void) new QListViewItem(lb_available, (*it));
}

void AppletTab::trusted_selection_changed(QListViewItem * item)
{
  pb_remove->setEnabled(item != 0);
  emit changed();
}

void AppletTab::available_selection_changed(QListViewItem * item)
{
  pb_add->setEnabled(item != 0);
  emit changed();
}

void AppletTab::add_clicked()
{
  QListViewItem *item = lb_available->selectedItem();
  if (!item) return;
  l_available.remove(item->text(0));
  l_trusted.append(item->text(0));

  updateTrusted();
  updateAvailable();
  updateAddRemoveButton();
}

void AppletTab::remove_clicked()
{
  QListViewItem *item = lb_trusted->selectedItem();
  if (!item) return;
  l_trusted.remove(item->text(0));
  l_available.append(item->text(0));

  updateTrusted();
  updateAvailable();
  updateAddRemoveButton();
}


void AppletTab::updateAddRemoveButton()
{
    pb_remove->setEnabled(l_trusted.count ()>0);
    pb_add->setEnabled(l_available.count()>0);
}
