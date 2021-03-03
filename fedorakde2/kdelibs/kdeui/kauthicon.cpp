/* This file is part of the KDE libraries
   Copyright (c) 1999 Preston Brown <pbrown@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
/*
 * KAuthIcon - an icon which shows whether privileges are in effect
 */

#include <unistd.h> // For getuid

#include <qlayout.h>
#include <qtimer.h>

#include <klocale.h>

#include "kauthicon.h"

/* XPM */
static const char * lock_xpm[] = {
"22 22 5 1",
"       c None",
".      c #808080",
"+      c #000000",
"@      c #FFFFFF",
"#      c #C0C0C0",
"                      ",
"                      ",
"                      ",
"                      ",
"        .+++.         ",
"        .@@@.+        ",
"      ..@+++@..       ",
"      +@+...+@+       ",
"      +@+.  +@+.      ",
"      +@+.  +@+.      ",
"     +++++++++++      ",
"     +#########+.     ",
"     +#.......#+.     ",
"     +#@@@@@@@#+.     ",
"     +#.......#+.     ",
"     +#########+.     ",
"     +++++++++++.     ",
"      ...........     ",
"                      ",
"                      ",
"                      ",
"                      "};

/* XPM */
static const char * openlock_xpm[] = {
"22 22 5 1",
"       c None",
".      c #808080",
"+      c #000000",
"@      c #FFFFFF",
"#      c #C0C0C0",
"                      ",
"                      ",
"        .+++.         ",
"        .@@@.+        ",
"      ..@+++@..       ",
"      +@+...+@+       ",
"      +@+.  +@+.      ",
"      +@+.  +@+.      ",
"      +++.  +@+.      ",
"       ...  +@+.      ",
"            +@+.      ",
"     +++++++++++      ",
"     +#########+.     ",
"     +#.......#+.     ",
"     +#@@@@@@@#+.     ",
"     +#.......#+.     ",
"     +#########+.     ",
"     +++++++++++.     ",
"      ...........     ",
"                      ",
"                      ",
"                      "};

KAuthIcon::KAuthIcon(QWidget *parent, const char *name)
  : QWidget(parent, name), lockPM(lock_xpm), openLockPM(openlock_xpm)
{
  lockText = i18n("Editing disabled");
  openLockText = i18n("Editing enabled");

  lockBox = new QLabel(this);
  lockBox->setFrameStyle(QFrame::WinPanel|QFrame::Raised);
  lockBox->setPixmap(lockPM);
  lockBox->setFixedSize(lockBox->sizeHint());

  lockLabel = new QLabel(this);
  lockLabel->setFrameStyle(QFrame::NoFrame);

  // set fixed size of this frame to whichever phrase is longer
  if (lockLabel->fontMetrics().boundingRect(lockText).width() >
      lockLabel->fontMetrics().boundingRect(openLockText).width())
    lockLabel->setText(lockText);
  else
    lockLabel->setText(openLockText);
  lockLabel->setAlignment(AlignCenter);
  lockLabel->setMinimumSize(lockLabel->sizeHint());
  lockLabel->setText(lockText);

  layout = new QHBoxLayout(this);

  layout->addWidget(lockBox, 0, AlignLeft|AlignVCenter);
  layout->addSpacing(5);
  layout->addWidget(lockLabel, 0, AlignRight|AlignVCenter);

  layout->activate();
  resize(sizeHint());
}

KAuthIcon::~KAuthIcon()
{
}


QSize KAuthIcon::sizeHint() const
{
  return layout->minimumSize();
}


/************************************************************************/

KRootPermsIcon::KRootPermsIcon(QWidget *parent, const char *name)
  : KAuthIcon(parent, name)
{
  updateStatus();
}


KRootPermsIcon::~KRootPermsIcon()
{
}

void KRootPermsIcon::updateStatus()
{
  bool newRoot;
  newRoot = (geteuid() == 0);
  lockBox->setPixmap(newRoot ? openLockPM : lockPM);
  lockLabel->setText(newRoot ? openLockText : lockText);
  update();
  if (root != newRoot) {
    root = newRoot;
    emit authChanged(newRoot);
  }
}

/************************************************************************/

KWritePermsIcon::KWritePermsIcon(QString fileName,
				 QWidget *parent, const char *name)
  : KAuthIcon(parent, name)
{
  fi.setFile(fileName);
  updateStatus();
}


KWritePermsIcon::~KWritePermsIcon()
{
}

void KWritePermsIcon::updateStatus()
{
  bool newwrite;
  newwrite = fi.isWritable();
  lockBox->setPixmap(newwrite ? openLockPM : lockPM);
  lockLabel->setText(newwrite ? openLockText : lockText);
  update();
  if (writable != newwrite) {
    writable = newwrite;
    emit authChanged(newwrite);
  }
}

#include "kauthicon.moc"
