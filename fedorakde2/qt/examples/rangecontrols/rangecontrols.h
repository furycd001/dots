/****************************************************************************
** $Id: qt/examples/rangecontrols/rangecontrols.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef RANGECONTROLS_H
#define RANGECONTROLS_H

#include <qvbox.h>

class QCheckBox;

class RangeControls : public QVBox
{
    Q_OBJECT

public:
    RangeControls( QWidget *parent = 0, const char *name = 0 );

private:
    QCheckBox *notches, *wrapping;
};

#endif
