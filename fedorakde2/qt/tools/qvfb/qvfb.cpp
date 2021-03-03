/****************************************************************************
**
** Qt/Embedded virtual framebuffer
**
** Created : 20000605
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the Qt GUI Toolkit.
**
** Licensees holding valid Qt Professional Edition licenses may use this
** file in accordance with the Qt Professional Edition License Agreement
** provided with the Qt Professional Edition.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
** information about the Professional Edition licensing.
**
*****************************************************************************/

#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qfiledialog.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qimage.h>
#include <qpixmap.h>


#include "qvfb.h"
#include "qvfbview.h"
#include "qvfbratedlg.h"
#include "config.h"
#include "skin.h"


static const char * logo[] = {
/* width height ncolors chars_per_pixel */
"50 50 17 1",
/* colors */
"  c #000000",
". c #495808",
"X c #2A3304",
"o c #242B04",
"O c #030401",
"+ c #9EC011",
"@ c #93B310",
"# c #748E0C",
"$ c #A2C511",
"% c #8BA90E",
"& c #99BA10",
"* c #060701",
"= c #181D02",
"- c #212804",
"; c #61770A",
": c #0B0D01",
"/ c None",
/* pixels */
"/$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$/",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$+++$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$@;.o=::=o.;@$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$+#X*         **X#+$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$#oO*         O  **o#+$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$&.* OO              O*.&$$$$$$$$$$$$$",
"$$$$$$$$$$$$@XOO            * OO    X&$$$$$$$$$$$$",
"$$$$$$$$$$$@XO OO  O  **:::OOO OOO   X@$$$$$$$$$$$",
"$$$$$$$$$$&XO      O-;#@++@%.oOO      X&$$$$$$$$$$",
"$$$$$$$$$$.O  :  *-#+$$$$$$$$+#- : O O*.$$$$$$$$$$",
"$$$$$$$$$#*OO  O*.&$$$$$$$$$$$$+.OOOO **#$$$$$$$$$",
"$$$$$$$$+-OO O *;$$$$$$$$$$$&$$$$;*     o+$$$$$$$$",
"$$$$$$$$#O*  O .+$$$$$$$$$$@X;$$$+.O    *#$$$$$$$$",
"$$$$$$$$X*    -&$$$$$$$$$$@- :;$$$&-    OX$$$$$$$$",
"$$$$$$$@*O  *O#$$$$$$$$$$@oOO**;$$$#    O*%$$$$$$$",
"$$$$$$$;     -+$$$$$$$$$@o O OO ;+$$-O   *;$$$$$$$",
"$$$$$$$.     ;$$$$$$$$$@-OO OO  X&$$;O    .$$$$$$$",
"$$$$$$$o    *#$$$$$$$$@o  O O O-@$$$#O   *o$$$$$$$",
"$$$$$$+=    *@$$$$$$$@o* OO   -@$$$$&:    =$$$$$$$",
"$$$$$$+:    :+$$$$$$@-      *-@$$$$$$:    :+$$$$$$",
"$$$$$$+:    :+$$$$$@o* O    *-@$$$$$$:    :+$$$$$$",
"$$$$$$$=    :@$$$$@o*OOO      -@$$$$@:    =+$$$$$$",
"$$$$$$$-    O%$$$@o* O O    O O-@$$$#*   OX$$$$$$$",
"$$$$$$$. O *O;$$&o O*O* *O      -@$$;    O.$$$$$$$",
"$$$$$$$;*   Oo+$$;O*O:OO--      Oo@+=    *;$$$$$$$",
"$$$$$$$@*  O O#$$$;*OOOo@@-O     Oo;O*  **@$$$$$$$",
"$$$$$$$$X* OOO-+$$$;O o@$$@-    O O     OX$$$$$$$$",
"$$$$$$$$#*  * O.$$$$;X@$$$$@-O O        O#$$$$$$$$",
"$$$$$$$$+oO O OO.+$$+&$$$$$$@-O         o+$$$$$$$$",
"$$$$$$$$$#*    **.&$$$$$$$$$$@o      OO:#$$$$$$$$$",
"$$$$$$$$$+.   O* O-#+$$$$$$$$+;O    OOO:@$$$$$$$$$",
"$$$$$$$$$$&X  *O    -;#@++@#;=O    O    -@$$$$$$$$",
"$$$$$$$$$$$&X O     O*O::::O      OO    Oo@$$$$$$$",
"$$$$$$$$$$$$@XOO                  OO    O*X+$$$$$$",
"$$$$$$$$$$$$$&.*       **  O      ::    *:#$$$$$$$",
"$$$$$$$$$$$$$$$#o*OO       O    Oo#@-OOO=#$$$$$$$$",
"$$$$$$$$$$$$$$$$+#X:* *     O**X#+$$@-*:#$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$%;.o=::=o.#@$$$$$$@X#$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$+++$$$$$$$$$$$+$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"/$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$/",
};

QVFb::QVFb( int display_id, int w, int h, int d, const QString &skin, QWidget *parent,
	    const char *name, uint flags )
    : QMainWindow( parent, name, flags )
{
    imagesave = new QFileDialog(".", "*.png", this, 0, TRUE);
    imagesave->setSelection("snapshot.png");
    setIcon(QPixmap(logo));
    rateDlg = 0;
    view = 0;
    init( display_id, w, h, d, skin );
    createMenu( menuBar() );
}

QVFb::~QVFb()
{
}

void QVFb::popupMenu()
{
    QPopupMenu *pm = new QPopupMenu( this );
    createMenu( pm );
    pm->exec(QCursor::pos());
}

void QVFb::init( int display_id, int w, int h, int d, const QString &skin_name )
{
    setCaption( QString("Virtual framebuffer %1x%2 %3bpp Display :%4")
		    .arg(w).arg(h).arg(d).arg(display_id) );
    delete view;

    if ( !skin_name.isEmpty() && QFile::exists(skin_name) ) {
	bool vis = isVisible();
	if ( vis ) hide();
	menuBar()->hide();
	Skin *skin = new Skin( this, skin_name, w, h );
	view = new QVFbView( display_id, w, h, d, skin );
	skin->setView( view );
	view->setMargin( 0 );
	view->setFrameStyle( QFrame::NoFrame );
	view->setFixedSize( w, h );
	setCentralWidget( skin );
	adjustSize();
	view->show();
	if ( vis ) show();
    } else {
	if ( !currentSkin.isEmpty() ) {
	    clearMask();
	    reparent( 0, 0, pos(), TRUE );
	}
	menuBar()->show();
	view = new QVFbView( display_id, w, h, d, this );
	view->setMargin( 0 );
	view->setFrameStyle( QFrame::NoFrame );
	setCentralWidget( view );
	resize(w,menuBar()->height()+h);
	view->show();
    }

    currentSkin = skin_name;
}

void QVFb::enableCursor( bool e )
{
    view->viewport()->setCursor( e ? ArrowCursor : BlankCursor );
    viewMenu->setItemChecked( cursorId, e );
}

void QVFb::createMenu(QMenuData *menu)
{
    QPopupMenu *file = new QPopupMenu( this );
    file->insertItem( "&Configure...", this, SLOT(configure()), ALT+CTRL+Key_C );
    file->insertSeparator();
    file->insertItem( "&Save image...", this, SLOT(saveImage()), ALT+CTRL+Key_S );
    file->insertItem( "&Animation...", this, SLOT(toggleAnimation()), ALT+CTRL+Key_A );
    file->insertSeparator();
    file->insertItem( "&Quit", qApp, SLOT(quit()) );

    menu->insertItem( "&File", file );

    viewMenu = new QPopupMenu( this );
    viewMenu->setCheckable( true );
    cursorId = viewMenu->insertItem( "Show &Cursor", this, SLOT(toggleCursor()) );
    enableCursor(TRUE);
    viewMenu->insertItem( "&Refresh Rate...", this, SLOT(changeRate()) );
    viewMenu->insertSeparator();
    viewMenu->insertItem( "Zoom scale &1", this, SLOT(setZoom1()) );
    viewMenu->insertItem( "Zoom scale &2", this, SLOT(setZoom2()) );
    viewMenu->insertItem( "Zoom scale &3", this, SLOT(setZoom3()) );
    viewMenu->insertItem( "Zoom scale &4", this, SLOT(setZoom4()) );
    viewMenu->insertItem( "Zoom scale &0.5", this, SLOT(setZoomHalf()) );

    menu->insertItem( "&View", viewMenu );

    QPopupMenu *help = new QPopupMenu( this );
    help->insertItem("About...", this, SLOT(about()));
    menu->insertSeparator();
    menu->insertItem( "&Help", help );
}

void QVFb::setZoom(double z)
{
    view->setZoom(z);
}

void QVFb::setZoomHalf()
{
    setZoom(0.5);
}

void QVFb::setZoom1()
{
    setZoom(1);
}

void QVFb::setZoom2()
{
    setZoom(2);
}

void QVFb::setZoom3()
{
    setZoom(3);
}

void QVFb::setZoom4()
{
    setZoom(4);
}

void QVFb::saveImage()
{
    QImage img = view->image();
    if ( imagesave->exec() ) {
	QString filename = imagesave->selectedFile();
	if ( !!filename )
	    img.save(filename,"PNG");
    }
}

void QVFb::toggleAnimation()
{
    if ( view->animating() ) {
	view->stopAnimation();
    } else {
	QString filename = imagesave->getSaveFileName("animation.mng", "*.mng", this, "", "Save animation...");
	if ( !filename ) {
	    view->stopAnimation();
	} else {
	    view->startAnimation(filename);
	}
    }
}

void QVFb::toggleCursor()
{
    enableCursor( !viewMenu->isItemChecked( cursorId ) );
}

void QVFb::changeRate()
{
    if ( !rateDlg ) {
	rateDlg = new QVFbRateDialog( view->rate(), this );
	connect( rateDlg, SIGNAL(updateRate(int)), view, SLOT(setRate(int)) );
    }

    rateDlg->show();
}

void QVFb::about()
{
    QMessageBox::about(this, "About QVFB",
	"<h2>The Qt/Embedded Virtual X11 Framebuffer</h2>"
	"<p>This application runs under Qt/X11, emulating a simple framebuffer, "
	"which the Qt/Embedded server and clients can attach to just as if "
	"it was a hardware Linux framebuffer. "
	"<p>With the aid of this development tool, you can develop Qt/Embedded "
	"applications under X11 without having to switch to a virtual console. "
	"This means you can comfortably use your other development tools such "
	"as GUI profilers and debuggers."
    );
}

void QVFb::configure()
{
    config = new Config(this,0,TRUE);

    int w = view->displayWidth();
    int h = view->displayHeight();
    QString skin;
    config->size_width->setValue(w);
    config->size_height->setValue(h);
    config->size_custom->setChecked(TRUE); // unless changed by settings below
    config->size_240_320->setChecked(w==240&&h==320);
    config->size_320_240->setChecked(w==320&&h==240);
    config->size_640_480->setChecked(w==640&&h==480);
    config->skin->setEditable(TRUE);
    if (!currentSkin.isNull()) {
	config->size_skin->setChecked(TRUE);
	config->skin->setEditText(currentSkin);
    }
    config->depth_1->setChecked(view->displayDepth()==1);
    config->depth_4gray->setChecked(view->displayDepth()==4);
    config->depth_8->setChecked(view->displayDepth()==8);
    config->depth_12->setChecked(view->displayDepth()==12);
    config->depth_16->setChecked(view->displayDepth()==16);
    config->depth_32->setChecked(view->displayDepth()==32);
    if ( view->gammaRed() == view->gammaGreen() && view->gammaGreen() == view->gammaBlue() ) {
	config->gammaslider->setValue(int(view->gammaRed()*400));
	config->rslider->setValue(100);
	config->gslider->setValue(100);
	config->bslider->setValue(100);
    } else {
	config->gammaslider->setValue(100);
	config->rslider->setValue(int(view->gammaRed()*400));
	config->gslider->setValue(int(view->gammaGreen()*400));
	config->bslider->setValue(int(view->gammaBlue()*400));
    }
    connect(config->gammaslider, SIGNAL(valueChanged(int)), this, SLOT(setGamma400(int)));
    connect(config->rslider, SIGNAL(valueChanged(int)), this, SLOT(setR400(int)));
    connect(config->gslider, SIGNAL(valueChanged(int)), this, SLOT(setG400(int)));
    connect(config->bslider, SIGNAL(valueChanged(int)), this, SLOT(setB400(int)));
    updateGammaLabels();

    double ogr=view->gammaRed(), ogg=view->gammaGreen(), ogb=view->gammaBlue();

    if ( config->exec() ) {
	int id = view->displayId(); // not settable yet
	if ( config->size_240_320->isChecked() ) {
	    w=240; h=320;
	} else if ( config->size_320_240->isChecked() ) {
	    w=320; h=240;
	} else if ( config->size_640_480->isChecked() ) {
	    w=640; h=480;
	} else if ( config->size_skin->isChecked() ) {
	    skin = config->skin->currentText();
	} else {
	    w=config->size_width->value();
	    h=config->size_height->value();
	}
	int d;
	if ( config->depth_1->isChecked() )
	    d=1;
	else if ( config->depth_4gray->isChecked() )
	    d=4;
	else if ( config->depth_8->isChecked() )
	    d=8;
	else if ( config->depth_12->isChecked() )
	    d=12;
	else if ( config->depth_16->isChecked() )
	    d=16;
	else
	    d=32;

	if ( w != view->displayWidth() || h != view->displayHeight()
		|| d != view->displayDepth() || skin != currentSkin )
	    init( id, w, h, d, skin );
    } else {
	view->setGamma(ogr, ogg, ogb);
    }

    delete config;
    config=0;
}

void QVFb::setGamma400(int n)
{
    double g = n/100.0;
    view->setGamma(config->rslider->value()/100.0*g,
                   config->gslider->value()/100.0*g,
                   config->bslider->value()/100.0*g);
    updateGammaLabels();
}

void QVFb::setR400(int n)
{
    double g = n/100.0;
    view->setGamma(config->rslider->value()/100.0*g,
                   view->gammaGreen(),
                   view->gammaBlue());
    updateGammaLabels();
}

void QVFb::setG400(int n)
{
    double g = n/100.0;
    view->setGamma(view->gammaRed(),
                   config->gslider->value()/100.0*g,
                   view->gammaBlue());
    updateGammaLabels();
}

void QVFb::setB400(int n)
{
    double g = n/100.0;
    view->setGamma(view->gammaRed(),
                   view->gammaGreen(),
                   config->bslider->value()/100.0*g);
    updateGammaLabels();
}

void QVFb::updateGammaLabels()
{
    config->rlabel->setText(QString::number(view->gammaRed(),'g',2));
    config->glabel->setText(QString::number(view->gammaGreen(),'g',2));
    config->blabel->setText(QString::number(view->gammaBlue(),'g',2));
}

QSize QVFb::sizeHint() const
{
    return QSize(int(view->displayWidth()*view->zoom()),
	    int(menuBar()->height()+view->displayHeight()*view->zoom()));
}
