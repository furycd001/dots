/****************************************************************************
** $Id: qt/examples/customlayout/border.h   2.3.2   edited 2001-01-26 $
**
** Definition of simple flow layout for custom layout example
**
** Created : 979899
**
** Copyright (C) 1997 by Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef BORDER_H
#define BORDER_H

#include <qlayout.h>
#include <qlist.h>

class BorderWidgetItem : public QWidgetItem
{
public:
    BorderWidgetItem( QWidget *w )
        : QWidgetItem( w )
    {}

    void setGeometry( const QRect &r )
    { widget()->setGeometry( r ); }

};

class BorderLayout : public QLayout
{
public:
    enum Position {
        West = 0,
        North,
        South,
        East,
        Center
    };

    struct BorderLayoutStruct
    {
        BorderLayoutStruct( QLayoutItem *i, Position p ) {
            item = i;
            pos = p;
        }

        QLayoutItem *item;
        Position pos;
    };

    enum SizeType {
        Minimum = 0,
        SizeHint
    };

    BorderLayout( QWidget *parent, int border = 0, int autoBorder = -1,
                  const char *name = 0 )
        : QLayout( parent, border, autoBorder, name ), cached( 0, 0 ), mcached( 0, 0 ),
          sizeDirty( TRUE ), msizeDirty( TRUE )
    {}

    BorderLayout( QLayout* parent, int autoBorder = -1, const char *name = 0 )
        : QLayout( parent, autoBorder, name  ), cached( 0, 0 ), mcached( 0, 0 ),
          sizeDirty( TRUE ), msizeDirty( TRUE )
    {}

    BorderLayout( int autoBorder = -1, const char *name = 0 )
        : QLayout( autoBorder, name ), cached( 0, 0 ), mcached( 0, 0 ),
          sizeDirty( TRUE ), msizeDirty( TRUE )
    {}

    ~BorderLayout();

    void addItem( QLayoutItem *item );

    void addWidget( QWidget *widget, Position pos );
    void add( QLayoutItem *item, Position pos );

    bool hasHeightForWidth() const;

    QSize sizeHint() const;
    QSize minimumSize() const;

    QLayoutIterator iterator();

    QSizePolicy::ExpandData expanding() const;

protected:
    void setGeometry( const QRect &rect );

private:
    void doLayout( const QRect &rect, bool testonly = FALSE );
    void calcSize( SizeType st );

    QList<BorderLayoutStruct> list;
    QSize cached, mcached;
    bool sizeDirty, msizeDirty;

};

#endif
