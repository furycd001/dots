/****************************************************************************
** $Id: qt/examples/tooltip/tooltip.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qwidget.h>
#include <qtooltip.h>


class DynamicTip : public QToolTip
{
public:
    DynamicTip( QWidget * parent );

protected:
    void maybeTip( const QPoint & );
};


class TellMe : public QWidget
{
    Q_OBJECT
public:
    TellMe( QWidget * parent = 0, const char * name = 0 );
    ~TellMe();

    QRect tip( const QPoint & );

protected:
    void paintEvent( QPaintEvent * );
    void mousePressEvent( QMouseEvent * );
    void resizeEvent( QResizeEvent * );

private:
    QRect randomRect();

    QRect r1, r2, r3;
    DynamicTip * t;
};
