/****************************************************************************
** $Id: qt/examples/dirview/dirview.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "dirview.h"

#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qpixmap.h>
#include <qevent.h>
#include <qpoint.h>
#include <qmessagebox.h>
#include <qdragobject.h>
#include <qmime.h>
#include <qstrlist.h>
#include <qstringlist.h>
#include <qapplication.h>
#include <qheader.h>

static const char* folder_closed_xpm[]={
    "16 16 9 1",
    "g c #808080",
    "b c #c0c000",
    "e c #c0c0c0",
    "# c #000000",
    "c c #ffff00",
    ". c None",
    "a c #585858",
    "f c #a0a0a4",
    "d c #ffffff",
    "..###...........",
    ".#abc##.........",
    ".#daabc#####....",
    ".#ddeaabbccc#...",
    ".#dedeeabbbba...",
    ".#edeeeeaaaab#..",
    ".#deeeeeeefe#ba.",
    ".#eeeeeeefef#ba.",
    ".#eeeeeefeff#ba.",
    ".#eeeeefefff#ba.",
    ".##geefeffff#ba.",
    "...##gefffff#ba.",
    ".....##fffff#ba.",
    ".......##fff#b##",
    ".........##f#b##",
    "...........####."};

static const char* folder_open_xpm[]={
    "16 16 11 1",
    "# c #000000",
    "g c #c0c0c0",
    "e c #303030",
    "a c #ffa858",
    "b c #808080",
    "d c #a0a0a4",
    "f c #585858",
    "c c #ffdca8",
    "h c #dcdcdc",
    "i c #ffffff",
    ". c None",
    "....###.........",
    "....#ab##.......",
    "....#acab####...",
    "###.#acccccca#..",
    "#ddefaaaccccca#.",
    "#bdddbaaaacccab#",
    ".eddddbbaaaacab#",
    ".#bddggdbbaaaab#",
    "..edgdggggbbaab#",
    "..#bgggghghdaab#",
    "...ebhggghicfab#",
    "....#edhhiiidab#",
    "......#egiiicfb#",
    "........#egiibb#",
    "..........#egib#",
    "............#ee#"};

static const char * folder_locked[]={
    "16 16 10 1",
    "h c #808080",
    "b c #ffa858",
    "f c #c0c0c0",
    "e c #c05800",
    "# c #000000",
    "c c #ffdca8",
    ". c None",
    "a c #585858",
    "g c #a0a0a4",
    "d c #ffffff",
    "..#a#...........",
    ".#abc####.......",
    ".#daa#eee#......",
    ".#ddf#e##b#.....",
    ".#dfd#e#bcb##...",
    ".#fdccc#daaab#..",
    ".#dfbbbccgfg#ba.",
    ".#ffb#ebbfgg#ba.",
    ".#ffbbe#bggg#ba.",
    ".#fffbbebggg#ba.",
    ".##hf#ebbggg#ba.",
    "...###e#gggg#ba.",
    ".....#e#gggg#ba.",
    "......###ggg#b##",
    ".........##g#b##",
    "...........####."};

static const char * pix_file []={
    "16 16 7 1",
    "# c #000000",
    "b c #ffffff",
    "e c #000000",
    "d c #404000",
    "c c #c0c000",
    "a c #ffffc0",
    ". c None",
    "................",
    ".........#......",
    "......#.#a##....",
    ".....#b#bbba##..",
    "....#b#bbbabbb#.",
    "...#b#bba##bb#..",
    "..#b#abb#bb##...",
    ".#a#aab#bbbab##.",
    "#a#aaa#bcbbbbbb#",
    "#ccdc#bcbbcbbb#.",
    ".##c#bcbbcabb#..",
    "...#acbacbbbe...",
    "..#aaaacaba#....",
    "...##aaaaa#.....",
    ".....##aa#......",
    ".......##......."};

QPixmap *folderLocked = 0;
QPixmap *folderClosed = 0;
QPixmap *folderOpen = 0;
QPixmap *fileNormal = 0;

/*****************************************************************************
 *
 * Class Directory
 *
 *****************************************************************************/

Directory::Directory( Directory * parent, const QString& filename )
    : QListViewItem( parent ), f(filename),
      showDirsOnly( parent->showDirsOnly ),
      pix( 0 )
{
    p = parent;
    readable = QDir( fullName() ).isReadable();

    if ( !readable )
	setPixmap( folderLocked );
    else
	setPixmap( folderClosed );
}


Directory::Directory( QListView * parent, const QString& filename )
    : QListViewItem( parent ), f(filename),
      showDirsOnly( ( (DirectoryView*)parent )->showDirsOnly() ),
      pix( 0 )
{
    p = 0;
    readable = QDir( fullName() ).isReadable();
}


void Directory::setPixmap( QPixmap *px )
{
    pix = px;
    setup();
    widthChanged( 0 );
    invalidateHeight();
    repaint();
}


const QPixmap *Directory::pixmap( int i ) const
{
    if ( i )
	return 0;
    return pix;
}

void Directory::setOpen( bool o )
{
    if ( o )
	setPixmap( folderOpen );
    else
	setPixmap( folderClosed );

    if ( o && !childCount() ) {
	QString s( fullName() );
	QDir thisDir( s );
	if ( !thisDir.isReadable() ) {
	    readable = FALSE;
	    setExpandable( FALSE );
	    return;
	}

	listView()->setUpdatesEnabled( FALSE );
	const QFileInfoList * files = thisDir.entryInfoList();
	if ( files ) {
	    QFileInfoListIterator it( *files );
	    QFileInfo * fi;
	    while( (fi=it.current()) != 0 ) {
		++it;
		if ( fi->fileName() == "." || fi->fileName() == ".." )
		    ; // nothing
		else if ( fi->isSymLink() && !showDirsOnly ) {
		    FileItem *item = new FileItem( this, fi->fileName(),
						     "Symbolic Link" );
		    item->setPixmap( fileNormal );
		}
		else if ( fi->isDir() )
		    (void)new Directory( this, fi->fileName() );
		else if ( !showDirsOnly ) {
		    FileItem *item
			= new FileItem( this, fi->fileName(),
					     fi->isFile()?"File":"Special" );
		    item->setPixmap( fileNormal );
		}
	    }
	}
	listView()->setUpdatesEnabled( TRUE );
    }
    QListViewItem::setOpen( o );
}


void Directory::setup()
{
    setExpandable( TRUE );
    QListViewItem::setup();
}


QString Directory::fullName()
{
    QString s;
    if ( p ) {
	s = p->fullName();
	s.append( f.name() );
	s.append( "/" );
    } else {
	s = f.name();
    }
    return s;
}


QString Directory::text( int column ) const
{
    if ( column == 0 )
	return f.name();
    else if ( readable )
	return "Directory";
    else
	return "Unreadable Directory";
}

/*****************************************************************************
 *
 * Class DirectoryView
 *
 *****************************************************************************/

DirectoryView::DirectoryView( QWidget *parent, const char *name, bool sdo )
    : QListView( parent, name ), dirsOnly( sdo ), oldCurrent( 0 ),
      dropItem( 0 ), mousePressed( FALSE )
{
    autoopen_timer = new QTimer( this );
    if ( !folderLocked ) {
	folderLocked = new QPixmap( folder_locked );
	folderClosed = new QPixmap( folder_closed_xpm );
	folderOpen = new QPixmap( folder_open_xpm );
	fileNormal = new QPixmap( pix_file );
    }

    connect( this, SIGNAL( doubleClicked( QListViewItem * ) ),
	     this, SLOT( slotFolderSelected( QListViewItem * ) ) );
    connect( this, SIGNAL( returnPressed( QListViewItem * ) ),
	     this, SLOT( slotFolderSelected( QListViewItem * ) ) );

    setAcceptDrops( TRUE );
    viewport()->setAcceptDrops( TRUE );

    connect( autoopen_timer, SIGNAL( timeout() ),
	     this, SLOT( openFolder() ) );
}

void DirectoryView::slotFolderSelected( QListViewItem *i )
{
    if ( !i || !showDirsOnly() )
	return;

    Directory *dir = (Directory*)i;
    emit folderSelected( dir->fullName() );
}

void DirectoryView::openFolder()
{
    autoopen_timer->stop();
    if ( dropItem && !dropItem->isOpen() ) {
	dropItem->setOpen( TRUE );
	dropItem->repaint();
    }
}

static const int autoopenTime = 750;


void DirectoryView::contentsDragEnterEvent( QDragEnterEvent *e )
{
    if ( !QUriDrag::canDecode(e) ) {
	e->ignore();
	return;
    }

    oldCurrent = currentItem();

    QListViewItem *i = itemAt( contentsToViewport(e->pos()) );
    if ( i ) {
	dropItem = i;
	autoopen_timer->start( autoopenTime );
    }
}


void DirectoryView::contentsDragMoveEvent( QDragMoveEvent *e )
{
    if ( !QUriDrag::canDecode(e) ) {
	e->ignore();
	return;
    }

    QPoint vp = contentsToViewport( ( (QDragMoveEvent*)e )->pos() );
    QListViewItem *i = itemAt( vp );
    if ( i ) {
	setSelected( i, TRUE );
	e->accept();
	if ( i != dropItem ) {
	    autoopen_timer->stop();
	    dropItem = i;
	    autoopen_timer->start( autoopenTime );
	}
	switch ( e->action() ) {
	case QDropEvent::Copy:
	    break;
	case QDropEvent::Move:
	    e->acceptAction();
	    break;
	case QDropEvent::Link:
	    e->acceptAction();
	    break;
	default:
	    ;
	}
    } else {
	e->ignore();
	autoopen_timer->stop();
	dropItem = 0;
    }
}

void DirectoryView::contentsDragLeaveEvent( QDragLeaveEvent * )
{
    autoopen_timer->stop();
    dropItem = 0;

    setCurrentItem( oldCurrent );
    setSelected( oldCurrent, TRUE );
}

void DirectoryView::contentsDropEvent( QDropEvent *e )
{
    autoopen_timer->stop();

    if ( !QUriDrag::canDecode(e) ) {
	e->ignore();
	return;
    }

    QListViewItem *item = itemAt( contentsToViewport(e->pos()) );
    if ( item ) {

	QStrList lst;

	QUriDrag::decode( e, lst );

	QString str;

	switch ( e->action() ) {
	    case QDropEvent::Copy:
	    str = "Copy";
	    break;
	    case QDropEvent::Move:
	    str = "Move";
	    e->acceptAction();
	    break;
	    case QDropEvent::Link:
	    str = "Link";
	    e->acceptAction();
	    break;
	    default:
	    str = "Unknown";
	}

	str += "\n\n";

	e->accept();

	for ( uint i = 0; i < lst.count(); ++i ) {
	    QString filename = lst.at( i );
	    str += filename + "\n";
	}
	str += QString( "\nTo\n\n   %1" )
	       .arg( fullPath(item) );

	QMessageBox::information( this, "Drop target", str, "Not implemented" );
    } else
	e->ignore();

}


QString DirectoryView::fullPath(QListViewItem* item)
{
    QString fullpath = item->text(0);
    while ( (item=item->parent()) ) {
	if ( item->parent() )
	    fullpath = item->text(0) + "/" + fullpath;
	else
	    fullpath = item->text(0) + fullpath;
    }
    return fullpath;
}

void DirectoryView::contentsMousePressEvent( QMouseEvent* e )
{
    QListView::contentsMousePressEvent(e);
    QPoint p( contentsToViewport( e->pos() ) );
    QListViewItem *i = itemAt( p );
    if ( i ) {
	// if the user clicked into the root decoration of the item, don't try to start a drag!
	if ( p.x() > header()->cellPos( header()->mapToActual( 0 ) ) +
	     treeStepSize() * ( i->depth() + ( rootIsDecorated() ? 1 : 0) ) + itemMargin() ||
	     p.x() < header()->cellPos( header()->mapToActual( 0 ) ) ) {
	    presspos = e->pos();
	    mousePressed = TRUE;
	}
    }
}

void DirectoryView::contentsMouseMoveEvent( QMouseEvent* e )
{
    if ( mousePressed && ( presspos - e->pos() ).manhattanLength() > QApplication::startDragDistance() ) {
	mousePressed = FALSE;
	QListViewItem *item = itemAt( contentsToViewport(presspos) );
	if ( item ) {
	    QString source = fullPath(item);
	    if ( QFile::exists(source) ) {
		QUriDrag* ud = new QUriDrag(viewport());
		ud->setUnicodeUris( source );
		if ( ud->drag() )
		    QMessageBox::information( this, "Drag source",
					      QString("Delete ")+source, "Not implemented" );
	    }
	}
    }
}

void DirectoryView::contentsMouseReleaseEvent( QMouseEvent * )
{
    mousePressed = FALSE;
}

void DirectoryView::setDir( const QString &s )
{
    QListViewItemIterator it( this );
    ++it;
    for ( ; it.current(); ++it ) {
	it.current()->setOpen( FALSE );
    }

    QStringList lst( QStringList::split( "/", s ) );
    QListViewItem *item = firstChild();
    QStringList::Iterator it2 = lst.begin();
    for ( ; it2 != lst.end(); ++it2 ) {
	while ( item ) {
	    if ( item->text( 0 ) == *it2 ) {
		item->setOpen( TRUE );
		break;
	    }
	    item = item->itemBelow();
	}
    }

    if ( item )
	setCurrentItem( item );
}

void FileItem::setPixmap( QPixmap *p )
{
    pix = p;
    setup();
    widthChanged( 0 );
    invalidateHeight();
    repaint();
}


const QPixmap *FileItem::pixmap( int i ) const
{
    if ( i )
	return 0;
    return pix;
}
