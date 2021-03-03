/****************************************************************************
** $Id: qt/src/widgets/qtabbar.cpp   2.3.2   edited 2001-03-13 $
**
** Implementation of QTab and QTabBar classes
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

#include "qtabbar.h"
#ifndef QT_NO_TABBAR
#include "qaccel.h"
#include "qbitmap.h"
#include "qtoolbutton.h"
#include "qapplication.h"

#include <ctype.h>


// NOT REVISED
/*!
  \class QTab qtabbar.h
  \brief The structures in a QTabBar.

  For custom QTabBar tab headings.

  \sa QTabBar
*/


/*! \fn QTab::QTab()
  Constructs an empty tab.  All fields are set to empty.
*/


/*! \fn QTab::QTab( const QString& text )
  Constructs a tab with a \a text.
*/


/*! \fn QTab::QTab( const QIconSet& icon, const QString& text )
  Constructs a tab with an \a icon and a \a text.
*/


/*! Destructs the tab and frees up all allocated resources */

QTab::~QTab()
{
    delete iconset;
}


/*!
  \class QTabBar qtabbar.h

  \brief The QTabBar class provides a tab bar, for use in e.g. tabbed
  dialogs.

  \ingroup advanced

  The class is quite simple; it draws the tabs in one of four shapes
  and emits a signal when one is selected.  It can be subclassed to
  tailor the look and feel.

  QTabBar itself support four possible shapes, described in the
  QTabBar::Shape documentation.

  The choice of tab shape is still a matter of taste, to a large
  degree.  Tab dialogs (preferences and the like) invariably use \c
  RoundedAbove and nobody uses \c TriangularAbove.  Tab controls in
  windows other than dialogs almost always either \c RoundedBelow or
  \c TriangularBelow.  Many spreadsheets and other tab controls where
  all the pages are essentially similar to use \c TriangularBelow,
  while \c RoundedBelow is used mostly when the pages are different
  (e.g. a multi-page tool palette).  There is no strong tradition yet,
  however, so use your taste and create the tradition.

  The most important part of QTabBar's API is the signal selected().
  It's emitted whenever the selected page changes (even at startup,
  when the selected page changes from 'none').  There are also a slot,
  setCurrentTab(), which can be used to select a page
  programmatically.

  QTabBar creates automatic accelerator keys in the manner of QButton;
  e.g. if a tab's label is "\&Graphics" Alt-G becomes an accelerator
  key for switching to that tab.

  The following virtual functions may need to be reimplemented: <ul>
  <li> paint() paints a single tab.  paintEvent() calls paint() for
  each tab in such a way that any overlap will look right.  <li>
  addTab() creates a new tab and adds it to the bar. <li> selectTab()
  decides which, if any, tab the user selects with the mouse. </ul>

  <img src=qtabbar-m.png> <img src=qtabbar-w.png>
*/

/*! \enum QTabBar::Shape
  This enum type lists the built-in shapes supported by QTabBar:<ul>

  <li> \c RoundedAbove - the normal rounded look, above the pages

  <li> \c RoundedBelow - the normal rounded look, below the pages

  <li> \c TriangularAbove - triangular tabs, above the pages (very
  unusual, included for completeness)

  <li> \c TriangularBelow - triangular tabs, similar to those used in
  e.g. the spreadsheet Excel

  </ul>
*/

struct QTabPrivate {
    int id;
    int focus;
#ifndef QT_NO_ACCEL
    QAccel * a;
#endif
    QTabBar::Shape s;
    QToolButton* rightB;
    QToolButton* leftB;
    bool  scrolls;
};


/*!
  \fn void QTabBar::selected( int id )

  QTabBar emits this signal whenever any tab is selected, whether by
  the program or the user.  The argument \a id is the ID if the tab
  as returned by addTab().

  show() is guaranteed to emit this signal, so that you can display
  your page in a slot connected to this signal.
*/


/*!
  Constructs a new, empty tab bar.
*/

QTabBar::QTabBar( QWidget * parent, const char *name )
    : QWidget( parent, name, WRepaintNoErase | WResizeNoErase  )
{
    d = new QTabPrivate;
    d->id = 0;
    d->focus = 0;
#ifndef QT_NO_ACCEL
    d->a = new QAccel( this, "tab accelerators" );
#endif
    d->s = RoundedAbove;
    d->scrolls = FALSE;
    d->leftB = new QToolButton( LeftArrow, this );
    connect( d->leftB, SIGNAL( clicked() ), this, SLOT( scrollTabs() ) );
    d->leftB->hide();
    d->rightB = new QToolButton( RightArrow, this );
    connect( d->rightB, SIGNAL( clicked() ), this, SLOT( scrollTabs() ) );
    d->rightB->hide();
    l = new QList<QTab>;
    lstatic = new QList<QTab>;
    lstatic->setAutoDelete( TRUE );
    setFocusPolicy( TabFocus );
    setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed ) );

#ifndef QT_NO_ACCEL
    connect( d->a, SIGNAL(activated(int)), this, SLOT(setCurrentTab(int)) );
#endif
}


/*!
  Destroys the tab control, freeing memory used.
*/

QTabBar::~QTabBar()
{
    delete d;
    d = 0;
    delete l;
    l = 0;
    delete lstatic;
    lstatic = 0;
}


/*!
  Adds \a newTab to the tab control.

  Allocates a new id, sets \a newTab's id, locates it just to the right of the
  existing tabs, inserts an accelerator if the tab's label contains the
  string "&p" for some value of p, adds it to the bar, and returns the
  newly allocated id.
*/

int QTabBar::addTab( QTab * newTab )
{
    return insertTab( newTab );
}


/*!
  Inserts \a newTab to the tab control.

  If \a index is not specified, the tab is simply added. Otherwise
  it's inserted at the specified position.

  Allocates a new id, sets \a newTab's id, locates it respectively,
  inserts an accelerator if the tab's label contains the string "&p"
  for some value of p, adds it to the bar, and returns the newly
  allocated id.
*/

int QTabBar::insertTab( QTab * newTab, int index )
{
    newTab->id = d->id++;
    l->insert( 0, newTab );
    if ( index < 0 || index > int(lstatic->count()) )
	lstatic->append( newTab );
    else
	lstatic->insert( index, newTab );

    layoutTabs();
    updateArrowButtons();
    makeVisible( tab( currentTab() ) );

#ifndef QT_NO_ACCEL
    int p = QAccel::shortcutKey( newTab->label );
    if ( p )
	d->a->insertItem( p, newTab->id );
#endif

    return newTab->id;
}


/*!
  Removes tab \a t from the tab control.
*/
void QTabBar::removeTab( QTab * t )
{
    //#### accelerator labels??
    l->remove( t );
    lstatic->remove( t );
    layoutTabs();
    updateArrowButtons();
    makeVisible( tab( currentTab() ) );
    update();
}


/*!
  Enable tab \a id if \a enable is TRUE, or disable it if \a enable is
  FALSE.  If \a id is currently selected, setTabEnabled() makes
  another tab selected.

  setTabEnabled() updates the display respectively if this causes a
  change in \a id's status.

  \sa update(), isTabEnabled()
*/

void QTabBar::setTabEnabled( int id, bool enabled )
{
    QTab * t;
    for( t = l->first(); t; t = l->next() ) {
	if ( t && t->id == id ) {
	    if ( t->enabled != enabled ) {
		t->enabled = enabled;
#ifndef QT_NO_ACCEL
		d->a->setItemEnabled( t->id, enabled );
#endif
		QRect r( t->r );
		if ( !enabled && id == currentTab() ) {
		    QPoint p1( t->r.center() ), p2;
		    int m = 2147483647;
		    int distance;
		    // look for the closest enabled tab - measure the
		    // distance between the centers of the two tabs
		    for( QTab * n = l->first(); n; n = l->next() ) {
			if ( n->enabled ) {
			    p2 = n->r.center();
			    distance = (p2.x() - p1.x())*(p2.x() - p1.x()) +
				       (p2.y() - p1.y())*(p2.y() - p1.y());
			    if ( distance < m ) {
				t = n;
				m = distance;
			    }
			}
		    }
		    if ( t->enabled ) {
			r = r.unite( t->r );
			l->append( l->take( l->findRef( t ) ) );
			emit selected( t->id );
		    }
		}
		updateMask();
		repaint( r );
	    }
	    return;
	}
    }
}


/*!
  Returns TRUE if the tab with id \a id is enabled, or FALSE if it
  is disabled or there is no such tab.

  \sa setTabEnabled()
*/

bool QTabBar::isTabEnabled( int id ) const
{
    QTab * t;
    for( t = l->first(); t; t = l->next() ) {
	if ( t && t->id == id )
	    return t->enabled;
    }
    return FALSE;
}



/*!\reimp
*/
QSize QTabBar::sizeHint() const
{
    QTab * t = l->first();
    if ( t ) {
	QRect r( t->r );
	while ( (t = l->next()) != 0 )
	    r = r.unite( t->r );
	return r.size().expandedTo( QApplication::globalStrut() );
    } else {
	return QSize( 0, 0 ).expandedTo( QApplication::globalStrut() );
    }
}

/*! \reimp */

QSize QTabBar::minimumSizeHint() const
{
    return QSize( d->rightB->sizeHint().width() * 2 + 75, sizeHint().height() );
}

/*!\reimp
*/
QSizePolicy QTabBar::sizePolicy() const
{
    //### removeme 3.0
    return QWidget::sizePolicy();
}


/*!  Paint the single tab \a t using \a p.  If and only if \a selected
  is TRUE, \a t is currently selected.

  This virtual function may be reimplemented to change the look of
  QTabBar.  If you decide to reimplement it, you may also need to
  reimplement sizeHint().
*/

void QTabBar::paint( QPainter * p, QTab * t, bool selected ) const
{
    style().drawTab( p, this, t, selected );

    QRect r( t->r );
    p->setFont( font() );

    int iw = 0;
    int ih = 0;
    if ( t->iconset != 0 ) {
	iw = t->iconset->pixmap( QIconSet::Small, QIconSet::Normal ).width() + 2;
	ih = t->iconset->pixmap( QIconSet::Small, QIconSet::Normal ).height();
    }
    int w = iw + p->fontMetrics().width( t->label ) + 4;
    int h = QMAX(p->fontMetrics().height() + 4, ih );
    paintLabel( p, QRect( r.left() + (r.width()-w)/2 - 3,
			  r.top() + (r.height()-h)/2,
			  w, h ), t, t->id == keyboardFocusTab() );
}

/*!
  Paints the label of tab \a t centered in rectangle \a br using
  painter \a p and draws a focus indication if \a has_focus is TRUE.
*/

void QTabBar::paintLabel( QPainter* p, const QRect& br,
			  QTab* t, bool has_focus ) const
{

    QRect r = br;
    if ( t->iconset) {
	// the tab has an iconset, draw it in the right mode
	QIconSet::Mode mode = (t->enabled && isEnabled())
	    ? QIconSet::Normal : QIconSet::Disabled;
	if ( mode == QIconSet::Normal && has_focus )
	    mode = QIconSet::Active;
	QPixmap pixmap = t->iconset->pixmap( QIconSet::Small, mode );
	int pixw = pixmap.width();
	int pixh = pixmap.height();
	r.setLeft( r.left() + pixw + 2 );
	p->drawPixmap( br.left()+2, br.center().y()-pixh/2, pixmap );
    }

    QRect tr = r;
    if ( t->id == currentTab() )
 	tr.setBottom( tr.bottom() - style().defaultFrameWidth() );
    
    if ( t->enabled && isEnabled()  ) {
#if defined(_WS_WIN32_)
	if ( colorGroup().brush( QColorGroup::Button ) == colorGroup().brush( QColorGroup::Background ) )
	    p->setPen( colorGroup().buttonText() );
	else
	    p->setPen( colorGroup().foreground() );
#else
	p->setPen( colorGroup().foreground() );
#endif
	p->drawText( tr, AlignCenter | ShowPrefix, t->label );
    } else if ( style() == MotifStyle ) {
	p->setPen( palette().disabled().foreground() );
	p->drawText( tr, AlignCenter | ShowPrefix, t->label );
    } else { // Windows style, disabled
	p->setPen( colorGroup().light() );
	QRect wr = tr;
	wr.moveBy( 1, 1 );
	p->drawText( wr, AlignCenter | ShowPrefix, t->label );
	p->setPen( palette().disabled().foreground() );
	p->drawText( tr, AlignCenter | ShowPrefix, t->label );
    }

    if ( !has_focus )
	return;

    if ( style() == WindowsStyle )
	p->drawWinFocusRect( br, backgroundColor() );
    else // shouldn't this be black, irrespective of everything?
	p->drawRect( br );
}


/*!
  Draws the mask for this tab bar.

  \internal
  This is not totally right - a few corner pixels missing.
*/

void  QTabBar::updateMask()
{
    if ( !autoMask() )
	return;
    QBitmap bm( size() );
    bm.fill( color0 );

    QPainter p;
    p.begin( &bm, this );
    p.setBrush(color1);
    p.setPen(color1);

    QTab * t;
    t = l->first();
    do {
	QTab * n = l->next();
	if ( t )
	    style().drawTabMask( &p, this, t, n == 0 );
	t = n;
    } while ( t != 0 );


    p.end();
    setMask( bm );
}

/*!
  Repaints the tab row.  All the painting is done by paint();
  paintEvent() only decides which tabs need painting and in what
  order.

  \sa paint()
*/

void QTabBar::paintEvent( QPaintEvent * e )
{
    QPainter p( this );

    if ( backgroundMode() == X11ParentRelative ) {
	erase();
    } else {
	p.setBrushOrigin( rect().bottomLeft() );
	p.fillRect( 0, 0, width(), height(),
		    QBrush( colorGroup().brush( QColorGroup::Background ) ));
    }

    QTab * t;
    t = l->first();
    do {
	QTab * n = l->next();
	if ( t && t->r.intersects( e->rect() ) )
	    paint( &p, t, n == 0 );
	t = n;
    } while ( t != 0 );

    if ( d->scrolls && lstatic->first()->r.left() < 0 ) {
	QPointArray a;
	int h = height();
	if ( d->s == RoundedAbove ) {
	    p.fillRect( 0, 3, 4, h-5,
			QBrush( colorGroup().brush( QColorGroup::Background ) ));
	    a.setPoints( 5,  0,2,  3,h/4, 0,h/2, 3,3*h/4, 0,h );
	} else if ( d->s == RoundedBelow ) {
	    p.fillRect( 0, 2, 4, h-5,
			QBrush( colorGroup().brush( QColorGroup::Background ) ));
	    a.setPoints( 5,  0,0,  3,h/4, 0,h/2, 3,3*h/4, 0,h-3 );
	}

	if ( !a.isEmpty() ) {
	    p.setPen( colorGroup().light() );
	    p.drawPolyline( a );
	    a.translate( 1, 0 );
	    p.setPen( colorGroup().midlight() );
	    p.drawPolyline( a );
	}
    }
}


/*!
  This virtual functions is called by the mouse event handlers to
  determine which tab is pressed.  The default implementation returns
  a pointer to the tab whose bounding rectangle contains \a p, if
  exactly one tab's bounding rectangle contains \a p.  It returns 0
  else.

  \sa mousePressEvent() mouseReleaseEvent()
*/

QTab * QTabBar::selectTab( const QPoint & p ) const
{
    QTab * selected = 0;
    bool moreThanOne = FALSE;

    QListIterator<QTab> i( *l );
    while( i.current() ) {
	QTab * t = i.current();
	++i;

	if ( t && t->r.contains( p ) ) {
	    if ( selected )
		moreThanOne = TRUE;
	    else
		selected = t;
	}
    }

    return moreThanOne ? 0 : selected;
}


/*!\reimp
*/
void QTabBar::mousePressEvent( QMouseEvent * e )
{
    if ( e->button() != LeftButton )
	return;
    QTab * t = selectTab( e->pos() );
    if ( t != 0 && t == selectTab( e->pos() ) && t->enabled ) {
	setCurrentTab( t );
    }
}


/*!\reimp
*/

void QTabBar::mouseReleaseEvent( QMouseEvent * )
{
}


/*!  \reimp
*/
void QTabBar::show()
{
    //  ensures that one tab is selected.
    QTab * t = l->last();
    QWidget::show();

    if ( t )
	emit selected( t->id );
}

/*!  If a page is currently visible, returns its ID.  If no page is
  currently visible, returns either -1 or the ID of one of the pages.

  Even if the return value is not -1, you cannot assume either that
  the user can see the relevant page, or that the tab \link
  isTabEnabled() is enabled.\endlink

  When you need to display something, the return value from this
  function represents the best page to display.  That's all.

  \sa selected()
*/

int QTabBar::currentTab() const
{
    const QTab * t = l->getLast();

    return t ? t->id : -1;
}


/*! Raises the tab with ID \a id and emits the selected() signal.

  \sa currentTab() selected() tab()
*/

void QTabBar::setCurrentTab( int id )
{
    setCurrentTab( tab( id ) );
}


/*! Raises \a tab and emits the selected() signal unless the tab was
  already current.

  \sa currentTab() selected()
*/

void QTabBar::setCurrentTab( QTab * tab )
{
    if ( tab && l ) {
	if ( l->last() == tab )
	    return;

	QRect r = l->last()->r;
	if ( l->findRef( tab ) >= 0 )
	    l->append( l->take() );

	d->focus = tab->id;
	updateMask();
	if ( tab->r.intersects( r ) ) {
	    repaint( r.unite( tab->r ) );
	} else {
	    repaint( r );
	    repaint( tab->r );
	}
	makeVisible( tab );
	emit selected( tab->id );
    }
}

/*!  If this tab control has keyboard focus, returns the ID of the
  tab Space will select.  Otherwise, returns -1.
*/

int QTabBar::keyboardFocusTab() const
{
    return hasFocus() ? d->focus : -1;
}


/*!\reimp
*/
void QTabBar::keyPressEvent( QKeyEvent * e )
{
    //   The right and left arrow keys move a selector, the spacebar
    //   makes the tab with the selector active.  All other keys are
    //   ignored.

    int old = d->focus;

    if ( e->key() == Key_Left ) {
	// left - skip past any disabled ones
	if ( d->focus > 0 ) {
	    QTab * t = lstatic->last();
	    while ( t && t->id != d->focus )
		t = lstatic->prev();
	    do {
		t = lstatic->prev();
	    } while ( t && !t->enabled);
	    if (t)
		d->focus = t->id;
	}
	if ( d->focus < 0 )
	    d->focus = old;
    } else if ( e->key() == Key_Right ) {
	QTab * t = lstatic->first();
	while ( t && t->id != d->focus )
	    t = lstatic->next();
	do {
	    t = lstatic->next();
	} while ( t && !t->enabled);
    if (t)
	d->focus = t->id;
    if ( d->focus >= d->id )
	d->focus = old;
    } else {
	// other keys - ignore
	e->ignore();
	return;
    }

    // if the focus moved, repaint and signal
    if ( old != d->focus ) {
	setCurrentTab( d->focus );
    }
}


/*!  Returns a pointer to the tab with ID \a id, or 0 if there is no
  such tab.

  \sa count()
*/

QTab * QTabBar::tab( int id )
{
    QTab * t;
    for( t = l->first(); t; t = l->next() )
	if ( t && t->id == id )
	    return t;
    return 0;
}


/*! Returns the number of tabs in the tab bar.

  \sa tab()
*/
int QTabBar::count() const
{
    return l->count();
}


/*!
  The list of QTab objects added.
*/
QList<QTab> * QTabBar::tabList()
{
    return l;
}


/*!  Returns the shape of this tab bar. \sa setShape() */

QTabBar::Shape QTabBar::shape() const
{
    return d ? d->s : RoundedAbove;
}


/*!  Sets the shape of this tab bar to \a s and refreshes the bar.
*/

void QTabBar::setShape( Shape s )
{
    if ( !d || d->s == s )
	return;
    //######### must recalculate heights
    d->s = s;
    updateMask();
    update();
}



/*!
  Layout all existing tabs (i.e. setting their \c r attribute) according
  to their label and their iconset.
 */
void QTabBar::layoutTabs()
{
    if ( lstatic->isEmpty() )
	return;

    int hframe, vframe, overlap;
    style().tabbarMetrics( this, hframe, vframe, overlap );
    QFontMetrics fm = fontMetrics();
    int x = 0;
    QRect r;
    QTab *t;
    for ( t = lstatic->first(); t; t = lstatic->next() ) {
	int lw = fm.width( t->label );
	int iw = 0;
	int ih = 0;
	if ( t->iconset != 0 ) {
	    iw = t->iconset->pixmap( QIconSet::Small, QIconSet::Normal ).width() + 2;
	    ih = t->iconset->pixmap( QIconSet::Small, QIconSet::Normal ).height();
	}
	int h = QMAX( fm.height(), ih );
	h = QMAX( h, QApplication::globalStrut().height() );

	h += vframe;
	t->r.setRect( x, 0, QMAX( lw + hframe + iw,
		    QApplication::globalStrut().width() ), h );
	x += t->r.width() - overlap;
	r = r.unite( t->r );
    }
    for ( t = lstatic->first(); t; t = lstatic->next() )
	t->r.setHeight( r.height() );
}

/*!
  \reimp
*/

void QTabBar::styleChange( QStyle& old )
{
	layoutTabs();
	QWidget::styleChange( old );
}

/*!
  \reimp
*/
void QTabBar::focusInEvent( QFocusEvent * )
{
    QTab *t = l->first();
    for ( ; t; t = l->next() ) {
	if ( t->id == d->focus ) {
	    QPainter p;
	    p.begin( this );
	    QRect r = t->r;
	    p.setFont( font() );

	    int iw = 0;
	    int ih = 0;
	    if ( t->iconset != 0 ) {
		iw = t->iconset->pixmap( QIconSet::Small, QIconSet::Normal ).width() + 2;
		ih = t->iconset->pixmap( QIconSet::Small, QIconSet::Normal ).height();
	    }
	    int w = iw + p.fontMetrics().width( t->label ) + 4;
	    int h = QMAX(p.fontMetrics().height() + 4, ih );
	    paintLabel( &p, QRect( r.left() + ( r.width() -w ) /2 - 3,
				   r.top() + ( r.height()-h ) / 2,
				   w, h ), t, TRUE );
	    p.end();
	}
    }
}

/*!
  \reimp
*/
void QTabBar::focusOutEvent( QFocusEvent * )
{
    QTab *t = l->first();
    for ( ; t; t = l->next() ) {
	if ( t->id == d->focus ) {
	    QPainter p;
	    p.begin( this );
	    p.setBrushOrigin( rect().bottomLeft() );
	    QRect r = t->r;
	    p.setFont( font() );

	    int iw = 0;
	    int ih = 0;
	    if ( t->iconset != 0 ) {
		iw = t->iconset->pixmap( QIconSet::Small, QIconSet::Normal ).width() + 2;
		ih = t->iconset->pixmap( QIconSet::Small, QIconSet::Normal ).height();
	    }
	    int w = iw + p.fontMetrics().width( t->label ) + 4;
	    int h = QMAX(p.fontMetrics().height() + 4, ih );
	    p.fillRect( QRect( r.left() + ( r.width() -w ) / 2 - 4,
				   r.top() + ( r.height()-h ) / 2 - 1,
			       w + 2, h + 2 ), colorGroup().brush(QColorGroup::Background ) );
	    style().drawTab( &p, this, t, TRUE );
	    paintLabel( &p, QRect( r.left() + ( r.width() -w ) /2 - 3,
				   r.top() + ( r.height()-h ) / 2,
				   w, h ), t, FALSE );
	    p.end();
	}
    }
}

/*!
  \reimp
*/
void QTabBar::resizeEvent( QResizeEvent * )
{
    const int arrowWidth = 16;
    d->rightB->setGeometry( width() - arrowWidth, 0, arrowWidth, height() );
    d->leftB->setGeometry( width() - 2*arrowWidth, 0, arrowWidth, height() );
    layoutTabs();
    updateArrowButtons();
    makeVisible( tab( currentTab() ));
}

void QTabBar::scrollTabs()
{
    QTab* left = 0;
    QTab* right = 0;
    for ( QTab* t = lstatic->first(); t; t = lstatic->next() ) {
	if ( t->r.left() < 0 && t->r.right() > 0 )
	    left = t;
	if ( t->r.left() < d->leftB->x()+2 )
	    right = t;
    }

    if ( sender() == d->leftB )
	makeVisible( left );
    else  if ( sender() == d->rightB )
	makeVisible( right );
}

void QTabBar::makeVisible( QTab* tab  )
{
    bool tooFarLeft = ( tab && tab->r.left() < 0 );
    bool tooFarRight = ( tab && tab->r.right() >= d->leftB->x() );

    if ( !d->scrolls || ( !tooFarLeft && ! tooFarRight ) )
	return;

    layoutTabs();

    int offset = 0;

    if ( tooFarLeft )
	offset = tab == lstatic->first() ? 0 : tab->r.left() - 8;
    else if ( tooFarRight ) {
	offset = tab->r.right() - d->leftB->x() + 1;
    }

    for ( QTab* t = lstatic->first(); t; t = lstatic->next() )
	t->r.moveBy( -offset, 0 );

    d->leftB->setEnabled( offset != 0 );
    d->rightB->setEnabled( lstatic->last()->r.right() >= d->leftB->x() );


    update();
}

void QTabBar::updateArrowButtons()
{
    bool b = lstatic->last() &&	( lstatic->last()->r.right() > width() );
    d->scrolls = b;
    if ( d->scrolls ) {
	d->leftB->setEnabled( FALSE );
	d->rightB->setEnabled( TRUE );
	d->leftB->show();
	d->rightB->show();
    } else {
	d->leftB->hide();
	d->rightB->hide();
    }
}
#endif
