/*
 * Copyright (c) 1998 Stefan Taferner <taferner@kde.org>
 */
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qcombobox.h>

#include <kapp.h>
#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>

#include "options.h"
#include "themecreator.h"
#include "global.h"
#include "groupdetails.h"

#include "assert.h"

//-----------------------------------------------------------------------------
Options::Options (QWidget * aParent, const char *aName, bool aInit)
  : OptionsInherited(aParent, aName)
{
  QLabel* lbl;
  QPushButton* btn;

  mGui = !aInit;
  if (!mGui)
  {
    return;
  }
  connect(theme, SIGNAL(changed()), SLOT(slotThemeChanged()));
  connect(theme, SIGNAL(apply()), SLOT(slotThemeApply()));

  mGrid = new QGridLayout(this, 16, 6, 10, 6);
  mGridRow = 0;

  mCbxOverwrite = new QCheckBox(i18n("Uninstall parts of previous theme"),
				this);
  mCbxOverwrite->setMinimumSize(mCbxOverwrite->sizeHint());
  mCbxOverwrite->setMaximumSize(32767, mCbxOverwrite->sizeHint().height()+5);
  mGrid->addMultiCellWidget(mCbxOverwrite, mGridRow, mGridRow, 0, 5);
  mGridRow++;

  lbl = new QLabel(i18n("Work on the following parts:"), this);
  lbl->setMinimumSize(lbl->sizeHint());
  mGrid->addMultiCellWidget(lbl, mGridRow, mGridRow, 0, 5);
  mGrid->setRowStretch(mGridRow, 3);
  mGridRow++;

  // The name of the entries and the name of the config groups must
  // be exactly the same. Otherwise the code in slotDetails()
  // will not be able to determine the translation for the
  // details dialog.
  mCbxColors = newLine("Colors", i18n("Colors"), &mStatColors);
  mCbxWallpapers = newLine("Display", i18n("Wallpapers"), &mStatWallpapers);
  mCbxSounds = newLine("Sounds", i18n("Sound effects"), &mStatSounds);
  mCbxIcons = newLine("Icons", i18n("Icons"), &mStatIcons);
  mCbxWM = newLine("Window Border", i18n("Window Decorations"), &mStatWM);
  mCbxPanel = newLine("Panel", i18n("Panel"), &mStatPanel);

  btn = new QPushButton(i18n("Clear"), this);
  btn->setFixedSize(btn->sizeHint());
  connect(btn, SIGNAL(pressed()), SLOT(slotClear()));
  mGrid->addWidget(btn, mGridRow, 0);

  btn = new QPushButton(i18n("Invert"), this);
  btn->setFixedSize(btn->sizeHint());
  connect(btn, SIGNAL(pressed()), SLOT(slotInvert()));
  mGrid->addWidget(btn, mGridRow, 1);

  mGridRow++;

  mGrid->setRowStretch(mGridRow, 1000);
  mGrid->setColStretch(0, 2);
  mGrid->setColStretch(1, 1);
  mGrid->setColStretch(2, 1);
  mGrid->setColStretch(3, 1);
  mGrid->setColStretch(4, 10);
  mGrid->activate();

  readConfig();
}


//-----------------------------------------------------------------------------
Options::~Options()
{
  writeConfig();
}


//-----------------------------------------------------------------------------
QCheckBox* Options::newLine(const char* aGroupName, const QString& aText,
			    QLabel** aStatusPtr) 
{
  QCheckBox* cbx = new QCheckBox(aText, this);
  QPushButton* btnDetails;
  QLabel* lbl;

  cbx->setMinimumSize(cbx->sizeHint());
  cbx->setMaximumSize(32767, cbx->sizeHint().height()+5);
  connect(cbx, SIGNAL(clicked()), this, SLOT(slotCbxClicked()));
  mGrid->addMultiCellWidget(cbx, mGridRow, mGridRow, 0, 2);

  lbl = new QLabel(i18n("unknown"), this);
  lbl->setMinimumSize(lbl->sizeHint());
  lbl->setMaximumSize(32767, lbl->sizeHint().height()+5);
  mGrid->addWidget(lbl, mGridRow, 3);
  *aStatusPtr = lbl;

  btnDetails = new QPushButton("...", this, aGroupName);
  btnDetails->setFixedSize(btnDetails->sizeHint() - QSize(6,2));
  connect(btnDetails, SIGNAL(clicked()), this, SLOT(slotDetails()));
  mGrid->addWidget(btnDetails, mGridRow, 4);
  btnDetails->hide();

  mGridRow++;
  return cbx;
}


//-----------------------------------------------------------------------------
void Options::load()
{
  kdDebug() << "Options::loadSettings() called" << endl;
}


//-----------------------------------------------------------------------------
void Options::save()
{
  theme->instColors = mCbxColors->isChecked();
  theme->instWallpapers = mCbxWallpapers->isChecked();
  theme->instSounds = mCbxSounds->isChecked();
  theme->instIcons = mCbxIcons->isChecked();
  theme->instWM = mCbxWM->isChecked();
  theme->instPanel = mCbxPanel->isChecked();
  theme->instOverwrite = !mCbxOverwrite->isChecked();
}


//-----------------------------------------------------------------------------
void Options::slotInvert()
{
  mCbxColors->setChecked(!mCbxColors->isChecked());
  mCbxWallpapers->setChecked(!mCbxWallpapers->isChecked());
  mCbxSounds->setChecked(!mCbxSounds->isChecked());
  mCbxIcons->setChecked(!mCbxIcons->isChecked());
  mCbxWM->setChecked(!mCbxWM->isChecked());
  mCbxPanel->setChecked(!mCbxPanel->isChecked());
  save();
}


//-----------------------------------------------------------------------------
void Options::slotClear()
{
  mCbxColors->setChecked(false);
  mCbxWallpapers->setChecked(false);
  mCbxSounds->setChecked(false);
  mCbxIcons->setChecked(false);
  mCbxWM->setChecked(false);
  mCbxPanel->setChecked(false);
  save();
}


//-----------------------------------------------------------------------------
void Options::slotDetails()
{
  const char * groupName = sender()->name();
  GroupDetails dlg(groupName);

  if (!groupName || !groupName[0])
  {
    kdWarning() << "Empty group name ?!";
    return;
  }

  dlg.setCaption(i18n(groupName));
  dlg.exec();
}


//-----------------------------------------------------------------------------
void Options::slotCbxClicked()
{
  save();
  emit changed(true);
}


//-----------------------------------------------------------------------------
void Options::slotThemeApply()
{
  save();
}


//-----------------------------------------------------------------------------
void Options::slotThemeChanged()
{
  kdDebug() << "Options::slotThemeChanged() called" << endl;
  updateStatus();
}


//-----------------------------------------------------------------------------
void Options::updateStatus(const char* aGroupName, QLabel* aLblStatus)
{
  QString statusStr;

  assert(aGroupName!=0);
  assert(aLblStatus!=NULL);

  if (theme->hasGroup(aGroupName, true))
    statusStr = i18n("available");
  else 
    statusStr = i18n("empty");

  aLblStatus->setText(statusStr);
  aLblStatus->setMinimumSize(aLblStatus->sizeHint());
}


//-----------------------------------------------------------------------------
void Options::updateStatus(void)
{
  updateStatus("Colors", mStatColors);
  updateStatus("Display", mStatWallpapers);
  updateStatus("Sounds", mStatSounds);
  updateStatus("Icons", mStatIcons);
  updateStatus("Window Border", mStatWM);
  updateStatus("Panel", mStatPanel);
}


//-----------------------------------------------------------------------------
void Options::writeConfig()
{
  KConfig* cfg = kapp->config();

  cfg->setGroup("Options");
  cfg->writeEntry("overwrite", !mCbxOverwrite->isChecked());
  cfg->writeEntry("colors", mCbxColors->isChecked());
  cfg->writeEntry("wallpapers", mCbxWallpapers->isChecked());
  cfg->writeEntry("sounds", mCbxSounds->isChecked());
  cfg->writeEntry("icons", mCbxIcons->isChecked());
  cfg->writeEntry("wm", mCbxWM->isChecked());
  cfg->writeEntry("panel", mCbxPanel->isChecked());
}


//-----------------------------------------------------------------------------
void Options::readConfig()
{
  KConfig* cfg = kapp->config();

  cfg->setGroup("Options");
  mCbxOverwrite->setChecked(!cfg->readBoolEntry("overwrite", false));
  mCbxColors->setChecked(cfg->readBoolEntry("colors", true));
  mCbxWallpapers->setChecked(cfg->readBoolEntry("wallpapers", true));
  mCbxSounds->setChecked(cfg->readBoolEntry("sounds", true));
  mCbxIcons->setChecked(cfg->readBoolEntry("icons", true));
  mCbxWM->setChecked(cfg->readBoolEntry("wm", true));
  mCbxPanel->setChecked(cfg->readBoolEntry("panel", true));
  save();
}

//-----------------------------------------------------------------------------
#include "options.moc"
