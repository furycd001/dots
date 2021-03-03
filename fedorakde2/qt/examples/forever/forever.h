/****************************************************************************
** $Id: qt/examples/forever/forever.h   2.3.2   edited 2001-01-26 $
**
** Definition of something or other
**
** Created : 979899
**
** Copyright (C) 1997 by Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef FOREVER_H
#define FOREVER_H

#include <qwidget.h>


const int numColors = 120;


class Forever : public QWidget
{
    Q_OBJECT
public:
    Forever( QWidget *parent=0, const char *name=0 );
protected:
    void	timerEvent( QTimerEvent * );
private slots:
    void	updateCaption();
private:
    int		rectangles;
    QColor	colors[numColors];
};


#endif
