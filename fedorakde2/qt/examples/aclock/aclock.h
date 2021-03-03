/****************************************************************************
** $Id: qt/examples/aclock/aclock.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef ACLOCK_H
#define ACLOCK_H

#include <qwidget.h>
#include <qdatetime.h>


class AnalogClock : public QWidget		// analog clock widget
{
    Q_OBJECT
public:
    AnalogClock( QWidget *parent=0, const char *name=0 );
    void setAutoMask(bool b);

protected:
    void updateMask();
    void paintEvent( QPaintEvent *);
    
private slots:
    void drawClock( QPainter* );
    void	timeout();

private:
    QTime	time;
};


#endif // ACLOCK_H
