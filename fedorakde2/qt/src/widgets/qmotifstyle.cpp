/****************************************************************************
** $Id: qt/src/widgets/qmotifstyle.cpp   2.3.2   edited 2001-08-20 $
**
** Implementation of Motif-like style class
**
** Created : 981231
**
** Copyright (C) 1998-2000 Trolltech AS.  All rights reserved.
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

#include "qmotifstyle.h"
#ifndef QT_NO_STYLE_MOTIF
#include "qapplication.h"
#include "qpainter.h"
#include "qdrawutil.h" // for now
#include "qpixmap.h" // for now
#include "qpalette.h" // for now
#include "qwidget.h"
#include "qlabel.h"
#include "qimage.h"
#include "qpushbutton.h"
#include "qwidget.h"
#include "qrangecontrol.h"
#include "qscrollbar.h"
#include "qtabbar.h"
#define INCLUDE_MENUITEM_DEF
#include "qpopupmenu.h"
#include <limits.h>

// NOT REVISED
/*!
  \class QMotifStyle qmotifstyle.h
  \brief Motif Look and Feel
  \ingroup appearance

  This class implements the Motif look and feel. It almost completely
  resembles the original Motif look as defined by the Open Group, but
  also contains minor improvements. The Motif style is Qt's default
  GUI style on UNIX platforms.
*/

/*!
    Constructs a QMotifStyle.

    If useHighlightCols is FALSE (default value), then the style will
    polish the application's color palette to emulate the Motif way of
    highlighting, which is a simple inversion between the base and the
    text color.
*/
QMotifStyle::QMotifStyle( bool useHighlightCols ) : QCommonStyle(MotifStyle)
{
    highlightCols = useHighlightCols;

#define Q_NICE_MOTIF_DEFAULT_BUTTON
#ifdef Q_NICE_MOTIF_DEFAULT_BUTTON
    setButtonDefaultIndicatorWidth( 3 );
#endif

#define Q_NICE_MOTIF_SLIDER_THICKNESS
#ifdef Q_NICE_MOTIF_SLIDER_THICKNESS
    setSliderThickness(24);
#endif
}

/*!
  Destructs the style.
*/
QMotifStyle::~QMotifStyle()
{
}


/*!
  If the argument is FALSE, then the style will polish the
  application's color palette to emulate the
  Motif way of highlighting, which is a simple inversion between the
  base and the text color.

  The effect will show up the next time a application palette is set
  via QApplication::setPalette(). The current color palette of the
  application remains unchanged.

  \sa QStyle::polish( QPalette& )
 */
void QMotifStyle::setUseHighlightColors( bool arg)
{
    highlightCols = arg;
}

/*!
  Returns whether the style treats the highlight colors of the palette
  Motif-like, which is a simple inversion between the base and the
  text color. The default is FALSE.
 */
bool QMotifStyle::useHighlightColors() const
{
    return highlightCols;
}

/*! \reimp */

void QMotifStyle::polish( QPalette& pal)
{
    if ( pal.normal().light() == pal.normal().base() ) {
	QColor nlight = pal.normal().light().dark(108 );
	pal.setColor( QPalette::Active, QColorGroup::Light, nlight ) ;
	pal.setColor( QPalette::Disabled, QColorGroup::Light, nlight ) ;
	pal.setColor( QPalette::Inactive, QColorGroup::Light, nlight ) ;
    }

    if ( highlightCols )
	return;

    // force the ugly motif way of highlighting *sigh*
    QColorGroup normal = pal.normal();
    QColorGroup disabled = pal.disabled();
    QColorGroup active = pal.active();

    pal.setColor( QPalette::Active, QColorGroup::Highlight,
		  normal.text() );
    pal.setColor( QPalette::Active, QColorGroup::HighlightedText,
		  normal.base());
    pal.setColor( QPalette::Disabled, QColorGroup::Highlight,
		  disabled.text() );
    pal.setColor( QPalette::Disabled, QColorGroup::HighlightedText,
		  disabled.base() );
    pal.setColor( QPalette::Inactive, QColorGroup::Highlight,
		  active.text() );
    pal.setColor( QPalette::Inactive, QColorGroup::HighlightedText,
		  active.base() );
}

/*!
 \reimp
 \internal
 Keep QStyle::polish() visible.
*/
void QMotifStyle::polish( QWidget* w )
{
    QStyle::polish(w);
}

/*!
 \reimp
 \internal
 Keep QStyle::polish() visible.
*/
void QMotifStyle::polish( QApplication* a )
{
    QStyle::polish(a);
}




/*! \reimp */


void QMotifStyle::drawIndicator( QPainter* p,
				 int x, int y, int w, int h, const QColorGroup &g,
				 int s, bool down, bool /*enabled*/ )
{
    bool on = s != QButton::Off;
    bool showUp = !(down ^ on);
    QBrush fill =  showUp || s == QButton::NoChange
		? g.brush( QColorGroup::Button ) : g.brush( QColorGroup::Mid );
    if ( s == QButton::NoChange ) {
	qDrawPlainRect( p, x, y, w, h, g.text(), 1, &fill );
	p->drawLine(x+w-1,y,x,y+h-1);
    } else
	qDrawShadePanel( p, x, y, w, h, g, !showUp, defaultFrameWidth(), &fill );
}


/*! \reimp */

QSize
QMotifStyle::indicatorSize() const
{
    return QSize(13,13);
}

#define QCOORDARRLEN(x) sizeof(x)/(sizeof(QCOORD)*2)
/*! \reimp */

void QMotifStyle::drawExclusiveIndicator( QPainter* p,
				   int x, int y, int w, int h, const QColorGroup &g,
				   bool on, bool down, bool /* enabled */ )
{
    QCOORD inner_pts[] =
	{ 2,h/2, w/2,2, w-3,h/2, w/2,h-3 };
    QCOORD top_pts[] =
        { 0,h/2, w/2,0 , w-2,h/2-1, w-3,h/2-1,
          w/2,1, 1,h/2, 2,h/2, w/2,2, w-4,h/2-1 };
    QCOORD bottom_pts[] =
        { 1,h/2+1, w/2,h-1, w-1,h/2, w-2,h/2,
          w/2,h-2, 2,h/2+1, 3,h/2+1, w/2,h-3, w-3,h/2 };

    bool showUp = !(down ^ on );
    QPointArray a( QCOORDARRLEN(inner_pts), inner_pts );
    p->eraseRect( x, y, w, h );
    p->setPen( NoPen );
    p->setBrush( showUp ? g.brush( QColorGroup::Button ) :
		 g.brush( QColorGroup::Mid ) )  ;
    a.translate( x, y );
    p->drawPolygon( a );			// clear inner area
    p->setPen( showUp ? g.light() : g.dark() );
    p->setBrush( NoBrush );
    a.setPoints( QCOORDARRLEN(top_pts), top_pts );
    a.translate( x, y );
    p->drawPolyline( a );			// draw top part
    p->setPen( showUp ? g.dark() : g.light() );
    a.setPoints( QCOORDARRLEN(bottom_pts), bottom_pts );
    a.translate( x, y );
    p->drawPolyline( a );			// draw bottom part
}


/*!
  Draws the mask of a mark indicating the state of an exclusive choice
*/
void
QMotifStyle::drawExclusiveIndicatorMask( QPainter *p, int x, int y, int w, int h, bool /* on */)
{
    p->setBrush( Qt::color1 );
    p->setPen( Qt::color1 );

    QPointArray a;
    a.setPoints( 4, 0,h/2, h/2,0, w-2,h/2, w/2,h-1 );
    a.translate(x,y);
    p->drawPolygon( a );
}


/*! \reimp */

QSize
QMotifStyle::exclusiveIndicatorSize() const
{
    return QSize(13,13);
}




/*! \reimp */

void
QMotifStyle::drawArrow( QPainter *p, ArrowType type, bool down,
		 int x, int y, int w, int h,
		 const QColorGroup &g, bool enabled, const QBrush * /* fill */ )
{
    // ### may be worth caching these as pixmaps, especially with the
    //	    cost of rotate() for vertical arrows.

    QPointArray bFill;				// fill polygon
    QPointArray bTop;				// top shadow.
    QPointArray bBot;				// bottom shadow.
    QPointArray bLeft;				// left shadow.
    QWMatrix	matrix;				// xform matrix
    bool vertical = type == UpArrow || type == DownArrow;
    bool horizontal = !vertical;
    int	 dim = w < h ? w : h;
    int	 colspec = 0x0000;			// color specification array

    if ( dim < 2 )				// too small arrow
	return;

    // adjust size and center (to fix rotation below)
    if ( w >  dim ) {
	x += (w-dim)/2;
	w = dim;
    }
    if ( h > dim ) {
	y += (h-dim)/2;
	h = dim;
    }

    if ( dim > 3 ) {
	if ( dim > 6 )
	    bFill.resize( dim & 1 ? 3 : 4 );
	bTop.resize( (dim/2)*2 );
	bBot.resize( dim & 1 ? dim + 1 : dim );
	bLeft.resize( dim > 4 ? 4 : 2 );
	bLeft.putPoints( 0, 2, 0,0, 0,dim-1 );
	if ( dim > 4 )
	    bLeft.putPoints( 2, 2, 1,2, 1,dim-3 );
	bTop.putPoints( 0, 4, 1,0, 1,1, 2,1, 3,1 );
	bBot.putPoints( 0, 4, 1,dim-1, 1,dim-2, 2,dim-2, 3,dim-2 );

	for( int i=0; i<dim/2-2 ; i++ ) {
	    bTop.putPoints( i*2+4, 2, 2+i*2,2+i, 5+i*2, 2+i );
	    bBot.putPoints( i*2+4, 2, 2+i*2,dim-3-i, 5+i*2,dim-3-i );
	}
	if ( dim & 1 )				// odd number size: extra line
	    bBot.putPoints( dim-1, 2, dim-3,dim/2, dim-1,dim/2 );
	if ( dim > 6 ) {			// dim>6: must fill interior
	    bFill.putPoints( 0, 2, 1,dim-3, 1,2 );
	    if ( dim & 1 )			// if size is an odd number
		bFill.setPoint( 2, dim - 3, dim / 2 );
	    else
		bFill.putPoints( 2, 2, dim-4,dim/2-1, dim-4,dim/2 );
	}
    }
    else {
	if ( dim == 3 ) {			// 3x3 arrow pattern
	    bLeft.setPoints( 4, 0,0, 0,2, 1,1, 1,1 );
	    bTop .setPoints( 2, 1,0, 1,0 );
	    bBot .setPoints( 2, 1,2, 2,1 );
	}
	else {					// 2x2 arrow pattern
	    bLeft.setPoints( 2, 0,0, 0,1 );
	    bTop .setPoints( 2, 1,0, 1,0 );
	    bBot .setPoints( 2, 1,1, 1,1 );
	}
    }

    if ( type == UpArrow || type == LeftArrow ) {
	matrix.translate( x, y );
	if ( vertical ) {
	    matrix.translate( 0, h - 1 );
	    matrix.rotate( -90 );
	} else {
	    matrix.translate( w - 1, h - 1 );
	    matrix.rotate( 180 );
	}
	if ( down )
	    colspec = horizontal ? 0x2334 : 0x2343;
	else
	    colspec = horizontal ? 0x1443 : 0x1434;
    }
    else if ( type == DownArrow || type == RightArrow ) {
	matrix.translate( x, y );
	if ( vertical ) {
	    matrix.translate( w-1, 0 );
	    matrix.rotate( 90 );
	}
	if ( down )
	    colspec = horizontal ? 0x2443 : 0x2434;
	else
	    colspec = horizontal ? 0x1334 : 0x1343;
    }

    QColor *cols[5];
    if ( enabled ) {
	cols[0] = 0;
	cols[1] = (QColor *)&g.button();
	cols[2] = (QColor *)&g.mid();
	cols[3] = (QColor *)&g.light();
	cols[4] = (QColor *)&g.dark();
    } else {
	cols[0] = 0;
	cols[1] = (QColor *)&g.button();
	cols[2] = (QColor *)&g.button();
	cols[3] = (QColor *)&g.button();
	cols[4] = (QColor *)&g.button();
    }
#define CMID	*cols[ (colspec>>12) & 0xf ]
#define CLEFT	*cols[ (colspec>>8) & 0xf ]
#define CTOP	*cols[ (colspec>>4) & 0xf ]
#define CBOT	*cols[ colspec & 0xf ]

    QPen     savePen   = p->pen();		// save current pen
    QBrush   saveBrush = p->brush();		// save current brush
    QWMatrix wxm = p->worldMatrix();
    QPen     pen( NoPen );
    QBrush brush = g.brush( enabled?QColorGroup::Button:QColorGroup::Mid );

    p->setPen( pen );
    p->setBrush( brush );
    p->setWorldMatrix( matrix, TRUE );		// set transformation matrix
    p->drawPolygon( bFill );			// fill arrow
    p->setBrush( NoBrush );			// don't fill

    p->setPen( CLEFT );
    p->drawLineSegments( bLeft );
    p->setPen( CTOP );
    p->drawLineSegments( bTop );
    p->setPen( CBOT );
    p->drawLineSegments( bBot );

    p->setWorldMatrix( wxm );
    p->setBrush( saveBrush );			// restore brush
    p->setPen( savePen );			// restore pen

#undef CMID
#undef CLEFT
#undef CTOP
#undef CBOT

}


/*!
  Draws a press-sensitive shape.
*/
void QMotifStyle::drawButton( QPainter *p, int x, int y, int w, int h,
				const QColorGroup &g, bool sunken, const QBrush* fill)
{
    qDrawShadePanel( p, x, y, w, h, g, sunken, defaultFrameWidth(),
		     fill ? fill : (sunken ?
				    &g.brush( QColorGroup::Mid )      :
				    &g.brush( QColorGroup::Button ) ));
}

/*! \reimp */

void QMotifStyle::drawBevelButton( QPainter *p, int x, int y, int w, int h,
				const QColorGroup &g, bool sunken, const QBrush* fill)
{
    QMotifStyle::drawButton(p, x, y, w, h, g, sunken, fill);
}


/*! \reimp */

void
QMotifStyle::drawFocusRect( QPainter* p,
			    const QRect& r, const QColorGroup &g , const QColor* bg, bool atBorder)
{
    if (bg ) {
	int h,s,v;
	bg->hsv(&h,&s,&v);
	if (v >= 128)
	    p->setPen( Qt::black );
	else
	    p->setPen( Qt::white );
    }
    else
	p->setPen( g.foreground() );
    p->setBrush( NoBrush );
    if ( atBorder )
	p->drawRect( QRect( r.x()+1, r.y()+1, r.width()-2, r.height()-2 ) );
    else
	p->drawRect( r );

}


/*! \reimp */

void
QMotifStyle::drawPushButton( QPushButton* btn, QPainter *p)
{
    QColorGroup g = btn->colorGroup();
    int x1, y1, x2, y2;

    btn->rect().coords( &x1, &y1, &x2, &y2 );	// get coordinates

    p->setPen( g.foreground() );
    p->setBrush( QBrush(g.button(),NoBrush) );

    int diw = buttonDefaultIndicatorWidth();
    if ( btn->isDefault() || btn->autoDefault() ) {
	x1 += diw;
	y1 += diw;
	x2 -= diw;
	y2 -= diw;
    }

    QBrush fill;
    if ( btn->isDown() )
	fill = g.brush( QColorGroup::Mid );
    else if ( btn->isOn() )
	fill = QBrush( g.mid(), Dense4Pattern );
    else
	fill = g.brush( QColorGroup::Button );

    if ( btn->isDefault() ) {
	if ( diw == 0 ) {
	    QPointArray a;
	    a.setPoints( 9,
			 x1, y1, x2, y1, x2, y2, x1, y2, x1, y1+1,
			 x2-1, y1+1, x2-1, y2-1, x1+1, y2-1, x1+1, y1+1 );
	    p->setPen( g.shadow() );
	    p->drawPolyline( a );
	    x1 += 2;
	    y1 += 2;
	    x2 -= 2;
	    y2 -= 2;
	} else {
	    qDrawShadePanel( p, btn->rect(), g, TRUE );
	}
    }

    if ( !btn->isFlat() || btn->isOn() || btn->isDown() )
	drawButton( p, x1, y1, x2-x1+1, y2-y1+1, g, btn->isOn() || btn->isDown(),
		    &fill );

    if ( p->brush().style() != NoBrush )
	p->setBrush( NoBrush );
}



/*! \reimp */
void QMotifStyle::tabbarMetrics( const QTabBar* t, int& hframe, int& vframe, int& overlap)
{
    QCommonStyle::tabbarMetrics( t, hframe, vframe, overlap );
}

/*! \reimp */
void QMotifStyle::drawTab( QPainter* p, const QTabBar* tb, QTab* t , bool selected )
{
    QRect r( t->r );
    int o = defaultFrameWidth() > 1 ? 1 : 0;

    if ( tb->shape()  == QTabBar::RoundedAbove ) {
	if ( o ) {
	    p->setPen( tb->colorGroup().light() );
	    p->drawLine( r.left(), r.bottom(), r.right(), r.bottom() );
	    p->setPen( tb->colorGroup().light() );
	    p->drawLine( r.left(), r.bottom()-1, r.right(), r.bottom()-1 );
	    if ( r.left() == 0 )
		p->drawPoint( tb->rect().bottomLeft() );
	}
	else {
	    p->setPen( tb->colorGroup().light() );
	    p->drawLine( r.left(), r.bottom(), r.right(), r.bottom() );
	}

	if ( selected ) {
	    p->fillRect( QRect( r.left()+1, r.bottom()-o, r.width()-3, 2),
			 tb->palette().normal().brush( QColorGroup::Background ));
	    p->setPen( tb->colorGroup().background() );
// 	    p->drawLine( r.left()+1, r.bottom(), r.right()-2, r.bottom() );
// 	    if (o)
// 		p->drawLine( r.left()+1, r.bottom()-1, r.right()-2, r.bottom()-1 );
	    p->drawLine( r.left()+1, r.bottom(), r.left()+1, r.top()+2 );
	    p->setPen( tb->colorGroup().light() );
	} else {
	    p->setPen( tb->colorGroup().light() );
	    r.setRect( r.left() + 2, r.top() + 2,
		       r.width() - 4, r.height() - 2 );
	}

	p->drawLine( r.left(), r.bottom()-1, r.left(), r.top() + 2 );
	p->drawPoint( r.left()+1, r.top() + 1 );
	p->drawLine( r.left()+2, r.top(),
		     r.right() - 2, r.top() );
	p->drawPoint( r.left(), r.bottom());

	if ( o ) {
	    p->drawLine( r.left()+1, r.bottom(), r.left()+1, r.top() + 2 );
	    p->drawLine( r.left()+2, r.top()+1,
			 r.right() - 2, r.top()+1 );
	}

	p->setPen( tb->colorGroup().dark() );
	p->drawLine( r.right() - 1, r.top() + 2,
		     r.right() - 1, r.bottom() - 1 + (selected?o:-o));
	if ( o ) {
	    p->drawPoint( r.right() - 1, r.top() + 1 );
	    p->drawLine( r.right(), r.top() + 2, r.right(), r.bottom() - (selected?1:1+o));
	    p->drawPoint( r.right() - 1, r.top() + 1 );
	}
    } else if ( tb->shape()  == QTabBar::RoundedBelow ) {
	if ( selected ) {
	    p->fillRect( QRect( r.left()+1, r.top(), r.width()-3, 1),
			 tb->palette().normal().brush( QColorGroup::Background ));
	    p->setPen( tb->colorGroup().background() );
// 	    p->drawLine( r.left()+1, r.top(), r.right()-2, r.top() );
	    p->drawLine( r.left()+1, r.top(), r.left()+1, r.bottom()-2 );
	    p->setPen( tb->colorGroup().dark() );
	} else {
	    p->setPen( tb->colorGroup().dark() );
	    p->drawLine( r.left(), r.top(), r.right(), r.top() );
	    r.setRect( r.left() + 2, r.top(),
		       r.width() - 4, r.height() - 2 );
	}

	p->drawLine( r.right() - 1, r.top(),
		     r.right() - 1, r.bottom() - 2 );
	p->drawPoint( r.right() - 2, r.bottom() - 2 );
	p->drawLine( r.right() - 2, r.bottom() - 1,
		     r.left() + 1, r.bottom() - 1 );
	p->drawPoint( r.left() + 1, r.bottom() - 2 );

	if (defaultFrameWidth() > 1) {
	    p->drawLine( r.right(), r.top(),
			 r.right(), r.bottom() - 1 );
	    p->drawPoint( r.right() - 1, r.bottom() - 1 );
	    p->drawLine( r.right() - 1, r.bottom(),
			 r.left() + 2, r.bottom() );
	}

	p->setPen( tb->colorGroup().light() );
	p->drawLine( r.left(), r.top(),
		     r.left(), r.bottom() - 2 );

    } else {
	QCommonStyle::drawTab( p, tb, t, selected );
    }

}

/*! \reimp */
void QMotifStyle::drawTabMask( QPainter* p,  const  QTabBar* tb, QTab* t, bool selected )
{
    QCommonStyle::drawTabMask(p, tb, t, selected );
}


#define HORIZONTAL	(sb->orientation() == QScrollBar::Horizontal)
#define VERTICAL	!HORIZONTAL
#define MOTIF_BORDER	defaultFrameWidth()
#define SLIDER_MIN	9 // ### motif says 6 but that's too small


/*! \reimp */

void QMotifStyle::scrollBarMetrics( const QScrollBar* sb, int &sliderMin, int &sliderMax, int &sliderLength, int &buttonDim )
{
    int maxLength;
    int b = MOTIF_BORDER;
    int length = HORIZONTAL ? sb->width()  : sb->height();
    int extent = HORIZONTAL ? sb->height() : sb->width();

    if ( length > ( extent - b*2 - 1 )*2 + b*2  )
	buttonDim = extent - b*2;
    else
	buttonDim = ( length - b*2 )/2 - 1;

    sliderMin = b + buttonDim;
    maxLength  = length - b*2 - buttonDim*2;

    if ( sb->maxValue() == sb->minValue() ) {
	sliderLength = maxLength;
    } else {
	uint range = sb->maxValue()-sb->minValue();
	sliderLength = (sb->pageStep()*maxLength)/
			(range + sb->pageStep());
	if ( sliderLength < SLIDER_MIN || range > INT_MAX/2 )
	    sliderLength = SLIDER_MIN;
	if ( sliderLength > maxLength )
	    sliderLength = maxLength;
    }
    sliderMax = sliderMin + maxLength - sliderLength;

}


/*! \reimp */

void QMotifStyle::drawScrollBarControls( QPainter* p, const QScrollBar* sb,
					 int sliderStart, uint controls,
					 uint activeControl )
{
#define ADD_LINE_ACTIVE ( activeControl == AddLine )
#define SUB_LINE_ACTIVE ( activeControl == SubLine )
    QColorGroup g  = sb->colorGroup();

    int sliderMin, sliderMax, sliderLength, buttonDim;
    scrollBarMetrics( sb, sliderMin, sliderMax, sliderLength, buttonDim );

    if ( controls == (AddLine | SubLine | AddPage | SubPage | Slider | First | Last ) )
	qDrawShadePanel( p, sb->rect(), g, TRUE );

    if (sliderStart > sliderMax) { // sanity check
	sliderStart = sliderMax;
    }

    int b = MOTIF_BORDER;
    int dimB = buttonDim;
    QRect addB;
    QRect subB;
    QRect addPageR;
    QRect subPageR;
    QRect sliderR;
    int addX, addY, subX, subY;
    int length = HORIZONTAL ? sb->width()  : sb->height();
    int extent = HORIZONTAL ? sb->height() : sb->width();

    if ( HORIZONTAL ) {
	subY = addY = ( extent - dimB ) / 2;
	subX = b;
	addX = length - dimB - b;
    } else {
	subX = addX = ( extent - dimB ) / 2;
	subY = b;
	addY = length - dimB - b;
    }

    subB.setRect( subX,subY,dimB,dimB );
    addB.setRect( addX,addY,dimB,dimB );

    int sliderEnd = sliderStart + sliderLength;
    int sliderW = extent - b*2;
    if ( HORIZONTAL ) {
	subPageR.setRect( subB.right() + 1, b,
			  sliderStart - subB.right() - 1 , sliderW );
	addPageR.setRect( sliderEnd, b, addX - sliderEnd, sliderW );
	sliderR .setRect( sliderStart, b, sliderLength, sliderW );
    } else {
	subPageR.setRect( b, subB.bottom() + 1, sliderW,
			  sliderStart - subB.bottom() - 1 );
	addPageR.setRect( b, sliderEnd, sliderW, addY - sliderEnd );
	sliderR .setRect( b, sliderStart, sliderW, sliderLength );
    }

    if ( controls & AddLine )
	drawArrow( p, VERTICAL ? DownArrow : RightArrow,
		   ADD_LINE_ACTIVE, addB.x(), addB.y(),
		   addB.width(), addB.height(), g, TRUE );
    if ( controls & SubLine )
	drawArrow( p, VERTICAL ? UpArrow : LeftArrow,
		   SUB_LINE_ACTIVE, subB.x(), subB.y(),
		   subB.width(), subB.height(), g, TRUE );

    QBrush fill = g.brush( QColorGroup::Mid );
    if (sb->backgroundPixmap() ){
	fill = QBrush( g.mid(), *sb->backgroundPixmap() );
    }

    if ( controls & SubPage )
	p->fillRect( subPageR, fill );

    if ( controls & AddPage )
	p->fillRect( addPageR, fill );

    if ( controls & Slider ) {
	QPoint bo = p->brushOrigin();
	p->setBrushOrigin(sliderR.topLeft());
	if ( sliderR.isValid() )
	    drawBevelButton( p, sliderR.x(), sliderR.y(),
			     sliderR.width(), sliderR.height(), g,
			     FALSE, &g.brush( QColorGroup::Button ) );

	//	qDrawShadePanel( p, sliderR, g, FALSE, 2, &g.fillButton() );
	p->setBrushOrigin(bo);
    }

}


/*!\reimp
 */
int QMotifStyle::sliderLength() const
{
    return 30;
}

/*!\reimp
 */
void QMotifStyle::drawSlider( QPainter *p,
			     int x, int y, int w, int h,
			     const QColorGroup &g,
			      Orientation orient, bool, bool )
{
    drawBevelButton( p, x, y, w, h, g, FALSE, &g.brush( QColorGroup::Button) );
    if ( orient == Horizontal ) {
	QCOORD mid = x + w / 2;
	qDrawShadeLine( p, mid,  y , mid,  y + h - 2,
			g, TRUE, 1);
    } else {
	QCOORD mid = y +h / 2;
	qDrawShadeLine( p, x, mid,  x + w - 2, mid,
			g, TRUE, 1);
    }
}

/*!\reimp
 */
void QMotifStyle::drawSliderGroove( QPainter *p,
				      int x, int y, int w, int h,
				      const QColorGroup& g, QCOORD /*c */,
				      Orientation )
{
    qDrawShadePanel( p, x, y, w, h, g, TRUE, 1, &g.brush( QColorGroup::Mid ) );
}


/*! \reimp
*/

int QMotifStyle::splitterWidth() const
{
    return QMAX( 10, QApplication::globalStrut().width() );
}


/*! \reimp
*/

void QMotifStyle::drawSplitter( QPainter *p, int x, int y, int w, int h,
  const QColorGroup &g, Orientation orient)
{
    const int motifOffset = 10;
    int sw = splitterWidth();
    if ( orient == Horizontal ) {
	    QCOORD xPos = x + w/2;
	    QCOORD kPos = motifOffset;
	    QCOORD kSize = sw - 2;

	    qDrawShadeLine( p, xPos, kPos + kSize - 1 ,
			    xPos, h, g );
	    qDrawShadePanel( p, xPos-sw/2+1, kPos,
			     kSize, kSize, g, FALSE, 1,
			     &g.brush( QColorGroup::Button ));
	    qDrawShadeLine( p, xPos, 0, xPos, kPos, g );
	} else {
	    QCOORD yPos = y + h/2;
	    QCOORD kPos = w - motifOffset - sw;
	    QCOORD kSize = sw - 2;

	    qDrawShadeLine( p, 0, yPos, kPos, yPos, g );
	    qDrawShadePanel( p, kPos, yPos-sw/2+1,
			     kSize, kSize, g, FALSE, 1,
			     &g.brush( QColorGroup::Button ));
	    qDrawShadeLine( p, kPos + kSize -1, yPos,
			    w, yPos, g );
	}

}

/*! \reimp
*/
void QMotifStyle::polishPopupMenu( QPopupMenu* p)
{
    p->setLineWidth( defaultFrameWidth() );
    p->setMouseTracking( FALSE );
    if ( !p->testWState( WState_Polished ) )
	p->setCheckable( FALSE );
    p->setLineWidth( 2 );
}


static const int motifItemFrame		= 2;	// menu item frame width
static const int motifSepHeight		= 2;	// separator item height
static const int motifItemHMargin	= 3;	// menu item hor text margin
static const int motifItemVMargin	= 2;	// menu item ver text margin
static const int motifArrowHMargin	= 6;	// arrow horizontal margin
static const int motifTabSpacing	= 12;	// space between text and tab
static const int motifCheckMarkHMargin	= 2;	// horiz. margins of check mark
static const int motifCheckMarkSpace    = 12;


/*! \reimp
*/
void QMotifStyle::drawCheckMark( QPainter *p, int x, int y, int w, int h,
				 const QColorGroup &g,
				 bool act, bool dis )
{
    const int markW = 6;
    const int markH = 6;
    int posX = x + ( w - markW )/2 - 1;
    int posY = y + ( h - markH )/2;

    if ( defaultFrameWidth() < 2) {
	// Could do with some optimizing/caching...
	QPointArray a( 7*2 );
	int i, xx, yy;
	xx = posX;
	yy = 3 + posY;
	for ( i=0; i<3; i++ ) {
	    a.setPoint( 2*i,   xx, yy );
	    a.setPoint( 2*i+1, xx, yy+2 );
	    xx++; yy++;
	}
	yy -= 2;
	for ( i=3; i<7; i++ ) {
	    a.setPoint( 2*i,   xx, yy );
	    a.setPoint( 2*i+1, xx, yy+2 );
	    xx++; yy--;
	}
	if ( dis && !act ) {
	    int pnt;
	    p->setPen( g.highlightedText() );
	    QPoint offset(1,1);
	    for ( pnt = 0; pnt < (int)a.size(); pnt++ )
		a[pnt] += offset;
	    p->drawLineSegments( a );
	    for ( pnt = 0; pnt < (int)a.size(); pnt++ )
		a[pnt] -= offset;
	}
	p->setPen( g.text() );
	p->drawLineSegments( a );

	qDrawShadePanel( p, posX-2, posY-2, markW+4, markH+6, g, TRUE,
			 defaultFrameWidth());
    }
    else {
	qDrawShadePanel( p, posX, posY, markW, markH, g, TRUE,
		    defaultFrameWidth(), &g.brush( QColorGroup::Mid ) );
    }
}


/*! \reimp
*/
int QMotifStyle::extraPopupMenuItemWidth( bool checkable, int maxpmw, QMenuItem* mi, const QFontMetrics& /* fm */)
{
    int w = 2*motifItemHMargin + 2*motifItemFrame; // a little bit of border can never harm

    if ( mi->isSeparator() )
	return 10; // arbitrary
    else if ( mi->pixmap() )
	w += mi->pixmap()->width();	// pixmap only

    if ( !mi->text().isNull() ) {
	if ( mi->text().find('\t') >= 0 )	// string contains tab
	    w += motifTabSpacing;
    }

    if ( checkable )
	maxpmw = QMAX( maxpmw, motifCheckMarkSpace );
    w += maxpmw;

    if ( maxpmw > 0 || checkable ) // we have a check-column ( iconsets or checkmarks)
	w += motifCheckMarkHMargin; // add space to separate the columns

    return w;
}

/*! \reimp
*/
int QMotifStyle::popupMenuItemHeight( bool /* checkable*/, QMenuItem* mi, const QFontMetrics& fm )
{
    int h = 0;
    if ( mi->isSeparator() ) {			// separator height
	h = motifSepHeight;
    } else if ( mi->pixmap() ) {		// pixmap height
	h = mi->pixmap()->height() + 2*motifItemFrame;
    } else {					// text height
	h = fm.height() + 2*motifItemVMargin + 2*motifItemFrame;
    }
    if ( !mi->isSeparator() && mi->iconSet() != 0 ) {
	h = QMAX( h, mi->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).height() + 2*motifItemFrame );
	h += 2;				// Room for check rectangle
    }
    if ( mi->custom() )
	h = QMAX( h, mi->custom()->sizeHint().height() + 2*motifItemVMargin + 2*motifItemFrame );
    return h;
}

/*! \reimp
*/
void QMotifStyle::drawPopupMenuItem( QPainter* p, bool checkable, int maxpmw, int tab, QMenuItem* mi,
				     const QPalette& pal,
				     bool act, bool enabled, int x, int y, int w, int h)
{
    const QColorGroup & g = pal.active();
    bool dis	  = !enabled;
    QColorGroup itemg = dis ? pal.disabled() : pal.active();

    if ( checkable )
	maxpmw = QMAX( maxpmw, motifCheckMarkSpace );
    int checkcol	  =     maxpmw;

    if ( mi && mi->isSeparator() ) {			// draw separator
	p->setPen( g.dark() );
	p->drawLine( x, y, x+w, y );
	p->setPen( g.light() );
	p->drawLine( x, y+1, x+w, y+1 );
	return;
    }

    int pw = motifItemFrame;

    if ( act && !dis ) {			// active item frame
	if (defaultFrameWidth() > 1)
	    qDrawShadePanel( p, x, y, w, h, g, FALSE, pw,
			     &g.brush( QColorGroup::Button ) );
	else
	    qDrawShadePanel( p, x+1, y+1, w-2, h-2, g, TRUE, 1,
			     &g.brush( QColorGroup::Button ) );
    }
    else				// incognito frame
	p->fillRect(x, y, w, h, g.brush( QColorGroup::Button ));

    if ( !mi )
	return;

    if ( mi->isChecked() ) {
	if ( mi->iconSet() ) {
	    qDrawShadePanel( p, x+motifItemFrame, y+motifItemFrame, checkcol, h-2*motifItemFrame,
			     g, TRUE, 1, &g.brush( QColorGroup::Midlight ) );
	}
    } else if ( !act ) {
	p->fillRect(x+motifItemFrame, y+motifItemFrame, checkcol, h-2*motifItemFrame,
		    g.brush( QColorGroup::Button ));
    }

    if ( mi->iconSet() ) {		// draw iconset
	QIconSet::Mode mode = QIconSet::Normal; // no disabled icons in Motif
	if (act && !dis )
	    mode = QIconSet::Active;
	QPixmap pixmap = mi->iconSet()->pixmap( QIconSet::Small, mode );
	int pixw = pixmap.width();
	int pixh = pixmap.height();
	QRect cr( x + motifItemFrame, y+motifItemFrame, checkcol, h-2*motifItemFrame );
	QRect pmr( 0, 0, pixw, pixh );
	pmr.moveCenter( cr.center() );
	p->setPen( itemg.text() );
	p->drawPixmap( pmr.topLeft(), pixmap );

    } else  if ( checkable ) {	// just "checking"...
	int mw = checkcol;
	int mh = h - 2*motifItemFrame;
	if ( mi->isChecked() ) {
	    drawCheckMark( p, x+motifItemFrame,
			   y+motifItemFrame, mw, mh, itemg, act, dis );
	}
    }


    p->setPen( g.buttonText() );

    QColor discol;
    if ( dis ) {
	discol = itemg.text();
	p->setPen( discol );
    }

    int xm = motifItemFrame + checkcol + motifItemHMargin;

    if ( mi->custom() ) {
	int m = motifItemVMargin;
	p->save();
	mi->custom()->paint( p, itemg, act, enabled,
			     x+xm, y+m, w-xm-tab+1, h-2*m );
	p->restore();
    }
    QString s = mi->text();
    if ( !s.isNull() ) {			// draw text
	int t = s.find( '\t' );
	int m = motifItemVMargin;
	const int text_flags = AlignVCenter|ShowPrefix | DontClip | SingleLine;
	if ( t >= 0 ) {				// draw tab text
	    p->drawText( x+w-tab-motifItemHMargin-motifItemFrame,
			 y+m, tab, h-2*m, text_flags, s.mid( t+1 ) );
	}
	p->drawText( x+xm, y+m, w-xm-tab+1, h-2*m, text_flags, s, t );
    } else if ( mi->pixmap() ) {			// draw pixmap
	QPixmap *pixmap = mi->pixmap();
	if ( pixmap->depth() == 1 )
	    p->setBackgroundMode( OpaqueMode );
	p->drawPixmap( x+xm, y+motifItemFrame, *pixmap );
	if ( pixmap->depth() == 1 )
	    p->setBackgroundMode( TransparentMode );
    }
    if ( mi->popup() ) {			// draw sub menu arrow
	int dim = (h-2*motifItemFrame) / 2;
	if ( act ) {
	    drawArrow( p, RightArrow,
		       mi->isEnabled(),
		       x+w - motifArrowHMargin - motifItemFrame - dim,  y+h/2-dim/2,
		       dim, dim, g,
		       mi->isEnabled() );
	} else {
	    drawArrow( p, RightArrow,
		       FALSE,
		       x+w - motifArrowHMargin - motifItemFrame - dim,  y+h/2-dim/2,
		       dim, dim, g, mi->isEnabled() );
	}
    }
}

static int get_combo_extra_width( int h, int *return_awh=0 )
{
    int awh;
    if ( h < 8 ) {
	awh = 6;
    } else if ( h < 14 ) {
	awh = h - 2;
    } else {
	awh = h/2;
    }
    if ( return_awh )
	*return_awh = awh;
    return awh*3/2;
}


static void get_combo_parameters( const QRect &r,
				  int &ew, int &awh, int &ax,
				  int &ay, int &sh, int &dh,
				  int &sy )
{
    ew = get_combo_extra_width( r.height(), &awh );

    sh = (awh+3)/4;
    if ( sh < 3 )
	sh = 3;
    dh = sh/2 + 1;

    ay = r.y() + (r.height()-awh-sh-dh)/2;
    if ( ay < 0 ) {
	//panic mode
	ay = 0;
	sy = r.height();
    } else {
	sy = ay+awh+dh;
    }
    ax = r.x() + r.width() - ew +(ew-awh)/2;
}

/*! \reimp
 */
void QMotifStyle::drawComboButton( QPainter *p, int x, int y, int w, int h,
				    const QColorGroup &g,
				    bool /* sunken */,
				    bool editable,
				    bool /*enabled */,
				    const QBrush *fb )
{
    QBrush fill = fb ? *fb : g.brush( QColorGroup::Button );

    int awh, ax, ay, sh, sy, dh, ew;
    get_combo_parameters( buttonRect(x,y,w,h), ew, awh, ax, ay, sh, dh, sy );

    drawButton( p, x, y, w, h, g, FALSE, &fill );

    qDrawArrow( p, DownArrow, MotifStyle, FALSE,
		ax, ay, awh, awh, g, TRUE );

    p->setPen( g.light() );
    p->drawLine( ax, sy, ax+awh-1, sy );
    p->drawLine( ax, sy, ax, sy+sh-1 );
    p->setPen( g.dark() );
    p->drawLine( ax+1, sy+sh-1, ax+awh-1, sy+sh-1 );
    p->drawLine( ax+awh-1, sy+1, ax+awh-1, sy+sh-1 );

    if ( editable ) {
	QRect r( comboButtonRect(x,y,w,h) );
	qDrawShadePanel( p, QRect(r.x()-1, r.y()-1, r.width()+2, r.height()+2), g, TRUE, 1, &fill );
    }
}


/*! \reimp
 */
QRect QMotifStyle::comboButtonRect( int x, int y, int w, int h)
{
    QRect r = buttonRect( x, y, w, h );
    int ew = get_combo_extra_width( r.height() );
    return QRect(r.x()+1, r.y()+1, r.width()-2-ew, r.height()-2);
}

/*! \reimp
 */
QRect QMotifStyle::comboButtonFocusRect( int x, int y, int w, int h)
{
    int awh, ax, ay, sh, sy, dh, ew;
    get_combo_parameters( buttonRect( x, y, w, h ),
			  ew, awh, ax, ay, sh, dh, sy );
    return QRect(ax-2, ay-2, awh+4, awh+sh+dh+4);
}
#endif
