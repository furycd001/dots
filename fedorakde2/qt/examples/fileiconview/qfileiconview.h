/****************************************************************************
** $Id: qt/examples/fileiconview/qfileiconview.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef QTFILEICONVIEW_H
#define QTFILEICONVIEW_H


#include <qiconset.h>
#include <qstring.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qtimer.h>
#include <qiconview.h>

class QtFileIconView;
class QDragObject;
class QResizeEvent;

/*****************************************************************************
 *
 * Class QtFileIconDrag
 *
 *****************************************************************************/

class QtFileIconDrag : public QIconDrag
{
    Q_OBJECT

public:
    QtFileIconDrag( QWidget * dragSource, const char* name = 0 );

    const char* format( int i ) const;
    QByteArray encodedData( const char* mime ) const;
    static bool canDecode( QMimeSource* e );
    void append( const QIconDragItem &item, const QRect &pr, const QRect &tr, const QString &url );

private:
    QStringList urls;

};

/*****************************************************************************
 *
 * Class QtFileIconView
 *
 *****************************************************************************/
class QtFileIconViewItem;
class QtFileIconView : public QIconView
{
    Q_OBJECT

public:
    QtFileIconView( const QString &dir, QWidget *parent = 0, const char *name = 0 );

    enum ViewMode { Large, Small };

    void setViewMode( ViewMode m );
    ViewMode viewMode() const { return vm; }
    void setOpenItem( QtFileIconViewItem *i ) {
	openItem = i;
    }

public slots:
    void setDirectory( const QString &dir );
    void setDirectory( const QDir &dir );
    void newDirectory();
    QDir currentDir();

signals:
    void directoryChanged( const QString & );
    void startReadDir( int dirs );
    void readNextDir();
    void readDirDone();
    void enableUp();
    void disableUp();
    void enableMkdir();
    void disableMkdir();

protected slots:
    void itemDoubleClicked( QIconViewItem *i );
    void slotDropped( QDropEvent *e, const QValueList<QIconDragItem> & );

    void viewLarge();
    void viewSmall();
    void viewBottom();
    void viewRight();
    void flowEast();
    void flowSouth();
    void itemTextTruncate();
    void itemTextWordWrap();
    void sortAscending();
    void sortDescending();
    void arrangeItemsInGrid() {
	QIconView::arrangeItemsInGrid( TRUE );
    }

    void slotRightPressed( QIconViewItem *item );
    void openFolder();

protected:
    void readDir( const QDir &dir );
    virtual QDragObject *dragObject();

    virtual void keyPressEvent( QKeyEvent *e );

    QDir viewDir;
    int newFolderNum;
    QSize sz;
    QPixmap pix;
    ViewMode vm;
    QtFileIconViewItem *openItem;

};

/*****************************************************************************
 *
 * Class QtFileIconViewItem
 *
 *****************************************************************************/

class QtFileIconViewItem : public QIconViewItem
{
    friend class QtFileIconView;

public:
    enum ItemType {
        File = 0,
        Dir,
        Link
    };

    QtFileIconViewItem( QtFileIconView *parent, QFileInfo *fi );

    virtual ~QtFileIconViewItem();

    ItemType type() const
    { return itemType; }
    QString filename() const { return itemFileName; }

    virtual bool acceptDrop( const QMimeSource *e ) const;

    virtual void setText( const QString &text );
    virtual QPixmap *pixmap() const;

    virtual void dragEntered();
    virtual void dragLeft();

    void viewModeChanged( QtFileIconView::ViewMode m );
    void paintItem( QPainter *p, const QColorGroup &cg );

protected:
    virtual void dropped( QDropEvent *e, const QValueList<QIconDragItem> & );

    QString itemFileName;
    QFileInfo *itemFileInfo;
    ItemType itemType;
    bool checkSetText;
    QTimer timer;
    QtFileIconView::ViewMode vm;

};


#endif
