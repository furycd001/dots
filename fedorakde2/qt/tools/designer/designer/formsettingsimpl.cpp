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

#include "formsettingsimpl.h"
#include "formwindow.h"
#include "metadatabase.h"
#include "command.h"
#include "asciivalidator.h"
#include "mainwindow.h"

#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qradiobutton.h>

FormSettings::FormSettings( QWidget *parent, FormWindow *fw )
    : FormSettingsBase( parent, 0, TRUE ), formwindow( fw )
{
    connect( buttonHelp, SIGNAL( clicked() ), MainWindow::self, SLOT( showDialogHelp() ) );
    MetaDataBase::MetaInfo info = MetaDataBase::metaInfo( fw );
    if ( info.classNameChanged && !info.className.isEmpty() )
	editClassName->setText( info.className );
    else
	editClassName->setText( fw->name() );
    editComment->setText( info.comment );
    editAuthor->setText( info.author );

    QValueList<MetaDataBase::Include> includes = MetaDataBase::includes( fw );
    for ( QValueList<MetaDataBase::Include>::Iterator it = includes.begin(); it != includes.end(); ++it ) {
	QListViewItem *item = new QListViewItem( listIncludes );
	item->setText( 0, (*it).header );
	item->setText( 1, (*it).location );
    }

    QStringList forwards = MetaDataBase::forwards( fw );
    for ( QStringList::Iterator it2 = forwards.begin(); it2 != forwards.end(); ++it2 ) {
	QListViewItem *item = new QListViewItem( listForwards );
	item->setText( 0, *it2 );
    }

    buttonRemove->setEnabled( FALSE );
    buttonRemoveForward->setEnabled( FALSE );
    editInclude->setEnabled( FALSE );
    editForward->setEnabled( FALSE );
    comboLocation->setEnabled( FALSE );
    listIncludes->setCurrentItem( listIncludes->firstChild() );
    listIncludes->setSelected( listIncludes->firstChild(), TRUE );
    listForwards->setCurrentItem( listForwards->firstChild() );
    listForwards->setSelected( listForwards->firstChild(), TRUE );
    editClassName->setValidator( new AsciiValidator( editClassName ) );
    editPixmapFunction->setValidator( new AsciiValidator( QString( ":" ), editClassName ) );

    if ( formwindow->savePixmapInline() )
	radioPixmapInline->setChecked( TRUE );
    else
	radioPixmapFunction->setChecked( TRUE );
    editPixmapFunction->setText( formwindow->pixmapLoaderFunction() );
}

void FormSettings::okClicked()
{
    MetaDataBase::MetaInfo info;
    info.className = editClassName->text();
    info.classNameChanged = info.className != QString( formwindow->name() );
    info.comment = editComment->text();
    info.author = editAuthor->text();
    MetaDataBase::setMetaInfo( formwindow, info );

    QListViewItemIterator it( listIncludes );
    QValueList<MetaDataBase::Include> includes;
    for ( ; it.current(); ++it ) {
	MetaDataBase::Include inc;
	inc.header = it.current()->text( 0 );
	inc.location = it.current()->text( 1 );
	includes.append( inc );
    }
    MetaDataBase::setIncludes( formwindow, includes );

    QListViewItemIterator it2( listForwards );
    QStringList forwards;
    for ( ; it2.current(); ++it2 ) {
	QString s = it2.current()->text( 0 );
	s = s.stripWhiteSpace();
	if ( s[ (int)s.length() - 1 ] != ';' )
	    s += ";";
	forwards << s;
    }
    MetaDataBase::setForwards( formwindow, forwards );

    formwindow->commandHistory()->setModified( TRUE );

    if ( formwindow->savePixmapInline() != radioPixmapInline->isChecked() )
	MetaDataBase::clearPixmapArguments( formwindow );

    formwindow->setSavePixmapInline( radioPixmapInline->isChecked() );
    formwindow->setPixmapLoaderFunction( editPixmapFunction->text() );

    accept();
}

void FormSettings::addInclude()
{
    QListViewItem *item = new QListViewItem( listIncludes );
    item->setText( 0, "include.h" );
    item->setText( 1, tr( "global" ) );
    listIncludes->setCurrentItem( item );
    listIncludes->setSelected( item, TRUE );
}

void FormSettings::includeNameChanged( const QString &txt )
{
    QListViewItem *i = listIncludes->currentItem();
    if ( !i )
	return;
    i->setText( 0, txt );
}

void FormSettings::includeAccessChanged( const QString &txt )
{
    QListViewItem *i = listIncludes->currentItem();
    if ( !i )
	return;
    i->setText( 1, txt );
}

void FormSettings::removeInclude()
{
    QListViewItem *i = listIncludes->currentItem();
    if ( !i )
	return;

    delete i;
    if ( listIncludes->firstChild() ) {
	listIncludes->setCurrentItem( listIncludes->firstChild() );
	listIncludes->setSelected( listIncludes->firstChild(), TRUE );
    }
}

void FormSettings::currentIncludeChanged( QListViewItem *i )
{
    if ( !i ) {
	buttonRemove->setEnabled( FALSE );
	editInclude->setEnabled( FALSE );
	comboLocation->setEnabled( FALSE );
	editInclude->blockSignals( TRUE );
	editInclude->setText( "" );
	editInclude->blockSignals( FALSE );
	return;
    }

    buttonRemove->setEnabled( TRUE );
    editInclude->setEnabled( TRUE );
    comboLocation->setEnabled( TRUE );

    editInclude->blockSignals( TRUE );
    editInclude->setText( i->text( 0 ) );
    editInclude->blockSignals( FALSE );

    comboLocation->blockSignals( TRUE );
    comboLocation->setCurrentItem( i->text( 1 ) == tr( "global" ) ? 0 : 1 );
    comboLocation->blockSignals( FALSE );
}

void FormSettings::addForward()
{
    QListViewItem *item = new QListViewItem( listForwards );
    item->setText( 0, "class MyClass;" );
    listForwards->setCurrentItem( item );
    listForwards->setSelected( item, TRUE );
}

void FormSettings::removeForward()
{
    QListViewItem *i = listForwards->currentItem();
    if ( !i )
	return;

    delete i;
    if ( listForwards->firstChild() ) {
	listForwards->setCurrentItem( listForwards->firstChild() );
	listForwards->setSelected( listForwards->firstChild(), TRUE );
    }
}

void FormSettings::forwardNameChanged( const QString &txt )
{
    QListViewItem *i = listForwards->currentItem();
    if ( !i )
	return;
    i->setText( 0, txt );
}

void FormSettings::currentForwardChanged( QListViewItem *i )
{
    if ( !i ) {
	buttonRemoveForward->setEnabled( FALSE );
	editForward->setEnabled( FALSE );
	editForward->blockSignals( TRUE );
	editForward->setText( "" );
	editForward->blockSignals( FALSE );
	return;
    }

    buttonRemoveForward->setEnabled( TRUE );
    editForward->setEnabled( TRUE );

    editForward->blockSignals( TRUE );
    editForward->setText( i->text( 0 ) );
    editForward->blockSignals( FALSE );
}
