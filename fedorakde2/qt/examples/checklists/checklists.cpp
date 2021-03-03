/****************************************************************************
** $Id: qt/examples/checklists/checklists.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "checklists.h"

#include <qlistview.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qvaluelist.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qlayout.h>

/*
 * Constructor
 *
 * Create all child widgets of the CheckList Widget
 */

CheckLists::CheckLists( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    QHBoxLayout *lay = new QHBoxLayout( this );
    lay->setMargin( 5 );

    // create a widget which layouts its childs in a column
    QVBoxLayout *vbox1 = new QVBoxLayout( lay );
    vbox1->setMargin( 5 );

    // First child: a Label
    vbox1->addWidget( new QLabel( "Check some items!", this ) );

    // Second child: the ListView
    lv1 = new QListView( this );
    vbox1->addWidget( lv1 );
    lv1->addColumn( "Items" );
    lv1->setRootIsDecorated( TRUE );

    // create a list with 4 ListViewItems which will be parent items of other ListViewItems
    QValueList<QListViewItem *> parentList;

    parentList.append( new QListViewItem( lv1, "Parent Item 1" ) );
    parentList.append( new QListViewItem( lv1, "Parent Item 2" ) );
    parentList.append( new QListViewItem( lv1, "Parent Item 3" ) );
    parentList.append( new QListViewItem( lv1, "Parent Item 4" ) );

    QListViewItem *item = 0;
    unsigned int num = 1;
    // go through the list of parent items...
    for ( QValueList<QListViewItem*>::Iterator it = parentList.begin(); it != parentList.end();
	  ( *it )->setOpen( TRUE ), ++it, num++ ) {
	item = *it;
	// ...and create 5 checkable child ListViewItems for each parent item
	for ( unsigned int i = 1; i <= 5; i++ )
	    (void)new QCheckListItem( item, QString( "%1. Child of Parent %2" ).arg( i ).arg( num ), QCheckListItem::CheckBox );
    }

    // Create another widget for layouting
    QVBoxLayout *tmp = new QVBoxLayout( lay );
    tmp->setMargin( 5 );

    // create a pushbutton
    QPushButton *copy1 = new QPushButton( "  ->	 ", this );
    tmp->addWidget( copy1 );
    copy1->setMaximumWidth( copy1->sizeHint().width() );
    // connect the SIGNAL clicked() of the pushbutton with the SLOT copy1to2()
    connect( copy1, SIGNAL( clicked() ), this, SLOT( copy1to2() ) );

    // another widget for layouting
    QVBoxLayout *vbox2 = new QVBoxLayout( lay );
    vbox2->setMargin( 5 );

    // and another label
    vbox2->addWidget( new QLabel( "Check one item!", this ) );

    // create the second listview
    lv2 = new QListView( this );
    vbox2->addWidget( lv2 );
    lv2->addColumn( "Items" );
    lv2->setRootIsDecorated( TRUE );

    // another widget needed for layouting only
    tmp = new QVBoxLayout( lay );
    tmp->setMargin( 5 );

    // create another pushbutton...
    QPushButton *copy2 = new QPushButton( "  ->	 ", this );
    lay->addWidget( copy2 );
    copy2->setMaximumWidth( copy2->sizeHint().width() );
    // ...and connect its clicked() SIGNAL to the copy2to3() SLOT
    connect( copy2, SIGNAL( clicked() ), this, SLOT( copy2to3() ) );

    tmp = new QVBoxLayout( lay );
    tmp->setMargin( 5 );

    // and create a label which will be at the right of the window
    label = new QLabel( "No Item yet...", this );
    tmp->addWidget( label );
}

/*
 * SLOT copy1to2()
 *
 * Copies all checked ListViewItems from the first ListView to
 * the second one, and inserts them as Radio-ListViewItem.
 */

void CheckLists::copy1to2()
{
    // create an iterator which operates on the first ListView
    QListViewItemIterator it( lv1 );

    lv2->clear();

    // Insert first a controller Item into the second ListView. Always if Radio-ListViewItems
    // are inserted into a Listview, the parent item of these MUST be a controller Item!
    QCheckListItem *item = new QCheckListItem( lv2, "Controller", QCheckListItem::Controller );
    item->setOpen( TRUE );

    // iterate through the first ListView...
    for ( ; it.current(); ++it )
	// ...check state of childs, and...
	if ( it.current()->parent() )
	    // ...if the item is checked...
	    if ( ( (QCheckListItem*)it.current() )->isOn() )
		// ...insert a Radio-ListViewItem with the same text into the second ListView
		(void)new QCheckListItem( item, it.current()->text( 0 ), QCheckListItem::RadioButton );

    if ( item->firstChild() )
	( ( QCheckListItem* )item->firstChild() )->setOn( TRUE );
}

/*
 * SLOT copy2to3()
 *
 * Copies the checked item of the second ListView into the
 * Label at the right.
 */

void CheckLists::copy2to3()
{
    // create an iterator which operates on the second ListView
    QListViewItemIterator it( lv2 );

    label->setText( "No Item checked" );

    // iterate through the second ListView...
    for ( ; it.current(); ++it )
	// ...check state of childs, and...
	if ( it.current()->parent() )
	    // ...if the item is checked...
	    if ( ( (QCheckListItem*)it.current() )->isOn() )
		// ...set the text of the item to the label
		label->setText( it.current()->text( 0 ) );
}

