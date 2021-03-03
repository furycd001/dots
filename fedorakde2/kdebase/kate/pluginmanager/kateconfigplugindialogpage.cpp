/***************************************************************************
                          kateconfigplugindialogpage.cpp  -  description
                             -------------------
    begin                : FRE Feb 23 2001
    copyright            : (C) 2001 by Joseph Wenninger
    email                : jowenn@bigfoot.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kateconfigplugindialogpage.h"
#include "kateconfigplugindialogpage.moc"

#include "katepluginmanager.h"
#include "../mainwindow/kateconfigdialog.h"
#include <klistbox.h>
#include "../app/kateapp.h"
#include <qstringlist.h>
#include <qhbox.h>
#include <qlabel.h>
#include <klocale.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <kiconloader.h>

KateConfigPluginPage::KateConfigPluginPage(QWidget *parent, KateConfigDialog *dialog):QVBox(parent)
{
  myPluginMan=((KateApp*)kapp)->getPluginManager();
  myDialog=dialog;

  QHBox *hbox = new QHBox (this);

  QVBox *vbox1 = new QVBox (hbox);
  QVBox *vbox2 = new QVBox (hbox);
  QVBox *vbox3 = new QVBox (hbox);

  QLabel *label1 = new QLabel (vbox1);
  label1->setText (i18n("Available Plugins"));

  QLabel *label2 = new QLabel (vbox3);
  label2->setText (i18n("Loaded Plugins"));

  availableBox=new KListBox(vbox1);
  loadedBox=new KListBox(vbox3);

  label = new QLabel (this);
  label->setMinimumHeight (50);
  label->setText (i18n("Select a plugin to get a short info here !"));

  unloadButton = new QPushButton( /*i18n("&Back"),*/ vbox2 );
  unloadButton->setPixmap(SmallIcon("back"));
  QToolTip::add(unloadButton, i18n("Previous directory"));
  loadButton = new QPushButton( /*i18n("&Next"),*/ vbox2 );
  loadButton->setPixmap(SmallIcon("forward"));
  QToolTip::add(loadButton, i18n("Next Directory"));

  unloadButton->setEnabled(false);
  loadButton->setEnabled(false);

  connect(availableBox,SIGNAL(highlighted(QListBoxItem *)),this,SLOT(slotActivatePluginItem (QListBoxItem *)));
  connect(availableBox,SIGNAL(selected(QListBoxItem *)), this,SLOT(slotActivatePluginItem (QListBoxItem *)));

  connect(loadedBox,SIGNAL(highlighted(QListBoxItem *)),this,SLOT(slotActivatePluginItem (QListBoxItem *)));
  connect(loadedBox,SIGNAL(selected(QListBoxItem *)), this,SLOT(slotActivatePluginItem (QListBoxItem *)));

  connect( unloadButton, SIGNAL( clicked() ), this, SLOT( unloadPlugin() ) );
  connect( loadButton, SIGNAL( clicked() ), this, SLOT( loadPlugin() ) );

  slotUpdate();
}

void KateConfigPluginPage::slotUpdate ()
{
  availableBox->clear();
  loadedBox->clear();

  for (uint i=0; i<myPluginMan->myPluginList.count(); i++)
  {
    if  (!myPluginMan->myPluginList.at(i)->load)
      availableBox->insertItem (myPluginMan->myPluginList.at(i)->name);
    else
      loadedBox->insertItem (myPluginMan->myPluginList.at(i)->name);
  }
}

void KateConfigPluginPage::slotActivatePluginItem (QListBoxItem *item)
{
  for (uint i=0; i<myPluginMan->myPluginList.count(); i++)
  {
    if  (myPluginMan->myPluginList.at(i)->name == item->text())
    {
      unloadButton->setEnabled(myPluginMan->myPluginList.at(i)->load);
      loadButton->setEnabled(!myPluginMan->myPluginList.at(i)->load);
      label->setText (i18n("Name: ") + myPluginMan->myPluginList.at(i)->name + i18n ("\nAuthor: ") + myPluginMan->myPluginList.at(i)->author + i18n ("\nDescription: ") + myPluginMan->myPluginList.at(i)->description);
    }
  }
}

void KateConfigPluginPage::loadPlugin ()
{
  QString text = availableBox->currentText ();

  for (uint i=0; i<myPluginMan->myPluginList.count(); i++)
  {
    if  (myPluginMan->myPluginList.at(i)->name == text)
    {
      myPluginMan->loadPlugin (myPluginMan->myPluginList.at(i));
      myPluginMan->enablePluginGUI (myPluginMan->myPluginList.at(i));
      myDialog->addPluginPage (myPluginMan->myPluginList.at(i)->plugin);
    }
  }

  slotUpdate();
}

void KateConfigPluginPage::unloadPlugin ()
{
  QString text = loadedBox->currentText ();

  for (uint i=0; i<myPluginMan->myPluginList.count(); i++)
  {
    if  (myPluginMan->myPluginList.at(i)->name == text)
    {
      myDialog->removePluginPage (myPluginMan->myPluginList.at(i)->plugin);
      myPluginMan->unloadPlugin (myPluginMan->myPluginList.at(i));
    }
  }

  slotUpdate();
}
