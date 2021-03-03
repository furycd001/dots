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

#include "connectioneditorimpl.h"
#include "metadatabase.h"
#include "formwindow.h"
#include "command.h"
#include "widgetfactory.h"
#include "editslotsimpl.h"
#include "mainwindow.h"

#include <qmetaobject.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qregexp.h>

static const char* const ignore_slots[] = {
    "destroyed()",
    "setCaption(const QString&)",
    "setIcon(const QPixmap&)",
    "setIconText(const QString&)",
    "setMouseTracking(bool)",
    "clearFocus()",
    "setUpdatesEnabled(bool)",
    "update()",
    "update(int,int,int,int)",
    "update(const QRect&)",
    "repaint()",
    "repaint(bool)",
    "repaint(int,int,int,int,bool)",
    "repaint(const QRect&,bool)",
    "repaint(const QRegion&,bool)",
    "show()",
    "hide()",
    "iconify()",
    "showMinimized()",
    "showMaximized()",
    "showFullScreen()",
    "showNormal()",
    "polish()",
    "constPolish()",
    "raise()",
    "lower()",
    "close()",
    "stackUnder(QWidget*)",
    "move(int,int)",
    "move(const QPoint&)",
    "resize(int,int)",
    "resize(const QSize&)",
    "setGeometry(int,int,int,int)",
    "setGeometry(const QRect&)",
    "focusProxyDestroyed()",
    "showExtension(bool)",
    0
};

ConnectionEditor::ConnectionEditor( QWidget *parent, QObject* sndr, QObject* rcvr, FormWindow *fw )
    : ConnectionEditorBase( parent, 0, TRUE ), formWindow( fw )
{
    connect( helpButton, SIGNAL( clicked() ), MainWindow::self, SLOT( showDialogHelp() ) );
    if ( rcvr == formWindow )
	rcvr = formWindow->mainContainer();
    if ( sndr == formWindow )
	sndr = formWindow->mainContainer();
    sender = sndr;
    receiver = rcvr;

    QStrList sigs = sender->metaObject()->signalNames( TRUE );
    sigs.remove( "destroyed()" );
    signalBox->insertStrList( sigs );

    if ( sender->inherits( "CustomWidget" ) ) {
	MetaDataBase::CustomWidget *w = ( (CustomWidget*)sender )->customWidget();
	for ( QValueList<QCString>::Iterator it = w->lstSignals.begin(); it != w->lstSignals.end(); ++it )
	    signalBox->insertItem( QString( *it ) );
    }

    labelSignal->setText( tr( "Signals (%1):" ).arg( sender->name() ) );
    labelSlot->setText( tr( "Slots (%1):" ).arg( receiver->name() ) );

    signalBox->setCurrentItem( signalBox->firstItem() );

    oldConnections = MetaDataBase::connections( formWindow, sender, receiver );
    if ( !oldConnections.isEmpty() ) {
	QValueList<MetaDataBase::Connection>::Iterator it = oldConnections.begin();
	for ( ; it != oldConnections.end(); ++it ) {
	    if ( formWindow->isMainContainer( (QWidget*)(*it).receiver ) && !MetaDataBase::hasSlot( formWindow, (*it).slot ) )
		continue;
	    MetaDataBase::Connection conn = *it;
	    QListViewItem *i = new QListViewItem( connectionView );
	    i->setText( 0, conn.sender->name() );
	    i->setText( 1, conn.signal );
	    i->setText( 2, conn.receiver->name() );
	    i->setText( 3, conn.slot );
	    Connection c;
	    c.signal = conn.signal;
	    c.slot = conn.slot;
	    this->connections.insert( i, c );
	}
    }

    connectionsChanged();
    connectButton->setEnabled( FALSE );
    buttonAddSlot->setEnabled( receiver == fw->mainContainer() );
}

ConnectionEditor::~ConnectionEditor()
{
}

void ConnectionEditor::signalChanged()
{
    QCString signal = signalBox->currentText().latin1();
    if ( !signal.data() )
	return;
    signal = normalizeSignalSlot( signal.data() );
    slotBox->clear();
    if ( signalBox->currentText().isEmpty() )
	return;
    int n = receiver->metaObject()->numSlots( TRUE );
    for( int i = 0; i < n; ++i ) {
	// accept only public slots. For the form window, also accept protected slots
	QMetaData* md =  receiver->metaObject()->slot( i, TRUE  );
	if ( ( (receiver->metaObject()->slot_access( i, TRUE ) == QMetaData::Public) ||
	       ( formWindow->isMainContainer( (QWidget*)receiver ) &&
		 receiver->metaObject()->slot_access(i, TRUE) == QMetaData::Protected) ) &&
	     !ignoreSlot( md->name ) &&
	     checkConnectArgs( signal.data(), receiver, md->name ) )
	    slotBox->insertItem( md->name );
    }

    if ( formWindow->isMainContainer( (QWidget*)receiver ) ) {
	QValueList<MetaDataBase::Slot> moreSlots = MetaDataBase::slotList( formWindow );
	if ( !moreSlots.isEmpty() ) {
	    for ( QValueList<MetaDataBase::Slot>::Iterator it = moreSlots.begin(); it != moreSlots.end(); ++it ) {
		QCString s = (*it).slot;
		if ( !s.data() )
		    continue;
		s = normalizeSignalSlot( s.data() );
		if ( checkConnectArgs( signal.data(), receiver, s ) )
		    slotBox->insertItem( QString( (*it).slot ) );
	    }
	}	
    }

    if ( receiver->inherits( "CustomWidget" ) ) {
	MetaDataBase::CustomWidget *w = ( (CustomWidget*)receiver )->customWidget();
	for ( QValueList<MetaDataBase::Slot>::Iterator it = w->lstSlots.begin(); it != w->lstSlots.end(); ++it ) {
	    QCString s = (*it).slot;
	    if ( !s.data() )
		continue;
	    s = normalizeSignalSlot( s.data() );
 	    if ( checkConnectArgs( signal.data(), receiver, s ) )
		slotBox->insertItem( QString( (*it).slot ) );
	}
    }

    slotsChanged();
}


bool ConnectionEditor::ignoreSlot( const char* slot ) const
{
    for ( int i = 0; ignore_slots[i]; i++ ) {
	if ( qstrcmp( slot, ignore_slots[i] ) == 0 )
	    return TRUE;
    }

    if ( !formWindow->isMainContainer( (QWidget*)receiver ) ) {
	if ( qstrcmp( slot, "close()" ) == 0  )
	    return TRUE;
    }

    if ( qstrcmp( slot, "setFocus()" ) == 0  )
	if ( receiver->isWidgetType() && ( (QWidget*)receiver )->focusPolicy() == QWidget::NoFocus )
	    return TRUE;

    return FALSE;
}

void ConnectionEditor::connectClicked()
{
    if ( signalBox->currentItem() == -1 ||
	 slotBox->currentItem() == -1 )
	return;
    Connection conn;
    conn.signal = signalBox->currentText();
    conn.slot = slotBox->currentText();
    QListViewItem *i = new QListViewItem( connectionView );
    i->setText( 0, sender->name() );
    i->setText( 1, conn.signal );
    i->setText( 2, receiver->name() );
    i->setText( 3, conn.slot );
    connectionView->setCurrentItem( i );
    connectionView->setSelected( i, TRUE );
    connections.insert( i, conn );
}

void ConnectionEditor::disconnectClicked()
{
    QListViewItem *i = connectionView->currentItem();
    if ( !i )
	return;

    QMap<QListViewItem*, Connection>::Iterator it = connections.find( i );
    if ( it != connections.end() )
	connections.remove( it );
    delete i;
    if ( connectionView->currentItem() )
	connectionView->setSelected( connectionView->currentItem(), TRUE );
    connectionsChanged();
}

void ConnectionEditor::okClicked()
{
    MacroCommand *rmConn = 0, *addConn = 0;
    QString n = tr( "Connect/Disconnect signals and slots of '%1' and '%2'" ).arg( sender->name() ).arg( receiver->name() );
    QValueList<MetaDataBase::Connection>::Iterator cit;
    if ( !oldConnections.isEmpty() ) {
	QList<Command> commands;
	for ( cit = oldConnections.begin(); cit != oldConnections.end(); ++cit ) {
	    commands.append( new RemoveConnectionCommand( tr( "Remove connection" ),
							  formWindow, *cit ) );
	}
	rmConn = new MacroCommand( tr( "Remove connections" ), formWindow, commands );
    }

    if ( !connections.isEmpty() ) {
	QMap<QListViewItem*, Connection>::Iterator it = connections.begin();
	QList<Command> commands;
	for ( ; it != connections.end(); ++it ) {
	    Connection c = *it;
	    MetaDataBase::Connection conn;
	    conn.sender = sender;
	    conn.signal = c.signal;
	    conn.receiver = receiver;
	    conn.slot = c.slot;
	    commands.append( new AddConnectionCommand( tr( "Add connection" ),
						       formWindow, conn ) );
	}
	addConn = new MacroCommand( tr( "Add connections" ), formWindow, commands );
    }

    if ( rmConn || addConn ) {
	QList<Command> commands;
	if ( rmConn )
	    commands.append( rmConn );
	if ( addConn )
	    commands.append( addConn );
	MacroCommand *cmd = new MacroCommand( n, formWindow, commands );
	formWindow->commandHistory()->addCommand( cmd );
	cmd->execute();
    }

    accept();
}

void ConnectionEditor::cancelClicked()
{
    reject();
}

void ConnectionEditor::slotsChanged()
{
    connectButton->setEnabled( slotBox->currentItem() != -1 );
}

void ConnectionEditor::connectionsChanged()
{
    disconnectButton->setEnabled( (bool)connectionView->currentItem() );
}

void ConnectionEditor::addSlotClicked()
{
    EditSlots dlg( this, formWindow );
    dlg.exec();
    signalChanged();
    QListViewItemIterator it( connectionView );
    QListViewItem *i = 0;
    while ( ( i = it.current() ) ) {
	++it;
	if ( !MetaDataBase::hasSlot( formWindow, i->text( 3 ).latin1() ) )
	    delete i;
    }
}
