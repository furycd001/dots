//-----------------------------------------------------------------------------
//
// kpolygon - Basic screen saver for KDE
//
// Copyright (c)  Martin R. Jones 1996
//
// layout management added 1998/04/19 by Mario Weilguni <mweilguni@kde.org>
// 2001/03/04 Converted to libkscreensaver by Martin R. Jones

#include <config.h>
#include <stdlib.h>
#include <time.h>
#include <qcolor.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qscrollbar.h>
#include <qslider.h>
#include <qlayout.h>
#include <kapp.h>
#include <kbuttonbox.h>
#include <klocale.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kmessagebox.h>

#include "polygon.h"

#include "polygon.moc"


#define MAXLENGTH	65
#define MAXVERTICES	19

template class QArray<QPoint>;

// libkscreensaver interface
extern "C"
{
    const char *kss_applicationName = "kpolygon2.kss";
    const char *kss_description = I18N_NOOP( "KPolygon" );
    const char *kss_version = "2.2.0";

    KScreenSaver *kss_create( WId id )
    {
        return new kPolygonSaver( id );
    }

    QDialog *kss_setup()
    {
        return new kPolygonSetup();
    }
}

//-----------------------------------------------------------------------------
// dialog to setup screen saver parameters
//
kPolygonSetup::kPolygonSetup( QWidget *parent, const char *name )
	: QDialog( parent, name, TRUE )
{
	saver = NULL;
	length = 10;
	vertices = 3;
	speed = 50;

	readSettings();

	QString str;
	QLabel *label;
	QPushButton *button;
	QSlider *sb;

	setCaption( i18n("Setup Polygon Screen Saver") );

	QVBoxLayout *tl = new QVBoxLayout(this, 10, 10);
	QHBoxLayout *tl1 = new QHBoxLayout;
	tl->addLayout(tl1);
	QVBoxLayout *tl11 = new QVBoxLayout(5);
	tl1->addLayout(tl11);	

	label = new QLabel( i18n("Length:"), this );
	tl11->addWidget(label);

	sb = new QSlider(1, MAXLENGTH, 10, length, QSlider::Horizontal, this );
	sb->setMinimumSize( 90, 20 );
    sb->setTickmarks(QSlider::Below);
    sb->setTickInterval(10);
	connect( sb, SIGNAL( valueChanged( int ) ), 
		 SLOT( slotLength( int ) ) );
	tl11->addWidget(sb);
	tl11->addSpacing(5);

	label = new QLabel( i18n("Vertices:"), this );
	tl11->addWidget(label);


	sb = new QSlider(3, MAXVERTICES, 2, vertices, QSlider::Horizontal, this);
	sb->setMinimumSize( 90, 20 );
    sb->setTickmarks(QSlider::Below);
    sb->setTickInterval(2);
	connect( sb, SIGNAL( valueChanged( int ) ), 
		 SLOT( slotVertices( int ) ) );
	tl11->addWidget(sb);
	tl11->addSpacing(5);

	label = new QLabel( i18n("Speed:"), this );
	tl11->addWidget(label);

	sb = new QSlider(0, 100, 10, speed, QSlider::Horizontal, this);
	sb->setMinimumSize( 90, 20 );
    sb->setTickmarks(QSlider::Below);
    sb->setTickInterval(10);
	connect( sb, SIGNAL( valueChanged( int ) ), 
		 SLOT( slotSpeed( int ) ) );
	tl11->addWidget(sb);
	tl11->addStretch(1);

	preview = new QWidget( this );
	preview->setFixedSize( 220, 170 );
	preview->setBackgroundColor( black );
	preview->show();    // otherwise saver does not get correct size
	saver = new kPolygonSaver( preview->winId() );
	tl1->addWidget(preview);

	KButtonBox *bbox = new KButtonBox(this);	
	button = bbox->addButton( i18n("About"));
	connect( button, SIGNAL( clicked() ), SLOT(slotAbout() ) );
	bbox->addStretch(1);

	button = bbox->addButton( i18n("OK"));	
	connect( button, SIGNAL( clicked() ), SLOT( slotOkPressed() ) );

	button = bbox->addButton(i18n("Cancel"));
	connect( button, SIGNAL( clicked() ), SLOT( reject() ) );
	bbox->layout();
	tl->addWidget(bbox);

	tl->freeze();
}

kPolygonSetup::~kPolygonSetup()
{
    delete saver;
}

// read settings from config file
void kPolygonSetup::readSettings()
{
    KConfig *config = KGlobal::config();
    config->setGroup( "Settings" );
    
    length = config->readNumEntry( "Length", length );
    if ( length > MAXLENGTH )
        length = MAXLENGTH;
    else if ( length < 1 )
        length = 1;
    
    vertices = config->readNumEntry( "Vertices", vertices );
    if ( vertices > MAXVERTICES )
        vertices = MAXVERTICES;
    else if ( vertices < 3 )
        vertices = 3;
    
    speed = config->readNumEntry( "Speed", speed );
    if ( speed > 100 )
        speed = 100;
    else if ( speed < 50 )
        speed = 50;
}

void kPolygonSetup::slotLength( int len )
{
	length = len;
	if ( saver )
		saver->setPolygon( length, vertices );
}

void kPolygonSetup::slotVertices( int num )
{
	vertices = num;
	if ( saver )
		saver->setPolygon( length, vertices );
}

void kPolygonSetup::slotSpeed( int num )
{
	speed = num;
	if ( saver )
		saver->setSpeed( speed );
}

// Ok pressed - save settings and exit
void kPolygonSetup::slotOkPressed()
{
    KConfig *config = KGlobal::config();
    config->setGroup( "Settings" );
    
    QString slength;
    slength.setNum( length );
    config->writeEntry( "Length", slength );
    
    QString svertices;
    svertices.setNum( vertices );
    config->writeEntry( "Vertices", svertices );
    
    QString sspeed;
    sspeed.setNum( speed );
    config->writeEntry( "Speed", sspeed );
    
    config->sync();
    
    accept();
}

void kPolygonSetup::slotAbout()
{
	KMessageBox::information(this,
			     i18n("Polygon Version 2.2.0\n\n"\
					       "written by Martin R. Jones 1996\n"\
					       "mjones@kde.org"));
}

//-----------------------------------------------------------------------------


kPolygonSaver::kPolygonSaver( WId id ) : KScreenSaver( id )
{
	polygons.setAutoDelete( TRUE );

	readSettings();

	directions.resize( numVertices );
	colorContext = QColor::enterAllocContext();

	blank();

	initialiseColor();
	initialisePolygons();

	timer.start( speed );
	connect( &timer, SIGNAL( timeout() ), SLOT( slotTimeout() ) );
}

kPolygonSaver::~kPolygonSaver()
{
	timer.stop();
	QColor::leaveAllocContext();
	QColor::destroyAllocContext( colorContext );
}

// set polygon properties
void kPolygonSaver::setPolygon( int len, int ver )
{
	timer.stop();
	numLines = len;
	numVertices = ver;

	directions.resize( numVertices );
	polygons.clear();
	initialisePolygons();
	blank();

	timer.start( speed );
}

// set the speed
void kPolygonSaver::setSpeed( int spd )
{
	timer.stop();
	speed = 100-spd;
	timer.start( speed );
}

// read configuration settings from config file
void kPolygonSaver::readSettings()
{
    KConfig *config = KGlobal::config();
    config->setGroup( "Settings" );

    numLines = config->readNumEntry( "Length", 10 );
    if ( numLines > 50 )
	    numLines = 50;
    else if ( numLines < 1 )
	    numLines = 1;

    numVertices = config->readNumEntry( "Vertices", 3 );
    if ( numVertices > 20 )
	    numVertices = 20;
    else if ( numVertices < 3 )
	    numVertices = 3;

    speed = 100 - config->readNumEntry( "Speed", 50 );
}

// draw next polygon and erase tail
void kPolygonSaver::slotTimeout()
{
    QPainter p( this );
	if ( polygons.count() > numLines )
	{
		p.setPen( black );
        p.drawPolyline( *polygons.first() );
	}

	nextColor();
    p.setPen( colors[currentColor] );
    p.drawPolyline( *polygons.last() );

	if ( polygons.count() > numLines )
		polygons.removeFirst();

	polygons.append( new QPointArray( polygons.last()->copy() ) );
	moveVertices();
}

void kPolygonSaver::blank()
{
	setBackgroundColor( black );
	erase();
}

// initialise the polygon
void kPolygonSaver::initialisePolygons()
{
	int i;

	polygons.append( new QPointArray( numVertices + 1 ) );

	QPointArray &poly = *polygons.last();

	for ( i = 0; i < numVertices; i++ )
	{
		poly.setPoint( i, rnd.getLong(width()), rnd.getLong(height()) );
		directions[i].setX( 16 - rnd.getLong(8) * 4 );
		if ( directions[i].x() == 0 )
			directions[i].setX( 1 );
		directions[i].setY( 16 - rnd.getLong(8) * 4 );
		if ( directions[i].y() == 0 )
			directions[i].setY( 1 );
	}

	poly.setPoint( i, poly.point(0) );
}

// move polygon in current direction and change direction if a border is hit
void kPolygonSaver::moveVertices()
{
	int i;
	QPointArray &poly = *polygons.last();

	for ( i = 0; i < numVertices; i++ )
	{
		poly.setPoint( i, poly.point(i) + directions[i] );
		if ( poly[i].x() >= (int)width() )
		{
			directions[i].setX( -(rnd.getLong(4) + 1) * 4 );
			poly[i].setX( (int)width() );
		}
		else if ( poly[i].x() < 0 )
		{
			directions[i].setX( (rnd.getLong(4) + 1) * 4 );
			poly[i].setX( 0 );
		}

		if ( poly[i].y() >= (int)height() )
		{
			directions[i].setY( -(rnd.getLong(4) + 1) * 4 );
			poly[i].setY( height() );
		}
		else if ( poly[i].y() < 0 )
		{
			directions[i].setY( (rnd.getLong(4) + 1) * 4 );
			poly[i].setY( 0 );
		}
	}

	poly.setPoint( i, poly.point(0) );
}

// create a color table of 64 colors
void kPolygonSaver::initialiseColor()
{
	for ( int i = 0; i < 64; i++ )
	{
		colors[i].setHsv( i * 360 / 64, 255, 255 );
	}

    currentColor = 0;
}

// set foreground color to next in the table
void kPolygonSaver::nextColor()
{
	currentColor++;

	if ( currentColor > 63 )
		currentColor = 0;
}

