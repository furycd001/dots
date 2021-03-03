/****************************************************************************
** $Id: qt/examples/customlayout/flow.h   2.3.2   edited 2001-01-26 $
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

#ifndef FLOW_H
#define FLOW_H

#include <qlayout.h>
#include <qlist.h>

class SimpleFlow : public QLayout
{
public:
    SimpleFlow( QWidget *parent, int border=0, int space=-1,
		const char *name=0 )
	: QLayout( parent, border, space, name ),
	cached_width(0) {}
    SimpleFlow( QLayout* parent, int space=-1, const char *name=0 )
	: QLayout( parent, space, name ),
	cached_width(0) {}
    SimpleFlow( int space=-1, const char *name=0 )
	: QLayout( space, name ),
	cached_width(0) {}

    ~SimpleFlow();

    void addItem( QLayoutItem *item);
    bool hasHeightForWidth() const;
    int heightForWidth( int ) const;
    QSize sizeHint() const;
    QSize minimumSize() const;
    QLayoutIterator iterator();
    QSizePolicy::ExpandData expanding() const;

protected:
    void setGeometry( const QRect& );

private:
    int doLayout( const QRect&, bool testonly = FALSE );
    QList<QLayoutItem> list;
    int cached_width;
    int cached_hfw;

};

#endif
