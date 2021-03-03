/****************************************************************************
** $Id: qt/examples/customlayout/border.cpp   2.3.2   edited 2001-01-26 $
**
** Implementing your own layout: flow example
**
** Copyright (C) 1996 by Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "border.h"

class BorderLayoutIterator : public QGLayoutIterator
{
public:
    BorderLayoutIterator( const QList<BorderLayout::BorderLayoutStruct> *l )
        : idx( 0 ) , list( (QList<BorderLayout::BorderLayoutStruct>*)l )
    {}

    uint count() const;
    QLayoutItem *current();
    BorderLayout::BorderLayoutStruct *currentStruct();
    void toFirst();
    QLayoutItem *next();
    QLayoutItem *takeCurrent();
    BorderLayoutIterator &operator++();

private:
    int idx;
    QList<BorderLayout::BorderLayoutStruct> *list;

};

uint BorderLayoutIterator::count() const
{
    return list->count();
}

QLayoutItem *BorderLayoutIterator::current()
{
    return idx < (int)count() ? list->at( idx )->item : 0;
}

BorderLayout::BorderLayoutStruct *BorderLayoutIterator::currentStruct()
{
    return idx < (int)count() ? list->at( idx ) : 0;
}

void BorderLayoutIterator::toFirst()
{
    idx = 0;
}

QLayoutItem *BorderLayoutIterator::next()
{
    idx++;
    return current();
}

QLayoutItem *BorderLayoutIterator::takeCurrent()
{
    BorderLayout::BorderLayoutStruct *b
	= idx < int( list->count() ) ? list->take(  idx  ) : 0;
    QLayoutItem *item =  b ? b->item : 0;
    delete b;
    return item;
}

BorderLayoutIterator &BorderLayoutIterator::operator++()
{
    next();
    return *this;
}

BorderLayout::~BorderLayout()
{
    deleteAllItems();
}


void BorderLayout::addItem( QLayoutItem *item )
{
    add( item, West );
}

void BorderLayout::addWidget( QWidget *widget, Position pos )
{
    add( new BorderWidgetItem( widget ), pos );
}

void BorderLayout::add( QLayoutItem *item, Position pos )
{
    list.append( new BorderLayoutStruct( item, pos ) );
    sizeDirty = TRUE; msizeDirty = TRUE;
    calcSize( SizeHint ); calcSize( Minimum );
}

bool BorderLayout::hasHeightForWidth() const
{
    return FALSE;
}

QSize BorderLayout::sizeHint() const
{
    return cached;
}

QSize BorderLayout::minimumSize() const
{
    return cached;
}

QSizePolicy::ExpandData BorderLayout::expanding() const

{
    return QSizePolicy::BothDirections;
}

QLayoutIterator BorderLayout::iterator()
{
    return QLayoutIterator( new BorderLayoutIterator( &list ) );
}

void BorderLayout::setGeometry( const QRect &rct )
{
    QLayout::setGeometry( rct );
    doLayout( rct );
}

void BorderLayout::doLayout( const QRect &rct, bool /*testonly*/ )
{
    int ew = 0, ww = 0, nh = 0, sh = 0;
    int h = 0;

    BorderLayoutIterator it = BorderLayoutIterator( &list );
    BorderLayoutStruct *o;
    BorderLayoutStruct *center = 0;
    while ( ( o = it.currentStruct() ) != 0 ) {
        ++it;

        if ( o->pos == North ) {
            o->item->setGeometry( QRect( rct.x(), nh, rct.width(), o->item->sizeHint().height() ) );
            nh += o->item->geometry().height() + spacing();
        }
        if ( o->pos == South ) {
            o->item->setGeometry( QRect( o->item->geometry().x(), o->item->geometry().y(),
                                         rct.width(), o->item->sizeHint().height() ) );
            sh += o->item->geometry().height() + spacing();
            o->item->setGeometry( QRect( rct.x(), rct.y() + rct.height() - sh + spacing(),
                                         o->item->geometry().width(), o->item->geometry().height() ) );
        }
        if ( o->pos == Center )
            center = o;
    }

    h = rct.height() - nh - sh;

    it.toFirst();
    while ( ( o = it.currentStruct() ) != 0 ) {
        ++it;

        if ( o->pos == West ) {
            o->item->setGeometry( QRect( rct.x() + ww, nh, o->item->sizeHint().width(), h ) );
            ww += o->item->geometry().width() + spacing();
        }
        if ( o->pos == East ) {
            o->item->setGeometry( QRect( o->item->geometry().x(), o->item->geometry().y(),
                                         o->item->sizeHint().width(), h ) );
            ew += o->item->geometry().width() + spacing();
            o->item->setGeometry( QRect( rct.x() + rct.width() - ew + spacing(), nh,
                                         o->item->geometry().width(), o->item->geometry().height() ) );
        }
    }

    if ( center )
        center->item->setGeometry( QRect( ww, nh, rct.width() - ew - ww, h ) );
}

void BorderLayout::calcSize( SizeType st )
{
    if ( ( st == Minimum && !msizeDirty ) ||
         ( st == SizeHint && !sizeDirty ) )
        return;

    int w = 0, h = 0;

    BorderLayoutIterator it = BorderLayoutIterator( &list );
    BorderLayoutStruct *o;
    while ( ( o = it.currentStruct() ) != 0 ) {
        ++it;
        if ( o->pos == North ||
             o->pos == South ) {
            if ( st == Minimum )
                h += o->item->minimumSize().height();
            else
                h += o->item->sizeHint().height();
        }
        else if ( o->pos == West ||
                  o->pos == East ) {
            if ( st == Minimum )
                w += o->item->minimumSize().width();
            else
                w += o->item->sizeHint().width();
        } else {
            if ( st == Minimum ) {
                h += o->item->minimumSize().height();
                w += o->item->minimumSize().width();
            }
            else {
                h += o->item->sizeHint().height();
                w += o->item->sizeHint().width();
            }
        }
    }

    if ( st == Minimum ) {
        msizeDirty = FALSE;
        mcached = QSize( w, h );
    } else {
        sizeDirty = FALSE;
        cached = QSize( w, h );
    }

    return;
}


