//-----------------------------------------------------------------------------
//
// kbanner - Basic screen saver for KDE
//
// Copyright (c)  Martin R. Jones 1996
//
// layout management added 1998/04/19 by Mario Weilguni <mweilguni@kde.org>
// clock function and color cycling added 2000/01/09 by Alexander Neundorf <alexander.neundorf@rz.tu-ilmenau.de>
// 2001/03/04 Converted to use libkscreensaver by Martin R. Jones
#include <stdlib.h>

#include <qcolor.h>
#include <qlabel.h>
#include <qscrollbar.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qslider.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qdatetime.h>
#include <qpushbutton.h>
#include <qfontdatabase.h>

#include <kapp.h>
#include <kglobal.h>
#include <klocale.h>
#include <kconfig.h>
#include <kbuttonbox.h>
#include <kmessagebox.h>
#include <kcolorbutton.h>

#include "banner.h"
#include "banner.moc"

// libkscreensaver interface
extern "C"
{
    const char *kss_applicationName = "kbanner2.kss";
    const char *kss_description = I18N_NOOP( "KBanner" );
    const char *kss_version = "2.2.0";

    KScreenSaver *kss_create( WId id )
    {
        return new KBannerSaver( id );
    }

    QDialog *kss_setup()
    {
        return new KBannerSetup();
    }
}

//-----------------------------------------------------------------------------

KBannerSetup::KBannerSetup( QWidget *parent, const char *name )
	: QDialog( parent, name, TRUE )
,ed(0)
{
	saver = NULL;
	speed = 50;

	readSettings();

	QString str;
	QLabel *label;
	QPushButton *button;
	QSlider *sb;
	QComboBox *combo;

	setCaption( i18n("Setup Banner Screen Saver") );

	QVBoxLayout *tl = new QVBoxLayout(this, 10, 10);
	QHBoxLayout *tl1 = new QHBoxLayout;
	tl->addLayout(tl1);
	QVBoxLayout *tl11 = new QVBoxLayout(5);
	tl1->addLayout(tl11);

	QGroupBox *group = new QGroupBox( i18n("Font"), this );
	QGridLayout *gl = new QGridLayout(group, 6, 2, 5);
	gl->addRowSpacing(0, 10);

	label = new QLabel( i18n("Family:"), group );
	gl->addWidget(label, 1, 0);

	combo = new QComboBox( FALSE, group);
	int i = 0;
	QFontDatabase database;
	QStringList fonts = database.families();
	while ( !fonts[i].isNull() )
	{
		combo->insertItem( fonts[i], i );
		if ( fontFamily == fonts[i] )
			combo->setCurrentItem( i );
		i++;
	}
	gl->addWidget(combo, 1, 1);
	connect( combo, SIGNAL( activated( const QString& ) ),
			SLOT( slotFamily( const QString& ) ) );

	label = new QLabel( i18n("Size:"), group );
	gl->addWidget(label, 2, 0);

	combo = new QComboBox( FALSE, group );
	i = 0;
	sizes = database.pointSizes( fontFamily );
	sizes << 96 << 0;
	while ( sizes[i] )
	{
		QString num;
		num.setNum( sizes[i] );
		combo->insertItem( num, i );
		if ( fontSize == sizes[i] )
			combo->setCurrentItem( i );
		i++;
	}
	gl->addWidget(combo, 2, 1);
	connect( combo, SIGNAL( activated( int ) ), SLOT( slotSize( int ) ) );

	QCheckBox *cb = new QCheckBox( i18n("Bold"),
				       group );
	cb->setChecked( bold );
	connect( cb, SIGNAL( toggled( bool ) ), SLOT( slotBold( bool ) ) );
	gl->addWidget(cb, 3, 0);

	cb = new QCheckBox( i18n("Italic"), group );
	cb->setChecked( italic );
	gl->addWidget(cb, 3, 1);
	connect( cb, SIGNAL( toggled( bool ) ), SLOT( slotItalic( bool ) ) );

	label = new QLabel( i18n("Color:"), group );
	gl->addWidget(label, 4, 0);

	colorPush = new KColorButton( fontColor, group );
	gl->addWidget(colorPush, 4, 1);
	connect( colorPush, SIGNAL( changed(const QColor &) ),
		 SLOT( slotColor(const QColor &) ) );

   QCheckBox *cyclingColorCb=new QCheckBox(i18n("Cycling color"),group);
   cyclingColorCb->setMinimumSize(cyclingColorCb->sizeHint());
   gl->addMultiCellWidget(cyclingColorCb,5,5,0,1);
   connect(cyclingColorCb,SIGNAL(toggled(bool)),this,SLOT(slotCyclingColor(bool)));
   cyclingColorCb->setChecked(cyclingColor);

	preview = new QWidget( this );
	preview->setFixedSize( 220, 170 );
	preview->setBackgroundColor( black );
	preview->show();    // otherwise saver does not get correct size
	saver = new KBannerSaver( preview->winId() );
	tl1->addWidget(preview);

	tl11->addWidget(group);

	label = new QLabel( i18n("Speed:"), this );
	tl11->addStretch(1);
	tl11->addWidget(label);

	sb = new QSlider(0, 100, 10, speed, QSlider::Horizontal, this );
	sb->setMinimumWidth( 180);
	sb->setFixedHeight(20);
    sb->setTickmarks(QSlider::Below);
    sb->setTickInterval(10);
	tl11->addWidget(sb);
	connect( sb, SIGNAL( valueChanged( int ) ), SLOT( slotSpeed( int ) ) );

	QHBoxLayout *tl2 = new QHBoxLayout;
	tl->addLayout(tl2);

	label = new QLabel( i18n("Message:"), this );
	tl2->addWidget(label);

	ed = new QLineEdit( this );
	tl2->addWidget(ed);
	ed->setText( message );
	connect( ed, SIGNAL( textChanged( const QString & ) ),
			SLOT( slotMessage( const QString &  ) ) );

   QCheckBox *timeCb=new QCheckBox( i18n("Show current time"),this);
   timeCb->setFixedSize(timeCb->sizeHint());
   tl->addWidget(timeCb,0,Qt::AlignLeft);
   connect(timeCb,SIGNAL(toggled(bool)),this,SLOT(slotTimeToggled(bool)));
   timeCb->setChecked(showTime);

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

// read settings from config file
void KBannerSetup::readSettings()
{
    KConfig *config = KGlobal::config();
    config->setGroup( "Settings" );

   speed=config->readNumEntry("Speed",50);
/*	if ( speed > 100 )
		speed = 100;
	else if ( speed < 50 )
		speed = 50;*/

   message=config->readEntry("Message","KDE");

   showTime=config->readBoolEntry("ShowTime",FALSE);

   fontFamily=config->readEntry("FontFamily",(QApplication::font()).family());

   fontSize=config->readNumEntry("FontSize",48);

   fontColor.setNamedColor(config->readEntry("FontColor","red"));

   cyclingColor=config->readBoolEntry("CyclingColor",FALSE);

   bold=config->readBoolEntry("FontBold",FALSE);

   italic=config->readBoolEntry("FontItalic",FALSE);
}

void KBannerSetup::slotFamily( const QString& fam )
{
	fontFamily = fam;
	if ( saver )
		saver->setFont( fontFamily, fontSize, fontColor, bold, italic );
}

void KBannerSetup::slotSize( int indx )
{
	fontSize = sizes[indx];
	if ( saver )
		saver->setFont( fontFamily, fontSize, fontColor, bold, italic );
}

void KBannerSetup::slotColor( const QColor &col )
{
    fontColor = col;
    if ( saver )
	saver->setColor(fontColor);
}

void KBannerSetup::slotCyclingColor(bool on)
{
   colorPush->setEnabled(!on);
   cyclingColor=on;

   if (saver) saver->setCyclingColor( cyclingColor );
}

void KBannerSetup::slotBold( bool state )
{
	bold = state;
	if ( saver )
		saver->setFont( fontFamily, fontSize, fontColor, bold, italic );
}

void KBannerSetup::slotItalic( bool state )
{
	italic = state;
	if ( saver )
		saver->setFont( fontFamily, fontSize, fontColor, bold, italic );
}

void KBannerSetup::slotSpeed( int num )
{
	speed = num;
	if ( saver )
		saver->setSpeed( speed );
}

void KBannerSetup::slotMessage( const QString &msg )
{
	message = msg;
	if ( saver )
		saver->setMessage( message );
}

void KBannerSetup::slotTimeToggled( bool on )
{
   ed->setEnabled(!on);
   showTime=on;
   if (saver)
   {
      if (showTime)
         saver->setTimeDisplay();
      else
      {
         message=ed->text();
         saver->setMessage(message);
      };
   };
}

// Ok pressed - save settings and exit
void KBannerSetup::slotOkPressed()
{
    KConfig *config = KGlobal::config();
	config->setGroup( "Settings" );

	config->writeEntry( "Speed", speed );

	config->writeEntry( "Message", message );
	config->writeEntry( "ShowTime", showTime );

	config->writeEntry( "FontFamily", fontFamily );

	QString fsize;
	fsize.setNum( fontSize );
	config->writeEntry( "FontSize", fsize );

	QString colName;
	colName.sprintf( "#%02x%02x%02x", fontColor.red(), fontColor.green(),
		fontColor.blue() );
	config->writeEntry( "FontColor", colName );
	config->writeEntry( "CyclingColor", cyclingColor );

	config->writeEntry( "FontBold", bold );
	config->writeEntry( "FontItalic", italic );

	config->sync();

	accept();
}

void KBannerSetup::slotAbout()
{
	KMessageBox::about(this,
			     i18n("Banner Version 2.2.0\n\nwritten by Martin R. Jones 1996\nmjones@kde.org\nextended by Alexander Neundorf 2000\nalexander.neundorf@rz.tu-ilmenau.de\n"));
}

//-----------------------------------------------------------------------------

KBannerSaver::KBannerSaver( WId id ) : KScreenSaver( id )
{
	readSettings();
	initialize();
	colorContext = QColor::enterAllocContext();
	blank();
	timer.start( speed );
	connect( &timer, SIGNAL( timeout() ), SLOT( slotTimeout() ) );
    fwidth = -1;
}

KBannerSaver::~KBannerSaver()
{
	timer.stop();
	QColor::leaveAllocContext();
	QColor::destroyAllocContext( colorContext );
}

void KBannerSaver::setSpeed( int spd )
{
	timer.stop();
	speed = 101-spd;
	timer.start( speed );
}

void KBannerSaver::setFont( const QString& family, int size, const QColor &color,
		bool b, bool i )
{
	fontFamily = family;
	fontSize = size;
	fontColor = color;
	bold = b;
	italic = i;

	blank();
	initialize();
}

void KBannerSaver::setColor(QColor &color)
{
   fontColor=color;
};

void KBannerSaver::setCyclingColor( bool on )
{
   cyclingColor=on;
   if (cyclingColor)
      fontColor.setHsv(0,SATURATION,VALUE);
}

void KBannerSaver::setMessage( const QString &msg )
{
    showTime=FALSE;
    QPainter p (this );
    p.setFont( font );
    p.setPen( black );
    p.drawText( xpos, ypos, message );

    message = msg;
    fwidth = -1;
}

void KBannerSaver::setTimeDisplay()
{
    QPainter p (this );
    p.setFont( font );
    p.setPen( black );
    p.drawText( xpos, ypos, message );
    showTime=TRUE;
    message=KGlobal::locale()->formatTime(QTime::currentTime(), true);
    fwidth = -1;
}

// read settings from config file
void KBannerSaver::readSettings()
{
    KConfig *config = KGlobal::config();
    config->setGroup( "Settings" );

   speed=101-config->readNumEntry("Speed",50);

   message=config->readEntry("Message","KDE");

   showTime=config->readBoolEntry("ShowTime",FALSE);

   fontFamily=config->readEntry("FontFamily",(QApplication::font()).family());

   fontSize=config->readNumEntry("FontSize",48);

   fontColor.setNamedColor(config->readEntry("FontColor","red"));

   cyclingColor=config->readBoolEntry("CyclingColor",FALSE);

   bold=config->readBoolEntry("FontBold",FALSE);
   italic=config->readBoolEntry("FontItalic",FALSE);

    if ( cyclingColor )
    {
        currentHue=0;
        fontColor.setHsv(0,SATURATION,VALUE);
    }
}

// initialize font
void KBannerSaver::initialize()
{
	int size;
	char ichar;

	if ( !qstricmp( fontFamily.local8Bit(), "Helvetica" ) )
		ichar = 'o';
	else
		ichar = 'i';

	size = fontSize * height() / QApplication::desktop()->height();

	font = QFont( fontFamily, size, bold ? QFont::Bold : QFont::Normal, italic );

	fwidth = -1;

	xpos = width();
	ypos = height() / 2;
	step = 6 * width() / QApplication::desktop()->width();
	if ( step == 0 )
		step = 1;
}

// erase old text and draw in new position
void KBannerSaver::slotTimeout()
{
    if (cyclingColor)
    {
        currentHue=(currentHue+4)%360;
        fontColor.setHsv(currentHue,SATURATION,VALUE);
    }
    QPainter p( this );
    p.setFont( font );
    if ( fwidth < 0 )
        fwidth = p.fontMetrics().width( message );
    p.setPen( black );
    p.drawText( xpos, ypos, message );

	xpos -= step;
	if ( xpos < -fwidth-(int)width()/2 )
		xpos = width();

    p.setPen( fontColor );

    if (showTime)
        message = KGlobal::locale()->formatTime(QTime::currentTime(), true);
    p.drawText( xpos, ypos, message );
}

void KBannerSaver::blank()
{
	setBackgroundColor( black );
	erase();
}

