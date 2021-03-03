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

#ifndef FORMLIST_H
#define FORMLIST_H

#include <qlistview.h>

class FormWindow;
class QResizeEvent;
class QCloseEvent;
class QDropEvent;
class QDragMoveEvent;
class QDragEnterEvent;
class MainWindow;

class FormListItem : public QListViewItem
{
public:
    FormListItem( QListView *parent, const QString &form, const QString &file, FormWindow *fw );

    FormWindow *formWindow() const { return formwindow; }

    void paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int align );
    void updateBackColor();

private:
    FormWindow *formwindow;
    QColor backgroundColor();
    QColor backColor;

};

class FormList : public QListView
{
    Q_OBJECT

public:
    FormList( QWidget *parent , MainWindow *mw );
    void addForm( FormWindow *fw );
    void closed( FormWindow *fw );

    void contentsDropEvent( QDropEvent *e );
    void contentsDragEnterEvent( QDragEnterEvent *e );
    void contentsDragMoveEvent( QDragMoveEvent *e );

public slots:
    void modificationChanged( bool m, FormWindow *fw );
    void fileNameChanged( const QString &s, FormWindow *fw );
    void activeFormChanged( FormWindow *fw );
    void nameChanged( FormWindow *fw );

protected:
    void resizeEvent( QResizeEvent *e );
    void closeEvent( QCloseEvent *e );
    
signals:
    void hidden();

private slots:
    void updateHeader();
    void itemClicked( QListViewItem *i );

private:
    FormListItem *findItem( FormWindow *fw );

private:
    MainWindow *mainWindow;
    
};

#endif
