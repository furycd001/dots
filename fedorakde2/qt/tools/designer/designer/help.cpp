/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include <qvariant.h>  // HP-UX compiler needs this here

#include "help.h"
#include "helpdialogimpl.h"
#include "qaction.h"
#include "pixmapchooser.h"
#include "mainwindow.h"

#include <qpixmap.h>
#include <qpopupmenu.h>
#if defined(HAVE_KDE)
#include <kmenubar.h>
#include <ktoolbar.h>
#else
#include <qmenubar.h>
#include <qtoolbar.h>
#endif
#include <qiconset.h>
#include <qstylesheet.h>
#include <qprinter.h>
#include <qsimplerichtext.h>
#include <qpaintdevicemetrics.h>
#include <qtextbrowser.h>
#include <qmime.h>
#include <qurl.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <stdlib.h>

class TextBrowser : public QTextBrowser
{
public:
    TextBrowser( QWidget *parent = 0, const char *name = 0 ) : QTextBrowser( parent, name ) {}

    void setSource( const QString &name ) {
	QUrl u( context(), name );
	if ( !u.isLocalFile() ) {
	    QMessageBox::information( this, tr( "Help" ), tr( "Can't load and display non-local file\n"
							      "%1" ).arg( name ) );
	    return;
	}

	QTextBrowser::setSource( name );
    }

};

static QIconSet createIconSet( const QString &name )
{
    QIconSet ic( PixmapChooser::loadPixmap( name, PixmapChooser::Small ) );
    ic.setPixmap( PixmapChooser::loadPixmap( name, PixmapChooser::Disabled ), QIconSet::Small, QIconSet::Disabled );
    return ic;
}

Help::Help( const QString& home, MainWindow* parent, const char *name )
#if defined(HAVE_KDE)
    : KMainWindow( 0, name, WType_TopLevel | WDestructiveClose | WGroupLeader ),
#elif defined(QT_NON_COMMERICAL)
    : QMainWindow( parent, name, WType_TopLevel | WDestructiveClose | WGroupLeader ),
#else
    : QMainWindow( 0, name, WType_TopLevel | WDestructiveClose | WGroupLeader ),
#endif
      mainWindow( parent ), helpDialog( 0 )
{
    bookmarkMenu = 0;
    browser = new TextBrowser( this );
    browser->mimeSourceFactory()->setFilePath( home );
    browser->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    browser->mimeSourceFactory()->addFilePath( QString( getenv( "QTDIR" ) ) + "/tools/designer/manual" );
    browser->mimeSourceFactory()->addFilePath( QString( getenv( "QTDIR" ) ) + "/doc/html/designer" );
    browser->mimeSourceFactory()->addFilePath( parent->documentationPath() );
    connect( browser, SIGNAL( textChanged() ),
	     this, SLOT( textChanged() ) );

    setCentralWidget( browser );

#if defined(HAVE_KDE)
    toolbar = new KToolBar( this );
#else
    toolbar = new QToolBar( this );
#endif
    addToolBar( toolbar, tr( "Tools" ) );

    setupFileActions();
    setupGoActions();
    setupBookmarkMenu();

    resize( 750,700 );

    setToolBarsMovable( FALSE );
}

void Help::setupFileActions()
{
    QPopupMenu* menu = new QPopupMenu( this );
    menuBar()->insertItem( tr( "&File" ), menu );
    QAction *a;

    a = new QAction( tr( "Print" ), createIconSet( "print.xpm" ), tr( "&Print" ), CTRL + Key_P, this );
    a->addTo( menu );
    a->addTo( toolbar );
    connect( a, SIGNAL( activated() ), this, SLOT( filePrint() ) );

    a = new QAction( tr( "Close" ), tr( "&Close" ), 0, this );
    a->addTo( menu );
    connect( a, SIGNAL( activated() ), this, SLOT( close() ) );
}

void Help::setupGoActions()
{
    QPopupMenu* menu = new QPopupMenu( this );
    menuBar()->insertItem( tr( "&Go" ), menu );
    QAction *a;

    a = new QAction( tr( "Home" ), createIconSet( "home.xpm" ), ( "&Home" ), ALT + Key_Home, this );
    a->addTo( menu );
    a->addTo( toolbar );
    connect( a, SIGNAL( activated() ), this, SLOT( goHome() ) );

    a = new QAction( tr( "Qt Reference Documentation" ), createIconSet( "customwidget.xpm" ),
		     ( "&Qt Reference Documentation" ), ALT + SHIFT + Key_Home, this );
    a->addTo( menu );
    a->addTo( toolbar );
    connect( a, SIGNAL( activated() ), this, SLOT( goQt() ) );

    menu->insertSeparator();

    a = new QAction( tr( "Backward" ), createIconSet( "left.xpm" ), ( "&Backward" ), ALT + Key_Left, this );
    a->addTo( menu );
    a->addTo( toolbar );
    connect( a, SIGNAL( activated() ), browser, SLOT( backward() ) );
    connect( browser, SIGNAL( backwardAvailable( bool ) ), a, SLOT( setEnabled( bool ) ) );

    a = new QAction( tr( "Forward" ), createIconSet( "right.xpm" ), tr( "&Forward" ), ALT + Key_Right, this );
    a->addTo( menu );
    a->addTo( toolbar );
    connect( a, SIGNAL( activated() ), browser, SLOT( forward() ) );
    connect( browser, SIGNAL( forwardAvailable( bool ) ), a, SLOT( setEnabled( bool ) ) );

    menu->insertSeparator();

    a = new QAction( tr( "Topics/Index" ), createIconSet( "help.xpm" ), tr( "&Topics/Index..." ), CTRL + Key_T, this );
    a->addTo( menu );
    a->addTo( toolbar );
    connect( a, SIGNAL( activated() ), this, SLOT( goTopics() ) );
}


void Help::setSource( const QString& s )
{
    browser->setSource( s );
}

void Help::textChanged()
{
    if ( browser->documentTitle().isNull() )
	setCaption( tr( "Help: " ) + browser->context() );
    else
	setCaption( tr( "Help: " ) + browser->documentTitle() );
}

Help::~Help()
{
}

void Help::filePrint()
{
    QPrinter printer;
    printer.setFullPage(TRUE);
    if ( printer.setup() ) {
	QPainter p( &printer );
	QPaintDeviceMetrics metrics(p.device());
	int dpix = metrics.logicalDpiX();
	int dpiy = metrics.logicalDpiY();
	const int margin = 72; // pt
	QRect body(margin*dpix/72, margin*dpiy/72,
		   metrics.width()-margin*dpix/72*2,
		   metrics.height()-margin*dpiy/72*2 );
	QFont font("times", 10);
	QSimpleRichText richText( browser->text(), font, browser->context(), browser->styleSheet(),
				  browser->mimeSourceFactory(), body.height() );
	richText.setWidth( &p, body.width() );
	QRect view( body );
	int page = 1;
	for (;;) {
	    richText.draw( &p, body.left(), body.top(), view, colorGroup() );
	    view.moveBy( 0, body.height() );
	    p.translate( 0 , -body.height() );
	    p.setFont( font );
	    p.drawText( view.right() - p.fontMetrics().width( QString::number(page) ),
			view.bottom() + p.fontMetrics().ascent() + 5, QString::number(page) );
	    if ( view.top()  >= richText.height() )
		break;
	    printer.newPage();
	    page++;
	}
    }
}

void Help::goTopics()
{
    if ( !helpDialog ) {
	helpDialog  = new HelpDialog( this, mainWindow, this );
	connect( helpDialog, SIGNAL( showLink( const QString &, const QString & ) ),
		 this, SLOT( showLink( const QString &, const QString & ) ) );
    }
    helpDialog->show();
    helpDialog->raise();
}

void Help::showLink( const QString &link, const QString &title )
{
    browser->setCaption( title );
    browser->setSource( link );
}

void Help::setupBookmarkMenu()
{
    if ( !helpDialog ) {
	helpDialog  = new HelpDialog( this, mainWindow, this );
	connect( helpDialog, SIGNAL( showLink( const QString &, const QString & ) ),
		 this, SLOT( showLink( const QString &, const QString & ) ) );
    }

    if ( !bookmarkMenu ) {
	bookmarkMenu = new QPopupMenu( this );
	menuBar()->insertItem( tr( "&Bookmarks" ), bookmarkMenu );
	connect( bookmarkMenu, SIGNAL( activated( int ) ),
		 this, SLOT( showBookmark( int ) ) );
    }

    bookmarkMenu->clear();
    bookmarks.clear();
    bookmarkMenu->insertItem( tr( "&Add Bookmark" ), helpDialog, SLOT( addBookmark() ) );

    QFile f( QDir::homeDirPath() + "/.designer/bookmarks" );
    if ( !f.open( IO_ReadOnly ) )
	return;
    QTextStream ts( &f );
    bookmarkMenu->insertSeparator();
    while ( !ts.atEnd() ) {
	QString title = ts.readLine();
	QString link = ts.readLine();
	bookmarks.insert( bookmarkMenu->insertItem( title ), link );
    }
}

void Help::showBookmark( int id )
{
    if ( bookmarks.find( id ) != bookmarks.end() )
	showLink( *bookmarks.find( id ), bookmarkMenu->text( id ) );
}

void Help::goHome()
{
    showLink( "book1.html", tr( "Home" ) );
}

void Help::goQt()
{
    showLink( "index.html", tr( "Qt Reference Documentation" ) );
}
