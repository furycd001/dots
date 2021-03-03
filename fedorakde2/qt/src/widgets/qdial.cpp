/****************************************************************************
** $Id: qt/src/widgets/qdial.cpp   2.3.2   edited 2001-03-13 $
**
** Implementation of the dial widget
**
** Created : 979899
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

#include "qdial.h"

#ifndef QT_NO_DIAL

#include "qpainter.h"
#include "qpointarray.h"
#include "qtimer.h"
#include "qcolor.h"
#include "qapplication.h"
#include "qregion.h"

#include <math.h> // sin(), cos(), atan()
//### Forutsetter linking med math lib - Jfr kommentar i qpainter_x11.cpp!

static const double m_pi = 3.14159265358979323846;
static const double rad_factor = 180.0 / m_pi;


class QDialPrivate
{
public:
    QDialPrivate()
    {
	wrapping = FALSE;
	tracking = TRUE;
	doNotEmit = FALSE;
	target = 3.7;
	mousePressed = FALSE;
    }

    bool wrapping;
    bool tracking;
    bool doNotEmit;
    double target;
    QRect eraseArea;
    bool eraseAreaValid;
    bool showNotches;
    bool onlyOutside;
    bool mousePressed;
    
    QPointArray lines;
};


// NOT REVISED
/*! \class QDial qdial.h

  \brief The QDial class provides a rounded rangecontrol (like a speedometer or potentiometer).

  \ingroup basic

  Q dial is used when the user needs to control a value within a
  program-definable range, and the range either wraps around
  (typically, 0-359 degrees) or the dialog layout needs a square widget.

  Both API- and UI-wise, the dial is very like a \link QSlider
  slider. \endlink Indeed, when wrapping() is FALSE (the default)
  there is no hard difference between a slider and a dial.  They have
  the same signals, slots and member functions, all of which do the
  same things.	Which one to use depends only on your taste and
  on the application.

  The dial initially emits valueChanged() signals continuously while
  the slider is being moved; you can make it emit the signal less
  often by calling setTracking( FALSE ).  dialMoved() is emitted
  continuously even when tracking() is FALSE.

  The slider also emits dialPressed() and dialReleased() signals when
  the mouse button is pressed and released.  But note that the dial's
  value can change without these signals being emitted; the keyboard
  and wheel can be used to change the value.

  Unlike the slider, QDial attempts to draw a "nice" number of notches
  rather than one per lineStep().  If possible, that number \e is
  lineStep(), but if there aren't enough pixels to draw every, QDial
  will draw every second, third or something.  notchSize() returns the
  number of units per notch, hopefully a multiple of lineStep();
  setNotchTarget() sets the target distance between neighbouring
  notches in pixels.  The default is 3.75 pixels.

  Like the slider, the dial makes the QRangeControl functions
  setValue(), addLine(), substractLine(), addPage() and subtractPage()
  available as slots.

  The dial's keyboard interface is fairly simple: The left/up and right/down
  arrow keys move by lineStep(), page up and page down by pageStep()
  and Home and End to minValue() and maxValue().

  <img src=qdial-m.png> <img src=qdial-w.png>

  \sa QScrollBar QSpinBox
  <a href="guibooks.html#fowler">GUI Design Handbook: Slider</a>
*/




/*!  Constructs a dial with the default range of QRangeControl. */

QDial::QDial( QWidget *parent, const char *name )
    : QWidget( parent, name, WRepaintNoErase | WResizeNoErase ), QRangeControl()
{
    d = new QDialPrivate;
    d->eraseAreaValid = FALSE;
    d->showNotches = FALSE;
    d->onlyOutside = FALSE;
    setFocusPolicy( QWidget::WheelFocus );
}



/*!  Constructs a dial whose value can never be smaller than \a
  minValue or greater than \a maxValue, whose line step size is \a
  lineStep and page step size is \a pageStep, and whose value is
  initially \a value.

  \a value is forced to be within the legal range.

*/

QDial::QDial( int minValue, int maxValue, int pageStep, int value,
	      QWidget *parent, const char *name )
    : QWidget( parent, name, WRepaintNoErase | WResizeNoErase ),
      QRangeControl( minValue, maxValue, 1, pageStep, value )
{
    d = new QDialPrivate;
    d->eraseAreaValid = FALSE;
    d->showNotches = FALSE;
    d->onlyOutside = FALSE;
    setFocusPolicy( QWidget::WheelFocus );
}

/*!
  Destructs the dial.
*/
QDial::~QDial()
{
    delete d;
}


/*!
  If \a enable is TRUE, tracking is enabled. This means that
  the arrow can be moved using the mouse. Else this is not
  possible.
*/

void QDial::setTracking( bool enable )
{
    d->tracking = enable;
}


/*!  Returns TRUE if tracking is enabled, or FALSE if tracking is disabled.

Tracking is initially enabled.

\sa setTracking().
*/

bool QDial::tracking() const
{
    return d ? d->tracking : TRUE;
}

/*!  Makes QRangeControl::setValue() available as a slot.
*/

void QDial::setValue( int newValue )
{ // ### set doNotEmit?	 Matthias?
    QRangeControl::setValue( newValue );
}


/*!  Moves the dial one lineStep() upwards. */

void QDial::addLine()
{
    QRangeControl::addLine();
}


/*!  Moves the dial one lineStep() downwards. */

void QDial::subtractLine()
{
    QRangeControl::subtractLine();
}


/*! \reimp */

void QDial::resizeEvent( QResizeEvent * e )
{
    d->lines.resize( 0 );
    QWidget::resizeEvent( e );
}


/*!
  \reimp
*/

void QDial::paintEvent( QPaintEvent * e )
{
    repaintScreen( &e->rect() );
}

/*!
  Paints the dial.
*/

void QDial::repaintScreen( const QRect *cr )
{
    QPainter p;
    p.begin( this );

    bool resetClipping = FALSE;

    // calculate clip-region for erasing background
    if ( cr ) {
	p.setClipRect( *cr );
    } else if ( !d->onlyOutside && d->eraseAreaValid ) {
	QRegion reg = d->eraseArea;
	double a;
	reg = reg.subtract( calcArrow( a ) );
	p.setClipRegion( reg );
	resetClipping = TRUE;
    }

    QRect br( calcDial() );
    p.setPen( NoPen );
    if ( style() == MotifStyle )
	p.setBrush( colorGroup().brush( QColorGroup::Mid ) );
    else {
      QBrush b;
      if ( colorGroup().brush( QColorGroup::Light ).pixmap() )
	b = QBrush( colorGroup().brush( QColorGroup::Light ) );
      else
	b = QBrush( colorGroup().light(), Dense4Pattern );
      p.setBrush( b );
      p.setBackgroundMode( OpaqueMode );
    }

    QRect te = br;
    te.setWidth(te.width()+2);
    te.setHeight(te.height()+2);
    // erase background of dial
    if ( !d->onlyOutside ) {
#ifdef _WS_QWS_
	// brush polygon fills too slow in Qt/Embedded
	p.drawRect( te );
#else
	p.drawEllipse( te );
#endif
    }

    // erase remaining space around the dial
    p.save();
    QRegion remaining( 0, 0, width(), height() );
    remaining = remaining.subtract( QRegion( te, QRegion::Ellipse ) );
    if ( p.hasClipping() )
	remaining = remaining.intersect( p.clipRegion() );
    p.setClipRegion( remaining );
    p.fillRect( remaining.boundingRect(), colorGroup().brush( QColorGroup::Background ) );
    p.restore();

    if ( resetClipping ) {
	if ( cr )
	    p.setClipRect( *cr );
	else
	    p.setClipRect( QRect( 0, 0, width(), height() ) );
    }

    // draw notches
    if ( d->showNotches ) {
	calcLines();
	p.setPen( colorGroup().foreground() );
	p.drawLineSegments( d->lines );
    }

    // calculate and paint arrow
    p.setPen( QPen( colorGroup().dark() ) );
    p.drawArc( te, 60 * 16, 180 * 16 );
    p.setPen( QPen( colorGroup().light() ) );
    p.drawArc( te, 240 * 16, 180 * 16 );

    double a;
    QPointArray arrow( calcArrow( a ) );
    QRect ea( arrow.boundingRect() );
    d->eraseArea = ea;
    d->eraseAreaValid = TRUE;

    p.setPen( NoPen );
    p.setBrush( colorGroup().brush( QColorGroup::Button ) );
    if ( !d->onlyOutside )
	p.drawPolygon( arrow );

    a = angle( QPoint( width() / 2, height() / 2 ), arrow[ 0 ] );
    p.setBrush( Qt::NoBrush );

    // that's still a hack...
    if ( a <= 0 || a > 200 ) {
	p.setPen( colorGroup().light() );
	p.drawLine( arrow[ 2 ], arrow[ 0 ] );
	p.drawLine( arrow[ 1 ], arrow[ 2 ] );
	p.setPen( colorGroup().dark() );
	p.drawLine( arrow[ 0 ], arrow[ 1 ] );
    } else if ( a > 0 && a < 45 ) {
	p.setPen( colorGroup().light() );
	p.drawLine( arrow[ 2 ], arrow[ 0 ] );
	p.setPen( colorGroup().dark() );
	p.drawLine( arrow[ 1 ], arrow[ 2 ] );
	p.drawLine( arrow[ 0 ], arrow[ 1 ] );
    } else if ( a >= 45 && a < 135 ) {
	p.setPen( colorGroup().dark() );
	p.drawLine( arrow[ 2 ], arrow[ 0 ] );
	p.drawLine( arrow[ 1 ], arrow[ 2 ] );
	p.setPen( colorGroup().light() );
	p.drawLine( arrow[ 0 ], arrow[ 1 ] );
    } else if ( a >= 135 && a < 200 ) {
	p.setPen( colorGroup().dark() );
	p.drawLine( arrow[ 2 ], arrow[ 0 ] );
	p.setPen( colorGroup().light() );
	p.drawLine( arrow[ 0 ], arrow[ 1 ] );
	p.drawLine( arrow[ 1 ], arrow[ 2 ] );
    }

    // draw focus rect around the dial
    if ( hasFocus() ) {
	p.setClipping( FALSE );
	br.setWidth( br.width() + 2 );
	br.setHeight( br.height() + 2 );
	if ( d->showNotches ) {
	    int r = QMIN( width(), height() ) / 2;
	    br.moveBy( -r / 6, - r / 6 );
	    br.setWidth( br.width() + r / 3 );
	    br.setHeight( br.height() + r / 3 );
	}
	// strange, but else we get redraw errors on windows!!
	p.end();
	p.begin( this );
	p.save();
	p.setPen( QPen( colorGroup().background() ) );
	p.setBrush( NoBrush );
	p.drawRect( br );
	p.restore();
	style().drawFocusRect( &p, br, colorGroup() , &colorGroup().background() );
    }
    p.end();
}


/*!
  \reimp
*/

void QDial::keyPressEvent( QKeyEvent * e )
{
    switch ( e->key() ) {
    case Key_Left: case Key_Down:
	subtractLine();
	break;
    case Key_Right: case Key_Up:
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
	break;
    }
}


/*!
  \reimp
*/

void QDial::mousePressEvent( QMouseEvent * e )
{
    d->mousePressed = TRUE;
    setValue( valueFromPoint( e->pos() ) );
    emit dialPressed();
}


/*!
  \reimp
*/

void QDial::mouseReleaseEvent( QMouseEvent * e )
{
    d->mousePressed = FALSE;
    setValue( valueFromPoint( e->pos() ) );
    emit dialReleased();
}


/*!
  \reimp
*/

void QDial::mouseMoveEvent( QMouseEvent * e )
{
    if ( !d->mousePressed )
	return;
    if ( !d->tracking || (e->state() & LeftButton) == 0 )
	return;
    d->doNotEmit = TRUE;
    setValue( valueFromPoint( e->pos() ) );
    emit dialMoved( value() );
    d->doNotEmit = FALSE;
}


/*!
  \reimp
*/

void QDial::wheelEvent( QWheelEvent *e )
{
    setValue( value() - e->delta() / 120 );
}


/*!
  \reimp
*/

void QDial::focusInEvent( QFocusEvent * )
{
    d->onlyOutside = TRUE;
    repaintScreen();
    d->onlyOutside = FALSE;
}


/*!
  \reimp
*/

void QDial::focusOutEvent( QFocusEvent * )
{
    d->onlyOutside = TRUE;
    repaintScreen();
    d->onlyOutside = FALSE;
}

/*!
  Reimplemented to ensure the display is correct and to emit
  the valueChanged(int) signal when appropriate.
*/

void QDial::valueChange()
{
    d->lines.resize( 0 );
    repaintScreen();
    if ( d->tracking || !d->doNotEmit )
	emit valueChanged( value() );
}


/*!
  Reimplemented to ensure tick-marks are consistent with the new range.
*/

void QDial::rangeChange()
{
    d->lines.resize( 0 );
    repaintScreen();
}


/*!
  \internal
*/

int QDial::valueFromPoint( const QPoint & p ) const
{
    double a = atan2( (double)height()/2.0 - p.y(),
		      (double)p.x() - width()/2.0 );
    if ( a < m_pi/-2 )
	a = a + m_pi*2;

    int dist = 0;
    int minv = minValue(), maxv = maxValue();

    if ( minValue() < 0 ) {
	dist = -minValue();
	minv = 0;
	maxv = maxValue() + dist;
    }

    int r = maxv - minv;
    int v;
    if ( d->wrapping )
	v =  (int)(0.5 + minv + r*(m_pi*3/2-a)/(2*m_pi));
    else
	v =  (int)(0.5 + minv + r*(m_pi*4/3-a)/(m_pi*10/6));

    if ( dist > 0 )
	v -= dist;

    return bound( v );
}


/*!
  \internal
*/

double QDial::angle( const QPoint &p1, const QPoint &p2 ) const
{
    double _angle = 0.0;

    if ( p1.x() == p2.x() ) {
	if ( p1.y() < p2.y() )
	    _angle = 270.0;
	else
	    _angle = 90.0;
    } else  {
	double x1, x2, y1, y2;

	if ( p1.x() <= p2.x() ) {
	    x1 = p1.x(); y1 = p1.y();
	    x2 = p2.x(); y2 = p2.y();
	} else {
	    x2 = p1.x(); y2 = p1.y();
	    x1 = p2.x(); y1 = p2.y();
	}

	double m = -( y2 - y1 ) / ( x2 - x1 );
	_angle = atan( m ) *  rad_factor;

	if ( p1.x() < p2.x() )
	    _angle = 180.0 - _angle;
	else
	    _angle = -_angle;
    }

    return _angle;
}

/*!
  If \a enable is TRUE, wrapping is enabled. This means
  that the arrow can be turned around 360°. Else there is
  some space at the bottom which is skipped by the arrow.
*/

void QDial::setWrapping( bool enable )
{
    if ( d->wrapping == enable )
	return;
    d->lines.resize( 0 );
    d->wrapping = enable;
    d->eraseAreaValid = FALSE;
    repaintScreen();
}


/*!
  Returns TRUE if wrapping is enabled, else FALSE.

  \sa QDial::setWrapping()
*/

bool QDial::wrapping() const
{
    return d->wrapping;
}


/*!  Returns the current notch size.  This is in range control units,
not pixels, and if possible it is a multiple of lineStep() that
results in an on-screen notch size near notchTarget().

\sa notchTarget() lineStep()
*/

int QDial::notchSize() const
{
    // radius of the arc
    int r = QMIN( width(), height() )/2;
    // length of the whole arc
    int l = (int)(r*(d->wrapping ? 6 : 5)*m_pi/6);
    // length of the arc from minValue() to minValue()+pageStep()
    if ( maxValue() > minValue()+pageStep() )
	l = (int)(0.5 + l * pageStep() / (maxValue()-minValue()));
    // length of a lineStep() arc
    l = l * lineStep() / pageStep();
    if ( l < 1 )
	l = 1;
    // how many times lineStep can be draw in d->target pixels
    l = (int)(0.5 + d->target / l);
    // we want notchSize() to be a non-zero multiple of lineStep()
    if ( !l )
	l = 1;
    return lineStep() * l;
}


/*!  Sets the dial to use a notch size as close to \a target pixels as
  possible.  QDial will find a suitable number close to this, based on
  the dial's on-screen size, range and lineStep().

  \sa notchTarget() notchSize()
*/

void QDial::setNotchTarget( double target )
{
    d->lines.resize( 0 );
    d->target = target;
    d->eraseAreaValid = FALSE;
    d->onlyOutside = TRUE;
    repaintScreen();
    d->onlyOutside = FALSE;
}


/*!  Returns the target size of the notch; this is the number of
  pixels QDial attempts to put between each little line.

  The actual size differs a bit from the target.
*/

double QDial::notchTarget() const
{
    return d->target;
}


/*!  Moves the dial one pageStep() upwards. */

void QDial::addPage()
{
    QRangeControl::addPage();
}


/*!  Moves the dial one pageStep() downwards. */

void QDial::subtractPage()
{
    QRangeControl::subtractPage();
}


/*!
  \fn void QDial::valueChanged( int value )

  This signal is emitted whenever the dial value changes.  The frequency
  of this signal is influenced by setTracking().
*/

/*!
  \fn void QDial::dialPressed()

  This signal is emitted when the use begins mouse interaction with
  the dial.

  \sa dialReleased()
*/

/*!
  \fn void QDial::dialMoved( int value )

  This signal is emitted whenever the dial value changes.  The frequency
  of this signal is \e not influenced by setTracking().

  \sa valueChanged(int)
*/

/*!
  \fn void QDial::dialReleased()

  This signal is emitted when the use ends mouse interaction with
  the dial.

  \sa dialPressed()
*/

/*!
  Enables or disables showing of notches. If \a b is TRUE, the notches
  are shown, else not.
*/

void QDial::setNotchesVisible( bool b )
{
    d->showNotches = b;
    d->eraseAreaValid = FALSE;
    d->onlyOutside = TRUE;
    repaintScreen();
    d->onlyOutside = FALSE;
}

/*!
  Retuns TRUE if notches are shown, else FALSE.

  \sa setNotchesVisible()
*/

bool QDial::notchesVisible() const
{
    return d->showNotches;
}

/*!
  \reimp
*/

QSize QDial::minimumSizeHint() const
{
    return QSize( 50, 50 );
}

/*!
  \reimp
*/

QSize QDial::sizeHint() const
{
    return QSize( 100, 100 ).expandedTo( QApplication::globalStrut() );
}



/*!
  \internal
*/

QPointArray QDial::calcArrow( double &a ) const
{
    int r = QMIN( width(), height() ) / 2;
    if ( maxValue() == minValue() )
	a = m_pi / 2;
    else if ( d->wrapping )
	a = m_pi * 3 / 2 - ( value() - minValue() ) * 2 * m_pi / ( maxValue() - minValue() );
    else
	a = ( m_pi * 8 - ( value() - minValue() ) * 10 * m_pi / ( maxValue() - minValue() ) ) / 6;

    int xc = width() / 2;
    int yc = height() / 2;

    int len = r - calcBigLineSize() - 5;
    if ( len < 5 )
	len = 5;
    int back = len / 4;
    if ( back < 1 )
	back = 1;

    QPointArray arrow( 3 );
    arrow[0] = QPoint( (int)( 0.5 + xc + len * cos(a) ),
		       (int)( 0.5 + yc -len * sin( a ) ) );
    arrow[1] = QPoint( (int)( 0.5 + xc + back * cos( a + m_pi * 5 / 6 ) ),
		       (int)( 0.5 + yc - back * sin( a + m_pi * 5 / 6 ) ) );
    arrow[2] = QPoint( (int)( 0.5 + xc + back * cos( a - m_pi * 5 / 6 ) ),
		       (int)( 0.5 + yc - back * sin( a - m_pi * 5 / 6 ) ) );
    return arrow;
}

/*!
  \internal
*/

QRect QDial::calcDial() const
{
    double r = QMIN( width(), height() ) / 2.0;
    double d_ = r / 6.0;
    double dx = d_ + ( width() - 2 * r ) / 2.0 + 1;
    double dy = d_ + ( height() - 2 * r ) / 2.0 + 1;
    return QRect( int(dx), int(dy),
		int(r * 2 - 2 * d_ - 2), int(r * 2 - 2 * d_ - 2) );
}

/*!
  \internal
*/

int QDial::calcBigLineSize() const
{
    int r = QMIN( width(), height() ) / 2;
    int bigLineSize = r / 6;
    if ( bigLineSize < 4 )
	bigLineSize = 4;
    if ( bigLineSize > r / 2 )
	bigLineSize = r / 2;
    return bigLineSize;
}

/*!
  \internal
*/

void QDial::calcLines()
{
    if ( !d->lines.size() ) {
	double r = QMIN( width(), height() ) / 2.0;
	int bigLineSize = calcBigLineSize();
	double xc = width() / 2.0;
	double yc = height() / 2.0;
	int ns = notchSize();
	int notches = ( maxValue() + ns - 1 - minValue() ) / ns;
	d->lines.resize( 2 + 2 * notches );
	int smallLineSize = bigLineSize / 2;
	int i;
	for( i = 0; i <= notches; i++ ) {
	    double angle = d->wrapping
		? m_pi * 3 / 2 - i * 2 * m_pi / notches
		: (m_pi * 8 - i * 10 * m_pi / notches) / 6;
	
	    double s = sin( angle ); // sin/cos aren't defined as const...
	    double c = cos( angle );
	    if ( i == 0 || ( ((ns * i ) % pageStep() ) == 0 ) ) {
		d->lines[2*i] = QPoint( (int)( xc + ( r - bigLineSize ) * c ),
					(int)( yc - ( r - bigLineSize ) * s ) );
		d->lines[2*i+1] = QPoint( (int)( xc + r * c ),
					  (int)( yc - r * s ) );
	    } else {
		d->lines[2*i] = QPoint( (int)( xc + ( r - 1 - smallLineSize ) * c ),
					(int)( yc - ( r - 1 - smallLineSize ) * s ) );
		d->lines[2*i+1] = QPoint( (int)( xc + ( r - 1 ) * c ),
					  (int)( yc -( r - 1 ) * s ) );
	    }
	}
    }
}

/*!
  \reimp
*/
int QDial::minValue() const
{
    return QRangeControl::minValue();
}

/*!
  \reimp
*/
int QDial::maxValue() const
{
    return QRangeControl::maxValue();
}

/*!
  A convenience function which just calls
  setRange( i, maxValue() )

  \sa setRange()
*/
void QDial::setMinValue( int i )
{
    setRange( i, maxValue() );
}

/*!
  A convenience function which just calls
  setRange( minValue(), i )

  \sa setRange()
*/
void QDial::setMaxValue( int i )
{
    setRange( minValue(), i );
}

/*!
  \reimp
*/
int QDial::lineStep() const
{
    return QRangeControl::lineStep();
}

/*!
  \reimp
*/
int QDial::pageStep() const
{
    return QRangeControl::pageStep();
}

/*!
  Sets the line step to \e i.

  Calls the virtual stepChange() function if the new line step is
  different from the previous setting.

  \sa lineStep() QRangeControl::setSteps() setPageStep() setRange()
*/
void QDial::setLineStep( int i )
{
    setSteps( i, pageStep() );
}

/*!
  Sets the page step to \e i.

  Calls the virtual stepChange() function if the new page step is
  different from the previous setting.

  \sa pageStep() QRangeControl::setSteps() setLineStep() setRange()
*/
void QDial::setPageStep( int i )
{
    setSteps( lineStep(), i );
}

/*!
  \reimp
*/
int QDial::value() const
{
    return QRangeControl::value();
}

#endif // QT_FEATURE_DIAL

