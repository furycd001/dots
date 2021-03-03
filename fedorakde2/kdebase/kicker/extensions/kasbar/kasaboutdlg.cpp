#include <qvbox.h>
#include <qlabel.h>
#include <qfile.h>
#include <qtextstream.h>

#include <klocale.h>
#include <kstddirs.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <ktextbrowser.h>

#ifdef USE_KSPY
#include <kspy.h>
#endif

#include "kasbar.h"
#include "kasitem.h"
#include "kasaboutdlg.h"
#include "version.h"

#define Icon(x) KGlobal::instance()->iconLoader()->loadIcon( x, KIcon::NoGroup, KIcon::SizeMedium )

KasAboutDialog::KasAboutDialog( QWidget *parent )
   : KDialogBase( KDialogBase::IconList, i18n("About Kasbar"),
		  KDialogBase::Ok,
		  KDialogBase::Ok,
		  parent, "kasbarAboutDialog", true )
{
#ifdef USE_KSPY
  KSpy::invoke();
#endif

   //
   // General about info
   //
   QVBox *aboutPage = addVBoxPage( i18n("About"),
				   i18n("About Kasbar"), 
				   Icon( "appearance" ) );
   aboutPage->setSpacing( spacingHint() );

   QHBox *versionBox = new QHBox( aboutPage );
   versionBox->setSpacing( spacingHint() );

   KasBar *bar = new KasBar( Horizontal, versionBox );
   bar->append( new KasItem( bar ) );
   bar->setFixedSize( bar->itemExtent(), bar->itemExtent() );

   new QLabel( i18n( "<qt><body>"
		     "<h2>Kasbar Version: %1</h2>"
		     "<b>CVS Id:</b> %2"
		     "</body></qt>" )
	       .arg( VERSION_STRING ).arg( CVS_ID ),
	       versionBox );

   KTextBrowser *text5 = new KTextBrowser( aboutPage );
   text5->setText( i18n( "<html><body>"
			 "<p>Kasbar TNG began as a port of the original Kasbar applet to "
			 "the (then new) extension API, but ended up as a complete "
			 "rewrite because of the range of features needed by different "
			 "groups of users. In the process of the rewrite all the standard "
			 "features provided by the default taskbar were added, along with "
			 "some more original ones such as thumbnails."
			 "</p>"
			 "<p>"
			 "You can find infomation about the latest developments in Kasbar at "
			 "<a href=\"%3\">%4</a>, the Kasbar homepage."
			 "</p>"
			 "</body></html>" )
		   .arg( HOMEPAGE_URL ).arg( HOMEPAGE_URL ) );

   //
   // Authors info
   //
   QVBox *authorsPage = addVBoxPage( i18n("Authors"),
				     i18n("Kasbar Authors"), 
				     Icon( "kuser" ) );

   KTextBrowser *text = new KTextBrowser( authorsPage );
   text->setText( i18n(
     "<html>"

     "<b>Richard Moore</b> <a href=\"mailto:rich@kde.org\">rich@kde.org</a><br>"
     "<b>Homepage:</b> <a href=\"http://www.ipso-facto.demon.co.uk/\">http://www.ipso-facto.demon.co.uk/</a>"

     "<p>Developer and maintainer of the Kasbar TNG code.</p>"

     "<hr/>"

     "<b>Daniel M. Duley (Mosfet)</b> <a href=\"mailto:mosfet@kde.org\">mosfet@kde.org</a><br>"
     "<b>Homepage:</b> <a href=\"http://www.mosfet.org/\">http://www.mosfet.org/</a>"

     "<p>Mosfet wrote the original Kasbar applet on which this "
     "extension is based. There is little of the original code "
     "remaining, but the basic look in opaque mode is almost "
     "identical to this first implementation.</p>"

     "</html>" ) );

   //
   // BSD info
   //
   QVBox *bsdLicense = addVBoxPage( i18n("BSD License"), QString::null, Icon( "filefind" ) );

   new QLabel( i18n( "Kasbar may be used under the terms of either the BSD license, "
		     "or the GNU Public License." ), bsdLicense );

   KTextBrowser *text2 = new KTextBrowser( bsdLicense );
   text2->setText( "Some text of unsurpassed tediousness goes here." );

   QString bsdFile = locate("data", "LICENSES/BSD");
   if ( !bsdFile.isEmpty() ) {
     QString result;
     QFile file( bsdFile );

     if ( file.open( IO_ReadOnly ) )
     {
        QTextStream str(&file);
        result += str.read();
     }

     text2->setText( result );
   }

   //
   // GPL info
   //
   QVBox *gplPage = addVBoxPage( i18n("GPL License"), QString::null, Icon( "filefind" ) );

   new QLabel( i18n( "Kasbar may be used under the terms of either the BSD license, "
		     "or the GNU Public License." ), gplPage );

   KTextBrowser *text3 = new KTextBrowser( gplPage );
   text3->setText( "Some more text of unsurpassed tediousness goes here." );

   QString gplFile = locate("data", "LICENSES/GPL_V2");
   if ( !gplFile.isEmpty() ) {
     QString result;
     QFile file( gplFile );

     if ( file.open( IO_ReadOnly ) )
     {
        QTextStream str(&file);
        result += str.read();
     }

     text3->setText( result );
   }


   resize( 340, 280 );
}

KasAboutDialog::~KasAboutDialog()
{

}

#include "kasaboutdlg.moc"
