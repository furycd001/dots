/****************************************************************************
** $Id: qt/src/iconview/qiconview.h   2.3.2   edited 2001-01-26 $
**
** Definition of QIconView widget class
**
** Created : 990707
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the iconview module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QICONVIEW_H
#define QICONVIEW_H

#ifndef QT_H
#include <qscrollview.h>
#include <qiconset.h>
#include <qstring.h>
#include <qrect.h>
#include <qpoint.h>
#include <qsize.h>
#include <qfont.h>
#include <qlist.h>
#include <qdragobject.h>
#include <qstringlist.h>
#include <qbitmap.h>
#endif // QT_H

#include <qmodules.h>

#if !defined(QT_MODULE_ICONVIEW)
#define QM_EXPORT
#else
#define QM_EXPORT Q_EXPORT
#endif

#ifndef QT_NO_ICONVIEW

class QIconView;
class QFontMetrics;
class QPainter;
class QMimeSource;
class QDragObject;
class QMouseEvent;
class QDragEnterEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QKeyEvent;
class QFocusEvent;
class QShowEvent;

class QIconViewPrivate;
struct QIconViewItemPrivate;
class QIconViewItem;
class QIconViewItemLineEdit;

class QStringList;

#ifndef QT_NO_DRAGANDDROP

/*****************************************************************************
 *
 * Class QIconDragItem
 *
 *****************************************************************************/

class QM_EXPORT QIconDragItem
{
public:
    QIconDragItem();
    virtual ~QIconDragItem();
    virtual QByteArray data() const;
    virtual void setData( const QByteArray &d );
    bool operator== ( const QIconDragItem& ) const;

private:
    QByteArray ba;

};


/*****************************************************************************
 *
 * Class QIconDrag
 *
 *****************************************************************************/

struct QIconDragPrivate;

class QM_EXPORT QIconDrag : public QDragObject
{
    Q_OBJECT
public:
    QIconDrag( QWidget * dragSource, const char* name = 0 );
    virtual ~QIconDrag();

    void append( const QIconDragItem &item, const QRect &pr, const QRect &tr );

    virtual const char* format( int i ) const;
    static bool canDecode( QMimeSource* e );
    virtual QByteArray encodedData( const char* mime ) const;

private:
    QIconDragPrivate *d;
    QChar endMark;

    friend class QIconView;
    friend class QIconViewPrivate;
};

#endif

/*****************************************************************************
 *
 * Class QIconViewItem
 *
 *****************************************************************************/

class QIconViewToolTip;

class QM_EXPORT QIconViewItem : public Qt
{
    friend class QIconView;
    friend class QIconViewToolTip;
    friend class QIconViewItemLineEdit;

public:
    QIconViewItem( QIconView *parent );
    QIconViewItem( QIconView *parent, QIconViewItem *after );
    QIconViewItem( QIconView *parent, const QString &text );
    QIconViewItem( QIconView *parent, QIconViewItem *after, const QString &text );
    QIconViewItem( QIconView *parent, const QString &text, const QPixmap &icon );
    QIconViewItem( QIconView *parent, QIconViewItem *after, const QString &text, const QPixmap &icon );
    virtual ~QIconViewItem();

    virtual void setRenameEnabled( bool allow );
    virtual void setDragEnabled( bool allow );
    virtual void setDropEnabled( bool allow );

    virtual QString text() const;
    virtual QPixmap *pixmap() const;
    virtual QString key() const;

    bool renameEnabled() const;
    bool dragEnabled() const;
    bool dropEnabled() const;

    QIconView *iconView() const;
    QIconViewItem *prevItem() const;
    QIconViewItem *nextItem() const;

    int index() const;

    virtual void setSelected( bool s, bool cb );
    virtual void setSelected( bool s );
    virtual void setSelectable( bool s );

    bool isSelected() const;
    bool isSelectable() const;

    virtual void repaint();

    virtual void move( int x, int y );
    virtual void moveBy( int dx, int dy );
    virtual void move( const QPoint &pnt );
    virtual void moveBy( const QPoint &pnt );

    QRect rect() const;
    int x() const;
    int y() const;
    int width() const;
    int height() const;
    QSize size() const;
    QPoint pos() const;
    QRect textRect( bool relative = TRUE ) const;
    QRect pixmapRect( bool relative = TRUE ) const;
    bool contains( QPoint pnt ) const;
    bool intersects( QRect r ) const;

    virtual bool acceptDrop( const QMimeSource *mime ) const;

    void rename();

    virtual int compare( QIconViewItem *i ) const;

    virtual void setText( const QString &text );
    virtual void setPixmap( const QPixmap &icon );
    virtual void setText( const QString &text, bool recalc, bool redraw = TRUE );
    virtual void setPixmap( const QPixmap &icon, bool recalc, bool redraw = TRUE );
    virtual void setKey( const QString &k );

protected:
    virtual void removeRenameBox();
    virtual void calcRect( const QString &text_ = QString::null );
    virtual void paintItem( QPainter *p, const QColorGroup &cg );
    virtual void paintFocus( QPainter *p, const QColorGroup &cg );
#ifndef QT_NO_DRAGANDDROP
    virtual void dropped( QDropEvent *e, const QValueList<QIconDragItem> &lst );
#endif
    virtual void dragEntered();
    virtual void dragLeft();
    void setItemRect( const QRect &r );
    void setTextRect( const QRect &r );
    void setPixmapRect( const QRect &r );
    void calcTmpText();

private:
    void init( QIconViewItem *after = 0 );
    void renameItem();
    void cancelRenameItem();
    void checkRect();

    QIconView *view;
    QString itemText, itemKey;
    QString tmpText;
    QPixmap *itemIcon;
    QIconViewItem *prev, *next;
    uint allow_rename : 1;
    uint allow_drag : 1;
    uint allow_drop : 1;
    uint selected : 1;
    uint selectable : 1;
    uint dirty : 1;
    uint wordWrapDirty : 1;
    QRect itemRect, itemTextRect, itemIconRect;
    QIconViewItemLineEdit *renameBox;
    QRect oldRect;

    QIconViewItemPrivate *d;

};


/*****************************************************************************
 *
 * Class QIconView
 *
 *****************************************************************************/

class QM_EXPORT QIconView : public QScrollView
{
    friend class QIconViewItem;
    friend class QIconViewPrivate;
    friend class QIconViewToolTip;

    Q_OBJECT
    // #### sorting and sort direction do not work
    Q_ENUMS( SelectionMode ItemTextPos Arrangement ResizeMode )
    Q_PROPERTY( bool sorting READ sorting )
    Q_PROPERTY( bool sortDirection READ sortDirection )
    Q_PROPERTY( SelectionMode selectionMode READ selectionMode WRITE setSelectionMode )
    Q_PROPERTY( int gridX READ gridX WRITE setGridX )
    Q_PROPERTY( int gridY READ gridY WRITE setGridY )
    Q_PROPERTY( int spacing READ spacing WRITE setSpacing )
    Q_PROPERTY( ItemTextPos itemTextPos READ itemTextPos WRITE setItemTextPos )
    Q_PROPERTY( QBrush itemTextBackground READ itemTextBackground WRITE setItemTextBackground )
    Q_PROPERTY( Arrangement arrangement READ arrangement WRITE setArrangement )
    Q_PROPERTY( ResizeMode resizeMode READ resizeMode WRITE setResizeMode )
    Q_PROPERTY( int maxItemWidth READ maxItemWidth WRITE setMaxItemWidth )
    Q_PROPERTY( int maxItemTextLength READ maxItemTextLength WRITE setMaxItemTextLength )
    Q_PROPERTY( bool autoArrange READ autoArrange WRITE setAutoArrange )
    Q_PROPERTY( bool itemsMovable READ itemsMovable WRITE setItemsMovable )
    Q_PROPERTY( bool wordWrapIconText READ wordWrapIconText WRITE setWordWrapIconText )
    Q_PROPERTY( bool showToolTips READ showToolTips WRITE setShowToolTips )
    Q_PROPERTY( uint count READ count )

public:
    enum SelectionMode {
	Single = 0,
	Multi,
	Extended,
	NoSelection
    };
    enum Arrangement {
	LeftToRight = 0,
	TopToBottom
    };
    enum ResizeMode {
	Fixed = 0,
	Adjust
    };
    enum ItemTextPos {
	Bottom = 0,
	Right
    };

    QIconView( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
    virtual ~QIconView();

    virtual void insertItem( QIconViewItem *item, QIconViewItem *after = 0L );
    virtual void takeItem( QIconViewItem *item );

    int index( const QIconViewItem *item ) const;

    QIconViewItem *firstItem() const;
    QIconViewItem *lastItem() const;
    QIconViewItem *currentItem() const;
    virtual void setCurrentItem( QIconViewItem *item );
    virtual void setSelected( QIconViewItem *item, bool s, bool cb = FALSE );

    uint count() const;

public:
    virtual void showEvent( QShowEvent * );

    virtual void setSelectionMode( SelectionMode m );
    SelectionMode selectionMode() const;

    QIconViewItem *findItem( const QPoint &pos ) const;
    QIconViewItem *findItem( const QString &text ) const;
    virtual void selectAll( bool select );
    virtual void clearSelection();
    virtual void invertSelection();

    virtual void repaintItem( QIconViewItem *item );

    void ensureItemVisible( QIconViewItem *item );
    QIconViewItem* findFirstVisibleItem( const QRect &r ) const;
    QIconViewItem* findLastVisibleItem( const QRect &r ) const;

    virtual void clear();

    virtual void setGridX( int rx );
    virtual void setGridY( int ry );
    int gridX() const;
    int gridY() const;
    virtual void setSpacing( int sp );
    int spacing() const;
    virtual void setItemTextPos( ItemTextPos pos );
    ItemTextPos itemTextPos() const;
    virtual void setItemTextBackground( const QBrush &b );
    QBrush itemTextBackground() const;
    virtual void setArrangement( Arrangement am );
    Arrangement arrangement() const;
    virtual void setResizeMode( ResizeMode am );
    ResizeMode resizeMode() const;
    virtual void setMaxItemWidth( int w );
    int maxItemWidth() const;
    virtual void setMaxItemTextLength( int w );
    int maxItemTextLength() const;
    virtual void setAutoArrange( bool b );
    bool autoArrange() const;
    virtual void setShowToolTips( bool b );
    bool showToolTips() const;

    void setSorting( bool sort, bool ascending = TRUE );
    bool sorting() const;
    bool sortDirection() const;

    virtual void setItemsMovable( bool b );
    bool itemsMovable() const;
    virtual void setWordWrapIconText( bool b );
    bool wordWrapIconText() const;

    bool eventFilter( QObject * o, QEvent * );

    QSize minimumSizeHint() const;
    QSizePolicy sizePolicy() const;
    QSize sizeHint() const;

    virtual void sort( bool ascending = TRUE );

    virtual void setFont( const QFont & );
    virtual void setPalette( const QPalette & );

public slots:
    virtual void arrangeItemsInGrid( const QSize &grid, bool update = TRUE );
    virtual void arrangeItemsInGrid( bool update = TRUE );
    virtual void setContentsPos( int x, int y );
    virtual void updateContents();

signals:
    void selectionChanged();
    void selectionChanged( QIconViewItem *item );
    void currentChanged( QIconViewItem *item );
    void clicked( QIconViewItem * );
    void clicked( QIconViewItem *, const QPoint & );
    void pressed( QIconViewItem * );
    void pressed( QIconViewItem *, const QPoint & );

    void doubleClicked( QIconViewItem *item );
    void returnPressed( QIconViewItem *item );
    void rightButtonClicked( QIconViewItem* item, const QPoint& pos );
    void rightButtonPressed( QIconViewItem* item, const QPoint& pos );
    void mouseButtonPressed( int button, QIconViewItem* item, const QPoint& pos );
    void mouseButtonClicked( int button, QIconViewItem* item, const QPoint& pos );

#ifndef QT_NO_DRAGANDDROP
    void dropped( QDropEvent *e, const QValueList<QIconDragItem> &lst );
#endif
    void moved();
    void onItem( QIconViewItem *item );
    void onViewport();
    void itemRenamed( QIconViewItem *item, const QString & );
    void itemRenamed( QIconViewItem *item );

protected slots:
    virtual void doAutoScroll();
    virtual void adjustItems();
    virtual void slotUpdate();

private slots:
    void clearInputString();
    void movedContents( int dx, int dy );

protected:
    virtual void drawContents( QPainter *p, int cx, int cy, int cw, int ch );
    virtual void contentsMousePressEvent( QMouseEvent *e );
    virtual void contentsMouseReleaseEvent( QMouseEvent *e );
    virtual void contentsMouseMoveEvent( QMouseEvent *e );
    virtual void contentsMouseDoubleClickEvent( QMouseEvent *e );

#ifndef QT_NO_DRAGANDDROP
    virtual void contentsDragEnterEvent( QDragEnterEvent *e );
    virtual void contentsDragMoveEvent( QDragMoveEvent *e );
    virtual void contentsDragLeaveEvent( QDragLeaveEvent *e );
    virtual void contentsDropEvent( QDropEvent *e );
#endif

    virtual void resizeEvent( QResizeEvent* e );
    virtual void keyPressEvent( QKeyEvent *e );
    virtual void focusInEvent( QFocusEvent *e );
    virtual void focusOutEvent( QFocusEvent *e );
    virtual void enterEvent( QEvent *e );

    virtual void drawRubber( QPainter *p );
#ifndef QT_NO_DRAGANDDROP
    virtual QDragObject *dragObject();
    virtual void startDrag();
#endif
    virtual void insertInGrid( QIconViewItem *item );
    virtual void drawBackground( QPainter *p, const QRect &r );

    void emitSelectionChanged( QIconViewItem * i = 0 );
    void emitRenamed( QIconViewItem *item );

    QIconViewItem *makeRowLayout( QIconViewItem *begin, int &y );

    void styleChange( QStyle& );

private:
    virtual void drawDragShapes( const QPoint &pnt );
#ifndef QT_NO_DRAGANDDROP
    virtual void initDragEnter( QDropEvent *e );
#endif
    void drawContents( QPainter* );
    void findItemByName( const QString &text );
    void handleItemChange( QIconViewItem *old, bool shift, bool control );

    int calcGridNum( int w, int x ) const;
    QIconViewItem *rowBegin( QIconViewItem *item ) const;
    void updateItemContainer( QIconViewItem *item );
    void appendItemContainer();
    void rebuildContainers();
    QBitmap mask( QPixmap *pix ) const;

    QIconViewPrivate *d;

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    QIconView( const QIconView & );
    QIconView& operator=( const QIconView & );
#endif
};

#endif // QT_NO_ICONVIEW

#endif // QICONVIEW_H
