/****************************************************************************
** $Id: qt/examples/statistics/statistics.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef STATISTICS_H
#define STATISTICS_H

#include <qtable.h>
#include <qcombobox.h>

class TableItem : public QTableItem
{
public:
    TableItem( QTable *t, EditType et, const QString &txt ) : QTableItem( t, et, txt ) {}
    void paint( QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected );
};

class ComboItem : public QTableItem
{
public:
    ComboItem( QTable *t, EditType et );
    QWidget *createEditor() const;
    void setContentFromEditor( QWidget *w );
    void setText( const QString &s );
    
private:
    QComboBox *cb;

};

class Table : public QTable
{
    Q_OBJECT

public:
    Table();
    void sortColumn( int col, bool ascending, bool wholeRows );

private slots:
    void recalcSum( int row, int col );

private:
    void initTable();

};

#endif
