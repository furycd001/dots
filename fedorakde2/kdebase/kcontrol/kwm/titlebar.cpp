/*
 * titlebar.cpp
 *
 * Copyright (c) 1997 Patrick Dowler dowler@morgul.fsh.uvic.ca
 *
 * Enhancements:
 * Copyright (c) 1999 Dirk A. Mueller <dmuell@gmx.net>
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

#include <qdir.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qwidgetstack.h>

#include <kapp.h>
#include <klocale.h>
#include <kconfig.h>
#include <kstddirs.h>
#include <kglobal.h>
#include <kicondialog.h>
#include <kiconloader.h>
#include <knuminput.h>
#include <kpixmapeffect.h>
#include <kmessagebox.h>

#include "titlebar.h"
#include "geom.h"

extern KConfig *config;

// config file keywords used by kwm
#define KWM_TITLEBARLOOK   "TitlebarLook"
#define KWM_TITLEANIMATION "TitleAnimation"
#define KWM_TITLEALIGN     "TitleAlignment"

//CT 02Dec1998 - weird hacks
#define KWM_TITLEFRAME     "TitleFrameShaded"
#define KWM_PIXMAP_TEXT    "PixmapUnderTitleText"

//  buttons 1 2 3 are on left, 4 5 6 on right
#define KWM_B1 "ButtonA"
#define KWM_B2 "ButtonB"
#define KWM_B3 "ButtonC"
#define KWM_B4 "ButtonF"
#define KWM_B5 "ButtonE"
#define KWM_B6 "ButtonD"

//CT 11feb98
#define KWM_DCTBACTION "TitlebarDoubleClickCommand"

KTitlebarButtons::~KTitlebarButtons ()
{
  // We need to delete ButtonBoxes only since all other
  // widgets will be deleted by layout
  delete minBox;
  delete maxBox;
  delete stickyBox;
  delete closeBox;
  delete menuBox;
}

KTitlebarButtons::KTitlebarButtons (QWidget * parent, const char *name)
  : KConfigWidget (parent, name)
{
  int i;
  QGridLayout *lay = new QGridLayout( this, 7, 4, 15);

  blankTitlebar = new TitlebarPreview(this, "blanktbar");
  lay->addMultiCellWidget (blankTitlebar, 0, 0, 0, 4);

  // button name labels
  lay->addWidget(new QLabel(i18n("Minimize"), this), 2, 0);
  lay->addWidget(new QLabel(i18n("Maximize"), this), 3, 0);
  lay->addWidget(new QLabel(i18n("Sticky"), this), 4, 0);
  lay->addWidget(new QLabel(i18n("Close"), this), 5, 0);
  lay->addWidget(new QLabel(i18n("Menu"), this), 6, 0);

  KGlobal::dirs()->addResourceType("kwm_pics", KStandardDirs::kde_default("data") + "kwm/pics/");

  // pixmap labels to show which button is which
  minP = new QLabel("", this);
  minP->setPixmap( QPixmap(locate("kwm_pics", "iconify.xpm")));
  lay->addWidget( minP, 2, 1);

  maxP = new QLabel("", this);
  maxP->setPixmap( QPixmap(locate("kwm_pics", "maximize.xpm")));
  lay->addWidget( maxP, 3, 1);

  stickyP = new QLabel("", this);
  stickyP->setPixmap( QPixmap(locate("kwm_pics", "pinup.xpm")));
  lay->addWidget( stickyP, 4, 1);

  closeP = new QLabel("", this);
  closeP->setPixmap( QPixmap(locate("kwm_pics", "close.xpm")));
  lay->addWidget( closeP, 5, 1);

  menuP = new QLabel("", this);
  menuP->setPixmap( QPixmap(locate("kwm_pics", "menu.xpm")));
  lay->addWidget( menuP, 6, 1);

  // left/right/off column labels
  left = new QLabel(i18n("Left"), this);
  lay->addWidget( left, 1, 2);

  right = new QLabel(i18n("Right"), this);
  lay->addWidget( right, 1, 3);

  off = new QLabel(i18n("Off"), this);
  lay->addWidget( off, 1, 4);

  // left/right/off radio buttons and groups
  minBox = new QButtonGroup();
  for (i=0; i<3; i++)
    {
      minBox->insert(minRB[i] = new QRadioButton(this));
      lay->addWidget(minRB[i], 2, 2+i);
      connect(minRB[i], SIGNAL(clicked()), this, SLOT(updatePreview()));
    }

  maxBox = new QButtonGroup();
  for (i=0; i<3; i++)
    {
      maxBox->insert(maxRB[i] = new QRadioButton(this));
      lay->addWidget(maxRB[i], 3, 2+i);
      connect(maxRB[i], SIGNAL(clicked()), this, SLOT(updatePreview()));
    }

  stickyBox = new QButtonGroup();
  for (i=0; i<3; i++)
    {
      stickyBox->insert(stickyRB[i] = new QRadioButton(this));
      lay->addWidget(stickyRB[i], 4, 2+i);
      connect(stickyRB[i], SIGNAL(clicked()), this, SLOT(updatePreview()));
    }

  closeBox = new QButtonGroup();
  for (i=0; i<3; i++)
    {
      closeBox->insert(closeRB[i] = new QRadioButton(this));
      lay->addWidget(closeRB[i], 5, 2+i);
      connect(closeRB[i], SIGNAL(clicked()), this, SLOT(updatePreview()));
    }

  menuBox = new QButtonGroup();
  for (i=0; i<3; i++)
    {
      menuBox->insert(menuRB[i] = new QRadioButton(this));
      lay->addWidget(menuRB[i], 6, 2+i);
      connect(menuRB[i], SIGNAL(clicked()), this, SLOT(updatePreview()));
    }

  lay->setRowStretch(7, 1);
  lay->activate();

  GetSettings();
  drawPreview(true);
}

void KTitlebarButtons::updatePreview()
{
  drawPreview(true);
}

void KTitlebarButtons::drawPreview(bool draw)
{
  int left = 0, right = 0;
  QPixmap *p = closeP->pixmap();

  for (int i=0; i<NUM_BUTTONS; i++)
    selectedFunc[i] = NOFUNC;

  blankTitlebar->removeAll();

  // place the highest priority pixmaps first

  // menu can only go at the edge: A or F
  if (menuRB[0]->isChecked())
    {
      if (draw)
	{
	  p = menuP->pixmap();
	  blankTitlebar->setA( p );
	}
      selectedFunc[0] = MENU;
      left++;
    }
  else if (menuRB[1]->isChecked())
    {
      if (draw)
	{
	  p = menuP->pixmap();
	  blankTitlebar->setF( p );
	}
      selectedFunc[5] = MENU;
      right++;
    }
  else
    {
      menuRB[2]->setChecked(true);
    }

  // close can go in A, B, E, or F
  if (closeRB[0]->isChecked())
    {
      if (draw) p = closeP->pixmap();
      if (left == 0)
	{
	  if (draw) blankTitlebar->setA( p );
	  selectedFunc[0] = CLOSE;
	}
      else
	{
	  if (draw) blankTitlebar->setB( p );
	  selectedFunc[1] = CLOSE;
	}
      left++;
    }
  else if (closeRB[1]->isChecked())
    {
      if (draw) p = closeP->pixmap();
      if (right == 0)
	{
	  if (draw) blankTitlebar->setF( p );
	  selectedFunc[5] = CLOSE;
	}
      else
	{
	  if (draw) blankTitlebar->setE( p );
	  selectedFunc[4] = CLOSE;
	}
      right++;
    }
  else
    {
      // make sure it is OFF
      closeRB[2]->setChecked(true);
    }


  // sticky can go anywhere but always fits
  if (stickyRB[0]->isChecked())
    {
      if (draw) p = stickyP->pixmap();
      if (left == 0)
	{
	  if (draw) blankTitlebar->setA( p );
	  selectedFunc[0] = STICKY;
	}
      else if (left == 1)
	{
	  if (draw) blankTitlebar->setB( p );
	  selectedFunc[1] = STICKY;
	}
      else
	{
	  if (draw) blankTitlebar->setC( p );
	  selectedFunc[2] = STICKY;
	}
      left++;
    }
  else if (stickyRB[1]->isChecked())
    {
      if (draw) p = stickyP->pixmap();
      if (right == 0)
	{
	  if (draw) blankTitlebar->setF( p );
	  selectedFunc[5] = STICKY;
	}
      else if (right == 1)
	{
	  if (draw) blankTitlebar->setE( p );
	  selectedFunc[4] = STICKY;
	}
      else
	{
	  if (draw) blankTitlebar->setD( p );
	  selectedFunc[3] = STICKY;
	}
      right++;
    }
  else
    {
      // make sure this func is OFF
      stickyRB[2]->setChecked(true);
    }

  // max may not fit is the selected side is full already
  if (maxRB[0]->isChecked())
    {
      if (draw) p = maxP->pixmap();
      if (left == 0)
	{
	  if (draw) blankTitlebar->setA( p );
	  selectedFunc[0] = MAXIMIZE;
	}
      else if (left == 1)
	{
	  if (draw) blankTitlebar->setB( p );
	  selectedFunc[1] = MAXIMIZE;
	}
      else if (left == 2)
	{
	  if (draw) blankTitlebar->setC( p );
	  selectedFunc[2] = MAXIMIZE;
	}
      else
	{

	  // can't place max on left
	  KMessageBox::information(this,
                               i18n("The left side of the titlebar "
                                    "is full... disabling the 'maximize' "
                                    "button\n"));
	  maxRB[0]->setChecked(false);
	  maxRB[2]->setChecked(true);
	  left--;
	}
      left++;
    }
  else if (maxRB[1]->isChecked())
    {
      if (draw) p = maxP->pixmap();
      if (right == 0)
	{
	 if (draw)  blankTitlebar->setF( p );
	  selectedFunc[5] = MAXIMIZE;
	}
      else if (right == 1)
	{
	  if (draw) blankTitlebar->setE( p );
	  selectedFunc[4] = MAXIMIZE;
	}
      else if (right == 2)
	{
	  if (draw) blankTitlebar->setD( p );
	  selectedFunc[3] = MAXIMIZE;
	}
      else
	{
	  // can't place max on right
	  KMessageBox::information(this,
                               i18n("The right side of the titlebar "
                                    "is full... disabling the 'maximize' "
                                    "button\n"));
	  maxRB[1]->setChecked(false);
	  maxRB[2]->setChecked(true);
	  right--;
	}
      right++;
    }
  else
    {
      // make sure this func is OFF
      maxRB[2]->setChecked(true);
    }

  // min may not fit is the selected side is full already
  if (minRB[0]->isChecked())
    {
      if (draw) p = minP->pixmap();
      if (left == 0)
	{
	  if (draw) blankTitlebar->setA( p );
	  selectedFunc[0] = ICONIFY;
	}
      else if (left == 1)
	{
	  if (draw) blankTitlebar->setB( p );
	  selectedFunc[1] = ICONIFY;
	}
      else if (left == 2)
	{
	  if (draw) blankTitlebar->setC( p );
	  selectedFunc[2] = ICONIFY;
	}
      else
	{
	  // left side is full
	  KMessageBox::information(this,
                               i18n("The left side of the titlebar "
                                    "is full... disabling the 'minimize' "
                                    "button\n"));
	  minRB[0]->setChecked(false);
	  minRB[2]->setChecked(true);
	  left--;
	}
      left++;
    }
  else if (minRB[1]->isChecked())
    {
      if (draw) p = minP->pixmap();
      if (right == 0)
	{
	  if (draw) blankTitlebar->setF( p );
	  selectedFunc[5] = ICONIFY;
	}
      else if (right == 1)
	{
	  if (draw) blankTitlebar->setE( p );
	  selectedFunc[4] = ICONIFY;
	}
      else if (right == 2)
	{
	  if (draw) blankTitlebar->setD( p );
	  selectedFunc[3] = ICONIFY;
	}
      else
	{
	  // can't place min on right
	  KMessageBox::information(this,
                               i18n("The right side of the titlebar "
                                    "is full... disabling the 'minimize' "
                                    "button\n"));
	  minRB[1]->setChecked(false);
	  minRB[2]->setChecked(true);
	  right--;
	}
      right++;
    }
  else
    {
      // make sure it is OFF
      minRB[2]->setChecked(true);
    }
}

int KTitlebarButtons::getFunc(int button)
{
  return selectedFunc[button];
}

void KTitlebarButtons::setButton(int button, int func)
{
  // if button < 3, the func button goes on the left side
  // otherwise, the func button goes on the right side

  switch (func)
    {
    case ICONIFY:
      if (button < 3)
      {
	minRB[0]->setChecked(true);
        minRB[1]->setChecked(false);
      }
      else
      {
	minRB[1]->setChecked(true);
        minRB[0]->setChecked(false);
      }
      break;
    case MAXIMIZE:
      if (button < 3)
      {
	maxRB[0]->setChecked(true);
        maxRB[1]->setChecked(false);
      }
      else
      {
	maxRB[1]->setChecked(true);
        maxRB[0]->setChecked(false);
      }
      break;
    case STICKY:
      if (button < 3)
      {
	stickyRB[0]->setChecked(true);
        stickyRB[1]->setChecked(false);
      }
      else
      {
	stickyRB[1]->setChecked(true);
        stickyRB[0]->setChecked(false);
      }
      break;
    case CLOSE:
      if (button < 3)
      {
	closeRB[0]->setChecked(true);
        closeRB[1]->setChecked(false);
      }
      else
      {
	closeRB[1]->setChecked(true);
        closeRB[0]->setChecked(false);
      }
      break;
    case MENU:
      if (button < 3)
      {
	menuRB[0]->setChecked(true);
        menuRB[1]->setChecked(false);
      }
      else
      {
	menuRB[1]->setChecked(true);
        menuRB[0]->setChecked(false);
      }
      break;
    }
}

void KTitlebarButtons::setState()
{
  drawPreview(false);
}

void KTitlebarButtons::getStringValue(int b, QString *str)
{
  switch (b)
    {
    case MENU:
      *str = "Menu";
      break;
    case STICKY:
      *str = "Sticky";
      break;
    case CLOSE:
      *str = "Close";
      break;
    case MAXIMIZE:
      *str = "Maximize";
      break;
    case ICONIFY:
      *str = "Iconify";
      break;
    case NOFUNC:
      *str = "Off";
      break;
    }
}

void KTitlebarButtons::SaveSettings( void )
{
  config->setGroup( "Buttons");

  QString str;
  int b;

  b = getFunc(0);
  getStringValue(b, &str);
  config->writeEntry(KWM_B1, str);

  b = getFunc(1);
  getStringValue(b, &str);
  config->writeEntry(KWM_B2, str);

  b = getFunc(2);
  getStringValue(b, &str);
  config->writeEntry(KWM_B3, str);

  b = getFunc(3);
  getStringValue(b, &str);
  config->writeEntry(KWM_B4, str);

  b = getFunc(4);
  getStringValue(b, &str);
  config->writeEntry(KWM_B5, str);

  b = getFunc(5);
  getStringValue(b, &str);
  config->writeEntry(KWM_B6, str);

  config->sync();

}

int KTitlebarButtons::buttonFunc(QString *key)
{
  int ret = NOFUNC;

  if( *key == "Off" )
    ret = NOFUNC;
  else if( *key == "Maximize" )
    ret = MAXIMIZE;
  else if( *key == "Iconify" )
    ret = ICONIFY;
  else if( *key == "Close" )
    ret = CLOSE;
  else if( *key == "Sticky" )
    ret = STICKY;
  else if (*key == "Menu" )
    ret = MENU;

  return ret;
}

void KTitlebarButtons::GetSettings( void )
{
  QString key;

  config->setGroup( "Buttons");
  int ABUTTON=0, BBUTTON=0, CBUTTON=0, DBUTTON=0, EBUTTON=0, FBUTTON=0;

  key = config->readEntry(KWM_B1);
  ABUTTON = buttonFunc(&key);

  key = config->readEntry(KWM_B2);
  BBUTTON = buttonFunc(&key);

  key = config->readEntry(KWM_B3);
  CBUTTON = buttonFunc(&key);

  key = config->readEntry(KWM_B4);
  DBUTTON = buttonFunc(&key);

  key = config->readEntry(KWM_B5);
  EBUTTON = buttonFunc(&key);

  key = config->readEntry(KWM_B6);
  FBUTTON = buttonFunc(&key);

  // clear all buttons (for reloading!)
  minRB[0]->setChecked(false);
  minRB[1]->setChecked(false);
  maxRB[0]->setChecked(false);
  maxRB[1]->setChecked(false);
  stickyRB[0]->setChecked(false);
  stickyRB[1]->setChecked(false);
  closeRB[0]->setChecked(false);
  closeRB[1]->setChecked(false);
  menuRB[0]->setChecked(false);
  menuRB[1]->setChecked(false);

  setButton(0, ABUTTON);
  setButton(1, BBUTTON);
  setButton(2, CBUTTON);
  setButton(3, DBUTTON);
  setButton(4, EBUTTON);
  setButton(5, FBUTTON);
  setState();
}

void KTitlebarButtons::loadSettings()
{
  GetSettings();
  drawPreview(true);
}

void KTitlebarButtons::applySettings()
{
  SaveSettings();
}

TitlebarPreview::~TitlebarPreview( )
{
  // All buttons are delete by layout
}

TitlebarPreview::TitlebarPreview( QWidget *parent, const char *name )
        : QFrame( parent, name )
{
  setFrameStyle(QFrame::WinPanel | QFrame::Raised );

  QHBoxLayout *hlay = new QHBoxLayout(this, 4, 1);

  hlay->addWidget(a = new QLabel(this, "a"));
  a->hide();
  hlay->addWidget(b = new QLabel(this, "b"));
  b->hide();
  hlay->addWidget(c = new QLabel(this, "c"));
  c->hide();

  QWidget *tmp = new QWidget(this);
  tmp->setBackgroundColor( QColor( 0, 10, 160 ) );
  hlay->addWidget(tmp, 1);

  hlay->addWidget(d = new QLabel(this, "d"));
  d->hide();
  hlay->addWidget(e = new QLabel(this, "e"));
  e->hide();
  hlay->addWidget(f = new QLabel(this, "f"));
  f->hide();

  hlay->activate();
}

void TitlebarPreview::setA( QPixmap *pm )
{
  a->setPixmap( *pm );
  a->show();
}
void TitlebarPreview::setB( QPixmap *pm )
{
  b->setPixmap( *pm );
  b->show();
}
void TitlebarPreview::setC( QPixmap *pm )
{
  c->setPixmap( *pm );
  c->show();
}
void TitlebarPreview::setD( QPixmap *pm )
{
  d->setPixmap( *pm );
  d->show();
}
void TitlebarPreview::setE( QPixmap *pm )
{
  e->setPixmap( *pm );
  e->show();
}
void TitlebarPreview::setF( QPixmap *pm )
{
  f->setPixmap( *pm );
  f->show();
}
void TitlebarPreview::removeAll( void )
{
  a->hide();
  b->hide();
  c->hide();
  d->hide();
  e->hide();
  f->hide();
}

// appearance dialog
KTitlebarAppearance::~KTitlebarAppearance ()
{
}

KTitlebarAppearance::KTitlebarAppearance (QWidget * parent, const char *name)
  : KConfigWidget (parent, name)
{
  // titlebar shading style

  QGridLayout *lay = new QGridLayout(this,4,2,5);
  lay->setRowStretch(0,0);
  lay->setRowStretch(1,1);
  lay->setRowStretch(2,0);
  lay->setRowStretch(3,0);

  lay->setColStretch(0,1);
  lay->setColStretch(1,1);

  //CT 06Nov1998 - title alignment GUI config
  alignBox = new QButtonGroup (i18n("Title Alignment"), this);

  QGridLayout *pixLay = new QGridLayout(alignBox,2,3,15,5);

  leftAlign = new QRadioButton(i18n("Left"),alignBox);
  pixLay->addWidget(leftAlign,1,0);

  midAlign = new QRadioButton(i18n("Middle"),alignBox);
  pixLay->addWidget(midAlign,1,1);

  rightAlign = new QRadioButton(i18n("Right"),alignBox);
  pixLay->addWidget(rightAlign,1,2);

  pixLay->activate();

  lay->addMultiCellWidget(alignBox,0,0,0,1);

  //CT 02Dec1998 - foul changes for some weird options
  appearBox = new QGroupBox(i18n("Appearance"),
				 this);

  QBoxLayout *appearLay = new QVBoxLayout (appearBox,10,5);
  appearLay->addSpacing(10);

  titlebarBox = new QButtonGroup(appearBox);
  titlebarBox->setFrameStyle(QFrame::NoFrame);

  QBoxLayout *pushLay = new QVBoxLayout (titlebarBox,10,5);

  bShaded = new QRadioButton(i18n("Gradient"), titlebarBox);
  pushLay->addWidget(bShaded);

  connect(bShaded, SIGNAL(clicked()), this, SLOT(titlebarChanged()));

  plain = new QRadioButton(i18n("Plain"),
			   titlebarBox);
  pushLay->addWidget(plain);

  connect(plain, SIGNAL(clicked()), this, SLOT(titlebarChanged()));

  pixmap = new QRadioButton(i18n("Pixmap"), titlebarBox);
  pushLay->addWidget(pixmap);

  connect(pixmap, SIGNAL(clicked()), this, SLOT(titlebarChanged()));


  appearLay->addWidget(titlebarBox);

  cbFrame = new QCheckBox(i18n("Active title has shaded frame"),
	                   appearBox);
  appearLay->addWidget(cbFrame);


  lay->addWidget(appearBox,1,0);

  optOpts =  new QWidgetStack( this, "optOpts");

  lay->addWidget(optOpts, 1, 1);

  // the first page - options for pixmap titlebars
  pixmapBox    = new QGroupBox(i18n("Pixmap"), optOpts);

  pixLay = new QGridLayout(pixmapBox,7,2,10,5);

  pbPixmapActive = new QPushButton(pixmapBox);
  pixLay->addWidget(pbPixmapActive,2,1);

  connect(pbPixmapActive, SIGNAL(clicked()), this, SLOT(activePressed()));

  pbPixmapInactive = new QPushButton(pixmapBox);
  pixLay->addWidget(pbPixmapInactive,5,1);

  connect(pbPixmapInactive, SIGNAL(clicked()), this, SLOT(inactivePressed()));

  lPixmapActive = new QLabel(pbPixmapActive, i18n("Active pixmap:"), pixmapBox);
  pixLay->addMultiCellWidget(lPixmapActive,1,1,0,1);

  lPixmapInactive = new QLabel(pbPixmapInactive, i18n("Inactive pixmap:"), pixmapBox);
  pixLay->addMultiCellWidget(lPixmapInactive,4,4,0,1);

  cbPixedText = new QCheckBox(i18n("No pixmap under text"),pixmapBox);
  pixLay->addMultiCellWidget(cbPixedText,6,6,0,1);

  // second page - options for gradients

  gradBox = new QGroupBox(i18n("Gradient"), optOpts);

  QBoxLayout *gradLay = new QVBoxLayout(gradBox, 10);
  gradLay->addSpacing(10);

  gradientTypes = new QListBox(gradBox);

  gradientTypes->insertItem(i18n("Vertical"));
  gradientTypes->insertItem(i18n("Horizontal"));
  gradientTypes->insertItem(i18n("Diagonal"));
  gradientTypes->insertItem(i18n("CrossDiagonal"));
  gradientTypes->insertItem(i18n("Pyramid"));
  gradientTypes->insertItem(i18n("Rectangle"));
  gradientTypes->insertItem(i18n("PipeCross"));
  gradientTypes->insertItem(i18n("Elliptic"));

  gradientTypes->setMultiSelection(false);

  connect(gradientTypes, SIGNAL(highlighted(const QString &)),
	  this, SLOT(setGradient(const QString &)));

  gradLay->addWidget(gradientTypes);

  gradLay->addSpacing(10);

  gradPreview = new QFrame ( gradBox);
  gradPreview->setFrameStyle(QFrame::Panel | QFrame::Raised);
  gradPreview->setFixedHeight(24);
  gradPreview->setFixedWidth(150);

  setGradient(i18n("Horizontal")); // build the gradient
  gradPreview->setBackgroundPixmap(gradPix);

  gradLay->addWidget(gradPreview);

  //CT 11feb98 - Title double click
  titlebarDblClickBox = new QGroupBox(i18n("Mouse action"),
				       this);

  pixLay = new QGridLayout(titlebarDblClickBox,2,2,10,5);
  pixLay->addRowSpacing(0,10);
  pixLay->setColStretch(0,0);
  pixLay->setColStretch(1,1);

  lDblClick = new QLabel(i18n("Left Button double click does:"),
			 titlebarDblClickBox);
  pixLay->addWidget(lDblClick,1,0);

  // I commented some stuff out, since it does not make sense (Matthias 23okt98)
  dblClickCombo = new QComboBox(false, titlebarDblClickBox);
  dblClickCombo->insertItem(i18n("(Un)Maximize"),DCTB_MAXIMIZE);
  dblClickCombo->insertItem(i18n("(Un)Shade"),DCTB_SHADE);
  dblClickCombo->insertItem(i18n("Iconify"),DCTB_ICONIFY);
  dblClickCombo->insertItem(i18n("(Un)Sticky"),DCTB_STICKY);
  dblClickCombo->insertItem(i18n("Close"),DCTB_CLOSE);
  dblClickCombo->setCurrentItem( DCTB_MAXIMIZE );

  pixLay->addWidget(dblClickCombo,1,1);

  pixLay->activate();

  lay->addMultiCellWidget(titlebarDblClickBox,2,2,0,1);

  titleAnim = new KIntNumInput(10, this);
  titleAnim->setRange(0, 100, 10, true);
  titleAnim->setLabel(i18n("Title animation"));
  titleAnim->setSuffix(i18n("ms"));

  lay->addMultiCellWidget(titleAnim,3,0, 1, 1);

  lay->activate();
  GetSettings();

  gradientTypes->setCurrentItem((int) gradient);
}

//CT 02Dec1998
bool KTitlebarAppearance::getFramedTitle() {
  return cbFrame->isChecked();
}

void KTitlebarAppearance::setFramedTitle(bool a) {
  cbFrame->setChecked(a);
}

bool KTitlebarAppearance::getPixedText() {
  return !cbPixedText->isChecked();
}

void KTitlebarAppearance::setPixedText(bool a) {
  cbPixedText->setChecked(!a);
}


//CT 06Nov1998
int KTitlebarAppearance::getAlign() {
  if (midAlign->isChecked()) return AT_MIDDLE;
  else if (rightAlign->isChecked()) return AT_RIGHT;
  else return AT_LEFT;
}

void KTitlebarAppearance::setAlign(int a) {
  if (a == AT_LEFT)
    leftAlign->setChecked(true);
  if (a == AT_MIDDLE)
    midAlign->setChecked(true);
  if (a == AT_RIGHT)
    rightAlign->setChecked(true);
}
//CT

int KTitlebarAppearance::getTitlebar()
{
  if (bShaded->isChecked()) {
    return TITLEBAR_SHADED;
  }
  else if (pixmap->isChecked())
      return TITLEBAR_PIXMAP;
  else
      return TITLEBAR_PLAIN;
}

void KTitlebarAppearance::setTitlebar(int tb)
{
  if (tb == TITLEBAR_PIXMAP)
    {
      bShaded->setChecked(false);
      plain->setChecked(false);
      pixmap->setChecked(true);
      optOpts->raiseWidget(pixmapBox);
      pixmapBox->setEnabled(true);
      lPixmapActive->setEnabled(true);
      pbPixmapActive->setEnabled(true);
      lPixmapInactive->setEnabled(true);
      pbPixmapInactive->setEnabled(true);
      cbPixedText->setEnabled(true);
      return;
    }
  if (tb == TITLEBAR_SHADED)
    {
      bShaded->setChecked(true);
      plain->setChecked(false);
      pixmap->setChecked(false);
      optOpts->raiseWidget(gradBox);
      pixmapBox->setEnabled(false);
      lPixmapActive->setEnabled(false);
      pbPixmapActive->setEnabled(false);
      lPixmapInactive->setEnabled(false);
      pbPixmapInactive->setEnabled(false);
      cbPixedText->setEnabled(false);
      return;
    }
  if (tb == TITLEBAR_PLAIN)
    {
      bShaded->setChecked(false);
      plain->setChecked(true);
      pixmap->setChecked(false);
      optOpts->raiseWidget(pixmapBox);
      pixmapBox->setEnabled(false);
      lPixmapActive->setEnabled(false);
      pbPixmapActive->setEnabled(false);
      lPixmapInactive->setEnabled(false);
      pbPixmapInactive->setEnabled(false);
      cbPixedText->setEnabled(false);
      return;
    }
}

//CT 11feb98 action on double click on titlebar
int KTitlebarAppearance::getDCTBAction()
{
  return dblClickCombo->currentItem();
}

void KTitlebarAppearance::setDCTBAction(int action)
{
  dblClickCombo->setCurrentItem(action);
}

void KTitlebarAppearance::SaveSettings( void )
{

  config->setGroup( "General" );

  int t = getAlign();
  if (t == AT_MIDDLE) config->writeEntry(KWM_TITLEALIGN, "middle");
  else if (t == AT_RIGHT) config->writeEntry(KWM_TITLEALIGN, "right");
  else config->writeEntry(KWM_TITLEALIGN, "left");

  //CT 02Dec1998 - optional shaded frame on titlebar
  config->writeEntry(KWM_TITLEFRAME, getFramedTitle()?"yes":"no");

  //CT 02Dec1998 - optional pixmap under the title text
  config->writeEntry(KWM_PIXMAP_TEXT, getPixedText()?"yes":"no");

  t = getTitlebar();
  if (t == TITLEBAR_SHADED)
    {
      if (gradient == VERT)
	config->writeEntry(KWM_TITLEBARLOOK, "shadedVertical");
      else if (gradient == HORIZ)
	config->writeEntry(KWM_TITLEBARLOOK, "shadedHorizontal");
      else if (gradient == DIAG)
	config->writeEntry(KWM_TITLEBARLOOK, "shadedDiagonal");
      else if (gradient == CROSSDIAG)
	config->writeEntry(KWM_TITLEBARLOOK, "shadedCrossDiagonal");
      else if (gradient == PYRAM)
	config->writeEntry(KWM_TITLEBARLOOK, "shadedPyramid");
      else if (gradient == RECT)
	config->writeEntry(KWM_TITLEBARLOOK, "shadedRectangle");
      else if (gradient == PIPE)
	config->writeEntry(KWM_TITLEBARLOOK, "shadedPipeCross");
      else if (gradient == ELLIP)
	config->writeEntry(KWM_TITLEBARLOOK, "shadedElliptic");
    }
  else if (t == TITLEBAR_PIXMAP)
    config->writeEntry(KWM_TITLEBARLOOK, "pixmap");
  else
    config->writeEntry(KWM_TITLEBARLOOK, "plain");

  //CT 18Oct1998 - save the pixmaps
  if (t == TITLEBAR_PIXMAP ) {
    QString kwmpicsdir = locateLocal("data", "kwm/");

    //first, a backup
    sPixmapActive   = "oldactivetitlebar.xpm";
    sPixmapInactive = "oldinactivetitlebar.xpm";

    if (!pixmapActiveOld.isNull()) {
      QFile( sPixmapActive ).remove();
      pixmapActiveOld.save(kwmpicsdir+'/'+sPixmapActive,"XPM");
    }

    if (!pixmapInactiveOld.isNull()) {
      QFile( sPixmapInactive ).remove();
      pixmapInactiveOld.save(kwmpicsdir+'/'+sPixmapInactive,"XPM");
    }

    //then, the save
    sPixmapActive   = "activetitlebar.xpm";
    sPixmapInactive = "inactivetitlebar.xpm";

    bool a_saved = true, i_saved = true;
    if (!pixmapActive.isNull()) {
      QFile( sPixmapActive ).remove();
      a_saved = pixmapActive.save(kwmpicsdir+'/'+sPixmapActive,"XPM");
    }

    if (!pixmapInactive.isNull()) {
      QFile( sPixmapInactive ).remove();
      i_saved = pixmapInactive.save(kwmpicsdir+'/'+sPixmapInactive,"XPM");
    }

    //and a little check
    if ( !( a_saved && i_saved ) ) {
      KMessageBox::sorry(this,
                         i18n("There was an error while saving\n"
                              "the titlebar pixmaps! Please check permissions."));
    }
  }

  config->writeEntry(KWM_TITLEANIMATION, titleAnim->value());

  int a = getDCTBAction();
  switch (a) {
  case DCTB_MAXIMIZE:
    config->writeEntry(KWM_DCTBACTION, "Maximize");
    break;
  case DCTB_ICONIFY:
    config->writeEntry(KWM_DCTBACTION, "Iconify");
    break;
  case DCTB_CLOSE:
    config->writeEntry(KWM_DCTBACTION, "Close");
    break;
  case DCTB_STICKY:
    config->writeEntry(KWM_DCTBACTION, "Sticky");
    break;
  case DCTB_SHADE:
    config->writeEntry(KWM_DCTBACTION, "Shade");
    break;
  //CT should never get here
  default:     config->writeEntry(KWM_DCTBACTION, "Maximize");
  }

  config->sync();

}

void KTitlebarAppearance::setGradient(const QString & grad_name)
{

  gradPix.resize(gradPreview->width(), gradPreview->height());

  if (grad_name == i18n("Vertical"))
  {
    gradient = VERT;
    KPixmapEffect::gradient(gradPix, cTitle, cBlend,
			    KPixmapEffect::VerticalGradient);
  }
  else if (grad_name == i18n("Horizontal"))
  {
    gradient = HORIZ;
    KPixmapEffect::gradient(gradPix, cTitle, cBlend,
			    KPixmapEffect::HorizontalGradient);
  }
  else if (grad_name == i18n("Diagonal"))
  {
    gradient = DIAG;
    KPixmapEffect::gradient(gradPix, cTitle, cBlend,
			    KPixmapEffect::DiagonalGradient);
  }
  else if (grad_name == i18n("CrossDiagonal"))
  {
    gradient = CROSSDIAG;
    KPixmapEffect::gradient(gradPix, cTitle, cBlend,
			    KPixmapEffect::CrossDiagonalGradient);
  }
  else if (grad_name == i18n("Pyramid"))
  {
    gradient = PYRAM;
    KPixmapEffect::gradient(gradPix, cTitle, cBlend,
			    KPixmapEffect::PyramidGradient);
  }
  else if (grad_name == i18n("Rectangle"))
  {
    gradient = RECT;
    KPixmapEffect::gradient(gradPix, cTitle, cBlend,
			    KPixmapEffect::RectangleGradient);
  }
  else if (grad_name == i18n("PipeCross"))
  {
    gradient = PIPE;
    KPixmapEffect::gradient(gradPix, cTitle, cBlend,
			    KPixmapEffect::PipeCrossGradient);
  }
  else if (grad_name == i18n("Elliptic"))
  {
    gradient = ELLIP;
    KPixmapEffect::gradient(gradPix, cTitle, cBlend,
			    KPixmapEffect::EllipticGradient);
  }

  gradPreview->setBackgroundPixmap(gradPix);
}

void KTitlebarAppearance::GetSettings( void )
{
  QString key;

  config->setGroup( "General" );

  //CT 06Nov1998
  key = config->readEntry(KWM_TITLEALIGN);
  if( key == "middle" ) setAlign(AT_MIDDLE);
  else if ( key == "right" ) setAlign(AT_RIGHT);
  else setAlign(AT_LEFT);

  //CT 02Dec1998 - optional shaded frame on titlebar
  setFramedTitle(config->readBoolEntry(KWM_TITLEFRAME, true));

  //CT 02Dec1998 - optional pixmap under the title text
  setPixedText(config->readBoolEntry(KWM_PIXMAP_TEXT, true));

  key = config->readEntry(KWM_TITLEBARLOOK);
  if( key.find("shaded") != -1)
    {
      setTitlebar(TITLEBAR_SHADED);
      if (key== "shadedVertical")
	setGradient(i18n("Vertical"));
      else if (key== "shadedHorizontal")
	setGradient(i18n("Horizontal"));
      else if (key== "shadedDiagonal")
	setGradient(i18n("Diagonal"));
      else if (key== "shadedCrossDiagonal")
	setGradient(i18n("CrossDiagonal"));
      else if (key== "shadedPyramid")
	setGradient(i18n("Pyramid"));
      else if (key== "shadedRectangle")
	setGradient(i18n("Rectangle"));
      else if (key== "shadedPipeCross")
	setGradient(i18n("PipeCross"));
      else if (key== "shadedElliptic")
	setGradient(i18n("Elliptic"));
    }
  else if( key == "pixmap")
    setTitlebar(TITLEBAR_PIXMAP);
  else
    setTitlebar(TITLEBAR_PLAIN);

  sPixmapActive = "activetitlebar.xpm";
  sPixmapInactive = "inactivetitlebar.xpm";
  pbPixmapActive->setPixmap(pixmapActiveOld =
			      BarIcon( sPixmapActive ));
  pbPixmapInactive->setPixmap(pixmapInactiveOld =
			      BarIcon( sPixmapInactive ));

  titleAnim->setValue(config->readNumEntry(KWM_TITLEANIMATION,0));

  key = config->readEntry(KWM_DCTBACTION);
  if (key == "Maximize") setDCTBAction(DCTB_MAXIMIZE);
  else if (key == "Iconify") setDCTBAction(DCTB_ICONIFY);
  else if (key == "Close") setDCTBAction(DCTB_CLOSE);
  else if (key == "Sticky") setDCTBAction(DCTB_STICKY);
  else if (key == "Shade") setDCTBAction(DCTB_SHADE);
  else setDCTBAction(DCTB_MAXIMIZE);

  // load titleBar colors
  config->setGroup("WM");

  cTitle = config->readColorEntry("activeBackground", &darkBlue);
  cBlend = config->readColorEntry("activeBlend", &black);
}

void KTitlebarAppearance::loadSettings()
{
  GetSettings();
}

void KTitlebarAppearance::applySettings()
{
  SaveSettings();
}


void KTitlebarAppearance::titlebarChanged()
{
  setTitlebar(getTitlebar());
}


void KTitlebarAppearance::activePressed()
{
  KIconDialog dlg(iconLoader, this);
  QString name ;//CT= sPixmapActive;
  //CT  QPixmap map;

  pixmapActive = dlg.selectIcon(name, "*");
  if (!name.isEmpty())
    {
      //CT      sPixmapActive = name;
      pbPixmapActive->setPixmap(pixmapActive);
    }
}


void KTitlebarAppearance::inactivePressed()
{
  KIconDialog dlg(iconLoader, this);
  QString name ;//CT= sPixmapInactive;
  //CT  QPixmap map;

  pixmapInactive = dlg.selectIcon(name, "*");
  if (!name.isEmpty())
    {
      //CT      sPixmapInactive = name;
      pbPixmapInactive->setPixmap(pixmapInactive);
    }
}


#include "titlebar.moc"


