/****************************************************************************
** $Id: qt/examples/fileiconview/qfileiconview.cpp   2.3.2   edited 2001-10-17 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "qfileiconview.h"

#include <qstringlist.h>
#include <qpixmap.h>
#include <qmime.h>
#include <qstrlist.h>
#include <qdragobject.h>
#include <qmessagebox.h>
#include <qevent.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qapplication.h>
#include <qwmatrix.h>

#include <stdlib.h>

static const char * file_icon[]={
    "32 32 17 1",
    "# c #000000",
    "a c #ffffff",
    "j c #808080",
    "n c #a0a0a4",
    "g c #c0c0c0",
    "m c #004000",
    "o c #000000",
    "l c #004040",
    "k c #404000",
    "i c #c0c000",
    "h c #ffff00",
    "b c #ffffc0",
    "e c #ff8000",
    "f c #c05800",
    "c c #ffa858",
    "d c #ffdca8",
    ". c None",
    "................................",
    "................................",
    "................................",
    "................................",
    ".............#....###...........",
    "...###......#a##.#aba##.........",
    "..#cdb#....#aaaa#aaaaaa##.......",
    "..#ecdb#..#aaaa#aaaaaaaba##.....",
    "..#fecdb##aaaa#aaaaaaaaaaab##...",
    "...#fecdb#aaa#aaaaaaabaabaaaa##.",
    "....#fecdb#a#baaaaa#baaaaaabaaa#",
    ".....#fecdb#aaaaab#a##baaaaaaa#.",
    ".....##fecdb#bbba#aaaa##baaab#..",
    "....#bb#fecdb#ba#aaaaaaa##aa#...",
    "...#bbbb#fecdb##aaabaaaaaa##....",
    "..#bbbb#b#fecdb#aaaaaaabaaaa##..",
    ".#bbbb#bbb#fecdg#aaaaaaaaaaaba#.",
    "#hhbb#bbbbb#fegg#iiaaaaaaaaaaaa#",
    "#jhhhklibbbk#ggj#aaiiaaaaaaaaa#j",
    ".#mjhhhkmikab####aaabiiaaaaaa#j.",
    "...##jhhhmaaibbaaiibaaaiiaab#n..",
    ".....##j#baaaiiabaaiibaabaa#n...",
    "......##baibaabiibaaaiiabb#j....",
    "......#bbbbiiaabbiiaaaaabon.....",
    ".....#bbbbbbbiiabbaiiaab#n......",
    ".....#jbbbbbbbbiibaabba#n.......",
    "......##jbbbbbbbbiiaabmj........",
    "........##jbbbbbbbbbb#j.........",
    "..........##nbbbbbbbmj..........",
    "............##jbbbb#j...........",
    "..............#mjj#n............",
    "................##n............."};

static const char * folder_icon[]={
    "32 32 11 1",
    "# c #000000",
    "b c #c0c000",
    "d c #585858",
    "a c #ffff00",
    "i c #400000",
    "h c #a0a0a4",
    "e c #000000",
    "c c #ffffff",
    "f c #303030",
    "g c #c0c0c0",
    ". c None",
    "...###..........................",
    "...#aa##........................",
    ".###baaa##......................",
    ".#cde#baaa##....................",
    ".#cccdeebaaa##..##f.............",
    ".#cccccdeebaaa##aaa##...........",
    ".#cccccccdeebaaaaaaaa##.........",
    ".#cccccccccdeebababaaa#.........",
    ".#cccccgcgghhebbbbbbbaa#........",
    ".#ccccccgcgggdebbbbbbba#........",
    ".#cccgcgcgcgghdeebiebbba#.......",
    ".#ccccgcggggggghdeddeeba#.......",
    ".#cgcgcgcggggggggghghdebb#......",
    ".#ccgcggggggggghghghghd#b#......",
    ".#cgcgcggggggggghghghhd#b#......",
    ".#gcggggggggghghghhhhhd#b#......",
    ".#cgcggggggggghghghhhhd#b#......",
    ".#ggggggggghghghhhhhhhdib#......",
    ".#gggggggggghghghhhhhhd#b#......",
    ".#hhggggghghghhhhhhhhhd#b#......",
    ".#ddhhgggghghghhhhhhhhd#b#......",
    "..##ddhhghghhhhhhhhhhhdeb#......",
    "....##ddhhhghhhhhhhhhhd#b#......",
    "......##ddhhhhhhhhhhhhd#b#......",
    "........##ddhhhhhhhhhhd#b#......",
    "..........##ddhhhhhhhhd#b#......",
    "............##ddhhhhhhd#b###....",
    "..............##ddhhhhd#b#####..",
    "................##ddhhd#b######.",
    "..................##dddeb#####..",
    "....................##d#b###....",
    "......................####......"};


static const char * link_icon[]={
    "32 32 12 1",
    "# c #000000",
    "h c #a0a0a4",
    "b c #c00000",
    "d c #585858",
    "i c #400000",
    "c c #ffffff",
    "e c #000000",
    "g c #c0c0c0",
    "a c #ff0000",
    "f c #303030",
    "n c white",
    ". c None",
    "...###..........................",
    "...#aa##........................",
    ".###baaa##......................",
    ".#cde#baaa##....................",
    ".#cccdeebaaa##..##f.............",
    ".#cccccdeebaaa##aaa##...........",
    ".#cccccccdeebaaaaaaaa##.........",
    ".#cccccccccdeebababaaa#.........",
    ".#cccccgcgghhebbbbbbbaa#........",
    ".#ccccccgcgggdebbbbbbba#........",
    ".#cccgcgcgcgghdeebiebbba#.......",
    ".#ccccgcggggggghdeddeeba#.......",
    ".#cgcgcgcggggggggghghdebb#......",
    ".#ccgcggggggggghghghghd#b#......",
    ".#cgcgcggggggggghghghhd#b#......",
    ".#gcggggggggghghghhhhhd#b#......",
    ".#cgcggggggggghghghhhhd#b#......",
    ".#ggggggggghghghhhhhhhdib#......",
    ".#gggggggggghghghhhhhhd#b#......",
    ".#hhggggghghghhhhhhhhhd#b#......",
    ".#ddhhgggghghghhhhhhhhd#b#......",
    "..##ddhhghghhhhhhhhhhhdeb#......",
    "############hhhhhhhhhhd#b#......",
    "#nnnnnnnnnn#hhhhhhhhhhd#b#......",
    "#nnnnnnnnnn#hhhhhhhhhhd#b#......",
    "#nn#nn#nnnn#ddhhhhhhhhd#b#......",
    "#nn##n##nnn###ddhhhhhhd#b###....",
    "#nnn#####nn#..##ddhhhhd#b#####..",
    "#nnnnn##nnn#....##ddhhd#b######.",
    "#nnnnn#nnnn#......##dddeb#####..",
    "#nnnnnnnnnn#........##d#b###....",
    "############..........####......"};

static const char * folder_locked_icon[]={
    "32 32 12 1",
    "# c #000000",
    "g c #808080",
    "h c #c0c0c0",
    "f c #c05800",
    "c c #ffffff",
    "d c #585858",
    "b c #ffa858",
    "a c #ffdca8",
    "e c #000000",
    "i c #a0a0a4",
    "j c #c0c0c0",
    ". c None",
    "...###..........................",
    "...#aa##........................",
    ".###baaa##......................",
    ".#cde#baaa##....................",
    ".#cccdeeba#######...............",
    ".#cccccde##fffff##..............",
    ".#cccccc##fffgggg#..............",
    ".#ccccccc#ffg####a##............",
    ".#ccccchc#ffg#eebbaa##..........",
    ".#ccccccc#ffg#ddeebbba##........",
    ".#ccchccc#ffg#ihddeebbba##......",
    ".#cccccaa#ffg#ihhhddeeba##......",
    ".#chchhbbaafg#ihhhihidebb#......",
    ".#cchccbbbbaa#ihhihihid#b#......",
    ".#chchhbb#bbbaaiihihiid#b#......",
    ".#hchhcbb#fbbbafhiiiiid#b#......",
    ".#chchhbb#ffgbbfihiiiid#b#......",
    ".#hhhhhbb#ffg#bfiiiiiid#b#......",
    ".#hhhhhbbaffg#bfiiiiiid#b#......",
    ".#iihhhjbbaab#bfiiiiiid#b#......",
    ".#ddiihhh#bbbabfiiiiiid#b#......",
    "..##ddiih#ffbbbfiiiiiid#b#......",
    "....##ddi#ffg#biiiiiiid#b#......",
    "......##d#ffg#iiiiiiiid#b#......",
    "........##ffg#iiiiiiiid#b#......",
    ".........#ffg#iiiiiiiid#b#......",
    ".........#ffg#ddiiiiiid#b###....",
    ".........##fg###ddiiiid#b#####..",
    "...........####.##ddiid#b######.",
    "..................##dddeb#####..",
    "....................##d#b###....",
    "......................####......"};

static QPixmap *iconFolderLockedLarge = 0;
static QPixmap *iconFolderLarge = 0;
static QPixmap *iconFileLarge = 0;
static QPixmap *iconLinkLarge = 0;
static QPixmap *iconFolderLockedSmall = 0;
static QPixmap *iconFolderSmall = 0;
static QPixmap *iconFileSmall = 0;
static QPixmap *iconLinkSmall = 0;

static void cleanup()
{
    delete iconFolderLockedLarge;
    iconFolderLockedLarge = 0;
    delete iconFolderLarge;
    iconFolderLarge = 0;
    delete iconFileLarge;
    iconFileLarge = 0;
    delete iconLinkLarge;
    iconLinkLarge = 0;
    delete iconFolderLockedSmall;
    iconFolderLockedSmall = 0;
    delete iconFolderSmall;
    iconFolderSmall = 0;
    delete iconFileSmall;
    iconFileSmall = 0;
    delete iconLinkSmall;
    iconLinkSmall = 0;
}

/*****************************************************************************
 *
 * Class QtFileIconDrag
 *
 *****************************************************************************/

QtFileIconDrag::QtFileIconDrag( QWidget * dragSource, const char* name )
    : QIconDrag( dragSource, name )
{
}

const char* QtFileIconDrag::format( int i ) const
{
    if ( i == 0 )
	return "application/x-qiconlist";
    else if ( i == 1 )
	return "text/uri-list";
    else
	return 0;
}

QByteArray QtFileIconDrag::encodedData( const char* mime ) const
{
    QByteArray a;
    if ( QString( mime ) == "application/x-qiconlist" ) {
	a = QIconDrag::encodedData( mime );
    } else if ( QString( mime ) == "text/uri-list" ) {
	QString s = urls.join( "\r\n" );
	a.resize( s.length() );
	memcpy( a.data(), s.latin1(), s.length() );
    }
    return a;
}

bool QtFileIconDrag::canDecode( QMimeSource* e )
{
    return e->provides( "application/x-qiconlist" ) ||
	e->provides( "text/uri-list" );
}

void QtFileIconDrag::append( const QIconDragItem &item, const QRect &pr,
			     const QRect &tr, const QString &url )
{
    QIconDrag::append( item, pr, tr );
    urls << url;
}

/*****************************************************************************
 *
 * Class QtFileIconViewItem
 *
 *****************************************************************************/

QtFileIconViewItem::QtFileIconViewItem( QtFileIconView *parent, QFileInfo *fi )
    : QIconViewItem( parent, fi->fileName() ), itemFileName( fi->filePath() ),
      itemFileInfo( fi ), checkSetText( FALSE )
{
    vm = QtFileIconView::Large;

    if ( itemFileInfo->isDir() )
	itemType = Dir;
    else if ( itemFileInfo->isFile() )
	itemType = File;
    if ( itemFileInfo->isSymLink() )
	itemType = Link;

    viewModeChanged( ( (QtFileIconView*)iconView() )->viewMode() );

    if ( itemFileInfo->fileName() == "." ||
	 itemFileInfo->fileName() == ".." )
	setRenameEnabled( FALSE );

    checkSetText = TRUE;

    QObject::connect( &timer, SIGNAL( timeout() ),
		      iconView(), SLOT( openFolder() ) );
}

void QtFileIconViewItem::paintItem( QPainter *p, const QColorGroup &cg )
{
    if ( itemFileInfo->isSymLink() ) {
	QFont f( p->font() );
	f.setItalic( TRUE );
	p->setFont( f );
    }

    QIconViewItem::paintItem( p, cg );
}

void QtFileIconViewItem::viewModeChanged( QtFileIconView::ViewMode m )
{
    vm = m;
    setDropEnabled( itemType == Dir && QDir( itemFileName ).isReadable() );
    calcRect();
}

QPixmap *QtFileIconViewItem::pixmap() const
{
    switch ( itemType ) {
    case Dir: {
	if ( !QDir( itemFileName ).isReadable() ) {
	    if ( vm == QtFileIconView::Small )
		return iconFolderLockedSmall;
	    else
		return iconFolderLockedLarge;
	} else {
	    if ( vm == QtFileIconView::Small )
		return iconFolderSmall;
	    else
		return iconFolderLarge;
	}
    }
    case Link: {
	    if ( vm == QtFileIconView::Small )
		return iconLinkSmall;
	    else
		return iconLinkLarge;
    }
    default: {
	    if ( vm == QtFileIconView::Small )
		return iconFileSmall;
	    else
		return iconFileLarge;
    }
    }
}

QtFileIconViewItem::~QtFileIconViewItem()
{
    delete itemFileInfo;
}

void QtFileIconViewItem::setText( const QString &text )
{
    if ( checkSetText ) {
	if ( text == "." || text == "." || text.isEmpty() )
	    return;
	QDir dir( itemFileInfo->dir() );
	if ( dir.rename( itemFileInfo->fileName(), text ) ) {
	    itemFileName = itemFileInfo->dirPath( TRUE ) + "/" + text;
	    delete itemFileInfo;
	    itemFileInfo = new QFileInfo( itemFileName );
	    QIconViewItem::setText( text );
	}
    } else {
	QIconViewItem::setText( text );
    }
}

bool QtFileIconViewItem::acceptDrop( const QMimeSource *e ) const
{
    if ( type() == Dir && e->provides( "text/uri-list" ) &&
	 dropEnabled() )
	return TRUE;

    return FALSE;
}

void QtFileIconViewItem::dropped( QDropEvent *e, const QValueList<QIconDragItem> & )
{
    timer.stop();

    if ( !QUriDrag::canDecode( e ) ) {
	e->ignore();
	return;
    }

    QStrList lst;
    QUriDrag::decode( e, lst );

    QString str;
    if ( e->action() == QDropEvent::Copy )
	str = "Copy\n\n";
    else
	str = "Move\n\n";
    for ( uint i = 0; i < lst.count(); ++i )
	str += QString( "   %1\n" ).arg( lst.at( i ) );
    str += QString( "\n"
		    "To\n\n"
		    "	%1" ).arg( filename() );

    QMessageBox::information( iconView(), e->action() == QDropEvent::Copy ? "Copy" : "Move" , str, "Not Implemented" );
    if ( e->action() == QDropEvent::Move )
	QMessageBox::information( iconView(), "Remove" , str, "Not Implemented" );
    e->acceptAction();
}

void QtFileIconViewItem::dragEntered()
{
    if ( type() != Dir ||
	 type() == Dir && !QDir( itemFileName ).isReadable() )
	return;

    ( (QtFileIconView*)iconView() )->setOpenItem( this );
    timer.start( 1500 );
}

void QtFileIconViewItem::dragLeft()
{
    if ( type() != Dir ||
	 type() == Dir && !QDir( itemFileName ).isReadable() )
	return;

    timer.stop();
}

/*****************************************************************************
 *
 * Class QtFileIconView
 *
 *****************************************************************************/

QtFileIconView::QtFileIconView( const QString &dir, QWidget *parent, const char *name )
    : QIconView( parent, name ), viewDir( dir ), newFolderNum( 0 )
{
    if ( !iconFolderLockedLarge ) {
	qAddPostRoutine( cleanup );
	QWMatrix m;
	m.scale( 0.6, 0.6 );
	QPixmap iconpix( folder_locked_icon );
	iconFolderLockedLarge = new QPixmap( folder_locked_icon );
	iconpix = iconpix.xForm( m );
	iconFolderLockedSmall = new QPixmap( iconpix );
	iconpix = QPixmap( folder_icon );
	iconFolderLarge = new QPixmap( folder_icon );
	iconpix = iconpix.xForm( m );
	iconFolderSmall = new QPixmap( iconpix );
	iconpix = QPixmap( file_icon );
	iconFileLarge = new QPixmap( file_icon );
	iconpix = iconpix.xForm( m );
	iconFileSmall = new QPixmap( iconpix );
	iconpix = QPixmap( link_icon );
	iconLinkLarge = new QPixmap( link_icon );
	iconpix = iconpix.xForm( m );
	iconLinkSmall = new QPixmap( iconpix );
    }

    vm = Large;

    setGridX( 75 );
    setResizeMode( Adjust );
    setWordWrapIconText( FALSE );

    connect( this, SIGNAL( doubleClicked( QIconViewItem * ) ),
	     this, SLOT( itemDoubleClicked( QIconViewItem * ) ) );
    connect( this, SIGNAL( returnPressed( QIconViewItem * ) ),
	     this, SLOT( itemDoubleClicked( QIconViewItem * ) ) );
    connect( this, SIGNAL( dropped( QDropEvent *, const QValueList<QIconDragItem> & ) ),
	     this, SLOT( slotDropped( QDropEvent *, const QValueList<QIconDragItem> & ) ) );
    connect( this, SIGNAL( rightButtonPressed( QIconViewItem *, const QPoint & ) ),
	     this, SLOT( slotRightPressed( QIconViewItem * ) ) );

    setHScrollBarMode( AlwaysOff );
    setVScrollBarMode( Auto );

    setAutoArrange( TRUE );
    setSorting( TRUE );
    openItem = 0;
}

void QtFileIconView::openFolder()
{
    if ( !openItem )
	return;
    if ( openItem->type() != QtFileIconViewItem::Dir ||
	 openItem->type() == QtFileIconViewItem::Dir &&
	 !QDir( openItem->itemFileName ).isReadable() )
	return;

    openItem->timer.stop();
    setDirectory( openItem->itemFileName );
}

void QtFileIconView::setDirectory( const QString &dir )
{
    viewDir = QDir( dir );
    readDir( viewDir );
}

void QtFileIconView::setDirectory( const QDir &dir )
{
    viewDir = dir;
    readDir( viewDir );
}

void QtFileIconView::newDirectory()
{
    setAutoArrange( FALSE );
    selectAll( FALSE );
    if ( viewDir.mkdir( QString( "New Folder %1" ).arg( ++newFolderNum ) ) ) {
	QFileInfo *fi = new QFileInfo( viewDir, QString( "New Folder %1" ).arg( newFolderNum ) );
	QtFileIconViewItem *item = new QtFileIconViewItem( this, new QFileInfo( *fi ) );
	item->setKey( QString( "000000%1" ).arg( fi->fileName() ) );
	delete fi;
	repaintContents( contentsX(), contentsY(), contentsWidth(), contentsHeight(), FALSE );
	ensureItemVisible( item );
	item->setSelected( TRUE, TRUE );
	setCurrentItem( item );
	repaintItem( item );
	qApp->processEvents();
	item->rename();
    }
    setAutoArrange( TRUE );
}

QDir QtFileIconView::currentDir()
{
    return viewDir;
}

static bool isRoot( const QString &s )
{
#if defined(_OS_UNIX_)
    if ( s == "/" )
	return TRUE;
#elif defined(_OS_WIN32_)
    QString p = s;
    if ( p.length() == 3 &&
	 p.right( 2 ) == ":/" )
	return TRUE;
    if ( p[ 0 ] == '/' && p[ 1 ] == '/' ) {
	int slashes = p.contains( '/' );
	if ( slashes <= 3 )
	    return TRUE;
	if ( slashes == 4 && p[ (int)p.length() - 1 ] == '/' )
	    return TRUE;
    }
#endif

    return FALSE;
}

void QtFileIconView::readDir( const QDir &dir )
{
    if ( !dir.isReadable() )
	return;

    if ( isRoot( dir.absPath() ) )
	emit disableUp();
    else
	emit enableUp();

    clear();

    emit directoryChanged( dir.absPath() );

    const QFileInfoList *filist = dir.entryInfoList( QDir::DefaultFilter, QDir::DirsFirst | QDir::Name );

    emit startReadDir( filist->count() );

    QFileInfoListIterator it( *filist );
    QFileInfo *fi;
    bool allowRename = FALSE, allowRenameSet = FALSE;
    while ( ( fi = it.current() ) != 0 ) {
	++it;
	if ( fi && fi->fileName() == ".." && ( fi->dirPath() == "/" || fi->dirPath().isEmpty() ) )
	    continue;
	emit readNextDir();
	QtFileIconViewItem *item = new QtFileIconViewItem( this, new QFileInfo( *fi ) );
	if ( fi->isDir() )
	    item->setKey( QString( "000000%1" ).arg( fi->fileName() ) );
	else
	    item->setKey( fi->fileName() );
	if ( !allowRenameSet ) {
	    if ( !QFileInfo( fi->absFilePath() ).isWritable() ||
		 item->text() == "." || item->text() == ".." )
		allowRename = FALSE;
	    else
		allowRename = TRUE;
	    if ( item->text() == "." || item->text() == ".." )
		allowRenameSet = FALSE;
	    else
		allowRenameSet = TRUE;
	}
	item->setRenameEnabled( allowRename );
    }

    if ( !QFileInfo( dir.absPath() ).isWritable() )
	emit disableMkdir();
    else
	emit enableMkdir();

    emit readDirDone();
}

void QtFileIconView::itemDoubleClicked( QIconViewItem *i )
{
    QtFileIconViewItem *item = ( QtFileIconViewItem* )i;

    if ( item->type() == QtFileIconViewItem::Dir ) {
	viewDir = QDir( item->filename() );
	readDir( viewDir );
    } else if ( item->type() == QtFileIconViewItem::Link &&
		QFileInfo( QFileInfo( item->filename() ).readLink() ).isDir() ) {
	viewDir = QDir( QFileInfo( item->filename() ).readLink() );
	readDir( viewDir );
    }
}

QDragObject *QtFileIconView::dragObject()
{
    if ( !currentItem() )
	return 0;

    QPoint orig = viewportToContents( viewport()->mapFromGlobal( QCursor::pos() ) );
    QtFileIconDrag *drag = new QtFileIconDrag( viewport() );
    drag->setPixmap( *currentItem()->pixmap(),
 		     QPoint( currentItem()->pixmapRect().width() / 2, currentItem()->pixmapRect().height() / 2 ) );
    for ( QtFileIconViewItem *item = (QtFileIconViewItem*)firstItem(); item;
	  item = (QtFileIconViewItem*)item->nextItem() ) {
	if ( item->isSelected() ) {
	    QIconDragItem id;
	    id.setData( QCString( item->filename() ) );
	    drag->append( id,
			  QRect( item->pixmapRect( FALSE ).x() - orig.x(),
				 item->pixmapRect( FALSE ).y() - orig.y(),
				 item->pixmapRect().width(), item->pixmapRect().height() ),
			  QRect( item->textRect( FALSE ).x() - orig.x(),
				 item->textRect( FALSE ).y() - orig.y(),
				 item->textRect().width(), item->textRect().height() ),
			  QString( item->filename() ) );
	}
    }

    return drag;
}

void QtFileIconView::keyPressEvent( QKeyEvent *e )
{
    if ( e->key() == Key_N &&
	 ( e->state() & ControlButton ) )
	newDirectory();
    else
	QIconView::keyPressEvent( e );
}

void QtFileIconView::slotDropped( QDropEvent *e, const QValueList<QIconDragItem> & )
{
    if ( openItem )
	openItem->timer.stop();
    if ( !QUriDrag::canDecode( e ) ) {
	e->ignore();
	return;
    }

    QStrList lst;
    QUriDrag::decode( e, lst );

    QString str;
    if ( e->action() == QDropEvent::Copy )
	str = "Copy\n\n";
    else
	str = "Move\n\n";
    for ( uint i = 0; i < lst.count(); ++i )
	str += QString( "   %1\n" ).arg( lst.at( i ) );
    str += QString( "\n"
		    "To\n\n"
		    "	%1" ).arg( viewDir.absPath() );

    QMessageBox::information( this, e->action() == QDropEvent::Copy ? "Copy" : "Move" , str, "Not Implemented" );
    if ( e->action() == QDropEvent::Move )
	QMessageBox::information( this, "Remove" , str, "Not Implemented" );
    e->acceptAction();
    openItem = 0;
}

void QtFileIconView::viewLarge()
{
    setViewMode( Large );
}

void QtFileIconView::viewSmall()
{
    setViewMode( Small );
}

void QtFileIconView::viewBottom()
{
    setItemTextPos( Bottom );
}

void QtFileIconView::viewRight()
{
    setItemTextPos( Right );
}

void QtFileIconView::flowEast()
{
    setHScrollBarMode( AlwaysOff );
    setVScrollBarMode( Auto );
    setArrangement( LeftToRight );
}

void QtFileIconView::flowSouth()
{
    setVScrollBarMode( AlwaysOff );
    setHScrollBarMode( Auto );
    setArrangement( TopToBottom );
}

void QtFileIconView::sortAscending()
{
    sort( TRUE );
}

void QtFileIconView::sortDescending()
{
    sort( FALSE );
}

void QtFileIconView::itemTextTruncate()
{
    setWordWrapIconText( FALSE );
}

void QtFileIconView::itemTextWordWrap()
{
    setWordWrapIconText( TRUE );
}

void QtFileIconView::slotRightPressed( QIconViewItem *item )
{
    if ( !item ) { // right pressed on viewport
	QPopupMenu menu( this );

	menu.insertItem( "&Large view", this, SLOT( viewLarge() ) );
	menu.insertItem( "&Small view", this, SLOT( viewSmall() ) );
	menu.insertSeparator();
	menu.insertItem( "Text at the &bottom", this, SLOT( viewBottom() ) );
	menu.insertItem( "Text at the &right", this, SLOT( viewRight() ) );
	menu.insertSeparator();
	menu.insertItem( "Arrange l&eft to right", this, SLOT( flowEast() ) );
	menu.insertItem( "Arrange t&op to bottom", this, SLOT( flowSouth() ) );
	menu.insertSeparator();
	menu.insertItem( "&Truncate item text", this, SLOT( itemTextTruncate() ) );
	menu.insertItem( "&Wordwrap item text", this, SLOT( itemTextWordWrap() ) );
	menu.insertSeparator();
	menu.insertItem( "Arrange items in &grid", this, SLOT( arrangeItemsInGrid() ) );
	menu.insertSeparator();
	menu.insertItem( "Sort &ascending", this, SLOT( sortAscending() ) );
	menu.insertItem( "Sort &descending", this, SLOT( sortDescending() ) );

	menu.setMouseTracking( TRUE );
	menu.exec( QCursor::pos() );
    } else { // on item
	QPopupMenu menu( this );

	int RENAME_ITEM = menu.insertItem( "Rename Item" );
	int REMOVE_ITEM = menu.insertItem( "Remove Item" );

	menu.setMouseTracking( TRUE );
	int id = menu.exec( QCursor::pos() );

	if ( id == -1 )
	    return;

	if ( id == RENAME_ITEM && item->renameEnabled() ) {
	    item->rename();
	} else if ( id == REMOVE_ITEM ) {
	    delete item;
	    QMessageBox::information( this, "Not implemented!", "Deleting files not implemented yet,\n"
				      "The item has only been removed from the view! " );
	}
    }
}

void QtFileIconView::setViewMode( ViewMode m )
{
    if ( m == vm )
	return;

    vm = m;
    QtFileIconViewItem *item = (QtFileIconViewItem*)firstItem();
    for ( ; item; item = (QtFileIconViewItem*)item->nextItem() )
	item->viewModeChanged( vm );

    arrangeItemsInGrid();
}
