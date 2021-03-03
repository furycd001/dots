/****************************************************************************
** $Id: qt/src/widgets/qframe.cpp   2.3.2   edited 2001-01-26 $
**
** Implementation of QFrame widget class
**
** Created : 950201
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

#include "qframe.h"
#ifndef QT_NO_FRAME
#include "qpainter.h"
#include "qdrawutil.h"
#include "qframe.h"
#include "qbitmap.h"

// REVISED: warwick
/*!
  \class QFrame qframe.h
  \brief The QFrame class is the base class of widgets that can have a frame.

  \ingroup abstractwidgets

  It draws a frame and calls a virtual function, drawContents(), to
  fill in the frame.  This function is reimplemented by
  subclasses.  There are also two other less useful functions,
  drawFrame() and frameChanged().

  QPopupMenu uses this to "raise" the menu above the surrounding
  screen. QProgressBar has a "sunken" look.  QLabel has a flat look.
  The frames of widgets such as these can be changed.

  \code
    QLabel label(...);
    label.setFrameStyle( QFrame::Panel | QFrame::Raised );
    label.setLineWidth( 2 );

    QProgressBar pbar(...);
    label.setFrameStyle( QFrame::NoFrame );
  \endcode

  The QFrame class can also be used directly for creating simple frames
  without any contents, although usually you would use a QHBox or QVBox
  as these layout the widgets you put inside the frame.

  A frame widget has four attributes: frameStyle(), lineWidth(),
  midLineWidth(), and margin().

  The frame style is specified by a
  \link QFrame::Shape frame shape\endlink and a
  \link QFrame::Shadow shadow style\endlink.
  The frame shapes are \c NoFrame, \c Box, \c Panel,
  \c StyledPanel, \c PopupPanel,
  \c WinPanel, \c HLine and \c VLine, and the shadow styles are \c Plain,
  \c Raised and \c Sunken.

  The line width is the width of the frame border.

  The mid-line width specifies the width of an extra line in the
  middle of the frame, that uses a third color to obtain a special 3D
  effect.  Notice that a mid-line is only drawn for \c Box, \c HLine
  and \c VLine frames that are raised or sunken.

  The margin is the gap between the frame and the contents of the frame.

  <a name=picture></a>
  This table shows the most useful combinations of styles and widths
  (and some rather useless ones):

  <img src=frames.png width=515 height=414 alt="Table of frame styles">
*/


/*! \enum QFrame::Shape

  This enum type defines the shapes of a QFrame's frame.  The
  currently defined shapes are: <ul>

  <li> \c NoFrame - QFrame draws nothing

  <li> \c Box - QFrame draws a box around its contents

  <li> \c Panel - QFrame draws a panel such that the contents appear raised or sunken

  <li> \c WinPanel - like \c Panel, but QFrame draws the 3D effects
  the way Microsoft Windows 95 (etc) does

  <li> \c HLine - QFrame draws a horizontal line that frames nothing
  (useful as separator)

  <li> \c VLine - QFrame draws a vertical line that frames nothing
  (useful as separator)

  <li> \c StyledPanel - QFrame calls QStyle::drawPanel()

  <li> \c PopupPanel - QFrame calls QStyle::drawPopupPanel()

  </ul>

  When it does not call QStyle, Shape interacts with QFrame::Shadow,
  the lineWidth() and the midLineWidth() to create the total result.
  The <a href="#picture">picture of the frames</a> in the class
  documentation may illustrate this better than words.

  \sa QFrame::Shadow QFrame::style() QStyle::drawPanel()
  QStyle::drawPopupPanel()
*/


/*! \enum QFrame::Shadow

  This enum type defines the 3D effect used for QFrame's frame.  The
  currently defined effects are: <ul>

  <li> \c Plain - the frame and contents appear level with the surroundings
  <li> \c Raised - the frame and contents appear raised
  <li> \c Sunken - the frame and contents appear sunken

  </ul>

  Shadow interacts with QFrame::Shape, the lineWidth() and the
  midLineWidth().  The <a href="#picture">picture of the frames</a> in
  the class documentation may illustrate this better than words.

  \sa QFrame::Shape lineWidth() midLineWidth()
*/


#if (QT_VERSION > 298)
#error "get rid of allowLines for good"
#endif

/*!
  Constructs a frame widget with frame style \c NoFrame and a 1 pixel frame
  width.

  The last argument exists for compatibility with Qt 1.x; it
  no longer has any meaning.

  The \e parent, \e name and \e f arguments are passed to the QWidget
  constructor.
*/

QFrame::QFrame( QWidget *parent, const char *name, WFlags f,
		bool )
    : QWidget( parent, name, f )
{
    frect  = QRect( 0, 0, 0, 0 );
    fstyle = NoFrame;
    lwidth = 1;
    mwidth = 0;
    mlwidth = 0;
    d = 0;
    updateFrameWidth();
}

static const int wpwidth = 2; // WinPanel lwidth

/*!
  \fn int QFrame::frameStyle() const
  Returns the frame style.

  The default value is QFrame::NoFrame.

  \sa setFrameStyle(), frameShape(), frameShadow()
*/

/*!
  \fn Shape QFrame::frameShape() const
  Returns the frame shape value from the frame style.
  \sa setFrameShape(), frameStyle(), frameShadow()
*/

/*!
  \fn void QFrame::setFrameShape(Shape)
  Sets the frame shape value of the frame style.
  \sa frameShape(), frameStyle(), setFrameShadow()
*/

/*!
  \fn Shadow QFrame::frameShadow() const
  Returns the frame shadow value from the frame style.
  \sa setFrameShadow(), frameStyle(), frameShape()
*/

/*!
  \fn void QFrame::setFrameShadow( Shadow )
  Sets the frame shadow value of the frame style.
  \sa frameShadow(), frameStyle(), setFrameShape()
*/

/*!
  Sets the frame style to \e style.

  The \e style is the bitwise OR between a frame shape and a frame
  shadow style.  See the <a href="#picture">illustration</a> in the
  class documentation.

  The frame shapes are:
  <ul>
  <li> \c NoFrame draws nothing. Naturally, you should not specify a shadow
  style if you use this.
  <li> \c Box draws a rectangular box.  The contents appear to be
  level with the surrounding screen, but the border itself may be
  raised or sunken.
  <li> \c Panel draws a rectangular panel that can be raised or sunken.
  <li> \c StyledPanel draws a rectangular panel with a look depending on
  the current GUI style.  It can be raised or sunken.
  <li> \c PopupPanel is used to draw a frame suitable for popup windows.
  Its look also depends on the current GUI style,  usually the same as
  \c StyledPanel.
  <li> \c WinPanel draws a rectangular panel that can be raised or
  sunken, very like those in Windows 95.  Specifying this shape sets
  the line width to 2 pixels.  WinPanel is provided for compatibility.
  For GUI style independence we recommend using StyledPanel instead.
  <li> \c HLine draws a horizontal line (vertically centered).
  <li> \c VLine draws a vertical line (horizontally centered).
  </ul>

  The shadow styles are:
  <ul>
  <li> \c Plain draws using the palette foreground color (without any
  3D effect).
  <li> \c Raised draws a 3D raised line using the light and dark
  colors of the current color group.
  <li> \c Sunken draws a 3D sunken line using the light and dark
  colors of the current color group.
  </ul>

  If a mid-line width greater than 0 is specified, an additional line
  is drawn for \c Raised or \c Sunken \c Box, \c HLine and \c VLine
  frames.  The mid color of the current color group is used for
  drawing middle lines.

  \sa <a href="#picture">Illustration</a>, frameStyle(),
  colorGroup(), QColorGroup
*/

void QFrame::setFrameStyle( int style )
{
    //   If this is a line, it may stretch in the direction of the
    //   line, but it is fixed in the other direction. If this is a
    //   normal frame, use QWidget's default behavior.
    switch (style & MShape) {
    case HLine:
	setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
	break;
    case VLine:
	setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Minimum ) );
	break;
    default:
	// only reset if it was hline or vline
	if ( (fstyle & MShape) == HLine || (fstyle & MShape) == VLine ) 
	    setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    }
    fstyle = (short)style;
    updateFrameWidth();
}

/*!
  \fn int QFrame::lineWidth() const
  Returns the line width.  (Note that the \e total line width
  for \c HLine and \c VLine is given by frameWidth(), not
  lineWidth().)

  The default value is 1.

  \sa setLineWidth(), midLineWidth(), frameWidth()
*/


/*!
  Sets the line width to \e w.
  \sa frameWidth(), lineWidth(), setMidLineWidth()
*/

void QFrame::setLineWidth( int w )
{
    lwidth = (short)w;
    updateFrameWidth();
}

/*!
  \fn int QFrame::midLineWidth() const
  Returns the width of the mid-line.

  The default value is 0.

  \sa setMidLineWidth(), lineWidth(), frameWidth()
*/

/*!
  Sets the width of the mid-line to \e w.
  \sa midLineWidth(), setLineWidth()
*/

void QFrame::setMidLineWidth( int w )
{
    mlwidth = (short)w;
    updateFrameWidth();
}



/*!
  \fn int QFrame::margin() const
  Returns the width of the margin. The margin is the distance between the
  innermost pixel of the frame and the outermost pixel of contentsRect().
  It is included in frameWidth().

  The margin is filled according to backgroundMode().

  The default value is 0.

  \sa setMargin(), lineWidth(), frameWidth()
*/

/*!
  Sets the width of the margin to \e w.
  \sa margin(), setLineWidth()
*/

void QFrame::setMargin( int w )
{
    mwidth = (short)w;
    updateFrameWidth();
}


/*!
  \internal
  Updated the fwidth parameter.
*/

void QFrame::updateFrameWidth()
{
    int type  = fstyle & MShape;
    int style = fstyle & MShadow;

    fwidth = -1;

    switch ( type ) {

    case NoFrame:
	fwidth = 0;
	break;

    case Box:
	switch ( style ) {
	case Plain:
	    fwidth = lwidth;
	    break;
	case Raised:
	case Sunken:
	    fwidth = (short)(lwidth*2 + midLineWidth() );
	    break;
	}
	break;

    case Panel:
    case StyledPanel:
    case PopupPanel:
	switch ( style ) {
	case Plain:
	case Raised:
	case Sunken:
	    fwidth = lwidth;
	    break;
	}
	break;

    case WinPanel:
	switch ( style ) {
	case Plain:
	case Raised:
	case Sunken:
	    fwidth =  wpwidth; //WinPanel does not use lwidth!
	    break;
	}
	break;

    case HLine:
    case VLine:
	switch ( style ) {
	case Plain:
	    fwidth = lwidth;
	    break;
	case Raised:
	case Sunken:
	    fwidth = (short)(lwidth*2 + midLineWidth());
	    break;
	}
	break;
    }

    if ( fwidth == -1 )				// invalid style
	fwidth = 0;

    fwidth += margin();

    frameChanged();
}


/*!
  \fn int QFrame::frameWidth() const
  Returns the width of the frame that is drawn.

  Note that the frame width depends on the \link setFrameStyle() frame
  style \endlink, not only the line width and the mid line width.  For
  example, the style \c NoFrame always has a frame width 0, while the
  style \c Panel has a frame width equivalent to the line width.
  The frame width also includes the margin.

  \sa lineWidth(), midLineWidth(), frameStyle(), margin()
*/


/*!
  Returns the frame rectangle.

  The default frame rectangle is equivalent to the \link
  QWidget::rect() widget rectangle\endlink.

  \sa setFrameRect()
*/

QRect QFrame::frameRect() const
{
    if ( frect.isNull() )
	return rect();
    else
	return frect;
}


/*!
  Sets the frame rectangle to \e r.

  The frame rectangle is the rectangle the frame is drawn in.  By
  default, this is the entire widget.  Calling setFrameRect() does \e
  not cause a widget update.

  If \e r is a null rectangle (for example
  <code>QRect(0,0,0,0)</code>), then the frame rectangle is equivalent
  to the \link QWidget::rect() widget rectangle\endlink.

  \sa frameRect(), contentsRect()
*/

void QFrame::setFrameRect( const QRect &r )
{
    frect = r.isValid() ? r : rect();
}


/*!
  Returns the rectangle inside the frame.
  \sa frameRect(), drawContents()
*/

QRect QFrame::contentsRect() const
{
    QRect r = frameRect();
    int	  w = frameWidth();			// total width
    r.setRect( r.x()+w, r.y()+w, r.width()-w*2, r.height()-w*2 );
    return r;
}

/*!\reimp
*/
QSize QFrame::sizeHint() const
{
    //   Returns a size hint for the frame - for HLine and VLine
    //   shapes, this is stretchable one way and 3 pixels wide the
    //   other.  For other shapes, QWidget::sizeHint() is used.
    switch (fstyle & MShape) {
    case HLine:
	return QSize(-1,3);
    case VLine:
	return QSize(3,-1);
    default:
	return QWidget::sizeHint();
    }
}



/*!\reimp
*/
QSizePolicy QFrame::sizePolicy() const
{
    //### removeme 3.0
    return QWidget::sizePolicy();

}


/*!
  Handles paint events for the frame.

  Paints the frame and the contents.

  Opens the painter on the frame and calls first drawFrame(), then
  drawContents().
*/

void QFrame::paintEvent( QPaintEvent *event )
{
    QPainter paint( this );

    if ( !contentsRect().contains( event->rect() ) ) {
	paint.save();
	paint.setClipRegion( event->region().intersect(frameRect()) );
	drawFrame( &paint );
	paint.restore();
    }
    if ( event->rect().intersects( contentsRect() ) &&
	 (fstyle & MShape) != HLine && (fstyle & MShape) != VLine ) {
	paint.setClipRegion( event->region().intersect( contentsRect() ) );
	drawContents( &paint );
    }
}


/*!
  Handles resize events for the frame.

  Adjusts the frame rectangle for the resized widget.  The frame
  rectangle is elastic, the surrounding area is static.

  The resulting frame rectangle may be null or invalid.  You can use
  setMinimumSize() to avoid that possibility.

  Nothing is done if the frame rectangle is a \link QRect::isNull()
  null rectangle\endlink already.
*/

void QFrame::resizeEvent( QResizeEvent *e )
{
    if ( !frect.isNull() ) {
	QRect r( frect.x(), frect.y(),
		 width()  - (e->oldSize().width()  - frect.width()),
		 height() - (e->oldSize().height() - frect.height()) );
	setFrameRect( r );
    }

    if ( autoMask())
	updateMask();
}


/*!
  Draws the frame using the current frame attributes and color
  group.  The rectangle inside the frame is not affected.

  This function is virtual, but in general you do not need to
  reimplement it.  If you do, note that the QPainter is already open
  and must remain open.

  \sa frameRect(), contentsRect(), drawContents(), frameStyle(), setPalette(), drawFrameMask()
*/

void QFrame::drawFrame( QPainter *p )
{
    QPoint	p1, p2;
    QRect	r     = frameRect();
    int		type  = fstyle & MShape;
    int		cstyle = fstyle & MShadow;
#ifdef QT_NO_DRAWUTIL
    p->setPen( black ); // ####
    p->drawRect( r ); //### a bit too simple
#else

    const QColorGroup & g = colorGroup();

    switch ( type ) {

    case Box:
	if ( cstyle == Plain )
	    qDrawPlainRect( p, r, g.foreground(), lwidth );
	else
	    qDrawShadeRect( p, r, g, cstyle == Sunken, lwidth,
			    midLineWidth() );
	break;

    case StyledPanel:
#ifndef QT_NO_STYLE
	if ( cstyle == Plain )
	    qDrawPlainRect( p, r, g.foreground(), lwidth );
	else
	    style().drawPanel( p, r.x(), r.y(), r.width(), r.height(), g, cstyle == Sunken, lwidth );
	break;
#endif // fall through to Panel if QT_NO_STYLE

    case PopupPanel:
#ifndef QT_NO_STYLE
	if ( cstyle == Plain )
	    qDrawPlainRect( p, r, g.foreground(), lwidth );
	else
	    style().drawPopupPanel( p, r.x(), r.y(), r.width(), r.height(), g, lwidth );
	break;
#endif // fall through to Panel if QT_NO_STYLE

    case Panel:
	if ( cstyle == Plain )
	    qDrawPlainRect( p, r, g.foreground(), lwidth );
	else
	    qDrawShadePanel( p, r, g, cstyle == Sunken, lwidth );
	break;

    case WinPanel:
	if ( cstyle == Plain )
	    qDrawPlainRect( p, r, g.foreground(), wpwidth );
	else
	    qDrawWinPanel( p, r, g, cstyle == Sunken );
	break;

    case HLine:
    case VLine:
	if ( type == HLine ) {
	    p1 = QPoint( r.x(), r.height()/2 );
	    p2 = QPoint( r.x()+r.width(), p1.y() );
	}
	else {
	    p1 = QPoint( r.x()+r.width()/2, 0 );
	    p2 = QPoint( p1.x(), r.height() );
	}
	if ( cstyle == Plain ) {
	    QPen oldPen = p->pen();
	    p->setPen( QPen(g.foreground(),lwidth) );
	    p->drawLine( p1, p2 );
	    p->setPen( oldPen );
	}
	else
	    qDrawShadeLine( p, p1, p2, g, cstyle == Sunken,
			    lwidth, midLineWidth() );
	break;
    }
#endif // QT_NO_DRAWUTIL
}


/*!
  Virtual function that draws the contents of the frame.

  The QPainter is already open when you get it, and you must leave it
  open.  Painter \link QPainter::setWorldMatrix() transformations\endlink
  are switched off on entry.  If you transform the painter, remember to
  take the frame into account and \link QPainter::resetXForm() reset
  transformation\endlink before returning.

  This function is reimplemented by subclasses that draw something
  inside the frame.  It should draw only inside contentsRect(). The
  default function does nothing.

  \sa contentsRect(), QPainter::setClipRect(), drawContentsMask()
*/

void QFrame::drawContents( QPainter * )
{
}


/*!
  Virtual function that is called when the frame style, line width or
  mid-line width changes.

  This function can be reimplemented by subclasses that need to know
  when the frame attributes change.

  The default implementation calls update().
*/

void QFrame::frameChanged()
{
    update();
}

/*!

 Reimplementation of QWidget::updateMask(). Draws the mask of the
 frame when transparency is required.

 This function calls the virtual functions drawFrameMask() and
 drawContentsMask(). These are the ones you may want to reimplement
 in subclasses.

 \sa QWidget::setAutoMask(), drawFrameMask(), drawContentsMask()

*/
void QFrame::updateMask()
{
    QBitmap bm( size() );
    bm.fill( color0 );
    QPainter p( &bm, this );
    p.setPen( color1 );
    p.setBrush( color1 );
    drawFrameMask( &p );
    drawContentsMask( &p );
    p.end();
    setMask( bm );
}


/*!
  Virtual function that draws the mask of the frame's frame.

  If you reimplemented drawFrame(QPainter*) and your widget should
  support transparency you probably have to re-implement this function as well.

  \sa drawFrame(), updateMask(), QWidget::setAutoMask(), QPainter::setClipRect()
*/
void QFrame::drawFrameMask( QPainter* p )
{
    QPoint	p1, p2;
    QRect	r     = frameRect();
    int		type  = fstyle & MShape;
    int		style = fstyle & MShadow;
#ifdef QT_NO_DRAWUTIL
    p->setPen( color1 );
    p->drawRect( r ); //### a bit too simple
#else
    QColorGroup g(color1, color1, color1, color1, color1, color1, color1, color1, color0);

    switch ( type ) {

    case Box:
	if ( style == Plain )
	    qDrawPlainRect( p, r, g.foreground(), lwidth );
	else
	    qDrawShadeRect( p, r, g, style == Sunken, lwidth,
			    midLineWidth() );
	break;

    case Panel:
	if ( style == Plain )
	    qDrawPlainRect( p, r, g.foreground(), lwidth );
	else
	    qDrawShadePanel( p, r, g, style == Sunken, lwidth );
	break;

    case WinPanel:
	if ( style == Plain )
	    qDrawPlainRect( p, r, g.foreground(), wpwidth );
	else
	    qDrawWinPanel( p, r, g, style == Sunken );
	break;

    case HLine:
    case VLine:
	if ( type == HLine ) {
	    p1 = QPoint( r.x(), r.height()/2 );
	    p2 = QPoint( r.x()+r.width(), p1.y() );
	}
	else {
	    p1 = QPoint( r.x()+r.width()/2, 0 );
	    p2 = QPoint( p1.x(), r.height() );
	}
	if ( style == Plain ) {
	    QPen oldPen = p->pen();
	    p->setPen( QPen(g.foreground(),lwidth) );
	    p->drawLine( p1, p2 );
	    p->setPen( oldPen );
	}
	else
	    qDrawShadeLine( p, p1, p2, g, style == Sunken,
			    lwidth, midLineWidth() );
	break;
    }
#endif // QT_NO_DRAWUTIL
}

/*!
  Virtual function that draws the mask of the frame's contents.

  If you reimplemented drawContents(QPainter*) and your widget should
  support transparency you probably have to re-implement this function as well.

  The default implementation masks the contents-rect.

  \sa drawContents(), updateMask(), QWidget::setAutoMask(), contentsRect(), QPainter::setClipRect()
*/
void QFrame::drawContentsMask( QPainter* p)
{
    int type  = fstyle & MShape;
    if ( type == HLine || type == VLine )
	return;
    QBrush oldBrush = p->brush();

    p->fillRect( contentsRect(), QBrush( color1 ) );

    p->setBrush( oldBrush );
}
#endif //QT_NO_FRAME
