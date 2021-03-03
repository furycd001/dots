/****************************************************************************
** $Id: qt/examples/listboxcombo/listboxcombo.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef LISTBOX_COMBO_H
#define LISTBOX_COMBO_H

#include <qvbox.h>

class QListBox;
class QLabel;

class ListBoxCombo : public QVBox
{
    Q_OBJECT

public:
    ListBoxCombo( QWidget *parent = 0, const char *name = 0 );

protected:
    QListBox *lb1, *lb2;
    QLabel *label1, *label2;

protected slots:
    void slotLeft2Right();
    void slotCombo1Activated( const QString &s );
    void slotCombo2Activated( const QString &s );

};

#endif
