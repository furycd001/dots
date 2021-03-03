//-----------------------------------------------------------------------------
//
// kblankscrn - Basic screen saver for KDE
//
// Copyright (c)  Martin R. Jones 1996
//
// 1998/04/19 Layout management added by Mario Weilguni <mweilguni@kde.org>
// 2001/03/04 Converted to use libkscreensaver by Martin R. Jones

#include <stdlib.h>
#include <qlabel.h>
#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>
#include <kconfig.h>
#include <kcolordlg.h>
#include <kbuttonbox.h>
#include <kcolorbutton.h>
#include <kglobal.h>
#include "blankscrn.h"
#include "blankscrn.moc"

// libkscreensaver interface
extern "C"
{
    const char *kss_applicationName = "kblankscrn.kss";
    const char *kss_description = I18N_NOOP( "KBlankScreen" );
    const char *kss_version = "2.2.0";

    KScreenSaver *kss_create( WId id )
    {
        return new KBlankSaver( id );
    }

    QDialog *kss_setup()
    {
        return new KBlankSetup();
    }
}

//-----------------------------------------------------------------------------
// dialog to setup screen saver parameters
//
KBlankSetup::KBlankSetup( QWidget *parent, const char *name )
	: QDialog( parent, name, TRUE )
{
	readSettings();

	QLabel *label;
	QPushButton *button;

	setCaption( i18n("Setup Blank Screen Saver") );

	QVBoxLayout *tl = new QVBoxLayout(this, 10);
	QHBoxLayout *tl1 = new QHBoxLayout;
	tl->addLayout(tl1);

	QVBoxLayout *tl11 = new QVBoxLayout(5);
	tl1->addLayout(tl11);

	label = new QLabel( i18n("Color:"), this );
	tl11->addWidget(label);

	colorPush = new KColorButton( color, this );
	colorPush->setMinimumWidth(80);
	connect( colorPush, SIGNAL( changed(const QColor &) ),
		SLOT( slotColor(const QColor &) ) );
	tl11->addWidget(colorPush);
	tl11->addStretch(1);

	preview = new QWidget( this );
	preview->setFixedSize( 220, 170 );
	preview->setBackgroundColor( black );
	preview->show();    // otherwise saver does not get correct size
	saver = new KBlankSaver( preview->winId() );
	tl1->addWidget(preview);

	KButtonBox *bbox = new KButtonBox(this);	
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
void KBlankSetup::readSettings()
{
	KConfig *config = KGlobal::config();
	config->setGroup( "Settings" );

	color = config->readColorEntry( "Color", &black );
}

void KBlankSetup::slotColor( const QColor &col )
{
    color = col;
    saver->setColor( color );
}

// Ok pressed - save settings and exit
void KBlankSetup::slotOkPressed()
{
	KConfig *config = KGlobal::config();
	config->setGroup( "Settings" );

	config->writeEntry( "Color", color );

	config->sync();

	accept();
}

//-----------------------------------------------------------------------------


KBlankSaver::KBlankSaver( WId id ) : KScreenSaver( id )
{
	readSettings();

	blank();
}

KBlankSaver::~KBlankSaver()
{
}

// set the color
void KBlankSaver::setColor( const QColor &col )
{
	color = col;
	blank();
}

// read configuration settings from config file
void KBlankSaver::readSettings()
{
	KConfig *config = KGlobal::config();
	config->setGroup( "Settings" );

	color = config->readColorEntry( "Color", &black );
}

void KBlankSaver::blank()
{
    setBackgroundColor( color );
    erase();
}

