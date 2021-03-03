/****************************************************************************
** $Id: qt/src/widgets/qscrollview.cpp   2.3.2   edited 2001-03-02 $
**
** Implementation of QScrollView class
**
** Created : 950524
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

#include "qwidget.h"
#ifndef QT_NO_SCROLLVIEW
#include "qscrollbar.h"
#include "qobjectlist.h"
#include "qobjectdict.h"
#include "qpainter.h"
#include "qpixmap.h"
#include "qfocusdata.h"
#include "qscrollview.h"
#include "qptrdict.h"
#include "qapplication.h"
#include "qtimer.h"


const int coord_limit = 4000;
static const int autoscroll_margin = 16;
static const int initialScrollTime = 30;
static const int initialScrollAccel = 5;

struct QSVChildRec {
    QSVChildRec(QWidget* c, int xx, int yy) :
	child(c),
	x(xx), y(yy)
    {
    }

    void moveBy(QScrollView* sv, int dx, int dy, QWidget* clipped_viewport)
    {
	moveTo( sv, x+dx, y+dy, clipped_viewport );
    }
    void moveTo(QScrollView* sv, int xx, int yy, QWidget* clipped_viewport)
    {
	if ( x != xx || y != yy ) {
	    x = xx;
	    y = yy;
	    hideOrShow(sv,clipped_viewport);
	}
    }
    void hideOrShow(QScrollView* sv, QWidget* clipped_viewport)
    {
	if ( clipped_viewport ) {
	    if ( x+child->width() < sv->contentsX()+clipped_viewport->x()
	      || x > sv->contentsX()+clipped_viewport->width()
	      || y+child->height() < sv->contentsY()+clipped_viewport->y()
	      || y > sv->contentsY()+clipped_viewport->height() )
	    {
		child->move(clipped_viewport->width(),
			    clipped_viewport->height());
	    } else {
		child->move(x-sv->contentsX()-clipped_viewport->x(),
			    y-sv->contentsY()-clipped_viewport->y());
	    }
	} else {
	    child->move(x-sv->contentsX(), y-sv->contentsY());
	}
    }
    QWidget* child;
    int x, y;
};


class QClipperWidget : public QWidget {
public:
    QClipperWidget( QWidget * parent=0, const char * name=0, WFlags f=0 )
        : QWidget ( parent,name,f) { blockFocus = FALSE; }
    bool focusNextPrevChild( bool next ) {
	if ( !blockFocus )
	    return QWidget::focusNextPrevChild( next );
	else
	    return TRUE;
    }
    bool blockFocus;
};


struct QScrollViewData {
    QScrollViewData(QWidget* parent, int vpwflags) :
	hbar( QScrollBar::Horizontal, parent, "qt_hbar" ),
	vbar( QScrollBar::Vertical, parent, "qt_vbar" ),
	viewport( parent, "qt_viewport", vpwflags ),
	clipped_viewport( 0 ),
	flags( vpwflags ),
	vx( 0 ), vy( 0 ), vwidth( 1 ), vheight( 1 )
#ifndef QT_NO_DRAGANDDROP
	, autoscroll_timer( parent ), drag_autoscroll( TRUE )
#endif
    {
	l_marg = r_marg = t_marg = b_marg = 0;
	viewport.setBackgroundMode( QWidget::PaletteDark );
	vMode = QScrollView::Auto;
	hMode = QScrollView::Auto;
	corner = 0;
	vbar.setSteps( 20, 1/*set later*/ );
	hbar.setSteps( 20, 1/*set later*/ );
	policy = QScrollView::Default;
	signal_choke = FALSE;
	static_bg = FALSE;
    }
    ~QScrollViewData()
    {
	deleteAll();
    }

    QSVChildRec* rec(QWidget* w) { return childDict.find(w); }
    QSVChildRec* ancestorRec(QWidget* w)
    {
	if ( clipped_viewport ) {
	    while (w->parentWidget() != clipped_viewport) {
		w = w->parentWidget();
		if (!w) return 0;
	    }
	} else {
	    while (w->parentWidget() != &viewport) {
		w = w->parentWidget();
		if (!w) return 0;
	    }
	}
	return rec(w);
    }
    QSVChildRec* addChildRec(QWidget* w, int x, int y )
    {
	QSVChildRec *r = new QSVChildRec(w,x,y);
	children.append(r);
	childDict.insert(w, r);
	return r;
    }
    void deleteChildRec(QSVChildRec* r)
    {
	childDict.remove(r->child);
	children.removeRef(r);
	delete r;
    }
    void hideOrShowAll(QScrollView* sv, bool isScroll = FALSE )
    {
        if ( clipped_viewport ) {
	    if ( clipped_viewport->x() <= 0
		 && clipped_viewport->y() <= 0
		 && clipped_viewport->width()+clipped_viewport->x() >=
		 viewport.width()
		 && clipped_viewport->height()+clipped_viewport->y() >=
		 viewport.height() ) {
		// clipped_viewport still covers viewport
		if( static_bg )
		    clipped_viewport->repaint( clipped_viewport->visibleRect(), TRUE );
		else if ( ( !isScroll && !clipped_viewport->testWFlags( Qt::WNorthWestGravity) ) || static_bg )
		    QApplication::postEvent( clipped_viewport, new QPaintEvent( clipped_viewport->visibleRect(),
										!clipped_viewport->testWFlags(Qt::WResizeNoErase) ) );
	    } else {
		// Re-center
		int nx = ( viewport.width() - clipped_viewport->width() ) / 2;
		int ny = ( viewport.height() - clipped_viewport->height() ) / 2;
		// hide the clipped_viewport while we mess around
		// with it. To avoid having the focus jumping
		// around, we block it.
		clipped_viewport->blockFocus = TRUE;
		clipped_viewport->hide();
		clipped_viewport->move(nx,ny);
		clipped_viewport->blockFocus = FALSE;
		// no need to update, we'll receive a paintevent after show.
	    }
	    for (QSVChildRec *r = children.first(); r; r=children.next()) {
		r->hideOrShow(sv, clipped_viewport);
	    }
	    clipped_viewport->show();
	}
    }

    void moveAllBy(int dx, int dy)
    {
	if ( clipped_viewport && !static_bg ) {
	    clipped_viewport->move(
		clipped_viewport->x()+dx,
		clipped_viewport->y()+dy
	    );
	} else {
	    for (QSVChildRec *r = children.first(); r; r=children.next()) {
		r->child->move(r->child->x()+dx,r->child->y()+dy);
	    }
	    if ( static_bg )
		viewport.repaint( viewport.visibleRect(), TRUE );
	}
    }
    void deleteAll()
    {
	for (QSVChildRec *r = children.first(); r; r=children.next()) {
	    delete r;
	}
    }
    bool anyVisibleChildren()
    {
	for (QSVChildRec *r = children.first(); r; r=children.next()) {
	    if (r->child->isVisible()) return TRUE;
	}
	return FALSE;
    }
    void autoMove(QScrollView* sv)
    {
	if ( policy == QScrollView::AutoOne ) {
	    QSVChildRec* r = children.first();
	    if (r)
	        sv->setContentsPos(-r->child->x(),-r->child->y());
	}
    }
    void autoResize(QScrollView* sv)
    {
	if ( policy == QScrollView::AutoOne ) {
	    QSVChildRec* r = children.first();
	    if (r)
		sv->resizeContents(r->child->width(),r->child->height());
	}
    }
    void autoResizeHint(QScrollView* sv)
    {
	if ( policy == QScrollView::AutoOne ) {
	    QSVChildRec* r = children.first();
	    if (r) {
                QSize s = r->child->sizeHint();
	        if ( s.isValid() )
		    r->child->resize(s);
	    }
	} else if ( policy == QScrollView::AutoOneFit ) {
	    QSVChildRec* r = children.first();
	    if (r) {
		QSize sh = r->child->sizeHint();
		sh = sh.boundedTo( r->child->maximumSize() );
	        sv->resizeContents( sh.width(), sh.height() );
	    }
	}
    }

    void viewportResized( int w, int h ) {
	if ( policy == QScrollView::AutoOneFit ) {
	    QSVChildRec* r = children.first();
	    if (r) {
		QSize sh = r->child->sizeHint();
		sh = sh.boundedTo( r->child->maximumSize() );
		r->child->resize( QMAX(w,sh.width()), QMAX(h,sh.height()) );
	    }

	}
    }

    QScrollBar	hbar;
    QScrollBar	vbar;
    QWidget	viewport;
    QClipperWidget*    clipped_viewport;
    int		flags;
    QList<QSVChildRec>	children;
    QPtrDict<QSVChildRec>	childDict;
    QWidget*	corner;
    int		vx, vy, vwidth, vheight; // for drawContents-style usage
    int		l_marg, r_marg, t_marg, b_marg;
    QScrollView::ResizePolicy policy;
    QScrollView::ScrollBarMode	vMode;
    QScrollView::ScrollBarMode	hMode;
#ifndef QT_NO_DRAGANDDROP
    QPoint cpDragStart;
    QTimer autoscroll_timer;
    int autoscroll_time;
    int autoscroll_accel;
    bool drag_autoscroll;
#endif

    bool static_bg;

    // This variable allows ensureVisible to move the contents then
    // update both the sliders.  Otherwise, updating the sliders would
    // cause two image scrolls, creating ugly flashing.
    //
    bool signal_choke;
};

// NOT REVISED
/*!
\class QScrollView qscrollview.h
\brief The QScrollView widget provides a scrolling area with on-demand scrollbars.

\ingroup abstractwidgets

The QScrollView is a large canvas - potentially larger than the
coordinate system normally supported by the underlying window system.
This is important, as is is quite easy to go beyond such limitations
(eg. many web pages are more than 32000 pixels high).  Additionally,
the QScrollView can have QWidgets positioned on it that scroll around
with the drawn content.  These subwidgets can also have positions
outside the normal coordinate range (but they are still limited in
size).

To provide content for the widget, inherit from QScrollView and
reimplement drawContents(), and use resizeContents() to set the size
of the viewed area.  Use addChild() / moveChild() to position widgets
on the view.

To use QScrollView effectively, it is important to understand its
widget structure in the three styles of usage: a single large child widget,
a large panning area with some widgets, a large panning area with many widgets.

<dl>
<dt><b>One Big Widget</b>
<dd>

<img src=qscrollview-vp2.png>

The first, simplest usage of QScrollView depicted above is
appropriate for scrolling areas
which are \e never more than about 4000 pixels in either dimension (this
is about the maximum reliable size on X11 servers).  In this usage, you
just make one large child in the QScrollView.  The child should
be a child of the viewport() of the scrollview, and be added with addChild():
\code
    QScrollView* sv = new QScrollView(...);
    QVBox* big_box = new QVBox(sv->viewport());
    sv->addChild(big_box);
\endcode
You may go on to add arbitrary child widgets to the single child in
the scrollview, as you would with any widget:
\code
    QLabel* child1 = new QLabel("CHILD", big_box);
    QLabel* child2 = new QLabel("CHILD", big_box);
    QLabel* child3 = new QLabel("CHILD", big_box);
    ...
\endcode
Here, the QScrollView has 4 children - the viewport(),
the verticalScrollBar(), the horizontalScrollBar(), and
a small cornerWidget().  The viewport() has 1 child, the big QVBox.
The QVBox has the three labels as child widgets.  When the view is scrolled,
the QVBox is moved, and its children move with it as child widgets normally
do.

<dt><b>Very Big View, some Widgets</b>
<dd>

<img src=qscrollview-vp.png>

The second usage of QScrollView depicted above is appropriate when
few, if any, widgets are on a very large scrolling area that is
potentially larger than 4000 pixels in either dimension. In this
usage, you call resizeContents() to set the size of the area, and
reimplement drawContents() to paint the contents.  You may also add
some widgets, by making them children of the viewport() and adding
them with addChild() (this is the same as the process for the single
large widget in the previous example): \code
    QScrollView* sv = new QScrollView(...);
    QLabel* child1 = new QLabel("CHILD", sv->viewport());
    sv->addChild(child1);
    QLabel* child2 = new QLabel("CHILD", sv->viewport());
    sv->addChild(child2);
    QLabel* child3 = new QLabel("CHILD", sv->viewport());
    sv->addChild(child3);
\endcode
Here, the QScrollView has the same 4 children - the viewport(),
the verticalScrollBar(), the horizontalScrollBar(), and
a small cornerWidget().  The viewport()
has the three labels as child widgets.  When the view is scrolled,
the scrollview moves the child widgets individually.

<dt><b>Very Big View, many Widgets</b>
<dd>

<img src=qscrollview-cl.png>

The final usage of QScrollView depicted above is
appropriate when many widgets are on a very large scrolling area
that is potentially larger than 4000 pixels in either dimension. In this
usage, you call resizeContents() to set the size of the area, and reimplement
drawContents() to paint the contents.  You then call enableClipper(TRUE)
and add widgets, again
by making them children of the viewport() and adding them with
addChild():
\code
    QScrollView* sv = new QScrollView(...);
    sv->enableClipper(TRUE);
    QLabel* child1 = new QLabel("CHILD", sv->viewport());
    sv->addChild(child1);
    QLabel* child2 = new QLabel("CHILD", sv->viewport());
    sv->addChild(child2);
    QLabel* child3 = new QLabel("CHILD", sv->viewport());
    sv->addChild(child3);
\endcode

Here, the QScrollView has 4 children - the clipper() (\e not the
viewport() this time), the verticalScrollBar(), the
horizontalScrollBar(), and a small cornerWidget().  The clipper() has
1 child - the viewport().  The viewport() has the three labels as
child widgets.  When the view is scrolled, the viewport() is moved,
and its children move with it as child widgets normally do.

</dl>

Normally you will use the first or third method if you want any child
widgets in the view.

Note that the widget you see in the scrolled area is the viewport()
widget, not the QScrollView itself.  So, to turn mouse tracking on for
example, use viewport()->setMouseTracking(TRUE).

To enable drag-and-drop, you would setAcceptDrops(TRUE) on the
QScrollView (since drag-and-drop events propagate to the parent), but
to work out what logical position in the view, you would need to map
the drop co-ordinate from being relative to the QScrollView to being
relative to the contents - use the function viewportToContents() for this.

To handle mouse events on the scrolling area, subclass scrollview as
you would subclass other widgets, but rather than overriding
mousePressEvent(), reimplement viewportMousePressEvent() instead (if
you reimplement mousePressEvent() you'll only get called when part of the
QScrollView is clicked - and the only such part is the "corner" (if
you don't set a cornerWidget()) and the frame, everything else being
covered up by the viewport, clipper, or scrollbars.

When you construct a QScrollView, some of the widget flags apply to the
viewport(), instead of being sent to the QWidget constructor for the
QScrollView. This applies to \c WResizeNoErase, \c WNorthWestGravity,
\c WRepaintNoErase and \c WPaintClever. See Qt::WidgetFlags for
documentation about these flags.  Here are some examples: <ul>

<li> An image manipulation widget would use \c
WResizeNoErase|WNorthWestGravity, because the widget draws all pixels
itself and when the size increases, it only needs a paint event for
the new part, since the old part remains unchanged.

<li>A word processing widget might use \c WResizeNoErase and repaint
itself line by line to get a less flickery resizing. If the widget is
in a mode where no text justification can take place, it might use \c
WNorthWestGravity too, so that it would only get a repaint for the
newly visible parts.

<li>A scrolling game widget where the background scrolls as the
characters move might use \c WRepaintNoErase (in addition to \c
WNorthWestGravity and \c WResizeNoErase) so that the window system
background does not flash in and out during scrolling.
</ul>

\warning WResizeNoErase is currently set by default, i.e. you always
have to clear the background manually in scrollview subclasses. This
will change in a future version of Qt, and we recommend specifying the
flag explicitly.



<img src=qscrollview-m.png> <img src=qscrollview-w.png>
*/


/*! \enum QScrollView::ResizePolicy

  This enum type is used to control QScrollView's reaction to resize
  events.  There are four possible settings:<ul>

  <li> \c Default - QScrollView selects one of the other settings
  automatically when it has to.  In this version of Qt, QScrollView
  changes to \c Manual if you resize the contents with
  resizeContents(), and to \c AutoOne if a child is added.

  <li> \c Manual - the view stays the size set by resizeContents().

  <li> \c AutoOne - if there is only child widget, the view stays
  the size of that widget.  Otherwise, the behaviour is undefined.

  <li> \c AutoOneFit - if there is only one child widget the view stays
  the size of that widget's sizeHint(). If the scrollview is resized bigger
  than the child's sizeHint(), the child will be resized to fit.
  If there is more than one child, the behaviour is undefined.

  </ul>
*/
//####  The widget will be resized to its sizeHint() when a LayoutHint event
//#### is received

/*!

  Constructs a QScrollView with a \a parent, a \a name and widget
  flags \a f.

  The widget flags \c WNorthWestGravity, \c WRepaintNoErase and \c
  WPaintClever are propagated to the viewport() widget. The other
  widget flags are propagated to the parent constructor as usual.
*/



QScrollView::QScrollView( QWidget *parent, const char *name, WFlags f ) :
    QFrame( parent, name, f & (~WNorthWestGravity) & (~WRepaintNoErase), FALSE )
{
    d = new QScrollViewData(this,WResizeNoErase |
	    (f&WPaintClever) | (f&WRepaintNoErase) | (f&WNorthWestGravity) );
#ifndef QT_NO_DRAGANDDROP
    connect( &d->autoscroll_timer, SIGNAL( timeout() ),
	     this, SLOT( doDragAutoScroll() ) );
#endif

    connect( &d->hbar, SIGNAL( valueChanged( int ) ),
	this, SLOT( hslide( int ) ) );
    connect( &d->vbar, SIGNAL( valueChanged( int ) ),
	this, SLOT( vslide( int ) ) );
    d->viewport.installEventFilter( this );

    setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    setLineWidth( style().defaultFrameWidth() );
    setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
}


/*!
  Destructs the QScrollView.  Any children added with addChild()
  will be destructed.
*/

QScrollView::~QScrollView()
{
    // Be careful not to get all those useless events...
    if ( d->clipped_viewport )
	d->clipped_viewport->removeEventFilter( this );
    else
	d->viewport.removeEventFilter( this );
    QScrollViewData* d2 = d;
    d = 0;
    delete d2;
}


/*!
  \reimp
*/
void QScrollView::styleChange( QStyle& old )
{
    QWidget::styleChange( old );
    updateScrollBars();
}


void QScrollView::hslide( int pos )
{
    if ( !d->signal_choke ) {
	moveContents( -pos, -contentsY() );
	QApplication::syncX();
    }
}

void QScrollView::vslide( int pos )
{
    if ( !d->signal_choke ) {
	moveContents( -contentsX(), -pos );
	QApplication::syncX();
    }
}

/*!
  Called when the horizontal scrollbar geometry changes.  This is provided
  as a protected function so that subclasses can do interesting things
  like providing extra buttons in some of the space normally used by the
  scrollbars.

  The default implementation simply gives all the space to \a hbar.

  \sa setVBarGeometry()
*/
void QScrollView::setHBarGeometry(QScrollBar& hbar,
    int x, int y, int w, int h)
{
    hbar.setGeometry( x, y, w, h );
}

/*!
  Called when the vertical scrollbar geometry changes.  This is provided
  as a protected function so that subclasses can do interesting things
  like providing extra buttons in some of the space normally used by the
  scrollbars.

  The default implementation simply gives all the space to \a vbar.

  \sa setHBarGeometry()
*/
void QScrollView::setVBarGeometry( QScrollBar& vbar,
    int x, int y, int w, int h)
{
    vbar.setGeometry( x, y, w, h );
}


/*! Returns the viewport size for size (\a x, \a y).

  The viewport size depends on \a x,y (the size of the contents), the
  size of this widget, the modes of the horizontal and vertical scroll
  bars.

  This function permits widgets that can trade vertical and horizontal
  space for each other to control scroll bar appearance better.  For
  example, a word processor or web browser can control the width of
  the right margin accurately, whether there needs to be a vertical
  scroll bar or not.
*/

QSize QScrollView::viewportSize( int x, int y ) const
{
    int fw = frameWidth();
    int lmarg = fw+d->l_marg;
    int rmarg = fw+d->r_marg;
    int tmarg = fw+d->t_marg;
    int bmarg = fw+d->b_marg;

    int w = width();
    int h = height();

    bool needh, needv;
    bool showh, showv;
    int hsbExt = horizontalScrollBar()->sizeHint().height();
    int vsbExt = verticalScrollBar()->sizeHint().width();

    if ( d->policy != AutoOne || d->anyVisibleChildren() ) {
	// Do we definitely need the scrollbar?
	needh = w-lmarg-rmarg < x;
	needv = h-tmarg-bmarg < y;

	// Do we intend to show the scrollbar?
	if (d->hMode == AlwaysOn)
	    showh = TRUE;
	else if (d->hMode == AlwaysOff)
	    showh = FALSE;
	else
	    showh = needh;

	if (d->vMode == AlwaysOn)
	    showv = TRUE;
	else if (d->vMode == AlwaysOff)
	    showv = FALSE;
	else
	    showv = needv;

	// Given other scrollbar will be shown, NOW do we need one?
	if ( showh && h-vsbExt-tmarg-bmarg < y ) {
	    if (d->vMode == Auto)
		showv=TRUE;
	}
	if ( showv && w-hsbExt-lmarg-rmarg < x ) {
	    if (d->hMode == Auto)
		showh=TRUE;
	}
    } else {
	// Scrollbars not needed, only show scrollbar that are always on.
	showh = d->hMode == AlwaysOn;
	showv = d->vMode == AlwaysOn;
    }

    return QSize( w-lmarg-rmarg - (showv ? vsbExt : 0),
		  h-tmarg-bmarg - (showh ? hsbExt : 0) );
}


/*!
  Updates scrollbars - all possibilities considered.  You should never
  need to call this in your code.
*/
void QScrollView::updateScrollBars()
{
    // I support this should use viewportSize()... but it needs
    // so many of the temporary variables from viewportSize.  hm.
    int fw = frameWidth();
    int lmarg = fw+d->l_marg;
    int rmarg = fw+d->r_marg;
    int tmarg = fw+d->t_marg;
    int bmarg = fw+d->b_marg;

    int w = width();
    int h = height();

    int portw, porth;

    bool needh;
    bool needv;
    bool showh;
    bool showv;

    int hsbExt = horizontalScrollBar()->sizeHint().height();
    int vsbExt = verticalScrollBar()->sizeHint().width();

    if ( d->policy != AutoOne || d->anyVisibleChildren() ) {
	// Do we definitely need the scrollbar?
	needh = w-lmarg-rmarg < contentsWidth();
	needv = h-tmarg-bmarg < contentsHeight();

	// Do we intend to show the scrollbar?
	if (d->hMode == AlwaysOn)
	    showh = TRUE;
	else if (d->hMode == AlwaysOff)
	    showh = FALSE;
	else
	    showh = needh;

	if (d->vMode == AlwaysOn)
	    showv = TRUE;
	else if (d->vMode == AlwaysOff)
	    showv = FALSE;
	else
	    showv = needv;

	// Given other scrollbar will be shown, NOW do we need one?
	if ( showh && h-vsbExt-tmarg-bmarg < contentsHeight() ) {
	    needv=TRUE;
	    if (d->vMode == Auto)
		showv=TRUE;
	}
	if ( showv && w-hsbExt-lmarg-rmarg < contentsWidth() ) {
	    needh=TRUE;
	    if (d->hMode == Auto)
		showh=TRUE;
	}
    } else {
	// Scrollbars not needed, only show scrollbar that are always on.
	needh = needv = FALSE;
	showh = d->hMode == AlwaysOn;
	showv = d->vMode == AlwaysOn;
    }

    bool sc = d->signal_choke;
    d->signal_choke=TRUE;

    // Hide unneeded scrollbar, calculate viewport size
    if ( showh ) {
        porth=h-hsbExt-tmarg-bmarg;
    } else {
	if (!needh)
	    d->hbar.setValue(0);
	d->hbar.hide();
	porth=h-tmarg-bmarg;
    }
    if ( showv ) {
	portw=w-vsbExt-lmarg-rmarg;
    } else {
	if (!needv)
	    d->vbar.setValue(0);
	d->vbar.hide();
	portw=w-lmarg-rmarg;
    }

    // Configure scrollbars that we will show
	if ( needv ) {
	    d->vbar.setRange( 0, contentsHeight()-porth );
	    d->vbar.setSteps( QScrollView::d->vbar.lineStep(), porth );
	} else {
	    d->vbar.setRange( 0, 0 );
	}
	if ( needh ) {
	    d->hbar.setRange( 0, contentsWidth()-portw );
	    d->hbar.setSteps( QScrollView::d->hbar.lineStep(), portw );
	} else {
	    d->hbar.setRange( 0, 0 );
	}

    // Position the scrollbars, viewport, and corner widget.
    int bottom;
    if ( showh ) {
	int right = ( showv || cornerWidget() ) ? w-vsbExt : w;
	if ( style() == WindowsStyle )
            setHBarGeometry(d->hbar, fw, h-hsbExt-fw,
                            right-fw-fw, hsbExt );
	else
            setHBarGeometry(d->hbar, 0, h-hsbExt, right,
                            hsbExt );
	bottom=h-hsbExt;
    } else {
        bottom=h;
    }
    if ( showv ) {
	clipper()->setGeometry( lmarg, tmarg,
                                w-vsbExt-lmarg-rmarg,
                                bottom-tmarg-bmarg );
	d->viewportResized( w-vsbExt-lmarg-rmarg, bottom-tmarg-bmarg );
	if ( style() == WindowsStyle )
	    changeFrameRect(QRect(0, 0, w, h) );
	else
	    changeFrameRect(QRect(0, 0, w-vsbExt, bottom));
	if (cornerWidget()) {
	    if ( style() == WindowsStyle )
                setVBarGeometry( d->vbar, w-vsbExt-fw,
                                 fw, vsbExt,
                                 h-hsbExt-fw-fw );
	    else
                setVBarGeometry( d->vbar, w-vsbExt, 0,
                                 vsbExt,
                                 h-hsbExt );
	}
	else {
	    if ( style() == WindowsStyle )
                setVBarGeometry( d->vbar, w-vsbExt-fw,
                                 fw, vsbExt,
                                 bottom-fw-fw );
	    else
                setVBarGeometry( d->vbar, w-vsbExt, 0,
                                 vsbExt, bottom );
	}
    } else {
	if ( style() == WindowsStyle )
	    changeFrameRect(QRect(0, 0, w, h));
	else
	    changeFrameRect(QRect(0, 0, w, bottom));
	clipper()->setGeometry( lmarg, tmarg,
				 w-lmarg-rmarg, bottom-tmarg-bmarg );
	d->viewportResized( w-lmarg-rmarg, bottom-tmarg-bmarg );
    }
    if ( d->corner ) {
	if ( style() == WindowsStyle )
            d->corner->setGeometry( w-vsbExt-fw,
                                    h-hsbExt-fw,
                                    vsbExt,
                                    hsbExt );
	else
            d->corner->setGeometry( w-vsbExt,
                                    h-hsbExt,
                                    vsbExt,
                                    hsbExt );
    }

    d->signal_choke=sc;

    if ( contentsX()+visibleWidth() > contentsWidth() ) {
	int x=QMAX(0,contentsWidth()-visibleWidth());
	d->hbar.setValue(x);
	// Do it even if it is recursive
	moveContents( -x, -contentsY() );
    }
    if ( contentsY()+visibleHeight() > contentsHeight() ) {
	int y=QMAX(0,contentsHeight()-visibleHeight());
	d->vbar.setValue(y);
	// Do it even if it is recursive
	moveContents( -contentsX(), -y );
    }

    // Finally, show the scrollbars.
    if ( showh && !d->hbar.isVisible() )
	d->hbar.show();
    if ( showv && !d->vbar.isVisible() )
	d->vbar.show();
}


/*! \reimp
*/
void QScrollView::show()
{
    //     Ensures that scrollbars have the correct size when the
    //     widget is shown.
    if (isVisible()) return;
    QWidget::show();
    updateScrollBars();
    d->hideOrShowAll(this);
}

/*! \reimp
 */
void QScrollView::resize( int w, int h )
{
    //   Ensures that scrollbars have the correct size when the widget is
    //   resized.
    QWidget::resize( w, h );
}

/*! \reimp
*/
void QScrollView::resize( const QSize& s )
{
    //   Ensures that scrollbars have the correct size when the widget is
    //   resized.
    resize(s.width(),s.height());
}

/*! \reimp
*/
void QScrollView::resizeEvent( QResizeEvent* event )
{
    // Ensures that scrollbars have the correct size when the widget
    // is resized.
    bool u = isUpdatesEnabled();
    setUpdatesEnabled( FALSE );
    QFrame::resizeEvent( event );

    // do _not_ update the scrollbars when updates have been
    // disabled. This makes it possible for subclasses to implement
    // dynamic wrapping without a horizontal scrollbar showing up all
    // the time when making a window smaller.
    if ( u )
	updateScrollBars();
    d->hideOrShowAll(this);
    setUpdatesEnabled( u );
}


/*! \reimp
*/
void QScrollView::wheelEvent( QWheelEvent *e ){
    QWheelEvent ce( viewport()->mapFromGlobal( e->globalPos() ),
		    e->globalPos(), e->delta(), e->state());
    viewportWheelEvent(&ce);
    if ( !ce.isAccepted() ) {
	if (verticalScrollBar())
	    QApplication::sendEvent( verticalScrollBar(), e);
    }
}

/*!
  Returns the currently set mode for the vertical scrollbar.

  \sa setVScrollBarMode()
*/
QScrollView::ScrollBarMode QScrollView::vScrollBarMode() const
{
    return d->vMode;
}


/*! \enum QScrollView::ScrollBarMode

  This enum type describes the various modes of QScrollView's scroll
  bars.  The defined modes are: <ul>

   <li> \c Auto - QScrollView shows a scrollbar when the content is
   too tall to fit and not else.  This is the default.

   <li> \c AlwaysOff - QScrollView never shows a scrollbar.

   <li> \c AlwaysOn - QScrollView always shows a scrollbar.

   </ul>

   (The modes for the horizontal and vertical scroll bars are independent.)
*/


/*!
  Sets the mode for the vertical scrollbar.

  \sa vScrollBarMode(), setHScrollBarMode()
*/
void  QScrollView::setVScrollBarMode( ScrollBarMode mode )
{
    if (d->vMode != mode) {
	d->vMode = mode;
	updateScrollBars();
    }
}


/*!
  Returns the currently set mode for the horizontal scrollbar.

  \sa setHScrollBarMode()
*/
QScrollView::ScrollBarMode QScrollView::hScrollBarMode() const
{
    return d->hMode;
}

/*!
  Sets the mode for the horizontal scrollbar.
  <ul>
   <li> \c Auto (the default) shows a scrollbar when the content is too wide to fit.
   <li> \c AlwaysOff never shows a scrollbar.
   <li> \c AlwaysOn always shows a scrollbar.
  </ul>

  \sa hScrollBarMode(), setVScrollBarMode()
*/
void QScrollView::setHScrollBarMode( ScrollBarMode mode )
{
    if (d->hMode != mode) {
	d->hMode = mode;
	updateScrollBars();
    }
}


/*!
Returns the widget in the corner between the two scrollbars.

By default, no corner widget is present.
*/
QWidget* QScrollView::cornerWidget() const
{
    return d->corner;
}

/*!
  Sets the widget in the corner between the two scrollbars.

  You will probably also want to
  set at least one of the scrollbar modes to AlwaysOn.

  Passing 0 shows no widget in the corner.

  Any previous corner widget is hidden.

  You may call setCornerWidget() with the same widget at different times.

  All widgets set here will be deleted by the QScrollView when it destructs
  unless you separately
  reparent the widget after setting some other corner widget (or 0).

  Any \e newly set widget should have no current parent.

  By default, no corner widget is present.

  \sa setVScrollBarMode(), setHScrollBarMode()
*/
void QScrollView::setCornerWidget(QWidget* corner)
{
    QWidget* oldcorner = d->corner;
    if (oldcorner != corner) {
	if (oldcorner) oldcorner->hide();
	d->corner = corner;

	if ( corner && corner->parentWidget() != this ) {
	    // #### No clean way to get current WFlags
	    corner->reparent( this, (((QScrollView*)corner))->getWFlags(),
			      QPoint(0,0), FALSE );
	}

	updateScrollBars();
	if ( corner ) corner->show();
    }
}


/*!
  Sets the resize policy to \a r.

  \sa resizePolicy() ResizePolicy
*/
void QScrollView::setResizePolicy( ResizePolicy r )
{
    d->policy = r;
}

/*!
  Returns the currently set ResizePolicy.

  \sa setResizePolicy() ResizePolicy
*/
QScrollView::ResizePolicy QScrollView::resizePolicy() const
{
    return d->policy;
}

/*! \reimp
*/
void QScrollView::setEnabled( bool enable )
{
    QFrame::setEnabled( enable );
}

/*!
  Removes a child from the scrolled area.  Note that this happens
  automatically if the child is deleted.
*/
void QScrollView::removeChild(QWidget* child)
{
    if ( !d ) // In case we are destructing
	return;

    QSVChildRec *r = d->rec(child);
    if ( r ) d->deleteChildRec( r );
}

/*! \reimp
*/
void QScrollView::removeChild(QObject* child)
{
    QFrame::removeChild(child);
}

/*!
  Inserts \a child into the scrolled area positioned at (\a x, \a y).
  The position defaults to (0,0). If the child is already in the view,
  it is just moved.

  You may want to call enableClipper(TRUE) if you add a large number
  of widgets.
*/
void QScrollView::addChild(QWidget* child, int x, int y)
{
    if ( child->parentWidget() == viewport() ) {
	// May already be there
	QSVChildRec *r = d->rec(child);
	if (r) {
	    r->moveTo(this,x,y,d->clipped_viewport);
	    if ( d->policy > Manual ) {
		d->autoResizeHint(this);
		d->autoResize(this); // #### better to just deal with this one widget!
	    }
	    return;
	}
    }

    if ( d->children.isEmpty() && d->policy != Manual ) {
	if ( d->policy == Default )
	    setResizePolicy( AutoOne );
	child->installEventFilter( this );
    } else if ( d->policy == AutoOne ) {
	child->removeEventFilter( this ); //#### ?????
        setResizePolicy( Manual );
    }
    if ( child->parentWidget() != viewport() ) {
	    child->reparent( viewport(), 0, QPoint(0,0), FALSE );
    }
    d->addChildRec(child,x,y)->hideOrShow(this, d->clipped_viewport);

    if ( d->policy > Manual ) {
	d->autoResizeHint(this);
	d->autoResize(this); // #### better to just deal with this one widget!
    }
}

/*!
  Repositions \a child to (\a x, \a y).
  This functions the same as addChild().
*/
void QScrollView::moveChild(QWidget* child, int x, int y)
{
    addChild(child,x,y);
}

/*!
  Returns the X position of the given child widget.
  Use this rather than QWidget::x() for widgets added to the view.
*/
int QScrollView::childX(QWidget* child)
{
    return d->rec(child)->x;
}

/*!
  Returns the Y position of the given child widget.
  Use this rather than QWidget::y() for widgets added to the view.
*/
int QScrollView::childY(QWidget* child)
{
    return d->rec(child)->y;
}

/*!
  \obsolete

  Returns TRUE if \a child is visible.  This is equivalent
  to child->isVisible().
*/
bool QScrollView::childIsVisible(QWidget* child)
{
    return child->isVisible();
}

/*!
  \obsolete

  Sets the visibility of \a child. Equivalent to
  QWidget::show() or QWidget::hide().
*/
void QScrollView::showChild(QWidget* child, bool y)
{
    if ( y )
	child->show();
    else
	child->hide();
}


/*!
  This event filter ensures the scrollbars are updated when a single
  contents widget is resized, shown, hidden, or destroyed, and passes
  mouse events to the QScrollView.
*/

bool QScrollView::eventFilter( QObject *obj, QEvent *e )
{
    if (!d) return FALSE; // we are destructing
    if ( obj == &d->viewport || obj == d->clipped_viewport ) {
	switch ( e->type() ) {

	    /* Forward many events to viewport...() functions */
	case QEvent::Paint:
	    viewportPaintEvent( (QPaintEvent*)e );
	    break;
	case QEvent::Resize:
	    viewportResizeEvent( (QResizeEvent*)e );
	    break;
	case QEvent::MouseButtonPress:
	    viewportMousePressEvent( (QMouseEvent*)e );
	    break;
	case QEvent::MouseButtonRelease:
	    viewportMouseReleaseEvent( (QMouseEvent*)e );
	    break;
	case QEvent::MouseButtonDblClick:
	    viewportMouseDoubleClickEvent( (QMouseEvent*)e );
	    break;
	case QEvent::MouseMove:
	    viewportMouseMoveEvent( (QMouseEvent*)e );
	    break;
#ifndef QT_NO_DRAGANDDROP
	case QEvent::DragEnter:
	    viewportDragEnterEvent( (QDragEnterEvent*)e );
	    break;
	case QEvent::DragMove: {
	    if ( d->drag_autoscroll ) {
		QPoint vp = ((QDragMoveEvent *) e)->pos();
		QRect inside_margin( autoscroll_margin, autoscroll_margin,
				     visibleWidth() - autoscroll_margin * 2,
				     visibleHeight() - autoscroll_margin * 2 );
		if ( !inside_margin.contains( vp ) ) {
		    startDragAutoScroll();
		    // Keep sending move events
		    ( (QDragMoveEvent*)e )->accept( QRect(0,0,0,0) );
		}
	    }
	    viewportDragMoveEvent( (QDragMoveEvent*)e );
	} break;
	case QEvent::DragLeave:
	    stopDragAutoScroll();
	    viewportDragLeaveEvent( (QDragLeaveEvent*)e );
	    break;
	case QEvent::Drop:
	    stopDragAutoScroll();
	    viewportDropEvent( (QDropEvent*)e );
	    break;
#endif // QT_NO_DRAGANDDROP
	case QEvent::Wheel:
	    viewportWheelEvent( (QWheelEvent*)e );
	    break;
	case QEvent::ChildRemoved:
	    removeChild((QWidget*)((QChildEvent*)e)->child());
	    break;
	case QEvent::LayoutHint:
	    d->autoResizeHint(this);
	    break;
	default:
	    break;
	}
    } else if ( d && d->rec((QWidget*)obj) ) {  // must be a child
	if ( e->type() == QEvent::Resize )
	    d->autoResize(this);
	else if ( e->type() == QEvent::Move )
	    d->autoMove(this);
    }
    return QFrame::eventFilter( obj, e );  // always continue with standard event processing
}

/*!
  This event handler is called whenever the QScrollView receives a
  mousePressEvent() - the press position is translated to be a
  point on the contents.
*/
void QScrollView::contentsMousePressEvent( QMouseEvent* )
{
}

/*!
  This event handler is called whenever the QScrollView receives a
  mouseReleaseEvent() - the release position is translated to be a
  point on the contents.
*/
void QScrollView::contentsMouseReleaseEvent( QMouseEvent* )
{
}

/*!
  This event handler is called whenever the QScrollView receives a
  mouseDoubleClickEvent() - the click position is translated to be a
  point on the contents.
*/
void QScrollView::contentsMouseDoubleClickEvent( QMouseEvent* )
{
}

/*!
  This event handler is called whenever the QScrollView receives a
  mouseMoveEvent() - the mouse position is translated to be a
  point on the contents.
*/
void QScrollView::contentsMouseMoveEvent( QMouseEvent* )
{
}

#ifndef QT_NO_DRAGANDDROP

/*!
  This event handler is called whenever the QScrollView receives a
  dragEnterEvent() - the drag position is translated to be a
  point on the contents.
*/
void QScrollView::contentsDragEnterEvent( QDragEnterEvent * )
{
}

/*!
  This event handler is called whenever the QScrollView receives a
  dragMoveEvent() - the drag position is translated to be a
  point on the contents.
*/
void QScrollView::contentsDragMoveEvent( QDragMoveEvent * )
{
}

/*!
  This event handler is called whenever the QScrollView receives a
  dragLeaveEvent() - the drag position is translated to be a
  point on the contents.
*/
void QScrollView::contentsDragLeaveEvent( QDragLeaveEvent * )
{
}

/*!
  This event handler is called whenever the QScrollView receives a
  dropEvent() - the drop position is translated to be a
  point on the contents.
*/
void QScrollView::contentsDropEvent( QDropEvent * )
{
}

#endif // QT_NO_DRAGANDDROP

/*!
  This event handler is called whenever the QScrollView receives a
  wheelEvent() - the mouse position is translated to be a
  point on the contents.
*/
void QScrollView::contentsWheelEvent( QWheelEvent * e )
{
    e->ignore();
}


/*!
  This is a low-level painting routine that draws the viewport
  contents.  Reimplement this if drawContents() is too high-level.
  (for example, if you don't want to open a QPainter on the viewport).
*/
void QScrollView::viewportPaintEvent( QPaintEvent* pe )
{
    QWidget* vp = viewport();
    QPainter p(vp);
    QRect r = pe->rect();
    if ( d->clipped_viewport ) {
	QRect rr(
	    -d->clipped_viewport->x(), -d->clipped_viewport->y(),
	    d->viewport.width(), d->viewport.height()
	);
	r &= rr;
	if ( r.isValid() ) {
	    int ex = r.x() + d->clipped_viewport->x() + contentsX();
	    int ey = r.y() + d->clipped_viewport->y() + contentsY();
	    int ew = r.width();
	    int eh = r.height();
	    drawContentsOffset(&p,
			       contentsX()+d->clipped_viewport->x(),
			       contentsY()+d->clipped_viewport->y(),
			       ex, ey, ew, eh);
	}
    } else {
	r &= d->viewport.rect();
	int ex = r.x() + contentsX();
	int ey = r.y() + contentsY();
	int ew = r.width();
	int eh = r.height();
	drawContentsOffset(&p, contentsX(), contentsY(), ex, ey, ew, eh);
    }
}


/*!
  To provide simple processing of events on the contents, this method
  receives all resize events sent to the viewport.

  \sa QWidget::resizeEvent()
*/
void QScrollView::viewportResizeEvent( QResizeEvent* )
{
}

/*!
  To provide simple processing of events on the contents, this method receives all mouse
  press events sent to the viewport.

  The default implementation translates the event and calls
  contentsMousePressEvent().

  \sa contentsMousePressEvent(), QWidget::mousePressEvent()
*/
void QScrollView::viewportMousePressEvent( QMouseEvent* e )
{
    QMouseEvent ce(e->type(), viewportToContents(e->pos()),
	e->globalPos(), e->button(), e->state());
    contentsMousePressEvent(&ce);
}

/*!
  To provide simple processing of events on the contents,
  this method receives all mouse
  release events sent to the viewport.

  The default implementation translates the event and calls
  contentsMouseReleaseEvent().

  \sa QWidget::mouseReleaseEvent()
*/
void QScrollView::viewportMouseReleaseEvent( QMouseEvent* e )
{
    QMouseEvent ce(e->type(), viewportToContents(e->pos()),
	e->globalPos(), e->button(), e->state());
    contentsMouseReleaseEvent(&ce);
}

/*!
  To provide simple processing of events on the contents,
  this method receives all mouse
  double click events sent to the viewport.

  The default implementation translates the event and calls
  contentsMouseDoubleClickEvent().

  \sa QWidget::mouseDoubleClickEvent()
*/
void QScrollView::viewportMouseDoubleClickEvent( QMouseEvent* e )
{
    QMouseEvent ce(e->type(), viewportToContents(e->pos()),
	e->globalPos(), e->button(), e->state());
    contentsMouseDoubleClickEvent(&ce);
}

/*!
  To provide simple processing of events on the contents,
  this method receives all mouse
  move events sent to the viewport.

  The default implementation translates the event and calls
  contentsMouseMoveEvent().

  \sa QWidget::mouseMoveEvent()
*/
void QScrollView::viewportMouseMoveEvent( QMouseEvent* e )
{
    QMouseEvent ce(e->type(), viewportToContents(e->pos()),
	e->globalPos(), e->button(), e->state());
    contentsMouseMoveEvent(&ce);
}

#ifndef QT_NO_DRAGANDDROP

/*!
  To provide simple processing of events on the contents,
  this method receives all drag enter
  events sent to the viewport.

  The default implementation translates the event and calls
  contentsDragEnterEvent().

  \sa QWidget::dragEnterEvent()
*/
void QScrollView::viewportDragEnterEvent( QDragEnterEvent* e )
{
    e->setPoint(viewportToContents(e->pos()));
    contentsDragEnterEvent(e);
    e->setPoint(contentsToViewport(e->pos()));
}

/*!
  To provide simple processing of events on the contents,
  this method receives all drag move
  events sent to the viewport.

  The default implementation translates the event and calls
  contentsDragMoveEvent().

  \sa QWidget::dragMoveEvent()
*/
void QScrollView::viewportDragMoveEvent( QDragMoveEvent* e )
{
    e->setPoint(viewportToContents(e->pos()));
    contentsDragMoveEvent(e);
    e->setPoint(contentsToViewport(e->pos()));
}

/*!
  To provide simple processing of events on the contents,
  this method receives all drag leave
  events sent to the viewport.

  The default implementation calls contentsDragLeaveEvent().

  \sa QWidget::dragLeaveEvent()
*/
void QScrollView::viewportDragLeaveEvent( QDragLeaveEvent* e )
{
    contentsDragLeaveEvent(e);
}

/*!
  To provide simple processing of events on the contents,
  this method receives all drop
  events sent to the viewport.

  The default implementation translates the event and calls
  contentsDropEvent().

  \sa QWidget::dropEvent()
*/
void QScrollView::viewportDropEvent( QDropEvent* e )
{
    e->setPoint(viewportToContents(e->pos()));
    contentsDropEvent(e);
    e->setPoint(contentsToViewport(e->pos()));
}

#endif // QT_NO_DRAGANDDROP

/*!
  To provide simple processing of events on the contents,
  this method receives all wheel
  events sent to the viewport.

  The default implementation translates the event and calls
  contentsWheelEvent().

  \sa QWidget::wheelEvent()
*/
void QScrollView::viewportWheelEvent( QWheelEvent* e )
{
    QWheelEvent ce( viewportToContents(e->pos()),
	e->globalPos(), e->delta(), e->state());
    contentsWheelEvent(&ce);
    if ( ce.isAccepted() )
	e->accept();
    else
	e->ignore();
}

/*!
 Returns the component horizontal scrollbar.  It is made available to allow
 accelerators, autoscrolling, etc., and to allow changing
 of arrow scroll rates: bar->setSteps( rate, bar->pageStep() ).

 It should not be otherwise manipulated.

 This function never returns 0.
*/
QScrollBar* QScrollView::horizontalScrollBar() const
{
    return &d->hbar;
}

/*!
 Returns the component vertical scrollbar.  It is made available to allow
 accelerators, autoscrolling, etc., and to allow changing
 of arrow scroll rates: bar->setSteps( rate, bar->pageStep() ).

 It should not be otherwise manipulated.

 This function never returns 0.
*/
QScrollBar* QScrollView::verticalScrollBar() const {
    return &d->vbar;
}


/*!
 Scrolls the content so that the point (x, y) is visible
 with at least 50-pixel margins (if possible, otherwise centered).
*/
void QScrollView::ensureVisible( int x, int y )
{
    ensureVisible(x, y, 50, 50);
}

/*!
 Scrolls the content so that the point (x, y) is visible
 with at least the given pixel margins (if possible, otherwise centered).
*/
void QScrollView::ensureVisible( int x, int y, int xmargin, int ymargin )
{
    int pw=visibleWidth();
    int ph=visibleHeight();

    int cx=-contentsX();
    int cy=-contentsY();
    int cw=contentsWidth();
    int ch=contentsHeight();

    if ( pw < xmargin*2 )
	xmargin=pw/2;
    if ( ph < ymargin*2 )
	ymargin=ph/2;

    if ( cw <= pw ) {
	xmargin=0;
	cx=0;
    }
    if ( ch <= ph ) {
	ymargin=0;
	cy=0;
    }

    if ( x < -cx+xmargin )
	cx = -x+xmargin;
    else if ( x >= -cx+pw-xmargin )
	cx = -x+pw-xmargin;

    if ( y < -cy+ymargin )
	cy = -y+ymargin;
    else if ( y >= -cy+ph-ymargin )
	cy = -y+ph-ymargin;

    if ( cx > 0 )
	cx=0;
    else if ( cx < pw-cw && cw>pw )
	cx=pw-cw;

    if ( cy > 0 )
	cy=0;
    else if ( cy < ph-ch && ch>ph )
	cy=ph-ch;

    setContentsPos( -cx, -cy );
}

/*!
 Scrolls the content so that the point (x, y) is in the top-left corner.
*/
void QScrollView::setContentsPos( int x, int y )
{
    if ( x < 0 ) x = 0;
    if ( y < 0 ) y = 0;
    // Choke signal handling while we update BOTH sliders.
    d->signal_choke=TRUE;
    moveContents( -x, -y );
    d->vbar.setValue( y );
    d->hbar.setValue( x );
//     updateScrollBars(); // ### warwick, why should we need that???
    d->signal_choke=FALSE;
//     updateScrollBars(); // ### warwick, why should we need that???
}

/*!
 Scrolls the content by \a x to the left and \a y upwards.
*/
void QScrollView::scrollBy( int dx, int dy )
{
    setContentsPos( contentsX()+dx, contentsY()+dy );
}

/*!
 Scrolls the content so that the point (x,y) is in the
 center of visible area.
*/
void QScrollView::center( int x, int y )
{
    ensureVisible( x, y, 32000, 32000 );
}

/*!
 Scrolls the content so that the point (x,y) is visible,
 with the given margins (as fractions of visible area).

 eg.
 <ul>
   <li>Margin 0.0 allows (x,y) to be on edge of visible area.
   <li>Margin 0.5 ensures (x,y) is in middle 50% of visible area.
   <li>Margin 1.0 ensures (x,y) is in the center of the visible area.
 </ul>
*/
void QScrollView::center( int x, int y, float xmargin, float ymargin )
{
    int pw=visibleWidth();
    int ph=visibleHeight();
    ensureVisible( x, y, int( xmargin/2.0*pw+0.5 ), int( ymargin/2.0*ph+0.5 ) );
}


/*!
  \fn void QScrollView::contentsMoving(int x, int y)

  This signal is emitted just before the contents is moved
  to the given position.

  \sa contentsX(), contentsY()
*/

/*!
  Moves the contents.
*/
void QScrollView::moveContents(int x, int y)
{
    if ( -x+visibleWidth() > contentsWidth() )
	x=QMIN(0,-contentsWidth()+visibleWidth());
    if ( -y+visibleHeight() > contentsHeight() )
	y=QMIN(0,-contentsHeight()+visibleHeight());

    int dx = x - d->vx;
    int dy = y - d->vy;

    if (!dx && !dy)
	return; // Nothing to do

    emit contentsMoving( -x, -y );

    d->vx = x;
    d->vy = y;

    if ( d->clipped_viewport || d->static_bg ) {
	// Cheap move (usually)
	d->moveAllBy(dx,dy);
    } else if ( /*dx && dy ||*/
	 ( QABS(dy) * 5 > visibleHeight() * 4 ) ||
	 ( QABS(dx) * 5 > visibleWidth() * 4 )
	)
    {
	// Big move
	if ( viewport()->isUpdatesEnabled() )
	    viewport()->update();
	d->moveAllBy(dx,dy);
    } else {
	// Small move
	clipper()->scroll(dx,dy);
    }
    d->hideOrShowAll(this, TRUE );
}

#if QT_VERSION >= 300
#error "Should rename contents{X,Y,Width,Height} to viewport{...}"
// Because it's the viewport rectangle that is "moving", not the contents.
#endif

#if QT_VERSION >= 300
#error "Should rename contents{X,Y,Width,Height} to viewport{...}"
// Because it's the viewport rectangle that is "moving", not the contents.
#endif

/*!
  Returns the X coordinate of the contents which is at the left
  edge of the viewport.
*/
int QScrollView::contentsX() const
{
    return -d->vx;
}

/*!
  Returns the Y coordinate of the contents which is at the top
  edge of the viewport.
*/
int QScrollView::contentsY() const
{
    return -d->vy;
}

/*!
  Returns the width of the contents area.
*/
int QScrollView::contentsWidth() const
{
    return d->vwidth;
}

/*!
  Returns the height of the contents area.
*/
int QScrollView::contentsHeight() const
{
    return d->vheight;
}

/*!
  Set the size of the contents area to \a w pixels wide and \a h
  pixels high, and updates the viewport accordingly.
*/
void QScrollView::resizeContents( int w, int h )
{
    int ow = d->vwidth;
    int oh = d->vheight;
    d->vwidth = w;
    d->vheight = h;

    // Could more efficiently scroll if shrinking, repaint if growing, etc.
    updateScrollBars();

    if ( d->children.isEmpty() && d->policy == Default )
	setResizePolicy( Manual );

    if ( ow > w ) {
	// Swap
	int t=w;
	w=ow;
	ow=t;
    }
    // Refresh area ow..w
    if ( ow < visibleWidth() && w >= 0 ) {
	if ( ow < 0 )
	    ow = 0;
	if ( w > visibleWidth() )
	    w = visibleWidth();
	clipper()->update( contentsX()+ow, 0, w-ow, visibleHeight() );
    }

    if ( oh > h ) {
	// Swap
	int t=h;
	h=oh;
	oh=t;
    }
    // Refresh area oh..h
    if ( oh < visibleHeight() && h >= 0 ) {
	if ( oh < 0 )
	    oh = 0;
	if ( h > visibleHeight() )
	    h = visibleHeight();
	clipper()->update( 0, contentsY()+oh, visibleWidth(), h-oh);
    }
}

/*!
  Calls update() on rectangle defined by \a x, \a y, \a w, \a h,
  translated appropriately.  If the rectangle in not visible,
  nothing is repainted.

  \sa repaintContents()
*/
void QScrollView::updateContents( int x, int y, int w, int h )
{
    QWidget* vp = viewport();

    // Translate
    x -= contentsX();
    y -= contentsY();

    // Clip to QCOORD space
    if ( x < 0 ) {
	w += x;
	x = 0;
    }
    if ( y < 0 ) {
	h += y;
	y = 0;
    }

    if ( w < 0 || h < 0 )
	return;
    if ( w > visibleWidth() )
	w = visibleWidth();
    if ( h > visibleHeight() )
	h = visibleHeight();

    if ( d->clipped_viewport ) {
	// Translate clipper() to viewport()
	x -= d->clipped_viewport->x();
	y -= d->clipped_viewport->y();
    }

    vp->update( x, y, w, h );
}

/*!
  \overload
*/
void QScrollView::updateContents( const QRect& r )
{
    updateContents(r.x(), r.y(), r.width(), r.height());
}

/*!
  \overload
*/
void QScrollView::repaintContents( const QRect& r, bool erase )
{
    repaintContents(r.x(), r.y(), r.width(), r.height(), erase);
}



/*!
  Calls repaint() on rectangle defined by \a x, \a y, \a w, \a h,
  translated appropriately.  If the rectangle in not visible,
  nothing is repainted.

  \sa updateContents()
*/
void QScrollView::repaintContents( int x, int y, int w, int h, bool erase )
{
    QWidget* vp = viewport();

    // Translate logical to clipper()
    x -= contentsX();
    y -= contentsY();

    // Clip to QCOORD space
    if ( x < 0 ) {
	w += x;
	x = 0;
    }
    if ( y < 0 ) {
	h += y;
	y = 0;
    }

    if ( w < 0 || h < 0 )
	return;
    if ( w > visibleWidth() )
	w = visibleWidth();
    if ( h > visibleHeight() )
	h = visibleHeight();

    if ( d->clipped_viewport ) {
	// Translate clipper() to viewport()
	x -= d->clipped_viewport->x();
	y -= d->clipped_viewport->y();
    }

    vp->repaint( x, y, w, h, erase );
}


/*!
  For backward compatibility only.
  It is easier to use drawContents(QPainter*,int,int,int,int).

  The default implementation translates the painter appropriately
  and calls drawContents(QPainter*,int,int,int,int).
*/
void QScrollView::drawContentsOffset(QPainter* p, int offsetx, int offsety, int clipx, int clipy, int clipw, int cliph)
{
    p->translate(-offsetx,-offsety);
    drawContents(p, clipx, clipy, clipw, cliph);
}

/*!
  \fn void QScrollView::drawContents(QPainter* p, int clipx, int clipy, int clipw, int cliph)

  Reimplement this method if you are viewing a drawing area rather
  than a widget.

  The function should draw the rectangle (\a clipx, \a clipy, \a clipw, \a
  cliph ) of the contents, using painter \a p.  The clip rectangle is
  in the scroll views's coordinates.

  For example:
  \code
  {
    // Fill a 40000 by 50000 rectangle at (100000,150000)

    // Calculate the coordinates...
    int x1 = 100000, y1 = 150000;
    int x2 = x1+40000-1, y2 = y1+50000-1;

    // Clip the coordinates so X/Windows will not have problems...
    if (x1 < clipx) x1=clipx;
    if (y1 < clipy) y1=clipy;
    if (x2 > clipx+clipw-1) x2=clipx+clipw-1;
    if (y2 > clipy+cliph-1) y2=clipy+cliph-1;

    // Paint using the small coordinates...
    if ( x2 >= x1 && y2 >= y1 )
	p->fillRect(x1, y1, x2-x1+1, y2-y1+1, red);
  }
  \endcode

  The clip rectangle and translation of the painter \a p is already set
  appropriately.
*/
void QScrollView::drawContents(QPainter*, int, int, int, int)
{
}


/*!
  \reimp
*/
void QScrollView::frameChanged()
{
    // Ensures that scrollbars have the correct size when the frame
    // style changes.
    updateScrollBars();
}


/*!
  Returns the viewport widget of the scrollview.  This is the widget
  containing the contents widget or which is the drawing area.
*/
QWidget* QScrollView::viewport() const
{
    return d->clipped_viewport ? d->clipped_viewport : &d->viewport;
}

/*!
  Returns the clipper widget.
  Contents in the scrollview is ultimately clipped to be inside
  the clipper widget.

  You should not need to access this.

  \sa visibleWidth(), visibleHeight()
*/
QWidget* QScrollView::clipper() const
{
    return &d->viewport;
}

/*!
  Returns the horizontal amount of the content that is visible.
*/
int QScrollView::visibleWidth() const
{
    return clipper()->width();
}

/*!
  Returns the vertical amount of the content that is visible.
*/
int QScrollView::visibleHeight() const
{
    return clipper()->height();
}


void QScrollView::changeFrameRect(const QRect& r)
{
    QRect oldr = frameRect();
    if (oldr != r) {
	QRect cr = contentsRect();
	QRegion fr( frameRect() );
	fr = fr.subtract( contentsRect() );
	setFrameRect( r );
	if ( isVisible() ) {
	    cr = cr.intersect( contentsRect() );
	    fr = fr.unite( frameRect() );
	    fr = fr.subtract( cr );
	    if ( !fr.isEmpty() )
		QApplication::postEvent( this, new QPaintEvent( fr, FALSE ) );
	}
    }
}


/*!
  Sets the margins around the scrolling area.  This is useful for
  applications such as spreadsheets with `locked' rows and columns.
  The marginal space is \e inside the frameRect() and is left blank -
  reimplement drawContents() or put widgets in the unused area.

  By default all margins are zero.

  \sa frameChanged()
*/
void QScrollView::setMargins(int left, int top, int right, int bottom)
{
    if ( left == d->l_marg &&
	 top == d->t_marg &&
	 right == d->r_marg &&
	 bottom == d->b_marg )
	return;

    d->l_marg = left;
    d->t_marg = top;
    d->r_marg = right;
    d->b_marg = bottom;
    updateScrollBars();
}


/*!
  Returns the current left margin.
  \sa setMargins()
*/
int QScrollView::leftMargin() const
{
    return d->l_marg;
}


/*!
  Returns the current top margin.
  \sa setMargins()
*/
int QScrollView::topMargin() const
{
    return d->t_marg;
}


/*!
  Returns the current right margin.
  \sa setMargins()
*/
int QScrollView::rightMargin() const
{
    return d->r_marg;
}


/*!
  Returns the current bottom margin.
  \sa setMargins()
*/
int QScrollView::bottomMargin() const
{
    return d->b_marg;
}

/*!
  \reimp
*/
bool QScrollView::focusNextPrevChild( bool next )
{
    //  Makes sure that the new focus widget is on-screen, if
    //  necessary by scrolling the scroll view.

    // first set things up for the scan
    QFocusData *f = focusData();
    QWidget *startingPoint = f->home();
    QWidget *candidate = 0;
    QWidget *w = next ? f->next() : f->prev();
    QSVChildRec *r;

    // then scan for a possible focus widget candidate
    while( !candidate && w != startingPoint ) {
	if ( w != startingPoint &&
	     (w->focusPolicy() & TabFocus) == TabFocus
	     && w->isEnabled() &&!w->focusProxy() && w->isVisible() )
	    candidate = w;
	w = next ? f->next() : f->prev();
    }

    // if we could not find one, maybe super or parentWidget() can?
    if ( !candidate )
	return QFrame::focusNextPrevChild( next );

    // we've found one.
    r = d->ancestorRec( candidate );
    if ( r && ( r->child == candidate ||
		candidate->isVisibleTo( r->child ) ) ) {
	QPoint cp = r->child->mapToGlobal(QPoint(0,0));
	QPoint cr = candidate->mapToGlobal(QPoint(0,0)) - cp;
	ensureVisible( r->x+cr.x()+candidate->width()/2,
		       r->y+cr.y()+candidate->height()/2,
		       candidate->width()/2,
		       candidate->height()/2 );
    }

    candidate->setFocus();
    return TRUE;
}



/*!
  When large numbers of child widgets are in a scrollview, especially
  if they are close together, the scrolling performance can suffer
  greatly.  If you call enableClipper(TRUE), the scrollview will
  use an extra widget to group child widgets.

  Note that you may only call enableClipper() prior to adding widgets.

  For a full discussion, see the overview documentation of this
  class.
*/
void QScrollView::enableClipper(bool y)
{
    if ( !d->clipped_viewport == !y )
	return;
    if ( d->children.count() )
	qFatal("May only call QScrollView::enableClipper() before adding widgets");
    if ( y ) {
	d->clipped_viewport = new QClipperWidget(clipper(), "qt_clipped_viewport", d->flags);
	d->clipped_viewport->setGeometry(-coord_limit/2,-coord_limit/2,
					coord_limit,coord_limit);
	d->viewport.setBackgroundMode(NoBackground); // no exposures for this
	d->viewport.removeEventFilter( this );
	d->clipped_viewport->installEventFilter( this );
    } else {
	delete d->clipped_viewport;
	d->clipped_viewport = 0;
    }
}

/*!
  Sets the scrollview to have a static background if \a y is TRUE, or a scrolling background otherwise. By default,
  the background is scrolling.

  Beware that this mode is quite slow, as a full repaint of the visible area has to be triggered on every contents move.

  \sa hasStaticBackground()
*/
void  QScrollView::setStaticBackground(bool y)
{
    d->static_bg = y;
}

/*!
  Returns wether QScrollView uses a static background.
  \sa setStaticBackground()
*/
bool QScrollView::hasStaticBackground() const
{
    return d->static_bg;
}

/*!
  Returns the
    point \a p
  translated to
    a point on the viewport() widget.
*/
//### make this const in 3.0
QPoint QScrollView::contentsToViewport(const QPoint& p)
{
    if ( d->clipped_viewport ) {
	return QPoint( p.x() - contentsX() - d->clipped_viewport->x(),
		       p.y() - contentsY() - d->clipped_viewport->y() );
    } else {
	return QPoint( p.x() - contentsX(),
		       p.y() - contentsY() );
    }
}

/*!
  Returns the
    point on the viewport \a vp
  translated to
    a point in the contents.
*/
//### make this const in 3.0
QPoint QScrollView::viewportToContents(const QPoint& vp)
{
    if ( d->clipped_viewport ) {
	return QPoint( vp.x() + contentsX() + d->clipped_viewport->x(),
		       vp.y() + contentsY() + d->clipped_viewport->y() );
    } else {
	return QPoint( vp.x() + contentsX(),
		       vp.y() + contentsY() );
    }
}


/*!
  Translates
    a point (\a x, \a y) in the contents
  to
    a point (\a vx, \a vy) on the viewport() widget.
*/
void QScrollView::contentsToViewport(int x, int y, int& vx, int& vy)
{
    const QPoint v = contentsToViewport(QPoint(x,y));
    vx = v.x();
    vy = v.y();
}

/*!
  Translates
    a point (\a vx, \a vy) on the viewport() widget
  to
    a point (\a x, \a y) in the contents.
*/
void QScrollView::viewportToContents(int vx, int vy, int& x, int& y)
{
    const QPoint c = viewportToContents(QPoint(vx,vy));
    x = c.x();
    y = c.y();
}


/*!
  \reimp
*/
QSizePolicy QScrollView::sizePolicy() const
{
    //### removeme 3.0
    return QWidget::sizePolicy();
}


/*!
  \reimp
*/
QSize QScrollView::sizeHint() const
{
    constPolish();
    QSize result = QSize(frameWidth()*2, frameWidth()*2);
    if ( d->policy > Manual ) {
	QSVChildRec* r = d->children.first();
	if (r)
	{
            QSize cs = r->child->sizeHint();
	    if ( cs.isValid() )
        	result += cs.boundedTo( r->child->maximumSize() );
	    else
        	result += r->child->size();
        }
    } else {
	result += QSize(contentsWidth(),contentsHeight());
    }
    return result;
}


/*!
  \reimp
*/
QSize QScrollView::minimumSizeHint() const
{
    return QSize(100+frameWidth()*2,
		 100+frameWidth()*2);
}


/*!
  \reimp

  (Implemented to get rid of a compiler warning.)
*/
void QScrollView::drawContents( QPainter * )
{
}

#ifndef QT_NO_DRAGANDDROP

/*!
  \internal
*/
void QScrollView::startDragAutoScroll()
{
    if ( !d->autoscroll_timer.isActive() ) {
	d->autoscroll_time = initialScrollTime;
	d->autoscroll_accel = initialScrollAccel;
	d->autoscroll_timer.start( d->autoscroll_time );
    }
}


/*!
  \internal
*/
void QScrollView::stopDragAutoScroll()
{
    d->autoscroll_timer.stop();
}


/*!
  \internal
*/
void QScrollView::doDragAutoScroll()
{
    QPoint p = viewport()->mapFromGlobal( QCursor::pos() );

    if ( d->autoscroll_accel-- <= 0 && d->autoscroll_time ) {
	d->autoscroll_accel = initialScrollAccel;
	d->autoscroll_time--;
	d->autoscroll_timer.start( d->autoscroll_time );
    }
    int l = QMAX( 1, ( initialScrollTime- d->autoscroll_time ) );

    int dx = 0, dy = 0;
    if ( p.y() < autoscroll_margin ) {
	dy = -l;
    } else if ( p.y() > visibleHeight() - autoscroll_margin ) {
	dy = +l;
    }
    if ( p.x() < autoscroll_margin ) {
	dx = -l;
    } else if ( p.x() > visibleWidth() - autoscroll_margin ) {
	dx = +l;
    }
    if ( dx || dy ) {
	scrollBy(dx,dy);
    } else {
	stopDragAutoScroll();
    }
}


/*!
  If \a b is set to TRUE, the QScrollView automatically scrolls the contents
  in drag move events if the user moves the cursor close to a border of the
  view. This of course only works id the viewport accepts drops!
  Specifying FALSE here disables this autoscroll feature.
*/

void QScrollView::setDragAutoScroll( bool b )
{
    d->drag_autoscroll = b;
}


/*!
  Returns TRUE if autoscrolling in drag move events is enabled, else
  FALSE.

  \sa setDragAutoScroll()
*/

bool QScrollView::dragAutoScroll() const
{
    return d->drag_autoscroll;
}

#endif // QT_NO_DRAGANDDROP

#endif // QT_NO_SCROLLVIEW
