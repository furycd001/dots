#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kconfig.h>

#include <kmessagebox.h>

#include "kastasker.h"
#include "kasprefsdlg.h"
#include "kasaboutdlg.h"

#include "kasbarextension.h"
#include "version.h"

extern "C"
{
   KPanelExtension *init( QWidget *parent, const QString& configFile )
   {
      KGlobal::locale()->insertCatalogue( "kasbarextension" );
      return new KasBarExtension( configFile,
				  KPanelExtension::Normal,
				  KPanelExtension::About | KPanelExtension::Preferences,
				  parent, "kasbarextension");
   }
}

KasBarExtension::KasBarExtension( const QString& configFile,
                                  Type type,
                                  int actions,
                                  QWidget *parent, const char *name )
   : KPanelExtension( configFile, type, actions, parent, name )
{
   setBackgroundMode( NoBackground );
   kasbar = new KasTasker( orientation(), this, name );
   connect( kasbar, SIGNAL( layoutChanged() ), this, SIGNAL( updateLayout() ) );

   updateConfig();
   kasbar->refreshAll();
   kasbar->updateLayout();
   repaint( true );
}

KasBarExtension::~KasBarExtension()
{
}

void KasBarExtension::resizeEvent(QResizeEvent*)
{
   kasbar->setOrientation( orientation() );
   kasbar->setGeometry(0, 0, width(), height());
}

QSize KasBarExtension::sizeHint(Position p, QSize maxSize ) const
{
   Orientation o = Horizontal;

   if ( p == Left || p == Right )
      o = Vertical;

   return kasbar->sizeHint( o, maxSize );
}

void KasBarExtension::positionChange( Position /* position */)
{
   kasbar->setOrientation( orientation() );
   kasbar->updateLayout();
   kasbar->refreshIconGeometry();
}

void KasBarExtension::about()
{
  KasAboutDialog *dlg = new KasAboutDialog( this );
  dlg->exec();
  delete dlg;
}

void KasBarExtension::preferences()
{
   KasPrefsDialog *dlg = new KasPrefsDialog( kasbar, config() );
   dlg->exec();
   delete dlg;

   updateConfig();
}

void KasBarExtension::updateConfig()
{
   KConfig *conf = config();

   //
   // Appearance Settings.
   //
   conf->setGroup("Appearance");

   kasbar->setItemSize( conf->readNumEntry( "ItemSize", KasBar::Medium ) );
   kasbar->setTint( conf->readBoolEntry( "EnableTint", false ) );
   kasbar->setTintColor( conf->readColorEntry( "TintColor", &Qt::black ) );
   kasbar->setTintAmount( conf->readDoubleNumEntry( "TintAmount", 0.1 ) );
   kasbar->setTransparent( conf->readBoolEntry( "Transparent", true ) );

   // paint active bg
   // paint inactive bg
   // active overlay color
   // inactive overlay color

   //
   // Thumbnail Settings
   //
   conf->setGroup("Thumbnails");
   kasbar->setThumbnailsEnabled( conf->readBoolEntry( "Thumbnails", true ) );
   kasbar->setThumbnailSize( conf->readDoubleNumEntry( "ThumbnailSize", 0.2 ) );
   kasbar->setThumbnailUpdateDelay( conf->readNumEntry( "ThumbnailUpdateDelay", 10 ) );

   //
   // Behaviour Settings
   //
   conf->setGroup("Behaviour");
   kasbar->setNotifierEnabled( conf->readBoolEntry( "StartupNotifier", true ) );
   kasbar->setShowModified( conf->readBoolEntry( "ModifiedIndicator", true ) );
   kasbar->setShowAllWindows( conf->readBoolEntry( "ShowAllWindows", true ) );
   kasbar->setGroupWindows( conf->readBoolEntry( "GroupWindows", true ) );

   //
   // Layout Settings
   //
   conf->setGroup("Layout");
   kasbar->setMaxBoxes( conf->readUnsignedNumEntry( "MaxBoxes", 0 ) );

   //    fillBg = conf->readBoolEntry( "FillIconBackgrounds", /*true*/ false );
   //    fillActiveBg = conf->readBoolEntry( "FillActiveIconBackground", true );
   //    enablePopup = conf->readBoolEntry( "EnablePopup", true );
}

#include "kasbarextension.moc"
