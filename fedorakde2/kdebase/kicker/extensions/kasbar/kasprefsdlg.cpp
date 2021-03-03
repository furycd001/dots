#include <qvbox.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qwhatsthis.h>

#include <kdialogbase.h>
#include <kcolorbtn.h>
#include <klocale.h>
#include <kconfig.h>
#include <knuminput.h>
#include <kglobal.h>
#include <kiconloader.h>

#include "kastasker.h"
#include "kasprefsdlg.h"

#define Icon(x) KGlobal::instance()->iconLoader()->loadIcon( x, KIcon::NoGroup, KIcon::SizeMedium )


KasPrefsDialog::KasPrefsDialog( KasTasker *kas, KConfig *config )
   : KDialogBase( KDialogBase::IconList, i18n("Kasbar Preferences"),
		  KDialogBase::Ok | KDialogBase::Cancel,
		  KDialogBase::Ok,
		  kas, "kasbarPrefsDialog", true ),
			       kasbar( kas ),
			       conf( config )
{
   //
   // Appearance settings
   //
   QVBox *lookPage = addVBoxPage( i18n("Appearance"), QString::null, Icon( "appearance" ) );

   QHBox *itemSizeBox = new QHBox( lookPage );
   QWhatsThis::add( itemSizeBox,
		    i18n( "Specifies the size of the task items." ) );

   QLabel *itemSizeLabel = new QLabel( i18n("Si&ze:"), itemSizeBox );

   itemSizeCombo = new QComboBox( itemSizeBox );
   itemSizeCombo->insertItem( i18n( "Large" ) );
   itemSizeCombo->insertItem( i18n( "Medium" ) );
   itemSizeCombo->insertItem( i18n( "Small" ) );
   itemSizeCombo->setCurrentItem( kasbar->itemSize() );

   connect( itemSizeCombo, SIGNAL( activated( int ) ), 
	    kasbar, SLOT( setItemSize( int ) ) );
   itemSizeLabel->setBuddy( itemSizeCombo );

   transCheck = new QCheckBox( i18n("Trans&parent"), lookPage );
   QWhatsThis::add( transCheck, i18n( "Enables pseudo-transparent mode." ) );
   transCheck->setChecked( kasbar->isTransparent() );
   connect( transCheck, SIGNAL( toggled(bool) ), kasbar, SLOT( setTransparent(bool) ) );

   tintCheck = new QCheckBox( i18n("Enable T&int"), lookPage );
   QWhatsThis::add( tintCheck,
		    i18n( "Enables tinting the background that shows through in transparent mode." ) );
   tintCheck->setChecked( kasbar->hasTint() );
   connect( tintCheck, SIGNAL( toggled(bool) ), kasbar, SLOT( setTint(bool) ) );

   QHBox *tintColBox = new QHBox( lookPage );
   QWhatsThis::add( tintColBox,
		    i18n( "Specifies the color used for the background tint." ) );

   QLabel *tintLabel = new QLabel( i18n("Tint &Color:"), tintColBox );

   tintButton = new KColorButton( kasbar->tintColor(), tintColBox );
   connect( tintButton, SIGNAL( changed( const QColor & ) ), 
	    kasbar, SLOT( setTintColor( const QColor & ) ) );
   tintLabel->setBuddy( tintButton );

   QHBox *tintAmtBox = new QHBox( lookPage );
   QWhatsThis::add( tintAmtBox,
		    i18n( "Specifies the strength of the background tint." ) );

   QLabel *tintStrengthLabel = new QLabel( i18n("Tint &Strength: "), tintAmtBox );

   int percent = (int) (kasbar->tintAmount() * 100.0);
   tintAmount = new QSlider( 0, 100, 1, percent, Horizontal, tintAmtBox );
   tintAmount->setTracking( true );
   connect( tintAmount, SIGNAL( valueChanged( int ) ),
	    kasbar, SLOT( setTintAmount( int ) ) );
   tintStrengthLabel->setBuddy( tintAmount );

   new QWidget( lookPage, "spacer" );

   //
   // Thumbnail settings
   //
   QVBox *thumbsPage = addVBoxPage( i18n("Thumbnails"), QString::null, Icon( "icons" ) );

   thumbsCheck = new QCheckBox( i18n("Enable Thu&mbnails"), thumbsPage );
   QWhatsThis::add( thumbsCheck,
		    i18n( "Enables the display of a thumbnailed image of the window when "
			  "you move your mouse pointer over an item. The thumbnails are "
			  "approximate, and may not reflect the current window contents.\n\n"
			  "Using this option on a slow machine may cause performance problems." ) );
   thumbsCheck->setChecked( kasbar->thumbnailsEnabled() );
   connect( thumbsCheck, SIGNAL( toggled(bool) ), kasbar, SLOT( setThumbnailsEnabled(bool) ) );

   QHBox *thumbSizeBox = new QHBox( thumbsPage );
   QWhatsThis::add( thumbSizeBox,
		    i18n( "Controls the size of the window thumbnails. Using large sizes may "
			  "cause performance problems." ) );
   QLabel *thumbSizeLabel = new QLabel( i18n("Thumbnail &Size: "), thumbSizeBox );
   percent = (int) (kasbar->thumbnailSize() * 100.0);
   thumbSizeSlider = new QSlider( 0, 100, 1, percent, Horizontal, thumbSizeBox );
   connect( thumbSizeSlider, SIGNAL( valueChanged( int ) ),
	    kasbar, SLOT( setThumbnailSize( int ) ) );
   thumbSizeLabel->setBuddy( thumbSizeSlider );

   QHBox *thumbUpdateBox = new QHBox( thumbsPage );
   thumbUpdateBox->setSpacing( spacingHint() );
   QWhatsThis::add( thumbUpdateBox,
		    i18n( "Controls the frequency with which the thumbnail of the active window "
			  "is updated. If the value is 0 then no updates will be performed.\n\n"
			  "Using small values may cause performance problems on slow machines." ) );
   QLabel *thumbUpdateLabel = new QLabel( i18n("&Update thumbnail every: "), thumbUpdateBox );
   thumbUpdateSpin = new QSpinBox( 0, 1000, 1, thumbUpdateBox );
   thumbUpdateSpin->setValue( kasbar->thumbnailUpdateDelay() );
   connect( thumbUpdateSpin, SIGNAL( valueChanged( int ) ),
   	    kasbar, SLOT( setThumbnailUpdateDelay( int ) ) );
   new QLabel( i18n("seconds"), thumbUpdateBox );
   thumbUpdateLabel->setBuddy( thumbUpdateSpin );

   new QWidget( thumbsPage, "spacer" );

   //
   // Behaviour settings
   //
   QVBox *behavePage = addVBoxPage( i18n("Behavior"), QString::null, Icon( "window_list" ) );

   notifierCheck = new QCheckBox( i18n("Enable &Startup Notifier"), behavePage );
   QWhatsThis::add( notifierCheck,
		    i18n( "Enables the display of tasks that are starting but have not yet "
			  "created a window." ) );
   notifierCheck->setChecked( kasbar->notifierEnabled() );
   connect( notifierCheck, SIGNAL( toggled(bool) ), kasbar, SLOT( setNotifierEnabled(bool) ) );

   modifiedCheck = new QCheckBox( i18n("Enable &Modified Indicator"), behavePage );
   QWhatsThis::add( modifiedCheck,
		    i18n( "Enables the display of a floppy disk state icon for windows containing "
			  "a modified document." ) );
   modifiedCheck->setChecked( kasbar->showModified() );
   connect( modifiedCheck, SIGNAL( toggled(bool) ), kasbar, SLOT( setShowModified(bool) ) );

   showAllWindowsCheck = new QCheckBox( i18n("Show All &Windows"), behavePage );
   QWhatsThis::add( showAllWindowsCheck,
		    i18n( "Enables the display of all windows, not just those on the current desktop." ) );
   showAllWindowsCheck->setChecked( kasbar->showAllWindows() );
   connect( showAllWindowsCheck, SIGNAL( toggled(bool) ), kasbar, SLOT( setShowAllWindows(bool) ) );

   groupWindowsCheck = new QCheckBox( i18n("&Group Windows"), behavePage );
   QWhatsThis::add( groupWindowsCheck,
		    i18n( "Enables the grouping together of related windows." ) );
   groupWindowsCheck->setChecked( kasbar->groupWindows() );
   connect( groupWindowsCheck, SIGNAL( toggled(bool) ), 
	    kasbar, SLOT( setGroupWindows(bool) ) );

   QHBox *maxBoxesBox = new QHBox( behavePage );
   QWhatsThis::add( maxBoxesBox,
		    i18n( "Specifies the maximum number of items that should be placed in a line "
			  "before starting a new row or column. If the value is 0 then all the "
			  "available space will be used." ) );
   QLabel *maxBoxesLabel = new QLabel( i18n("Ma&x Boxes: "), maxBoxesBox );

   conf->setGroup( "Layout" );
   maxBoxesSpin = new KIntSpinBox( 0, 50, 1,
				   conf->readNumEntry( "MaxBoxes", 0 ),
				   10,
				   maxBoxesBox, "maxboxes" );
   connect( maxBoxesSpin, SIGNAL( valueChanged( int ) ), kasbar, SLOT( setMaxBoxes( int ) ) );
   maxBoxesLabel->setBuddy( maxBoxesSpin );

   new QWidget( behavePage, "spacer" );

   resize( 410, 225 );
}

KasPrefsDialog::~KasPrefsDialog()
{

}

void KasPrefsDialog::accept()
{
   conf->setGroup("Appearance");
   conf->writeEntry( "ItemSize", kasbar->itemSize() );
   conf->writeEntry( "Transparent", kasbar->isTransparent() );
   conf->writeEntry( "EnableTint", kasbar->hasTint() );
   conf->writeEntry( "TintColor", kasbar->tintColor() );
   conf->writeEntry( "TintAmount", kasbar->tintAmount() );

   conf->setGroup("Thumbnails");
   conf->writeEntry( "Thumbnails", kasbar->thumbnailsEnabled() );
   conf->writeEntry( "ThumbnailSize", kasbar->thumbnailSize() );
   conf->writeEntry( "ThumbnailUpdateDelay", kasbar->thumbnailUpdateDelay() );

   conf->setGroup("Behaviour");
   conf->writeEntry( "StartupNotifier", kasbar->notifierEnabled() );
   conf->writeEntry( "ModifiedIndicator", kasbar->showModified() );
   conf->writeEntry( "ShowAllWindows", kasbar->showAllWindows() );
   conf->writeEntry( "GroupWindows", kasbar->groupWindows() );

   conf->setGroup("Layout");
   conf->writeEntry( "MaxBoxes", maxBoxesSpin->value() );

   conf->sync();

   QDialog::accept();
}

#include "kasprefsdlg.moc"
