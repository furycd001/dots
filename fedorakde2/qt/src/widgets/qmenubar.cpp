/****************************************************************************
** $Id: qt/src/widgets/qmenubar.cpp   2.3.2   edited 2001-06-29 $
**
** Implementation of QMenuBar class
**
** Created : 941209
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the widgets module of the Qt GUI Toolkit.
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

// qmainwindow.h before qmenubar.h because of GCC-2.7.* compatibility
// ### could be reorganised by discarding INCLUDE_MENUITEM_DEF and put
// the relevant declarations in a private header?
#include "qmainwindow.h"
#ifndef QT_NO_MENUBAR
#define	 INCLUDE_MENUITEM_DEF
#include "qmenubar.h"
#include "qaccel.h"
#include "qpainter.h"
#include "qdrawutil.h"
#include "qapplication.h"
#include "qguardedptr.h"
#include "qlayout.h"
#include <ctype.h>

class QMenuDataData {
    // attention: also defined in qmenudata.cpp
public:
    QMenuDataData();
    QGuardedPtr<QWidget> aWidget;
    int aInt;
};


// NOT REVISED
/*!
  \class QMenuBar qmenubar.h
  \brief The QMenuBar class provides a horizontal menu bar.

  \ingroup application

  A menu bar consists of a list of submenu items, so-called pulldown
  menus.  You add submenu items with insertItem(). Assuming that \c
  menubar is a pointer to a QMenuBar and \c filemenu a pointer to a
  QPopupMenu, \code
  menubar->insertItem( "&File", filemenu );
  \endcode
  inserts the menu into the menu bar. The ampersand in the item text declares
  Alt-f as shortcut for this menu. Use "&&" to get a real ampsend in the menubar.

  Items are either enabled or disabled. You toggle their state with
  setItemEnabled().

  Note that there is no need to layout a menu bar. It automatically
  sets its own geometry to the top of the parent widget and changes it
  appropriately whenever the parent is resized.

  \important insertItem removeItem clear insertSeparator setItemEnabled isItemEnabled

  menu/menu.cpp is a typical example of QMenuBar and QPopupMenu use.

  <img src=qmenubar-m.png> <img src=qmenubar-w.png>

  \sa QPopupMenu
  <a href="guibooks.html#fowler">GUI Design Handbook: Menu Bar</a>
*/


/*! \enum QMenuBar::Separator

  This enum type is used to decide whether QMenuBar should draw a
  separator line at its bottom.  The possible values are: <ul>

  <li> \c Never - in many applications, there already is a separator,
  and two looks stupid.

  <li> \c InWindowsStyle - in some other applications, a separator
  looks good in Windows style but not else.

  </ul>
 */

/*!
  \fn void QMenuBar::activated( int id )

  This signal is emitted when a menu item is selected; \a id is the id
  of the selected item.

  Normally, you will connect each menu item to a single slot using
  QMenuData::insertItem(), but sometimes you will want to connect
  several items to a single slot (most often if the user selects from
  an array).  This signal is handy in such cases.

  \sa highlighted(), QMenuData::insertItem()
*/

/*!
  \fn void QMenuBar::highlighted( int id )

  This signal is emitted when a menu item is highlighted; \a id is the
  id of the highlighted item.

  Normally, you will connect each menu item to a single slot using
  QMenuData::insertItem(), but sometimes you will want to connect
  several items to a single slot (most often if the user selects from
  an array).  This signal is handy in such cases.

  \sa activated(), QMenuData::insertItem()
*/


// Motif style parameters

static const int motifBarHMargin	= 2;	// menu bar hor margin to item
static const int motifBarVMargin	= 1;	// menu bar ver margin to item
static const int motifItemFrame		= 2;	// menu item frame width
static const int motifItemHMargin	= 5;	// menu item hor text margin
static const int motifItemVMargin	= 4;	// menu item ver text margin

/*

+-----------------------------
|      BarFrame
|   +-------------------------
|   |	   V  BarMargin
|   |	+---------------------
|   | H |      ItemFrame
|   |	|  +-----------------
|   |	|  |			   \
|   |	|  |  ^	 T E X T   ^	    | ItemVMargin
|   |	|  |  |		   |	   /
|   |	|      ItemHMargin
|   |
|

*/


/*****************************************************************************
  QMenuBar member functions
 *****************************************************************************/


/*!
  Constructs a menu bar with a \e parent and a \e name.
*/

QMenuBar::QMenuBar( QWidget *parent, const char *name )
    : QFrame( parent, name, 0, FALSE )
{
    isMenuBar = TRUE;
#ifndef QT_NO_ACCEL
    autoaccel = 0;
#endif
    irects    = 0;
    rightSide = 0; // Right of here is rigth-aligned content
    mseparator = 0;
    waitforalt = 0;
    popupvisible = 0;
    hasmouse = 0;
    defaultup = 0;
    toggleclose = 0;
    if ( parent ) {
	// filter parent events for resizing
	parent->installEventFilter( this );

	// filter top-level-widget events for accelerators
	QWidget *tlw = topLevelWidget();
	if ( tlw != parent )
	    tlw->installEventFilter( this );
    }

    QFontMetrics fm = fontMetrics();
    int gs = style();
    int h;
    if ( gs == WindowsStyle ) {
	h = 2 + fm.height() + motifItemVMargin + 2*style().defaultFrameWidth();
    } else {
	h =  style().defaultFrameWidth() + motifBarVMargin + fm.height()
	    + motifItemVMargin + 2*style().defaultFrameWidth() + 2*motifItemFrame;
    }

    move( 0, 0 );
    resize( width(), h );

    switch ( gs ) {
	case WindowsStyle:
	    setFrameStyle( QFrame::NoFrame );
	    setMouseTracking( TRUE );
	    break;
	case MotifStyle:
	    setFrameStyle( QFrame::Panel | QFrame::Raised );
	    setLineWidth( style().defaultFrameWidth() );
	    break;
	default:
	    break;
    }
    setBackgroundMode( PaletteButton );
}



/*! \reimp */

void QMenuBar::styleChange( QStyle& old )
{
    switch ( style().guiStyle() ) {
	case WindowsStyle:
	    setFrameStyle( QFrame::NoFrame );
	    setMouseTracking( TRUE );
	    break;
	case MotifStyle:
	    setFrameStyle( QFrame::Panel | QFrame::Raised );
	    setLineWidth( style().defaultFrameWidth() );
	    setMouseTracking( FALSE );
	    break;
	default:
	    break;
    }
    updateGeometry();
    QFrame::styleChange( old );
}



/*!
  Destroys the menu bar.
*/

QMenuBar::~QMenuBar()
{
#ifndef QT_NO_ACCEL
    delete autoaccel;
#endif
    if ( irects )		// Avoid purify complaint.
	delete [] irects;
}

/*!
  \internal
  Needs documentation.
*/
void QMenuBar::updateItem( int id )
{
    int i = indexOf( id );
    if ( i >= 0 && irects )
	repaint( irects[i], FALSE );
}


/*!
  Recomputes the menu bar's display data according to the new
  contents.

  You should never need to call this, it is called automatically by
  QMenuData whenever it needs to be called.
*/

void QMenuBar::menuContentsChanged()
{
#ifndef QT_NO_ACCEL
    setupAccelerators();
#endif
    badSize = TRUE;				// might change the size
    if ( isVisible() ) {
	calculateRects();
	update();
#ifndef QT_NO_MAINWINDOW
	if ( parent() && parent()->inherits( "QMainWindow" ) ) {
	    ( (QMainWindow*)parent() )->triggerLayout();
	    ( (QMainWindow*)parent() )->update();
	}
#endif
#ifndef QT_NO_LAYOUT	
	if ( parentWidget() && parentWidget()->layout() )
	    parentWidget()->layout()->activate();
#endif
    }
}

/*!
  Recomputes the menu bar's display data according to the new
  state.

  You should never need to call this, it is called automatically by
  QMenuData whenever it needs to be called.
*/

void QMenuBar::menuStateChanged()
{
#ifndef QT_NO_ACCEL
    setupAccelerators(); // ### when we have a good solution for the accel vs. focus widget problem, remove that. That is only a workaround
#endif
    update();
}


void QMenuBar::menuInsPopup( QPopupMenu *popup )
{
    popup->parentMenu = this;			// set parent menu
    connect( popup, SIGNAL(activatedRedirect(int)),
	     SLOT(subActivated(int)) );
    connect( popup, SIGNAL(highlightedRedirect(int)),
	     SLOT(subHighlighted(int)) );
}

void QMenuBar::menuDelPopup( QPopupMenu *popup )
{
    popup->parentMenu = 0;
    popup->disconnect( SIGNAL(activatedRedirect(int)), this,
		       SLOT(subActivated(int)) );
    popup->disconnect( SIGNAL(highlightedRedirect(int)), this,
		       SLOT(subHighlighted(int)) );
}

void QMenuBar::frameChanged()
{
    menuContentsChanged();
}


/*!
  This function is used to adjust the menu bar's geometry to the
  parent widget's.  Note that this is \e not part of the public
  interface - the function is \c public only because
  QObject::eventFilter() is.

  \internal
  Resizes the menu bar to fit in the parent widget when the parent receives
  a resize event.
*/

bool QMenuBar::eventFilter( QObject *object, QEvent *event )
{
    if ( object == parent() && object && !object->inherits( "QToolBar" ) &&
	 event->type() == QEvent::Resize ) {
	QResizeEvent *e = (QResizeEvent *)event;
	int w = e->size().width();
	setGeometry( 0, y(), w, heightForWidth(w) );
	return FALSE;
    }

    if ( style() != WindowsStyle ||
	 !isVisible() ||
	 !object->isWidgetType() ||
	 !( event->type() == QEvent::Accel ||
	    event->type() == QEvent::KeyPress ||
	    event->type() == QEvent::KeyRelease ) )
	return FALSE;

#ifndef QT_NO_ACCEL
    // look for Alt press and Alt-anything press
    if ( event->type() == QEvent::Accel ) {
	QWidget * f = ((QWidget *)object)->focusWidget();
	QKeyEvent * ke = (QKeyEvent *) event;
	if ( f ) { // ### this thinks alt and meta are the same
	    if ( ke->key() == Key_Alt || ke->key() == Key_Meta ) {
		if ( waitforalt ) {
		    waitforalt = 0;
		    if ( object->parent() )
			object->removeEventFilter( this );
 		    ke->accept();
 		    return TRUE;
		} else if ( hasFocus() ) {
 		    setAltMode( FALSE );
 		    ke->accept();
 		    return TRUE;
		} else {
 		    waitforalt = 1;
 		    if ( f != object )
 			f->installEventFilter( this );
		}
	    } else if ( ke->key() == Key_Control || ke->key() == Key_Shift)
		setAltMode( FALSE );
	}
	// ### ! block all accelerator events when the menu bar is active
	if ( qApp && qApp->focusWidget() == this ) {
	    return TRUE;
	}

	return FALSE;
    }
#endif
    // look for Alt release
    if ( ((QWidget*)object)->focusWidget() == object ||
	 (object->parent() == 0 && ((QWidget*)object)->focusWidget() == 0) ) {
	if ( waitforalt &&
	     event->type() == QEvent::KeyRelease &&
	     (((QKeyEvent *)event)->key() == Key_Alt ||
	      ((QKeyEvent *)event)->key() == Key_Meta) ) {
	    setAltMode( TRUE );
	    if ( object->parent() )
		object->removeEventFilter( this );
	    QWidget * tlw = ((QWidget *)object)->topLevelWidget();
	    if ( tlw ) {
		// ### !
		// make sure to be the first event filter, so we can kill
		// accelerator events before the accelerators get to them.
		tlw->removeEventFilter( this );
		tlw->installEventFilter( this );
	    }
	    return TRUE;
	} else if ( (event->type() == QEvent::KeyPress ||
		     event->type() == QEvent::KeyRelease) &&
		    !(((QKeyEvent *)event)->key() == Key_Alt ||
		      ((QKeyEvent *)event)->key() == Key_Meta) ) {
	    if ( object->parent() )
		object->removeEventFilter( this );
	    setAltMode( FALSE );
	}
    }


    return FALSE;				// don't stop event
}



/*!
  \internal
  Receives signals from menu items.
*/

void QMenuBar::subActivated( int id )
{
    emit activated( id );
}

/*!
  \internal
  Receives signals from menu items.
*/

void QMenuBar::subHighlighted( int id )
{
    emit highlighted( id );
}

/*!
  \internal
  Receives signals from menu accelerator.
*/
#ifndef QT_NO_ACCEL
void QMenuBar::accelActivated( int id )
{
    if ( !isEnabled() )				// the menu bar is disabled
	return;
    setActiveItem( indexOf( id ) );
}
#endif

/*!
  \internal
  This slot receives signals from menu accelerator when it is about to be
  destroyed.
*/
#ifndef QT_NO_ACCEL
void QMenuBar::accelDestroyed()
{
    autoaccel = 0;				// don't delete it twice!
}
#endif

bool QMenuBar::tryMouseEvent( QPopupMenu *, QMouseEvent *e )
{
    QPoint pos = mapFromGlobal( e->globalPos() );
    if ( !rect().contains( pos ) )		// outside
	return FALSE;
    int item = itemAtPos( pos );
    if ( item == -1 && (e->type() == QEvent::MouseButtonPress ||
			e->type() == QEvent::MouseButtonRelease) ) {
	hidePopups();
	goodbye();
	return FALSE;
    }
    QMouseEvent ee( e->type(), pos, e->globalPos(), e->button(), e->state() );
    event( &ee );
    return TRUE;
}


void QMenuBar::tryKeyEvent( QPopupMenu *, QKeyEvent *e )
{
    event( e );
}


void QMenuBar::goodbye( bool cancelled )
{
    mouseBtDn = FALSE;
    popupvisible = 0;
    if ( cancelled && style() == WindowsStyle )
	setAltMode( TRUE );
    else
	setAltMode( FALSE );
}


void QMenuBar::openActPopup()
{
    if ( actItem < 0 )
	return;
//     setWindowsAltMode( FALSE, actItem );
    QPopupMenu *popup = mitems->at(actItem)->popup();
    if ( !popup || !popup->isEnabled() )
	return;

    QRect  r = itemRect( actItem );
    QPoint pos = r.bottomLeft() + QPoint(0,1);
    int ph = popup->sizeHint().height();
    pos = mapToGlobal(pos);
    int sh = QApplication::desktop()->height();
    if ( defaultup || (pos.y() + ph > sh) ) {
	QPoint t = mapToGlobal( r.topLeft() );
	t.ry() -= (QCOORD)ph;
	if ( !defaultup || t.y() >= 0 )
	    pos = t;
    }

    //avoid circularity
    if ( popup->isVisible() )
	return;

    if (popup->parentMenu != this ){
	// reuse
	if (popup->parentMenu)
	    popup->parentMenu->menuDelPopup(popup);
	menuInsPopup(popup);
    }

    popup->snapToMouse = FALSE;
    popup->popup( pos );
    popup->snapToMouse = TRUE;
}

/*!
  \internal
  Hides all popup menu items.
*/

void QMenuBar::hidePopups()
{
    QMenuItemListIt it(*mitems);
    register QMenuItem *mi;
    while ( (mi=it.current()) ) {
	++it;
	if ( mi->popup() ) {
	    mi->popup()->hide();
	}
    }
}


/*!
  Reimplements QWidget::show() in order to set up the correct keyboard
  accelerators and raise itself to the top of the widget stack.
*/

void QMenuBar::show()
{
#ifndef QT_NO_ACCEL
    setupAccelerators();
#endif
    if ( parentWidget() )
		resize( parentWidget()->width(), height() );
    calculateRects();
    QWidget::show();
#ifndef QT_NO_MAINWINDOW
    if ( parent() && parent()->inherits( "QMainWindow" ) ) //### ugly workaround
		( (QMainWindow*)parent() )->triggerLayout();
#endif
    raise();
}

/*!
  Reimplements QWidget::hide() in order to deselect any selected item and
  calls setUpLayout() for the mainwindow.
*/

void QMenuBar::hide()
{
    QWidget::hide();
    setAltMode( FALSE );
    hidePopups();
#ifndef QT_NO_MAINWINDOW
    if ( parent() && parent()->inherits( "QMainWindow" ) ) //### ugly workaround
		( (QMainWindow*)parent() )->triggerLayout();
#endif
}

/*!
  \internal
  Needs to change the size of the menu bar when a new font is set.
*/

void QMenuBar::fontChange( const QFont & f )
{
    badSize = TRUE;
    updateGeometry();
    if ( isVisible() )
	calculateRects();
    QWidget::fontChange( f );
}


/*****************************************************************************
  Item geometry functions
 *****************************************************************************/

/*!
  This function serves two different purposes.  If the parameter is negative,
  it updates the irects member for the current width and resizes.  Otherwise,
  it does the same calculations for the GIVEN width, and returns the height
  to which it WOULD have resized.  A bit tricky, but both operations require
  almost identical steps.
*/
int QMenuBar::calculateRects( int max_width )
{
    polish();
    bool update = ( max_width < 0 );

    if ( update ) {
	rightSide = 0;
	if ( !badSize )				// size was not changed
	    return 0;
	if ( irects )				// Avoid purify complaint.
	    delete [] irects;
	if ( mitems->isEmpty() ) {
	    irects = 0;
	    return 0;
	}
	int i = mitems->count();		// workaround for gcc 2.95.2
	irects = new QRect[ i ];		// create rectangle array
	CHECK_PTR( irects );
	max_width = width();
    }
    QFontMetrics fm = fontMetrics();
    int max_height = 0;
    int max_item_height = 0;
    int nlitems = 0;				// number on items on cur line
    int gs = style();
    int x = style().defaultFrameWidth();
    int y = style().defaultFrameWidth();
    if( gs == MotifStyle ) {
	x += motifBarHMargin;
	y += motifBarVMargin;
    }
    int i = 0;
    int separator = -1;
    while ( i < (int)mitems->count() ) {	// for each menu item...
	QMenuItem *mi = mitems->at(i);
	int w=0, h=0;
	if ( mi->widget() ) {
	    if ( mi->widget()->parentWidget() != this ) {
		mi->widget()->reparent( this, QPoint(0,0), TRUE );
	    }
	    w = mi->widget()->sizeHint().expandedTo( QApplication::globalStrut() ).width()+2;
	    h = mi->widget()->sizeHint().expandedTo( QApplication::globalStrut() ).height()+2;
	    if ( i && separator < 0 )
		separator = i;
	} else if ( mi->pixmap() ) {			// pixmap item
	    w = QMAX( mi->pixmap()->width() + 4, QApplication::globalStrut().width() );
	    h = QMAX( mi->pixmap()->height() + 4, QApplication::globalStrut().height() );
	} else if ( !mi->text().isNull() ) {	// text item
	    QString s = mi->text();
	    w = fm.boundingRect( s ).width()
		+ 2*motifItemHMargin;
	    w -= s.contains('&')*fm.width('&');
	    w += s.contains("&&")*fm.width('&');
	    w = QMAX( w, QApplication::globalStrut().width() );
	    h = QMAX( fm.height() + motifItemVMargin, QApplication::globalStrut().height() );
	} else if ( mi->isSeparator() ) {	// separator item
	    if ( style() == MotifStyle )
		separator = i; //### only motif?
	}
	if ( !mi->isSeparator() || mi->widget() ) {
	    if ( gs == MotifStyle ) {
		w += 2*motifItemFrame;
		h += 2*motifItemFrame;
	    }
	    if ( x + w + style().defaultFrameWidth() - max_width > 0 && nlitems > 0 ) {
		nlitems = 0;
		x = style().defaultFrameWidth();
		y += h;
		if( gs == MotifStyle ) {
		    x += motifBarHMargin;
		    y += motifBarVMargin;
		}
		separator = -1;
	    }
	    if ( y + h + 2*style().defaultFrameWidth() > max_height )
		max_height = y + h + 2*style().defaultFrameWidth();
	    if ( h > max_item_height )
		max_item_height = h;
	}
	if ( update ) {
	    irects[i].setRect( x, y, w, h );
	}
	x += w;
	nlitems++;
	i++;
    }
    if ( update ) {
	if ( separator >= 0 ) {
	    int moveBy = max_width - x;
	    rightSide = x;
	    while( --i >= separator ) {
		irects[i].moveBy( moveBy, 0 );
	    }
	} else {
	    rightSide = width()-frameWidth();
	}
	if ( max_height != height() )
	    resize( max_width, max_height );
 	for ( i = 0; i < (int)mitems->count(); i++ ) {
 	    irects[i].setHeight( max_item_height  );
	    QMenuItem *mi = mitems->at(i);
	    if ( mi->widget() ) {
		QRect r ( QPoint(0,0), mi->widget()->sizeHint() );
		r.moveCenter( irects[i].center() );
		mi->widget()->setGeometry( r );
	    }
	}
	badSize = FALSE;
    }

    return max_height;
}

/*!
  Returns the height that the menu would resize itself to if its parent
  (and hence itself) resized to the given width.  This can be useful for
  simple layout tasks where the height of the menubar is needed after
  items have been inserted.  See examples/showimg/showimg.cpp for an
  example of the usage.
*/
int QMenuBar::heightForWidth(int max_width) const
{
    // Okay to cast away const, as we are not updating.
    if ( max_width < 0 ) max_width = 0;
    return ((QMenuBar*)this)->calculateRects( max_width );
}

/*!
  \internal
  Return the bounding rectangle for the menu item at position \e index.
*/

QRect QMenuBar::itemRect( int index )
{
    calculateRects();
    return irects ? irects[index] : QRect(0,0,0,0);
}

/*!
  \internal
  Return the item at \e pos, or -1 if there is no item there, or if
  it is a separator item.
*/

int QMenuBar::itemAtPos( const QPoint &pos )
{
    calculateRects();
    if ( !irects )
	return -1;
    int i = 0;
    while ( i < (int)mitems->count() ) {
	if ( irects[i].contains( pos ) ) {
	    QMenuItem *mi = mitems->at(i);
	    return mi->isSeparator() ? -1 : i;
	}
	++i;
    }
    return -1;					// no match
}


/*!
  When a menubar is used above an unframed widget, it may look better
  with a separating line when displayed with \link QWidget::style()
  WindowsStyle\endlink.

  This function sets the usage of such a separator to appear either
  QMenuBar::Never, or QMenuBar::InWindowsStyle.

  The default is QMenuBar::Never.

  \sa Separator separator()
*/
void QMenuBar::setSeparator( Separator when )
{
    mseparator = when;
}

/*!
  Returns the currently set Separator usage.

  \sa Separator setSeparator()
*/
QMenuBar::Separator QMenuBar::separator() const
{
    return mseparator ? InWindowsStyle : Never;
}

/*****************************************************************************
  Event handlers
 *****************************************************************************/


/*!
  Called from QFrame::paintEvent().
*/

void QMenuBar::drawContents( QPainter *p )
{
    QColorGroup g;
    GUIStyle gs = style().guiStyle();
    bool e;

    for ( int i=0; i<(int)mitems->count(); i++ ) {
	QMenuItem *mi = mitems->at( i );
	if ( !mi->text().isNull() || mi->pixmap() ) {
	    QRect r = irects[i];
	    e = mi->isEnabled();
	    if ( e )
		g = colorGroup();
	    else
		g = palette().disabled();

	    if ( gs == WindowsStyle || style().defaultFrameWidth() < 2) {
		p->fillRect( r,palette().normal().brush( QColorGroup::Button ) );
		if ( i == actItem && ( hasFocus() || hasmouse || popupvisible ) ) {
		    QBrush b = palette().normal().brush( QColorGroup::Button );
		    if ( actItemDown )
			p->setBrushOrigin(p->brushOrigin() + QPoint(1,1));
		    qDrawShadeRect( p,
				    r.left(), r.top(), r.width(), r.height(),
				    g, actItemDown, 1, 0, &b );
		    if ( actItemDown ) {
			r.setRect( r.left()+2, r.top()+2,
				   r.width()-2, r.height()-2 );
			p->setBrushOrigin(p->brushOrigin() - QPoint(1,1));
		    }
		}
	    } else { // motif
		if ( i == actItem && actItemDown ) // active item
		    qDrawShadePanel( p, r, palette().normal(), FALSE,
				     motifItemFrame,
			    &palette().normal().brush( QColorGroup::Button ));
		else // other item
		    p->fillRect(r, palette().normal().brush( QColorGroup::Button ));
	    }
	    style().drawMenuBarItem( p, r.left(), r.top(), r.width(), r.height(),
			    mi, g, e, (i == actItem && actItemDown) );
	}
    }
    if ( mseparator == InWindowsStyle && gs == WindowsStyle ) {
	p->setPen( g.light() );
	p->drawLine( 0, height()-1, width()-1, height()-1 );
	p->setPen( g.dark() );
	p->drawLine( 0, height()-2, width()-1, height()-2 );
    }
}


/*!\reimp
*/
void QMenuBar::mousePressEvent( QMouseEvent *e )
{
    if ( e->button() != LeftButton )
	return;
    mouseBtDn = TRUE;				// mouse button down
    int item = itemAtPos( e->pos() );
    if ( item == actItem && popupvisible )
	toggleclose = 1;
    setActiveItem( item, TRUE, FALSE );
}


/*!\reimp
*/
void QMenuBar::mouseReleaseEvent( QMouseEvent *e )
{
    if ( e->button() != LeftButton )
	return;
    if ( !mouseBtDn )
	return;
    mouseBtDn = FALSE;				// mouse button up
    int item = itemAtPos( e->pos() );
    if ( item >= 0 && !mitems->at(item)->isEnabled() ||
	 actItem >= 0 && !mitems->at(actItem)->isEnabled() ) {
	hidePopups();
	return;
    }
    bool showMenu = TRUE;
    if ( toggleclose && style() == WindowsStyle && actItem == item )
	showMenu = FALSE;
    setActiveItem( item, showMenu, !hasMouseTracking() );
    toggleclose = 0;
}


/*!\reimp
*/
void QMenuBar::mouseMoveEvent( QMouseEvent *e )
{
    int item = itemAtPos( e->pos() );
    if ( !mouseBtDn && !popupvisible) {
	if ( item >= 0 ) {
	    if ( !hasmouse ) {
		hasmouse = 1;
		if ( actItem == item )
		    actItem = -1; // trigger update
	    }
	}
	setActiveItem( item, FALSE, FALSE );
	return;
    }
    if ( item != actItem && item >= 0  && ( popupvisible || mouseBtDn ) )
	setActiveItem( item, TRUE, FALSE );
}


/*!\reimp
*/
void QMenuBar::leaveEvent( QEvent * e )
{
    hasmouse = 0;
    int actId = idAt( actItem );
    if ( !hasFocus() && !popupvisible )
	actItem = -1;
    updateItem( actId );
    QFrame::leaveEvent( e );
}


/*!\reimp
*/
void QMenuBar::keyPressEvent( QKeyEvent *e )
{
    if ( actItem < 0 )
	return;

    QMenuItem  *mi = 0;
    int dx = 0;

    switch ( e->key() ) {
    case Key_Left:
	dx = -1;
	break;

    case Key_Right:
    case Key_Tab:
	dx = 1;
	break;

    case Key_Up:
    case Key_Down:
    case Key_Enter:
    case Key_Return:
	if ( style() == WindowsStyle )
	    setActiveItem( actItem );
	break;

    case Key_Escape:
 	setAltMode( FALSE );
	break;
    }

    if ( dx ) {					// highlight next/prev
	register int i = actItem;
	int c = mitems->count();
	int n = c;
	while ( n-- ) {
	    i = i + dx;
	    if ( i == c )
		i = 0;
	    else if ( i < 0 )
		i = c - 1;
	    mi = mitems->at( i );
	    // ### fix windows-style traversal - currently broken due to
	    // QMenuBar's reliance on QPopupMenu
	    if ( /* (style() == WindowsStyle || */ mi->isEnabled() /* ) */
		 && !mi->isSeparator() )
		break;
	}
	setActiveItem( i, popupvisible   );
    } else if ( !e->state() && e->text().length()==1 ) {
	QChar c = e->text()[0].upper();

	QMenuItemListIt it(*mitems);
	register QMenuItem *m;
	int indx = 0;
	while ( (m=it.current()) ) {
	    ++it;
	    QString s = m->text();
	    if ( !s.isEmpty() ) {
		int i = s.find( '&' );
		if ( i >= 0 )
		{
		    if ( s[i+1].upper() == c ) {
			mi = m;
			break;
		    }
		}
	    }
	    indx++;
	}
	if ( mi ) {
	    setActiveItem( indx );
	}
    }
}


/*!\reimp
*/
void QMenuBar::resizeEvent( QResizeEvent * )
{
    QRect fr = frameRect();
    if ( !fr.isNull() ) {
	QRect r( fr.x(), fr.y(), width(), height() );
	setFrameRect( r );
    }

    if ( badSize )
	return;
    badSize = TRUE;
    if ( isVisible() )
	calculateRects();
}


/*!
  Wins the price for the most stupid virtual function in Qt.
  Second price went to setWindowsAltMode()
  \sa setWindowsAltMode
 */
void QMenuBar::setActItem( int , bool )
{

}

/*!  Sets actItem to \a i and calls repaint for the
  changed things.

  Takes care to optimize the repainting.  Assumes that
  calculateRects() has been called as appropriate.
*/

void QMenuBar::setActiveItem( int i, bool show, bool activate_first_item )
{
    if ( i == actItem && (uint)show == popupvisible )
	return;

    QMenuItem* mi = 0;
    if ( i >= 0 )
	mi = mitems->at( i );
    if ( mi && !mi->isEnabled() )
	return;

    popupvisible = i >= 0 ? (show) : 0;
    actItemDown = popupvisible;

    if ( i < 0 || actItem < 0 ) {
	// just one item needs repainting
	int n = QMAX( actItem, i );
	actItem = i;
	if ( irects && n >= 0 )
	    repaint( irects[n], FALSE );
    } else if ( QABS(i-actItem) == 1 ) {
	// two neighbouring items need repainting
	int o = actItem;
	actItem = i;
	if ( irects )
	    repaint( irects[i].unite( irects[o] ), FALSE );
    } else {
	// two non-neighbouring items need repainting
	int o = actItem;
	actItem = i;
	if ( irects ) {
	    repaint( irects[o], FALSE );
	    repaint( irects[i], FALSE );
	}
    }

    hidePopups();

    if ( actItem < 0 || !popupvisible || !mi  )
	return;

    QPopupMenu *popup = mi->popup();
    if ( popup ) {
	emit highlighted( mi->id() );
	openActPopup();
	if ( activate_first_item )
	    popup->setFirstItemActive();
    } else {				// not a popup
	goodbye( FALSE );
	if ( mi->signal() )			// activate signal
	    mi->signal()->activate();
	emit activated( mi->id() );
    }
}


void QMenuBar::setAltMode( bool enable )
{
    waitforalt = 0;
    actItemDown = FALSE;
    if ( enable ) {
	if ( !QMenuData::d->aWidget )
	    QMenuData::d->aWidget = qApp->focusWidget();
	setFocus();
	updateItem( idAt( actItem ) );
    } else {
	if ( QMenuData::d->aWidget )
	    QMenuData::d->aWidget->setFocus();
	int actId = idAt( actItem );
	actItem = -1;
	updateItem( actId );
	QMenuData::d->aWidget = 0;
    }
}


/*!
  Wins the second price for the most stupid virtual function in Qt.
  First price went to setActItem()
  \sa setActItem
*/

void QMenuBar::setWindowsAltMode( bool, int )
{
}


/*!  Sets up keyboard accelerators for the menu bar. */
#ifndef QT_NO_ACCEL

void QMenuBar::setupAccelerators()
{
    delete autoaccel;
    autoaccel = 0;

    QMenuItemListIt it(*mitems);
    register QMenuItem *mi;
    while ( (mi=it.current()) ) {
	++it;
	if ( !mi->isEnabled() ) // ### when we have a good solution for the accel vs. focus widget problem, remove that. That is only a workaround
	    continue;
	QString s = mi->text();
	if ( !s.isEmpty() ) {
	    int i = QAccel::shortcutKey( s );
	    if ( i ) {
		if ( !autoaccel ) {
		    autoaccel = new QAccel( this );
		    CHECK_PTR( autoaccel );
		    autoaccel->setIgnoreWhatsThis( TRUE );
		    connect( autoaccel, SIGNAL(activated(int)),
			     SLOT(accelActivated(int)) );
		    connect( autoaccel, SIGNAL(destroyed()),
			     SLOT(accelDestroyed()) );
		}
		autoaccel->insertItem( i, mi->id() );
	    }
	}
	if ( mi->popup() ) {
	    // reuse
	    QPopupMenu* popup = mi->popup();
	    if (popup->parentMenu)
		popup->parentMenu->menuDelPopup(popup);
	    menuInsPopup(popup);
	    popup->updateAccel( this );
	    if ( !popup->isEnabled() )
		popup->enableAccel( FALSE );
	}
    }
}
#endif

/*!\reimp
 */
bool QMenuBar::customWhatsThis() const
{
    return TRUE;
}



/*!\reimp
 */
void QMenuBar::focusInEvent( QFocusEvent * )
{
    if ( actItem < 0 ) {
	int i = -1;
	while ( actItem < 0 && ++i < (int) mitems->count() ) {
	    QMenuItem* mi = mitems->at( i );
	    if ( mi && mi->isEnabled() && !mi->isSeparator() )
		setActiveItem( i, FALSE );
	}
    } else if ( !popupvisible ) {
	updateItem( idAt( actItem ) );
    }
}

/*!\reimp
 */
void QMenuBar::focusOutEvent( QFocusEvent * )
{
    updateItem( idAt( actItem ) );
    if ( !popupvisible )
	setAltMode( FALSE );
}

/*!
  \reimp
*/

QSize QMenuBar::sizeHint() const
{
    if ( badSize )
	( (QMenuBar*)this )->calculateRects();
    QSize s( style().defaultFrameWidth(), 0 );
    if ( irects ) {
	for ( int i = 0; i < (int)mitems->count(); ++i )
	    s.setWidth( s.width() + irects[ i ].width() + 2 );
    }
    s.setHeight( height() );
    return s.expandedTo( QApplication::globalStrut() );
}

/*!
  \reimp
*/

QSize QMenuBar::minimumSize() const
{
    if ( parent() && parent()->inherits( "QToolBar" ) )
	return sizeHint();
    return QFrame::minimumSize();
}

/*!
  \reimp
*/

QSize QMenuBar::minimumSizeHint() const
{
    return minimumSize();
}

/*!
  Sets the default popup orientation. By default, menus pop "down" the
  screen.  By calling setDefaultUp(TRUE) the menu will pop "up".  You
  might call this for menus that are \e below the document to which
  they refer.

  If the menu would not fit on the screen, the other direction is used
  rather than the default.

  \sa defaultUp();
*/
void QMenuBar::setDefaultUp( bool on )
{
    defaultup = on;
}

/*!
  Returns whether the menus default to popping "down" the screen
  (the default), or "up".

  \sa setDefaultUp();
*/
bool QMenuBar::isDefaultUp() const
{
    return defaultup;
}


/*!\reimp
 */
void QMenuBar::activateItemAt( int index )
{
    if ( index >= 0 && index < (int) mitems->count() )
	setActiveItem( index );
    else
	goodbye( FALSE );
}

#endif // QT_NO_MENUBAR
