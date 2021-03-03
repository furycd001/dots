/****************************************************************************
** $Id: qt/examples/qdir/qdir.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qapplication.h>
#include <qtextview.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qhbox.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qmultilineedit.h>
#include <qheader.h>
#include <qevent.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qtooltip.h>

#include "../dirview/dirview.h"
#include "qdir.h"

#include <stdlib.h>

/* XPM */
static const char *bookmarks[]={
    "22 14 8 1",
    "# c #000080",
    "a c #585858",
    "b c #000000",
    "c c #ffffff",
    "d c #ffffff",
    "e c #ffffff",
    "f c #000000",
    ". c None",
    "...bb.................",
    "..bacb....bbb.........",
    "..badcb.bbccbab.......",
    "..bacccbadccbab.......",
    "..baecdbcccdbab.......",
    "..bacccbacccbab.......",
    "..badcdbcecdfab.......",
    "..bacecbacccbab.......",
    "..baccdbcccdbab.......",
    "...badcbacdbbab.......",
    "....bacbcbbccab.......",
    ".....babbaaaaab.......",
    ".....bbabbbbbbb.......",
    "......bb.............."
};

/* XPM */
static const char *home[]={
    "16 15 4 1",
    "# c #000000",
    "a c #ffffff",
    "b c #c0c0c0",
    ". c None",
    ".......##.......",
    "..#...####......",
    "..#..#aabb#.....",
    "..#.#aaaabb#....",
    "..##aaaaaabb#...",
    "..#aaaaaaaabb#..",
    ".#aaaaaaaaabbb#.",
    "###aaaaaaaabb###",
    "..#aaaaaaaabb#..",
    "..#aaa###aabb#..",
    "..#aaa#.#aabb#..",
    "..#aaa#.#aabb#..",
    "..#aaa#.#aabb#..",
    "..#aaa#.#aabb#..",
    "..#####.######.."
};

// ****************************************************************************************************

PixmapView::PixmapView( QWidget *parent )
    : QScrollView( parent )
{
    viewport()->setBackgroundMode( PaletteBase );
}

void PixmapView::setPixmap( const QPixmap &pix )
{
    pixmap = pix;
    resizeContents( pixmap.size().width(), pixmap.size().height() );
    viewport()->repaint( FALSE );
}

void PixmapView::drawContents( QPainter *p, int cx, int cy, int cw, int ch )
{
    p->fillRect( cx, cy, cw, ch, colorGroup().brush( QColorGroup::Base ) );
    p->drawPixmap( 0, 0, pixmap );
}

// ****************************************************************************************************

Preview::Preview( QWidget *parent )
    : QWidgetStack( parent )
{
    normalText = new QMultiLineEdit( this );
    normalText->setReadOnly( TRUE );
    html = new QTextView( this );
    pixmap = new PixmapView( this );
    raiseWidget( normalText );
}

void Preview::showPreview( const QUrl &u, int size )
{
    if ( u.isLocalFile() ) {
	QString path = u.path();
	QFileInfo fi( path );
	if ( fi.isFile() && (int)fi.size() > size * 1000 ) {
	    normalText->setText( tr( "The File\n%1\nis too large, so I don't show it!" ).arg( path ) );
	    raiseWidget( normalText );
	    return;
	}
	
	QPixmap pix( path );
	if ( pix.isNull() ) {
	    if ( fi.isFile() ) {
		QFile f( path );
		if ( f.open( IO_ReadOnly ) ) {
		    QTextStream ts( &f );
		    QString text = ts.read();
		    f.close();
		    if ( fi.extension().lower().contains( "htm" ) ) {
			QString url = html->mimeSourceFactory()->makeAbsolute( path, html->context() );
			html->setText( text, url ); 	
			raiseWidget( html );
			return;
		    } else {
			normalText->setText( text ); 	
			raiseWidget( normalText );
			return;
		    }
		}
	    }
	    normalText->setText( QString::null );
	    raiseWidget( normalText );
	} else {
	    pixmap->setPixmap( pix );
	    raiseWidget( pixmap );
	}
    } else {
	normalText->setText( "I only show local files!" );
	raiseWidget( normalText );
    }
}

// ****************************************************************************************************

PreviewWidget::PreviewWidget( QWidget *parent )
    : QVBox( parent ), QFilePreview()
{
    setSpacing( 5 );
    setMargin( 5 );
    QHBox *row = new QHBox( this );
    row->setSpacing( 5 );
    (void)new QLabel( tr( "Only show files smaller than: " ), row );
    sizeSpinBox = new QSpinBox( 1, 10000, 1, row );
    sizeSpinBox->setSuffix( " KB" );
    sizeSpinBox->setValue( 64 );
    row->setFixedHeight( 10 + sizeSpinBox->sizeHint().height() );
    preview = new Preview( this );
}

void PreviewWidget::previewUrl( const QUrl &u )
{
    preview->showPreview( u, sizeSpinBox->value() );
}

// ****************************************************************************************************

CustomFileDialog::CustomFileDialog()
    :  QFileDialog( 0, 0, TRUE )
{
    setDir( "/" );

    dirView = new DirectoryView( this, 0, TRUE );
    dirView->addColumn( "" );
    dirView->header()->hide();
    ::Directory *root = new ::Directory( dirView, "/" );
    root->setOpen( TRUE );
    dirView->setFixedWidth( 150 );

    addLeftWidget( dirView );

    QPushButton *p = new QPushButton( this );
    p->setPixmap( QPixmap( bookmarks ) );
    QToolTip::add( p, tr( "Bookmarks" ) );

    bookmarkMenu = new QPopupMenu( this );
    connect( bookmarkMenu, SIGNAL( activated( int ) ),
	     this, SLOT( bookmarkChosen( int ) ) );
    addId = bookmarkMenu->insertItem( tr( "Add bookmark" ) );
    bookmarkMenu->insertSeparator();

    QFile f( ".bookmarks" );
    if ( f.open( IO_ReadOnly ) ) {
	QDataStream ds( &f );
	ds >> bookmarkList;
	f.close();
	
	QStringList::Iterator it = bookmarkList.begin();
	for ( ; it != bookmarkList.end(); ++it ) {
	    bookmarkMenu->insertItem( *it );
	}
    }
	
    p->setPopup( bookmarkMenu );

    addToolButton( p, TRUE );

    connect( dirView, SIGNAL( folderSelected( const QString & ) ),
	     this, SLOT( setDir2( const QString & ) ) );
    connect( this, SIGNAL( dirEntered( const QString & ) ),
	     dirView, SLOT( setDir( const QString & ) ) );

    QToolButton *b = new QToolButton( this );
    QToolTip::add( b, tr( "Go Home!" ) );
    b->setPixmap( QPixmap( home ) );
    connect( b, SIGNAL( clicked() ),
	     this, SLOT( goHome() ) );

    addToolButton( b );

    resize( width() + width() / 3, height() );
}

CustomFileDialog::~CustomFileDialog()
{
    if ( !bookmarkList.isEmpty() ) {
	QFile f( ".bookmarks" );
	if ( f.open( IO_WriteOnly ) ) {
	    QDataStream ds( &f );
	    ds << bookmarkList;
	    f.close();
	}
    }
}

void CustomFileDialog::setDir2( const QString &s )
{
    blockSignals( TRUE );
    setDir( s );
    blockSignals( FALSE );
}

void CustomFileDialog::showEvent( QShowEvent *e )
{
    QFileDialog::showEvent( e );
    dirView->setDir( dirPath() );
}

void CustomFileDialog::bookmarkChosen( int i )
{
    if ( i == addId ) {
	bookmarkList << dirPath();
	bookmarkMenu->insertItem( dirPath() );
    } else {
	setDir( bookmarkMenu->text( i ) );
    }
}

void CustomFileDialog::goHome()
{
    if ( getenv( "HOME" ) )
	setDir( getenv( "HOME" ) );
    else
	setDir( "/" );
}

// ****************************************************************************************************

int main( int argc, char ** argv )
{
    QFileDialog::Mode mode = QFileDialog::ExistingFile;
    QString start;
    QString filter;
    QString caption;
    bool preview = FALSE;
    bool custom = FALSE;
    QApplication a( argc, argv );
    for (int i=1; i<argc; i++) {
	QString arg = argv[i];
	if ( arg == "-any" )
	    mode = QFileDialog::AnyFile;
	else if ( arg == "-dir" )
	    mode = QFileDialog::Directory;
	else if ( arg == "-default" )
	    start = argv[++i];
	else if ( arg == "-filter" )
	    filter = argv[++i];
	else if ( arg == "-preview" )
	    preview = TRUE;
	else if ( arg == "-custom" )
	    custom = TRUE;
	else if ( arg[0] == '-' ) {
	    qDebug("Usage: qdir [-any | -dir | -custom] [-preview] [-default f] {-filter f} [caption ...]\n"
		   "      -any         Get any filename, need not exist.\n"
		   "      -dir         Return a directory rather than a file.\n"
		   "      -custom      Opens a customized QFileDialog with \n"
		   "                   dir browser, bookmark menu, etc.\n"
		   "      -preview     Show a preview widget.\n"
		   "      -default f   Start from directory/file f.\n"
		   "      -filter f    eg. '*.gif' '*.bmp'\n"
		   "      caption ...  Caption for dialog.\n"
		   );
	    return 1;
	} else {
	    if ( !caption.isNull() )
		caption += ' ';
	    caption += arg;
	}
    }

    if ( !start )
	start = QDir::currentDirPath();

    if ( !caption )
	caption = mode == QFileDialog::Directory
		    ? "Choose directory..." : "Choose file...";

    if ( !custom ) {
	QFileDialog fd( QString::null, filter, 0, 0, TRUE );
	fd.setMode( mode );
	if ( preview ) {
	    fd.setContentsPreviewEnabled( TRUE );
	    PreviewWidget *pw = new PreviewWidget( &fd );
	    fd.setContentsPreview( pw, pw );
	    fd.setViewMode( QFileDialog::List );
	    fd.setPreviewMode( QFileDialog::Contents );
	}
	fd.setCaption( caption );
	fd.setSelection( start );
	if ( fd.exec() == QDialog::Accepted ) {
	    QString result = fd.selectedFile();
	    printf("%s\n", (const char*)result);
	    return 0;
	} else {
	    return 1;
	}
    } else {
	CustomFileDialog fd;
	fd.exec();
	return 1;
    }
}
