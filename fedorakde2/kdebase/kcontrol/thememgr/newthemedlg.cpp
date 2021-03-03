/*
 * newthemedlg.cpp
 *
 * Copyright (c) 1998 Stefan Taferner <taferner@kde.org>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
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

#include "config.h"

#include <unistd.h>

#include <qlabel.h>
#include <qimage.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qvbox.h>

#include <kapp.h>
#include <klocale.h>
#include <kwin.h>

#include "newthemedlg.h"
#include "themecreator.h"
#include "global.h"


//-----------------------------------------------------------------------------
NewThemeDlg::NewThemeDlg(QWidget *parent): 
  KDialogBase(parent, "newthemedlg", true,
	i18n("Create New Theme"), Ok|Cancel, Ok, true)
{
  QWidget *page = new QWidget(this);
  setMainWidget(page);

  mGrid = new QGridLayout(page, 8, 4, 0, spacingHint());
  mGridRow = 0;

  mEdtFilename = newLine(i18n("File Name:"),1);
  mEdtName = newLine(i18n("Detailed Name:"),1);
  mEdtAuthor = newLine(i18n("Author:"),1);
  mEdtEmail = newLine(i18n("Email:"),1);
  mEdtHomepage = newLine(i18n("Webpage:"),1);

  
//  QLabel *previewL = new QLabel(i18n("Preview:"), page);
//  mGrid->addWidget(previewL, 0, 2);
  mPreviewLabel = new QLabel(page);
  mPreviewLabel->setFrameStyle(QFrame::Panel|QFrame::Sunken);
  mPreviewLabel->setMinimumSize(QSize(160,120));
  mGrid->addMultiCellWidget(mPreviewLabel, 0, 3, 2, 3);

  //lukas: no op ... ?
  //QPushButton *browseBut = new QPushButton(i18n("&Browse..."), page);
  //mGrid->addWidget(browseBut, 4, 2);

  QPushButton *snapshotBut = new QPushButton(i18n("&Snapshot"), page);
  mGrid->addWidget(snapshotBut, 4, 3);
  connect(snapshotBut, SIGNAL(clicked()), this, SLOT(slotSnapshot()));

  mGrid->setRowStretch(mGridRow++, 10);

  setValues();

  mGrid->setColStretch(0, 0);
  mGrid->setColStretch(1, 1);
  mGrid->setColStretch(2, 0);
}


//-----------------------------------------------------------------------------
NewThemeDlg::~NewThemeDlg()
{
}


//-----------------------------------------------------------------------------
void NewThemeDlg::setValues(void)
{
  KConfig* cfg = kapp->config();
  cfg->setGroup("General");

  mEdtFilename->setText(i18n("NewTheme"));
  mEdtName->setText(i18n("A New Theme"));
  mEdtAuthor->setText(cfg->readEntry("author"));
  mEdtEmail->setText(cfg->readEntry("email"));
  mEdtHomepage->setText(cfg->readEntry("homepage"));
}


//-----------------------------------------------------------------------------
QLineEdit* NewThemeDlg::newLine(const QString& aLabelText, int cols)
{
  QLineEdit *edt = new QLineEdit(getMainWidget());
  edt->setMinimumSize(edt->sizeHint());
  mGrid->addMultiCellWidget(edt, mGridRow, mGridRow, 1, 0+cols);

  QLabel *lbl = new QLabel(aLabelText, getMainWidget());
  lbl->setMinimumSize(lbl->sizeHint());
  lbl->setBuddy(edt);
  mGrid->addWidget(lbl, mGridRow, 0);

  mGridRow++;
  return edt;
}

void NewThemeDlg::slotSnapshot()
{
  int desktop = KWin::currentDesktop();
  SnapshotDlg *dlg = new SnapshotDlg(this);
  int result = dlg->exec();
  delete dlg;
  if (result == SnapshotDlg::Rejected) 
     return;

  kapp->processEvents();
#ifdef HAVE_USLEEP
  usleep(100000);
  kapp->processEvents();
#endif  

  mPreview = QPixmap::grabWindow( qt_xrootwin()).convertToImage().smoothScale(320,240);
  QPixmap snapshot;
  snapshot.convertFromImage(mPreview.smoothScale(160,120));
  mPreviewLabel->setPixmap(snapshot);
  KWin::setCurrentDesktop(desktop);
  KWin::deIconifyWindow(winId(), false);  
}

SnapshotDlg::SnapshotDlg(QWidget *parent)
   : KDialogBase(parent, "snapshot", true, 
         i18n("Make Snapshot"), Cancel, Cancel, true)
{
  QVBox *page = makeVBoxMainWidget();
  mLabel = new QLabel(page);
  mSeconds = 5;
  connect(&mTimer, SIGNAL(timeout()), this, SLOT(slotCountdown()));
  slotCountdown();
}

void SnapshotDlg::slotCountdown()
{
  if (mSeconds == 0)
  {
     accept();
     return;
  }
  kapp->beep();
  mLabel->setText(i18n("Taking snapshot in %1 seconds!").arg(mSeconds--));
  mTimer.start(1000, true);
}

#include "newthemedlg.moc"
