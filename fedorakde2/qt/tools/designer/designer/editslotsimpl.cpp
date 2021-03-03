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

#include "editslotsimpl.h"
#include "formwindow.h"
#include "metadatabase.h"
#include "asciivalidator.h"
#include "mainwindow.h"

#include <qlistview.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qstrlist.h>
#include <qmessagebox.h>
#include <qlayout.h>

EditSlots::EditSlots( QWidget *parent, FormWindow *fw )
    : EditSlotsBase( parent, 0, TRUE ), formWindow( fw )
{
    connect( helpButton, SIGNAL( clicked() ), MainWindow::self, SLOT( showDialogHelp() ) );
    QValueList<MetaDataBase::Slot> slotList = MetaDataBase::slotList( fw );
    for ( QValueList<MetaDataBase::Slot>::Iterator it = slotList.begin(); it != slotList.end(); ++it ) {
	QListViewItem *i = new QListViewItem( slotListView );
	i->setText( 0, (*it).slot );
	i->setText( 1, (*it).access );
	if ( MetaDataBase::isSlotUsed( formWindow, (*it).slot ) )
	    i->setText( 2, tr( "Yes" ) );
	else
	    i->setText( 2, tr( "No" ) );
    }

    slotName->setEnabled( FALSE );
    slotAccess->setEnabled( FALSE );
    slotName->setValidator( new AsciiValidator( TRUE, slotName ) );

    if ( slotListView->firstChild() )
	slotListView->setCurrentItem( slotListView->firstChild() );
}

void EditSlots::okClicked()
{
    QValueList<MetaDataBase::Slot> slotList = MetaDataBase::slotList( formWindow );
    MacroCommand *rmSlt = 0, *addSlt = 0;
    QString n = tr( "Add/Remove slots of '%1'" ).arg( formWindow->name() );
    QValueList<MetaDataBase::Slot>::Iterator sit;
    if ( !slotList.isEmpty() ) {
	QList<Command> commands;
	for ( sit = slotList.begin(); sit != slotList.end(); ++sit ) {
	    commands.append( new RemoveSlotCommand( tr( "Remove slot" ),
						    formWindow, (*sit).slot, (*sit).access ) );
	}
	rmSlt = new MacroCommand( tr( "Remove slots" ), formWindow, commands );
    }

    bool invalidSlots = FALSE;
    QList<QListViewItem> invalidItems;
    if ( slotListView->firstChild() ) {
	QList<Command> commands;
	QListViewItemIterator it( slotListView );
	QStrList lst;
	lst.append( "accept()" );
	lst.append( "reject()" );
	lst.append( "done(int)" );
	for ( ; it.current(); ++it ) {
	    MetaDataBase::Slot slot;
	    slot.slot = it.current()->text( 0 );
	    slot.access = it.current()->text( 1 );
	    QString s = it.current()->text( 0 );
	    s = s.simplifyWhiteSpace();
	    bool startNum = s[ 0 ] >= '0' && s[ 0 ] <= '9';
	    bool noParens = s.contains( '(' ) != 1 || s.contains( ')' ) != 1;
	    bool illegalSpace = s.find( ' ' ) != -1 && s.find( ' ' ) < s.find( '(' );
	    if ( startNum || noParens || illegalSpace || lst.find( slot.slot ) != -1 ) {
		invalidSlots = TRUE;
		invalidItems.append( it.current() );
		continue;
	    }
	    commands.append( new AddSlotCommand( tr( "Add slot" ),
						 formWindow, slot.slot, slot.access ) );
	    lst.append( slot.slot );
	}
	
	if ( !commands.isEmpty() )
	    addSlt = new MacroCommand( tr( "Add slots" ), formWindow, commands );
    }

    if ( invalidSlots ) {
	if ( QMessageBox::information( this, tr( "Edit Slots" ), tr( "Some syntatically wrong slots defined.\n"
								     "Remove these slots?" ),
				       tr( "&Yes" ), tr( "&No" ) ) == 0 ) {
	    QListViewItemIterator it( slotListView );
	    QListViewItem *i;
	    while ( (i = it.current() ) ) {
		++it;
		if ( invalidItems.findRef( i ) != -1 )
		    delete i;
	    }
	    if ( slotListView->firstChild() ) {
		slotListView->setCurrentItem( slotListView->firstChild() );
		slotListView->setSelected( slotListView->firstChild(), TRUE );
	    }
	}
	return;
    }

    if ( rmSlt || addSlt ) {
	QList<Command> commands;
	if ( rmSlt )
	    commands.append( rmSlt );
	if ( addSlt )
	    commands.append( addSlt );
	MacroCommand *cmd = new MacroCommand( n, formWindow, commands );
	formWindow->commandHistory()->addCommand( cmd );
	cmd->execute();
    }

    accept();
}

void EditSlots::slotAdd()
{
    QListViewItem *i = new QListViewItem( slotListView );
    i->setText( 0, "new_slot()" );
    i->setText( 1, "public" );
    if ( MetaDataBase::isSlotUsed( formWindow, "new_slot()" ) )
	i->setText( 2, tr( "Yes" ) );
    else
	i->setText( 2, tr( "No" ) );
    slotListView->setCurrentItem( i );
    slotListView->setSelected( i, TRUE );
}

void EditSlots::slotRemove()
{
    if ( !slotListView->currentItem() )
	return;

    slotListView->blockSignals( TRUE );
    delete slotListView->currentItem();
    if ( slotListView->currentItem() )
	slotListView->setSelected( slotListView->currentItem(), TRUE );
    slotListView->blockSignals( FALSE );
    currentItemChanged( slotListView->currentItem() );
}

void EditSlots::currentItemChanged( QListViewItem *i )
{
    slotName->blockSignals( TRUE );
    slotName->setText( "" );
    slotAccess->setCurrentItem( 0 );
    slotName->blockSignals( FALSE );

    if ( !i ) {
	slotName->setEnabled( FALSE );
	slotAccess->setEnabled( FALSE );
	return;
    }

    slotName->blockSignals( TRUE );
    slotName->setEnabled( TRUE );
    slotAccess->setEnabled( TRUE );
    slotName->setText( i->text( 0 ) );
    if ( i->text( 1 ) == "public" )
	slotAccess->setCurrentItem( 0 );
    else
	slotAccess->setCurrentItem( 1 );
    slotName->blockSignals( FALSE );
}

void EditSlots::currentTextChanged( const QString &txt )
{
    if ( !slotListView->currentItem() )
	return;

    slotListView->currentItem()->setText( 0, txt );
    if ( MetaDataBase::isSlotUsed( formWindow, txt.utf8() ) )
	slotListView->currentItem()->setText( 2, tr( "Yes" ) );
    else
	slotListView->currentItem()->setText( 2, tr( "No" ) );
}

void EditSlots::currentAccessChanged( const QString &acc )
{
    if ( !slotListView->currentItem() )
	return;

    slotListView->currentItem()->setText( 1, acc );
}
