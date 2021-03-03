/****************************************************************************
** $Id: qt/src/widgets/qslider.cpp   2.3.2   edited 2001-06-26 $
**
** Implementation of QSlider class
**
** Created : 961019
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

#include "qslider.h"
#ifndef QT_NO_SLIDER
#include "qpainter.h"
#include "qdrawutil.h"
#include "qtimer.h"
#include "qbitmap.h"
#include "qapplication.h"

static const int motifBorder = 2;
static const int thresholdTime = 500;
static const int repeatTime    = 100;

static const bool funnyWindowsStyle = FALSE;

static int sliderStartVal = 0; //##### class member?


// NOT REVISED
/*!
  \class QSlider qslider.h
  \brief The QSlider widget provides a vertical or horizontal slider.
  \ingroup basic

  The slider is the classic widget for controlling a bounded value.
  It lets the user move a slider along a horizontal or vertical
  groove, and translates the slider's position into an integer value
  in the legal range.

  QSlider inherits QRangeControl, which provides the "integer" side of
  the slider.  setRange() and value() are likely to be used by
  practically all slider users; see the \l QRangeControl documentation
  for information about the many other functions that class provides.

  The main functions offered by the slider itself are tickmark and
  orientation control; you can use setTickmarks() to indicate where
  you want the tickmarks to be, setTickInterval() to indicate how many
  of them you want, and setOrientation() to indicate whether the
  slider is to e horizontal or vertical.

  A slider has a default focusPolicy() of \a WeakWheelFocus, i.e. it
  accepts focus on Tab and by using the mouse wheel, and a
  suitable keyboard interface.

  <img src=qslider-m.png> <img src=qslider-w.png>

  \sa QScrollBar QSpinBox
  <a href="guibooks.html#fowler">GUI Design Handbook: Slider</a>
*/


/*! \enum QSlider::TickSetting

  This enum specifies where the tick marks are to be drawn, relative
  to the slider's groove and the handle the user moves.  The possible
  values are \c NoMarks (no tickmarks are drawn), \c Above, \c Below,
  \c Left, \c Right and \c Both.

  \c NoMarks means to not draw any tickmarks; \c Both means to draw
  tickmarks on both sides of the groove.  \c Above and \c Below mean
  to draw tickmarks above and below the (horizontal) slider.  \c Left
  and \c Right mean to draw tickmarks to the left and right of the
  (vertical) slider.
*/


/*!
  Constructs a vertical slider.

  The \e parent and \e name arguments are sent to the QWidget constructor.
*/

QSlider::QSlider( QWidget *parent, const char *name )
    : QWidget( parent, name  )
{
    orient = Vertical;
    init();
}

/*!
  Constructs a slider.

  The \e orientation must be QSlider::Vertical or QSlider::Horizontal.

  The \e parent and \e name arguments are sent to the QWidget constructor.
*/

QSlider::QSlider( Orientation orientation, QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    orient = orientation;
    init();
}

/*!
  Constructs a slider.

  \arg \e minValue is the minimum slider value.
  \arg \e maxValue is the maximum slider value.
  \arg \e step is the page step value.
  \arg \e value is the initial value.
  \arg \e orientation must be QSlider::Vertical or QSlider::Horizontal.

  The \e parent and \e name arguments are sent to the QWidget constructor.
*/

QSlider::QSlider( int minValue, int maxValue, int pageStep,
		  int value, Orientation orientation,
		  QWidget *parent, const char *name )
    : QWidget( parent, name ),
      QRangeControl( minValue, maxValue, 1, pageStep, value )
{
    orient = orientation;
    init();
    sliderVal = value;
}


void QSlider::init()
{
    extra = 0;
    timer = 0;
    sliderPos = 0;
    sliderVal = 0;
    clickOffset = 0;
    state = Idle;
    track = TRUE;
    ticks = NoMarks;
    tickInt = 0;
    setFocusPolicy( TabFocus  );
    initTicks();
}


/*!
  Does what's needed when someone changes the tickmark status
*/

void QSlider::initTicks()
{
    int space = (orient == Horizontal) ? height() : width();
    if ( ticks == Both ) {
	tickOffset = ( space - thickness() ) / 2;
    } else if ( ticks == Above ) {
	tickOffset = space - thickness();
    } else {
	tickOffset = 0;
    }
}


/*!
  Enables slider tracking if \e enable is TRUE, or disables tracking
  if \e enable is FALSE.

  If tracking is enabled (default), the slider emits the
  valueChanged() signal whenever the slider is being dragged.  If
  tracking is disabled, the slider emits the valueChanged() signal
  when the user releases the mouse button (unless the value happens to
  be the same as before).

  \sa tracking()
*/

void QSlider::setTracking( bool enable )
{
    track = enable;
}


/*!
  \fn bool QSlider::tracking() const
  Returns TRUE if tracking is enabled, or FALSE if tracking is disabled.

  Tracking is initially enabled.

  \sa setTracking()
*/


/*!
  \fn void QSlider::valueChanged( int value )
  This signal is emitted when the slider value is changed, with the
  new slider value as an argument.
*/

/*!
  \fn void QSlider::sliderPressed()
  This signal is emitted when the user presses the slider with the mouse.
*/

/*!
  \fn void QSlider::sliderMoved( int value )
  This signal is emitted when the slider is dragged, with the
  new slider value as an argument.
*/

/*!
  \fn void QSlider::sliderReleased()
  This signal is emitted when the user releases the slider with the mouse.
*/

/*!
  Calculates slider position corresponding to value \a v.
*/

int QSlider::positionFromValue( int v ) const
{
    int  a = available();
    return QRangeControl::positionFromValue( v, a );
}

/*!
  Returns the available space in which the slider can move.
*/

int QSlider::available() const
{
    int a;
    switch ( (GUIStyle)style() ) {
    case WindowsStyle:
	a = (orient == Horizontal) ? width() - style().sliderLength()
	    : height() - style().sliderLength();
	break;
    default:
    case MotifStyle:
	a = (orient == Horizontal) ? width() -style().sliderLength() - 2*motifBorder
	    : height() - style().sliderLength() - 2*motifBorder;
	break;
    }
    return a;
}

/*!
  Calculates value corresponding to slider position \a p.
*/

int QSlider::valueFromPosition( int p ) const
{
    int a = available();
    return QRangeControl::valueFromPosition( p, a );

}

/*!
  Implements the virtual QRangeControl function.
*/

void QSlider::rangeChange()
{
    int newPos = positionFromValue( value() );
    if ( newPos != sliderPos ) {
	reallyMoveSlider( newPos );
    }
}

/*!
  Implements the virtual QRangeControl function.
*/

void QSlider::valueChange()
{
    if ( sliderVal != value() ) {
	int newPos = positionFromValue( value() );
	sliderVal = value();
	reallyMoveSlider( newPos );
    }
    emit valueChanged(value());
}


/*!\reimp
*/
void QSlider::resizeEvent( QResizeEvent * )
{
    rangeChange();
    initTicks();
    if ( autoMask() )
	updateMask();
}


/*!
  Reimplements the virtual function QWidget::setPalette().

  Sets the background color to the mid color for Motif style sliders.
*/

void QSlider::setPalette( const QPalette &p )
{
    QWidget::setPalette( p );
}



/*!
  Sets the slider orientation.  The \e orientation must be
  QSlider::Vertical or QSlider::Horizontal.
  \sa orientation()
*/

void QSlider::setOrientation( Orientation orientation )
{
    orient = orientation;
    rangeChange();
    update();
}


/*!
  \fn Orientation QSlider::orientation() const
  Returns the slider orientation; QSlider::Vertical or
  QSlider::Horizontal.
  \sa setOrientation()
*/


/*!
  Returns the slider handle rectangle. (The actual moving-around thing.)
*/

QRect QSlider::sliderRect() const
{
    QRect r;
    switch ( (GUIStyle)style() ) {
    case WindowsStyle:
	if (orient == Horizontal )
	    r.setRect( sliderPos, tickOffset,
		       style().sliderLength(), thickness()  );
	else
	    r.setRect ( tickOffset, sliderPos,
			thickness(), style().sliderLength()  );
	break;
    default:
    case MotifStyle:
	if (orient == Horizontal )
	    r.setRect ( sliderPos + motifBorder, tickOffset + motifBorder,
			style().sliderLength(), thickness() - 2 * motifBorder );
	else
	    r.setRect ( tickOffset + motifBorder, sliderPos + motifBorder,
			thickness() - 2 * motifBorder, style().sliderLength() );
	break;
    }
    return r;
}


/*!
  Paints the slider button using painter \a p with size, a colorgroup
  and position given by \a r. Reimplement this function to change the
  look of the slider button.

  Setting the colorgroup is useful to reuse the code to draw a mask if
  the slider supports transparceny.

  \sa setAutoMask(), updateMask()
*/

void QSlider::paintSlider( QPainter *p, const QColorGroup &g, const QRect &r )
{ //####### should this one be removed? private? non-virtual?
    QPoint bo = p->brushOrigin();
    p->setBrushOrigin(r.topLeft());

    style().drawSlider( p, r.x(), r.y(), r.width(), r.height(), g, orient,
			ticks & Above, ticks & Below );
    p->setBrushOrigin(bo);
}

/*!
  Performs the actual moving of the slider.
*/

void QSlider::reallyMoveSlider( int newPos )
{
    QRect oldR = sliderRect();
    sliderPos = newPos;
    QRect newR = sliderRect();
    //since sliderRect isn't virtual, I know that oldR and newR
    // are the same size.
    if ( orient == Horizontal ) {
	if ( oldR.left() < newR.left() )
	    oldR.setRight( QMIN ( oldR.right(), newR.left()));
	else           //oldR.right() >= newR.right()
	    oldR.setLeft( QMAX ( oldR.left(), newR.right()));
    } else {
	if ( oldR.top() < newR.top() )
	    oldR.setBottom( QMIN ( oldR.bottom(), newR.top()));
	else           //oldR.bottom() >= newR.bottom()
	    oldR.setTop( QMAX ( oldR.top(), newR.bottom()));
    }
    repaint( oldR );
    repaint( newR, FALSE );
    if ( autoMask() )
	updateMask();
}

/*!\obsolete

  Draws the "groove" on which the slider moves, using the painter \a p.
  \a c gives the distance from the top (or left) edge of the widget to
  the center of the groove.
*/

void QSlider::drawWinGroove( QPainter *p, QCOORD c )
{
    if ( orient == Horizontal ) {
	qDrawWinPanel( p, 0, c - 2,  width(), 4, colorGroup(), TRUE );
	p->setPen( colorGroup().foreground() );
	p->drawLine( 1, c - 1, width() - 3, c - 1 );
    } else {
	qDrawWinPanel( p, c - 2, 0, 4, height(), colorGroup(), TRUE );
	p->setPen( colorGroup().foreground() );
	p->drawLine( c - 1, 1, c - 1, height() - 3 );
    }
}


/*!\reimp
*/
void QSlider::paintEvent( QPaintEvent * )
{

    QPainter p( this );
    const QRect & sliderR = sliderRect();
    const QColorGroup & g = colorGroup();
    int mid = thickness()/2;
    if ( ticks & Above )
	mid += style().sliderLength() / 8;
    if ( ticks & Below )
	mid -= style().sliderLength() / 8;
    if ( orient == Horizontal ) {
	style().drawSliderGroove(&p, 0, tickOffset, width(), thickness(),
				     g, mid, Horizontal );
// 	    p.fillRect( 0, 0, width(), tickOffset, g.background() );
// 	    p.fillRect( 0, tickOffset + thickness(),
// 			width(), height()/*###*/, g.background() );
	erase( 0, 0, width(), tickOffset );
	erase( 0, tickOffset + thickness(), width(), height() );
    }
    else {
	style().drawSliderGroove( &p, tickOffset, 0, thickness(), height(),
				      g, mid, Vertical );
// 	    p.fillRect( 0, 0,  tickOffset, height(), g.background() );
// 	    p.fillRect( tickOffset + thickness(), 0,
// 			width()/*###*/, height(), g.background() );
	erase( 0, 0,  tickOffset, height() );
	erase( tickOffset + thickness(), 0, width()/*###*/, height() );
    }

    int interval = tickInt;
    if ( interval <= 0 ) {
	interval = lineStep();
	if ( positionFromValue( interval ) - positionFromValue( 0 ) < 3 )
	    interval = pageStep();
    }
    if ( ticks & Above )
	drawTicks( &p, g, 0, tickOffset - 2, interval );

    if ( ticks & Below ) {
	int avail = (orient == Horizontal) ? height() : width();
	avail -= tickOffset + thickness();
	drawTicks( &p, g, tickOffset + thickness() + 1, avail - 2, interval );
    }
    if ( hasFocus() ) {
	QRect r;
	if ( orient == Horizontal )
	    r.setRect( 0, tickOffset-1, width(), thickness()+2 );
	else
	    r.setRect( tickOffset-1, 0, thickness()+2, height() );
	r = r.intersect( rect() );
	if (style() == MotifStyle)
	    style().drawFocusRect(&p, QRect(r.x()+1, r.y()+1, r.width()-2, r.height()-2), g);
	else
	    style().drawFocusRect(&p, r, g);
    }
    paintSlider( &p, g, sliderR );

}


/*!

 Reimplementation of QWidget::updateMask(). Draws the mask of the
 slider when transparency is required.

 \sa QWidget::setAutoMask()
*/
void QSlider::updateMask()
{
    QBitmap bm( size() );
    bm.fill( color0 );

    {
	QPainter p( &bm, this );
	QRect sliderR = sliderRect();
	QColorGroup g(color1, color1, color1, color1, color1, color1, color1, color1, color0);
	int mid = tickOffset + thickness()/2;
	if ( ticks & Above )
	    mid += style().sliderLength() / 8;
	if ( ticks & Below )
	    mid -= style().sliderLength() / 8;
	if ( orient == Horizontal ) {
	    style().drawSliderGrooveMask(&p, 0, tickOffset, width(), thickness(),
					 mid, Horizontal );
	}
	else {
	    style().drawSliderGrooveMask( &p, tickOffset, 0, thickness(), height(),
					  mid, Vertical );
	}
	style().drawSliderMask( &p, sliderR.x(), sliderR.y(),
				sliderR.width(), sliderR.height(),
				orient, ticks & Above, ticks & Below );

	int interval = tickInt;
	if ( interval <= 0 ) {
	    interval = lineStep();
	    if ( positionFromValue( interval ) - positionFromValue( 0 ) < 3 )
		interval = pageStep();
	}
	if ( ticks & Above )
	    drawTicks( &p, g, 0, tickOffset - 2, interval );

	if ( ticks & Below ) {
	    int avail = (orient == Horizontal) ? height() : width();
	    avail -= tickOffset + thickness();
	    drawTicks( &p, g, tickOffset + thickness() + 1, avail - 2, interval );
	}

    }
    setMask( bm );
}

/*!\reimp
*/
void QSlider::mousePressEvent( QMouseEvent *e )
{
    resetState();
    sliderStartVal = sliderVal;
    QRect r = sliderRect();

    if ( e->button() == RightButton ) {
	return;
    } else if ( r.contains( e->pos() ) ) {
	state = Dragging;
	clickOffset = (QCOORD)( goodPart( e->pos() ) - sliderPos );
	emit sliderPressed();
    } else if ( e->button() == MidButton ||
		(funnyWindowsStyle && style() == WindowsStyle) ) {
	int pos = goodPart( e->pos() );
	moveSlider( pos - slideLength() / 2 );
	state = Dragging;
	clickOffset = slideLength() / 2;
    } else if ( orient == Horizontal && e->pos().x() < r.left() //### goodPart
		|| orient == Vertical && e->pos().y() < r.top() ) {
	state = TimingDown;
	subtractPage();
	if ( !timer )
	    timer = new QTimer( this );
	connect( timer, SIGNAL(timeout()), SLOT(repeatTimeout()) );
	timer->start( thresholdTime, TRUE );
    } else if ( orient == Horizontal && e->pos().x() > r.right() //### goodPart
		|| orient == Vertical && e->pos().y() > r.bottom() ) {
	state = TimingUp;
	addPage();
	if ( !timer )
	    timer = new QTimer( this );
	connect( timer, SIGNAL(timeout()), SLOT(repeatTimeout()) );
	timer->start( thresholdTime, TRUE );
    }
}

/*!\reimp
*/
void QSlider::mouseMoveEvent( QMouseEvent *e )
{
    if ( state != Dragging )
	return;

    if ( style() == WindowsStyle ) {
	QRect r = rect();
	int m = style().maximumSliderDragDistance();
	if ( m >= 0 ) {
	    if ( orientation() == Horizontal )
		r.setRect( r.x() - m, r.y() - 2*m/3,
			   r.width() + 2*m, r.height() + 3*m );
	    else
		r.setRect( r.x() - 2*m/3, r.y() - m,
			   r.width() + 3*m, r.height() + 2*m );
	    if ( !r.contains( e->pos() ) ) {
		moveSlider( positionFromValue( sliderStartVal) );
		return;
	    }
	}
    }

    int pos = goodPart( e->pos() );
    moveSlider( pos - clickOffset );
}

/*!\reimp
*/
void QSlider::wheelEvent( QWheelEvent * e){
    static float offset = 0;
    static QSlider* offset_owner = 0;
    if (offset_owner != this){
	offset_owner = this;
	offset = 0;
    }
    offset += -e->delta()*QMAX(pageStep(),lineStep())/120;
    if (QABS(offset)<1)
	return;
    setValue( value() + int(offset) );
    offset -= int(offset);
    e->accept();
}


/*!\reimp
*/
void QSlider::mouseReleaseEvent( QMouseEvent * )
{
    resetState();
}

/*!\reimp
*/
void QSlider::focusInEvent( QFocusEvent * e)
{
    QWidget::focusInEvent( e );
}

/*!\reimp
*/
void QSlider::focusOutEvent( QFocusEvent * e )
{
    QWidget::focusOutEvent( e );
}

/*!
  Moves the left (or top) edge of the slider to position
  \a pos. Performs snapping.
*/

void QSlider::moveSlider( int pos )
{
    int  a = available();
    int newPos = QMIN( a, QMAX( 0, pos ) );
    int newVal = valueFromPosition( newPos );
    if ( sliderVal != newVal ) {
	sliderVal = newVal;
	emit sliderMoved( sliderVal );
    }
    if ( tracking() && sliderVal != value() ) {
	setValue( sliderVal );
	// ### Why do we emit the valueChanged signal here?  It will get emitted in 
	// valueChange() anyway...
	//emit valueChanged( sliderVal );
    }

    switch ( (GUIStyle)style() ) {
    case WindowsStyle:
	newPos = positionFromValue( newVal );
	break;
    default:
    case MotifStyle:
	break;
    }

    if ( sliderPos != newPos )
	reallyMoveSlider( newPos );
}


/*!
  Resets all state information and stops my timer.
*/

void QSlider::resetState()
{
    if ( timer ) {
	timer->stop();
	timer->disconnect();
    }
    switch ( state ) {
    case TimingUp:
    case TimingDown:
	break;
    case Dragging: {
	setValue( valueFromPosition( sliderPos ) );
	emit sliderReleased();
	break;
    }
    case Idle:
	break;
    default:
	qWarning("QSlider: (%s) in wrong state", name( "unnamed" ) );
    }
    state = Idle;
}


/*!\reimp
*/
void QSlider::keyPressEvent( QKeyEvent *e )
{
    bool sloppy = ( style() == MotifStyle );
    switch ( e->key() ) {
    case Key_Left:
	if ( sloppy || orient == Horizontal )
	    subtractLine();
	break;
    case Key_Right:
	if ( sloppy || orient == Horizontal )
	    addLine();
	break;
    case Key_Up:
	if ( sloppy || orient == Vertical )
	    subtractLine();
	break;
    case Key_Down:
	if ( sloppy || orient == Vertical )
	    addLine();
	break;
    case Key_Prior:
	subtractPage();
	break;
    case Key_Next:
	addPage();
	break;
    case Key_Home:
	setValue( minValue() );
	break;
    case Key_End:
	setValue( maxValue() );
	break;
    default:
	e->ignore();
	return;
    }
}


/*!
  Returns the length of the slider.
*/

int QSlider::slideLength() const
{
    switch ( (GUIStyle)style() ) {
    case WindowsStyle:
	return style().sliderLength();
    default:
    case MotifStyle:
	return style().sliderLength();
    }
}


/*!
  Makes QRangeControl::setValue() available as a slot.
*/

void QSlider::setValue( int value )
{
    QRangeControl::setValue( value );
}


/*!
  Moves the slider one pageStep() upwards.
*/

void QSlider::addStep()
{
    addPage();
}


/*!
  Moves the slider one pageStep() downwards.
*/

void QSlider::subtractStep()
{
    subtractPage();
}


/*!
  Waits for autorepeat.
*/

void QSlider::repeatTimeout()
{
    ASSERT( timer );
    timer->disconnect();
    if ( state == TimingDown )
	connect( timer, SIGNAL(timeout()), SLOT(subtractStep()) );
    else if ( state == TimingUp )
	connect( timer, SIGNAL(timeout()), SLOT(addStep()) );
    timer->start( repeatTime, FALSE );
}


/*!
  Returns the relevant dimension of \a p.
*/

int QSlider::goodPart( const QPoint &p ) const
{
    return (orient == Horizontal) ?  p.x() : p.y();
}

/*!\reimp
*/
QSize QSlider::sizeHint() const
{
    constPolish();
    const int length = 84;
    //    int thick = style() == MotifStyle ? 24 : 16;
    int thick = style().sliderThickness();
    const int tickSpace = 5;

    if ( ticks & Above )
	thick += tickSpace;
    if ( ticks & Below )
	thick += tickSpace;
    if ( style() == WindowsStyle && ticks != Both && ticks != NoMarks )
	thick += style().sliderLength() / 4;	    // pointed slider
    if ( orient == Horizontal )
	return QSize( length, thick ).expandedTo( QApplication::globalStrut() );
    else
	return QSize( thick, length ).expandedTo( QApplication::globalStrut() );
}



/*!
  \reimp
*/

QSize QSlider::minimumSizeHint() const
{
    QSize s = sizeHint();
    int length = style().sliderLength();
    if ( orient == Horizontal )
	s.setWidth( length );
    else
	s.setHeight( length );

    return s;
}



/*!\reimp
*/
QSizePolicy QSlider::sizePolicy() const
{
    if ( orient == Horizontal )
	return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    else
	return QSizePolicy(  QSizePolicy::Fixed, QSizePolicy::Expanding );
}


/*!
  Returns the number of pixels to use for the business part of the
  slider (i.e. the non-tickmark portion). The remaining space is shared
  equally between the tickmark regions. This function and  sizeHint()
  are closely related; if you change one, you almost certainly
  have to change the other.
*/

int QSlider::thickness() const
{
    int space = (orient == Horizontal) ? height() : width();
    int n = 0;
    if ( ticks & Above )
	n++;
    if ( ticks & Below )
	n++;
    if ( !n )
	return space;

    int thick = 6;	// Magic constant to get 5 + 16 + 5
    if ( style() == WindowsStyle && ticks != Both && ticks != NoMarks ) {
	thick += style().sliderLength() / 4;
    }
    space -= thick;
    //### the two sides may be unequal in size
    if ( space > 0 )
	thick += ( space * 2 ) / ( n + 2 );
    return thick;
}

/*! \obsolete
  \overload

  Do not reimplement this function, it's only there for compatibility
  reasons. It simply calls the other version with colorGroup() as the
  second argument.
*/

void QSlider::drawTicks( QPainter *p, int dist, int w, int i ) const
{
    drawTicks( p, colorGroup(), dist, w, i);
}

/*!
  Using \a p, draws tickmarks at a distance of \a dist from the edge
  of the widget, using \a w pixels and with an interval of \a i.

  Setting the colorgroup is useful to reuse the code to draw a mask if
  the slider supports transparceny.

  \sa setAutoMask(), updateMask()
*/

void QSlider::drawTicks( QPainter *p, const QColorGroup& g, int dist, int w,
			 int i ) const
{
    p->setPen( g.foreground() );
    int v = minValue();
    int fudge = slideLength() / 2 + 1;
    if(!i) 
	i = 1;
    while ( v <= maxValue() + 1 ) {
	int pos = positionFromValue( v ) + fudge;
	if ( orient == Horizontal )
	    p->drawLine( pos, dist, pos, dist + w );
	else
	    p->drawLine( dist, pos, dist + w, pos );
	v += i;
    }
}


/*!
  Sets the way tickmarks are displayed by the slider. \a s can take
  the following values:
  <ul>
  <li> \c NoMarks
  <li> \c Above
  <li> \c Left
  <li> \c Below
  <li> \c Right
  <li> \c Both
  </ul>
  The initial value is \c NoMarks.
  \sa tickmarks(), setTickInterval()
*/

void QSlider::setTickmarks( TickSetting s )
{
    ticks = s;
    initTicks();
    update();
    if ( autoMask() )
	updateMask();
}


/*!
  \fn QSlider::TickSetting QSlider::tickmarks() const

  Returns the tickmark settings for this slider.

  \sa setTickmarks()
  */

/*!
  \fn QSlider::TickSetting tickmarks() const
  Returns the way tickmarks are displayed by the slider.
  \sa setTickmarks()
*/

/*!
  Sets the interval between tickmarks to \a i. This is a value interval,
  not a pixel interval. If \a i is 0, the slider
  will choose between lineStep() and pageStep(). The initial value of
  tickInterval() is 0.
  \sa tickInterval(), QRangeControl::lineStep(), QRangeControl::pageStep()
*/

void QSlider::setTickInterval( int i )
{
    tickInt = QMAX( 0, i );
    update();
    if ( autoMask() )
	updateMask();
}


/*!
  \fn int QSlider::tickInterval() const
  Returns the interval between tickmarks. Returns 0 if the slider
  chooses between pageStep() and lineStep().
  \sa setTickInterval()
*/


/*!
  \reimp
 */
void QSlider::styleChange( QStyle& old )
{
    QWidget::styleChange( old );
}

/*!
  \reimp
*/
int QSlider::minValue() const
{
    return QRangeControl::minValue();
}

/*!
  \reimp
*/
int QSlider::maxValue() const
{
    return QRangeControl::maxValue();
}

/*!
  A convenience function which just calls
  setRange( i, maxValue() )

  \sa setRange()
*/
void QSlider::setMinValue( int i )
{
    setRange( i, maxValue() );
}

/*!
  A convenience function which just calls
  setRange( minValue(), i )

  \sa setRange()
*/
void QSlider::setMaxValue( int i )
{
    setRange( minValue(), i );
}

/*!
  \reimp
*/
int QSlider::lineStep() const
{
    return QRangeControl::lineStep();
}

/*!
  \reimp
*/
int QSlider::pageStep() const
{
    return QRangeControl::pageStep();
}

/*!
  Sets the line step to \e i.

  Calls the virtual stepChange() function if the new line step is
  different from the previous setting.

  \sa lineStep() QRangeControl::setSteps() setPageStep() setRange()
*/
void QSlider::setLineStep( int i )
{
    setSteps( i, pageStep() );
}

/*!
  Sets the page step to \e i.

  Calls the virtual stepChange() function if the new page step is
  different from the previous setting.

  \sa pageStep() QRangeControl::setSteps() setLineStep() setRange()
*/
void QSlider::setPageStep( int i )
{
    setSteps( lineStep(), i );
}

/*!
  \reimp
*/
int QSlider::value() const
{
    return QRangeControl::value();
}
#endif
