/*
 *
 * Copyright (c) 1998 Matthias Ettrich <ettrich@kde.org>
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

#include <qlabel.h>

#include <kapp.h>
#include <dcopclient.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kdialog.h>
#include <kglobalsettings.h>
#include <kseparator.h>
#include <qcombobox.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "mouse.h"
#include "geom.h"
#include <qlayout.h>
#include <qwhatsthis.h>
#include <qtl.h>

#include <kaccel.h>	// For KAccel::keyboardHasMetaKey()

KActionsConfig::~KActionsConfig ()
{

}

KActionsConfig::KActionsConfig (KConfig *_config, QWidget * parent, const char *name)
  : KCModule (parent, name), config(_config)
{
  QString strWin1, strWin2, strWin3, strAllKey, strAll1, strAll2, strAll3;
  QGridLayout *layout = new QGridLayout( this, 17, 4,
                     KDialog::marginHint(), 1);
  QLabel* label;
  QString strMouseButton1, strMouseButton3;
  QString txtButton1, txtButton3;
  bool leftHandedMouse;

  label = new QLabel(i18n("Titlebar doubleclick:"), this);
  layout->addMultiCellWidget(label, 0,0,0,1);
  QWhatsThis::add( label, i18n("Here you can customize mouse click behavior when double clicking on the"
    " titlebar of a window.") );

  KSeparator *line = new KSeparator(KSeparator::HLine, this);
  layout->addMultiCellWidget(line, 1, 1, 0, 3, AlignVCenter);
  layout->setRowStretch(1, 1);

  QComboBox* combo = new QComboBox(this);
  combo->insertItem(i18n("Maximize"));
  combo->insertItem(i18n("Maximize (vertical only)"));
  combo->insertItem(i18n("Maximize (horizontal only)"));
  combo->insertItem(i18n("Shade"));
  combo->insertItem(i18n("Lower"));
  connect(combo, SIGNAL(activated(int)), this, SLOT(slotChanged()));
  layout->addMultiCellWidget(combo, 0, 0, 2, 3);
  coTiDbl = combo;
  QWhatsThis::add(combo, i18n("Behavior on <em>double</em> click into the titlebar."));

  label = new QLabel(i18n("Active"), this);
  layout->addWidget(label, 2,2, AlignHCenter);
  QWhatsThis::add( label, i18n("In this column you can customize mouse clicks into the titlebar"
    " or the frame of an active window.") );

  label = new QLabel(i18n("Inactive"), this);
  layout->addWidget(label, 2,3, AlignHCenter);
  QWhatsThis::add( label, i18n("In this column you can customize mouse clicks into the titlebar"
    " or the frame of an inactive window.") );

  label = new QLabel(i18n("Titlebar and frame:"), this);
  layout->addMultiCellWidget(label, 2,2,0,1);
  QWhatsThis::add( label, i18n("Here you can customize mouse click behavior when clicking on the"
    " titlebar or the frame of a window.") );

  line = new KSeparator(KSeparator::HLine, this);
  layout->addMultiCellWidget(line, 6, 6, 0, 3, AlignVCenter);
  layout->setRowStretch(6, 1);

  label = new QLabel(i18n("Inactive inner window:"), this);
  layout->addMultiCellWidget(label, 7,7,0,3);
  QWhatsThis::add( label, i18n("Here you can customize mouse click behavior when clicking on an inactive"
    " inner window ('inner' means: not titlebar, not frame).") );

  line = new KSeparator(KSeparator::HLine, this);
  layout->addMultiCellWidget(line, 11, 11, 0, 3, AlignVCenter);
  layout->setRowStretch(11, 1);

  label = new QLabel(i18n("Inner window, titlebar and frame:"), this);
  layout->addMultiCellWidget(label, 12,12,0,3);
  QWhatsThis::add( label, i18n("Here you can customize KDE's behavior when clicking somewhere into"
    " a window while pressing a modifier key."));

  /*
   * The text on this form depends on the mouse setting, which can be right
   * or left handed.  The outer button functionality is actually swapped
   *
   */
  leftHandedMouse = ( KGlobalSettings::mouseSettings().handed == KGlobalSettings::KMouseSettings::LeftHanded);

  strMouseButton1 = i18n("Left Button");
  txtButton1 = i18n("In this row you can customize left click behavior when clicking into"
     " the titlebar or the frame.");

  strMouseButton3 = i18n("Right Button");
  txtButton3 = i18n("In this row you can customize right click behavior when clicking into"
     " the titlebar or the frame." );

  if ( leftHandedMouse )
  {
     qSwap(strMouseButton1, strMouseButton3);
     qSwap(txtButton1, txtButton3);
  }

  label = new QLabel(strMouseButton1, this);
  layout->addWidget(label, 3,1);
  QWhatsThis::add( label, txtButton1);

  label = new QLabel(i18n("Middle Button"), this);
  layout->addWidget(label, 4,1);
  QWhatsThis::add( label, i18n("In this row you can customize middle click behavior when clicking into"
    " the titlebar or the frame.") );

  label = new QLabel(strMouseButton3, this);
  layout->addWidget(label, 5,1);
  QWhatsThis::add( label, txtButton3);

  // Inactive inner window

  strWin1 = i18n("In this row you can customize left click behavior when clicking into"
     " an inactive inner window ('inner' means: not titlebar, not frame).");

  strWin3 = i18n("In this row you can customize right click behavior when clicking into"
     " an inactive inner window ('inner' means: not titlebar, not frame).");

  // Be nice to lefties
  if ( leftHandedMouse ) qSwap(strWin1, strWin3);

  label = new QLabel(strMouseButton1, this);
  layout->addWidget(label, 8,1);
  QWhatsThis::add( label, strWin1 );

  label = new QLabel(i18n("Middle Button"), this);
  layout->addWidget(label, 9,1);
  strWin2 = i18n("In this row you can customize middle click behavior when clicking into"
     " an inactive inner window ('inner' means: not titlebar, not frame).");
  QWhatsThis::add( label, strWin2 );

  label = new QLabel(strMouseButton3, this);
  layout->addWidget(label, 10,1);
  QWhatsThis::add( label, strWin3 );

  // Inner window, titlebar and frame

  strMouseButton1 = i18n("Modifier Key + Left Button");
  strAll1 = i18n("In this row you can customize left click behavior when clicking into"
     " the titlebar or the frame.");

  strMouseButton3 = i18n("Modifier Key + Right Button");
  strAll3 = i18n("In this row you can customize right click behavior when clicking into"
     " the titlebar or the frame." );

  if ( leftHandedMouse )
  {
     qSwap(strMouseButton1, strMouseButton3);
     qSwap(strAll1, strAll3);
  }

  label = new QLabel(i18n("Modifier Key"), this );
  layout->addWidget(label, 13, 1);
  strAllKey = i18n("Here you select whether holding the Meta key or Alt key "
    "will allow you to perform the following actions.");
  QWhatsThis::add( label, strAllKey );

  label = new QLabel(strMouseButton1, this);
  layout->addWidget(label, 14,1);
  QWhatsThis::add( label, strAll1);

  label = new QLabel(i18n("Modifier Key + Middle Button"), this);
  layout->addWidget(label, 15,1);
  strAll2 = i18n("Here you can customize KDE's behavior when middle clicking into a window"
    " while pressing the modifier key.");
  QWhatsThis::add( label, strAll2 );

  label = new QLabel(strMouseButton3, this);
  layout->addWidget(label, 16,1);
  QWhatsThis::add( label, strAll3);

  // Titlebar and frame, active, mouse button 1
  combo = new QComboBox(this);
  combo->insertItem(i18n("Raise"));
  combo->insertItem(i18n("Lower"));
  combo->insertItem(i18n("Operations menu"));
  combo->insertItem(i18n("Toggle raise and lower"));
  connect(combo, SIGNAL(activated(int)), this, SLOT(slotChanged()));
  layout->addWidget(combo, 3,2);
  coTiAct1 = combo;

  txtButton1 = i18n("Behavior on <em>left</em> click into the titlebar or frame of an "
     "<em>active</em> window.");

  txtButton3 = i18n("Behavior on <em>right</em> click into the titlebar or frame of an "
     "<em>active</em> window.");

  // Be nice to left handed users
  if ( leftHandedMouse ) qSwap(txtButton1, txtButton3);

  QWhatsThis::add(combo, txtButton1);

  // Titlebar and frame, active, mouse button 2
  combo = new QComboBox(this);
  combo->insertItem(i18n("Raise"));
  combo->insertItem(i18n("Lower"));
  combo->insertItem(i18n("Operations menu"));
  combo->insertItem(i18n("Toggle raise and lower"));
  combo->insertItem(i18n("Nothing"));
  combo->insertItem(i18n("Shade"));
  connect(combo, SIGNAL(activated(int)), this, SLOT(slotChanged()));
  layout->addWidget(combo, 4,2);
  coTiAct2 = combo;
  QWhatsThis::add(combo, i18n("Behavior on <em>middle</em> click into the titlebar or frame of an <em>active</em> window."));

  // Titlebar and frame, active, mouse button 3
  combo = new QComboBox(this);
  combo->insertItem(i18n("Raise"));
  combo->insertItem(i18n("Lower"));
  combo->insertItem(i18n("Operations menu"));
  combo->insertItem(i18n("Toggle raise and lower"));
  combo->insertItem(i18n("Nothing"));
  combo->insertItem(i18n("Shade"));
  connect(combo, SIGNAL(activated(int)), this, SLOT(slotChanged()));
  layout->addWidget(combo, 5,2);
  coTiAct3 =  combo;
  QWhatsThis::add(combo, txtButton3 );

  txtButton1 = i18n("Behavior on <em>left</em> click into the titlebar or frame of an "
     "<em>inactive</em> window.");

  txtButton3 = i18n("Behavior on <em>right</em> click into the titlebar or frame of an "
     "<em>inactive</em> window.");

  // Be nice to left handed users
  if ( leftHandedMouse ) qSwap(txtButton1, txtButton3);

  combo = new QComboBox(this);
  combo->insertItem(i18n("Activate and raise"));
  combo->insertItem(i18n("Activate and lower"));
  combo->insertItem(i18n("Activate"));
  combo->insertItem(i18n("Shade"));
  connect(combo, SIGNAL(activated(int)), this, SLOT(slotChanged()));
  layout->addWidget(combo, 3,3);
  coTiInAct1 = combo;
  QWhatsThis::add(combo, txtButton1);

  combo = new QComboBox(this);
  combo->insertItem(i18n("Activate and raise"));
  combo->insertItem(i18n("Activate and lower"));
  combo->insertItem(i18n("Activate"));
  combo->insertItem(i18n("Shade"));
  connect(combo, SIGNAL(activated(int)), this, SLOT(slotChanged()));
  layout->addWidget(combo, 4,3);
  coTiInAct2 = combo;
  QWhatsThis::add(combo, i18n("Behavior on <em>middle</em> click into the titlebar or frame of an <em>inactive</em> window."));

  combo = new QComboBox(this);
  combo->insertItem(i18n("Activate and raise"));
  combo->insertItem(i18n("Activate and lower"));
  combo->insertItem(i18n("Activate"));
  combo->insertItem(i18n("Shade"));
  connect(combo, SIGNAL(activated(int)), this, SLOT(slotChanged()));
  layout->addWidget(combo, 5,3);
  coTiInAct3 = combo;
  QWhatsThis::add(combo, txtButton3);

  combo = new QComboBox(this);
  combo->insertItem(i18n("Activate, raise and pass click"));
  combo->insertItem(i18n("Activate and pass click"));
  combo->insertItem(i18n("Activate"));
  combo->insertItem(i18n("Activate and raise"));
  connect(combo, SIGNAL(activated(int)), this, SLOT(slotChanged()));
  layout->addMultiCellWidget(combo, 8,8, 2, 3);
  coWin1 = combo;
  QWhatsThis::add( combo, strWin1 );

  combo = new QComboBox(this);
  combo->insertItem(i18n("Activate, raise and pass click"));
  combo->insertItem(i18n("Activate and pass click"));
  combo->insertItem(i18n("Activate"));
  combo->insertItem(i18n("Activate and raise"));
  connect(combo, SIGNAL(activated(int)), this, SLOT(slotChanged()));
  layout->addMultiCellWidget(combo, 9,9, 2, 3);
  coWin2 = combo;
  QWhatsThis::add( combo, strWin2 );

  combo = new QComboBox(this);
  combo->insertItem(i18n("Activate, raise and pass click"));
  combo->insertItem(i18n("Activate and pass click"));
  combo->insertItem(i18n("Activate"));
  combo->insertItem(i18n("Activate and raise"));
  connect(combo, SIGNAL(activated(int)), this, SLOT(slotChanged()));
  layout->addMultiCellWidget(combo, 10,10, 2, 3);
  coWin3 = combo;
  QWhatsThis::add( combo, strWin3 );

  combo = new QComboBox(this);
  combo->insertItem(i18n("Meta"));
  combo->insertItem(i18n("Alt"));
  connect(combo, SIGNAL(activated(int)), this, SLOT(slotChanged()));
  layout->addMultiCellWidget(combo, 13,13, 2, 3);
  coAllKey = combo;
  QWhatsThis::add( combo, strAll1 );

  combo = new QComboBox(this);
  combo->insertItem(i18n("Move"));
  combo->insertItem(i18n("Toggle raise and lower"));
  combo->insertItem(i18n("Resize"));
  combo->insertItem(i18n("Raise"));
  combo->insertItem(i18n("Lower"));
  combo->insertItem(i18n("Nothing"));
  connect(combo, SIGNAL(activated(int)), this, SLOT(slotChanged()));
  layout->addMultiCellWidget(combo, 14,14, 2, 3);
  coAll1 = combo;
  QWhatsThis::add( combo, strAll1 );

  combo = new QComboBox(this);
  combo->insertItem(i18n("Move"));
  combo->insertItem(i18n("Toggle raise and lower"));
  combo->insertItem(i18n("Resize"));
  combo->insertItem(i18n("Raise"));
  combo->insertItem(i18n("Lower"));
  combo->insertItem(i18n("Nothing"));
  connect(combo, SIGNAL(activated(int)), this, SLOT(slotChanged()));
  layout->addMultiCellWidget(combo, 15,15, 2, 3);
  coAll2 = combo;
  QWhatsThis::add( combo, strAll2 );

  combo = new QComboBox(this);
  combo->insertItem(i18n("Move"));
  combo->insertItem(i18n("Toggle raise and lower"));
  combo->insertItem(i18n("Resize"));
  combo->insertItem(i18n("Raise"));
  combo->insertItem(i18n("Lower"));
  combo->insertItem(i18n("Nothing"));
  connect(combo, SIGNAL(activated(int)), this, SLOT(slotChanged()));
  layout->addMultiCellWidget(combo, 16,16, 2, 3);
  coAll3 =  combo;
  QWhatsThis::add( combo, strAll3 );

  layout->setRowStretch(16, 1);

  load();
}

void KActionsConfig::setComboText(QComboBox* combo, const char* text){
  int i;
  QString s = i18n(text); // no problem. These are already translated!
  for (i=0;i<combo->count();i++){
    if (s==combo->text(i)){
      combo->setCurrentItem(i);
      return;
    }
  }
}

const char*  KActionsConfig::functionTiDbl(int i)
{
  switch (i){
  case 0: return "Maximize"; break;
  case 1: return "Maximize (vertical only)"; break;
  case 2: return "Maximize (horizontal only)"; break;
  case 3: return "Shade"; break;
  case 4: return "Lower"; break;
  }
  return "";
}

const char*  KActionsConfig::functionTiAc(int i)
{
  switch (i){
  case 0: return "Raise"; break;
  case 1: return "Lower"; break;
  case 2: return "Operations menu"; break;
  case 3: return "Toggle raise and lower"; break;
  case 4: return "Nothing"; break;
  case 5: return "Shade"; break;
  }
  return "";
}
const char*  KActionsConfig::functionTiInAc(int i)
{
  switch (i){
  case 0: return "Activate and raise"; break;
  case 1: return "Activate and lower"; break;
  case 2: return "Activate"; break;
  case 3: return "Shade"; break;
  case 4: return ""; break;
  case 5: return ""; break;
  }
  return "";
}
const char*  KActionsConfig::functionWin(int i)
{
  switch (i){
  case 0: return "Activate, raise and pass click"; break;
  case 1: return "Activate and pass click"; break;
  case 2: return "Activate"; break;
  case 3: return "Activate and raise"; break;
  case 4: return ""; break;
  case 5: return ""; break;
  }
  return "";
}
const char*  KActionsConfig::functionAllKey(int i)
{
  switch (i){
  case 0: return "Meta"; break;
  case 1: return "Alt"; break;
  }
  return "";
}
const char*  KActionsConfig::functionAll(int i)
{
  switch (i){
  case 0: return "Move"; break;
  case 1: return "Toggle raise and lower"; break;
  case 2: return "Resize"; break;
  case 3: return "Raise"; break;
  case 4: return "Lower"; break;
  case 5: return "Nothing"; break;
  }
  return "";
}


void KActionsConfig::load()
{
  config->setGroup("Windows");
  setComboText(coTiDbl, config->readEntry("TitlebarDoubleClickCommand","Shade").ascii());

  config->setGroup( "MouseBindings");
  setComboText(coTiAct1,config->readEntry("CommandActiveTitlebar1","Raise").ascii());
  setComboText(coTiAct2,config->readEntry("CommandActiveTitlebar2","Lower").ascii());
  setComboText(coTiAct3,config->readEntry("CommandActiveTitlebar3","Operations menu").ascii());
  setComboText(coTiInAct1,config->readEntry("CommandInactiveTitlebar1","Activate and raise").ascii());
  setComboText(coTiInAct2,config->readEntry("CommandInactiveTitlebar2","Activate and lower").ascii());
  setComboText(coTiInAct3,config->readEntry("CommandInactiveTitlebar3","Activate").ascii());
  setComboText(coWin1,config->readEntry("CommandWindow1","Activate, raise and pass click").ascii());
  setComboText(coWin2,config->readEntry("CommandWindow2","Activate and pass click").ascii());
  setComboText(coWin3,config->readEntry("CommandWindow3","Activate and pass click").ascii());
  setComboText(coAllKey,config->readEntry("CommandAllKey","Alt").ascii());
  setComboText(coAll1,config->readEntry("CommandAll1","Move").ascii());
  setComboText(coAll2,config->readEntry("CommandAll2","Toggle raise and lower").ascii());
  setComboText(coAll3,config->readEntry("CommandAll3","Resize").ascii());
}

// many widgets connect to this slot
void KActionsConfig::slotChanged()
{
  emit changed(true);
}

void KActionsConfig::save()
{
  config->setGroup("Windows");
  config->writeEntry("TitlebarDoubleClickCommand", functionTiDbl( coTiDbl->currentItem() ) );

  config->setGroup("MouseBindings");
  config->writeEntry("CommandActiveTitlebar1", functionTiAc(coTiAct1->currentItem()));
  config->writeEntry("CommandActiveTitlebar2", functionTiAc(coTiAct2->currentItem()));
  config->writeEntry("CommandActiveTitlebar3", functionTiAc(coTiAct3->currentItem()));
  config->writeEntry("CommandInactiveTitlebar1", functionTiInAc(coTiInAct1->currentItem()));
  config->writeEntry("CommandInactiveTitlebar2", functionTiInAc(coTiInAct2->currentItem()));
  config->writeEntry("CommandInactiveTitlebar3", functionTiInAc(coTiInAct3->currentItem()));
  config->writeEntry("CommandWindow1", functionWin(coWin1->currentItem()));
  config->writeEntry("CommandWindow2", functionWin(coWin2->currentItem()));
  config->writeEntry("CommandWindow3", functionWin(coWin3->currentItem()));
  config->writeEntry("CommandAllKey", functionAllKey(coAllKey->currentItem()));
  config->writeEntry("CommandAll1", functionAll(coAll1->currentItem()));
  config->writeEntry("CommandAll2", functionAll(coAll2->currentItem()));
  config->writeEntry("CommandAll3", functionAll(coAll3->currentItem()));
}

void KActionsConfig::defaults()
{
  setComboText(coTiAct1,"Raise");
  setComboText(coTiAct2,"Lower");
  setComboText(coTiAct3,"Operations menu");
  setComboText(coTiInAct1,"Activate and raise");
  setComboText(coTiInAct2,"Activate and lower");
  setComboText(coTiInAct3,"Activate");
  setComboText(coWin1,"Activate, raise and pass click");
  setComboText(coWin2,"Activate and pass click");
  setComboText(coWin3,"Activate and pass click");
  setComboText(coAllKey, KAccel::keyboardHasMetaKey() ? "Meta" : "Alt");
  setComboText (coAll1,"Move");
  setComboText(coAll2,"Toggle raise and lower");
  setComboText(coAll3,"Resize");
}

#include "mouse.moc"
