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
#include "formlist.h"
#include "pixmapchooser.h"
#include "formwindow.h"
#include "globaldefs.h"
#include "command.h"
#include "mainwindow.h"

#include <qheader.h>
#include <qdragobject.h>
#include <qfileinfo.h>
#include <qapplication.h>

FormListItem::FormListItem( QListView *parent, const QString &form, const QString &file, FormWindow *fw )
    : QListViewItem( parent, form, file ), formwindow( fw )
{
    setPixmap( 0, PixmapChooser::loadPixmap( "form.xpm", PixmapChooser::Mini ) );
    setText( 1, "" );
}

void FormListItem::paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int align )
{
    QColorGroup g( cg );
    g.setColor( QColorGroup::Base, backgroundColor() );
    g.setColor( QColorGroup::Foreground, Qt::black );
    g.setColor( QColorGroup::Text, Qt::black );
    p->save();

    if ( formWindow()->commandHistory()->isModified() ) {
	QFont f = p->font();
	f.setBold( TRUE );
	p->setFont( f );
    }
	
    QListViewItem::paintCell( p, g, column, width, align );
    p->setPen( QPen( cg.dark(), 1 ) );
    if ( column == 0 )
	p->drawLine( 0, 0, 0, height() - 1 );
    p->drawLine( 0, height() - 1, width, height() - 1 );
    p->drawLine( width - 1, 0, width - 1, height() );
    p->restore();
}

QColor FormListItem::backgroundColor()
{
    updateBackColor();
    return backColor;
}

void FormListItem::updateBackColor()
{
    if ( listView()->firstChild() == this ) {
 	backColor = backColor1;
	return;
    }

    QListViewItemIterator it( this );
    --it;
    if ( it.current() ) {
	if ( ( ( FormListItem*)it.current() )->backColor == backColor1 )
	    backColor = backColor2;
	else
	    backColor = backColor1;
    } else {
	backColor == backColor1;
    }
}

FormList::FormList( QWidget *parent, MainWindow *mw )
    : QListView( parent, 0, WStyle_Customize | WStyle_NormalBorder | WStyle_Title |
		 WStyle_Tool | WStyle_MinMax | WStyle_SysMenu ), mainWindow( mw )
{
    header()->setMovingEnabled( FALSE );
    setResizePolicy( QScrollView::Manual );
    setIcon( PixmapChooser::loadPixmap( "logo" ) );
    QPalette p( palette() );
    p.setColor( QColorGroup::Base, QColor( backColor2 ) );
    setPalette( p );
    addColumn( tr( "Form" ) );
    addColumn( tr( "Changed" ) );
    addColumn( tr( "Filename" ) );
    setAllColumnsShowFocus( TRUE );
    connect( header(), SIGNAL( sizeChange( int, int, int ) ),
	     this, SLOT( updateHeader() ) );
    connect( this, SIGNAL( clicked( QListViewItem * ) ),
	     this, SLOT( itemClicked( QListViewItem * ) ) ),
    setHScrollBarMode( AlwaysOff );
    viewport()->setAcceptDrops( TRUE );
    setAcceptDrops( TRUE );
}

void FormList::addForm( FormWindow *fw )
{
    (void)new FormListItem( this, QString( fw->name() ), fw->fileName(), fw );
    updateHeader();
}

void FormList::modificationChanged( bool m, FormWindow *fw )
{
    FormListItem *i = findItem( fw );
    if ( i ) {
    	QString s;
    	if ( m )
      	s = tr( "*" );
      else
        s = tr( "" );
    	i->setText( 1, s );
    }
}

void FormList::fileNameChanged( const QString &s, FormWindow *fw )
{
    FormListItem *i = findItem( fw );
    if ( !i )
	return;
    if ( s.isEmpty() )
	i->setText( 2, tr( "(unnamed)" ) );
    else
	i->setText( 2, s );
}

void FormList::activeFormChanged( FormWindow *fw )
{
    FormListItem *i = findItem( fw );
    if ( i ) {
	setCurrentItem( i );
	setSelected( i, TRUE );
    }
}

void FormList::nameChanged( FormWindow *fw )
{
    FormListItem *i = findItem( fw );
    if ( !i )
	return;
    i->setText( 0, fw->name() );
}

FormListItem *FormList::findItem( FormWindow *fw )
{
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
	if ( ( (FormListItem*)it.current() )->formWindow() == fw )
	    return (FormListItem*)it.current();
    }
    return 0;
}


void FormList::closed( FormWindow *fw )
{
    FormListItem *i = findItem( fw );
    if ( i )
	delete i;
}

void FormList::resizeEvent( QResizeEvent *e )
{
    QListView::resizeEvent( e );
    QSize vs = viewportSize( 0, contentsHeight() );

    int os = header()->sectionSize( 2 );
    int ns = vs.width() - header()->sectionSize( 0 ) - header()->sectionSize( 1 );
    if ( ns < 16 )
	ns = 16;
	
    header()->resizeSection( 2, ns );
    header()->repaint( header()->width() - header()->sectionSize( 2 ), 0, header()->sectionSize( 2 ), header()->height() );

    int elipsis = fontMetrics().width("...") + 10;
    viewport()->repaint( header()->sectionPos( 2 ) + os - elipsis, 0, elipsis, viewport()->height(), FALSE );
}

void FormList::updateHeader()
{
    QSize s( header()->sectionPos( 2 ) + header()->sectionSize(1), height() );
    QResizeEvent e( s, size() );
    resizeEvent( &e );
    viewport()->repaint( s.width(), 0, width() - s.width(), height(), FALSE );
}

void FormList::closeEvent( QCloseEvent *e )
{
    emit hidden();
    e->accept();
}

void FormList::itemClicked( QListViewItem *i )
{
    if ( !i )
	return;
    ( (FormListItem*)i )->formWindow()->setFocus();
}

void FormList::contentsDropEvent( QDropEvent *e )
{
    if ( !QUriDrag::canDecode( e ) ) {
	e->ignore();
    } else {
	QStringList files;
	QUriDrag::decodeLocalFiles( e, files );
	if ( !files.isEmpty() ) {
	    for ( QStringList::Iterator it = files.begin(); it != files.end(); ++it ) {
		QString fn = *it;
		if ( QFileInfo( fn ).extension().lower() == "ui" )
		    mainWindow->openFile( fn );
	    }
	}
    }
}

void FormList::contentsDragEnterEvent( QDragEnterEvent *e )
{
    if ( !QUriDrag::canDecode( e ) )
	e->ignore();
    else
	e->accept();
}

void FormList::contentsDragMoveEvent( QDragMoveEvent *e )
{
    if ( !QUriDrag::canDecode( e ) )
	e->ignore();
    else
	e->accept();
}
