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

#include "connectionviewerimpl.h"
#include "formwindow.h"
#include "connectioneditorimpl.h"
#include "mainwindow.h"
#include "command.h"

#include <qlistview.h>
#include <qpushbutton.h>
#include <qobjectlist.h>

ConnectionViewer::ConnectionViewer( QWidget *parent, FormWindow *fw )
    : ConnectionViewerBase( parent, 0, TRUE ), formWindow( fw )
{
    readConnections();
    editButton->setEnabled( (bool)connectionListView->currentItem() );	
    disconnectButton->setEnabled( (bool)connectionListView->currentItem() );	
    connect( helpButton, SIGNAL( clicked() ), MainWindow::self, SLOT( showDialogHelp() ) );
}

void ConnectionViewer::editConnection()
{
    QListViewItem *i = connectionListView->currentItem();
    if ( !i )
	return;

    QObject *sender = 0, *receiver = 0;
    QString senderName = i->text( 0 );
    QString receiverName = i->text( 2 );

    if ( senderName == "this" || qstrcmp( formWindow->name(), senderName ) == 0 ) {
	sender = formWindow;
    } else {
	QObjectList *l = formWindow->queryList( 0, senderName, FALSE );
	if ( l ) {
	    if ( l->first() )
		sender = l->first();
	    delete l;
	}
    }

    if ( receiverName == "this" || qstrcmp( formWindow->name(), receiverName ) == 0 ) {
	receiver = formWindow;
    } else {
	QObjectList *l = formWindow->queryList( 0, receiverName, FALSE );
	if ( l ) {
	    if ( l->first() )
		receiver = l->first();
	    delete l;
	}
    }

    if ( sender && receiver ) {
	ConnectionEditor editor( formWindow->mainWindow(), sender, receiver, formWindow );
	editor.exec();
	connectionListView->clear();
	readConnections();
    }	
}

void ConnectionViewer::currentConnectionChanged( QListViewItem *i )
{
    if ( !i ) {
	editButton->setEnabled( FALSE );
	disconnectButton->setEnabled( FALSE );
	return;
    }

    editButton->setEnabled( TRUE );	
    disconnectButton->setEnabled( TRUE );	
}

void ConnectionViewer::readConnections()
{
    QValueList<MetaDataBase::Connection> connectionlist
	= MetaDataBase::connections( formWindow );
    for ( QValueList<MetaDataBase::Connection>::Iterator it = connectionlist.begin(); it != connectionlist.end(); ++it ) {
	if ( formWindow->isMainContainer( (QWidget*)(*it).receiver ) && !MetaDataBase::hasSlot( formWindow, (*it).slot ) )
	    continue;
	QListViewItem *i = new QListViewItem( connectionListView );
	MetaDataBase::Connection conn = *it;
	i->setText( 0, conn.sender->name() );
	i->setText( 1, conn.signal );
	i->setText( 2, conn.receiver->name() );
	i->setText( 3, conn.slot );
	connections.insert( i, conn );
    }
    if ( connectionListView->firstChild() )
	connectionListView->setCurrentItem( connectionListView->firstChild() );
}

void ConnectionViewer::disconnectConnection()
{
    if ( !connectionListView->currentItem() )
	return;

    MetaDataBase::Connection conn = connections[ connectionListView->currentItem() ];
    RemoveConnectionCommand *cmd = new RemoveConnectionCommand( tr( "Remove connection between %1 and %2" ).
								arg( conn.sender->name() ).arg( conn.receiver->name() ),
								formWindow, conn );
    formWindow->commandHistory()->addCommand( cmd );
    cmd->execute();
    delete connectionListView->currentItem();
    if ( connectionListView->currentItem() )
	connectionListView->setSelected( connectionListView->currentItem(), TRUE );
}
