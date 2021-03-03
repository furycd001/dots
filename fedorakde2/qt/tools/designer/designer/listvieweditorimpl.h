/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef LISTVIEWEDITORIMPL_H
#define LISTVIEWEDITORIMPL_H

#include "listvieweditor.h"

#include <qmap.h>
#include <qpixmap.h>
#include <qvaluelist.h>

class QPixmap;
class FormWindow;

class ListViewEditor : public ListViewEditorBase
{
    Q_OBJECT

public:
    ListViewEditor( QWidget *parent, QListView *lv, FormWindow *fw );

protected slots:
    void applyClicked();
    void columnClickable(bool);
    void columnDownClicked();
    void columnPixmapChosen();
    void columnPixmapDeleted();
    void columnResizable(bool);
    void columnTextChanged(const QString &);
    void columnUpClicked();
    void currentColumnChanged(QListBoxItem*);
    void currentItemChanged(QListViewItem*);
    void deleteColumnClicked();
    void itemColChanged(int);
    void itemDeleteClicked();
    void itemDownClicked();
    void itemNewClicked();
    void itemNewSubClicked();
    void itemPixmapChoosen();
    void itemPixmapDeleted();
    void itemTextChanged(const QString &);
    void itemUpClicked();
    void itemLeftClicked();
    void itemRightClicked();
    void newColumnClicked();
    void okClicked();
    void initTabPage( const QString &page );

private:
    struct Column
    {
	QListBoxItem *item;
	QString text;
	QPixmap pixmap;
	bool clickable, resizable;
#if defined(Q_FULL_TEMPLATE_INSTANTIATION)
	bool operator==( const Column& ) const { return FALSE; }
#endif                                                                       
    };

private:
    void setupColumns();
    void setupItems();
    Column *findColumn( QListBoxItem *i );
    void transferItems( QListView *from, QListView *to );
    void displayItem( QListViewItem *i, int col );

private:
    QListView *listview;
    QValueList<Column> columns;
    int numColumns;
    FormWindow *formwindow;

};


#endif
