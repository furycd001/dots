/****************************************************************************
** $Id: qt/examples/dclock/dclock.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef DCLOCK_H
#define DCLOCK_H

#include <qlcdnumber.h>


class DigitalClock : public QLCDNumber		// digital clock widget
{
    Q_OBJECT
public:
    DigitalClock( QWidget *parent=0, const char *name=0 );

protected:					// event handlers
    void	timerEvent( QTimerEvent * );
    void	mousePressEvent( QMouseEvent * );

private slots:					// internal slots
    void	stopDate();
    void	showTime();

private:					// internal data
    void	showDate();

    bool	showingColon;
    int		normalTimer;
    int		showDateTimer;
};


#endif // DCLOCK_H
