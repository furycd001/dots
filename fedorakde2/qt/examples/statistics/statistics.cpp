/****************************************************************************
** $Id: qt/examples/statistics/statistics.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "statistics.h"

#include <qdir.h>
#include <qstringlist.h>
#include <qheader.h>
#include <qcombobox.h>
#include <stdlib.h>

const char* dirs[] = {
    "kernel",
    "tools",
    "widgets",
    "dialogs",
    "xml",
    "table",
    "network",
    "opengl",
    "canvas",
    0
};

Table::Table()
    : QTable( 10, 100, 0, "table" )
{
    setSorting( TRUE );
    horizontalHeader()->setLabel( 0, tr( "File" ) );
    horizontalHeader()->setLabel( 1, tr( "Size (bytes)" ) );
    horizontalHeader()->setLabel( 2, tr( "Use in Sum" ) );
    initTable();
    adjustColumn( 0 );

    // if the user edited something we might need to recalculate the sum
    connect( this, SIGNAL( valueChanged( int, int ) ),
	     this, SLOT( recalcSum( int, int ) ) );
}

void Table::initTable()
{
    // read all QTtsource and header files in one list
    QStringList all;
    int i = 0;
    QString qtdir = getenv( "QTDIR" );
    while ( dirs[ i ] ) {
	QDir dir( qtdir + "/src/" + dirs[ i ] );
	QStringList lst = dir.entryList( "*.cpp; *.h" );
	for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) {
	    if ( ( *it ).contains( "moc" ) )
		continue;
	    all << QString( dirs[ i ] ) + "/" + *it;
	}
	++i;
    }

    // set the number of needed rows of the table
    setNumRows( all.count() + 1 );
    i = 0;
    int sum = 0;

    // insert the data into the table
    for ( QStringList::Iterator it = all.begin(); it != all.end(); ++it ) {
	setText( i, 0, *it );
	QFile f( qtdir + "/src/" + *it );
	setText( i, 1, QString::number( f.size() ) );
	ComboItem *ci = new ComboItem( this, QTableItem::WhenCurrent );
	setItem( i++, 2, ci );
	sum += f.size();
    }

    // last row should show the sum
    TableItem *i1 = new TableItem( this, QTableItem::Never, tr( "Sum" ) );
    setItem( i, 0, i1 );
    TableItem *i2 = new TableItem( this, QTableItem::Never, QString::number( sum ) );
    setItem( i, 1, i2 );
}

void Table::recalcSum( int, int col )
{
    // only recalc if a value in the second or third column changed
    if ( col < 1 || col > 2 )
	return;

    // recalc sum
    int sum = 0;
    for ( int i = 0; i < numRows() - 1; ++i ) {
	if ( text( i, 2 ) == "No" )
	    continue;
	sum += text( i, 1 ).toInt();
    }

    // insert calculated data
    TableItem *i1 = new TableItem( this, QTableItem::Never, tr( "Sum" ) );
    setItem( numRows() - 1, 0, i1 );
    TableItem *i2 = new TableItem( this, QTableItem::Never, QString::number( sum ) );
    setItem( numRows() - 1, 1, i2 );
}

void Table::sortColumn( int col, bool ascending, bool /*wholeRows*/ )
{
    // sum row should not be sorted, so get rid of it for now
    clearCell( numRows() - 1, 0 );
    clearCell( numRows() - 1, 1 );
    // do sort
    QTable::sortColumn( col, ascending, TRUE );
    // re-insert sum row
    recalcSum( 0, 1 );
}



void TableItem::paint( QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected )
{
    QColorGroup g( cg );
    // last row is the sum row - we want to make it better visible by
    // using a red background
    if ( row() == table()->numRows() - 1 )
	g.setColor( QColorGroup::Base, red );
    QTableItem::paint( p, g, cr, selected );
}




ComboItem::ComboItem( QTable *t, EditType et )
    : QTableItem( t, et, "Yes" ), cb( 0 )
{
    // we do not want that this item can be replaced
    setReplaceable( FALSE );
}

QWidget *ComboItem::createEditor() const
{
    // create an editor - a combobox in our case
    ( (ComboItem*)this )->cb = new QComboBox( table()->viewport() );
    cb->insertItem( "Yes" );
    cb->insertItem( "No" );
    // and initialize it
    cb->setCurrentItem( text() == "No" ? 1 : 0 );
    return cb;
}

void ComboItem::setContentFromEditor( QWidget *w )
{
    // the user changed the value of the combobox, so syncronize the
    // value of the item (text), with the value of the combobox
    if ( w->inherits( "QComboBox" ) )
	setText( ( (QComboBox*)w )->currentText() );
    else
	QTableItem::setContentFromEditor( w );
}	

void ComboItem::setText( const QString &s )
{
    if ( cb ) {
	// initialize the combobox from the text
	if ( s == "No" )
	    cb->setCurrentItem( 1 );
	else
	    cb->setCurrentItem( 0 );
    }	
    QTableItem::setText( s );
}
