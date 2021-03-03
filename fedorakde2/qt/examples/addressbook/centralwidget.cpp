/****************************************************************************
** $Id: qt/examples/addressbook/centralwidget.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright ( C ) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "centralwidget.h"

#include <qtabwidget.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qfile.h>
#include <qtextstream.h>

ABCentralWidget::ABCentralWidget( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    mainGrid = new QGridLayout( this, 2, 1, 5, 5 );

    setupTabWidget();
    setupListView();

    mainGrid->setRowStretch( 0, 0 );
    mainGrid->setRowStretch( 1, 1 );
}

void ABCentralWidget::save( const QString &filename )
{
    if ( !listView->firstChild() )
        return;

    QFile f( filename );
    if ( !f.open( IO_WriteOnly ) )
        return;

    QTextStream t( &f );

    QListViewItemIterator it( listView );

    for ( ; it.current(); ++it )
        for ( unsigned int i = 0; i < 4; i++ )
            t << it.current()->text( i ) << "\n";

    f.close();
}

void ABCentralWidget::load( const QString &filename )
{
    listView->clear();

    QFile f( filename );
    if ( !f.open( IO_ReadOnly ) )
        return;

    QTextStream t( &f );

    while ( !t.eof() ) {
        QListViewItem *item = new QListViewItem( listView );
        for ( unsigned int i = 0; i < 4; i++ )
            item->setText( i, t.readLine() );
    }

    f.close();
}

void ABCentralWidget::setupTabWidget()
{
    tabWidget = new QTabWidget( this );

    QWidget *input = new QWidget( tabWidget );
    QGridLayout *grid1 = new QGridLayout( input, 2, 5, 5, 5 );

    QLabel *liFirstName = new QLabel( "First &Name", input );
    liFirstName->resize( liFirstName->sizeHint() );
    grid1->addWidget( liFirstName, 0, 0 );

    QLabel *liLastName = new QLabel( "&Last Name", input );
    liLastName->resize( liLastName->sizeHint() );
    grid1->addWidget( liLastName, 0, 1 );

    QLabel *liAddress = new QLabel( "&Address", input );
    liAddress->resize( liAddress->sizeHint() );
    grid1->addWidget( liAddress, 0, 2 );

    QLabel *liEMail = new QLabel( "&E-Mail", input );
    liEMail->resize( liEMail->sizeHint() );
    grid1->addWidget( liEMail, 0, 3 );

    add = new QPushButton( "&Add", input );
    add->resize( add->sizeHint() );
    grid1->addWidget( add, 0, 4 );
    connect( add, SIGNAL( clicked() ), this, SLOT( addEntry() ) );

    iFirstName = new QLineEdit( input );
    iFirstName->resize( iFirstName->sizeHint() );
    grid1->addWidget( iFirstName, 1, 0 );
    liFirstName->setBuddy( iFirstName );

    iLastName = new QLineEdit( input );
    iLastName->resize( iLastName->sizeHint() );
    grid1->addWidget( iLastName, 1, 1 );
    liLastName->setBuddy( iLastName );

    iAddress = new QLineEdit( input );
    iAddress->resize( iAddress->sizeHint() );
    grid1->addWidget( iAddress, 1, 2 );
    liAddress->setBuddy( iAddress );

    iEMail = new QLineEdit( input );
    iEMail->resize( iEMail->sizeHint() );
    grid1->addWidget( iEMail, 1, 3 );
    liEMail->setBuddy( iEMail );

    change = new QPushButton( "&Change", input );
    change->resize( change->sizeHint() );
    grid1->addWidget( change, 1, 4 );
    connect( change, SIGNAL( clicked() ), this, SLOT( changeEntry() ) );

    tabWidget->addTab( input, "&Add/Change Entry" );

    // --------------------------------------

    QWidget *search = new QWidget( this );
    QGridLayout *grid2 = new QGridLayout( search, 2, 5, 5, 5 );

    cFirstName = new QCheckBox( "First &Name", search );
    cFirstName->resize( cFirstName->sizeHint() );
    grid2->addWidget( cFirstName, 0, 0 );
    connect( cFirstName, SIGNAL( clicked() ), this, SLOT( toggleFirstName() ) );

    cLastName = new QCheckBox( "&Last Name", search );
    cLastName->resize( cLastName->sizeHint() );
    grid2->addWidget( cLastName, 0, 1 );
    connect( cLastName, SIGNAL( clicked() ), this, SLOT( toggleLastName() ) );

    cAddress = new QCheckBox( "&Address", search );
    cAddress->resize( cAddress->sizeHint() );
    grid2->addWidget( cAddress, 0, 2 );
    connect( cAddress, SIGNAL( clicked() ), this, SLOT( toggleAddress() ) );

    cEMail = new QCheckBox( "&E-Mail", search );
    cEMail->resize( cEMail->sizeHint() );
    grid2->addWidget( cEMail, 0, 3 );
    connect( cEMail, SIGNAL( clicked() ), this, SLOT( toggleEMail() ) );

    sFirstName = new QLineEdit( search );
    sFirstName->resize( sFirstName->sizeHint() );
    grid2->addWidget( sFirstName, 1, 0 );

    sLastName = new QLineEdit( search );
    sLastName->resize( sLastName->sizeHint() );
    grid2->addWidget( sLastName, 1, 1 );

    sAddress = new QLineEdit( search );
    sAddress->resize( sAddress->sizeHint() );
    grid2->addWidget( sAddress, 1, 2 );

    sEMail = new QLineEdit( search );
    sEMail->resize( sEMail->sizeHint() );
    grid2->addWidget( sEMail, 1, 3 );

    find = new QPushButton( "&Find", search );
    find->resize( find->sizeHint() );
    grid2->addWidget( find, 1, 4 );
    connect( find, SIGNAL( clicked() ), this, SLOT( findEntries() ) );

    cFirstName->setChecked( TRUE );
    sFirstName->setEnabled( TRUE );
    sLastName->setEnabled( FALSE );
    sAddress->setEnabled( FALSE );
    sEMail->setEnabled( FALSE );

    tabWidget->addTab( search, "&Search" );

    mainGrid->addWidget( tabWidget, 0, 0 );
}

void ABCentralWidget::setupListView()
{
    listView = new QListView( this );
    listView->addColumn( "First Name" );
    listView->addColumn( "Last Name" );
    listView->addColumn( "Address" );
    listView->addColumn( "E-Mail" );

    listView->setSelectionMode( QListView::Extended );

    connect( listView, SIGNAL( clicked( QListViewItem* ) ), this, SLOT( itemSelected( QListViewItem* ) ) );

    mainGrid->addWidget( listView, 1, 0 );
    listView->setAllColumnsShowFocus( TRUE );
}

void ABCentralWidget::addEntry()
{
    if ( !iFirstName->text().isEmpty() || !iLastName->text().isEmpty() ||
         !iAddress->text().isEmpty() || !iEMail->text().isEmpty() ) {
        QListViewItem *item = new QListViewItem( listView );
        item->setText( 0, iFirstName->text() );
        item->setText( 1, iLastName->text() );
        item->setText( 2, iAddress->text() );
        item->setText( 3, iEMail->text() );
    }

    iFirstName->setText( "" );
    iLastName->setText( "" );
    iAddress->setText( "" );
    iEMail->setText( "" );
}

void ABCentralWidget::changeEntry()
{
    QListViewItem *item = listView->currentItem();

    if ( item &&
         ( !iFirstName->text().isEmpty() || !iLastName->text().isEmpty() ||
           !iAddress->text().isEmpty() || !iEMail->text().isEmpty() ) ) {
        item->setText( 0, iFirstName->text() );
        item->setText( 1, iLastName->text() );
        item->setText( 2, iAddress->text() );
        item->setText( 3, iEMail->text() );
    }
}

void ABCentralWidget::selectionChanged()
{
    iFirstName->setText( "" );
    iLastName->setText( "" );
    iAddress->setText( "" );
    iEMail->setText( "" );
}

void ABCentralWidget::itemSelected( QListViewItem *item )
{
    if ( !item )
	return;
    item->setSelected( TRUE );
    item->repaint();

    iFirstName->setText( item->text( 0 ) );
    iLastName->setText( item->text( 1 ) );
    iAddress->setText( item->text( 2 ) );
    iEMail->setText( item->text( 3 ) );
}

void ABCentralWidget::toggleFirstName()
{
    sFirstName->setText( "" );

    if ( cFirstName->isChecked() ) {
        sFirstName->setEnabled( TRUE );
        sFirstName->setFocus();
    }
    else
        sFirstName->setEnabled( FALSE );
}

void ABCentralWidget::toggleLastName()
{
    sLastName->setText( "" );

    if ( cLastName->isChecked() ) {
        sLastName->setEnabled( TRUE );
        sLastName->setFocus();
    }
    else
        sLastName->setEnabled( FALSE );
}

void ABCentralWidget::toggleAddress()
{
    sAddress->setText( "" );

    if ( cAddress->isChecked() ) {
        sAddress->setEnabled( TRUE );
        sAddress->setFocus();
    }
    else
        sAddress->setEnabled( FALSE );
}

void ABCentralWidget::toggleEMail()
{
    sEMail->setText( "" );

    if ( cEMail->isChecked() ) {
        sEMail->setEnabled( TRUE );
        sEMail->setFocus();
    }
    else
        sEMail->setEnabled( FALSE );
}

void ABCentralWidget::findEntries()
{
    if ( !cFirstName->isChecked() &&
         !cLastName->isChecked() &&
         !cAddress->isChecked() &&
         !cEMail->isChecked() ) {
        listView->clearSelection();
        return;
    }

    QListViewItemIterator it( listView );

    for ( ; it.current(); ++it ) {
        bool select = TRUE;

        if ( cFirstName->isChecked() ) {
            if ( select && it.current()->text( 0 ).contains( sFirstName->text() ) )
                select = TRUE;
            else
                select = FALSE;
        }
        if ( cLastName->isChecked() ) {
            if ( select && it.current()->text( 1 ).contains( sLastName->text() ) )
                select = TRUE;
            else
                select = FALSE;
        }
        if ( cAddress->isChecked() ) {
            if ( select && it.current()->text( 2 ).contains( sAddress->text() ) )
                select = TRUE;
            else
                select = FALSE;
        }
        if ( cEMail->isChecked() ) {
            if ( select && it.current()->text( 3 ).contains( sEMail->text() ) )
                select = TRUE;
            else
                select = FALSE;
        }

        if ( select )
            it.current()->setSelected( TRUE );
        else
            it.current()->setSelected( FALSE );
        it.current()->repaint();
    }
}
