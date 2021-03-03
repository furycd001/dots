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

#ifndef HIRARCHYVIEW_H
#define HIRARCHYVIEW_H

#include <qvariant.h>
#include <qlistview.h>
#include <qvbox.h>
#include <qlist.h>

class HierarchyView;
class FormWindow;
class QResizeEvent;
class QCloseEvent;
class QPopupMenu;
class QTabWidget;
class QKeyEvent;
class QMouseEvent;
class QWizard;

class HierarchyItem : public QListViewItem
{
public:
    HierarchyItem( QListViewItem *parent, const QString &txt1, const QString &txt2 );
    HierarchyItem( QListView *parent, const QString &txt1, const QString &txt2 );

    void paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int align );
    void updateBackColor();

    void setWidget( QWidget *w );
    QWidget *widget() const;

private:
    QColor backgroundColor();
    QColor backColor;
    QWidget *wid;

};

class HierarchyList : public QListView
{
    Q_OBJECT

public:
    HierarchyList( QWidget *parent, HierarchyView *view );

    void setup();
    void setCurrent( QWidget *w );
    void setOpen( QListViewItem *i, bool b );
    void changeNameOf( QWidget *w, const QString &name );

protected:
    void keyPressEvent( QKeyEvent *e );
    void keyReleaseEvent( QKeyEvent *e );
    void viewportMousePressEvent( QMouseEvent *e );
    void viewportMouseReleaseEvent( QMouseEvent *e );

public slots:
    void addTabPage();
    void removeTabPage();

private:
    void resizeEvent( QResizeEvent *e );
    void insertObject( QObject *o, QListViewItem *parent );
    QWidget *findWidget( QListViewItem *i );
    QListViewItem *findItem( QWidget *w );
    QWidget *current() const;

private slots:
    void updateHeader();
    void objectClicked( QListViewItem *i );
    void showRMBMenu( QListViewItem *, const QPoint & );

private:
    HierarchyView *hierarchyView;
    QPopupMenu *normalMenu, *tabWidgetMenu;
    bool deselect;

};

class HierarchyView : public QVBox
{
    Q_OBJECT

public:
    HierarchyView( QWidget *parent );

    void setFormWindow( FormWindow *fw, QWidget *w );
    FormWindow *formWindow() const;

    void widgetInserted( QWidget *w );
    void widgetRemoved( QWidget *w );
    void widgetsInserted( const QWidgetList &l );
    void widgetsRemoved( const QWidgetList &l );
    void namePropertyChanged( QWidget *w, const QVariant &old );
    void tabsChanged( QTabWidget *w );
    void pagesChanged( QWizard *w );
    void rebuild();
    void closed( FormWindow *fw );

protected:
    void closeEvent( QCloseEvent *e );

signals:
    void hidden();

private:
    FormWindow *formwindow;
    HierarchyList *listview;

};


#endif
