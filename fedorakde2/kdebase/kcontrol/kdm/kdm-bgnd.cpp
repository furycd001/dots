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

#include <qframe.h>
#include <qlayout.h>
#include <qdragobject.h>
#include <qwhatsthis.h>

#include <kdebug.h>
#include <kpixmapeffect.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kglobal.h>
#include <kstddirs.h>

#include "kdropsite.h"
#include "kdm-bgnd.moc"

extern KSimpleConfig *c;

void KBGMonitor::setAllowDrop(bool a)
{
  if(a == allowdrop)
    return;
  allowdrop = a;

  QPainter p;
  p.begin( this );
  p.setRasterOp (NotROP);
  p.drawRect(0, 0, width(), height() );
  p.end();
}

// Destructor
KDMBackgroundWidget::~KDMBackgroundWidget()
{
  if(gui)
  {
    delete cGroup;
    delete wpGroup;
  }
}


KDMBackgroundWidget::KDMBackgroundWidget(QWidget *parent, const char *name, bool init)
  : QWidget(parent, name)
{
      gui = !init;
      loadSettings();
      if(gui)
        setupPage(parent);
}

void KDMBackgroundWidget::setupPage(QWidget *)
{
      QString wtstr;
      QLabel *label;
      QGroupBox *tGroup, *lGroup, *rGroup;
      QRadioButton *rb;
      QGridLayout *topLayout = new QGridLayout(this, 2, 2, 10);

      QPixmap p = BarIcon("monitor");

      tGroup = new QGroupBox( i18n("Preview"), this );

      label = new QLabel( tGroup );
      label->setAlignment( AlignCenter );
      label->setPixmap( p );

      monitor = new KBGMonitor( label );
      monitor->setGeometry( 23, 14, 151, 115 );
      QBoxLayout *tLayout = new QVBoxLayout(tGroup, 10, 10, "tLayout");
      tLayout->addSpacing(tGroup->fontMetrics().height()/2);
      tLayout->addWidget(label, 1, AlignCenter);

      QWhatsThis::add( monitor, i18n("This shows a preview of KDM's background.") );

      topLayout->addMultiCellWidget(tGroup, 0, 0, 0, 1);

      KDropSite *dropsite = new KDropSite( monitor );
      connect( dropsite, SIGNAL( dropAction( QDropEvent*) ),
        this, SLOT( slotQDrop( QDropEvent*) ) );

      connect( dropsite, SIGNAL( dragLeave( QDragLeaveEvent*) ),
        this, SLOT( slotQDragLeave( QDragLeaveEvent*) ) );

      connect( dropsite, SIGNAL( dragEnter( QDragEnterEvent*) ),
        this, SLOT( slotQDragEnter( QDragEnterEvent*) ) );

      lGroup = new QGroupBox( i18n("Color"), this );
      cGroup = new QButtonGroup();
      cGroup->setFrameStyle(QFrame::NoFrame);
      cGroup->setExclusive( TRUE );
      QBoxLayout *lLayout = new QVBoxLayout(lGroup, 10, 10);
      lLayout->addSpacing(lGroup->fontMetrics().height()/2);

      QRadioButton *crb1 = new QRadioButton( i18n("Solid Color"), lGroup );
      cGroup->insert( crb1, Plain );
      lLayout->addWidget(crb1);

      QRadioButton *crb2 = new QRadioButton( i18n("Horizontal Blend"), lGroup );
      cGroup->insert( crb2, Horizontal );
      lLayout->addWidget(crb2);

      QRadioButton *crb3 = new QRadioButton( i18n("Vertical Blend"), lGroup );
      cGroup->insert( crb3, Vertical );
      lLayout->addWidget(crb3);

      connect( cGroup, SIGNAL( clicked( int ) ),
	       SLOT( slotColorMode( int ) ) );

      colButton1 = new KColorButton( color1, lGroup );
      colButton1->setFixedSize( colButton1->sizeHint());
      connect( colButton1, SIGNAL( changed( const QColor & ) ),
               SLOT( slotSelectColor1( const QColor & ) ) );
      lLayout->addWidget(colButton1);

      colButton2 = new KColorButton( color1, lGroup );
      colButton2->setFixedSize( colButton2->sizeHint());
      connect( colButton2, SIGNAL( changed( const QColor & ) ),
               SLOT( slotSelectColor2( const QColor & ) ) );
      lLayout->addWidget(colButton2);

      topLayout->addWidget(lGroup, 1, 0);

      rGroup = new QGroupBox( i18n("Wallpaper"), this );
      QGridLayout *rLayout = new QGridLayout(rGroup, 6, 5, 10);
      rLayout->addRowSpacing(0, rGroup->fontMetrics().height()/2);
      rLayout->setRowStretch(5, 1);
      rLayout->setColStretch(3, 1);

      QStringList list = KGlobal::dirs()->findAllResources("wallpaper");
      if(!wallpaper.isEmpty())
        list.append( wallpaper );

      wpCombo = new QComboBox( FALSE, rGroup );

      for ( uint i = 0; i < list.count(); i++ )
      {
	wpCombo->insertItem( *list.at(i) );
	if ( wallpaper == *list.at(i) )
		wpCombo->setCurrentItem( i );
      }

      if ( wallpaper.length() > 0 && wpCombo->currentItem() == 0 )
      {
	wpCombo->insertItem( wallpaper );
	wpCombo->setCurrentItem( wpCombo->count()-1 );
      }

      wpCombo->setMinimumSize(1, 1);
      rLayout->addMultiCellWidget(wpCombo, 1, 1, 0, 3);

      rLayout->addWidget(wpCombo, 1, 0);
      connect( wpCombo, SIGNAL( activated( const QString& ) ),
		SLOT( slotWallpaper( const QString& )  )  );

      button = new QPushButton( i18n("Browse..."), rGroup );
      rLayout->addWidget(button, 1, 4);

      connect( button, SIGNAL( clicked() ), SLOT( slotBrowse() ) );

      wpGroup = new QButtonGroup();
      wpGroup->setFrameStyle(QFrame::NoFrame);
      wpGroup->setExclusive( TRUE );

      rb = new QRadioButton( i18n("None"), rGroup );
      wpGroup->insert( rb, NoPic );
      rLayout->addWidget(rb, 2, 0);
      rb = new QRadioButton( i18n("Tile"), rGroup );
      wpGroup->insert( rb, Tile );
      rLayout->addWidget(rb, 3, 0);
      rb = new QRadioButton( i18n("Center"), rGroup );
      wpGroup->insert( rb, Center );
      rLayout->addWidget(rb, 4, 0);

      rb = new QRadioButton( i18n("Scale"), rGroup );
      wpGroup->insert( rb, Scale );
      rLayout->addWidget(rb, 2, 1);
      rb = new QRadioButton( i18n("TopLeft"), rGroup );
      wpGroup->insert( rb, TopLeft );
      rLayout->addWidget(rb, 3, 1);
      rb = new QRadioButton( i18n("TopRight"), rGroup );
      wpGroup->insert( rb, TopRight );
      rLayout->addWidget(rb, 4, 1);

      rb = new QRadioButton( i18n("BottomLeft"), rGroup );
      wpGroup->insert( rb, BottomLeft );
      rLayout->addWidget(rb, 2, 2);
      rb = new QRadioButton( i18n("BottomRight"), rGroup );
      wpGroup->insert( rb, BottomRight );
      rLayout->addWidget(rb, 3, 2);
      rb = new QRadioButton( i18n("Fancy"), rGroup );
      wpGroup->insert( rb, Fancy );
      rLayout->addWidget(rb, 4, 2);

      connect( wpGroup, SIGNAL( clicked( int ) ),
	       SLOT( slotWallpaperMode( int ) ) );

      topLayout->addWidget(rGroup, 1, 1);

      lLayout->activate();
      rLayout->activate();
      topLayout->activate();

      showSettings();
}

void KDMBackgroundWidget::slotQDrop( QDropEvent *e )
{
  QStrList list;
  QString s;

  if(QUrlDrag::decode( e, list ) )
  {
    monitor->setAllowDrop(false);
    s = list.first(); // we only want the first
    //kdDebug() << "slotQDropEvent - " << s << endl;
    s = QUrlDrag::uriToLocalFile(s.ascii()); // a hack. should be improved
    if(!s.isEmpty())
      loadWallpaper(s);
  }
}

void KDMBackgroundWidget::slotQDragLeave( QDragLeaveEvent* )
{
  //kdDebug() << "Got QDragLeaveEvent!" << endl;
  monitor->setAllowDrop(false);
}

void KDMBackgroundWidget::slotQDragEnter( QDragEnterEvent *e )
{
  //kdDebug() << "Got QDragEnterEvent!" << endl;
  if( QUrlDrag::canDecode( e ) )
  {
    monitor->setAllowDrop(true);
    e->accept();
  }
  else
    e->ignore();
}

void KDMBackgroundWidget::slotSelectColor1(const QColor &col)
{
  color1 = col;
  slotWallpaperMode(wpMode);
}

void KDMBackgroundWidget::slotSelectColor2(const QColor &col)
{
  color2 = col;
  slotWallpaperMode(wpMode);
}

void KDMBackgroundWidget::slotBrowse()
{
    QString fileName() = KFileDialog::getOpenFileName( 0 );
    slotWallpaper( fileName() );
    if ( !fileName().isEmpty() && fileName() != wallpaper)
	{
	    wpCombo->insertItem( wallpaper );
	    wpCombo->setCurrentItem( wpCombo->count() - 1 );
	}
}

void KDMBackgroundWidget::setMonitor()
{
  QApplication::setOverrideCursor( waitCursor );

  if ( !wallpaper.isEmpty() )
  {
    //kdDebug() << "slotSelectColor - setting wallpaper" << endl;
    float sx = (float)monitor->width() / QApplication::desktop()->width();
    float sy = (float)monitor->height() / QApplication::desktop()->height();

    QWMatrix matrix;
    matrix.scale( sx, sy );
    monitor->setBackgroundPixmap( wpPixmap.xForm( matrix ) );
  }

  QApplication::restoreOverrideCursor();
}

void KDMBackgroundWidget::slotWallpaper( const QString& fileName() )
{
  if ( !fileName().isEmpty() )
      if ( loadWallpaper( fileName() ) == TRUE )
	  setMonitor();
}

void KDMBackgroundWidget::slotWallpaperMode( int m )
{
  wpMode = m;

  if(wpMode == NoPic)
  {
    wpCombo->setEnabled(false);
    button->setEnabled(false);
  }
  else
  {
    wpCombo->setEnabled(true);
    button->setEnabled(true);
  }

  if ( loadWallpaper( wallpaper ) == TRUE )
    setMonitor();
}

void KDMBackgroundWidget::slotColorMode( int m )
{
  colorMode = m;

  if(colorMode == Plain)
    colButton2->setEnabled(false);
  else
    colButton2->setEnabled(true);

  if ( loadWallpaper( wallpaper ) == TRUE )
    setMonitor();
}

// Attempts to load the specified wallpaper and creates a centred/scaled
// version if necessary.
// Note that centred pixmaps are placed on a full screen image of background
// color1, so if you want to save memory use a small tiled pixmap.
//
int KDMBackgroundWidget::loadWallpaper( const QString& name, bool useContext )
{
  static int context = 0;
  QString fileName();
  int rv = FALSE;
  KPixmap tmp;

  if ( useContext )
  {
	if ( context )
		QColor::destroyAllocContext( context );
		context = QColor::enterAllocContext();
  }

  fileName() = locate("wallpaper", name);

  if ( wpMode == NoPic || tmp.load( fileName() ) == TRUE )
  {
    wallpaper = fileName();
    int w = QApplication::desktop()->width();
    int h = QApplication::desktop()->height();

    wpPixmap.resize( w, h );

    if(wpMode != Scale && wpMode != Tile)
    {
      switch(colorMode)
      {
        default:
        case Plain:
          wpPixmap.fill( color1 );
          break;
        case Horizontal:
	  KPixmapEffect::gradient(wpPixmap, color1, color2,
				  KPixmapEffect::HorizontalGradient);
          break;
        case Vertical:
	  KPixmapEffect::gradient(wpPixmap, color1, color2,
				  KPixmapEffect::VerticalGradient);
          break;
      } // switch..
    } // if..

    switch ( wpMode )
    {
	case Tile:
	  wpPixmap = tmp;
	  break;
	case Center:
	  bitBlt( &wpPixmap, (w - tmp.width())/2,
			(h - tmp.height())/2, &tmp, 0, 0, tmp.width(), tmp.height() );
	  break;

	case Scale:
	{
	  float sx = (float)w / tmp.width();
	  float sy = (float)h / tmp.height();
	  QWMatrix matrix;
	  matrix.scale( sx, sy );
	  wpPixmap = tmp.xForm( matrix );
	}
	break;

	case TopLeft:
	  bitBlt( &wpPixmap, 0, 0, &tmp, 0, 0, tmp.width(), tmp.height() );
	  break;

	case TopRight:
	  bitBlt( &wpPixmap, w-tmp.width(), 0,
		&tmp, 0, 0, tmp.width(), tmp.height() );
	  break;

	case BottomLeft:
	  bitBlt( &wpPixmap, 0, h-tmp.height(),
		&tmp, 0, 0, tmp.width(), tmp.height() );
	  break;

	case BottomRight:
	  bitBlt( &wpPixmap, w-tmp.width(), h-tmp.height(),
		&tmp, 0, 0, tmp.width(), tmp.height() );
	  break;

	case NoPic:
	case Fancy:
	  break;
	default:
        {
	  wpPixmap = tmp;
        }
        break;
    }
    rv = TRUE;
  }
  else
  {
    kdDebug() << "KDMBackgroundWidget::loadWallpaper(): failed loading " << fileName() << endl;
    wallpaper = "";
  }

  if ( useContext )
    QColor::leaveAllocContext();

  return rv;
}

void KDMBackgroundWidget::showSettings()
{ 
  wpGroup->setButton(wpMode);
  cGroup->setButton(colorMode);

  colButton1->setColor( color1 );
  colButton2->setColor( color2 );

  if(colorMode == Plain)
    colButton2->setEnabled(false);
  else
    colButton2->setEnabled(true);

  wpCombo->setCurrentItem( 0 );
  for ( int i = 1; i < wpCombo->count(); i++ )
  {
    if ( wallpaper == wpCombo->text( i ) )
    {
      wpCombo->setCurrentItem( i );
      break;
    }
  }

  if ( wpMode == NoPic || !wallpaper.isEmpty() ) // && wpCombo->currentItem() == 0 )
  {
    loadWallpaper(wallpaper);
    wpCombo->insertItem( wallpaper );
    wpCombo->setCurrentItem( wpCombo->count()-1 );
  }

  if(wpMode == NoPic)
  {
    wpCombo->setEnabled(false);
    button->setEnabled(false);
  }
  else
  {
    wpCombo->setEnabled(true);
    button->setEnabled(true);
  }
/*
  ((QRadioButton *)wpGroup->find( NoPic ))->setChecked( wpMode == NoPic );
  ((QRadioButton *)wpGroup->find( Tile ))->setChecked( wpMode == Tile );
  ((QRadioButton *)wpGroup->find( Center ))->setChecked( wpMode == Center );
  ((QRadioButton *)wpGroup->find( Scale ))->setChecked( wpMode == Scale );
  ((QRadioButton *)wpGroup->find( TopLeft ))->setChecked( wpMode == TopLeft );
  ((QRadioButton *)wpGroup->find( TopRight ))->setChecked( wpMode == TopRight );
  ((QRadioButton *)wpGroup->find( BottomLeft ))->setChecked( wpMode == BottomLeft );
  ((QRadioButton *)wpGroup->find( BottomRight ))->setChecked( wpMode == BottomRight );
  ((QRadioButton *)wpGroup->find( Fancy ))->setChecked( wpMode == Fancy );

  ((QRadioButton *)cGroup->find( Plain ))->setChecked( colorMode == Plain );
  ((QRadioButton *)cGroup->find( Horizontal ))->setChecked( colorMode == Horizontal );
  ((QRadioButton *)cGroup->find( Vertical ))->setChecked( colorMode == Vertical );
*/
  setMonitor();
}

void KDMBackgroundWidget::applySettings()
{
  //kdDebug() << "KDMBackgroundWidget::applySettings()" << endl;

  c->setGroup("KDMDESKTOP");

  // write color
  c->writeEntry( "BackGroundColor1", color1 );
  c->writeEntry( "BackGroundColor2", color2 );

  switch( colorMode )
  {
    case Vertical:
      c->writeEntry( "BackGroundColorMode", "Vertical" );
      break;
    case Horizontal:
      c->writeEntry( "BackGroundColorMode", "Horizontal" );
      break;
    case Plain:
    default:
      c->writeEntry( "BackGroundColorMode", "Plain" );
      break;
  }

  // write wallpaper

  if(!wallpaper.isEmpty())
  {
    QFileInfo fi(wallpaper);
    if(fi.exists())
      c->writeEntry( "BackGroundPicture", wallpaper );
    else
      c->deleteEntry( "BackGroundPicture", false);
  }
  else
    c->deleteEntry( "BackGroundPicture", false );

  switch ( wpMode )
  {
    case NoPic:
      c->writeEntry( "BackGroundPictureMode", "None" );
      break;
    case Tile:
      c->writeEntry( "BackGroundPictureMode", "Tile" );
      break;
    case Center:
      c->writeEntry( "BackGroundPictureMode", "Center" );
      break;
    case Scale:
      c->writeEntry( "BackGroundPictureMode", "Scale" );
      break;
    case TopLeft:
      c->writeEntry( "BackGroundPictureMode", "TopLeft" );
      break;
    case TopRight:
      c->writeEntry( "BackGroundPictureMode", "TopRight" );
      break;
    case BottomLeft:
      c->writeEntry( "BackGroundPictureMode", "BottomLeft" );
      break;
    case BottomRight:
      c->writeEntry( "BackGroundPictureMode", "BottomRight" );
      break;
    case Fancy:
      c->writeEntry( "BackGroundPictureMode", "Fancy" );
    break;
  } // switch
}

void KDMBackgroundWidget::loadSettings()
{
    iconloader = KGlobal::iconLoader();
  QString str;
  
  c->setGroup("KDMDESKTOP");

  color1  = c->readColorEntry( "BackGroundColor1", &darkCyan);
  color2  = c->readColorEntry( "BackGroundColor2", &darkBlue);

  wallpaper = "";
  str = c->readEntry( "BackGroundPicture" );
  if ( !str.isEmpty() )
  {
      str = locate("wallpaper", str);
      if(!str.isEmpty())
	  {
	      KGlobal::dirs()->
		  addResourceDir("wallpaper",
				 KGlobal::dirs()->
				 findResourceDir("wallpaper", str));
	      wallpaper = str;
	  }
  }

  QString strmode = c->readEntry( "BackGroundColorMode", "Plain");
  if(strmode == "Plain")
    colorMode = Plain;
  else if(strmode == "Horizontal")
    colorMode = Horizontal;
  else if(strmode == "Vertical")
    colorMode = Vertical;
  else
    colorMode = Plain;


  strmode = c->readEntry( "BackGroundPictureMode", "Scale");
  if(strmode == "None")
    wpMode = NoPic;
  else if(strmode == "Tile")
    wpMode = Tile;
  else if(strmode == "Center")
    wpMode = Center;
  else if(strmode == "Scale")
    wpMode = Scale;
  else if(strmode == "TopLeft")
    wpMode = TopLeft;
  else if(strmode == "TopRight")
    wpMode = TopRight;
  else if(strmode == "BottomLeft")
    wpMode = BottomLeft;
  else if(strmode == "BottomRight")
    wpMode = BottomRight;
  else if(strmode == "Fancy")
    wpMode = Fancy;
  else
    wpMode = Tile;
}


