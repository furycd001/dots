/****************************************************************************
** $Id: qt/src/iconview/qiconview.cpp   2.3.2   edited 2001-10-14 $
**
** Implementation of QIconView widget class
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

#include "qiconview.h"

#ifndef QT_NO_ICONVIEW

#include "qpixmap.h"
#include "qfontmetrics.h"
#include "qpainter.h"
#include "qevent.h"
#include "qpalette.h"
#include "qmime.h"
#include "qimage.h"
#include "qpen.h"
#include "qbrush.h"
#include "qtimer.h"
#include "qcursor.h"
#include "qkeycode.h"
#include "qapplication.h"
#include "qmultilineedit.h"
#include "qmap.h"
#include "qarray.h"
#include "qlist.h"
#include "qvbox.h"
#include "qtooltip.h"
#include "qbitmap.h"
#include "qpixmapcache.h"
#include "qptrdict.h"
#include "qstringlist.h"

#include <stdlib.h>
#include <math.h>
#include <limits.h>

#define RECT_EXTENSION 300

static const char * const unknown_xpm[] = {
    "32 32 11 1",
    "c c #ffffff",
    "g c #c0c0c0",
    "a c #c0ffc0",
    "h c #a0a0a4",
    "d c #585858",
    "f c #303030",
    "i c #400000",
    "b c #00c000",
    "e c #000000",
    "# c #000000",
    ". c None",
    "...###..........................",
    "...#aa##........................",
    ".###baaa##......................",
    ".#cde#baaa##....................",
    ".#cccdeebaaa##..##f.............",
    ".#cccccdeebaaa##aaa##...........",
    ".#cccccccdeebaaaaaaaa##.........",
    ".#cccccccccdeebaaaaaaa#.........",
    ".#cccccgcgghhebbbbaaaaa#........",
    ".#ccccccgcgggdebbbbbbaa#........",
    ".#cccgcgcgcgghdeebiebbba#.......",
    ".#ccccgcggggggghdeddeeba#.......",
    ".#cgcgcgcggggggggghghdebb#......",
    ".#ccgcggggggggghghghghd#b#......",
    ".#cgcgcggggggggghghghhd#b#......",
    ".#gcggggggggghghghhhhhd#b#......",
    ".#cgcggggggggghghghhhhd#b#......",
    ".#ggggggggghghghhhhhhhdib#......",
    ".#gggggggggghghghhhhhhd#b#......",
    ".#hhggggghghghhhhhhhhhd#b#......",
    ".#ddhhgggghghghhhhhhhhd#b#......",
    "..##ddhhghghhhhhhhhhhhdeb#......",
    "....##ddhhhghhhhhhhhhhd#b#......",
    "......##ddhhhhhhhhhhhhd#b#......",
    "........##ddhhhhhhhhhhd#b#......",
    "..........##ddhhhhhhhhd#b#......",
    "............##ddhhhhhhd#b###....",
    "..............##ddhhhhd#b#####..",
    "................##ddhhd#b######.",
    "..................##dddeb#####..",
    "....................##d#b###....",
    "......................####......"};

static QPixmap *unknown_icon = 0;
static QPixmap *qiv_buffer_pixmap = 0;
static QPixmap *qiv_selection = 0;

static void qt_cleanup_qiv1()
{
    delete unknown_icon;
    unknown_icon = 0;
    delete qiv_selection;
    qiv_selection = 0;
}

static void qt_cleanup_qiv2()
{
    delete qiv_buffer_pixmap;
    qiv_buffer_pixmap = 0;
    delete qiv_selection;
    qiv_selection = 0;
}

#if !defined(_WS_X11_)
static void createSelectionPixmap( const QColorGroup &cg )
{
    qiv_selection = new QPixmap( 2, 2 );
    qiv_selection->fill( Qt::color0 );
    QBitmap m( 2, 2 );
    m.fill( Qt::color1 );
    QPainter p( &m );
    p.setPen( Qt::color0 );
    for ( int j = 0; j < 2; ++j ) {
	p.drawPoint( j % 2, j );
    }
    p.end();
    qiv_selection->setMask( m );
    qiv_selection->fill( cg.highlight() );
}
#endif

static QPixmap *get_qiv_buffer_pixmap( const QSize &s )
{
    if ( !qiv_buffer_pixmap ) {
	qiv_buffer_pixmap = new QPixmap( s );
	qAddPostRoutine( qt_cleanup_qiv2 );
	return qiv_buffer_pixmap;
    }

    qiv_buffer_pixmap->resize( s );
    return qiv_buffer_pixmap;
}

/*****************************************************************************
 *
 * Struct QIconViewPrivate
 *
 *****************************************************************************/

#ifndef QT_NO_DRAGANDDROP

class QIconDragData
{
public:
    QIconDragData();
    QIconDragData( const QRect &ir, const QRect &tr );

    QRect pixmapRect() const;
    QRect textRect() const;

    void setPixmapRect( const QRect &r );
    void setTextRect( const QRect &r );

    QRect iconRect_, textRect_;
    QString key_;

    bool operator==( const QIconDragData &i ) const;
};

class QIconDragDataItem
{
public:
    QIconDragDataItem() {}
    QIconDragDataItem( const QIconDragItem &i1, const QIconDragData &i2 ) : data( i1 ), item( i2 ) {}
    QIconDragItem data;
    QIconDragData item;
    bool operator== ( const QIconDragDataItem& ) const;
};

struct QIconDragPrivate
{
    QValueList<QIconDragDataItem> items;
    static bool decode( QMimeSource* e, QValueList<QIconDragDataItem> &lst );
};

#endif

class QIconViewToolTip;

class QIconViewPrivate
{
public:
    QIconViewItem *firstItem, *lastItem;
    uint count;
    bool mousePressed;
    QIconView::SelectionMode selectionMode;
    QIconViewItem *currentItem, *tmpCurrentItem, *highlightedItem, *startDragItem, *pressedItem, *selectAnchor;
    QRect *rubber;
    QTimer *scrollTimer, *adjustTimer, *updateTimer, *inputTimer,
	*fullRedrawTimer;
    int rastX, rastY, spacing;
    bool cleared, dropped, clearing;
    int dragItems;
    QPoint oldDragPos;
    bool oldDragAcceptAction;
    QIconView::Arrangement arrangement;
    QIconView::ResizeMode resizeMode;
    QSize oldSize;
#ifndef QT_NO_DRAGANDDROP
    QValueList<QIconDragDataItem> iconDragData;
#endif
    bool isIconDrag;
    int numDragItems, cachedW, cachedH;
    int maxItemWidth, maxItemTextLength;
    QPoint dragStart;
    bool drawDragShapes;
    QString currInputString;
    bool dirty, rearrangeEnabled;
    QIconView::ItemTextPos itemTextPos;
    bool reorderItemsWhenInsert;
#ifndef QT_NO_CURSOR
    QCursor oldCursor;
#endif
    bool resortItemsWhenInsert, sortDirection;
    bool wordWrapIconText;
    int cachedContentsX, cachedContentsY;
    QBrush itemTextBrush;
    bool drawAllBack;
    QRegion clipRegion;
    QPoint dragStartPos;
    QFontMetrics *fm;
    int minLeftBearing, minRightBearing;
    bool containerUpdateLocked;
    bool firstSizeHint;
    QIconViewToolTip *toolTip;
    bool showTips;
    QPixmapCache maskCache;
    bool pressedSelected;
    bool dragging;
    QPtrDict<QIconViewItem> selectedItems;

    struct ItemContainer {
	ItemContainer( ItemContainer *pr, ItemContainer *nx, const QRect &r )
	    : p( pr ), n( nx ), rect( r ) {
		items.setAutoDelete( FALSE );
		if ( p )
		    p->n = this;
		if ( n )
		    n->p = this;
	}
	ItemContainer *p, *n;
	QRect rect;
	QList<QIconViewItem> items;
    } *firstContainer, *lastContainer;

    struct SortableItem {
	QIconViewItem *item;
    };
};

/*****************************************************************************
 *
 * Class QIconViewToolTip
 *
 *****************************************************************************/

class QIconViewToolTip : public QToolTip
{
public:
    QIconViewToolTip( QWidget *parent, QIconView *iv );

    void maybeTip( const QPoint &pos );

private:
    QIconView *view;

};

QIconViewToolTip::QIconViewToolTip( QWidget *parent, QIconView *iv )
    : QToolTip( parent ), view( iv )
{
}

void QIconViewToolTip::maybeTip( const QPoint &pos )
{
    if ( !parentWidget() || !view || view->wordWrapIconText() || !view->showToolTips() )
	return;

    QIconViewItem *item = view->findItem( view->viewportToContents( pos ) );
    if ( !item || item->tmpText == item->itemText )
	return;

    QRect r( item->textRect( FALSE ) );
    r = QRect( view->contentsToViewport( QPoint( r.x(), r.y() ) ), QSize( r.width(), r.height() ) );
    QRect r2( item->pixmapRect( FALSE ) );
    r2 = QRect( view->contentsToViewport( QPoint( r2.x(), r2.y() ) ), QSize( r2.width(), r2.height() ) );
    tip( r, r2, item->itemText );
}

/*****************************************************************************
 *
 * Struct QIconViewItemPrivate
 *
 *****************************************************************************/

struct QIconViewItemPrivate
{
    QIconViewPrivate::ItemContainer *container1, *container2;
};

/*****************************************************************************
 *
 * Class QIconViewItemLineEdit
 *
 *****************************************************************************/

class QIconViewItemLineEdit : public QMultiLineEdit
{
    friend class QIconViewItem;

    Q_OBJECT

public:
    QIconViewItemLineEdit( const QString &text, QWidget *parent, QIconViewItem *theItem, const char *name = 0 );

protected:
    void keyPressEvent( QKeyEvent *e );
    void focusOutEvent( QFocusEvent *e );

protected:
    QIconViewItem *item;
    QString startText;

};

QIconViewItemLineEdit::QIconViewItemLineEdit( const QString &text, QWidget *parent,
					      QIconViewItem *theItem, const char *name )
    : QMultiLineEdit( parent, name ), item( theItem ), startText( text )
{
    setWordWrap( QMultiLineEdit::FixedPixelWidth );
    setWrapColumnOrWidth( item->iconView()->maxItemWidth() -
			  ( item->iconView()->itemTextPos() == QIconView::Bottom ?
			    0 : item->pixmapRect().width() ) );
    setWrapPolicy( QMultiLineEdit::Anywhere );
    setMaxLength( item->iconView()->maxItemTextLength() );
    setAlignment( Qt::AlignCenter );
    setText( text );
    clearTableFlags();

    int w = width();
    int h = height();
    w = totalWidth();
    h = totalHeight();
    QSize s( size() );
    resize( w, h );
    if ( s != QSize( w, h ) ) {
	item->calcRect( QMultiLineEdit::text() );
	item->repaint();
    }
    item->calcRect( QMultiLineEdit::text() );
}

void QIconViewItemLineEdit::keyPressEvent( QKeyEvent *e )
{
    if ( e->key()  == Key_Escape ) {
	item->QIconViewItem::setText( startText );
	item->cancelRenameItem();
    } else if ( e->key() == Key_Enter ||
		e->key() == Key_Return ) {
	item->renameItem();
    } else {
	QMultiLineEdit::keyPressEvent( e );
	int w = width();
	int h = height();
	w = totalWidth();
	h = totalHeight();
	QSize s( size() );
	if ( s != QSize( w, h ) && e->key() != Key_Shift && e->key() != Key_Control && e->key() != Key_Alt) {
	    parentWidget()->resize( w + 6, h + 6 );
	    item->calcRect( text() );
	    item->iconView()->moveChild( parentWidget(), item->textRect( FALSE ).x() - 3,
					 item->textRect( FALSE ).y() - 3 );
	    item->repaint();
	}
    }
}

void QIconViewItemLineEdit::focusOutEvent( QFocusEvent *e )
{
    if ( e->reason() != QFocusEvent::Popup )
	item->cancelRenameItem();
}

#ifndef QT_NO_DRAGANDDROP

/*****************************************************************************
 *
 * Class QIconDragItem
 *
 *****************************************************************************/

// NOT REVISED
/*!
  \class QIconDragItem qiconview.h

  \brief The QIconDragItem class is the internal data structure of a QIconDrag

  \module iconview

  This class is used internally in the QIconDrag to store the data of each item
  (in fact, a list of QIconDragItems is used by QIconDrag).

  So, normally for each iconview item which is dragged, a QIconDragItem
  class (or a class derived from QIconDragItem) is created and stored
  in the QIconDrag object.

  See QIconView::dragObject() for more information.

  An example, how to implement this, is in the QtFileIconView example.
  (qt/examples/qfileiconview/qfileiconview.h and qt/examples/qfileiconview/qfileiconview.cpp)
*/

/*!
  Constructs a QIconDragItem with no data.
*/

QIconDragItem::QIconDragItem()
    : ba( strlen( "no data" ) )
{
    memcpy( ba.data(), "no data", strlen( "no data" ) );
}

/*!
  Destructor.
*/

QIconDragItem::~QIconDragItem()
{
}

/*!
  Returns the data of this QIconDragItem.
*/

QByteArray QIconDragItem::data() const
{
    return ba;
}

/*!
  Sets the data of this QIconDragItem.
*/

void QIconDragItem::setData( const QByteArray &d )
{
    ba = d;
}

/*! \reimp */

bool QIconDragItem::operator==( const QIconDragItem &i ) const
{
    return ba == i.ba;
}

/*!
  \reimp
*/

bool QIconDragDataItem::operator==( const QIconDragDataItem &i ) const
{
    return ( i.item == item &&
	     i.data == data );
}

/*!
  \reimp
*/

bool QIconDragData::operator==( const QIconDragData &i ) const
{
    return key_ == i.key_;
}

/*****************************************************************************
 *
 * Class QIconDrag
 *
 *****************************************************************************/

/*!
  \class QIconDrag qiconview.h

  \brief The QIconDrag class is the drag object which is used for moving items in the iconview

  \ingroup draganddrop

  \module iconview

  The QIconDrag is the drag object which is used for moving items in the
  iconview. The QIconDrag stores exact informations about the positions of
  the items, which are dragged, so that each iconview is able to draw drag
  shapes in correct positions. Also the data of each dragged item is stored here.

  If you want to use extended DnD functionality of the QIconView, normally it's
  enough to just create a QIconDrag object in QIconView::dragObject(). Then
  create for each item which should be dragged a QIconDragItem and set the
  data it represents with QIconDragItem::setData() and add this item to the
  drag object using append().

  If you want to offer the data in other mime-types too, derive a class from this
  and implement the needed encoding and decoding here.

  An example, how to implement this, is in the QtFileIconView example
  (qt/examples/qfileiconview/qfileiconview.h and qt/examples/qfileiconview/qfileiconview.cpp)
*/

/*!
  \reimp
*/

QIconDrag::QIconDrag( QWidget * dragSource, const char* name )
    : QDragObject( dragSource, name )
{
    d = new QIconDragPrivate;
}

/*!
  Destructor.
*/

QIconDrag::~QIconDrag()
{
    delete d;
}

/*!
  Appends an icon drag item which should be stored in this
  dragobject and the geometry of it.

  \sa QIconDragItem
*/

void QIconDrag::append( const QIconDragItem &i, const QRect &pr, const QRect &tr )
{
    d->items.append( QIconDragDataItem( i, QIconDragData( pr, tr ) ) );
}

/*!
  \reimp
*/

const char* QIconDrag::format( int i ) const
{
    if ( i == 0 )
	return "application/x-qiconlist";
    return 0;
}

/*!
  Returns the encoded data of the drag object if
  \a mime is application/x-qiconlist.
*/

QByteArray QIconDrag::encodedData( const char* mime ) const
{
    QByteArray a;
    if ( QString( mime ) == "application/x-qiconlist" ) {
	QValueList<QIconDragDataItem>::ConstIterator it = d->items.begin();
	QString s;
	for ( ; it != d->items.end(); ++it ) {
	    QString k( "%1$@@$%2$@@$%3$@@$%4$@@$%5$@@$%6$@@$%7$@@$%8$@@$" );
	    k = k.arg( (*it).item.pixmapRect().x() ).arg( (*it).item.pixmapRect().y() ).arg( (*it).item.pixmapRect().width() ).
		arg( (*it).item.pixmapRect().height() ).arg( (*it).item.textRect().x() ).arg( (*it).item.textRect().y() ).
		arg( (*it).item.textRect().width() ).arg( (*it).item.textRect().height() );
	    k += QString( (*it).data.data() ) + "$@@$";
	    s += k;
	}
	a.resize( s.length() + 1 );
	memcpy( a.data(), s.latin1(), a.size() );
    }

    return a;
}

/*!
  Returns TRUE if \a e can be decoded by the QIconDrag,
  else FALSE.
*/

bool QIconDrag::canDecode( QMimeSource* e )
{
    if ( e->provides( "application/x-qiconlist" ) )
	return TRUE;
    return FALSE;
}

/*!
  Decodes the data which is stored (endocded) in \a e and if successful,
  fills the \a list of icon drag items with the decoded data.
*/

bool QIconDragPrivate::decode( QMimeSource* e, QValueList<QIconDragDataItem> &lst )
{
    QByteArray ba = e->encodedData( "application/x-qiconlist" );
    if ( ba.size() ) {
	lst.clear();
	QString s = ba.data();
	QIconDragDataItem item;
	QRect ir, tr;
	QByteArray d;
	QStringList l = QStringList::split( "$@@$", s );

	int i = 0;
	QStringList::Iterator it = l.begin();
	for ( ; it != l.end(); ++it ) {
	    if ( i == 0 ) {
		ir.setX( ( *it ).toInt() );
	    } else if ( i == 1 ) {
		ir.setY( ( *it ).toInt() );
	    } else if ( i == 2 ) {
		ir.setWidth( ( *it ).toInt() );
	    } else if ( i == 3 ) {
		ir.setHeight( ( *it ).toInt() );
	    } else if ( i == 4 ) {
		tr.setX( ( *it ).toInt() );
	    } else if ( i == 5 ) {
		tr.setY( ( *it ).toInt() );
	    } else if ( i == 6 ) {
		tr.setWidth( ( *it ).toInt() );
	    } else if ( i == 7 ) {
		tr.setHeight( ( *it ).toInt() );
	    } else if ( i == 8 ) {
		d.resize( ( *it ).length() );
		memcpy( d.data(), ( *it ).latin1(), ( *it ).length() );
		item.item.setPixmapRect( ir );
		item.item.setTextRect( tr );
		item.data.setData( d );
		lst.append( item );
	    }
	    ++i;
	    if ( i > 8 )
		i = 0;
	}
	return TRUE;
    }

    return FALSE;
}

QIconDragData::QIconDragData()
    : iconRect_(), textRect_()
{
}

QIconDragData::QIconDragData( const QRect &ir, const QRect &tr )
    : iconRect_( ir ), textRect_( tr )
{
}

QRect QIconDragData::textRect() const
{
    return textRect_;
}

QRect QIconDragData::pixmapRect() const
{
    return iconRect_;
}

void QIconDragData::setPixmapRect( const QRect &r )
{
    iconRect_ = r;
}

void QIconDragData::setTextRect( const QRect &r )
{
    textRect_ = r;
}

#endif

/*****************************************************************************
 *
 * Class QIconViewItem
 *
 *****************************************************************************/

/*!
  \class QIconViewItem qiconview.h
  \brief The QIconViewItem class implements an iconview item.
  \module iconview


  An iconview item is a object containing an icon and a text, which can
  display itself in an iconview.

  The simplest way to create an iconview item and insert it into an
  iconview is to construct it with a pointer to the iconview, a string
  and an icon:

  \code
    \/ parent is a pointer to our iconview, pixmap a QPixmap,
    \/ which we want to use as icon
    (void) new QIconViewItem( parent,
			      "This is the text of the item",
			      pixmap );
  \endcode

  When the iconview is deleted, all items of it are deleted automatically,
  so the programmer need not delete the iconview items itself.

  To iterate over all items of an iconview do something like

  \code
    QIconViewItem *item;
    for ( item = iconview->firstItem(); item; item = item->nextItem() )
      do_something_with( item );
  \endcode

  To remove an item from an iconview, just delete the item. The
  destructor of the QIconViewItem does all the work for removing it
  from the iconview.

  As the iconview is designed to use DnD, the iconview item has
  methods for DnD too which may be reimplemented.

  The class is designed to be very similar to QListView and QListBox
  in use, both via instantiation and subclassing.
*/

/*!
  Constructs an iconview item with no text and a default icon, and
  inserts it into the iconview \a parent.
*/

QIconViewItem::QIconViewItem( QIconView *parent )
    : view( parent ), itemText(), itemIcon( unknown_icon ),
      prev( 0 ), next( 0 ), allow_rename( FALSE ), allow_drag( TRUE ), allow_drop( TRUE ),
      selected( FALSE ), selectable( TRUE ), renameBox( 0 )
{
    init();
}

/*!
  Constructs an iconview item with no text and a default icon, and inserts
  it into the iconview \a parent after the iconview item \a after.
*/

QIconViewItem::QIconViewItem( QIconView *parent, QIconViewItem *after )
    : view( parent ), itemText(), itemIcon( unknown_icon ),
      prev( 0 ), next( 0 ), allow_rename( FALSE ), allow_drag( TRUE ), allow_drop( TRUE ),
      selected( FALSE ), selectable( TRUE ), renameBox( 0 )
{
    init( after );
}

/*!
  Constructs an iconview item using \a text as text and a default icon,
  and inserts it into the iconview \a parent.
*/

QIconViewItem::QIconViewItem( QIconView *parent, const QString &text )
    : view( parent ), itemText( text ), itemIcon( unknown_icon ),
      prev( 0 ), next( 0 ), allow_rename( FALSE ), allow_drag( TRUE ), allow_drop( TRUE ),
      selected( FALSE ), selectable( TRUE ), renameBox( 0 )
{
    init( 0 );
}

/*!
  Constructs an iconview item using \a text as text and a default icon, and
  inserts it into the iconview \a parent after the iconview item \a after.
*/

QIconViewItem::QIconViewItem( QIconView *parent, QIconViewItem *after,
			      const QString &text )
    : view( parent ), itemText( text ), itemIcon( unknown_icon ),
      prev( 0 ), next( 0 ), allow_rename( FALSE ), allow_drag( TRUE ), allow_drop( TRUE ),
      selected( FALSE ), selectable( TRUE ), renameBox( 0 )
{
    init( after );
}

/*!
  Constructs an iconview item using \a text as text and \a icon as icon,
  and inserts it into the iconview \a parent.
*/

QIconViewItem::QIconViewItem( QIconView *parent, const QString &text,
			      const QPixmap &icon )
    : view( parent ), itemText( text ), itemIcon( new QPixmap( icon ) ),
      prev( 0 ), next( 0 ), allow_rename( FALSE ), allow_drag( TRUE ), allow_drop( TRUE ),
      selected( FALSE ), selectable( TRUE ), renameBox( 0 )
{
    init( 0 );
}

/*!
  Constructs an iconview item using \a text as text and \a icon as icon,
  and inserts it into the iconview \a parent after the iconview item \a
  after.
*/

QIconViewItem::QIconViewItem( QIconView *parent, QIconViewItem *after, const QString &text, const QPixmap &icon )
    : view( parent ), itemText( text ), itemIcon( new QPixmap( icon ) ),
      prev( 0 ), next( 0 ), allow_rename( FALSE ), allow_drag( TRUE ), allow_drop( TRUE ),
      selected( FALSE ), selectable( TRUE ), renameBox( 0 )
{
    init( after );
}

/*!
  This private function initializes the iconview item and inserts it
  into the iconview.
*/

void QIconViewItem::init( QIconViewItem *after )
{
    d = new QIconViewItemPrivate;
    d->container1 = 0;
    d->container2 = 0;
    if ( view ) {
	itemKey = itemText;
	dirty = TRUE;
	wordWrapDirty = TRUE;
	calcRect();
	view->insertItem( this, after );
    }
}

/*!
  Destructs the iconview item and tells the iconview about it.
*/

QIconViewItem::~QIconViewItem()
{
    if ( view && !view->d->clearing )
	view->takeItem( this );
    removeRenameBox();
    view = 0;
    if ( itemIcon && itemIcon->serialNumber() != unknown_icon->serialNumber() )
	delete itemIcon;
    delete d;
}

/*!
  Sets \a text as text of the iconview item.  This method might be a no-op
  if you reimplement text().

  \sa text()
*/

void QIconViewItem::setText( const QString &text )
{
    if ( text == itemText )
	return;

    wordWrapDirty = TRUE;
    itemText = text;
    if ( itemKey.isEmpty() )
	itemKey = itemText;

    QRect oR = rect();
    calcRect();
    oR = oR.unite( rect() );

    if ( view ) {
	if ( QRect( view->contentsX(), view->contentsY(),
		    view->visibleWidth(), view->visibleHeight() ).
	     intersects( oR ) )
	    view->repaintContents( oR.x() - 1, oR.y() - 1,
				   oR.width() + 2, oR.height() + 2, FALSE );
    }
}

/*!
  Sets \a k as key of the iconview item. This is used for sorting.

  \sa compareItems()
*/

void QIconViewItem::setKey( const QString &k )
{
    if ( k == itemKey )
	return;

    itemKey = k;
}

/*!
  Sets \a icon as item icon of the iconview item. This method might be
  a no-op if you reimplement pixmap().

  \sa pixmap()
*/

void QIconViewItem::setPixmap( const QPixmap &icon )
{
    if ( itemIcon && itemIcon == unknown_icon )
	itemIcon = 0;

    if ( itemIcon )
	*itemIcon = icon;
    else
	itemIcon = new QPixmap( icon );
    QRect oR = rect();
    calcRect();
    oR = oR.unite( rect() );

    if ( view ) {
	if ( QRect( view->contentsX(), view->contentsY(),
		    view->visibleWidth(), view->visibleHeight() ).
	     intersects( oR ) )
	    view->repaintContents( oR.x() - 1, oR.y() - 1,
				   oR.width() + 2, oR.height() + 2, FALSE );
    }
}

/*!
  Sets \a text as text of the iconview item. If \a recalc is TRUE, the
  iconview's layout is recalculated. If \a redraw is TRUE (the
  default), the icon view is repainted.

  \sa text()
*/

void QIconViewItem::setText( const QString &text, bool recalc, bool redraw )
{
    if ( text == itemText )
	return;

    wordWrapDirty = TRUE;
    itemText = text;

    if ( recalc )
	calcRect();
    if ( redraw )
	repaint();
}

/*!
  Sets \a icon as item icon of the iconview item. If \a recalc is TRUE, the
  iconview's layout is recalculated. If \a redraw is TRUE (the
  default), the icon view is repainted.

  \sa pixmap()
*/

void QIconViewItem::setPixmap( const QPixmap &icon, bool recalc, bool redraw )
{
    if ( itemIcon && itemIcon == unknown_icon )
	itemIcon = 0;

    if ( itemIcon )
	*itemIcon = icon;
    else
	itemIcon = new QPixmap( icon );

    if ( recalc )
	calcRect();
    if ( redraw ) {
	if ( recalc ) {
	    QRect oR = rect();
	    calcRect();
	    oR = oR.unite( rect() );

	    if ( view ) {
		if ( QRect( view->contentsX(), view->contentsY(),
			    view->visibleWidth(), view->visibleHeight() ).
		     intersects( oR ) )
		    view->repaintContents( oR.x() - 1, oR.y() - 1,
				       oR.width() + 2, oR.height() + 2, FALSE );
	    }
	} else {
	    repaint();
	}
    }
}

/*!
  If \a allow is TRUE, the user can rename the iconview item by clicking
  on the text while the item is selected (in-place renaming). If \a
  allow is FALSE, in-place renaming is not possible.
*/

void QIconViewItem::setRenameEnabled( bool allow )
{
    allow_rename = (uint)allow;
}

/*!
  If \a allow is TRUE, the iconview lets the user to drag the iconview
  item (inside the iconview and outside of it). if \a allow is FALSE,
  the item cannot be dragged.
*/

void QIconViewItem::setDragEnabled( bool allow )
{
    allow_drag = (uint)allow;
}

/*!
  If \a allow is TRUE, the iconview lets the user drop something on
  this iconview item.
*/

void QIconViewItem::setDropEnabled( bool allow )
{
    allow_drop = (uint)allow;
}

/*!
  Returns the text of the iconview item. Normally you will set the
  text if the item with setText(), but sometimes it's inconvenient to
  call setText() for each item, so you can subclass QIconViewItem,
  reimplement this method and return the text of the item. If you do
  this you have to call calcRect() manually each time the text (and so
  the size of it) changes.

  \sa setText()
*/

QString QIconViewItem::text() const
{
    return itemText;
}

/*!
  Returns the key of the iconview item.

  \sa setKey(), compareItems()
*/

QString QIconViewItem::key() const
{
    return itemKey;
}

/*!
  Returns the icon of the iconview item. Normally you will set the
  pixmap if the item with setPixmap(), but sometimes it's inconvenient
  to call setText() for each item, so you can subclass QIconViewItem,
  reimplement this method and return a pointer to the item's pixmap.
  If you do this you have to call calcRect() manually each time the
  size of this pixmap changes!

  \sa setPixmap()
*/

QPixmap *QIconViewItem::pixmap() const
{
    return itemIcon;
}

/*!
  Returns TRUE if the item can be renamed by the user with in-place
  renaming, or else FALSE.

  \sa setRenameEnabled()
*/

bool QIconViewItem::renameEnabled() const
{
    return (bool)allow_rename;
}

/*!
  Returns TRUE if the user is allowed to drag the iconview item, or else
  FALSE.

  \sa setDragEnabled()
*/

bool QIconViewItem::dragEnabled() const
{
    return (bool)allow_drag;
}

/*!
  Returns TRUE if the user is allowed to drop something onto the item,
  otherwise FALSE.

  \sa setDropEnabled()
*/

bool QIconViewItem::dropEnabled() const
{
    return (bool)allow_drop;
}

/*!
  Returns a pointer to this items' iconview parent.
*/

QIconView *QIconViewItem::iconView() const
{
    return view;
}

/*!
  Returns a pointer to the previous item, or 0 if this is the first item
  of the iconview.
*/

QIconViewItem *QIconViewItem::prevItem() const
{
    return prev;
}

/*!
  Returns a pointer to the next item, or 0 if this is the last item
  of the iconview.
*/

QIconViewItem *QIconViewItem::nextItem() const
{
    return next;
}

/*!
  Returns the index of this item in the iconview, or -1 if an error occurred.
*/

int QIconViewItem::index() const
{
    if ( view )
	return view->index( this );

    return -1;
}



/*! \overload

  This variant is equivalent to calling the other variant with \a cb
  set to FALSE.
*/

void QIconViewItem::setSelected( bool s )
{
    setSelected( s, FALSE );
}

/*!
  Selects or unselects the item depending on \a s, and may also
  unselect other items, depending on QIconView::selectionMode() and \a
  cb.

  If \a s is FALSE, the item is unselected.

  If \a s is TRUE and QIconView::selectionMode() is \c Single, the item
  is selected, and the item which was selected is unselected.

  If \a s is TRUE and QIconView::selectionMode() is \c Extended, the item
  is selected. If \a cb is TRUE, the other items of the iconview are
  not touched. If \a cb is FALSE all other items are unselected.

  If \a s is TRUE and QIconView::selectionMode() is \c Multi the item
  is selected.

  Note that \a cb is used only if QIconView::selectionMode() is \c
  Extended.

  All items whose selection status change repaint themselves.
*/

void QIconViewItem::setSelected( bool s, bool cb )
{
    if ( !view )
	return;
    if ( view->selectionMode() != QIconView::NoSelection &&
	 selectable && s != (bool)selected ) {

	if ( view->d->selectionMode == QIconView::Single && this != view->d->currentItem ) {
	    QIconViewItem *o = view->d->currentItem;
	    if ( o && o->selected )
		o->selected = FALSE;
	    view->d->currentItem = this;
	    if ( o )
		o->repaint();
	    emit view->currentChanged( this );
	}

	if ( !s ) {
	    selected = FALSE;
	} else {
	    if ( view->d->selectionMode == QIconView::Single && view->d->currentItem ) {
		view->d->currentItem->selected = FALSE;
	    }
	    if ( ( view->d->selectionMode == QIconView::Extended && !cb ) ||
		 view->d->selectionMode == QIconView::Single ) {
		bool b = view->signalsBlocked();
		view->blockSignals( TRUE );
		view->selectAll( FALSE );
		view->blockSignals( b );
	    }
	    selected = s;
	}

	repaint();
	if ( !view->signalsBlocked() ) {
	    bool emitIt = view->d->selectionMode == QIconView::Single && s;
	    QIconView *v = view;
	    emit v->selectionChanged();
	    if ( emitIt )
		emit v->selectionChanged( this );
	}
    }
}

/*!   Sets this items to be selectable if \a enable is TRUE (the
  default) or not to be selectable if \a enable is FALSE.

  The user is not able to select a non-selectable item using either
  the keyboard or mouse.  The application programmer still can, of
  course.  \sa isSelectable() */

void QIconViewItem::setSelectable( bool enable )
{
    selectable = (uint)enable;
}

/*!
  Returns TRUE if the item is selected, or else FALSE.

  \sa setSelected()
*/

bool QIconViewItem::isSelected() const
{
    return (bool)selected;
}

/*!
  Returns TRUE if the item is selectable, or else FALSE.

  \sa setSelectable()
*/

bool QIconViewItem::isSelectable() const
{
    return (bool)selectable;
}

/*!
  Repaints the item.
*/

void QIconViewItem::repaint()
{
    if ( view )
	view->repaintItem( this );
}

/*!
  Moves the item to \a x and \a y in the iconview (these are contents
  coordinates).
*/

void QIconViewItem::move( int x, int y )
{
    itemRect.setRect( x, y, itemRect.width(), itemRect.height() );
    checkRect();
    if ( view )
	view->updateItemContainer( this );
}

/*!
  Moves the item by the distance \a dx and \a dy.
*/

void QIconViewItem::moveBy( int dx, int dy )
{
    itemRect.moveBy( dx, dy );
    checkRect();
    if ( view )
	view->updateItemContainer( this );
}

/*!
  \overload void QIconViewItem::move( const QPoint &pnt  )
*/

void QIconViewItem::move( const QPoint &pnt )
{
    move( pnt.x(), pnt.y() );
}

/*!
  \overload void QIconViewItem::moveBy( const QPoint &pnt )
*/

void QIconViewItem::moveBy( const QPoint &pnt )
{
    moveBy( pnt.x(), pnt.y() );
}

/*!
  Returns the bounding rect of the item (in contents coordinates).
*/

QRect QIconViewItem::rect() const
{
    return itemRect;
}

/*!
  Returns the X-Coordinate of the item (in contents coordinates).
*/

int QIconViewItem::x() const
{
    return itemRect.x();
}

/*!
  Returns the Y-Coordinate of the item (in contents coordinates).
*/

int QIconViewItem::y() const
{
    return itemRect.y();
}

/*!
  Returns the width of the item.
*/

int QIconViewItem::width() const
{
    return QMAX( itemRect.width(), QApplication::globalStrut().width() );
}

/*!
  Returns the height of the item.
*/

int QIconViewItem::height() const
{
    return QMAX( itemRect.height(), QApplication::globalStrut().height() );
}

/*!
  Returns the size of the item.
*/

QSize QIconViewItem::size() const
{
    return QSize( itemRect.width(), itemRect.height() );
}

/*!
  Returns the position of the item (in contents coordinates).
*/

QPoint QIconViewItem::pos() const
{
    return QPoint( itemRect.x(), itemRect.y() );
}

/*!
  Returns the bounding rectangle of the item's text.

  If \a relative is FALSE the returned rectangle is relative to the
  origin of the iconview's contents coordinate system. If \a relative
  is TRUE the rectangle is relative to the origin of the item's
  rectangle.
*/

QRect QIconViewItem::textRect( bool relative ) const
{
    if ( relative )
	return itemTextRect;
    else
	return QRect( x() + itemTextRect.x(), y() + itemTextRect.y(), itemTextRect.width(), itemTextRect.height() );
}

/*!
  Returns the bounding rectangle of the item's icon.

  If \a relative is FALSE the returned rectangle is relative to the
  origin of the iconview's contents coordinate system. If \a relative
  is TRUE the rectangle is relative to the origin of the item's
  rectangle.
*/

QRect QIconViewItem::pixmapRect( bool relative ) const
{
    if ( relative )
	return itemIconRect;
    else
	return QRect( x() + itemIconRect.x(), y() + itemIconRect.y(), itemIconRect.width(), itemIconRect.height() );
}

/*!
  Returns TRUE if the item contains the point \a pnt (in contents
  coordinates), and FALSE if it does not.
*/

bool QIconViewItem::contains( QPoint pnt ) const
{
    return ( textRect( FALSE ).contains( pnt ) ||
	     pixmapRect( FALSE ).contains( pnt ) );
}

/*!
  Returns TRUE, if the item intersects the rectangle \a r (in contents
  coordinates), and FALSE if it does not.
*/

bool QIconViewItem::intersects( QRect r ) const
{
    return ( textRect( FALSE ).intersects( r ) ||
	     pixmapRect( FALSE ).intersects( r ) );
}

/*!
  \fn bool QIconViewItem::acceptDrop( const QMimeSource *mime ) const

  Returns TRUE if the item accepts the QMimeSource \a mime (so it
  could be dropped on the item), and FALSE if it does not.

  The default implementation does nothing and returns always TRUE. A
  subclass must reimplement this to accept drops.
*/

bool QIconViewItem::acceptDrop( const QMimeSource * ) const
{
    return FALSE;
}

/*!
  Starts in-place renaming an icon, if allowed.

  This function sets the icon view up so that the user can edit the
  item text, and then returns. When the user is done, setText() will
  be called and QIconView::itemRenamed() be emitted.
*/

void QIconViewItem::rename()
{
    if ( !view )
	return;
    if ( renameBox )
	removeRenameBox();
    oldRect = rect();
    QVBox *box = new QVBox( view->viewport() );
    renameBox = new QIconViewItemLineEdit( itemText, box, this );
    box->setFrameStyle( QFrame::Plain | QFrame::Box );
    box->setMargin( 2 );
    box->setBackgroundMode( QWidget::PaletteBase );
    box->resize( textRect().width() + view->d->fm->width( ' ' ) + 6, textRect().height() + 6 );
    view->addChild( box, textRect( FALSE ).x() - 3, textRect( FALSE ).y() - 3 );
    renameBox->setFrameStyle( QFrame::NoFrame );
    renameBox->setLineWidth( 0 );
    renameBox->selectAll();
    view->viewport()->setFocusProxy( renameBox );
    renameBox->setFocus();
    box->show();
}

/*!
  Compares this iconview item to \a i. Returns -1 if this item
  is less than \a i, 0 if they are equal and 1 if this iconview item
  is greater than \a i.

  The default implementation uses QIconViewItem::key() to compare the
  items. A reimplementation may use different values.

  \sa key()
*/

int QIconViewItem::compare( QIconViewItem *i ) const
{
    return key().compare( i->key() );
}

/*!
  This private function is called when the user pressed Return in the
  in-place renaming.
*/

void QIconViewItem::renameItem()
{
    if ( !renameBox || !view )
	return;

    if ( !view->d->wordWrapIconText ) {
	wordWrapDirty = TRUE;
	calcRect();
    }
    QRect r = itemRect;
    setText( renameBox->text() );
    view->repaintContents( oldRect.x() - 1, oldRect.y() - 1, oldRect.width() + 2, oldRect.height() + 2, FALSE );
    view->repaintContents( r.x() - 1, r.y() - 1, r.width() + 2, r.height() + 2, FALSE );
    removeRenameBox();

    view->emitRenamed( this );
}

/*!
  Cancels in-place renaming.
*/

void QIconViewItem::cancelRenameItem()
{
    if ( !view )
	return;

    QRect r = itemRect;
    calcRect();
    view->repaintContents( oldRect.x() - 1, oldRect.y() - 1, oldRect.width() + 2, oldRect.height() + 2, FALSE );
    view->repaintContents( r.x() - 1, r.y() - 1, r.width() + 2, r.height() + 2, FALSE );

    if ( !renameBox )
	return;

    removeRenameBox();
}

/*!
  Removes the editbox which is used for in-place renaming.
*/

void QIconViewItem::removeRenameBox()
{
    if ( !renameBox || !view )
	return;

    bool resetFocus = view->viewport()->focusProxy() == renameBox;
    delete renameBox->parentWidget();
    renameBox = 0;
    if ( resetFocus ) {
	view->viewport()->setFocusProxy( view );
	view->setFocus();
    }
}

/*! This virtual function is responsible for calculating the
  rectangles returned by rect(), textRect() and pixmapRect().
  setRect(), setTextRect() and setPixmapRect() are provided mainly for
  reimplementations of this function.
*/

void QIconViewItem::calcRect( const QString &text_ )
{
    if ( !view ) // #####
	return;

    wordWrapDirty = TRUE;
    int pw = 0;
    int ph = 0;

    pw = ( pixmap() ? pixmap() : unknown_icon )->width() + 2;
    ph = ( pixmap() ? pixmap() : unknown_icon )->height() + 2;

    itemIconRect.setWidth( pw );
    itemIconRect.setHeight( ph );

    calcTmpText();

    QString t = text_;
    if ( t.isEmpty() ) {
	if ( view->d->wordWrapIconText )
	    t = itemText;
	else
	    t = tmpText;
    }

    int tw = 0;
    int th = 0;
    // ##### TODO: fix font bearings!
    QRect r;
    if ( view->d->wordWrapIconText ) {
	r = QRect( view->d->fm->boundingRect( 0, 0, iconView()->maxItemWidth() -
					      ( iconView()->itemTextPos() == QIconView::Bottom ? 0 :
						pixmapRect().width() ) - 4,
					      0xFFFFFFFF, AlignHCenter | WordBreak, t ) );
	r.setWidth( r.width() + 4 );
    } else {
	r = QRect( 0, 0, view->d->fm->width( t ), view->d->fm->height() );
	r.setWidth( r.width() + 4 );
    }

    if ( r.width() > iconView()->maxItemWidth() -
	 ( iconView()->itemTextPos() == QIconView::Bottom ? 0 :
	   pixmapRect().width() ) )
	r.setWidth( iconView()->maxItemWidth() - ( iconView()->itemTextPos() == QIconView::Bottom ? 0 :
						   pixmapRect().width() ) );

    tw = r.width();
    th = r.height();
    if ( tw < view->d->fm->width( "X" ) )
	tw = view->d->fm->width( "X" );

    itemTextRect.setWidth( tw );
    itemTextRect.setHeight( th );

    int w = 0;
    int h = 0;
    if ( view->itemTextPos() == QIconView::Bottom ) {
	w = QMAX( itemTextRect.width(), itemIconRect.width() );
	h = itemTextRect.height() + itemIconRect.height() + 1;

	itemRect.setWidth( w );
	itemRect.setHeight( h );

	itemTextRect = QRect( ( width() - itemTextRect.width() ) / 2, height() - itemTextRect.height(),
			      itemTextRect.width(), itemTextRect.height() );
	itemIconRect = QRect( ( width() - itemIconRect.width() ) / 2, 0,
			      itemIconRect.width(), itemIconRect.height() );
    } else {
	h = QMAX( itemTextRect.height(), itemIconRect.height() );
	w = itemTextRect.width() + itemIconRect.width() + 1;

	itemRect.setWidth( w );
	itemRect.setHeight( h );

	itemTextRect = QRect( width() - itemTextRect.width(), ( height() - itemTextRect.height() ) / 2,
			      itemTextRect.width(), itemTextRect.height() );
	itemIconRect = QRect( 0, ( height() - itemIconRect.height() ) / 2,
			      itemIconRect.width(), itemIconRect.height() );
    }
    if ( view )
	view->updateItemContainer( this );
}

/*!
  Paints the item using the painter \a p and the color group \a cg. If
  you want the item to be drawn with a different font or color,
  reimplement this method and change the values of the color group or
  the painter's font, and call then the QIconViewItem::paintItem()
  with the changed values.
*/

void QIconViewItem::paintItem( QPainter *p, const QColorGroup &cg )
{
    if ( !view )
	return;

    p->save();

    if ( isSelected() ) {
	p->setPen( cg.highlightedText() );
    } else {
	p->setPen( cg.text() );
    }

    calcTmpText();

    if ( view->itemTextPos() == QIconView::Bottom ) {
	int w = ( pixmap() ? pixmap() : unknown_icon )->width();

	if ( isSelected() ) {
	    QPixmap *pix = pixmap() ? pixmap() : unknown_icon;
	    if ( pix && !pix->isNull() ) {
		QPixmap *buffer = get_qiv_buffer_pixmap( pix->size() );
		QBitmap mask = view->mask( pix );

		QPainter p2( buffer );
		p2.fillRect( pix->rect(), white );
		p2.drawPixmap( 0, 0, *pix );
		p2.end();
		buffer->setMask( mask );
		p2.begin( buffer );
#if defined(_WS_X11_)
 		p2.fillRect( pix->rect(), QBrush( cg.highlight(), QBrush::Dense4Pattern) );
#else // in WIN32 Dense4Pattern doesn't work correctly (transparence problem), so work around it
		if ( !qiv_selection )
		    createSelectionPixmap( cg );
		p2.drawTiledPixmap( 0, 0, pix->width(), pix->height(), *qiv_selection );
#endif
		p2.end();
		QRect cr = pix->rect();
		p->drawPixmap( x() + ( width() - w ) / 2, y(), *buffer, 0, 0, cr.width(), cr.height() );

	    }
	} else {
	    p->drawPixmap( x() + ( width() - w ) / 2, y(), *( pixmap() ? pixmap() : unknown_icon ) );
	}

	p->save();
	if ( isSelected() ) {
	    p->fillRect( textRect( FALSE ), cg.highlight() );
	    p->setPen( QPen( cg.highlightedText() ) );
	} else if ( view->d->itemTextBrush != NoBrush )
	    p->fillRect( textRect( FALSE ), view->d->itemTextBrush );

	int align = AlignHCenter;
	if ( view->d->wordWrapIconText )
	    align |= WordBreak;
	p->drawText( textRect( FALSE ), align, view->d->wordWrapIconText ? itemText : tmpText );

	p->restore();
    } else {
	int h = ( pixmap() ? pixmap() : unknown_icon )->height();

	if ( isSelected() ) {
	    QPixmap *pix = pixmap() ? pixmap() : unknown_icon;
	    if ( pix && !pix->isNull() ) {
		QPixmap *buffer = get_qiv_buffer_pixmap( pix->size() );
		QBitmap mask = view->mask( pix );

		QPainter p2( buffer );
		p2.fillRect( pix->rect(), white );
		p2.drawPixmap( 0, 0, *pix );
		p2.end();
		buffer->setMask( mask );
		p2.begin( buffer );
#if defined(_WS_X11_)
		p2.fillRect( pix->rect(), QBrush( cg.highlight(), QBrush::Dense4Pattern) );
#else // in WIN32 Dense4Pattern doesn't work correctly (transparence problem), so work around it
		if ( !qiv_selection )
		    createSelectionPixmap( cg );
		p2.drawTiledPixmap( 0, 0, pix->width(), pix->height(), *qiv_selection );
#endif
		p2.end();
		QRect cr = pix->rect();
		p->drawPixmap( x() , y() + ( height() - h ) / 2, *buffer, 0, 0, cr.width(), cr.height() );
	    }
	} else {
	    p->drawPixmap( x() , y() + ( height() - h ) / 2, *( pixmap() ? pixmap() : unknown_icon ) );
	}

	p->save();
	if ( isSelected() ) {
	    p->fillRect( textRect( FALSE ), cg.highlight() );
	    p->setPen( QPen( cg.highlightedText() ) );
	} else if ( view->d->itemTextBrush != NoBrush )
	    p->fillRect( textRect( FALSE ), view->d->itemTextBrush );

	int align = AlignLeft;
	if ( view->d->wordWrapIconText )
	    align |= WordBreak;
	p->drawText( textRect( FALSE ), align, view->d->wordWrapIconText ? itemText : tmpText );

	p->restore();
    }

    p->restore();
}

/*!
  Paints the focus rect of the item using the painter \a p and the color group \a cg.
*/

void QIconViewItem::paintFocus( QPainter *p, const QColorGroup &cg )
{
    if ( !view )
	return;

    view->style().drawFocusRect( p, QRect( textRect( FALSE ).x(), textRect( FALSE ).y(),
					   textRect( FALSE ).width(), textRect( FALSE ).height() ),
				 cg, isSelected() ? &cg.highlight() : &cg.base(), isSelected() );
    if ( this != view->d->currentItem ) {
	view->style().drawFocusRect( p, QRect( pixmapRect( FALSE ).x(), pixmapRect( FALSE ).y(),
					       pixmapRect( FALSE ).width(),
					       pixmapRect( FALSE ).height() ),
				     cg, &cg.base(), FALSE );
    }
}

/*!
  \fn void QIconViewItem::dropped( QDropEvent *e, const QValueList<QIconDragItem> &lst )

  This method is called when something was dropped on the item.  \a e
  contains all the information about the drop.  If the drag object of the drop
  was a QIconDrag, \a lst contains the list of the dropped items.  You can get
  the data using QIconDragItem::data() of each item then.

  So if \a lst is not empty, use this data for further operations,
  else the drag was not a QIconDrag, so you have to decode \a e
  yourself and work with that.

  The default implementation does nothing, subclasses should reimplement this
  method.
*/

#ifndef QT_NO_DRAGANDDROP
void QIconViewItem::dropped( QDropEvent *, const QValueList<QIconDragItem> & )
{
}
#endif

/*!
  This method is called, when a drag entered the item's bounding rect.

  The default implementation does nothing, subclasses should reimplement
  this method.
*/

void QIconViewItem::dragEntered()
{
}

/*!
  This method is called, when a drag left the item's bounding rect.

  The default implementation does nothing, subclasses should reimplement
  this method.
*/

void QIconViewItem::dragLeft()
{
}

/*!
  Sets the bounding rectangle of the whole item.  This should be only used in
  subclasses, which reimplement calcRect() to be able to set the calculated
  rectangle.
*/

void QIconViewItem::setItemRect( const QRect &r )
{
    itemRect = r;
    checkRect();
    if ( view )
	view->updateItemContainer( this );
}

/*!
  Sets the bounding rectangle of the item text.  This should be only used in
  subclasses, which reimplement calcRect() to be able to set the calculated
  rectangle.
*/

void QIconViewItem::setTextRect( const QRect &r )
{
    itemTextRect = r;
    if ( view )
	view->updateItemContainer( this );
}

/*!
  Sets the bounding rectangle of the item icon.  This should be only used in
  subclasses, which reimplement calcRect() to be able to set the calculated
  rectangle.
*/

void QIconViewItem::setPixmapRect( const QRect &r )
{
    itemIconRect = r;
    if ( view )
	view->updateItemContainer( this );
}

/*!
  \internal
*/

void QIconViewItem::calcTmpText()
{
    if ( !view || view->d->wordWrapIconText || !wordWrapDirty )
	return;
    wordWrapDirty = FALSE;

    int w = iconView()->maxItemWidth() - ( iconView()->itemTextPos() == QIconView::Bottom ? 0 :
					   pixmapRect().width() ) - 4;
    if ( view->d->fm->width( itemText ) < w ) {
	tmpText = itemText;
	return;
    }

    tmpText = "...";
    int i = 0;
    while ( view->d->fm->width( tmpText + itemText[ i ] ) < w )
	tmpText += itemText[ i++ ];
    tmpText.remove( 0, 3 );
    tmpText += "...";
}

void QIconViewItem::checkRect()
{
    int x = itemRect.x();
    int y = itemRect.y();
    int w = itemRect.width();
    int h = itemRect.height();

    bool changed = FALSE;
    if ( x < 0 ) {
	x = 0;
	changed = TRUE;
    }
    if ( y < 0 ) {
	y = 0;
	changed = TRUE;
    }

    if ( changed )
	itemRect.setRect( x, y, w, h );
}

/*****************************************************************************
 *
 * Class QIconView
 *
 *****************************************************************************/

/*!
  \class QIconView qiconview.h
  \brief The QIconView class provides an area with movable labelled icons.
  \module iconview

  \ingroup advanced

  It can display and control a grid or other 2-d layout of items, and
  provides the ability to add or remove new items at any time, lets
  the user select one or may items, rearrange the items, provides drag
  and drop of items, and so on.

  Each item (a QIconViewItem) contains a text and a pixmap (the icon itself).

  The simplest usage of QIconView is to create the object, create some
  QIconViewItems with the view as parent, set the view's geometry, and
  show it.

  When an item is inserted, QIconView allocates a spot for it. The
  default Arrangement is \c LeftToRight - QIconView fills up the
  leftmost column first, then goes rightwards. You can change that
  using setArrangement(), or insert items in a specified position by
  calling the appropriate constructors or QIconViewItem::insertItem(),
  or sort while the view is on-screen using setSorting() and/or
  sort().

  Each (\link QIconViewItem::isSelectable() selectable\endlink) item
  can be selected, and the view provides various SelectionMode
  settings. The default is \c Single - when one item is selected, the
  previously selected item is unselected.



  The QIconView provides a widget which can contain lots of iconview
  items which can be selected, dragged and so on.

  Items can be inserted in a grid and can flow from top to bottom
  (TopToBottom) or from left to right (LeftToRight). The text can be
  either displayed at the bottom of the icons or the the right of the
  icons. Items can also be inserted in a sorted order. There are also
  methods to re-arrange and re-sort the items after they have been
  inserted.

  There is a variety of selection modes, described in the
  QIconView::SelectionMode documentation. The default is
  single-selection, and you can change it using setSelectionMode().

  Since QIconView offers multiple selection it has to display keyboard
  focus and selection state separately. Therefore there are functions
  both to set the selection state of an item, setSelected(), and to
  select which item displays keyboard focus, setCurrentItem().

  When multiple items may be selected, the iconview provides a
  rubberband too.

  Items can also be in-place renamed.

  The normal way to insert some items is to create QIconViewItems and
  pass the iconview as parent. By using insertItem(), items can be
  inserted manually too. The QIconView offers basic methods similar to
  the QListView and QListBox, like QIconView::takeItem(),
  QIconView::clearSelection(), QIconView::setSelected(),
  QIconView::setCurrentItem(), QIconView::currentItem() and much more.

  As the internal structure to store the iconview items is linear (a
  double linked list), no iterator class is needed to iterate over all
  items. This can be easily done with a code like

  \code
  QIconView *iv = the iconview
  for ( QIconViewItem *i = iv->firstItem(); i; i = i->nextItem() ) {
      i->doSmething();
  }
  \endcode

  To notify the application about changes in the iconview there are
  several signals which are emitted by the QIconView.

  The QIconView is designed for Drag'n'Drop, as the icons are also
  moved inside the iconview itself using DnD. So the QIconView
  provides some methods for extended DnD too. To use DnD correctly in
  the iconview, please read following instructions:

  There are two different ways to do that, depending what you want.
  The first case is the simple one, in which case just the dragobject
  you created is dragged around. If you want, that drag shapes (the
  rectangles of the dragged items with exact positions) are drawn, you
  have to choose the more complicated way. Here first the simple case
  is described:

  In the simple case you only need for starting a drag to reimplement
  QIconView::dragObject(). There you create a QDragObject with the
  data you want to drag and return it. And for entering drags you
  don't need to do anything special then. Just connect to dropped()
  signal to get notified about drops onto the viewport and reimplement
  QIconViewItem::acceptDrop() and QIconViewItem::dropped() to be able
  to react on drops onto an iconview item.

  If you want to have drag shapes drawn, you have to do quite a bit
  more and complex things:

  The first part is starting drags: If you want to use extended DnD in
  the QIconView, you should use QIconDrag (or a derived class from
  that) as dragobject and in dragObject() create such an object and
  return it. Before returning it, fill it there with QIconDragItems.
  Normally such a drag should offer data of each selected item. So in
  dragObject() you should iterate over all items, create for each
  selected item a QIconDragItem and append this with
  QIconDrag::append() to the QIconDrag object. With
  QIconDragItem::setData() you can set the data of each item which
  should be dragged. If you want to offer the data in additional
  mime-types, it's the best to use a class derived from QIconDrag
  which implements additional encoding and decoding functions.

  Now, when a drag enters the iconview, there is not much todo. Just
  connect to the dropped() signal and reimplement
  QIconViewItem::dropped() and QIconViewItem::acceptDrop(). The only
  special thing in this case is the second argument in the dropped()
  signal and in QIconViewItem::dropped(). Fur further details about
  that look at the documentation of these signal/method.

  For an example implementation of the complex Drag'n'Drop stuff look at the
  qfileiconview example (qt/examples/qfileiconview)

  Finally, see also QIconViewItem::setDragEnabled(),
  QIconViewItem::setDropEnabled(), QIconViewItem::acceptDrop() and
  QIconViewItem::dropped()

  <img src=qiconview-m.png> <img src=qiconview-w.png>
*/

/*! \enum QIconView::ResizeMode

  This enum type decides how QIconView should treat the positions of
  its icons when the widget is resized.  The currently defined modes
  are: <ul>

  <li> \c Fixed - the icons' positions are not changed.
  <li> \c Adjust - the icons' positions are adjusted to be within the
  new geometry, if possible.

  </ul>
*/


/*! \enum QIconView::SelectionMode

  This enumerated type is used by QIconView to indicate how it reacts
  to selection by the user. It has four values: <ul>

  <li> \c Single - When the user selects an item, any already-selected
  item becomes unselected, and the user cannot unselect the selected
  item. This means that the user can never clear the selection, even
  though the selection may be cleared by the application programmer
  using QIconView::clearSelection().

  <li> \c Multi - When the user selects an item in the most ordinary
  way, the selection status of that item is toggled and the other
  items are left alone.

  <li> \c Extended - When the user selects an item in the most
  ordinary way, the selection is cleared and the new item selected.
  However, if the user presses the CTRL key when clicking on an item,
  the clicked item gets toggled and all other items are left untouched. And
  if the user presses the SHIFT key while clicking on an item, all items
  between the current item and the clicked item get selected or unselected
  depending on the state of the clicked item.
  Also multiple items can be selected by dragging the mouse while the
  left mouse button stayes pressed.

  <li> \c NoSelection - Items cannot be selected.

  </ul>

  In other words, \c Single is a real single-selection iconview, \c
  Multi a real multi-selection iconview, and \c Extended iconview
  where users can select multiple items but usually want to select
  either just one or a range of contiguous items, and \c NoSelection
  is for a iconview where the user can look but not touch.
*/

/*! \enum QIconView::Arrangement

   This enum type decides in which direction the items, which do not
   fit onto the screen anymore flow.

   <ul>
   <li> \c LeftToRight - Items, which don't fit onto the view, go further down (you get a vertical scrollbar)
   <li> \c TopToBottom - Items, which don't fit onto the view, go further right (you get a horizontal scrollbar)
   </ul>
*/

/*! \enum QIconView::ItemTextPos

   This enum type specifies the position of the item text in relation to the icon.

   <ul>
   <li> \c Bottom - The text is drawn at the bottom of the icon)
   <li> \c Right - The text is drawn at the right of the icon)
   </ul>
*/

/*! \fn void  QIconView::dropped ( QDropEvent * e, const QValueList<QIconDragItem> &lst )

  This signal is emitted, when a drop event occurred onto the viewport
  (not onto an icon), which the iconview itself can't handle.

  \a e gives you all information about the drop. If the drag object of
  the drop was a QIconDrag, \a lst contains the list of the dropped
  items. You can get the data using QIconDragItem::data() of each item
  then.

  So, if \a lst is not empty, use this data for further operations,
  else the drag was not a QIconDrag, so you have to decode \a e
  yourself and work with that.
*/

/*! \fn void  QIconView::moved ()
  This signal is emitted after successfully dropping an (or some) item(s) of the iconview
  somewhere and if they should be removed now.
*/

/*! \fn void  QIconView::doubleClicked (QIconViewItem * item)
  This signal is emitted, if the user double-clicked on the item \a item.
*/

/*! \fn void  QIconView::returnPressed (QIconViewItem * item)
  This signal is emitted, if the user pressed the return or enter button.
  \a item is the item which was current while return or enter was pressed.
*/

/*! \fn void  QIconView::selectionChanged ()
  This signal is emitted when the selection has been changed. It's emitted
  in each selection mode.
*/

/*! \fn void  QIconView::selectionChanged( QIconViewItem *item )
  This signal is emitted when the selection has been changed. \a item
  is the new selected item. This signal is only emitted in single
  selection mode.
*/

/*! \fn void QIconView::currentChanged ( QIconViewItem *item )
  This signal is emitted, when the different items got current.
  \a item is the new current item or 0, if no item is current now.
*/

/*! \fn void  QIconView::onItem( QIconViewItem *i )
  This signal is emitted, when the user moves the mouse cursor onto an item.
  It�s only emitted once per item.
*/

/*! \fn void  QIconView::onViewport()
  This signal is emitted, when the user moves the mouse cursor, which was
  on an item away from the item onto the viewport.
*/

/*!
  \fn void QIconView::itemRenamed (QIconViewItem * item)
  If the \a item has been renamed (e.g. by in-place renaming),
  this signal is emitted.
*/

/*!
  \fn void QIconView::itemRenamed (QIconViewItem * item, const QString &name)
  If the \a item has been renamed (e.g. by in-place renaming),
  this signal is emitted. \a name is the new text (name) of the item.
*/

/*!
  \fn void QIconView::rightButtonPressed (QIconViewItem * item, const QPoint & pos)
  This signal is emitted when the user pressed with the right mouse button on
  either and item (then \a item is the item under the mouse cursor) or
  somewhere else (then \a item is 0). \a pos the position of the mouse cursor.
*/

/*!
  \fn void QIconView::rightButtonClicked (QIconViewItem * item, const QPoint & pos)
  This signal is emitted when the user clicked (pressed + released) with the right
  mouse button on either and item (then \a item is the item under the mouse cursor) or
  somewhere else (then \a item is 0). \a pos the position of the mouse cursor.
*/

/*!
  \fn void QIconView::mouseButtonPressed (int button, QIconViewItem * item, const QPoint & pos)
  This signal is emitted when the user pressed with any mouse button on
  either and item (then \a item is the item under the mouse cursor) or
  somewhere else (then \a item is 0). \a button is the number of the mouse button which
  the user pressed, and \a pos the position of the mouse cursor.
*/

/*!
  \fn void QIconView::mouseButtonClicked (int button, QIconViewItem * item, const QPoint & pos)
  This signal is emitted when the user clicked (pressed + released) with any mouse button on
  either and item (then \a item is the item under the mouse cursor) or
  somewhere else (then \a item is 0). \a button is the number of the mouse button which
  the user clicked, and \a pos the position of the mouse cursor.
*/

/*!
  \fn void QIconView::clicked ( QIconViewItem * item, const QPoint & pos)
  This signal is emitted when the user clicked (pressed + released) with any mouse button on
  either and item (then \a item is the item under the mouse cursor) or
  somewhere else (then \a item is 0). \a pos the position of the mouse cursor.
*/

/*!
  \fn void QIconView::pressed ( QIconViewItem * item, const QPoint & pos)
  This signal is emitted when the user pressed with any mouse button on
  either and item (then \a item is the item under the mouse cursor) or
  somewhere else (then \a item is 0). \a pos the position of the mouse cursor.
*/

/*!
  \fn void QIconView::clicked ( QIconViewItem * item )
  This signal is emitted when the user clicked (pressed + released) with any mouse button on
  either and item (then \a item is the item under the mouse cursor) or
  somewhere else (then \a item is 0).
*/

/*!
  \fn void QIconView::pressed ( QIconViewItem * item )
  This signal is emitted when the user pressed with any mouse button on
  either and item (then \a item is the item under the mouse cursor) or
  somewhere else (then \a item is 0).
*/

/*!
  Constructs an empty icon view
*/

QIconView::QIconView( QWidget *parent, const char *name, WFlags f )
    : QScrollView( parent, name, WNorthWestGravity | WRepaintNoErase  | f )
{
    if ( !unknown_icon ) {
	unknown_icon = new QPixmap( (const char **)unknown_xpm );
	qAddPostRoutine( qt_cleanup_qiv1 );
    }

    d = new QIconViewPrivate;
    d->dragging = FALSE;
    d->firstItem = 0;
    d->lastItem = 0;
    d->count = 0;
    d->mousePressed = FALSE;
    d->selectionMode = Single;
    d->currentItem = 0;
    d->highlightedItem = 0;
    d->rubber = 0;
    d->scrollTimer = 0;
    d->startDragItem = 0;
    d->tmpCurrentItem = 0;
    d->rastX = d->rastY = -1;
    d->spacing = 5;
    d->cleared = FALSE;
    d->arrangement = LeftToRight;
    d->resizeMode = Fixed;
    d->dropped = FALSE;
    d->adjustTimer = new QTimer( this );
    d->isIconDrag = FALSE;
#ifndef QT_NO_DRAGANDDROP
    d->iconDragData.clear();
#endif
    d->numDragItems = 0;
    d->updateTimer = new QTimer( this );
    d->cachedW = d->cachedH = 0;
    d->maxItemWidth = 200;
    d->maxItemTextLength = 255;
    d->inputTimer = new QTimer( this );
    d->currInputString = QString::null;
    d->dirty = FALSE;
    d->rearrangeEnabled = TRUE;
    d->itemTextPos = Bottom;
    d->reorderItemsWhenInsert = TRUE;
#ifndef QT_NO_CURSOR
    d->oldCursor = arrowCursor;
#endif
    d->resortItemsWhenInsert = FALSE;
    d->sortDirection = TRUE;
    d->wordWrapIconText = TRUE;
    d->cachedContentsX = d->cachedContentsY = -1;
    d->clearing = FALSE;
    d->fullRedrawTimer = new QTimer( this );
    d->itemTextBrush = NoBrush;
    d->drawAllBack = TRUE;
    d->fm = new QFontMetrics( font() );
    d->minLeftBearing = d->fm->minLeftBearing();
    d->minRightBearing = d->fm->minRightBearing();
    d->firstContainer = d->lastContainer = 0;
    d->containerUpdateLocked = FALSE;
    d->firstSizeHint = TRUE;
    d->selectAnchor = 0;

    connect( d->adjustTimer, SIGNAL( timeout() ),
	     this, SLOT( adjustItems() ) );
    connect( d->updateTimer, SIGNAL( timeout() ),
	     this, SLOT( slotUpdate() ) );
    connect( d->inputTimer, SIGNAL( timeout() ),
	     this, SLOT( clearInputString() ) );
    connect( d->fullRedrawTimer, SIGNAL( timeout() ),
	     this, SLOT( updateContents() ) );
    connect( this, SIGNAL( contentsMoving( int, int ) ),
	     this, SLOT( movedContents( int, int ) ) );

    setAcceptDrops( TRUE );
    viewport()->setAcceptDrops( TRUE );

    setMouseTracking( TRUE );
    viewport()->setMouseTracking( TRUE );

    viewport()->setBackgroundMode( PaletteBase );
    viewport()->setFocusProxy( this );
    viewport()->setFocusPolicy( QWidget::WheelFocus );

    d->toolTip = new QIconViewToolTip( viewport(), this );
    d->showTips = TRUE;
}

/*!
  \reimp
*/

void QIconView::styleChange( QStyle& old )
{
    QScrollView::styleChange( old );
    *d->fm = QFontMetrics( font() );
    d->minLeftBearing = d->fm->minLeftBearing();
    d->minRightBearing = d->fm->minRightBearing();

    QIconViewItem *item = d->firstItem;
    for ( ; item; item = item->next ) {
	item->wordWrapDirty = TRUE;
	item->calcRect();
    }

    delete qiv_selection;
    qiv_selection = 0;
}

/*!
  \reimp
*/

void QIconView::setFont( const QFont & f )
{
    QScrollView::setFont( f );
    *d->fm = QFontMetrics( font() );
    d->minLeftBearing = d->fm->minLeftBearing();
    d->minRightBearing = d->fm->minRightBearing();

    QIconViewItem *item = d->firstItem;
    for ( ; item; item = item->next ) {
	item->wordWrapDirty = TRUE;
	item->calcRect();
    }
}

/*!
  \reimp
*/

void QIconView::setPalette( const QPalette & p )
{
    QScrollView::setPalette( p );
    *d->fm = QFontMetrics( font() );
    d->minLeftBearing = d->fm->minLeftBearing();
    d->minRightBearing = d->fm->minRightBearing();

    QIconViewItem *item = d->firstItem;
    for ( ; item; item = item->next ) {
	item->wordWrapDirty = TRUE;
	item->calcRect();
    }
}

/*!
  Destructs the iconview and deletes all items.
*/

QIconView::~QIconView()
{
    QIconViewItem *tmp, *item = d->firstItem;
    d->clearing = TRUE;
    QIconViewPrivate::ItemContainer *c = d->firstContainer, *tmpc;
    while ( c ) {
	tmpc = c->n;
	delete c;
	c = tmpc;
    }
    while ( item ) {
	tmp = item->next;
	delete item;
	item = tmp;
    }
    delete d->fm;
    d->fm = 0;
    delete d->toolTip;
    d->toolTip = 0;
    delete d;
}

/*!
  Inserts the iconview item \a item after \a after. If \a after is 0,
  \a item is appended.

  You should never need to call this method yourself, you should rather do

  \code
    (void) new QIconViewItem( iconview, "This is the text of the item", pixmap );
  \endcode

  This does everything required for inserting an item.
*/

void QIconView::insertItem( QIconViewItem *item, QIconViewItem *after )
{
    if ( !item )
	return;

    if ( !d->firstItem ) {
	d->firstItem = d->lastItem = item;
	item->prev = 0;
	item->next = 0;
    } else {
	if ( !after || after == d->lastItem ) {
	    d->lastItem->next = item;
	    item->prev = d->lastItem;
	    item->next = 0;
	    d->lastItem = item;
	} else {
	    QIconViewItem *i = d->firstItem;
	    while ( i != after )
		i = i->next;

	    if ( i ) {
		QIconViewItem *next = i->next;
		item->next = next;
		item->prev = i;
		i->next = item;
		next->prev = item;
	    }
	}
    }

    if ( isVisible() ) {
	if ( d->reorderItemsWhenInsert ) {
	    if ( d->updateTimer->isActive() )
		d->updateTimer->stop();
	    d->fullRedrawTimer->stop();
	    // #### uncomment this ASA insertInGrid uses cached values and is efficient
	    //insertInGrid( item );

	    d->cachedW = QMAX( d->cachedW, item->x() + item->width() );
	    d->cachedH= QMAX( d->cachedH, item->y() + item->height() );

	    d->updateTimer->start( 100, TRUE );
	} else {
	    insertInGrid( item );
	}
    }

    d->count++;
    d->dirty = TRUE;
}

/*!
  Because of efficiency, the iconview is not redrawn immediately after
  inserting a new item, but with a very small delay using a QTimer. The
  result of this is, that if lots of items are inserted in a short time
  (e.g. in a loop), the iconview is not redrawn after each inserted item,
  but after inserting all of them, which makes the operation much faster
  and flicker-free.
*/

void QIconView::slotUpdate()
{
    d->updateTimer->stop();
    d->fullRedrawTimer->stop();

    if ( !d->firstItem || !d->lastItem )
	return;

    // #### remove that ASA insertInGrid uses cached values and is efficient
    if ( d->resortItemsWhenInsert )
	sort( d->sortDirection );
    else {
	int y = d->spacing;
	QIconViewItem *item = d->firstItem;
	int w = 0, h = 0;
	while ( item ) {
	    item = makeRowLayout( item, y );

	    if ( !item || !item->next )
		break;

	    w = QMAX( w, item->x() + item->width() );
	    h = QMAX( h, item->y() + item->height() );
	    if ( d->arrangement == LeftToRight )
		h = QMAX( h, y );

	    item = item->next;
	}

	if ( d->lastItem && d->arrangement == TopToBottom ) {
	    item = d->lastItem;
	    int x = item->x();
	    while ( item && item->x() >= x ) {
		w = QMAX( w, item->x() + item->width() );
		h = QMAX( h, item->y() + item->height() );
		item = item->prev;
	    }
	}

	w = QMAX( QMAX( d->cachedW, w ), d->lastItem->x() + d->lastItem->width() );
	h = QMAX( QMAX( d->cachedH, h ), d->lastItem->y() + d->lastItem->height() );

	if ( d->arrangement == TopToBottom )
	    w += d->spacing;
	else
	    h += d->spacing;
	viewport()->setUpdatesEnabled( FALSE );
	resizeContents( w, h );
	viewport()->setUpdatesEnabled( TRUE );
	viewport()->repaint( FALSE );
    }

    int cx = d->cachedContentsX == -1 ? contentsX() : d->cachedContentsX;
    int cy = d->cachedContentsY == -1 ? contentsY() : d->cachedContentsY;

    if ( cx != contentsX() || cy != contentsY() )
	setContentsPos( cx, cy );

    d->cachedContentsX = d->cachedContentsY = -1;
    d->cachedW = d->cachedH = 0;
}

/*!
  Takes the iconview item \a item out of the iconview and causes an update
  of the screen display.  The item is not deleted.  You should normally not
  need to call this function, as QIconViewItem::~QIconViewItem() calls it.
  The normal way to delete an item is \c delete.
*/

void QIconView::takeItem( QIconViewItem *item )
{
    if ( !item )
	return;

    if ( item->d->container1 )
	item->d->container1->items.removeRef( item );
    if ( item->d->container2 )
	item->d->container2->items.removeRef( item );
    item->d->container2 = 0;
    item->d->container1 = 0;

    bool block = signalsBlocked();
    blockSignals( d->clearing );

    QRect r = item->rect();

    if ( d->currentItem == item ) {
	if ( item->prev ) {
	    d->currentItem = item->prev;
	    emit currentChanged( d->currentItem );
	    repaintItem( d->currentItem );
	} else if ( item->next ) {
	    d->currentItem = item->next;
	    emit currentChanged( d->currentItem );
	    repaintItem( d->currentItem );
	} else {
	    d->currentItem = 0;
	    emit currentChanged( d->currentItem );
	}
    }
    if ( item->isSelected() ) {
	item->selected = FALSE;
	emit selectionChanged();
    }

    if ( item == d->firstItem ) {
	d->firstItem = d->firstItem->next;
	if ( d->firstItem )
	    d->firstItem->prev = 0;
    } else if ( item == d->lastItem ) {
	d->lastItem = d->lastItem->prev;
	if ( d->lastItem )
	    d->lastItem->next = 0;
    } else {
	QIconViewItem *i = item;
	if ( i ) {
	    if ( i->prev )
		i->prev->next = i->next;
	    if ( i->next )
		i->next->prev = i->prev;
	}
    }

    if ( d->selectAnchor == item )
	d->selectAnchor = d->currentItem;

    if ( !d->clearing )
	repaintContents( r.x(), r.y(), r.width(), r.height(), TRUE );

    d->count--;

    blockSignals( block );
}

/*!
  Returns the index of \a item or -1 if \a item doesn't exist
  in this icon view.
*/

int QIconView::index( const QIconViewItem *item ) const
{
    if ( !item )
	return -1;

    if ( item == d->firstItem )
	return 0;
    else if ( item == d->lastItem )
	return d->count - 1;
    else {
	QIconViewItem *i = d->firstItem;
	int j = 0;
	while ( i && i != item ) {
	    i = i->next;
	    ++j;
	}

	return i ? j : -1;
    }
}

/*!
  Returns a pointer to the first item fo the iconview, or 0, if there
  are no items in the iconview.
*/

QIconViewItem *QIconView::firstItem() const
{
    return d->firstItem;
}

/*!
  Returns a pointer to the last item fo the iconview, or 0, if there
  are no items in the iconview.
*/

QIconViewItem *QIconView::lastItem() const
{
    return d->lastItem;
}

/*!
  Returns a pointer to the current item fo the iconview, or 0, if no
  item is current.
*/

QIconViewItem *QIconView::currentItem() const
{
    return d->currentItem;
}

/*!
  Makes \a item the new current item of the iconview.
*/

void QIconView::setCurrentItem( QIconViewItem *item )
{
    if ( !item || item == d->currentItem )
	return;
    QIconViewItem *old = d->currentItem;
    d->currentItem = item;
    emit currentChanged( d->currentItem );
    if ( d->selectionMode == Single ) {
	bool changed = FALSE;
	if ( old && old->selected ) {
	    old->selected = FALSE;
	    changed = TRUE;
	}
	if ( item && !item->selected && item->isSelectable() && d->selectionMode != NoSelection ) {
	    item->selected = TRUE;
	    changed = TRUE;
	    emit selectionChanged( item );
	}
	if ( changed )
	    emit selectionChanged();
    }

    if ( old )
	repaintItem( old );
    repaintItem( d->currentItem );
}

/*!
  Selects or unselects \a item depending on \a s, and may also
  unselect other items, depending on QIconView::selectionMode() and \a
  cb.

  If \a s is FALSE, \a item is unselected.

  If \a s is TRUE and QIconView::selectionMode() is \c Single, \a item
  is selected, and the item which was selected is unselected.

  If \a s is TRUE and QIconView::selectionMode() is \c Extended, \a
  item is selected. If \a cb is TRUE, the other items of the iconview
  are not touched. If \a cb is FALSE (the default) all other items are
  unselected.

  If \a s is TRUE and QIconView::selectionMode() is \c Multi \a item
  is selected.

  Note that \a cb is used only if QIconView::selectionMode() is \c
  Extended. \a cb defaults to FALSE.

  All items whose selection status change repaint themselves.
*/

void QIconView::setSelected( QIconViewItem *item, bool s, bool cb )
{
    if ( !item )
	return;
    item->setSelected( s, cb );
}

/*!
  Returns the number of items in the iconview.
*/

uint QIconView::count() const
{
    return d->count;
}

/*!
  Does autoscrolling when selecting multiple icons with the rubber band.
*/

void QIconView::doAutoScroll()
{
    if ( !d->rubber )
	return;
    QRect oldRubber = QRect( *d->rubber );

    QPoint pos = QCursor::pos();
    pos = viewport()->mapFromGlobal( pos );
    pos = viewportToContents( pos );

    d->rubber->setRight( pos.x() );
    d->rubber->setBottom( pos.y() );

    int minx = contentsWidth(), miny = contentsHeight();
    int maxx = 0, maxy = 0;
    bool changed = FALSE;
    bool block = signalsBlocked();

    QRect rr;
    QRegion region( 0, 0, visibleWidth(), visibleHeight() );

    blockSignals( TRUE );
    viewport()->setUpdatesEnabled( FALSE );
    bool alreadyIntersected = FALSE;
    QRect nr = d->rubber->normalize();
    QRect rubberUnion = nr.unite( oldRubber.normalize() );
    QIconViewPrivate::ItemContainer *c = d->firstContainer;
    for ( ; c; c = c->n ) {
	if ( c->rect.intersects( rubberUnion ) ) {
	    alreadyIntersected = TRUE;
	    QIconViewItem *item = c->items.first();
	    for ( ; item; item = c->items.next() ) {
		if ( d->selectedItems.find( item ) )
		    continue;
		if ( !item->intersects( nr ) ) {
		    if ( item->isSelected() ) {
			item->setSelected( FALSE );
			changed = TRUE;
			rr = rr.unite( item->rect() );
		    }
		} else if ( item->intersects( nr ) ) {
		    if ( !item->isSelected() ) {
			item->setSelected( TRUE, TRUE );
			changed = TRUE;
			rr = rr.unite( item->rect() );
		    } else {
			region = region.subtract( QRect( contentsToViewport( item->pos() ),
							 item->size() ) );
		    }

		    minx = QMIN( minx, item->x() - 1 );
		    miny = QMIN( miny, item->y() - 1 );
		    maxx = QMAX( maxx, item->x() + item->width() + 1 );
		    maxy = QMAX( maxy, item->y() + item->height() + 1 );
		}
	    }
	} else {
 	    if ( alreadyIntersected )
 		break;
	}
    }
    viewport()->setUpdatesEnabled( TRUE );
    blockSignals( block );

    QRect r = *d->rubber;
    *d->rubber = oldRubber;

    QPainter p;
    p.begin( viewport() );
    p.setRasterOp( NotROP );
    p.setPen( QPen( color0, 1 ) );
    p.setBrush( NoBrush );
    drawRubber( &p );
    d->dragging = FALSE;
    p.end();

    *d->rubber = r;

    if ( changed ) {
	d->drawAllBack = FALSE;
	d->clipRegion = region;
	repaintContents( rr, FALSE );
	d->drawAllBack = TRUE;
    }

    ensureVisible( pos.x(), pos.y() );

    p.begin( viewport() );
    p.setRasterOp( NotROP );
    p.setPen( QPen( color0, 1 ) );
    p.setBrush( NoBrush );
    drawRubber( &p );
    d->dragging = TRUE;

    p.end();

    if ( changed ) {
	emit selectionChanged();
	if ( d->selectionMode == Single )
	    emit selectionChanged( d->currentItem );
    }

    if ( !QRect( 0, 0, viewport()->width(), viewport()->height() ).contains( pos ) &&
	 !d->scrollTimer ) {
	d->scrollTimer = new QTimer( this );

	connect( d->scrollTimer, SIGNAL( timeout() ),
		 this, SLOT( doAutoScroll() ) );
	d->scrollTimer->start( 100, FALSE );
    } else if ( QRect( 0, 0, viewport()->width(), viewport()->height() ).contains( pos ) &&
		d->scrollTimer ) {
	disconnect( d->scrollTimer, SIGNAL( timeout() ),
		    this, SLOT( doAutoScroll() ) );
	d->scrollTimer->stop();
	delete d->scrollTimer;
	d->scrollTimer = 0;
    }
}

/*!
  \reimp
*/

void QIconView::drawContents( QPainter *p, int cx, int cy, int cw, int ch )
{
    if ( d->dragging && d->rubber )
	drawRubber( p );

    QRect r = QRect( cx, cy, cw, ch );

    QIconViewPrivate::ItemContainer *c = d->firstContainer;
    QRegion remaining( QRect( cx, cy, cw, ch ) );
    bool alreadyIntersected = FALSE;
    while ( c ) {
	if ( c->rect.intersects( r ) ) {
	    p->save();
	    p->resetXForm();
	    QRect r2 = c->rect;
	    r2 = r2.intersect( r );
	    QRect r3( contentsToViewport( QPoint( r2.x(), r2.y() ) ), QSize( r2.width(), r2.height() ) );
	    if ( d->drawAllBack ) {
		p->setClipRect( r3 );
	    } else {
		QRegion reg = d->clipRegion.intersect( r3 );
		p->setClipRegion( reg );
	    }
	    drawBackground( p, r3 );
	    remaining = remaining.subtract( r3 );
	    p->restore();

	    QIconViewItem *item = c->items.first();
	    for ( ; item; item = c->items.next() ) {
		if ( item->rect().intersects( r ) && !item->dirty ) {
		    p->save();
		    p->setFont( font() );
		    item->paintItem( p, colorGroup() );
		    p->restore();
		}
	    }
	    alreadyIntersected = TRUE;
	} else {
	    if ( alreadyIntersected )
		break;
	}
	c = c->n;
    }

    if ( !remaining.isNull() && !remaining.isEmpty() ) {
	p->save();
	p->resetXForm();
	if ( d->drawAllBack ) {
	    p->setClipRegion( remaining );
	} else {
	    remaining = d->clipRegion.intersect( remaining );
	    p->setClipRegion( remaining );
	}
	drawBackground( p, remaining.boundingRect() );
	p->restore();
    }

    if ( ( hasFocus() || viewport()->hasFocus() ) && d->currentItem &&
	 d->currentItem->rect().intersects( r ) ) {
	d->currentItem->paintFocus( p, colorGroup() );
    }

    if ( d->dragging && d->rubber )
	drawRubber( p );
}

/*!
  Arranges all items in the grid. For the grid the specified
  values, given by QIconView::setGridX() and QIconView::setGridY()
  are used.
  Even if QIconView::sorting() is enabled, the items are not resorted
  in this method. If you want to sort and re-arrange all items, use
  iconview->sort( iconview->sortDirection() );

  If \a update is TRUE, the viewport is repainted.

  \sa QIconView::setGridX(), QIconView::setGridY(), QIconView::sort()
*/

void QIconView::arrangeItemsInGrid( bool update )
{
    if ( !d->firstItem || !d->lastItem )
	return;

    d->containerUpdateLocked = TRUE;

    int w = 0, h = 0, y = d->spacing;

    QIconViewItem *item = d->firstItem;
    while ( item ) {
	item = makeRowLayout( item, y );
	w = QMAX( w, item->x() + item->width() );
	h = QMAX( h, item->y() + item->height() );
	if ( d->arrangement == LeftToRight )
	    h = QMAX( h, y );

	if ( !item || !item->next )
	    break;

	item = item->next;
    }

    if ( d->lastItem && d->arrangement == TopToBottom ) {
	item = d->lastItem;
	int x = item->x();
	while ( item && item->x() >= x ) {
	    w = QMAX( w, item->x() + item->width() );
	    h = QMAX( h, item->y() + item->height() );
	    item = item->prev;
	}
    }
    d->containerUpdateLocked = FALSE;

    w = QMAX( QMAX( d->cachedW, w ), d->lastItem->x() + d->lastItem->width() );
    h = QMAX( QMAX( d->cachedH, h ), d->lastItem->y() + d->lastItem->height() );

    if ( d->arrangement == TopToBottom )
	w += d->spacing;
    else
	h += d->spacing;

    viewport()->setUpdatesEnabled( FALSE );
    int vw = visibleWidth();
    int vh = visibleHeight();
    resizeContents( w, h );
    bool doAgain = FALSE;
    if ( d->arrangement == LeftToRight )
	doAgain = visibleWidth() != vw;
    if ( d->arrangement == TopToBottom )
	doAgain = visibleHeight() != vh;
    if ( doAgain ) // in the case that the visibleExtend changed because of the resizeContents (scrollbar show/hide), redo layout again
	arrangeItemsInGrid( FALSE );
    viewport()->setUpdatesEnabled( TRUE );
    d->dirty = FALSE;
    rebuildContainers();
    if ( update )
	repaintContents( contentsX(), contentsY(), viewport()->width(), viewport()->height(), FALSE );
}

/*!
  Arranges all items in the \a grid; If the grid is invalid (see
  QSize::isValid(), an invalid size is created when using the default
  constructor of QSize()) the best fitting grid is calculated first
  and used then.

  if \a update is TRUE, the viewport is repainted.

*/

void QIconView::arrangeItemsInGrid( const QSize &grid, bool update )
{
    d->containerUpdateLocked = TRUE;
    QSize grid_( grid );
    if ( !grid_.isValid() ) {
	int w = 0, h = 0;
	QIconViewItem *item = d->firstItem;
	for ( ; item; item = item->next ) {
	    w = QMAX( w, item->width() );
	    h = QMAX( h, item->height() );
	}

	grid_ = QSize( QMAX( d->rastX + d->spacing, w ),
		       QMAX( d->rastY + d->spacing, h ) );
    }

    int w = 0, h = 0;
    QIconViewItem *item = d->firstItem;
    for ( ; item; item = item->next ) {
	int nx = item->x() / grid_.width();
	int ny = item->y() / grid_.height();
	item->move( nx * grid_.width(),
		    ny * grid_.height() );
	w = QMAX( w, item->x() + item->width() );
	h = QMAX( h, item->y() + item->height() );
	item->dirty = FALSE;
    }
    d->containerUpdateLocked = FALSE;

    resizeContents( w, h );
    rebuildContainers();
    if ( update )
	repaintContents( contentsX(), contentsY(), viewport()->width(), viewport()->height(), FALSE );
}

/*!
  \reimp
*/

void QIconView::setContentsPos( int x, int y )
{
    if ( d->updateTimer->isActive() ) {
	d->cachedContentsX = x;
	d->cachedContentsY = y;
    } else {
	d->cachedContentsY = d->cachedContentsX = -1;
	QScrollView::setContentsPos( x, y );
    }
}

/*!
  \reimp
*/

void QIconView::showEvent( QShowEvent * )
{
    if ( d->dirty ) {
	resizeContents( viewport()->width(), viewport()->height() );
	if ( autoArrange() )
	    arrangeItemsInGrid( FALSE );
    }
    QScrollView::show();
}

/*!
  Sets the selection mode of the iconview to \a m, which may be one of
\c Single (the default), \c Extended, \c Multi or \c NoSelection.

  \sa selectionMode()
*/

void QIconView::setSelectionMode( SelectionMode m )
{
    d->selectionMode = m;
}

/*!
  Returns the selection mode of the iconview.  The initial mode is
  \c Single.

  \sa setSelectionMode()
*/

QIconView::SelectionMode QIconView::selectionMode() const
{
    return d->selectionMode;
}

/*!
  Returns a pointer to the item which contains \a pos, which is given
  on contents coordinates.
*/

QIconViewItem *QIconView::findItem( const QPoint &pos ) const
{
    if ( !d->firstItem )
	return 0;

    QIconViewPrivate::ItemContainer *c = d->lastContainer;
    for ( ; c; c = c->p ) {
	if ( c->rect.contains( pos ) ) {
	    QIconViewItem *item = c->items.last();
	    for ( ; item; item = c->items.prev() )
		if ( item->contains( pos ) )
		    return item;
	}
    }

    return 0;
}

/*!
  Returns a pointer to the first item which could be found that contains
  \a text, or 0 if no such item could be found.
*/

QIconViewItem *QIconView::findItem( const QString &text ) const
{
    if ( !d->firstItem )
	return 0;

    QIconViewItem *item = d->currentItem;
    for ( ; item; item = item->next ) {
	if ( item->text().lower().left( text.length() ) == text.lower() )
	    return item;
    }

    item = d->firstItem;
    for ( ; item && item != d->currentItem; item = item->next ) {
	if ( item->text().lower().left( text.length() ) == text.lower() )
	    return item;
    }

    return 0;
}

/*!
  Unselects all items.
*/

void QIconView::clearSelection()
{
    selectAll( FALSE );
}

/*!
  If \a select is TRUE, all items get selected, else all get unselected.
  This works only in the selection modes Multi and Extended. In
  Single and NoSelection mode the selection of the current item is
  just set to \a select.
*/

void QIconView::selectAll( bool select )
{
    if ( d->selectionMode == NoSelection )
	return;

    if ( d->selectionMode == Single ) {
	if ( d->currentItem )
	    d->currentItem->setSelected( select );
	return;
    }

    bool b = signalsBlocked();
    blockSignals( TRUE );
    QIconViewItem *item = d->firstItem;
    QIconViewItem *i = d->currentItem;
    bool changed = FALSE;
    bool ue = viewport()->isUpdatesEnabled();
    viewport()->setUpdatesEnabled( FALSE );
    QRect rr;
    for ( ; item; item = item->next ) {
	if ( select != item->isSelected() ) {
	    item->setSelected( select, TRUE );
	    rr = rr.unite( item->rect() );
	    changed = TRUE;
	}
    }
    viewport()->setUpdatesEnabled( ue );
    repaintContents( rr, FALSE );
    if ( i )
	setCurrentItem( i );
    blockSignals( b );
    if ( changed ) {
	emit selectionChanged();
    }
}

/*!
  Inverts the selection. Works only in Multi and Extended selection mode.
*/

void QIconView::invertSelection()
{
    if ( d->selectionMode == Single ||
	 d->selectionMode == NoSelection )
	return;

    bool b = signalsBlocked();
    blockSignals( TRUE );
    QIconViewItem *item = d->firstItem;
    for ( ; item; item = item->next )
	item->setSelected( !item->isSelected(), TRUE );
    blockSignals( b );
    emit selectionChanged();
}

/*!
  Repaints the \a item.
*/

void QIconView::repaintItem( QIconViewItem *item )
{
    if ( !item || item->dirty )
	return;

    if ( QRect( contentsX(), contentsY(), visibleWidth(), visibleHeight() ).
	 intersects( QRect( item->x() - 1, item->y() - 1, item->width() + 2, item->height() + 2 ) ) )
	repaintContents( item->x() - 1, item->y() - 1, item->width() + 2, item->height() + 2, FALSE );
}

/*!
  Makes sure, that \a item is visible, and scrolls the view if required.
*/

void QIconView::ensureItemVisible( QIconViewItem *item )
{
    if ( !item )
	return;

    if ( d->updateTimer && d->updateTimer->isActive() ||
	 d->fullRedrawTimer && d->fullRedrawTimer->isActive() )
	slotUpdate();

    int w = item->width();
    int h = item->height();
    ensureVisible( item->x() + w / 2, item->y() + h / 2,
		   w / 2 + 1, h / 2 + 1 );
}

/*!
  Finds the first item which is visible in the rectangle \a r in contents coordinates. If no items are
  visible at all, 0 is returned.
*/

QIconViewItem* QIconView::findFirstVisibleItem( const QRect &r ) const
{
    QIconViewPrivate::ItemContainer *c = d->firstContainer;
    QIconViewItem *i = 0;
    bool alreadyIntersected = FALSE;
    for ( ; c; c = c->n ) {
	if ( c->rect.intersects( r ) ) {
	    alreadyIntersected = TRUE;
	    QIconViewItem *item = c->items.first();
	    for ( ; item; item = c->items.next() ) {
		if ( r.intersects( item->rect() ) ) {
		    if ( !i ) {
			i = item;
		    } else {
			QRect r2 = item->rect();
			QRect r3 = i->rect();
			if ( r2.y() < r3.y() )
			    i = item;
			else if ( r2.y() == r3.y() &&
				  r2.x() < r3.x() )
			    i = item;
		    }
		}
	    }
	} else {
	    if ( alreadyIntersected )
		break;
	}
    }

    return i;
}

/*!
  Finds the last item which is visible in the rectangle \a r in contents coordinates. If no items are
  visible at all, 0 is returned.
*/

QIconViewItem* QIconView::findLastVisibleItem( const QRect &r ) const
{
    QIconViewPrivate::ItemContainer *c = d->firstContainer;
    QIconViewItem *i = 0;
    bool alreadyIntersected = FALSE;
    for ( ; c; c = c->n ) {
	if ( c->rect.intersects( r ) ) {
	    alreadyIntersected = TRUE;
	    QIconViewItem *item = c->items.first();
	    for ( ; item; item = c->items.next() ) {
		if ( r.intersects( item->rect() ) ) {
		    if ( !i ) {
			i = item;
		    } else {
			QRect r2 = item->rect();
			QRect r3 = i->rect();
			if ( r2.y() > r3.y() )
			    i = item;
			else if ( r2.y() == r3.y() &&
				  r2.x() > r3.x() )
			    i = item;
		    }
		}
	    }
	} else {
	    if ( alreadyIntersected )
		break;
	}
    }

    return i;
}

/*!
  Cleares the iconview.
*/

void QIconView::clear()
{
    d->clearing = TRUE;
    bool block = signalsBlocked();
    blockSignals( TRUE );
    clearSelection();
    blockSignals( block );
    setContentsPos( 0, 0 );
    d->currentItem = 0;

    if ( !d->firstItem ) {
	d->clearing = FALSE;
	return;
    }

    QIconViewItem *item = d->firstItem, *tmp;
    d->firstItem = 0;
    while ( item ) {
	tmp = item->next;
	delete item;
	item = tmp;
    }
    QIconViewPrivate::ItemContainer *c = d->firstContainer, *tmpc;
    while ( c ) {
	tmpc = c->n;
	delete c;
	c = tmpc;
    }
    d->firstContainer = d->lastContainer = 0;

    d->count = 0;
    d->lastItem = 0;
    setCurrentItem( 0 );
    d->highlightedItem = 0;
    d->tmpCurrentItem = 0;
    d->drawDragShapes = FALSE;

    resizeContents( 0, 0 );
    // maybe we don�t need this update, so delay it
    d->fullRedrawTimer->start( 0, TRUE );

    d->cleared = TRUE;
    d->clearing = FALSE;
}

/*!
  Sets the horizontal grid to \a rx.  If \a rx is -1, there is no
  horizontal grid used for arranging items.
*/

void QIconView::setGridX( int rx )
{
    d->rastX = rx;
}

/*!
  Sets the vertical grid to \a ry.  If \a ry is -1, there is no
  vertical grid used for arranging items.
*/

void QIconView::setGridY( int ry )
{
    d->rastY = ry;
}

/*!
  Returns the horizontal grid.

  \sa QIconView::setGridX()
*/

int QIconView::gridX() const
{
    return d->rastX;
}

/*!
  Returns the vertica grid.

  \sa QIconView::setGridY()
*/

int QIconView::gridY() const
{
    return d->rastY;
}

/*!
  Sets the space between iconview items to \a sp.
*/

void QIconView::setSpacing( int sp )
{
    d->spacing = sp;
}

/*!
  Returns the spacing between iconview items.
*/

int QIconView::spacing() const
{
    return d->spacing;
}

/*!
  Sets the position, where the text of the items is drawn. This can be Bottom
  or Right.
*/

void QIconView::setItemTextPos( ItemTextPos pos )
{
    if ( pos == d->itemTextPos )
	return;

    d->itemTextPos = pos;

    QIconViewItem *item = d->firstItem;
    for ( ; item; item = item->next ) {
	item->wordWrapDirty = TRUE;
	item->calcRect();
    }

    arrangeItemsInGrid( TRUE );
}

/*!
  Returns the position, at which the text of the items are drawn.

  \sa QIconView::setItemTextPos()
*/

QIconView::ItemTextPos QIconView::itemTextPos() const
{
    return d->itemTextPos;
}

/*!
  Sets the \a brush, which should be used when drawing the background
  of an item text. By default, this brush is set to NoBrush, which means
  no extra brush is used for drawing the item text background (just the normal
  iconview background).
*/

void QIconView::setItemTextBackground( const QBrush &brush )
{
    d->itemTextBrush = brush;
}

/*!
  Returns the brush which is used to draw the background of an item text

  \sa setItemTextBackground()
*/

QBrush QIconView::itemTextBackground() const
{
    return d->itemTextBrush;
}

/*!
  Sets the arrangement mode of the iconview to \a am, which must be
  one of \c LeftToRight and TopToBottom.

  \sa Arrangement
*/

void QIconView::setArrangement( Arrangement am )
{
    if ( d->arrangement == am )
	return;

    d->arrangement = am;

    viewport()->setUpdatesEnabled( FALSE );
    resizeContents( viewport()->width(), viewport()->height() );
    viewport()->setUpdatesEnabled( TRUE );
    arrangeItemsInGrid( TRUE );
}

/*!
  Returns the arrangement mode of the iconview.

  \sa QIconView::setArrangement()
*/

QIconView::Arrangement QIconView::arrangement() const
{
    return d->arrangement;
}

/*!
  Sets the resize mode of the iconview to \a rm, which must be one of
  \c Fixed and Adjust.

  \sa ResizeMode
*/

void QIconView::setResizeMode( ResizeMode rm )
{
    if ( d->resizeMode == rm )
	return;

    d->resizeMode = rm;
}

/*!
  Returns the resize mode of the iconview.

  \sa QIconView::setResizeMode()
*/

QIconView::ResizeMode QIconView::resizeMode() const
{
    return d->resizeMode;
}

/*!
  Sets the maximum width, which an item may have. If a gridX() is set,
  this value is ignored, and the gridX() value is used.
*/

void QIconView::setMaxItemWidth( int w )
{
    d->maxItemWidth = w;
}

/*!
  Sets the maximum length (in characters), which an item text may have.
*/

void QIconView::setMaxItemTextLength( int w )
{
    d->maxItemTextLength = w;
}

/*!
  Returns the maximum width (in pixels), which an item may have.

  \sa QIconView::setMaxItemWidth()
*/

int QIconView::maxItemWidth() const
{
    if ( d->rastX != -1 )
	return d->rastX - 2;
    else
	return d->maxItemWidth;
}

/*!
  Returns the maximum length (in characters), which the
  text of an icon may have.

  \sa QIconView::setMaxItemTextLength()
*/

int QIconView::maxItemTextLength() const
{
    return d->maxItemTextLength;
}

/*!
  If \a b is TRUE, the user is allowed to move items around in
  the iconview.
  if \a b is FALSE, the user is not allowed to do that.
*/

void QIconView::setItemsMovable( bool b )
{
    d->rearrangeEnabled = b;
}

/*!
  Returns TRUE, if the user is allowed to move items around
  in the iconview, else FALSE;

  \sa QIconView::setItemsMovable()
*/

bool QIconView::itemsMovable() const
{
    return d->rearrangeEnabled;
}

/*!
  If \a b is TRUE, all items are re-arranged in the grid if a new one is
  inserted. Else, the best fitting place for the new item is searched and
  the other ones are not touched.

  This setting only applies if the iconview is visible. If you insert
  items and the iconview is not visible, the icons are reordered when it
  gets visible.
*/

void QIconView::setAutoArrange( bool b )
{
    d->reorderItemsWhenInsert = b;
}

/*!
  Returns TRUE if all items are re-arranged in the grid if a new one is
  inserted, else FALSE.

  \sa QIconView::setAutoArrange()
*/

bool QIconView::autoArrange() const
{
    return d->reorderItemsWhenInsert;
}

/*!
  If \a sort is TRUE, new items are inserted sorted. The sort
  direction is specified using \a ascending.

  Inserting items sorted only works when re-arranging items is
  set to TRUE as well (using QIconView::setAutoArrange()).

  \sa QIconView::setAutoArrange(), QIconView::autoArrange()
*/

void QIconView::setSorting( bool sort, bool ascending )
{
    d->resortItemsWhenInsert = sort;
    d->sortDirection = ascending;
}

/*!
  Returns TRUE if new items are inserted sorted, else FALSE.

  \sa QIconView::setSorting()
*/

bool QIconView::sorting() const
{
    return d->resortItemsWhenInsert;
}

/*!
  Returns TRUE if the sort direction for inserting new items is ascending,
  FALSE means descending. This sort direction has only a meaning if re-sorting
  and re-arranging of new inserted items is enabled.

  \sa QIconView::setSorting(), QIconView::setAutoArrange()
*/

bool QIconView::sortDirection() const
{
    return d->sortDirection;
}

/*!
  If the width of an item text is larger than the maximal item width,
  there are two possibilities how the QIconView can deal with this.
  Either it does a word wrap of the item text, so that it uses
  multiple lines. Or it truncates the item text so that it shrinks
  to the maximal item width and appends three dots "..." to the
  displayed text to indicate that not the full text is displayed.

  If you set \a b to TRUE, a word wrap is done, else the
  text is displayed truncated.

  NOTE: Both possibilities just change the way how the text is
  displayed, they do NOT modify the item text itslef.

  \sa setShowToolTips()
*/

void QIconView::setWordWrapIconText( bool b )
{
    if ( d->wordWrapIconText == b )
	return;

    d->wordWrapIconText = b;
    for ( QIconViewItem *item = d->firstItem; item; item = item->next ) {
	item->wordWrapDirty = TRUE;
	item->calcRect();
    }
    arrangeItemsInGrid( TRUE );
}

/*!
  Returns TRUE, if an item text which needs too much
  space (to the width) is displayed word wrapped, or FALSE
  if it gets displayed truncated.

  \sa setWordWrapIconText(), setShowToolTips()
*/

bool QIconView::wordWrapIconText() const
{
    return d->wordWrapIconText;
}

/*!
  If wordWrapIconText() is FALSE, it happens that an item text
  is truncated because it's too large for one line. If you specify TRUE for
  \a b here and the user moves the mouse onto the item a tooltip with
  the whole item text is shown.
  If you pass \a FALSE here this feature is switched off.

  \sa setWordWrapIconText()
*/

void QIconView::setShowToolTips( bool b )
{
    d->showTips = b;
}

/*!
  Returns TRUE if a tooltip is shown for truncated item texts and
  FALSE otherwise.

  \sa setShowToolTips(), setWordWrapIconText()
*/

bool QIconView::showToolTips() const
{
    return d->showTips;
}

/*!
  \reimp
*/

void QIconView::contentsMousePressEvent( QMouseEvent *e )
{
    if ( d->rubber ) {
	QPainter p;
	p.begin( viewport() );
	p.setRasterOp( NotROP );
	p.setPen( QPen( color0, 1 ) );
	p.setBrush( NoBrush );

	drawRubber( &p );
	d->dragging = FALSE;
	p.end();
	delete d->rubber;
	d->rubber = 0;
    }

    d->dragStartPos = e->pos();
    QIconViewItem *item = findItem( e->pos() );
    d->pressedItem = item;

    if ( item )
	d->selectAnchor = item;

    if ( d->currentItem )
	d->currentItem->renameItem();

    if ( !d->currentItem && !item && d->firstItem ) {
	d->currentItem = d->firstItem;
	repaintItem( d->firstItem );
    }

    d->startDragItem = 0;

    if ( e->button() == LeftButton && !( e->state() & ShiftButton ) &&
	 !( e->state() & ControlButton ) && item && item->isSelected() &&
	 item->textRect( FALSE ).contains( e->pos() ) ) {

	if ( !item->renameEnabled() ) {
	    d->mousePressed = TRUE;
	} else {
	    ensureItemVisible( item );
	    setCurrentItem( item );
	    item->rename();
	    goto emit_signals;
	}
    }

    d->pressedSelected = item && item->isSelected();

    if ( item && item->isSelectable() ) {
	if ( d->selectionMode == Single )
	    item->setSelected( TRUE, e->state() & ControlButton );
	else if ( d->selectionMode == Multi )
	    item->setSelected( !item->isSelected(), e->state() & ControlButton );
	else if ( d->selectionMode == Extended ) {
	    if ( e->state() & ShiftButton ) {
		d->pressedSelected = FALSE;
		bool block = signalsBlocked();
		blockSignals( TRUE );
		viewport()->setUpdatesEnabled( FALSE );
		QRect r;
		bool select = TRUE;
		if ( d->currentItem )
		    r = QRect( QMIN( d->currentItem->x(), item->x() ),
			       QMIN( d->currentItem->y(), item->y() ),
			       0, 0 );
		else
		    r = QRect( 0, 0, 0, 0 );
		if ( d->currentItem ) {
		    if ( d->currentItem->x() < item->x() )
			r.setWidth( item->x() - d->currentItem->x() + item->width() );
		    else
			r.setWidth( d->currentItem->x() - item->x() + d->currentItem->width() );
		    if ( d->currentItem->y() < item->y() )
			r.setHeight( item->y() - d->currentItem->y() + item->height() );
		    else
			r.setHeight( d->currentItem->y() - item->y() + d->currentItem->height() );
		    r = r.normalize();
		    QIconViewPrivate::ItemContainer *c = d->firstContainer;
		    bool alreadyIntersected = FALSE;
		    QRect redraw;
		    for ( ; c; c = c->n ) {
			if ( c->rect.intersects( r ) ) {
			    alreadyIntersected = TRUE;
			    QIconViewItem *i = c->items.first();
			    for ( ; i; i = c->items.next() ) {
				if ( r.intersects( i->rect() ) ) {
				    redraw = redraw.unite( i->rect() );
				    i->setSelected( select, TRUE );
				}
			    }
			} else {
			    if ( alreadyIntersected )
				break;
			}
		    }
		    redraw = redraw.unite( item->rect() );
		    viewport()->setUpdatesEnabled( TRUE );
		    repaintContents( redraw, FALSE );
		}
		blockSignals( block );
		viewport()->setUpdatesEnabled( TRUE );
		item->setSelected( select, TRUE );
		emit selectionChanged();
	    } else if ( e->state() & ControlButton ) {
		d->pressedSelected = FALSE;
		item->setSelected( !item->isSelected(), e->state() & ControlButton );
	    } else {
		item->setSelected( TRUE, e->state() & ControlButton );
	    }
	}
    } else if ( ( d->selectionMode != Single || e->button() == RightButton )
		&& !( e->state() & ControlButton ) )
	selectAll( FALSE );

    setCurrentItem( item );

    if ( e->button() == LeftButton ) {
	if ( !item && ( d->selectionMode == Multi ||
				  d->selectionMode == Extended ) ) {
	    d->tmpCurrentItem = d->currentItem;
	    d->currentItem = 0;
	    repaintItem( d->tmpCurrentItem );
	    if ( d->rubber )
		delete d->rubber;
	    d->rubber = 0;
	    d->rubber = new QRect( e->x(), e->y(), 0, 0 );
	    d->selectedItems.clear();
	    if ( ( e->state() & ControlButton ) == ControlButton ) {
		for ( QIconViewItem *i = firstItem(); i; i = i->nextItem() )
		    if ( i->isSelected() )
			d->selectedItems.insert( i, i );
	    }
	}

	d->mousePressed = TRUE;
    }

 emit_signals:
    if ( !d->rubber ) {
	emit mouseButtonPressed( e->button(), item, e->globalPos() );
	emit pressed( item );
	emit pressed( item, e->globalPos() );

	if ( e->button() == RightButton )
	    emit rightButtonPressed( item, e->globalPos() );
    }
}

/*!
  \reimp
*/

void QIconView::contentsMouseReleaseEvent( QMouseEvent *e )
{
    QIconViewItem *item = findItem( e->pos() );
    d->selectedItems.clear();

    bool emitClicked = TRUE;
    d->mousePressed = FALSE;
    d->startDragItem = 0;

    if ( d->rubber ) {
	emitClicked = FALSE;
	QPainter p;
	p.begin( viewport() );
	p.setRasterOp( NotROP );
	p.setPen( QPen( color0, 1 ) );
	p.setBrush( NoBrush );

	drawRubber( &p );
	d->dragging = FALSE;
	p.end();

	if ( ( d->rubber->topLeft() - d->rubber->bottomRight() ).manhattanLength() >
	     QApplication::startDragDistance() )
	    emitClicked = FALSE;
	delete d->rubber;
	d->rubber = 0;
	d->currentItem = d->tmpCurrentItem;
	d->tmpCurrentItem = 0;
	if ( d->currentItem )
	    repaintItem( d->currentItem );
    }

    if ( d->scrollTimer ) {
	disconnect( d->scrollTimer, SIGNAL( timeout() ), this, SLOT( doAutoScroll() ) );
	d->scrollTimer->stop();
	delete d->scrollTimer;
	d->scrollTimer = 0;
    }

    if ( d->selectionMode == Extended &&
	 d->currentItem == d->pressedItem &&
	 d->pressedSelected && d->currentItem ) {
	bool block = signalsBlocked();
	blockSignals( TRUE );
	clearSelection();
	blockSignals( block );
	if ( d->currentItem->isSelectable() ) {
	    d->currentItem->selected = TRUE;
	    repaintItem( d->currentItem );
	}
	emit selectionChanged();
    }
    d->pressedItem = 0;

    if ( emitClicked ) {
	emit mouseButtonClicked( e->button(), item, e->globalPos() );
	emit clicked( item );
	emit clicked( item, e->globalPos() );
	if ( e->button() == RightButton ) {
	    emit rightButtonClicked( item, e->globalPos() );
	}
    }
}

/*!
  \reimp
*/

void QIconView::contentsMouseMoveEvent( QMouseEvent *e )
{
    QIconViewItem *item = findItem( e->pos() );
    if ( d->highlightedItem != item ) {
	if ( item )
	    emit onItem( item );
	else
	    emit onViewport();
	d->highlightedItem = item;
    }

    if ( d->mousePressed && e->state() == NoButton )
	d->mousePressed = FALSE;

    if ( d->startDragItem )
	item = d->startDragItem;

    if ( d->mousePressed && item && item == d->currentItem &&
	 ( item->isSelected() || d->selectionMode == NoSelection ) && item->dragEnabled() ) {
	if ( !d->startDragItem ) {
	    d->currentItem->setSelected( TRUE, TRUE );
	    d->startDragItem = item;
	}
	if ( ( d->dragStartPos - e->pos() ).manhattanLength() > QApplication::startDragDistance() ) {
	    d->mousePressed = FALSE;
	    d->cleared = FALSE;
#ifndef QT_NO_DRAGANDDROP
	    startDrag();
#endif
	    if ( d->tmpCurrentItem )
		repaintItem( d->tmpCurrentItem );
	}
    } else if ( d->mousePressed && !d->currentItem && d->rubber ) {
	doAutoScroll();
    }
}

/*!
  \reimp
*/

void QIconView::contentsMouseDoubleClickEvent( QMouseEvent *e )
{
    QIconViewItem *item = findItem( e->pos() );
    if ( item ) {
	selectAll( FALSE );
	item->setSelected( TRUE, TRUE );
	emit doubleClicked( item );
    }
}

/*!
  \reimp
*/

#ifndef QT_NO_DRAGANDDROP
void QIconView::contentsDragEnterEvent( QDragEnterEvent *e )
{
    d->dragging = TRUE;
    d->drawDragShapes = TRUE;
    d->tmpCurrentItem = 0;
    initDragEnter( e );
    d->oldDragPos = e->pos();
    d->oldDragAcceptAction = FALSE;
    drawDragShapes( e->pos() );
    d->dropped = FALSE;
}

/*!
  \reimp
*/

void QIconView::contentsDragMoveEvent( QDragMoveEvent *e )
{
    if ( e->pos() == d->oldDragPos ) {
	if (d->oldDragAcceptAction)
	    e->acceptAction();
	else
	    e->ignore();
	return;
    }

    drawDragShapes( d->oldDragPos );
    d->dragging = FALSE;

    QIconViewItem *old = d->tmpCurrentItem;
    d->tmpCurrentItem = 0;

    QIconViewItem *item = findItem( e->pos() );

    if ( item ) {
	if ( old ) {
	    old->dragLeft();
	    repaintItem( old );
	}
	item->dragEntered();
	if ( item->acceptDrop( e ) ) {
	    d->oldDragAcceptAction = TRUE;
	    e->acceptAction();
	} else {
	    d->oldDragAcceptAction = FALSE;
	    e->ignore();
	}

	d->tmpCurrentItem = item;
	QPainter p;
	p.begin( viewport() );
	p.translate( -contentsX(), -contentsY() );
	item->paintFocus( &p, colorGroup() );
	p.end();
    } else {
	e->acceptAction();
	d->oldDragAcceptAction = TRUE;
	if ( old ) {
	    old->dragLeft();
	    repaintItem( old );
	}
    }

    d->oldDragPos = e->pos();
    drawDragShapes( e->pos() );
    d->dragging = TRUE;
}

/*!
  \reimp
*/

void QIconView::contentsDragLeaveEvent( QDragLeaveEvent * )
{
    if ( !d->dropped )
	drawDragShapes( d->oldDragPos );
    d->dragging = FALSE;

    if ( d->tmpCurrentItem ) {
	repaintItem( d->tmpCurrentItem );
	d->tmpCurrentItem->dragLeft();
    }

    d->tmpCurrentItem = 0;
    d->isIconDrag = FALSE;
    d->iconDragData.clear();
}

/*!
  \reimp
*/

void QIconView::contentsDropEvent( QDropEvent *e )
{
    d->dropped = TRUE;
    d->dragging = FALSE;
    drawDragShapes( d->oldDragPos );

    if ( d->tmpCurrentItem )
	repaintItem( d->tmpCurrentItem );

    QIconViewItem *i = findItem( e->pos() );

    if ( !i && e->source() == viewport() && d->currentItem && !d->cleared ) {
	if ( !d->rearrangeEnabled )
	    return;
	QRect r = d->currentItem->rect();

	d->currentItem->move( e->pos() - d->dragStart );

	int w = d->currentItem->x() + d->currentItem->width() + 1;
	int h = d->currentItem->y() + d->currentItem->height() + 1;

	repaintItem( d->currentItem );
	repaintContents( r.x(), r.y(), r.width(), r.height(), FALSE );

	if ( d->selectionMode != Single ) {
	    int dx = d->currentItem->x() - r.x();
	    int dy = d->currentItem->y() - r.y();

	    QIconViewItem *item = d->firstItem;
	    QRect rr;
	    for ( ; item; item = item->next ) {
		if ( item->isSelected() && item != d->currentItem ) {
		    rr = rr.unite( item->rect() );
		    item->moveBy( dx, dy );
		    rr = rr.unite( item->rect() );
		    w = QMAX( w, item->x() + item->width() + 1 );
		    h = QMAX( h, item->y() + item->height() + 1 );
		}
	    }
	    repaintContents( rr, FALSE );
	}
	bool fullRepaint = FALSE;
	if ( w > contentsWidth() ||
	     h > contentsHeight() )
	    fullRepaint = TRUE;

	int oldw = contentsWidth();
	int oldh = contentsHeight();

	resizeContents( QMAX( contentsWidth(), w ), QMAX( contentsHeight(), h ) );

	if ( fullRepaint ) {
	    repaintContents( oldw, 0, contentsWidth() - oldw, contentsHeight(), FALSE );
	    repaintContents( 0, oldh, contentsWidth(), contentsHeight() - oldh, FALSE );
	}
	e->acceptAction();
    } else if ( !i && ( e->source() != viewport() || d->cleared ) ) {
	QValueList<QIconDragItem> lst;
	if ( QIconDrag::canDecode( e ) ) {
	    QValueList<QIconDragDataItem> l;
	    QIconDragPrivate::decode( e, l );
	    QValueList<QIconDragDataItem>::Iterator it = l.begin();
	    for ( ; it != l.end(); ++it )
		lst << ( *it ).data;
	}
	emit dropped( e, lst );
    } else if ( i ) {
	QValueList<QIconDragItem> lst;
	if ( QIconDrag::canDecode( e ) ) {
	    QValueList<QIconDragDataItem> l;
	    QIconDragPrivate::decode( e, l );
	    QValueList<QIconDragDataItem>::Iterator it = l.begin();
	    for ( ; it != l.end(); ++it )
		lst << ( *it ).data;
	}
	i->dropped( e, lst );
    }
    d->isIconDrag = FALSE;
}
#endif

/*!
  \reimp
*/

void QIconView::resizeEvent( QResizeEvent* e )
{
    QScrollView::resizeEvent( e );
    if ( d->resizeMode == Adjust ) {
	d->oldSize = e->oldSize();
	if ( d->adjustTimer->isActive() )
	    d->adjustTimer->stop();
	d->adjustTimer->start( 100, TRUE );
    }
}

/*!
  Adjusts the positions of the items to the geometry of the iconview.
*/

void QIconView::adjustItems()
{
    d->adjustTimer->stop();
    if ( d->resizeMode == Adjust )
	    arrangeItemsInGrid( TRUE );
}

/*!
  \reimp
*/

void QIconView::keyPressEvent( QKeyEvent *e )
{
    if ( !d->firstItem )
	return;

    if ( !d->currentItem ) {
	setCurrentItem( d->firstItem );
	if ( d->selectionMode == Single )
	    d->currentItem->setSelected( TRUE, TRUE );
	return;
    }

    bool selectCurrent = TRUE;

    switch ( e->key() ) {
    case Key_Escape:
	e->ignore();
	break;
    case Key_F2: {
	if ( d->currentItem->renameEnabled() ) {
	    d->currentItem->renameItem();
	    d->currentItem->rename();
	    return;
	}
    } break;
    case Key_Home: {
	d->currInputString = QString::null;
	if ( !d->firstItem )
	    break;

	selectCurrent = FALSE;

	QIconViewItem *item = d->currentItem;
	setCurrentItem( d->firstItem );

	handleItemChange( item, e->state() & ShiftButton, e->state() & ControlButton );
    } break;
    case Key_End: {
	d->currInputString = QString::null;
	if ( !d->lastItem )
	    break;

	selectCurrent = FALSE;

	QIconViewItem *item = d->currentItem;
	setCurrentItem( d->lastItem );

	handleItemChange( item, e->state() & ShiftButton, e->state() & ControlButton );
    } break;
    case Key_Right: {
	d->currInputString = QString::null;
	QIconViewItem *item;
	selectCurrent = FALSE;
	if ( d->arrangement == LeftToRight ) {
	    if ( !d->currentItem->next )
		break;

	    item = d->currentItem;
	    setCurrentItem( d->currentItem->next );

	    handleItemChange( item, e->state() & ShiftButton, e->state() & ControlButton );
	} else {
	    item = d->firstItem;
	    QRect r( 0, d->currentItem->y(), contentsWidth(), d->currentItem->height() );
	    for ( ; item; item = item->next ) {
		if ( item->x() > d->currentItem->x() && r.intersects( item->rect() ) ) {
		    QRect ir = r.intersect( item->rect() );
		    if ( item->next && r.intersects( item->next->rect() ) ) {
			QRect irn = r.intersect( item->next->rect() );
			if ( irn.height() > ir.height() )
			    item = item->next;
		    }
		    QIconViewItem *i = d->currentItem;
		    setCurrentItem( item );
		    item = i;
		    handleItemChange( item, e->state() & ShiftButton, e->state() & ControlButton );
		    break;
		}
	    }
	}
    } break;
    case Key_Left: {
	d->currInputString = QString::null;
	QIconViewItem *item;
	selectCurrent = FALSE;
	if ( d->arrangement == LeftToRight ) {
	    if ( !d->currentItem->prev )
		break;

	    item = d->currentItem;
	    setCurrentItem( d->currentItem->prev );

	    handleItemChange( item, e->state() & ShiftButton, e->state() & ControlButton );
	} else {
	    item = d->lastItem;
	    QRect r( 0, d->currentItem->y(), contentsWidth(), d->currentItem->height() );
	    for ( ; item; item = item->prev ) {
		if ( item->x() < d->currentItem->x() && r.intersects( item->rect() ) ) {
		    QRect ir = r.intersect( item->rect() );
		    if ( item->prev && r.intersects( item->prev->rect() ) ) {
			QRect irn = r.intersect( item->prev->rect() );
			if ( irn.height() > ir.height() )
			    item = item->prev;
		    }
		    QIconViewItem *i = d->currentItem;
		    setCurrentItem( item );
		    item = i;
		    handleItemChange( item, e->state() & ShiftButton, e->state() & ControlButton );
		    break;
		}
	    }
	}
    } break;
    case Key_Space: {
	d->currInputString = QString::null;
	if ( d->selectionMode == Single)
	    break;

	d->currentItem->setSelected( !d->currentItem->isSelected(), TRUE );
    } break;
    case Key_Enter: case Key_Return:
	d->currInputString = QString::null;
	emit returnPressed( d->currentItem );
	break;
    case Key_Down: {
	d->currInputString = QString::null;
	QIconViewItem *item;
	selectCurrent = FALSE;

	if ( d->arrangement == LeftToRight ) {
	    item = d->firstItem;
	    QRect r( d->currentItem->x(), 0, d->currentItem->width(), contentsHeight() );
	    for ( ; item; item = item->next ) {
		if ( item->y() > d->currentItem->y() && r.intersects( item->rect() ) ) {
		    QRect ir = r.intersect( item->rect() );
		    if ( item->next && r.intersects( item->next->rect() ) ) {
			QRect irn = r.intersect( item->next->rect() );
			if ( irn.width() > ir.width() )
			    item = item->next;
		    }
		    QIconViewItem *i = d->currentItem;
		    setCurrentItem( item );
		    item = i;
		    handleItemChange( item, e->state() & ShiftButton, e->state() & ControlButton );
		    break;
		}
	    }
	} else {
	    if ( !d->currentItem->next )
		break;

	    item = d->currentItem;
	    setCurrentItem( d->currentItem->next );

	    handleItemChange( item, e->state() & ShiftButton, e->state() & ControlButton );
	}
    } break;
    case Key_Up: {
	d->currInputString = QString::null;
	QIconViewItem *item;
	selectCurrent = FALSE;

	if ( d->arrangement == LeftToRight ) {
	    item = d->lastItem;
	    QRect r( d->currentItem->x(), 0, d->currentItem->width(), contentsHeight() );
	    for ( ; item; item = item->prev ) {
		if ( item->y() < d->currentItem->y() && r.intersects( item->rect() ) ) {
		    QRect ir = r.intersect( item->rect() );
		    if ( item->prev && r.intersects( item->prev->rect() ) ) {
			QRect irn = r.intersect( item->prev->rect() );
			if ( irn.width() > ir.width() )
			    item = item->prev;
		    }
		    QIconViewItem *i = d->currentItem;
		    setCurrentItem( item );
		    item = i;
		    handleItemChange( item, e->state() & ShiftButton, e->state() & ControlButton );
		    break;
		}
	    }
	} else {
	    if ( !d->currentItem->prev )
		break;

	    item = d->currentItem;
	    setCurrentItem( d->currentItem->prev );

	    handleItemChange( item, e->state() & ShiftButton, e->state() & ControlButton );
	}
    } break;
    case Key_Next: {
	d->currInputString = QString::null;
	selectCurrent = FALSE;
	QRect r;
	if ( d->arrangement == LeftToRight )
	    r = QRect( 0, d->currentItem->y() + visibleHeight(), contentsWidth(), visibleHeight() );
	else
	    r = QRect( d->currentItem->x() + visibleWidth(), 0, visibleWidth(), contentsHeight() );
	QIconViewItem *item = d->currentItem;
	QIconViewItem *ni = findFirstVisibleItem( r  );
	if ( !ni ) {
	    if ( d->arrangement == LeftToRight )
		r = QRect( 0, d->currentItem->y() + d->currentItem->height(), contentsWidth(), contentsHeight() );
	    else
		r = QRect( d->currentItem->x() + d->currentItem->width(), 0, contentsWidth(), contentsHeight() );
	    ni = findLastVisibleItem( r  );
	}
	if ( ni ) {
	    setCurrentItem( ni );
	    handleItemChange( item, e->state() & ShiftButton, e->state() & ControlButton );
	}
    } break;
    case Key_Prior: {
	d->currInputString = QString::null;
	selectCurrent = FALSE;
	QRect r;
	if ( d->arrangement == LeftToRight )
	    r = QRect( 0, d->currentItem->y() - visibleHeight(), contentsWidth(), visibleHeight() );
	else
	    r = QRect( d->currentItem->x() - visibleWidth(), 0, visibleWidth(), contentsHeight() );
	QIconViewItem *item = d->currentItem;
	QIconViewItem *ni = findFirstVisibleItem( r  );
	if ( !ni ) {
	    if ( d->arrangement == LeftToRight )
		r = QRect( 0, 0, contentsWidth(), d->currentItem->y() );
	    else
		r = QRect( 0, 0, d->currentItem->x(), contentsHeight() );
	    ni = findFirstVisibleItem( r  );
	}
	if ( ni ) {
	    setCurrentItem( ni );
	    handleItemChange( item, e->state() & ShiftButton, e->state() & ControlButton );
	}
    } break;
    default:
	if ( !e->text().isEmpty() && e->text()[ 0 ].isPrint() ) {
	    selectCurrent = FALSE;
	    findItemByName( e->text() );
	} else {
	    selectCurrent = FALSE;
	    d->currInputString = QString::null;
	    if ( e->state() & ControlButton ) {
		switch ( e->key() ) {
		case Key_A:
		    selectAll( TRUE );
		    break;
		}
	    }
	}
    }

    if ( !( e->state() & ShiftButton ) || !d->selectAnchor )
	d->selectAnchor = d->currentItem;

    if ( d->currentItem && !d->currentItem->isSelected() &&
	 d->selectionMode == Single && selectCurrent ) {
	d->currentItem->setSelected( TRUE );
    }

    if ( e->key() != Key_Shift &&
	 e->key() != Key_Control &&
	 e->key() != Key_Alt )
	ensureItemVisible( d->currentItem );
}

/*!
  \reimp
*/

void QIconView::focusInEvent( QFocusEvent *e )
{
    d->mousePressed = FALSE;
    if ( d->currentItem )
	repaintItem( d->currentItem );
    else if ( d->firstItem && e->reason() != QFocusEvent::Mouse ) {
	d->currentItem = d->firstItem;
	emit currentChanged( d->currentItem );
	repaintItem( d->currentItem );
    }
}

/*!
  \reimp
*/

void QIconView::focusOutEvent( QFocusEvent * )
{
    if ( d->currentItem )
	repaintItem( d->currentItem );
}

/*!
  Draws the rubber band using the painter \a p.
*/

void QIconView::drawRubber( QPainter *p )
{
    if ( !p || !d->rubber )
	return;

    QPoint pnt( d->rubber->x(), d->rubber->y() );
    pnt = contentsToViewport( pnt );
    style().drawFocusRect( p, QRect( pnt.x(), pnt.y(), d->rubber->width(), d->rubber->height() ),
			   colorGroup(), &colorGroup().base() );
}

/*!
  Returns the QDragObject which should be used for DnD. This method
  is called by the iconview when starting a drag to get the dragobject
  which should be used for the drag.
  Subclasses may reimplement this.

  \sa QIconDrag
*/

#ifndef QT_NO_DRAGANDDROP
QDragObject *QIconView::dragObject()
{
    if ( !d->currentItem )
	return 0;

    QPoint orig = d->dragStartPos;

    QIconDrag *drag = new QIconDrag( viewport() );
    drag->setPixmap( *d->currentItem->pixmap(),
 		     QPoint( d->currentItem->pixmapRect().width() / 2,
			     d->currentItem->pixmapRect().height() / 2 ) );

    if ( d->selectionMode == NoSelection ) {
	QIconViewItem *item = d->currentItem;
	drag->append( QIconDragItem(),
		      QRect( item->pixmapRect( FALSE ).x() - orig.x(),
			     item->pixmapRect( FALSE ).y() - orig.y(),
			     item->pixmapRect().width(), item->pixmapRect().height() ),
		      QRect( item->textRect( FALSE ).x() - orig.x(),
			     item->textRect( FALSE ).y() - orig.y(),
			     item->textRect().width(), item->textRect().height() ) );
    } else {
	for ( QIconViewItem *item = d->firstItem; item; item = item->next ) {
	    if ( item->isSelected() ) {
		drag->append( QIconDragItem(),
			      QRect( item->pixmapRect( FALSE ).x() - orig.x(),
				     item->pixmapRect( FALSE ).y() - orig.y(),
				     item->pixmapRect().width(), item->pixmapRect().height() ),
			      QRect( item->textRect( FALSE ).x() - orig.x(),
				     item->textRect( FALSE ).y() - orig.y(),
				     item->textRect().width(), item->textRect().height() ) );
	    }
	}
    }

    return drag;
}

/*!
  Starts a drag.
*/

void QIconView::startDrag()
{
    if ( !d->startDragItem )
	return;

    QPoint orig = d->dragStartPos;
    d->dragStart = QPoint( orig.x() - d->startDragItem->x(),
			   orig.y() - d->startDragItem->y() );
    d->startDragItem = 0;
    d->mousePressed = FALSE;
    d->pressedItem = 0;
    d->pressedSelected = 0;

    QDragObject *drag = dragObject();
    if ( !drag )
	return;

    if ( drag->drag() )
	if ( drag->target() != viewport() )
	    emit moved();
}

#endif

/*!
  Inserts an item in the grid of the iconview. You should never need
  to call this manually.
*/

void QIconView::insertInGrid( QIconViewItem *item )
{
    if ( !item )
	return;

    if ( d->reorderItemsWhenInsert ) {
	// #### make this efficient - but it's not too dramatic
	int y = d->spacing;

	item->dirty = FALSE;
	if ( item == d->firstItem ) {
	    makeRowLayout( item, y );
	    return;
	}

	QIconViewItem *begin = rowBegin( item );
	y = begin->y();
	while ( begin ) {
	    begin = makeRowLayout( begin, y );

	    if ( !begin || !begin->next )
		break;

	    begin = begin->next;
	}
	item->dirty = FALSE;
    } else {
	QRegion r( QRect( 0, 0, QMAX( contentsWidth(), visibleWidth() ),
			  QMAX( contentsHeight(), visibleHeight() ) ) );

	QIconViewItem *i = d->firstItem;
	int y = -1;
	for ( ; i; i = i->next ) {
	    r = r.subtract( i->rect() );
	    y = QMAX( y, i->y() + i->height() );
	}

	QArray<QRect> rects = r.rects();
	QArray<QRect>::Iterator it = rects.begin();
	bool foundPlace = FALSE;
	for ( ; it != rects.end(); ++it ) {
	    QRect rect = *it;
	    if ( rect.width() >= item->width() &&
		 rect.height() >= item->height() ) {
		int sx = 0, sy = 0;
		if ( rect.width() >= item->width() + d->spacing )
		    sx = d->spacing;
		if ( rect.height() >= item->height() + d->spacing )
		    sy = d->spacing;
		item->move( rect.x() + sx, rect.y() + sy );
		foundPlace = TRUE;
		break;
	    }
	}

	if ( !foundPlace )
	    item->move( d->spacing, y + d->spacing );

	resizeContents( QMAX( contentsWidth(), item->x() + item->width() ),
			QMAX( contentsHeight(), item->y() + item->height() ) );
	item->dirty = FALSE;
    }
}

/*!
  Emits signals, that indicate selection changes.
*/

void QIconView::emitSelectionChanged( QIconViewItem *i )
{
    emit selectionChanged();
    if ( d->selectionMode == Single )
	emit selectionChanged( i ? i : d->currentItem );
}

/*!
  \internal
*/

void QIconView::emitRenamed( QIconViewItem *item )
{
    if ( !item )
	return;

    emit itemRenamed( item, item->text() );
    emit itemRenamed( item );
}

/*!
  If a drag enters the iconview, shapes of the objects, which the drag
  contains are drawn, using \a pos as origin.
*/

void QIconView::drawDragShapes( const QPoint &pos )
{
#ifndef QT_NO_DRAGANDDROP
    if ( pos == QPoint( -1, -1 ) )
	return;

    if ( !d->drawDragShapes ) {
	d->drawDragShapes = TRUE;
	return;
    }

    if ( d->isIconDrag ) {
	QPainter p;
	p.begin( viewport() );
	p.translate( -contentsX(), -contentsY() );
	p.setRasterOp( NotROP );
	p.setPen( QPen( color0 ) );

	QValueList<QIconDragDataItem>::Iterator it = d->iconDragData.begin();
	for ( ; it != d->iconDragData.end(); ++it ) {
	    QRect ir = (*it).item.pixmapRect();
	    QRect tr = (*it).item.textRect();
	    tr.moveBy( pos.x(), pos.y() );
	    ir.moveBy( pos.x(), pos.y() );
	    if ( !ir.intersects( QRect( contentsX(), contentsY(), visibleWidth(), visibleHeight() ) ) )
		continue;
	    style().drawFocusRect( &p, ir, colorGroup(), &colorGroup().base() );
	    style().drawFocusRect( &p, tr, colorGroup(), &colorGroup().base() );
	}

	p.end();
    } else if ( d->numDragItems > 0 ) {
	QPainter p;
	p.begin( viewport() );
	p.setRasterOp( NotROP );
	p.setPen( QPen( color0 ) );

	for ( int i = 0; i < d->numDragItems; ++i ) {
	    QRect r( pos.x() + i * 40, pos.y(), 35, 35 );
	    style().drawFocusRect( &p, r, colorGroup(), &colorGroup().base() );
	}

	p.end();
    }
#endif
}

/*!
  When a drag enters the iconview, this method is called to
  initialize it. Initializing means here to get information about
  the drag, this means if the iconview knows enough about
  the drag to be able to draw drag shapes of the drag data
  (e.g. shapes of icons which are dragged), etc.
*/

#ifndef QT_NO_DRAGANDDROP
void QIconView::initDragEnter( QDropEvent *e )
{
    if ( QIconDrag::canDecode( e ) ) {
	QIconDragPrivate::decode( e, d->iconDragData );
	d->isIconDrag = TRUE;
    } else if ( QUriDrag::canDecode( e ) ) {
	QStrList lst;
	QUriDrag::decode( e, lst );
	d->numDragItems = lst.count();
    } else {
	d->numDragItems = 0;
    }

}
#endif

/*!
  This method is called to draw the rectangle \a r of the background using
  the painter \a p. xOffset and yOffset are known using the methods
  contentsX() and contentsY().

  The default implementation only fills \a r with colorGroup().base(). Subclasses
  may reimplement this to draw fency backgrounds.
*/

void QIconView::drawBackground( QPainter *p, const QRect &r )
{
    p->fillRect( r, QBrush( colorGroup().base() ) );
}

/*!
  \reimp
*/

bool QIconView::eventFilter( QObject * o, QEvent * e )
{
    if ( !o || !e )
	return FALSE;

    switch( e->type() ) {
    case QEvent::FocusIn:
	focusInEvent( (QFocusEvent*)e );
	return TRUE;
    case QEvent::FocusOut:
	focusOutEvent( (QFocusEvent*)e );
	return TRUE;
    case QEvent::Enter:
	enterEvent( e );
	return TRUE;
    case QEvent::Paint:
	if ( o == viewport() ) {
	    if ( d->dragging ) {
		if ( !d->rubber )
		    drawDragShapes( d->oldDragPos );
	    }
	    viewportPaintEvent( (QPaintEvent*)e );
	    if ( d->dragging ) {
		if ( !d->rubber )
		    drawDragShapes( d->oldDragPos );
	    }
	}
	return TRUE;
    default:
	// nothing
	break;
    }

    return QScrollView::eventFilter( o, e );
}


/*!
  \reimp
*/

QSize QIconView::minimumSizeHint() const
{
    return QSize( 100, 100 );
}

/*!
  \reimp
*/

QSizePolicy QIconView::sizePolicy() const
{
    //### removeme 3.0
    return QScrollView::sizePolicy();
}

/*!
  \internal
  Clears string which is used for setting the current item
  when the user types something in
*/

void QIconView::clearInputString()
{
    d->currInputString = QString::null;
}

/*!
  \internal
  Finds the first item beginning with \a text and makes
  it the current one
*/

void QIconView::findItemByName( const QString &text )
{
    if ( d->inputTimer->isActive() )
	d->inputTimer->stop();
    d->inputTimer->start( 500, TRUE );
    d->currInputString += text.lower();
    QIconViewItem *item = findItem( d->currInputString );
    if ( item ) {
	setCurrentItem( item );
	if ( d->selectionMode == Extended ) {
	    bool changed = FALSE;
	    bool block = signalsBlocked();
	    blockSignals( TRUE );
	    selectAll( FALSE );
	    blockSignals( block );
	    if ( !item->selected && item->isSelectable() ) {
		changed = TRUE;
		item->selected = TRUE;
		repaintItem( item );
	    }
	    if ( changed )
		emit selectionChanged();
	}
    }
}

/*!
  Lays out a row of icons (in Arrangement == TopToBottom this is a column). Starts
  laying out with the item \a begin. \a y is the starting coordinate.
  Returns the last item of the row and sets the new starting coordinate to \a y.

  This function may be made private in a future version of Qt. We
  recommend not calling it.
*/

QIconViewItem *QIconView::makeRowLayout( QIconViewItem *begin, int &y )
{
    QIconViewItem *end = 0;

    if ( d->arrangement == LeftToRight ) {

	if ( d->rastX == -1 ) {
	    // first calculate the row height
	    int h = 0;
	    int x = 0;
	    int ih = 0;
	    QIconViewItem *item = begin;
	    for (;;) {
		x += d->spacing + item->width();
		if ( x > visibleWidth() && item != begin ) {
		    h = QMAX( h, item->height() );
		    ih = QMAX( ih, item->pixmapRect().height() );
		    item = item->prev;
		    break;
		}
		h = QMAX( h, item->height() );
		ih = QMAX( ih, item->pixmapRect().height() );
		QIconViewItem *old = item;
		item = item->next;
		if ( !item ) {
		    item = old;
		    break;
		}
	    }
	    end = item;

	    if ( d->rastY != -1 )
		h = QMAX( h, d->rastY );

	    // now move the items
	    item = begin;
	    for (;;) {
		item->dirty = FALSE;
		if ( item == begin )
		    item->move( d->spacing, y + ih - item->pixmapRect().height() );
		else
		    item->move( item->prev->x() + item->prev->width() + d->spacing,
				y + ih - item->pixmapRect().height() );
		if ( y + h < item->y() + item->height() )
		    h = QMAX( h, ih + item->textRect().height() );
		if ( item == end )
		    break;
		item = item->next;
	    }
	    y += h + d->spacing;
	} else {
	    // first calculate the row height
	    int h = begin->height();
	    int x = d->spacing;
	    int ih = begin->pixmapRect().height();
	    QIconViewItem *item = begin;
	    int i = 0;
	    int sp = 0;
	    for (;;) {
		int r = calcGridNum( item->width(), d->rastX );
		if ( item == begin ) {
		    i += r;
		    sp += r;
		    x = d->spacing + d->rastX * r;
		} else {
		    sp += r;
		    i += r;
		    x = i * d->rastX + sp * d->spacing;
		}
		if ( x > visibleWidth() && item != begin ) {
		    h = QMAX( h, item->height() );
		    ih = QMAX( ih, item->pixmapRect().height() );
		    item = item->prev;
		    break;
		}
		h = QMAX( h, item->height() );
		ih = QMAX( ih, item->pixmapRect().height() );
		QIconViewItem *old = item;
		item = item->next;
		if ( !item ) {
		    item = old;
		    break;
		}
	    }
	    end = item;

	    if ( d->rastY != -1 )
		h = QMAX( h, d->rastY );

	    // now move the items
	    item = begin;
	    i = 0;
	    sp = 0;
	    for (;;) {
		item->dirty = FALSE;
		int r = calcGridNum( item->width(), d->rastX );
		if ( item == begin ) {
		    if ( d->itemTextPos == Bottom )
			item->move( d->spacing + ( r * d->rastX - item->width() ) / 2,
				    y + ih - item->pixmapRect().height() );
		    else
			item->move( d->spacing, y + ih - item->pixmapRect().height() );
		    i += r;
		    sp += r;
		} else {
		    sp += r;
		    int x = i * d->rastX + sp * d->spacing;
		    if ( d->itemTextPos == Bottom )
			item->move( x + ( r * d->rastX - item->width() ) / 2,
				    y + ih - item->pixmapRect().height() );
		    else
			item->move( x, y + ih - item->pixmapRect().height() );
		    i += r;
		}
		if ( y + h < item->y() + item->height() )
		    h = QMAX( h, ih + item->textRect().height() );
		if ( item == end )
		    break;
		item = item->next;
	    }
	    y += h + d->spacing;
	}


    } else { // -------------------------------- SOUTH ------------------------------

	int x = y;

	{
	    int w = 0;
	    int y = 0;
	    QIconViewItem *item = begin;
	    for (;;) {
		y += d->spacing + item->height();
		if ( y > visibleHeight() && item != begin ) {
		    item = item->prev;
		    break;
		}
		w = QMAX( w, item->width() );
		QIconViewItem *old = item;
		item = item->next;
		if ( !item ) {
		    item = old;
		    break;
		}
	    }
	    end = item;

	    if ( d->rastX != -1 )
		w = QMAX( w, d->rastX );

	    // now move the items
	    item = begin;
	    for (;;) {
		item->dirty = FALSE;
		if ( d->itemTextPos == Bottom ) {
		    if ( item == begin )
			item->move( x + ( w - item->width() ) / 2, d->spacing );
		    else
			item->move( x + ( w - item->width() ) / 2,
				    item->prev->y() + item->prev->height() + d->spacing );
		} else {
		    if ( item == begin )
			item->move( x, d->spacing );
		    else
			item->move( x, item->prev->y() + item->prev->height() + d->spacing );
		}
		if ( item == end )
		    break;
		item = item->next;
	    }
	    x += w + d->spacing;
	}

	y = x;
    }

    return end;
}

/*!
  \internal
  Calculates how many cells and item of the width \a w needs in a grid with of
  \a x and returns the result.
*/

int QIconView::calcGridNum( int w, int x ) const
{
    float r = (float)w / (float)x;
    if ( ( w / x ) * x != w )
	r += 1.0;
    return (int)r;
}

/*!
  \internal
  Returns the first item of the row which contains \a item.
*/

QIconViewItem *QIconView::rowBegin( QIconViewItem * ) const
{
    // #### todo
    return d->firstItem;
}

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

static int cmpIconViewItems( const void *n1, const void *n2 )
{
    if ( !n1 || !n2 )
	return 0;

    QIconViewPrivate::SortableItem *i1 = (QIconViewPrivate::SortableItem *)n1;
    QIconViewPrivate::SortableItem *i2 = (QIconViewPrivate::SortableItem *)n2;

    return i1->item->compare( i2->item );
}

#if defined(Q_C_CALLBACKS)
}
#endif

/*!
  Sorts the items of the listview and re-arranges them afterwards. If
  \a ascending is TRUE, the items are sorted in increasing order, else
  in decreasing order. For sorting QIconViewItem::compare() is used.
  The default sort direction is set to the sort direction you set
  here.

  \sa QIconViewItem::key(), QIconViewItem::setKey(), QIconViewItem::compare(),
  QIconView::setSorting(), QIconView::sortDirection()
*/

void QIconView::sort( bool ascending )
{
    if ( count() == 0 )
	return;

    d->sortDirection = ascending;
    QIconViewPrivate::SortableItem *items = new QIconViewPrivate::SortableItem[ count() ];

    QIconViewItem *item = d->firstItem;
    int i = 0;
    for ( ; item; item = item->next )
	items[ i++ ].item = item;

    qsort( items, count(), sizeof( QIconViewPrivate::SortableItem ), cmpIconViewItems );

    QIconViewItem *prev = 0;
    item = 0;
    if ( ascending ) {
	for ( i = 0; i < (int)count(); ++i ) {
	    item = items[ i ].item;
	    if ( item ) {
		item->prev = prev;
		if ( item->prev )
		    item->prev->next = item;
		item->next = 0;
	    }
	    if ( i == 0 )
		d->firstItem = item;
	    if ( i == (int)count() - 1 )
		d->lastItem = item;
	    prev = item;
	}
    } else {
	for ( i = (int)count() - 1; i >= 0 ; --i ) {
	    item = items[ i ].item;
	    if ( item ) {
		item->prev = prev;
		if ( item->prev )
		    item->prev->next = item;
		item->next = 0;
	    }
	    if ( i == (int)count() - 1 )
		d->firstItem = item;
	    if ( i == 0 )
		d->lastItem = item;
	    prev = item;
	}
    }

    delete [] items;

    arrangeItemsInGrid( TRUE );
}

/*!
  \reimp
*/

QSize QIconView::sizeHint() const
{
    constPolish();

    if ( !d->firstItem )
	return QSize( 50, 50 );

    if ( d->dirty && d->firstSizeHint ) {
	( (QIconView*)this )->resizeContents( QMAX( 400, contentsWidth() ),
					      QMAX( 400, contentsHeight() ) );
	( (QIconView*)this )->arrangeItemsInGrid( FALSE );
	d->firstSizeHint = FALSE;
    }

    d->dirty = TRUE;

    return QSize( QMIN( 400, contentsWidth() + style().scrollBarExtent().width()),
		  QMIN( 400, contentsHeight() + style().scrollBarExtent().height() ) );
}

/*!
  \internal
*/

void QIconView::updateContents()
{
    viewport()->update();
}

/*!
  \reimp
*/

void QIconView::enterEvent( QEvent *e )
{
    QScrollView::enterEvent( e );
    emit onViewport();
}

/*!
  \internal
  This method is always called when the geometry of an item changes. This method
  moves the item into the correct area in the internal data structure then.
*/

void QIconView::updateItemContainer( QIconViewItem *item )
{
    if ( !item || d->containerUpdateLocked || !isVisible() )
	return;

    if ( item->d->container1 && d->firstContainer ) {
	item->d->container1->items.removeRef( item );
    }
    item->d->container1 = 0;
    if ( item->d->container2 && d->firstContainer ) {
	item->d->container2->items.removeRef( item );
    }
    item->d->container2 = 0;

    QIconViewPrivate::ItemContainer *c = d->firstContainer;
    if ( !c ) {
	appendItemContainer();
	c = d->firstContainer;
    }

    bool contains;
    for (;;) {
	if ( c->rect.intersects( item->rect() ) ) {
	    contains = c->rect.contains( item->rect() );
	    break;
	}

	c = c->n;
	if ( !c ) {
	    appendItemContainer();
	    c = d->lastContainer;
	}
    }

    if ( !c ) {
#if defined(CHECK_RANGE)
	qWarning( "QIconViewItem::updateItemContainer(): No fitting container found!" );
#endif
	return;
    }

    c->items.append( item );
    item->d->container1 = c;

    if ( !contains ) {
	c = c->n;
	if ( !c ) {
	    appendItemContainer();
	    c = d->lastContainer;
	}
	c->items.append( item );
	item->d->container2 = c;
    }
}

/*!
  \internal
  Appends a new rect area to the internal data structure of the items
*/

void QIconView::appendItemContainer()
{
    QSize s;
    // #### We have to find out which value is best here
    if ( d->arrangement == LeftToRight )
	s = QSize( INT_MAX - 1, RECT_EXTENSION );
    else
	s = QSize( RECT_EXTENSION, INT_MAX - 1 );

    if ( !d->firstContainer ) {
	d->firstContainer = new QIconViewPrivate::ItemContainer( 0, 0, QRect( QPoint( 0, 0 ), s ) );
	d->lastContainer = d->firstContainer;
    } else {
	if ( d->arrangement == LeftToRight )
	    d->lastContainer = new QIconViewPrivate::ItemContainer(
		d->lastContainer, 0, QRect( d->lastContainer->rect.bottomLeft(), s ) );
	else
	    d->lastContainer = new QIconViewPrivate::ItemContainer(
		d->lastContainer, 0, QRect( d->lastContainer->rect.topRight(), s ) );
    }
}

/*!
  \internal
  Rebuilds the whole internal data structure. This is done when
  most certainly all items change their geometry (e.g. in arrangeItemsInGrid()), because
  calling this is then more efficient than calling updateItemContainer() for each
  item
*/

void QIconView::rebuildContainers()
{
    QIconViewPrivate::ItemContainer *c = d->firstContainer, *tmpc;
    while ( c ) {
	tmpc = c->n;
	delete c;
	c = tmpc;
    }
    d->firstContainer = d->lastContainer = 0;

    QIconViewItem *item = d->firstItem;
    appendItemContainer();
    c = d->lastContainer;
    while ( item ) {
	if ( c->rect.contains( item->rect() ) ) {
	    item->d->container1 = c;
	    item->d->container2 = 0;
	    c->items.append( item );
	    item = item->next;
	} else if ( c->rect.intersects( item->rect() ) ) {
	    item->d->container1 = c;
	    c->items.append( item );
	    c = c->n;
	    if ( !c ) {
		appendItemContainer();
		c = d->lastContainer;
	    }
	    c->items.append( item );
	    item->d->container2 = c;
	    item = item->next;
	    c = c->p;
	} else {
	    if ( d->arrangement == LeftToRight ) {
		if ( item->y() < c->rect.y() && c->p ) {
		    c = c->p;
		    continue;
		}
	    } else {
		if ( item->x() < c->rect.x() && c->p ) {
		    c = c->p;
		    continue;
		}
	    }

	    c = c->n;
	    if ( !c ) {
		appendItemContainer();
		c = d->lastContainer;
	    }
	}
    }
}

/*!
  \internal
*/

void QIconView::movedContents( int, int )
{
    if ( d->drawDragShapes ) {
	drawDragShapes( d->oldDragPos );
	d->oldDragPos = QPoint( -1, -1 );
    }
}

void QIconView::handleItemChange( QIconViewItem *old, bool shift, bool control )
{
    if ( d->selectionMode == Single ) {
	bool block = signalsBlocked();
	blockSignals( TRUE );
	if ( old )
	    old->setSelected( FALSE );
	blockSignals( block );
	d->currentItem->setSelected( TRUE, TRUE );
    } else if ( d->selectionMode == Extended ) {
	if ( control ) {
	    // nothing
	} else if ( shift ) {
	    if ( !d->selectAnchor ) {
		if ( old && !old->selected && old->isSelectable() ) {
		    old->selected = TRUE;
		    repaintItem( old );
		}
		d->currentItem->setSelected( TRUE, TRUE );
	    } else {
		QIconViewItem *from = d->selectAnchor, *to = d->currentItem;
		if ( !from || !to )
		    return;
		QIconViewItem *i = 0;
		int index =0;
		int f_idx = -1, t_idx = -1;
		for ( i = d->firstItem; i; i = i->next, index++ ) {
		    if ( i == from )
			f_idx = index;
		    if ( i == to )
			t_idx = index;
		    if ( f_idx != -1 && t_idx != -1 )
			break;
		}
		if ( f_idx > t_idx ) {
		    i = from;
		    from = to;
		    to = i;
		}

		bool changed = FALSE;
		for ( i = d->firstItem; i && i != from; i = i->next ) {
			if ( i->selected ) {
			    i->selected = FALSE;
			    changed = TRUE;
			    repaintItem( i );
			}
		    }
		for ( i = to->next; i; i = i->next ) {
		    if ( i->selected ) {
			i->selected = FALSE;
			changed = TRUE;
			repaintItem( i );
		    }
		}

		for ( i = from; i; i = i->next ) {
		    if ( !i->selected && i->isSelectable() ) {
			i->selected = TRUE;
			changed = TRUE;
			repaintItem( i );
		    }
		    if ( i == to )
			break;
		}
		if ( changed )
		    emit selectionChanged();
	    }
	} else {
	    blockSignals( TRUE );
	    selectAll( FALSE );
	    blockSignals( FALSE );
	    d->currentItem->setSelected( TRUE, TRUE );
	}
    } else {
	if ( shift )
	    d->currentItem->setSelected( !d->currentItem->isSelected(), TRUE );
    }
}

QBitmap QIconView::mask( QPixmap *pix ) const
{
    QBitmap m;
    if ( d->maskCache.find( QString::number( pix->serialNumber() ), m ) )
	return m;
    m = pix->createHeuristicMask();
    d->maskCache.insert( QString::number( pix->serialNumber() ), m );
    return m;
}

/*!
  \reimp

  (Implemented to get rid of a compiler warning.)
*/
void QIconView::drawContents( QPainter * )
{
}

#include "qiconview.moc"

#endif // QT_NO_ICONVIEW
