/****************************************************************************
** $Id: qt/examples/showimg/showimg.cpp   2.3.2   edited 2001-02-14 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "showimg.h"
#include "imagetexteditor.h"
#include <qmenubar.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qkeycode.h>
#include <qapplication.h>
#include <qclipboard.h>


/*
  In the constructor, we just pass the standard parameters on to
  QWidget.

  The menu uses a single slot to simplify the process of adding
  more items to the options menu.
*/
ImageViewer::ImageViewer( QWidget *parent, const char *name, int wFlags )
    : QWidget( parent, name, wFlags ),
      conversion_flags( PreferDither ),
      filename( 0 ),
      helpmsg( 0 )
{
    pickx = -1;
    picky = -1;
    clickx = -1;
    clicky = -1;
    alloc_context = 0;

    menubar = new QMenuBar(this);
    menubar->setSeparator( QMenuBar::InWindowsStyle );

    QStrList fmt = QImage::outputFormats();
    saveimage = new QPopupMenu();
    savepixmap = new QPopupMenu();
    for (const char* f = fmt.first(); f; f = fmt.next()) {
	saveimage->insertItem( f );
	savepixmap->insertItem( f );
    }
    connect( saveimage, SIGNAL(activated(int)), this, SLOT(saveImage(int)) );
    connect( savepixmap, SIGNAL(activated(int)), this, SLOT(savePixmap(int)) );

    file = new QPopupMenu();
    menubar->insertItem( "&File", file );
    file->insertItem( "&New window", this,  SLOT(newWindow()), CTRL+Key_N );
    file->insertItem( "&Open...", this,  SLOT(openFile()), CTRL+Key_O );
    si = file->insertItem( "Save image", saveimage );
    sp = file->insertItem( "Save pixmap", savepixmap );
    file->insertSeparator();
    file->insertItem( "E&xit", qApp,  SLOT(quit()), CTRL+Key_Q );

    edit =  new QPopupMenu();
    menubar->insertItem( "&Edit", edit );
    edit->insertItem("&Copy", this, SLOT(copy()), CTRL+Key_C);
    edit->insertItem("&Paste", this, SLOT(paste()), CTRL+Key_V);
    edit->insertSeparator();
    edit->insertItem("&Horizontal flip", this, SLOT(hFlip()), ALT+Key_H);
    edit->insertItem("&Vertical flip", this, SLOT(vFlip()), ALT+Key_V);
    edit->insertItem("&Rotate 180", this, SLOT(rot180()), ALT+Key_R);
    edit->insertSeparator();
    edit->insertItem("&Text...", this, SLOT(editText()));
    edit->insertSeparator();
    t1 = edit->insertItem( "Convert to &1 bit", this, SLOT(to1Bit()) );
    t8 = edit->insertItem( "Convert to &8 bit", this, SLOT(to8Bit()) );
    t32 = edit->insertItem( "Convert to &32 bit", this, SLOT(to32Bit()) );

    options =  new QPopupMenu();
    menubar->insertItem( "&Options", options );
    ac = options->insertItem( "AutoColor" );
    co = options->insertItem( "ColorOnly" );
    mo = options->insertItem( "MonoOnly" );
    options->insertSeparator();
    fd = options->insertItem( "DiffuseDither" );
    bd = options->insertItem( "OrderedDither" );
    td = options->insertItem( "ThresholdDither" );
    options->insertSeparator();
    ta = options->insertItem( "ThresholdAlphaDither" );
    ba = options->insertItem( "OrderedAlphaDither" );
    fa = options->insertItem( "DiffuseAlphaDither" );
    options->insertSeparator();
    ad = options->insertItem( "PreferDither" );
    dd = options->insertItem( "AvoidDither" );
    options->insertSeparator();
    ns = options->insertItem( "No scaling" );
    us = options->insertItem( "Normal scaling" );
    ss = options->insertItem( "Smooth scaling" );
    options->insertSeparator();
    cc = options->insertItem( "Use color context" );
    if ( QApplication::colorSpec() == QApplication::ManyColor )
	options->setItemEnabled( cc, FALSE );
    options->setCheckable( TRUE );
    setMenuItemFlags();

    menubar->insertSeparator();

    QPopupMenu* help = new QPopupMenu();
    menubar->insertItem( "&Help", help );
    help->insertItem( "Help!", this, SLOT(giveHelp()), CTRL+Key_H );

    connect( options, SIGNAL(activated(int)), this, SLOT(doOption(int)) );

    status = new QLabel(this);
    status->setFrameStyle( QFrame::WinPanel | QFrame::Sunken );
    status->setFixedHeight( fontMetrics().height() + 4 );

    setMouseTracking( TRUE );
}

ImageViewer::~ImageViewer()
{
    if ( alloc_context )
	QColor::destroyAllocContext( alloc_context );
    if ( other == this )
	other = 0;
}

/*
  This function modifies the conversion_flags when an options menu item
  is selected, then ensures all menu items are up to date, and reconverts
  the image if possibly necessary.
*/
void ImageViewer::doOption(int item)
{
    if ( item == cc ) {
	// Toggle
	bool newbool = !options->isItemChecked(item);
	options->setItemChecked(item, newbool);
	// And reconvert...
	reconvertImage();
	repaint(image.hasAlphaBuffer());	// show image in widget
	return;
    }

    if ( options->isItemChecked( item ) ) return; // They are all radio buttons

    if ( item == ns || item == us || item == ss ) {
	options->setItemChecked( ns, item == ns );
	options->setItemChecked( us, item == us );
	options->setItemChecked( ss, item == ss );
	reconvertImage();
	repaint(image.hasAlphaBuffer() || unscaled() );	// show image in widget
	return;
    }
    
    int ocf = conversion_flags;

    if ( item == ac ) {
	conversion_flags = conversion_flags & ~ColorMode_Mask | AutoColor;
    } else if ( item == co ) {
	conversion_flags = conversion_flags & ~ColorMode_Mask | ColorOnly;
    } else if ( item == mo ) {
	conversion_flags = conversion_flags & ~ColorMode_Mask | MonoOnly;
    } else if ( item == fd ) {
	conversion_flags = conversion_flags & ~Dither_Mask | DiffuseDither;
    } else if ( item == bd ) {
	conversion_flags = conversion_flags & ~Dither_Mask | OrderedDither;
    } else if ( item == td ) {
	conversion_flags = conversion_flags & ~Dither_Mask | ThresholdDither;
    } else if ( item == ta ) {
	conversion_flags = conversion_flags & ~AlphaDither_Mask | ThresholdAlphaDither;
    } else if ( item == fa ) {
	conversion_flags = conversion_flags & ~AlphaDither_Mask | DiffuseAlphaDither;
    } else if ( item == ba ) {
	conversion_flags = conversion_flags & ~AlphaDither_Mask | OrderedAlphaDither;
    } else if ( item == ad ) {
	conversion_flags = conversion_flags & ~DitherMode_Mask | PreferDither;
    } else if ( item == dd ) {
	conversion_flags = conversion_flags & ~DitherMode_Mask | AvoidDither;
    }

    if ( ocf != conversion_flags ) {
	setMenuItemFlags();
	// And reconvert...
	reconvertImage();
	repaint(image.hasAlphaBuffer());	// show image in widget
    }
}

/*
  Set the options menu to reflect the conversion_flags value.
*/
void ImageViewer::setMenuItemFlags()
{
    // File
    bool valid_image = pm.size() != QSize( 0, 0 );
    file->setItemEnabled( si, valid_image );
    file->setItemEnabled( sp, valid_image );

    // Edit
    edit->setItemEnabled( t1, image.depth() != 1 );
    edit->setItemEnabled( t8, image.depth() != 8 );
    edit->setItemEnabled( t32, image.depth() != 32 );

    // Options
    bool may_need_color_dithering =
	    !valid_image
	|| image.depth() == 32 && QPixmap::defaultDepth() < 24;
    bool may_need_dithering = may_need_color_dithering
	 || image.depth() > 1 && options->isItemChecked(mo)
	 || image.depth() > 1 && QPixmap::defaultDepth() == 1;
    bool has_alpha_mask = !valid_image || image.hasAlphaBuffer();

    options->setItemEnabled( fd, may_need_dithering );
    options->setItemEnabled( bd, may_need_dithering );
    options->setItemEnabled( td, may_need_dithering );

    options->setItemEnabled( ta, has_alpha_mask );
    options->setItemEnabled( fa, has_alpha_mask );
    options->setItemEnabled( ba, has_alpha_mask );

    options->setItemEnabled( ad, may_need_color_dithering );
    options->setItemEnabled( dd, may_need_color_dithering );

    options->setItemChecked( ac, (conversion_flags & ColorMode_Mask) == AutoColor );
    options->setItemChecked( co, (conversion_flags & ColorMode_Mask) == ColorOnly );
    options->setItemChecked( mo, (conversion_flags & ColorMode_Mask) == MonoOnly );
    options->setItemChecked( fd, (conversion_flags & Dither_Mask) == DiffuseDither );
    options->setItemChecked( bd, (conversion_flags & Dither_Mask) == OrderedDither );
    options->setItemChecked( td, (conversion_flags & Dither_Mask) == ThresholdDither );
    options->setItemChecked( ta, (conversion_flags & AlphaDither_Mask) == ThresholdAlphaDither );
    options->setItemChecked( fa, (conversion_flags & AlphaDither_Mask) == DiffuseAlphaDither );
    options->setItemChecked( ba, (conversion_flags & AlphaDither_Mask) == OrderedAlphaDither );
    options->setItemChecked( ad, (conversion_flags & DitherMode_Mask) == PreferDither );
    options->setItemChecked( dd, (conversion_flags & DitherMode_Mask) == AvoidDither );
}

void ImageViewer::updateStatus()
{
    if ( pm.size() == QSize( 0, 0 ) ) {
	if ( filename )
	    status->setText("Could not load image");
	else
	    status->setText("No image - select Open from File menu.");
    } else {
	QString message, moremsg;
	message.sprintf("%dx%d", image.width(), image.height());
	if ( pm.size() != pmScaled.size() ) {
	    moremsg.sprintf(" [%dx%d]", pmScaled.width(),
		pmScaled.height());
	    message += moremsg;
	}
	moremsg.sprintf(", %d bits ", image.depth());
	message += moremsg;
	if (image.valid(pickx,picky)) {
	    moremsg.sprintf("(%d,%d)=#%0*x ",
			  pickx, picky,
			  image.hasAlphaBuffer() ? 8 : 6,
			  image.pixel(pickx,picky));
	    message += moremsg;
	}
	if ( image.numColors() > 0 ) {
	    if (image.valid(pickx,picky)) {
		moremsg.sprintf(", %d/%d colors", image.pixelIndex(pickx,picky),
		    image.numColors());
	    } else {
		moremsg.sprintf(", %d colors", image.numColors());
	    }
	    message += moremsg;
	}
	if ( image.hasAlphaBuffer() ) {
	    if ( image.depth() == 8 ) {
		int i;
		bool alpha[256];
		int nalpha=0;

		for (i=0; i<256; i++)
		    alpha[i] = FALSE;

		for (i=0; i<image.numColors(); i++) {
		    int alevel = image.color(i) >> 24;
		    if (!alpha[alevel]) {
			alpha[alevel] = TRUE;
			nalpha++;
		    }
		}
		moremsg.sprintf(", %d alpha levels", nalpha);
	    } else {
		// Too many pixels to bother counting.
		moremsg = ", 8-bit alpha channel";
	    }
	    message += moremsg;
	}
	status->setText(message);
    }
}

/*
  This function saves the image.
*/
void ImageViewer::saveImage( int item )
{
    const char* fmt = saveimage->text(item);
    QString savefilename = QFileDialog::getSaveFileName(QString::null, QString::null,
					this, filename);
    if ( !savefilename.isEmpty() )
	if ( !image.save( savefilename, fmt ) )
	    QMessageBox::warning( this, "Save failed", "Error saving file" );
}

/*
  This function saves the converted image.
*/
void ImageViewer::savePixmap( int item )
{
    const char* fmt = savepixmap->text(item);
    QString savefilename = QFileDialog::getSaveFileName(QString::null,
					QString::null, this, filename);
    if ( !savefilename.isEmpty() )
	if ( !pmScaled.save( savefilename, fmt ) )
	    QMessageBox::warning( this, "Save failed", "Error saving file" );
}


void ImageViewer::newWindow()
{
    ImageViewer* that = new ImageViewer(0, "new window", WDestructiveClose);
    that->options->setItemChecked( that->cc, useColorContext() );
    that->show();
}

/*
  This function is the slot for processing the Open menu item.
*/
void ImageViewer::openFile()
{
    QString newfilename = QFileDialog::getOpenFileName();
    if ( !newfilename.isEmpty() ) {
	loadImage( newfilename ) ;
	repaint();			// show image in widget
    }
}

/*
  This function loads an image from a file and resizes the widget to
  exactly fit the image size. If the file was not found or the image
  format was unknown it will resize the widget to fit the errorText
  message (see above) displayed in the current font.

  Returns TRUE if the image was successfully loaded.
*/

bool ImageViewer::loadImage( const char *fileName )
{
    filename = fileName;
    bool ok = FALSE;
    if ( filename ) {
	QApplication::setOverrideCursor( waitCursor ); // this might take time
	ok = image.load(filename, 0);
	pickx = -1;
	clickx = -1;
	if ( ok )
	    ok = reconvertImage();
	if ( ok ) {
	    setCaption( filename );			// set window caption
	    int w = pm.width();
	    int h = pm.height();

	    const int reasonable_width = 128;
	    if ( w < reasonable_width ) {
		// Integer scale up to something reasonable
		int multiply = ( reasonable_width + w - 1 ) / w;
		w *= multiply;
		h *= multiply;
	    }

	    h += menubar->heightForWidth(w) + status->height();
	    resize( w, h );				// we resize to fit image
	} else {
	    pm.resize(0,0);				// couldn't load image
	    update();
	}
	QApplication::restoreOverrideCursor();	// restore original cursor
    }
    updateStatus();
    setMenuItemFlags();
    return ok;
}

bool ImageViewer::reconvertImage()
{
    bool success = FALSE;

    if ( image.isNull() ) return FALSE;

    if ( alloc_context ) {
	QColor::destroyAllocContext( alloc_context );
	alloc_context = 0;
    }
    if ( useColorContext() ) {
	alloc_context = QColor::enterAllocContext();
	// Clear the image to hide flickering palette
	QPainter painter(this);
	painter.eraseRect(0, menubar->heightForWidth( width() ), width(), height());
    }

    QApplication::setOverrideCursor( waitCursor ); // this might take time
    if ( pm.convertFromImage(image, conversion_flags) )
    {
	pmScaled = QPixmap();
	scale();
	resize( width(), height() );
	success = TRUE;				// load successful
    } else {
	pm.resize(0,0);				// couldn't load image
    }
    updateStatus();
    setMenuItemFlags();
    QApplication::restoreOverrideCursor();	// restore original cursor

    if ( useColorContext() )
	QColor::leaveAllocContext();

    return success;				// TRUE if loaded OK
}

bool ImageViewer::unscaled() const
{
    return options->isItemChecked(ns);
}

bool ImageViewer::smooth() const
{
    return options->isItemChecked(ss);
}

bool ImageViewer::useColorContext() const
{
    return options->isItemChecked(cc);
}

/*
  This functions scales the pixmap in the member variable "pm" to fit the
  widget size and  puts the resulting pixmap in the member variable "pmScaled".
*/

void ImageViewer::scale()
{
    int h = height() - menubar->heightForWidth( width() ) - status->height();

    if ( image.isNull() ) return;

    QApplication::setOverrideCursor( waitCursor ); // this might take time
    if ( width() == pm.width() && h == pm.height() || unscaled() )
    {						// no need to scale if widget
	pmScaled = pm;				// size equals pixmap size
    } else {
	if (smooth()) {
	    pmScaled.convertFromImage(image.smoothScale(width(), h),
		conversion_flags);
	} else {
	    QWMatrix m;				// transformation matrix
	    m.scale(((double)width())/pm.width(),// define scale factors
		    ((double)h)/pm.height());
	    pmScaled = pm.xForm( m );		// create scaled pixmap
	}
    }
    QApplication::restoreOverrideCursor();	// restore original cursor
}

/*
  The resize event handler, if a valid pixmap was loaded it will call
  scale() to fit the pixmap to the new widget size.
*/

void ImageViewer::resizeEvent( QResizeEvent * )
{
    status->setGeometry(0, height() - status->height(),
		        width(), status->height());

    if ( pm.size() == QSize( 0, 0 ) )		// we couldn't load the image
	return;

    int h = height() - menubar->heightForWidth( width() ) - status->height();
    if ( width() != pmScaled.width() || h != pmScaled.height())
    {						// if new size,
	scale();				// scale pmScaled to window
	updateStatus();
    }
    if ( image.hasAlphaBuffer() )
	erase();
}

bool ImageViewer::convertEvent( QMouseEvent* e, int& x, int& y)
{
    if ( pm.size() != QSize( 0, 0 ) ) {
	int h = height() - menubar->heightForWidth( width() ) - status->height();
	int nx = e->x() * image.width() / width();
	int ny = (e->y()-menubar->heightForWidth( width() )) * image.height() / h;
	if (nx != x || ny != y ) {
	    x = nx;
	    y = ny;
	    updateStatus();
	    return TRUE;
	}
    }
    return FALSE;
}

void ImageViewer::mousePressEvent( QMouseEvent *e )
{
    may_be_other = convertEvent(e, clickx, clicky);
}

void ImageViewer::mouseReleaseEvent( QMouseEvent * )
{
    if ( may_be_other )
	other = this;
}

/*
  Record the pixel position of interest.
*/
void ImageViewer::mouseMoveEvent( QMouseEvent *e )
{
    if (convertEvent(e,pickx,picky)) {
	updateStatus();
	if ((e->state()&LeftButton)) {
	    may_be_other = FALSE;
	    if ( clickx >= 0 && other) {
		copyFrom(other);
	    }
	}
    }
}

/*
  Draws the portion of the scaled pixmap that needs to be updated or prints
  an error message if no legal pixmap has been loaded.
*/

void ImageViewer::paintEvent( QPaintEvent *e )
{
    if ( pm.size() != QSize( 0, 0 ) ) {		// is an image loaded?
	QPainter painter(this);
	painter.setClipRect(e->rect());
	painter.drawPixmap(0, menubar->heightForWidth( width() ), pmScaled);
    }
}


/*
  Explain anything that might be confusing.
*/
void ImageViewer::giveHelp()
{
    if (!helpmsg) {
	QString helptext =
	    "<b>Usage:</b> <tt>showimg [-m] <i>filename ...</i></tt>"
	    "<blockquote>"
		"<tt>-m</tt> - use <i>ManyColor</i> color spec"
	    "</blockquote>"
	    "<p>Supported input formats:"
	    "<blockquote>";
	helptext += QImage::inputFormatList().join(", ");
	helptext += "</blockquote>";

	helpmsg = new QMessageBox( "Help", helptext,
	    QMessageBox::Information, QMessageBox::Ok, 0, 0, 0, 0, FALSE );
    }
    helpmsg->show();
    helpmsg->raise();
}

void ImageViewer::copyFrom(ImageViewer* s)
{
    if ( clickx >= 0 ) {
	int dx = clickx;
	int dy = clicky;
	int sx = s->clickx;
	int sy = s->clicky;
	int sw = QABS(clickx - pickx)+1;
	int sh = QABS(clicky - picky)+1;
	if ( clickx > pickx ) {
	    dx = pickx;
	    sx -= sw-1;
	}
	if ( clicky > picky ) {
	    dy = picky;
	    sy -= sh-1;
	}
	bitBlt( &image, dx, dy, &s->image, sx, sy, sw, sh );
	reconvertImage();
	repaint( image.hasAlphaBuffer() );
    }
}
ImageViewer* ImageViewer::other = 0;

void ImageViewer::hFlip()
{
    setImage(image.mirror(TRUE,FALSE));
}

void ImageViewer::vFlip()
{
    setImage(image.mirror(FALSE,TRUE));
}

void ImageViewer::rot180()
{
    setImage(image.mirror(TRUE,TRUE));
}

void ImageViewer::copy()
{
#ifndef QT_NO_MIMECLIPBOARD
    QApplication::clipboard()->setImage(image); // Less information loss
#endif
}

void ImageViewer::paste()
{
#ifndef QT_NO_MIMECLIPBOARD
    QImage p = QApplication::clipboard()->image();

    if ( !image.isNull() ) {
	filename = "pasted";
	setImage(p);
    }
#endif
}

void ImageViewer::setImage(const QImage& newimage)
{
    image = newimage;

    pickx = -1;
    clickx = -1;
    setCaption( filename );			// set window caption
    int w = image.width();
    int h = image.height();

    if ( !w )
	return;

    const int reasonable_width = 128;
    if ( w < reasonable_width ) {
	// Integer scale up to something reasonable
	int multiply = ( reasonable_width + w - 1 ) / w;
	w *= multiply;
	h *= multiply;
    }

    h += menubar->heightForWidth(w) + status->height();
    resize( w, h );				// we resize to fit image

    reconvertImage();
    repaint( image.hasAlphaBuffer() );

    updateStatus();
    setMenuItemFlags();
}

void ImageViewer::editText()
{
    ImageTextEditor editor(image,this);
    editor.exec();
}

void ImageViewer::to1Bit()
{
    toBitDepth(1);
}

void ImageViewer::to8Bit()
{
    toBitDepth(8);
}

void ImageViewer::to32Bit()
{
    toBitDepth(32);
}

void ImageViewer::toBitDepth(int d)
{
    image = image.convertDepth(d);
    reconvertImage();
    repaint( image.hasAlphaBuffer() );
}
