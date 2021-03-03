/****************************************************************************
** $Id: qt/src/widgets/qplatinumstyle.cpp   2.3.2   edited 2001-01-26 $
**
** Implementation of Platinum-like style class
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

#include "qplatinumstyle.h"
#ifndef QT_NO_STYLE_PLATINUM
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
#include <limits.h>

// NOT REVISED
/*!
  \class QPlatinumStyle qplatinumstyle.h
  \brief Platinum Look and Feel
  \ingroup appearance

  This class implements the Platinum look and feel. It's an
  experimental class that tries to resemble a Macinosh-like GUI style
  with the QStyle system. The emulation is, however, far from being
  perfect yet.
*/


/*!
    Constructs a QPlatinumStyle
*/
QPlatinumStyle::QPlatinumStyle()
{
    setButtonDefaultIndicatorWidth( 3 );
}

/*!
  Destructs the style.
*/
QPlatinumStyle::~QPlatinumStyle()
{
}


/*! \reimp */
void QPlatinumStyle::drawPopupPanel( QPainter *p, int x, int y, int w, int h,
			       const QColorGroup &g,  int lineWidth,
			       const QBrush *fill )
{
    QWindowsStyle::drawPopupPanel( p, x, y, w, h, g, lineWidth, fill );
}

/*!
  Draws a press-sensitive shape.
*/
void QPlatinumStyle::drawButton( QPainter *p, int x, int y, int w, int h,
				const QColorGroup &g, bool sunken, const QBrush* fill)
{

    QPen oldPen = p->pen();

     if (!sunken) {
	 p->fillRect(x+3, y+3, w-6, h-6,fill ? *fill :
					 g.brush( QColorGroup::Button ));
	 // the bright side
	 p->setPen(g.shadow());
	 p->drawLine(x, y, x+w-1, y);
	 p->drawLine(x, y, x, y+h-1);

	 p->setPen(g.button());
	 p->drawLine(x+1, y+1, x+w-2, y+1);
	 p->drawLine(x+1, y+1, x+1, y+h-2);

	 p->setPen(g.light());
	 p->drawLine(x+2, y+2, x+2, y+h-2);
	 p->drawLine(x+2, y+2, x+w-2, y+2);


	 // the dark side!

	 p->setPen(g.mid());
	 p->drawLine(x+3, y+h-3 ,x+w-3, y+h-3);
	 p->drawLine(x+w-3, y+3, x+w-3, y+h-3);

	 p->setPen(g.dark());
	 p->drawLine(x+2, y+h-2 ,x+w-2, y+h-2);
	 p->drawLine(x+w-2, y+2, x+w-2, y+h-2);

	 p->setPen(g.shadow());
	 p->drawLine(x+1, y+h-1,x+w-1, y+h-1);
	 p->drawLine(x+w-1, y, x+w-1, y+h-1);


	 // top left corner:
	 p->setPen(g.background());
	 p->drawPoint(x, y);
	 p->drawPoint(x+1, y);
	 p->drawPoint(x, y+1);
	 p->setPen(g.shadow());
	 p->drawPoint(x+1, y+1);
	 p->setPen(g.button());
 	 p->drawPoint(x+2, y+2);
	 p->setPen(white);
 	 p->drawPoint(x+3, y+3);
	 // bottom left corner:
	 p->setPen(g.background());
	 p->drawPoint(x, y+h-1);
	 p->drawPoint(x+1, y+h-1);
	 p->drawPoint(x, y+h-2);
	 p->setPen(g.shadow());
	 p->drawPoint(x+1, y+h-2);
	 p->setPen(g.dark());
	 p->drawPoint(x+2, y+h-3);
	 // top right corner:
	 p->setPen(g.background());
	 p->drawPoint(x+w-1, y);
	 p->drawPoint(x+w-2, y);
	 p->drawPoint(x+w-1, y+1);
	 p->setPen(g.shadow());
	 p->drawPoint(x+w-2, y+1);
	 p->setPen(g.dark());
	 p->drawPoint(x+w-3, y+2);
	 // bottom right corner:
	 p->setPen(g.background());
	 p->drawPoint(x+w-1, y+h-1);
	 p->drawPoint(x+w-2, y+h-1);
	 p->drawPoint(x+w-1, y+h-2);
	 p->setPen(g.shadow());
	 p->drawPoint(x+w-2, y+h-2);
	 p->setPen(g.dark());
	 p->drawPoint(x+w-3, y+h-3);
	 p->setPen(g.mid());
	 p->drawPoint(x+w-4, y+h-4);

     }
     else {
	 p->fillRect(x+2, y+2, w-4, h-4,fill ? *fill :
					   g.brush( QColorGroup::Dark ));

	 // the dark side
	 p->setPen(g.shadow());
	 p->drawLine(x, y, x+w-1, y);
	 p->drawLine(x, y, x, y+h-1);

	 p->setPen(g.dark().dark());
	 p->drawLine(x+1, y+1, x+w-2, y+1);
	 p->drawLine(x+1, y+1, x+1, y+h-2);


	 // the bright side!

	 p->setPen(g.button());
	 p->drawLine(x+1, y+h-2 ,x+w-2, y+h-2);
	 p->drawLine(x+w-2, y+1, x+w-2, y+h-2);

	 p->setPen(g.dark());
	 p->drawLine(x, y+h-1,x+w-1, y+h-1);
	 p->drawLine(x+w-1, y, x+w-1, y+h-1);

	 // top left corner:
	 p->setPen(g.background());
	 p->drawPoint(x, y);
	 p->drawPoint(x+1, y);
	 p->drawPoint(x, y+1);
	 p->setPen(g.shadow());
	 p->drawPoint(x+1, y+1);
	 p->setPen(g.dark().dark());
	 p->drawPoint(x+3, y+3);
	 // bottom left corner:
	 p->setPen(g.background());
	 p->drawPoint(x, y+h-1);
	 p->drawPoint(x+1, y+h-1);
	 p->drawPoint(x, y+h-2);
	 p->setPen(g.shadow());
	 p->drawPoint(x+1, y+h-2);
	 // top right corner:
	 p->setPen(g.background());
	 p->drawPoint(x+w-1, y);
	 p->drawPoint(x+w-2, y);
	 p->drawPoint(x+w-1, y+1);
	 p->setPen(g.shadow());
	 p->drawPoint(x+w-2, y+1);
	 // bottom right corner:
	 p->setPen(g.background());
	 p->drawPoint(x+w-1, y+h-1);
	 p->drawPoint(x+w-2, y+h-1);
	 p->drawPoint(x+w-1, y+h-2);
	 p->setPen(g.shadow());
	 p->drawPoint(x+w-2, y+h-2);
	 p->setPen(g.dark());
	 p->drawPoint(x+w-3, y+h-3);
	 p->setPen(g.mid());
	 p->drawPoint(x+w-4, y+h-4);


     }

     //     // top left corner:
//     p->setPen(g.background());
//     p->drawPoint(x, y);
//     p->drawPoint(x, y);


    p->setPen(oldPen);
}

/*! \reimp */

QRect QPlatinumStyle::buttonRect( int x, int y, int w, int h){
    QRect r = QCommonStyle::buttonRect(x,y,w,h);
    r.setTop( r.top()+1);
    r.setLeft( r.left()+1);
    r.setBottom( r.bottom()-1);
    r.setRight( r.right()-1);
    return r;
}

/*!
  mixes two colors to a new colors
  */
QColor QPlatinumStyle::mixedColor(const QColor &c1, const QColor &c2)
{
    int h1,s1,v1,h2,s2,v2;
    c1.hsv(&h1,&s1,&v1);
    c2.hsv(&h2,&s2,&v2);
    return QColor( (h1+h2)/2, (s1+s2)/2, (v1+v2)/2, QColor::Hsv );
}

/*! \reimp */

void QPlatinumStyle::drawBevelButton( QPainter *p, int x, int y, int w, int h,
				const QColorGroup &g, bool sunken, const QBrush* fill)
{
    QPen oldPen = p->pen();

    // small or non-square bevel buttons are drawn in a small style, others in a big style.
    if ( w * h < 1600 || QABS(w-h) > 10) {
	// small buttons

	if (!sunken) {
	    p->fillRect(x+2, y+2, w-4, h-4,fill ? *fill :
					     g.brush( QColorGroup::Button ));
	    // the bright side
	    p->setPen(g.dark());
	    p->drawLine(x, y, x+w-1, y);
	    p->drawLine(x, y, x, y+h-1);

	    p->setPen(g.light());
	    p->drawLine(x+1, y+1, x+w-2, y+1);
	    p->drawLine(x+1, y+1, x+1, y+h-2);

	    // the dark side!

	    p->setPen(g.mid());
	    p->drawLine(x+2, y+h-2 ,x+w-2, y+h-2);
	    p->drawLine(x+w-2, y+2, x+w-2, y+h-3);

	    p->setPen(g.dark().dark());
	    p->drawLine(x+1, y+h-1,x+w-1, y+h-1);
	    p->drawLine(x+w-1, y+1, x+w-1, y+h-2);


	}
	else {
	    p->fillRect(x+2, y+2, w-4, h-4,fill ? *fill :
					       g.brush( QColorGroup::Mid ));

	    // the dark side
	    p->setPen(g.dark().dark());
	    p->drawLine(x, y, x+w-1, y);
	    p->drawLine(x, y, x, y+h-1);

	    p->setPen(g.mid().dark());
	    p->drawLine(x+1, y+1, x+w-2, y+1);
	    p->drawLine(x+1, y+1, x+1, y+h-2);


	    // the bright side!

	    p->setPen(g.button());
	    p->drawLine(x+1, y+h-2 ,x+w-2, y+h-2);
	    p->drawLine(x+w-2, y+1, x+w-2, y+h-2);

	    p->setPen(g.dark());
	    p->drawLine(x, y+h-1,x+w-1, y+h-1);
	    p->drawLine(x+w-1, y, x+w-1, y+h-1);
	}
    }
    else {
	// big ones

	if (!sunken) {
	    p->fillRect(x+3, y+3, w-6, h-6,fill ? * fill :
					    g.brush( QColorGroup::Button ));
	    // the bright side
	    p->setPen(g.button().dark());
	    p->drawLine(x, y, x+w-1, y);
	    p->drawLine(x, y, x, y+h-1);

	    p->setPen(g.button());
	    p->drawLine(x+1, y+1, x+w-2, y+1);
	    p->drawLine(x+1, y+1, x+1, y+h-2);

	    p->setPen(g.light());
	    p->drawLine(x+2, y+2, x+2, y+h-2);
	    p->drawLine(x+2, y+2, x+w-2, y+2);


	 // the dark side!

	    p->setPen(g.mid());
	    p->drawLine(x+3, y+h-3 ,x+w-3, y+h-3);
	    p->drawLine(x+w-3, y+3, x+w-3, y+h-3);

	    p->setPen(g.dark());
	    p->drawLine(x+2, y+h-2 ,x+w-2, y+h-2);
	    p->drawLine(x+w-2, y+2, x+w-2, y+h-2);

	    p->setPen(g.dark().dark());
	    p->drawLine(x+1, y+h-1,x+w-1, y+h-1);
	    p->drawLine(x+w-1, y+1, x+w-1, y+h-1);


	}
	else {
	    p->fillRect(x+3, y+3, w-6, h-6,fill ? *fill :
						  g.brush( QColorGroup::Mid ));

	    // the dark side
	    p->setPen(g.dark().dark().dark());
	    p->drawLine(x, y, x+w-1, y);
	    p->drawLine(x, y, x, y+h-1);

	    p->setPen(g.dark().dark());
	    p->drawLine(x+1, y+1, x+w-2, y+1);
	    p->drawLine(x+1, y+1, x+1, y+h-2);

	    p->setPen(g.mid().dark());
	    p->drawLine(x+2, y+2, x+2, y+w-2);
	    p->drawLine(x+2, y+2, x+w-2, y+2);


	    // the bright side!

	    p->setPen(g.button());
	    p->drawLine(x+2, y+h-3 ,x+w-3, y+h-3);
	    p->drawLine(x+w-3, y+3, x+w-3, y+h-3);

	    p->setPen(g.midlight());
	    p->drawLine(x+1, y+h-2 ,x+w-2, y+h-2);
	    p->drawLine(x+w-2, y+1, x+w-2, y+h-2);

	    p->setPen(g.dark());
	    p->drawLine(x, y+h-1,x+w-1, y+h-1);
	    p->drawLine(x+w-1, y, x+w-1, y+h-1);


	    // corners
	    p->setPen( mixedColor(g.dark().dark().dark(), g.dark()) );
	    p->drawPoint( x, y+h-1 );
	    p->drawPoint( x+w-1, y);

	    p->setPen( mixedColor(g.dark().dark(), g.midlight() ) );
	    p->drawPoint( x+1, y+h-2 );
	    p->drawPoint( x+w-2, y+1);

	    p->setPen( mixedColor(g.mid().dark(), g.button() ) );
	    p->drawPoint( x+2, y+h-3 );
	    p->drawPoint( x+w-3, y+2);
	}
    }
    p->setPen(oldPen);

}

/*! \reimp */

void
QPlatinumStyle::drawPushButton( QPushButton* btn, QPainter *p)
{
    QColorGroup g = btn->colorGroup();
    int x1, y1, x2, y2;
    bool useBevelButton;

    btn->rect().coords( &x1, &y1, &x2, &y2 );	// get coordinates

    p->setPen( g.foreground() );
    p->setBrush( QBrush(g.button(),NoBrush) );

    QBrush fill;
    if ( btn->isDown() )
	fill = g.brush( QColorGroup::Dark );
    else if ( btn->isOn() )
	fill = QBrush( g.mid(), Dense4Pattern );
    else
	fill = g.brush( QColorGroup::Button );

    // small or square image buttons as well as toggle buttons are
    // bevel buttons (what a heuristic....)
    if ( btn->isToggleButton()
	 || ( btn->pixmap() &&
	      (btn->width() * btn->height() < 1600 || QABS( btn->width() - btn->height()) < 10 )) )
	useBevelButton = TRUE;
    else
	useBevelButton = FALSE;

    int diw = buttonDefaultIndicatorWidth();
    if (btn->isDefault() ) {
	x1 += 1;
	y1 += 1;
	x2 -= 1;
	y2 -= 1;
	QColorGroup g2( g );
	g2.setColor( QColorGroup::Button,  g.mid() );
	if( useBevelButton )
	    drawBevelButton( p, x1, y1, x2-x1+1, y2-y1+1, g, FALSE, &fill );
	else
	    drawButton( p, x1, y1, x2-x1+1, y2-y1+1, g2, FALSE, &fill );
    }

    if ( btn->isDefault() || btn->autoDefault() ) {
	x1 += diw;
	y1 += diw;
	x2 -= diw;
	y2 -= diw;
    }

    if ( !btn->isFlat() || btn->isOn() || btn->isDown() ) {
	if( useBevelButton)
	    drawBevelButton( p, x1, y1, x2-x1+1, y2-y1+1, g, btn->isOn() || btn->isDown(),
			     &fill );
	else
	    drawButton( p, x1, y1, x2-x1+1, y2-y1+1, g, btn->isOn() || btn->isDown(),
			&fill );
    }

    if ( p->brush().style() != NoBrush )
	p->setBrush( NoBrush );
}

/*! \reimp */

void QPlatinumStyle::drawPushButtonLabel( QPushButton* btn, QPainter *p)
{
    bool on = btn->isDown() || btn->isOn();
    QRect r = pushButtonContentsRect( btn );
    int x, y, w, h;
    r.rect( &x, &y, &w, &h );
    if ( btn->isMenuButton() ) {
	int dx = menuButtonIndicatorWidth( btn->height() );

	QColorGroup g( btn->colorGroup() );
	int xx = x+w-dx-4;
	int yy = y-3;
	int hh = h+6;

	if ( !on ) {
	    p->setPen( g.mid() );
	    p->drawLine(xx, yy+2, xx, yy+hh-3);
	    p->setPen( g.button() );
	    p->drawLine(xx+1, yy+1, xx+1, yy+hh-2);
	    p->setPen( g.light() );
	    p->drawLine(xx+2, yy+2, xx+2, yy+hh-2);
	}
	drawArrow( p, DownArrow, FALSE,
		   x+w-dx-1, y+2, dx, h-4,
		   btn->colorGroup(),
		   btn->isEnabled() );
	w -= dx;
    }

    if ( btn->iconSet() && !btn->iconSet()->isNull() ) {
	QIconSet::Mode mode = btn->isEnabled()
			      ? QIconSet::Normal : QIconSet::Disabled;
	if ( mode == QIconSet::Normal && btn->hasFocus() )
	    mode = QIconSet::Active;
	QPixmap pixmap = btn->iconSet()->pixmap( QIconSet::Small, mode );
	int pixw = pixmap.width();
	int pixh = pixmap.height();
	p->drawPixmap( x+2, y+h/2-pixh/2, pixmap );
	x += pixw + 4;
	w -= pixw + 4;
    }

    drawItem( p, x, y, w, h,
	      AlignCenter|ShowPrefix,
	      btn->colorGroup(), btn->isEnabled(),
	      btn->pixmap(), btn->text(), -1,
	      on?&btn->colorGroup().brightText():&btn->colorGroup().buttonText());
}


#define HORIZONTAL	(sb->orientation() == QScrollBar::Horizontal)
#define VERTICAL	!HORIZONTAL
#define MOTIF_BORDER	2
#define SLIDER_MIN	9 // ### motif says 6 but that's too small

/*! \reimp */

void QPlatinumStyle::scrollBarMetrics( const QScrollBar* sb, int &sliderMin, int &sliderMax, int &sliderLength, int& buttonDim )
{
    int maxLength;
    int b = 0;
    int length = HORIZONTAL ? sb->width()  : sb->height();
    int extent = HORIZONTAL ? sb->height() : sb->width();

    if ( length > ( extent - b*2 - 1 )*2 + b*2 )
	buttonDim = extent - b*2;
    else
	buttonDim = ( length - b*2 )/2 - 1;

    sliderMin = b + 1; //b + buttonDim;
    maxLength  = length - b*2 - buttonDim*2 - 1;

     if ( sb->maxValue() == sb->minValue() ) {
	sliderLength = maxLength;
     } else {
	sliderLength = (sb->pageStep()*maxLength)/
			(sb->maxValue()-sb->minValue()+sb->pageStep());
	uint range = sb->maxValue()-sb->minValue();
	if ( sliderLength < SLIDER_MIN || range > INT_MAX/2 )
	    sliderLength = SLIDER_MIN;
	if ( sliderLength > maxLength )
	    sliderLength = maxLength;
     }
     /*	Old macintosh, but they changed it for 8.5
      if (maxLength >=  buttonDim)
	 sliderLength = buttonDim; // macintosh

	 */

    sliderMax = sliderMin + maxLength - sliderLength;

}

/*! \reimp */

void QPlatinumStyle::drawScrollBarBackground( QPainter *p, int x, int y, int w, int h,
					      const QColorGroup &g, bool horizontal, const QBrush* fill)
{
    QPen oldPen = p->pen();

    if (w < 3 || h < 3) {
	p->fillRect(x, y, w, h, fill?*fill:g.brush( QColorGroup::Mid ));
	p->setPen(g.shadow());
	p->drawRect(x, y, w, h);
	p->setPen(oldPen);
	return;
    }


    if (horizontal) {
	p->fillRect(x+2, y+2, w-2, h-4,fill?*fill:g.brush( QColorGroup::Mid ));

	// the dark side
	p->setPen(g.dark().dark());
	p->drawLine(x, y, x+w-1, y);
	p->setPen(g.shadow());
	p->drawLine(x, y, x, y+h-1);

	p->setPen(g.mid().dark());
	p->drawLine(x+1, y+1, x+w-1, y+1);
	p->drawLine(x+1, y+1, x+1, y+h-2);

	// the bright side!

	p->setPen(g.button());
	p->drawLine(x+1, y+h-2 ,x+w-1, y+h-2);
	//p->drawLine(x+w-2, y+1, x+w-2, y+h-2);

	p->setPen(g.shadow());
	p->drawLine(x, y+h-1,x+w-1, y+h-1);
	// p->drawLine(x+w-1, y, x+w-1, y+h-1);
    }
    else {
	p->fillRect(x+2, y+2, w-4, h-2,fill?*fill:g.brush( QColorGroup::Mid ));

	// the dark side
	p->setPen(g.dark().dark());
	p->drawLine(x, y, x+w-1, y);
	p->setPen(g.shadow());
	p->drawLine(x, y, x, y+h-1);

	p->setPen(g.mid().dark());
	p->drawLine(x+1, y+1, x+w-2, y+1);
	p->drawLine(x+1, y+1, x+1, y+h-1);


	// the bright side!

	p->setPen(g.button());
	//p->drawLine(x+1, y+h-2 ,x+w-2, y+h-2);
	p->drawLine(x+w-2, y+1, x+w-2, y+h-1);

	p->setPen(g.shadow());
	//p->drawLine(x, y+h-1,x+w-1, y+h-1);
	p->drawLine(x+w-1, y, x+w-1, y+h-1);

    }
    p->setPen(oldPen);

}


/*!\reimp
 */
QStyle::ScrollControl QPlatinumStyle::scrollBarPointOver( const QScrollBar* sb, int sliderStart, const QPoint& p )
{
	if ( !sb->rect().contains( p ) )
	return NoScroll;
    int sliderMin, sliderMax, sliderLength, buttonDim, pos;
    scrollBarMetrics( sb, sliderMin, sliderMax, sliderLength, buttonDim );

    if (sb->orientation() == QScrollBar::Horizontal)
	pos = p.x();
    else
	pos = p.y();

    if (pos < sliderStart)
	return SubPage;
    if (pos < sliderStart + sliderLength)
	return Slider;
    if (pos < sliderMax + sliderLength)
	return AddPage;
    if (pos < sliderMax + sliderLength + buttonDim)
	return SubLine;
    return AddLine;

/*
    if (pos < buttonDim)
	return SubLine;
    if (pos < 2 * buttonDim)
	return AddLine;
    if (pos < sliderStart)
	return SubPage;
    if (pos > sliderStart + sliderLength)
	return AddPage;
    return Slider;

*/
}

/*! \reimp */

void QPlatinumStyle::drawScrollBarControls( QPainter* p, const QScrollBar* sb, int sliderStart, uint controls, uint activeControl )
{
#define ADD_LINE_ACTIVE ( activeControl == AddLine )
#define SUB_LINE_ACTIVE ( activeControl == SubLine )
    QColorGroup g  = sb->colorGroup();

    int sliderMin, sliderMax, sliderLength, buttonDim;
    scrollBarMetrics( sb, sliderMin, sliderMax, sliderLength, buttonDim );

    if (sliderStart > sliderMax) { // sanity check
	sliderStart = sliderMax;
    }

    int b = 0;
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
	subX = length - dimB - dimB - b; //b;
	addX = length - dimB - b;
// 	subY = addY = ( extent - dimB ) / 2;
// 	subX = b;
// 	addX = b + dimB; //length - dimB - b;
    } else {
	subX = addX = ( extent - dimB ) / 2;
	subY = length - dimB - dimB - b; //b;
	addY = length - dimB - b;
    }

    subB.setRect( subX,subY,dimB,dimB );
    addB.setRect( addX,addY,dimB,dimB );

    int sliderEnd = sliderStart + sliderLength;
    int sliderW = extent - b*2;
    if ( HORIZONTAL ) {
	subPageR.setRect( b + 1, b,
			  sliderStart - 1 , sliderW );
	addPageR.setRect( sliderEnd, b, subX - sliderEnd, sliderW );
	sliderR .setRect( sliderStart, b, sliderLength, sliderW );
// 	subPageR.setRect( subB.right() + 1, b,
// 			  sliderStart - subB.right() - 1 , sliderW );
// 	addPageR.setRect( sliderEnd, b, addX - sliderEnd, sliderW );
// 	sliderR .setRect( sliderStart, b, sliderLength, sliderW );

    } else {
	subPageR.setRect( b, b + 1, sliderW,
			  sliderStart - b - 1 );
	addPageR.setRect( b, sliderEnd, sliderW, subY - sliderEnd );
	sliderR .setRect( b, sliderStart, sliderW, sliderLength );
    }

    bool maxedOut = (sb->maxValue() == sb->minValue());
    if ( controls & AddLine ) {
	drawBevelButton( p, addB.x(), addB.y(),
			 addB.width(), addB.height(), g,
			 ADD_LINE_ACTIVE);
	p->setPen(g.shadow());
	p->drawRect( addB );
	drawArrow( p, VERTICAL ? DownArrow : RightArrow,
		   FALSE, addB.x()+2, addB.y()+2,
		   addB.width()-4, addB.height()-4, g, !maxedOut,
		   ADD_LINE_ACTIVE ? &g.brush( QColorGroup::Mid )    :
				     &g.brush( QColorGroup::Button ));
    }
    if ( controls & SubLine ) {
	drawBevelButton( p, subB.x(), subB.y(),
			 subB.width(), subB.height(), g,
			 SUB_LINE_ACTIVE );
	p->setPen(g.shadow());
	p->drawRect( subB );
	drawArrow( p, VERTICAL ? UpArrow : LeftArrow,
		    FALSE, subB.x()+2, subB.y()+2,
		   subB.width()-4, subB.height()-4, g, !maxedOut,
		   SUB_LINE_ACTIVE ? &g.brush( QColorGroup::Mid )    :
				     &g.brush( QColorGroup::Button ));
    }


    if ( controls & SubPage )
	drawScrollBarBackground( p, subPageR.x(), subPageR.y(), subPageR.width(),
				 subPageR.height(),
				 g, HORIZONTAL );
    if ( controls & AddPage )
	drawScrollBarBackground( p, addPageR.x(), addPageR.y(), addPageR.width(),
				 addPageR.height(),
				 g, HORIZONTAL );
    if ( controls & Slider ) {
	QPoint bo = p->brushOrigin();
	p->setBrushOrigin(sliderR.topLeft());
	drawBevelButton( p, sliderR.x(), sliderR.y(),
			 sliderR.width(), sliderR.height(), g,
			 FALSE, &g.brush( QColorGroup::Button ) );
	p->setBrushOrigin(bo);
	drawRiffles(p, sliderR.x(), sliderR.y(),
		    sliderR.width(), sliderR.height(), g, HORIZONTAL);
	p->setPen(g.shadow());
	p->drawRect( sliderR );
    }

    // ### perhaps this should not be able to accept focus if maxedOut?
    if ( sb->hasFocus() && (controls & Slider) )
	p->drawWinFocusRect( sliderR.x()+2, sliderR.y()+2,
			     sliderR.width()-5, sliderR.height()-5,
			     sb->backgroundColor() );

}

/*!
  draw the nifty Macintosh decoration used on  sliders
  */
void QPlatinumStyle::drawRiffles( QPainter* p,  int x, int y, int w, int h,
		      const QColorGroup &g, bool horizontal )
{
	if (!horizontal) {
	    if (h > 20) {
		y += (h-20)/2 ;
		h = 20;
	    }
	    if (h > 8) {
		int n = h / 4;
		int my = y+h/2-n;
		int i ;
		p->setPen(g.light());
		for (i=0; i<n; i++) {
		    p->drawLine(x+3, my+2*i, x+w-5, my+2*i);
		}
		p->setPen(g.dark());
		my++;
		for (i=0; i<n; i++) {
		    p->drawLine(x+4, my+2*i, x+w-4, my+2*i);
		}
	    }
	}
	else {
	    if (w > 20) {
		x += (w-20)/2 ;
		w = 20;
	    }
	    if (w > 8) {
		int n = w / 4;
		int mx = x+w/2-n;
		int i ;
		p->setPen(g.light());
		for (i=0; i<n; i++) {
		    p->drawLine(mx+2*i, y+3, mx + 2*i, y+h-5);
		}
		p->setPen(g.dark());
		mx++;
		for (i=0; i<n; i++) {
		    p->drawLine(mx+2*i, y+4, mx + 2*i, y+h-4);
		}
	    }
	}
}


/*! \reimp */

void QPlatinumStyle::drawIndicator( QPainter* p,
				    int x, int y, int w, int h, const QColorGroup &g,
				    int s, bool down, bool /*enabled*/ )
{
    QBrush fill;
    if ( down )
	fill = g.brush( QColorGroup::Dark );
    else
	fill = g.brush( QColorGroup::Button );

    drawBevelButton( p, x, y, w-2, h, g,
		     down, &fill );
    p->fillRect(x+w-2, y, 2, h, g.brush( QColorGroup::Background ) );
    p->setPen( g.shadow() );
    p->drawRect( x, y, w-2, h );

    static const QCOORD nochange_mark[] = { 3,5, 9,5,  3,6, 9,6 };
    static const QCOORD check_mark[] = {
	3,5, 5,5,  4,6, 5,6,  5,7, 6,7,  5,8, 6,8,	6,9, 9,9,
	6,10, 8,10,	 7,11, 8,11,  7,12, 7,12,  8,8, 9,8,  8,7, 10,7,
	9,6, 10,6,	9,5, 11,5,  10,4, 11,4,	 10,3, 12,3,
	11,2, 12,2,	 11,1, 13,1,  12,0, 13,0 };
    if (s != QButton::Off) {
	QPen oldPen = p->pen();
// 	p->setPen (QPen(g.text(), 2));
// 	p->drawLine( x+2, y+h/2-1, x+w/2-1, y+h-4);
// 	p->drawLine(x+w/2-1, y+h-4, x+w, 0);
// 	p->setPen( oldPen );

	int x1 = x;
	int y1 = y;
	if ( down ) {			// shift check mark
	    x1++;
	    y1++;
	}
	QPointArray amark;
	if ( s == QButton::On )
	    amark = QPointArray( sizeof(check_mark)/(sizeof(QCOORD)*2),
			       check_mark );
	else
	    amark = QPointArray( sizeof(nochange_mark)/(sizeof(QCOORD)*2),
			       nochange_mark );
	amark.translate( x1+1, y1+1 );
	p->setPen( g.dark() );
	p->drawLineSegments( amark );
	amark.translate( -1, -1 );
	p->setPen( g.foreground() );
	p->drawLineSegments( amark );
/*
	if ( s == QButton::On ) {
	    p->setPen( g.dark() );
	    for ( int i=0; i<(int)(sizeof(check_mark_pix)/sizeof(QCOORD));
		  i+=2 )
		p->drawPoint( x1 + check_mark_pix[i],
			      y1 + check_mark_pix[i+1] );
	}
*/
	p->setPen( oldPen );
    }


}

/*! \reimp */

void
QPlatinumStyle::drawIndicatorMask( QPainter *p, int x, int y, int w, int h, int s)
{
    p->fillRect(x, y, w-2, h, color1);
    if (s != QButton::Off) {
	QPen oldPen = p->pen();
	p->setPen (QPen(color1, 2));
	p->drawLine( x+2, y+h/2-1, x+w/2-1, y+h-4);
	p->drawLine(x+w/2-1, y+h-4, x+w, 0);
	p->setPen( oldPen );
    }
}

/*! \reimp */

QSize
QPlatinumStyle::indicatorSize() const
{
    return QSize(15,13);
}



#define QCOORDARRLEN(x) sizeof(x)/(sizeof(QCOORD)*2)
/*! \reimp */

void QPlatinumStyle::drawExclusiveIndicator( QPainter* p,
				   int x, int y, int w, int h, const QColorGroup &g,
				   bool on, bool down, bool /* enabled */ )
{
    static const QCOORD pts1[] = {		// normal circle
	5,0, 8,0, 9,1, 10,1, 11,2, 12,3, 12,4, 13,5,
	13,8, 12,9, 12,10, 11,11, 10,12, 9,12, 8,13,
	5,13, 4,12, 3,12, 2,11, 1,10, 1,9, 0,8, 0,5,
	1,4, 1,3, 2,2, 3,1, 4,1 };
    static const QCOORD pts2[] = {		// top left shadow
	5,1, 8,1,	3,2, 7,2,  2,3, 5,3,  2,4, 4,4,
	1,5, 3,5,  1,6, 1,8,  2,6, 2,7 };
    static const QCOORD pts3[] = {		// bottom right, dark
	5,12, 8,12,  7,11, 10,11,	8,10, 11,10,
	9,9, 11,9,  10,8, 12,8,  11,7, 11,7,
	12,5, 12,7 };
    static const QCOORD pts4[] = {		// bottom right, light
	5,12, 8,12,  7,11, 10,11,	9,10, 11,10,
	10,9, 11,9,  11,7, 11,8,	 12,5, 12,8 };
    static const QCOORD pts5[] = {		// check mark
	6,4, 8,4, 10,6, 10,8, 8,10, 6,10, 4,8, 4,6 };
    static const QCOORD pts6[] = {		// check mark extras
	4,5, 5,4,  9,4, 10,5,  10,9, 9,10,	5,10, 4,9 };
    p->eraseRect(x,y,w,h);
    p->setBrush((down||on) ? g.brush( QColorGroup::Dark )   :
			     g.brush( QColorGroup::Button ));
    p->setPen(NoPen);
    p->drawEllipse( x, y, 13, 13);
    p->setPen( g.shadow() );
    QPointArray a( QCOORDARRLEN(pts1), pts1 );
    a.translate( x, y );
    p->drawPolyline( a );			// draw normal circle
    QColor tc, bc;
    const QCOORD *bp;
    int	bl;
    if ( down || on) {			// pressed down or on
	tc = g.dark().dark();
	bc = g.light();
	bp = pts4;
	bl = QCOORDARRLEN(pts4);
    }
    else {					// released
	tc = g.light();
	bc = g.dark();
	bp = pts3;
	bl = QCOORDARRLEN(pts3);
    }
    p->setPen( tc );
    a.setPoints( QCOORDARRLEN(pts2), pts2 );
    a.translate( x, y );
    p->drawLineSegments( a );		// draw top shadow
    p->setPen( bc );
    a.setPoints( bl, bp );
    a.translate( x, y );
    p->drawLineSegments( a );
    if ( on ) {				// draw check mark
	int x1=x, y1=y;
	p->setBrush( g.foreground() );
	p->setPen( g.foreground() );
	a.setPoints( QCOORDARRLEN(pts5), pts5 );
	a.translate( x1, y1 );
	p->drawPolygon( a );
	p->setBrush( NoBrush );
	p->setPen( g.dark() );
	a.setPoints( QCOORDARRLEN(pts6), pts6 );
	a.translate( x1, y1 );
	p->drawLineSegments( a );
    }
}

/*! \reimp */

QSize
QPlatinumStyle::exclusiveIndicatorSize() const
{
    return QSize(15,15);
}

/*! \reimp */

void QPlatinumStyle::drawComboButton( QPainter *p, int x, int y, int w, int h,
				      const QColorGroup &g, bool /* sunken */,
				      bool editable,
				      bool /* enabled */,
				      const QBrush *fill )
{
    QPen oldPen = p->pen();


    p->fillRect(x+2, y+2, w-4, h-4,fill?*fill:g.brush( QColorGroup::Button ));
    // the bright side
    p->setPen(g.shadow());
    p->drawLine(x, y, x+w-1, y);
    p->drawLine(x, y, x, y+h-1);

    p->setPen(g.light());
    p->drawLine(x+1, y+1, x+w-2, y+1);
    p->drawLine(x+1, y+1, x+1, y+h-2);


	 // the dark side!


    p->setPen(g.mid());
    p->drawLine(x+2, y+h-2 ,x+w-2, y+h-2);
    p->drawLine(x+w-2, y+2, x+w-2, y+h-2);

    p->setPen(g.shadow());
    p->drawLine(x+1, y+h-1,x+w-1, y+h-1);
    p->drawLine(x+w-1, y, x+w-1, y+h-1);


    // top left corner:
    p->setPen(g.background());
    p->drawPoint(x, y);
    p->drawPoint(x+1, y);
    p->drawPoint(x, y+1);
    p->setPen(g.shadow());
    p->drawPoint(x+1, y+1);
    p->setPen(white);
    p->drawPoint(x+3, y+3);
    // bottom left corner:
    p->setPen(g.background());
    p->drawPoint(x, y+h-1);
    p->drawPoint(x+1, y+h-1);
    p->drawPoint(x, y+h-2);
    p->setPen(g.shadow());
    p->drawPoint(x+1, y+h-2);
    // top right corner:
    p->setPen(g.background());
    p->drawPoint(x+w-1, y);
    p->drawPoint(x+w-2, y);
    p->drawPoint(x+w-1, y+1);
    p->setPen(g.shadow());
    p->drawPoint(x+w-2, y+1);
    // bottom right corner:
    p->setPen(g.background());
    p->drawPoint(x+w-1, y+h-1);
    p->drawPoint(x+w-2, y+h-1);
    p->drawPoint(x+w-1, y+h-2);
    p->setPen(g.shadow());
    p->drawPoint(x+w-2, y+h-2);
    p->setPen(g.dark());
    p->drawPoint(x+w-3, y+h-3);
//     p->setPen(g.mid());
//     p->drawPoint(x+w-4, y+h-4);


//     drawButton(p, w-2-16,2,16,h-4, g, sunken );


    // now the arrow button

    {
	int xx = x+w-20;
	int yy = y;
	int ww = 20;
	int hh = h;
	// the bright side

	 p->setPen(g.mid());
	 p->drawLine(xx, yy+2, xx, yy+hh-3);

	p->setPen(g.button());
	p->drawLine(xx+1, yy+1, xx+ww-2, yy+1);
	p->drawLine(xx+1, yy+1, xx+1, yy+hh-2);

	p->setPen(g.light());
	p->drawLine(xx+2, yy+2, xx+2, yy+hh-2);
	p->drawLine(xx+2, yy+2, xx+ww-2, yy+2);


	// the dark side!

	p->setPen(g.mid());
	p->drawLine(xx+3, yy+hh-3 ,xx+ww-3, yy+hh-3);
	p->drawLine(xx+ww-3, yy+3, xx+ww-3, yy+hh-3);

	p->setPen(g.dark());
	p->drawLine(xx+2, yy+hh-2 ,xx+ww-2, yy+hh-2);
	p->drawLine(xx+ww-2, yy+2, xx+ww-2, yy+hh-2);

	p->setPen(g.shadow());
	p->drawLine(xx+1, yy+hh-1,xx+ww-1, yy+hh-1);
	p->drawLine(xx+ww-1, yy, xx+ww-1, yy+hh-1);

	// top right corner:
	p->setPen(g.background());
	p->drawPoint(xx+ww-1, yy);
	p->drawPoint(xx+ww-2, yy);
	p->drawPoint(xx+ww-1, yy+1);
	p->setPen(g.shadow());
	p->drawPoint(xx+ww-2, yy+1);
	// bottom right corner:
	p->setPen(g.background());
	p->drawPoint(xx+ww-1, yy+hh-1);
	p->drawPoint(xx+ww-2, yy+hh-1);
	p->drawPoint(xx+ww-1, yy+hh-2);
	p->setPen(g.shadow());
	p->drawPoint(xx+ww-2, yy+hh-2);
	p->setPen(g.dark());
	p->drawPoint(xx+ww-3, yy+hh-3);
	p->setPen(g.mid());
	p->drawPoint(xx+ww-4, yy+hh-4);

	// and the arrows
	p->setPen( g.foreground() );
	QPointArray a;
	a.setPoints( 7, -3,1, 3,1, -2,0, 2,0, -1,-1, 1,-1, 0,-2 );
	a.translate( xx+ww/2, yy+hh/2-3 );
	p->drawLineSegments( a, 0, 3 );		// draw arrow
	p->drawPoint( a[6] );
	a.setPoints( 7, -3,-1, 3,-1, -2,0, 2,0, -1,1, 1,1, 0,2 );
	a.translate( xx+ww/2, yy+hh/2+2 );
	p->drawLineSegments( a, 0, 3 );		// draw arrow
	p->drawPoint( a[6] );

    }


    if (editable) {
	QRect r = comboButtonRect(x, y, w, h);
	r.setRect( r.left()-1, r.top()-1, r.width()+2, r.height()+2 );
	qDrawShadePanel( p, r, g, TRUE, 2, 0 );
    }
    p->setPen( oldPen );

}


/*! \reimp */

QRect QPlatinumStyle::comboButtonRect( int x, int y, int w, int h){
    return QRect(x+3, y+3, w-6-16, h-6);
}

/*! \reimp */

QRect QPlatinumStyle::comboButtonFocusRect( int x, int y, int w, int h)
{
    return QRect(x+4, y+4, w-8-16, h-8);
}


/*! \reimp */
int QPlatinumStyle::sliderLength() const
{
    return 17;
}

/*! \reimp */
void QPlatinumStyle::drawSlider( QPainter *p,
				 int x, int y, int w, int h,
				 const QColorGroup &g,
				 Orientation orient , bool /* tickAbove */, bool /*tickBelow*/ )
{
    const QColor c0 = g.shadow();
    const QColor c1 = g.dark();
    //    const QColor c2 = g.button();
    const QColor c3 = g.light();

    int x1 = x;
    int x2 = x+w-1;
    int y1 = y;
    int y2 = y+h-1;
    int mx = w/2;
    int my = h/2;


    if ( orient == Vertical ) {

	// Background
	QBrush oldBrush = p->brush();
	p->setBrush( g.brush( QColorGroup::Button ) );
	p->setPen( NoPen );
	QPointArray a(6);
	a.setPoint( 0, x1+1, y1+1 );
	a.setPoint( 1, x2-my+2, y1+1 );
	a.setPoint( 2, x2-1, y1+my-1 );
	a.setPoint( 3, x2-1, y2-my+1 );
	a.setPoint( 4, x2-my+2, y2-1 );
	a.setPoint( 5, x1+1, y2-1 );
	p->drawPolygon( a );
	p->setBrush( oldBrush );

	// shadow border
	p->setPen( c0 );
	p->drawLine(x1, y1+1, x1,y2-1);
	p->drawLine( x2-my+2, y1, x2, y1+my-2);
	p->drawLine( x2-my+2, y2, x2, y1+my+2);
	p->drawLine(x2, y1+my-2, x2, y1+my+2);
	p->drawLine(x1+1, y1, x2-my+2, y1);
	p->drawLine(x1+1, y2, x2-my+2, y2);

	// light shadow
	p->setPen(c3);
	p->drawLine(x1+1, y1+2, x1+1,y2-2);
	p->drawLine(x1+1, y1+1, x2-my+2, y1+1);
	p->drawLine( x2-my+2, y1+1, x2-1, y1+my-2);

	// dark shadow
	p->setPen(c1);
	p->drawLine(x2-1, y1+my-2, x2-1, y1+my+2);
	p->drawLine( x2-my+2, y2-1, x2-1, y1+my+2);
	p->drawLine(x1+1, y2-1, x2-my+2, y2-1);


	drawRiffles(p, x, y+2, w-3, h-4, g, TRUE);
    }
    else { // Horizontal

	// Background
	QBrush oldBrush = p->brush();
	p->setBrush( g.brush( QColorGroup::Button ) );
	p->setPen( NoPen );
	QPointArray a(6);
	a.setPoint( 0, x2-1, y1+1 );
	a.setPoint( 1, x2-1, y2-mx+2 );
	a.setPoint( 2, x2-mx+1, y2-1 );
	a.setPoint( 3, x1+mx-1, y2-1 );
	a.setPoint( 4, x1+1, y2-mx+2 );
	a.setPoint( 5, x1+1, y1+1 );
	p->drawPolygon( a );
	p->setBrush( oldBrush );

	// shadow border
	p->setPen( c0 );
	p->drawLine(x1+1,y1,x2-1,y1);
	p->drawLine(x1, y2-mx+2, x1+mx-2, y2);
	p->drawLine(x2, y2-mx+2, x1+mx+2, y2);
	p->drawLine(x1+mx-2, y2, x1+mx+2, y2);
	p->drawLine(x1, y1+1, x1, y2-mx+2);
	p->drawLine(x2, y1+1, x2, y2-mx+2);

	// light shadow
	p->setPen(c3);
	p->drawLine(x1+1, y1+1,x2-1, y1+1);
	p->drawLine(x1+1, y1+1, x1+1, y2-mx+2);

	// dark shadow
	p->setPen(c1);
	p->drawLine(x2-1, y1+1, x2-1, y2-mx+2);
	p->drawLine(x1+1, y2-mx+2, x1+mx-2, y2-1);
	p->drawLine(x2-1, y2-mx+2, x1+mx+2, y2-1);
	p->drawLine(x1+mx-2, y2-1, x1+mx+2, y2-1);

	drawRiffles(p, x+2, y, w-4, h-5, g, FALSE);
    }
}

/*! \reimp */
void QPlatinumStyle::drawSliderMask( QPainter *p,
				 int x, int y, int w, int h,
				 Orientation orient , bool /* tickAbove */, bool /*tickBelow*/ )
{
    int x1 = x;
    int x2 = x+w-1;
    int y1 = y;
    int y2 = y+h-1;
    int mx = w/2;
    int my = h/2;


    if ( orient == Vertical ) {

	// Background
	p->setBrush( color1 );
	p->setPen( NoPen );
	QPointArray a(6);
	a.setPoint( 0, x1+1, y1+1 );
	a.setPoint( 1, x2-my+2, y1+1 );
	a.setPoint( 2, x2-1, y1+my-1 );
	a.setPoint( 3, x2-1, y2-my+1 );
	a.setPoint( 4, x2-my+2, y2-1 );
	a.setPoint( 5, x1+1, y2-1 );
	p->drawPolygon( a );
	p->setPen( color1 );

	// shadow border
	p->drawLine(x1, y1+1, x1,y2-1);
	p->drawLine( x2-my+2, y1, x2, y1+my-2);
	p->drawLine( x2-my+2, y2, x2, y1+my+2);
	p->drawLine(x2, y1+my-2, x2, y1+my+2);
	p->drawLine(x1+1, y1, x2-my+2, y1);
	p->drawLine(x1+1, y2, x2-my+2, y2);

	// light shadow
	p->drawLine(x1+1, y1+2, x1+1,y2-2);
	p->drawLine(x1+1, y1+1, x2-my+2, y1+1);
	p->drawLine( x2-my+2, y1+1, x2-1, y1+my-2);

	// dark shadow
	p->drawLine(x2-1, y1+my-2, x2-1, y1+my+2);
	p->drawLine( x2-my+2, y2-1, x2-1, y1+my+2);
	p->drawLine(x1+1, y2-1, x2-my+2, y2-1);
    }
    else { // Horizontal

	// Background
	p->setBrush( color1 );
	p->setPen( NoPen );
	QPointArray a(6);
	a.setPoint( 0, x2-1, y1+1 );
	a.setPoint( 1, x2-1, y2-mx+2 );
	a.setPoint( 2, x2-mx+1, y2-1 );
	a.setPoint( 3, x1+mx-1, y2-1 );
	a.setPoint( 4, x1+1, y2-mx+2 );
	a.setPoint( 5, x1+1, y1+1 );
	p->drawPolygon( a );
	p->setPen( color1 );

	// shadow border
	p->drawLine(x1+1,y1,x2-1,y1);
	p->drawLine(x1, y2-mx+2, x1+mx-2, y2);
	p->drawLine(x2, y2-mx+2, x1+mx+2, y2);
	p->drawLine(x1+mx-2, y2, x1+mx+2, y2);
	p->drawLine(x1, y1+1, x1, y2-mx+2);
	p->drawLine(x2, y1+1, x2, y2-mx+2);

	// light shadow
	p->drawLine(x1+1, y1+1,x2-1, y1+1);
	p->drawLine(x1+1, y1+1, x1+1, y2-mx+2);

	// dark shadow
	p->drawLine(x2-1, y1+1, x2-1, y2-mx+2);
	p->drawLine(x1+1, y2-mx+2, x1+mx-2, y2-1);
	p->drawLine(x2-1, y2-mx+2, x1+mx+2, y2-1);
	p->drawLine(x1+mx-2, y2-1, x1+mx+2, y2-1);
    }
}


/*! \reimp */
void QPlatinumStyle::drawSliderGroove( QPainter *p,
				      int x, int y, int w, int h,
				      const QColorGroup& g, QCOORD c,
				       Orientation orient )
{

    p->fillRect(x, y, w, h, g.brush( QColorGroup::Background ));
    if ( orient == Horizontal ) {
	y = y+c-3;
	h = 7;
    }
    else {
	x = x+c-3;
	w = 7;
    }
    p->fillRect(x, y, w, h, g.brush( QColorGroup::Dark ));

	 // the dark side
    p->setPen(g.dark());
    p->drawLine(x, y, x+w-1, y);
    p->drawLine(x, y, x, y+h-1);

    p->setPen(g.shadow());
    p->drawLine(x+1, y+1, x+w-2, y+1);
    p->drawLine(x+1, y+1, x+1, y+h-2);


	 // the bright side!

    p->setPen(g.shadow());
    p->drawLine(x+1, y+h-2 ,x+w-2, y+h-2);
    p->drawLine(x+w-2, y+1, x+w-2, y+h-2);

    p->setPen(g.light());
    p->drawLine(x, y+h-1,x+w-1, y+h-1);
    p->drawLine(x+w-1, y, x+w-1, y+h-1);

    // top left corner:
    p->setPen(g.background());
    p->drawPoint(x, y);
    p->drawPoint(x+1, y);
    p->drawPoint(x, y+1);
    p->setPen(g.shadow());
    p->drawPoint(x+1, y+1);
    // bottom left corner:
    p->setPen(g.background());
    p->drawPoint(x, y+h-1);
    p->drawPoint(x+1, y+h-1);
    p->drawPoint(x, y+h-2);
    p->setPen(g.light());
    p->drawPoint(x+1, y+h-2);
    // top right corner:
    p->setPen(g.background());
    p->drawPoint(x+w-1, y);
    p->drawPoint(x+w-2, y);
    p->drawPoint(x+w-1, y+1);
    p->setPen(g.dark());
    p->drawPoint(x+w-2, y+1);
    // bottom right corner:
    p->setPen(g.background());
    p->drawPoint(x+w-1, y+h-1);
    p->drawPoint(x+w-2, y+h-1);
    p->drawPoint(x+w-1, y+h-2);
    p->setPen(g.light());
    p->drawPoint(x+w-2, y+h-2);
    p->setPen(g.dark());
    p->drawPoint(x+w-3, y+h-3);

}

/*!
  Reimplementation from QWindowsStyle to disable the Windows typical jump
  back when dragging controls.

  \sa QStyle
  */
int QPlatinumStyle::maximumSliderDragDistance() const
{
    return -1;
}


/*! \reimp
*/
void QPlatinumStyle::polishPopupMenu( QPopupMenu* p)
{
    QWindowsStyle::polishPopupMenu( p );
}



/*! \reimp
*/
void QPlatinumStyle::drawCheckMark( QPainter *p, int x, int y, int w, int h,
				   const QColorGroup &g,
				   bool act, bool dis )
{
    QWindowsStyle::drawCheckMark( p, x, y, w, h, g, act, dis );
}


/*! \reimp
*/
int QPlatinumStyle::extraPopupMenuItemWidth( bool checkable, int maxpmw, QMenuItem* mi, const QFontMetrics& fm )
{
    return QWindowsStyle::extraPopupMenuItemWidth( checkable, maxpmw, mi, fm );
}

/*! \reimp
*/
int QPlatinumStyle::popupMenuItemHeight( bool checkable, QMenuItem* mi, const QFontMetrics& fm )
{
    return QWindowsStyle::popupMenuItemHeight( checkable, mi, fm );
}

/*! \reimp
*/
void QPlatinumStyle::drawPopupMenuItem( QPainter* p, bool checkable, int maxpmw, int tab, QMenuItem* mi,
					const QPalette& pal,
					bool act, bool enabled, int x, int y, int w, int h)
{
    QWindowsStyle::drawPopupMenuItem( p, checkable, maxpmw, tab, mi, pal, act, enabled, x, y, w, h);
}

/*!\reimp
 */
void QPlatinumStyle::getButtonShift( int &x, int &y)
{
    x = 0;
    y = 0;
}
#endif
