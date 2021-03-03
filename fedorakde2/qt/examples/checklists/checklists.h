/****************************************************************************
** $Id: qt/examples/checklists/checklists.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef CHECKLISTS_H
#define CHECKLISTS_H

#include <qwidget.h>

class QListView;
class QLabel;

class CheckLists : public QWidget
{
    Q_OBJECT

public:
    CheckLists( QWidget *parent = 0, const char *name = 0 );

protected:
    QListView *lv1, *lv2;
    QLabel *label;

protected slots:
    void copy1to2();
    void copy2to3();

};

#endif
