/****************************************************************************
** $Id: qt/examples/lineedits/lineedits.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef LINEDITS_H
#define LINEDITS_H

#include <qgroupbox.h>

class QLineEdit;
class QComboBox;

class LineEdits : public QGroupBox
{
    Q_OBJECT

public:
    LineEdits( QWidget *parent = 0, const char *name = 0 );

protected:
    QLineEdit *lined1, *lined2, *lined3, *lined4;
    QComboBox *combo1, *combo2, *combo3, *combo4;

protected slots:
    void slotEchoChanged( int );
    void slotValidatorChanged( int );
    void slotAlignmentChanged( int );
    void slotReadOnlyChanged( int );
};

#endif
