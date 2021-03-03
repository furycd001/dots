/* This file is part of the KDE Display Manager Configuration package
    Copyright (C) 1997 Thomas Tanghus (tanghus@earthling.net)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include <unistd.h>
#include <sys/types.h>


#include <qcombobox.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qwhatsthis.h>

#include <ksimpleconfig.h>
#include <klocale.h>
#include <kstddirs.h>
#include "kdm-font.moc"


extern KSimpleConfig *c;

KDMFontWidget::KDMFontWidget(QWidget *parent, const char *name)
  : KCModule(parent, name)
{
  QGroupBox *tGroup = new QGroupBox(i18n("Select fonts"), this);
  QGroupBox *bGroup = new QGroupBox(i18n("Example"), this);

  QWhatsThis::add( bGroup, i18n("Shows a preview of the selected font.") );

  QPushButton *fontbtn = new QPushButton(i18n("C&hange font..."), tGroup);
  connect(fontbtn, SIGNAL(clicked()), SLOT(slotGetFont()));
  fontbtn->setFixedSize(fontbtn->sizeHint());

  QWhatsThis::add( fontbtn, i18n("Click here to change the selected font.") );

  fontcombo = new QComboBox( FALSE, tGroup );
  connect(fontcombo, SIGNAL(highlighted(int)), SLOT(slotSetFont(int)));
  fontcombo->insertItem(i18n("Greeting"), 0);
  fontcombo->insertItem(i18n("Fail"), 1);
  fontcombo->insertItem(i18n("Standard"), 2);
  fontcombo->setFixedSize(fontcombo->sizeHint());

  QWhatsThis::add( fontcombo, i18n("Here you can select the font you want to change."
    " KDM knows three fonts: <ul><li><em>Greeting:</em> used to display KDM's greeting"
    " string (see \"Appearance\" tab)</li><li><em>Fail:</em> used to display a message"
    " when a person fails to login</li><li><em>Standard:</em> used for the rest of the text</li></ul>") );

  fontlabel = new QLabel( bGroup );
  fontlabel->setFrameStyle(QFrame::WinPanel|QFrame::Sunken);

  QBoxLayout *ml = new QVBoxLayout(this, 10);

  QBoxLayout *tLayout = new QVBoxLayout(tGroup, 10);
  QBoxLayout *bLayout = new QVBoxLayout(bGroup, 10);
  tLayout->addSpacing(tGroup->fontMetrics().height());
  bLayout->addSpacing(bGroup->fontMetrics().height());

  QBoxLayout *tLayout2 = new QHBoxLayout();
  tLayout->addLayout(tLayout2, 1);
  tLayout2->addWidget(fontbtn);
  tLayout2->addWidget(fontcombo);
  tLayout2->addStretch();

  bLayout->addWidget(fontlabel);

  ml->addWidget(tGroup);
  ml->addWidget(bGroup, 3);
  ml->addStretch(2);

  load();
  slotSetFont(0);

  if (getuid() != 0)
    {
      fontbtn->setEnabled(false);
      fontcombo->setEnabled(false);
    }
}


void KDMFontWidget::save()
{
  c->setGroup("X-*-Greeter");

  // write font
  c->writeEntry("StdFont", stdfont);
  c->writeEntry("GreetFont", greetfont);
  c->writeEntry("FailFont", failfont);
}


void KDMFontWidget::set_def()
{
  stdfont = QFont("helvetica", 12);
  failfont = QFont("helvetica", 12, QFont::Bold);
  greetfont = QFont("charter", 24);
}


void KDMFontWidget::load()
{
  set_def();

  c->setGroup("X-*-Greeter");

  // Read the fonts
  stdfont = c->readFontEntry("StdFont", &stdfont);
  failfont = c->readFontEntry("FailFont", &failfont);
  greetfont = c->readFontEntry("GreetFont", &greetfont);

  slotSetFont(fontcombo->currentItem());
}


void KDMFontWidget::defaults()
{
  set_def();
  slotSetFont(fontcombo->currentItem());
}


void KDMFontWidget::slotGetFont()
{
  QApplication::setOverrideCursor( waitCursor );

  KFontDialog *fontdlg = new KFontDialog(0, 0, TRUE, 0L);
  QApplication::restoreOverrideCursor( );
  fontdlg->getFont(tmpfont);
  switch (fontcombo->currentItem())
  {
    case 0:
      greetfont = tmpfont;
      break;
    case 1:
      failfont = tmpfont;
      break;
    case 2:
      stdfont = tmpfont;
      break;
  }
  fontlabel->setFont(tmpfont);
  //fontlabel->setFixedSize(fontlabel->sizeHint());
  delete fontdlg;

  emit KCModule::changed(true);
}


void KDMFontWidget::slotSetFont(int id)
{
  QApplication::setOverrideCursor( waitCursor );
  switch (id)
  {
    case 0:
      tmpfont = greetfont;
      fontlabel->setText(i18n("Greeting font"));
      break;
    case 1:
      tmpfont = failfont;
      fontlabel->setText(i18n("Fail font"));
      break;
    case 2:
      tmpfont = stdfont;
      fontlabel->setText(i18n("Standard font"));
      break;
  }
  fontlabel->setFont(tmpfont);
  //fontlabel->adjustSize();
  QApplication::restoreOverrideCursor( );
}

