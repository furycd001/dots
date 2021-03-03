//-----------------------------------------------------------------------------
//
// kblob - Basic screen saver for KDE
//
// Copyright (c)  Tiaan Wessels, 1997
//
// To add new alg :
//		- add blob_alg enum in blob.h before ALG_LAST
//		- choose 2 letter prefix for alg and add vars needed to private vars
//		  in KBlobSaver in blob.h
//		- add xxSetup and xxNextFrame method definitions in blob.h
//		- implement methods in this file. xxSetup to init vars mentioned
//		  in step 2. xxNextFrame to advance blob painter ( calc tx,ty and
//		  use box() method to position painter
//		- add descriptive string in alg_str array in this file before "Random"
//		- add to Algs array in KBlobSaver constructor in this file
//		- test by setup saver and choosing alg from list

#include <config.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>

#include <qcolor.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qscrollbar.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qvalidator.h>

#include <kapp.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <kbuttonbox.h>
#include <klocale.h>
#include <kglobal.h>
#include <krandomsequence.h>

#include "blob.moc"
#include "blob.h"

#define SMALLRAND(a)	(int)(rnd->getLong(a)+1)


// libkscreensaver interface
extern "C"
{
    const char *kss_applicationName = "kblob2.kss";
    const char *kss_description = I18N_NOOP( "KBlob" );
    const char *kss_version = "2.2.0";

    KScreenSaver *kss_create( WId id )
    {
        return new KBlobSaver( id );
    }

    QDialog *kss_setup()
    {
        return new KBlobSetup();
    }
}

static KRandomSequence *rnd = 0;

QString alg_str[5];
void initAlg()
{
  alg_str[0] = i18n("Random Linear");
  alg_str[1] = i18n("Horizonal Sine");
  alg_str[2] = i18n("Circular Bounce");
  alg_str[3] = i18n("Polar Coordinates");
  alg_str[4] = i18n("Random");
}

//-----------------------------------------------------------------------------
// the blob screensaver's code

KBlobSaver::KBlobSaver ( WId id)
    : KScreenSaver( id )
{
	rnd = new KRandomSequence();
 initAlg();
 QColor color;
	float ramp = (256.0-64.0)/(float)RAMP;
	QString msg =
	  i18n("Sorry. This screen saver requires a color display");

    blank();

	// needs colors to work this one
	if (QPixmap::defaultDepth() < 8)
	{
        QPainter p(this);
        p.setPen( white );
        p.drawText( width()/2, height()/2, msg );
		return;
	}

	colorContext = QColor::enterAllocContext();

	// if 8-bit, create lookup table for color ramping further down
	if (QPixmap::defaultDepth() == 8)
	{
		memset(lookup, 0, 256*sizeof(uint));
                int i;
		for (i = 0; i < RAMP; i++)
		{
			color.setRgb(64+(int)(ramp*(float)i), 0, 0);
			colors[i] = color.alloc();
		}
		memset(lookup, black.pixel(), sizeof(uint)*256);
		for (i = 0; i < RAMP-1; i++)
			lookup[colors[i]] = colors[i+1];
		lookup[black.pixel()] = lookup[colors[RAMP-1]] = colors[0];
	}
	else
	{
		// make special provision for preview mode
		if (height() < 400)
		{
			if (QPixmap::defaultDepth() > 8 )
				setColorInc(7);
			else
				setColorInc(4);
		}
		else
		{
			if (QPixmap::defaultDepth() > 8 )
				setColorInc(3);
			else
				setColorInc(2);
		}
	}

	// the dimensions of the blob painter
	dim = height()/70+1;

	// record starting time to know when to change frames
	start = time(NULL);

	// init some parameters used by all algorithms
	xhalf = width()/2;
	yhalf = height()/2;

	// means a new algorithm should be set at entrance of timer
	newalg = newalgp = 1;

	// init algorithm space
	Algs[0].Name = alg_str[0];
	Algs[0].Init = &KBlobSaver::lnSetup;
	Algs[0].NextFrame = &KBlobSaver::lnNextFrame;

	Algs[1].Name = alg_str[1];
	Algs[1].Init = &KBlobSaver::hsSetup;
	Algs[1].NextFrame = &KBlobSaver::hsNextFrame;

	Algs[2].Name = alg_str[2];
	Algs[2].Init = &KBlobSaver::cbSetup;
	Algs[2].NextFrame = &KBlobSaver::cbNextFrame;

	Algs[3].Name = alg_str[3];
	Algs[3].Init = &KBlobSaver::pcSetup;
	Algs[3].NextFrame = &KBlobSaver::pcNextFrame;

	// get setup from kde registry
	readSettings();

	// start timer which will update blob painter
	timer.start(SPEED);
	connect(&timer, SIGNAL(timeout()), SLOT(slotTimeout()));
}

KBlobSaver::~KBlobSaver()
{
	timer.stop();

	QColor::leaveAllocContext();
	QColor::destroyAllocContext(colorContext);
	delete rnd; rnd = 0;
}

void KBlobSaver::setAlgorithm(int a)
{
	newalg = newalgp = ((a == ALG_RANDOM) ? 1 : 2);
	alg = a;
}

void KBlobSaver::lnSetup()
{
	// initialize the blob movement dictators with random vals
	// incrementals on axis
	ln_xinc = SMALLRAND(3);
	ln_yinc = SMALLRAND(2);

	// start position
	tx = SMALLRAND(width()-dim-ln_xinc*2);
	ty = SMALLRAND(height()-dim-ln_yinc*2);
}

void KBlobSaver::hsSetup()
{
	hs_per = SMALLRAND(7);
	hs_radians = 0.0;
	hs_rinc = (hs_per*M_PI)/(hs_per*90*4);
	hs_flip = 1.0;
}

void KBlobSaver::cbSetup()
{
	cb_radians = 0.0;
	cb_rinc = (2.0*M_PI)/360.0;
	cb_sradians = 0.0;
	cb_deviate = SMALLRAND(height()/20)+(height()/15);
	cb_radius = height()/2-cb_deviate*2-2*dim;
        cb_devradinc = (rnd->getDouble()*10.0*2.0*M_PI)/360.0;
}

void KBlobSaver::pcSetup()
{
	pc_angle = 0.0;
	pc_radius = 0.0;
	pc_inc = (2.0*M_PI)/720.0;
	pc_crot = 0.0;
	pc_div = SMALLRAND(4)-1;
}

// render next frame ( or change algorithms )
void KBlobSaver::slotTimeout()
{
	time_t now = time(NULL);

	// should algorithm be changed
	if (now-start > showlen)
		newalg = newalgp;

	// set new algorithm
	if (newalg)
	{
		blank();
		if (newalg == 1)
			alg = SMALLRAND(ALG_LAST)-1;
		(this->*Algs[alg].Init)();
		newalg = 0;
		start = time(NULL);
	}

	// gen next fram for current algorithm
	(this->*Algs[alg].NextFrame)();
}

void KBlobSaver::lnNextFrame()
{
	int dir;

	// depending on the algorithm to use, move the blob painter to
	// a new location
	// check for wall hit to change direction
	if (tx+dim+ln_xinc > (int)width()-1 || tx+ln_xinc < 0)
	{
		if (ln_xinc > 0)
			dir = -1;
		else
			dir = 1;
		ln_xinc = SMALLRAND(3.0)*dir;
	}
	if (ty+dim+ln_yinc > (int)height()-1 || ty+ln_yinc < 0)
	{
		if (ln_yinc > 0)
			dir = -1;
		else
			dir = 1;
		ln_yinc = SMALLRAND(2.0)*dir;
	}

	// move box to new position
	tx += ln_xinc;
	ty += ln_yinc;

	// draw new box
	box(tx, ty);
}

void KBlobSaver::hsNextFrame()
{
	static int xlen = width()-(4*dim);
	static int ylen = height()-(4*dim);

	// calc x as offset on angle line and y as vertical offset
	// on interval -1..1 sine of angle
	tx = (int)((hs_radians/(hs_per*M_PI))*(float)xlen);
	ty = (int)((float)(ylen/4)*(hs_flip*sin(hs_radians)))+yhalf;

	// draw new box
	box(tx, ty);

	// set new radians
	hs_radians += hs_rinc;
	if (hs_radians > hs_per*M_PI)
	{
		hs_rinc *= -1.0;
		hs_radians += hs_rinc;
		hs_flip *= -1.0;
	}
	else if (hs_radians < 0.0)
		hsSetup();
}

void KBlobSaver::cbNextFrame()
{
	int deviate;

	// calculate deviation of circle main radius
	deviate = (int)(sin(cb_sradians)*cb_deviate);

	// calculate topleft of box as a circle with a sine perturbed radius
	tx = (int)(cos(cb_radians)*(cb_radius+deviate))+xhalf;
	ty = (int)(sin(cb_radians)*(cb_radius+deviate))+yhalf;

	// draw the box
	box(tx, ty);

	// increase greater circle render angle
	cb_radians += cb_rinc;
	if (cb_radians > 2.0*M_PI)
		cb_radians -= 2.0*M_PI;

	// increase radius deviation offset on sine wave
	cb_sradians += cb_devradinc;
}

void KBlobSaver::pcNextFrame()
{
	static float scale = (float)height()/3.0 - 4.0*dim;

	// simple polar coordinate equation
	if (pc_div < 1.0)
		pc_radius = cos(2.0*pc_angle);
	else
		pc_radius = 1.0/pc_div + cos(2.0*pc_angle);

	tx = (int)(scale*pc_radius*cos(pc_angle+pc_crot))+xhalf;
	ty = (int)(scale*pc_radius*sin(pc_angle+pc_crot))+yhalf;

	// advance blob painter
	box(tx, ty);

	// new movement parameters
	pc_angle += pc_inc;
	if (pc_angle > 2.0*M_PI)
	{
		pc_angle -= 2.0*M_PI;
		pc_crot += M_PI/45.0;
	}
}

void KBlobSaver::box ( int x, int y )
{
	// for bad behaving algorithms that wants to cause an X trap
	// confine to the valid region before using potentially fatal XGetImage
	if ((x+dim) >= width())
		x = width()-dim-1;
	else if (x < 0)
		x = 0;
	if ((y+dim) > height())
		y = height()-dim-1;
	else if (y < 0)
		y = 0;

	// get the box region from the display to upgrade
    QImage img = QPixmap::grabWindow(winId(), x, y, dim, dim).convertToImage();

	// depending on the depth of the display, use either lookup table for
	// next rgb val ( 8-bit ) or ramp the color directly for other displays
	if ( img.depth() == 8)
	{
		// manipulate image by upgrading each pixel with 1 using a lookup
		// table as the color allocation could have resulted in a spread out
		// configuration of the color ramp
		for (int j = 0; j < img.height(); j++)
		{
			for (int i = 0; i < img.width(); i++)
			{
				img.scanLine(j)[i] = lookup[img.scanLine(j)[i]];
			}
		}
	}
	else
	{
		for (int j = 0; j < img.height(); j++)
		{
			for (int i = 0; i < img.width(); i++)
			{
                QRgb p = img.pixel( i, j );
                p += (colorInc<<18);
                img.setPixel( i, j, p );
			}
		}
	}

	// put the image back onto the screen
    QPainter p(this);
    p.drawImage( x, y, img );
}

void KBlobSaver::blank()
{
    setBackgroundColor( black );
    erase();
}

void KBlobSaver::readSettings()
{
    KConfig *config = KGlobal::config();
    config->setGroup("Settings");

    // number of seconds to spend on a frame
    showlen = config->readNumEntry("Showtime", 3*60);

    // algorithm to use. if not set then use random
    alg = config->readNumEntry("Algorithm", ALG_RANDOM);
    if (alg == ALG_RANDOM)
	newalg = 1;
    else
	newalg = 2;
    newalgp = newalg;
}

//-----------------------------------------------------------------------------
// dialog to setup screen saver parameters
//
KBlobSetup::KBlobSetup
(
	QWidget *parent,
	const char *name
)
: QDialog( parent, name, TRUE )
{
  QLabel *label;

  initAlg();
	QPushButton *button;
	QString str;

	// get saver configuration from kde registry
	readSettings();

	setCaption(i18n("Setup Blob Screen Saver"));

	QVBoxLayout *tl = new QVBoxLayout(this, 10);
	QHBoxLayout *tl1 = new QHBoxLayout;
	tl->addLayout(tl1);

	QVBoxLayout *tl11 = new QVBoxLayout(5);
	tl1->addLayout(tl11);

	// seconds to generate on a frame
	label = new QLabel(i18n("Frame Show (secs)"),
		    this);
	stime = new QLineEdit(this);
        stime->setValidator(new QIntValidator(stime));
	str.setNum(showtime);
	stime->setText(str);
	tl11->addWidget(label);
	tl11->addWidget(stime);
	tl11->addSpacing(10);

	// available algorithms
	label = new QLabel(i18n("Algorithm"), this);
	algs = new QListBox(this);
	algs->setMinimumSize(150, 105);
	for (int i = 0; i <= ALG_RANDOM; i++)
		algs->insertItem(alg_str[i]);
	algs->setCurrentItem(alg);
	tl11->addWidget(label);
	tl11->addWidget(algs);
	tl11->addStretch(1);

	// preview window
	QWidget *preview = new QWidget( this );
	preview->setFixedSize(220, 170);
	preview->setBackgroundColor(black);
	preview->show();
	tl1->addWidget(preview);
	saver = new KBlobSaver(preview->winId());
	saver->setDimension(3);
	if (QPixmap::defaultDepth() > 8)
		saver->setColorInc(7);
	else
		saver->setColorInc(4);

	// so selecting an algorithm will start previewing that alg
	connect(algs, SIGNAL(highlighted(int)), saver,
		SLOT(setAlgorithm(int)));

	KButtonBox *bbox = new KButtonBox(this);

	// show an address to send flame mail to
	button = bbox->addButton( i18n("About"));
	connect( button, SIGNAL( clicked() ), SLOT(slotAbout() ) );
	bbox->addStretch(1);

	// means attempt to register settings with kde registry
	button = bbox->addButton( i18n("OK"));
	connect( button, SIGNAL( clicked() ), SLOT( slotOkPressed() ) );

	// ignore changes
	button = bbox->addButton(i18n("Cancel"));
	connect( button, SIGNAL( clicked() ), SLOT( reject() ) );
	bbox->layout();
	tl->addWidget(bbox);

	tl->freeze();
}

void KBlobSetup::readSettings()
{
    KConfig *config = KGlobal::config();
    config->setGroup("Settings");

    // number of seconds to spend on a frame
    showtime = config->readNumEntry("Showtime", 3*60);

    // algorithm to use. if not set then use random
    alg = config->readNumEntry("Algorithm", ALG_LAST);
}

// Ok pressed - save settings and exit
void KBlobSetup::slotOkPressed()
{
    KConfig *config = KGlobal::config();

    config->setGroup("Settings");

    config->writeEntry("Showtime", stime->text());
    config->writeEntry("Algorithm", algs->currentItem());

    config->sync();

    accept();
}

void KBlobSetup::slotAbout()
{
	KMessageBox::about(this,
			     i18n("Blobsaver Version 0.1\n\nwritten by Tiaan Wessels 1997\ntiaan@netsys.co.za"));
	if (saver)
		saver->setAlgorithm(algs->currentItem());
}


