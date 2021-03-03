/****************************************************************************
** $Id: qt/examples/progressbar/progressbar.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <qbuttongroup.h>
#include <qtimer.h>

class QRadioButton;
class QPushButton;
class QProgressBar;

class ProgressBar : public QButtonGroup
{
    Q_OBJECT

public:
    ProgressBar( QWidget *parent = 0, const char *name = 0 );

protected:
    QRadioButton *slow, *normal, *fast;
    QPushButton *start, *pause, *reset;
    QProgressBar *progress;
    QTimer timer;

protected slots:
    void slotStart();
    void slotReset();
    void slotTimeout();

};

#endif
