/****************************************************************************
** $Id: qt/examples/customlayout/card.cpp   2.3.2   edited 2001-01-26 $
**
** Implementing your own layout: flow example
**
** Copyright (C) 1996 by Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "card.h"

class CardLayoutIterator :public QGLayoutIterator
{
public:
    CardLayoutIterator( QList<QLayoutItem> *l )
        : idx( 0 ), list( l )  {}

    QLayoutItem *current();
    QLayoutItem *next();
    QLayoutItem *takeCurrent();

private:
    int idx;
    QList<QLayoutItem> *list;
};

QLayoutItem *CardLayoutIterator::current()
{
    return idx < int( list->count() ) ? list->at( idx ) : 0;
}

QLayoutItem *CardLayoutIterator::next()
{
    idx++; return current();
}

QLayoutItem *CardLayoutIterator::takeCurrent()
{
    return idx < int( list->count() ) ?list->take( idx ) : 0;
}



QLayoutIterator CardLayout::iterator()
{
    return QLayoutIterator(  new CardLayoutIterator( &list )  );
}

CardLayout::~CardLayout()
{
    deleteAllItems();
}

void CardLayout::addItem(  QLayoutItem *item  )
{
    list.append( item );
}

void CardLayout::setGeometry( const QRect &rct )
{
    QLayout::setGeometry( rct );

    QListIterator<QLayoutItem> it( list );
    if ( it.count() == 0 )
        return;

    QLayoutItem *o;

    int i = 0;

    int w = rct.width() - ( list.count() - 1 ) * spacing();
    int h = rct.height() - ( list.count() - 1 ) * spacing();

    while ( ( o=it.current() ) != 0 ) {
        ++it;
        QRect geom( rct.x() + i * spacing(), rct.y() + i * spacing(),
                    w, h  );
        o->setGeometry(  geom  );
        ++i;
    }
}

QSize CardLayout::sizeHint() const
{
    QSize s(0,0);
    int n = list.count();
    if ( n > 0 )
        s = QSize(100,70); //start with a nice default size
    QListIterator<QLayoutItem> it(list);
    QLayoutItem *o;
    while ( (o=it.current()) != 0 ) {
        ++it;
        s = s.expandedTo( o->minimumSize() );
    }
    return s + n*QSize(spacing(),spacing());
}

QSize CardLayout::minimumSize() const
{
    QSize s(0,0);
    int n = list.count();
    QListIterator<QLayoutItem> it(list);
    QLayoutItem *o;
    while ( (o=it.current()) != 0 ) {
        ++it;
        s = s.expandedTo( o->minimumSize() );
    }
    return s + n*QSize(spacing(),spacing());
}

