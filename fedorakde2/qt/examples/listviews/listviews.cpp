/****************************************************************************
** $Id: qt/examples/listviews/listviews.cpp   2.3.2   edited 2001-05-25 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "listviews.h"

#include <qlabel.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qobjectlist.h>
#include <qpopupmenu.h>
#include <qheader.h>
#include <qregexp.h>

// -----------------------------------------------------------------

MessageHeader::MessageHeader( const MessageHeader &mh )
{
    msender = mh.msender;
    msubject = mh.msubject;
    mdatetime = mh.mdatetime;
}

MessageHeader &MessageHeader::operator=( const MessageHeader &mh )
{
    msender = mh.msender;
    msubject = mh.msubject;
    mdatetime = mh.mdatetime;

    return *this;
}

// -----------------------------------------------------------------

Folder::Folder( Folder *parent, const QString &name )
    : QObject( parent, name ), fName( name )
{
    lstMessages.setAutoDelete( TRUE );
}

// -----------------------------------------------------------------

FolderListItem::FolderListItem( QListView *parent, Folder *f )
    : QListViewItem( parent )
{
    myFolder = f;
    setText( 0, f->folderName() );

    if ( myFolder->children() )
	insertSubFolders( myFolder->children() );
}

FolderListItem::FolderListItem( FolderListItem *parent, Folder *f )
    : QListViewItem( parent )
{
    myFolder = f;

    setText( 0, f->folderName() );

    if ( myFolder->children() )
	insertSubFolders( myFolder->children() );
}

void FolderListItem::insertSubFolders( const QObjectList *lst )
{
    Folder *f;
    for ( f = ( Folder* )( ( QObjectList* )lst )->first(); f; f = ( Folder* )( ( QObjectList* )lst )->next() )
	(void)new FolderListItem( this, f );
}

// -----------------------------------------------------------------

MessageListItem::MessageListItem( QListView *parent, Message *m )
    : QListViewItem( parent )
{
    myMessage = m;
    setText( 0, myMessage->header().sender() );
    setText( 1, myMessage->header().subject() );
    setText( 2, myMessage->header().datetime().toString() );
}

void MessageListItem::paintCell( QPainter *p, const QColorGroup &cg,
				 int column, int width, int alignment )
{
    QColorGroup _cg( cg );
    QColor c = _cg.text();

    if ( myMessage->state() == Message::Unread )
	_cg.setColor( QColorGroup::Text, Qt::red );

    QListViewItem::paintCell( p, _cg, column, width, alignment );

    _cg.setColor( QColorGroup::Text, c );
}

// -----------------------------------------------------------------

ListViews::ListViews( QWidget *parent, const char *name )
    : QSplitter( Qt::Horizontal, parent, name )
{
    lstFolders.setAutoDelete( TRUE );

    folders = new QListView( this );
    folders->header()->setClickEnabled( FALSE );
    folders->addColumn( "Folder" );

    initFolders();
    setupFolders();

    folders->setRootIsDecorated( TRUE );
    setResizeMode( folders, QSplitter::KeepSize );

    QSplitter *vsplitter = new QSplitter( Qt::Vertical, this );

    messages = new QListView( vsplitter );
    messages->addColumn( "Sender" );
    messages->addColumn( "Subject" );
    messages->addColumn( "Date" );
    messages->setColumnAlignment( 1, Qt::AlignRight );
    messages->setAllColumnsShowFocus( TRUE );
    messages->setShowSortIndicator( TRUE );
    menu = new QPopupMenu( messages );
    for( int i = 1; i <= 10; i++ )
	menu->insertItem( QString( "Context Item %1" ).arg( i ) );
    connect(messages, SIGNAL( rightButtonPressed( QListViewItem *, const QPoint& , int ) ),
	    this, SLOT( slotRMB( QListViewItem *, const QPoint &, int ) ) );
    vsplitter->setResizeMode( messages, QSplitter::KeepSize );

    message = new QLabel( vsplitter );
    message->setAlignment( Qt::AlignTop );
    message->setBackgroundMode( PaletteBase );

    connect( folders, SIGNAL( selectionChanged( QListViewItem* ) ),
	     this, SLOT( slotFolderChanged( QListViewItem* ) ) );
    connect( messages, SIGNAL( selectionChanged() ),
	     this, SLOT( slotMessageChanged() ) );
    connect( messages, SIGNAL( currentChanged( QListViewItem * ) ),
	     this, SLOT( slotMessageChanged() ) );

    messages->setSelectionMode( QListView::Extended );
    // some preparations
    folders->firstChild()->setOpen( TRUE );
    folders->firstChild()->firstChild()->setOpen( TRUE );
    folders->setCurrentItem( folders->firstChild()->firstChild()->firstChild() );
    folders->setSelected( folders->firstChild()->firstChild()->firstChild(), TRUE );

    messages->setSelected( messages->firstChild(), TRUE );
    messages->setCurrentItem( messages->firstChild() );
    message->setMargin( 5 );

    QValueList<int> lst;
    lst.append( 170 );
    setSizes( lst );
}

void ListViews::initFolders()
{
    unsigned int mcount = 1;

    for ( unsigned int i = 1; i < 20; i++ ) {
	QString str;
	str = QString( "Folder %1" ).arg( i );
	Folder *f = new Folder( 0, str );
	for ( unsigned int j = 1; j < 5; j++ ) {
	    QString str2;
	    str2 = QString( "Sub Folder %1" ).arg( j );
	    Folder *f2 = new Folder( f, str2 );
	    for ( unsigned int k = 1; k < 3; k++ ) {
		QString str3;
		str3 = QString( "Sub Sub Folder %1" ).arg( k );
		Folder *f3 = new Folder( f2, str3 );
		initFolder( f3, mcount );
	    }
	}
	lstFolders.append( f );
    }
}

void ListViews::initFolder( Folder *folder, unsigned int &count )
{
    for ( unsigned int i = 0; i < 15; i++, count++ ) {
	QString str;
	str = QString( "Message %1  " ).arg( count );
	QDateTime dt = QDateTime::currentDateTime();
	dt = dt.addSecs( 60 * count );
	MessageHeader mh( "Trolltech <info@trolltech.com>  ", str, dt );

	QString body;
	body = QString( "This is the message number %1 of this application, \n"
			"which shows how to use QListViews, QListViewItems, \n"
			"QSplitters and so on. The code should show how easy\n"
			"this can be done in Qt." ).arg( count );
	Message *msg = new Message( mh, body );
	folder->addMessage( msg );
    }
}

void ListViews::setupFolders()
{
    folders->clear();

    for ( Folder* f = lstFolders.first(); f; f = lstFolders.next() )
	(void)new FolderListItem( folders, f );
}

void ListViews::slotRMB( QListViewItem* Item, const QPoint & point, int )
{
    if( Item )
	menu->popup( point );
}


void ListViews::slotFolderChanged( QListViewItem *i )
{
    if ( !i )
	return;
    messages->clear();
    message->setText( "" );

    FolderListItem *item = ( FolderListItem* )i;

    for ( Message* msg = item->folder()->firstMessage(); msg;
	  msg = item->folder()->nextMessage() )
	(void)new MessageListItem( messages, msg );
}

void ListViews::slotMessageChanged()
{
    QListViewItem *i = messages->currentItem();
    if ( !i )
	return;

    if ( !i->isSelected() ) {
	message->setText( "" );
	return;
    }

    MessageListItem *item = ( MessageListItem* )i;
    Message *msg = item->message();

    QString text;
    QString tmp = msg->header().sender();
    tmp = tmp.replace( QRegExp( "[<]" ), "&lt;" );
    tmp = tmp.replace( QRegExp( "[>]" ), "&gt;" );
    text = QString( "<b><i>From:</i></b> <a href=\"mailto:info@trolltech.com\">%1</a><br>"
		    "<b><i>Subject:</i></b> <big><big><b>%2</b></big></big><br>"
		    "<b><i>Date:</i></b> %3<br><br>"
		    "%4" ).
	   arg( tmp ).arg( msg->header().subject() ).
	   arg( msg->header().datetime().toString() ).arg( msg->body() );

    message->setText( text );

    msg->setState( Message::Read );
}
