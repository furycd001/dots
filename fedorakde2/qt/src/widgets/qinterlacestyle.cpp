/****************************************************************************
** $Id: qt/src/widgets/qinterlacestyle.cpp   2.3.2   edited 2001-02-02 $
**
** Implementation of QQInterlaceStyle widget class
**
** Created : 22 January 2001
**
** Copyright (C) 1992-2001 Trolltech AS.  All rights reserved.
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

#include "qinterlacestyle.h"

#ifndef QT_NO_STYLE_INTERLACE

#include <qapplication.h>
#include <qpainter.h>
#include <qdrawutil.h> // for now
#include <qpalette.h> // for now
#include <qwidget.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <qrangecontrol.h>
#include <qscrollbar.h>
#include <qlistbox.h>
#include <limits.h>

/*!
  \class QInterlaceStyle qinterlacestyle.h
  \brief Look and Feel suitable for interlaced displays
  \ingroup appearance

  This class implements a look and feel that reduces flicker as much as
  possible on interlaced displays (i.e. television).  It is an experimental
  style.  In addition to using this style you will need to select a font
  that does not flicker.
*/

/*!
  Constructs a QInterlaceStyle
*/
QInterlaceStyle::QInterlaceStyle() : QMotifStyle()
{
    setButtonDefaultIndicatorWidth( 0 );
    setUseHighlightColors( TRUE );
    setSliderThickness(18);
    setScrollBarExtent(18);
}

int QInterlaceStyle::defaultFrameWidth() const
{
    return 2;
}

/*!
  \reimp
 */
void QInterlaceStyle::polish( QApplication *app)
{
    oldPalette = app->palette();
#if 0
    QColor bg( 128, 64, 128 );
    QColor btn( 255, 145, 0 );
    QColor mid = bg.dark( 120 );
    QColor low = mid.dark( 120 );
    QColor fg( white );
#else
    QColor bg( 224, 224, 224 );
    QColor btn = bg.dark( 105 );
    QColor mid = bg.dark( 120 );
    QColor low = mid.dark( 120 );
    QColor fg( black );
#endif

    QColorGroup cg( fg, btn, low, low, mid, black, black, white, bg );
    cg.setColor( QColorGroup::Highlight, QColor( 255, 255, 192 ) );
    cg.setColor( QColorGroup::HighlightedText, black );

    QColorGroup dcg( cg );
    dcg.setColor( QColorGroup::ButtonText, low );
    dcg.setColor( QColorGroup::Text, low );

    app->setPalette( QPalette( cg, dcg, cg ), TRUE );
}

/*!
  \reimp
 */
void QInterlaceStyle::unPolish( QApplication *app)
{
    app->setPalette(oldPalette, TRUE);
}

/*!
  \reimp
 */
void QInterlaceStyle::polish( QWidget* w)
{

   // the polish function sets some widgets to transparent mode and
    // some to translate background mode in order to get the full
    // benefit from the nice pixmaps in the color group.

    if ( w->inherits("QLCDNumber") ){
	return;
    }

    if ( !w->isTopLevel() ) {
	if ( w->inherits("QGroupBox")
	     || w->inherits("QTabWidget") 
	     || w->inherits("QPushButton") ) {
	    w->setAutoMask( TRUE );
	    return;
	}
 	if (w->inherits("QLabel")
 	    || w->inherits("QSlider")
 	    || w->inherits("QButton")
	    || w->inherits("QProgressBar")
	    ){
	    w->setBackgroundOrigin( QWidget::ParentOrigin );
 	}
    }

    if ( w->inherits( "QFrame" ) ) {
	QFrame *f = (QFrame *)w;
	switch ( f->frameShape() ) {
	    case QFrame::WinPanel:
		f->setFrameShape( QFrame::StyledPanel );

	    case QFrame::Panel:
	    case QFrame::Box:
	    case QFrame::StyledPanel:
	    case QFrame::PopupPanel:
		if ( f->frameWidth() == 1 )
		    f->setLineWidth( 2 );
		break;
	    default:
		break;
	}
    }

    if ( w->inherits( "QListBox" ) ) {
	// the list box in combos has an ugly border otherwise
	QFrame *f = (QFrame *)w;
	if ( f->frameShadow() == QFrame::Plain ) {
	    f->setFrameShadow( QFrame::Raised );
	    f->setLineWidth( 1 );
	}
    }
}

/*!
  \reimp
*/
void QInterlaceStyle::unPolish( QWidget* w)
{

   // the polish function sets some widgets to transparent mode and
    // some to translate background mode in order to get the full
    // benefit from the nice pixmaps in the color group.

    if ( w->inherits("QLCDNumber") ){
	return;
    }

    if ( !w->isTopLevel() ) {
	if ( w->inherits("QGroupBox")
	     || w->inherits("QTabWidget") 
	     || w->inherits("QPushButton" ) ) {
	    w->setAutoMask( FALSE );
	    return;
	}
 	if (w->inherits("QLabel")
 	    || w->inherits("QSlider")
 	    || w->inherits("QButton")
	    || w->inherits("QProgressBar")
	    ){
	    w->setBackgroundOrigin( QWidget::WidgetOrigin );
 	}
    }

}

/*!
  \reimp
*/
QRect QInterlaceStyle::pushButtonContentsRect( QPushButton *btn )
{
    int fw = 0;
    if ( btn->isDefault() || btn->autoDefault() )
	fw = buttonDefaultIndicatorWidth();
	     
    return buttonRect( fw+5, fw, btn->width()-2*fw-10, btn->height()-2*fw );
}

/*!
  \reimp
*/
void QInterlaceStyle::drawFocusRect ( QPainter *p, const QRect &/*r*/, const QColorGroup &g, const QColor * bg, bool /*atBorder*/ )
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
/*
    p->setBrush( NoBrush );
    if ( atBorder ) {
	p->drawRect( QRect( r.x()+1, r.y()+2, r.width()-2, r.height()-4 ) );
	p->drawRect( QRect( r.x()+2, r.y()+1, r.width()-4, r.height()-2 ) );
    } else {
	p->drawRect( QRect( r.x(), r.y()+1, r.width(), r.height()-2 ) );
	p->drawRect( QRect( r.x()+1, r.y(), r.width()-2, r.height() ) );
    }
*/
}

/*!
  \reimp
*/
void QInterlaceStyle::drawButton( QPainter *p, int x, int y, int w, int h,
			     const QColorGroup &g, bool /*sunken*/, const QBrush *fill)
{
    const int lineWidth = 2;

    p->setBrush( g.brush( QColorGroup::Dark ) );
    p->setPen( NoPen );
    p->drawRect( x+1, y+1, 2, 2 );
    p->drawRect( x+w-3, y+1, 2, 2 );
    p->drawRect( x+1, y+h-3, 2, 2 );
    p->drawRect( x+w-3, y+h-3, 2, 2 );

    p->drawRect( x+2, y, w-4, 2 );
    p->drawRect( x+2, y+h-lineWidth, w-4, lineWidth );
    p->drawRect( x, y+2, lineWidth, h-4 );
    p->drawRect( x+w-lineWidth, y+2, lineWidth, h-4 );

    if ( fill ) {
	x += 2;
	y += 2;
	w -= 4;
	h -= 4;
	p->setBrush( *fill );
	p->setPen( NoPen );
	p->drawRect( x+1, y, w-2, 1 );
	p->drawRect( x, y+1, w, h-2 );
	p->drawRect( x+1, y+h-1, w-2, 1 );
    }
}

void QInterlaceStyle::drawButtonMask ( QPainter * p, int x, int y, int w, int h )
{
    QBrush fill( color1 );
    QColorGroup cg;
    cg.setBrush( QColorGroup::Dark, color1 );
    drawButton( p, x, y, w, h, cg, FALSE, &fill );
}

/*!
  \reimp
*/
void QInterlaceStyle::drawBevelButton( QPainter *p, int x, int y, int w, int h,
				const QColorGroup &g, bool sunken, const QBrush* fill)
{
    QInterlaceStyle::drawButton(p, x, y, w, h, g, sunken, fill);
}

/*!
  \reimp
*/
void QInterlaceStyle::drawPushButton( QPushButton* btn, QPainter *p)
{
    QColorGroup g = btn->colorGroup();
    int x1, y1, x2, y2;

    btn->rect().coords( &x1, &y1, &x2, &y2 );	// get coordinates

    QBrush fill( g.button() );
    if ( btn->isDown() || btn->isOn() )
	fill = g.mid();

    if ( btn->hasFocus() )
	g.setBrush( QColorGroup::Dark, black );
    drawButton( p, x1, y1, x2-x1+1, y2-y1+1, g, FALSE, &fill );
	
    if ( btn->isMenuButton() ) {
	int dx = (y1-y2-4)/3;
	drawArrow( p, DownArrow, FALSE,
		   x2 - dx, dx, y1, y2 - y1,
		   g, btn->isEnabled() );
    }

    if ( p->brush().style() != NoBrush )
	p->setBrush( NoBrush );
}

/*!
  \reimp
*/
QSize QInterlaceStyle::indicatorSize () const
{
    return QSize(13,13);
}

/*!
  \reimp
*/
void QInterlaceStyle::drawIndicator( QPainter * p, int x, int y, int w, int h, const QColorGroup &g, int s, bool down, bool enabled )
{
    p->fillRect( x, y, w, h, g.brush( QColorGroup::Background ) );
    QBrush fill;
    if ( s == QButton::NoChange ) {
	QBrush b = p->brush();
	QColor c = p->backgroundColor();
	p->setBackgroundMode( TransparentMode );
	p->setBackgroundColor( green );
	fill = QBrush(g.base(), Dense4Pattern);
	p->setBackgroundColor( c );
	p->setBrush( b );
    } else if ( down )
	fill = g.brush( QColorGroup::Button );
    else
	fill = g.brush( enabled ? QColorGroup::Base : QColorGroup::Background );

    drawButton( p, x, y, w, h, g, FALSE, &fill );

    if ( s != QButton::Off ) {
	QPointArray a( 7*2 );
	int i, xx, yy;
	xx = x+3;
	yy = y+5;
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
	if ( s == QButton::NoChange ) {
	    p->setPen( g.dark() );
	} else {
	    p->setPen( g.text() );
	}
	p->drawLineSegments( a );
    }
}

/*!
  \reimp
*/
void QInterlaceStyle::drawIndicatorMask( QPainter *p, int x, int y, int w, int h, int )
{
    drawButtonMask( p, x, y, w, h );
}

/*!
  \reimp
*/
QSize QInterlaceStyle::exclusiveIndicatorSize() const
{
    return QSize(13,13);
}

/*!
  \reimp
*/
void QInterlaceStyle::drawExclusiveIndicator( QPainter *p, int x, int y, int w, int h, const QColorGroup &g, bool on, bool down, bool enabled )
{
    p->fillRect( x, y, w, h, g.brush( QColorGroup::Background ) );
    p->setBrush( g.dark() );
    p->setPen( QPen( NoPen ) );
    p->drawEllipse( x, y, w, h );

    x += 2;
    y += 2;
    w -= 4;
    h -= 4;
    QColor fillColor = ( down || !enabled ) ? g.button() : g.base();
    p->setBrush( fillColor );
    p->drawEllipse( x, y, w, h );

    if ( on ) {
	p->setBrush( g.text() );
	p->drawEllipse( x+2, y+2, w-4, h-4 );
    }
}

/*!
  \reimp
*/
void QInterlaceStyle::drawExclusiveIndicatorMask( QPainter *p, int x, int y, int w, int h, bool )
{
    p->setBrush( color1 );
    p->setPen( QPen( NoPen ) );
    p->drawEllipse( x, y, w, h );
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

/*!
  \reimp
*/
QRect QInterlaceStyle::comboButtonRect ( int x, int y, int w, int h )
{
    QRect r = buttonRect( x, y, w, h );
    int ew = get_combo_extra_width( r.height() );
    return QRect(r.x(), r.y(), r.width()-ew, r.height());
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

/*!
  \reimp
*/
void QInterlaceStyle::drawComboButton( QPainter *p, int x, int y, int w, int h,
				const QColorGroup &g,
				bool /* sunken */,
				bool /*editable*/,
				bool /*enabled */,
				const QBrush *fb )
{
    QBrush fill = fb ? *fb : g.brush( QColorGroup::Button );

    int awh, ax, ay, sh, sy, dh, ew;
    get_combo_parameters( buttonRect(x,y,w,h), ew, awh, ax, ay, sh, dh, sy );

    drawButton( p, x, y, w, h, g, FALSE, &fill );

    qDrawArrow( p, DownArrow, MotifStyle, FALSE, ax, ay, awh, awh, g, TRUE );

    p->setPen( g.dark() );
    p->drawRect( ax+1, sy+1, awh-1, sh-1 );
}

/*!
  \reimp
*/
void QInterlaceStyle::drawPushButtonLabel( QPushButton* btn, QPainter *p)
{
    QRect r = btn->rect();
    int x, y, w, h;
    r.rect( &x, &y, &w, &h );

    int x1, y1, x2, y2;
    btn->rect().coords( &x1, &y1, &x2, &y2 );	// get coordinates
    int dx = 0;
    int dy = 0;
    if ( btn->isMenuButton() )
	dx = (y2-y1) / 3;
    if ( btn->isOn() || btn->isDown() ) {
//	dx--;
//	dy--;
    }
    if ( dx || dy )
	p->translate( dx, dy );

    x += 2;  y += 2;  w -= 4;  h -= 4;
    QColorGroup g = btn->colorGroup();
    const QColor *col = &btn->colorGroup().buttonText();
    if ( (btn->isDown() || btn->isOn()) )
	col = &btn->colorGroup().brightText();
    else if ( !btn->isEnabled() )
	col = &btn->colorGroup().dark();
    drawItem( p, x, y, w, h,
	      AlignCenter|ShowPrefix,
	      g, btn->isEnabled(),
	      btn->pixmap(), btn->text(), -1, col );

    if ( dx || dy )
	p->translate( -dx, -dy );
}

#define HORIZONTAL	(sb->orientation() == QScrollBar::Horizontal)
#define VERTICAL	!HORIZONTAL
#define MOTIF_BORDER	defaultFrameWidth()
#define SLIDER_MIN	9 // ### motif says 6 but that's too small


/*! \reimp */

void QInterlaceStyle::scrollBarMetrics( const QScrollBar* sb, int &sliderMin, int &sliderMax, int &sliderLength, int &buttonDim )
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

void QInterlaceStyle::drawScrollBarControls( QPainter* p, const QScrollBar* sb,
					 int sliderStart, uint controls,
					 uint activeControl )
{
#define ADD_LINE_ACTIVE ( activeControl == AddLine )
#define SUB_LINE_ACTIVE ( activeControl == SubLine )
    QColorGroup g  = sb->colorGroup();

    int sliderMin, sliderMax, sliderLength, buttonDim;
    scrollBarMetrics( sb, sliderMin, sliderMax, sliderLength, buttonDim );

    QBrush fill = g.brush( QColorGroup::Mid );
    if (sb->backgroundPixmap() ){
	fill = QBrush( g.mid(), *sb->backgroundPixmap() );
    }

    if ( controls == (AddLine | SubLine | AddPage | SubPage | Slider | First | Last ) )
	drawPanel( p, 0, 0, sb->width(), sb->height(), g, FALSE, 2, &fill );

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
			  sliderStart - subB.right() , sliderW );
	addPageR.setRect( sliderEnd-1, b, addX - sliderEnd+1, sliderW );
	sliderR .setRect( sliderStart, b, sliderLength, sliderW );
    } else {
	subPageR.setRect( b, subB.bottom()+1, sliderW,
			  sliderStart - subB.bottom() );
	addPageR.setRect( b, sliderEnd-1, sliderW, addY - sliderEnd + 1);
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

    if ( controls & SubPage )
	p->fillRect( subPageR, fill );

    if ( controls & AddPage )
	p->fillRect( addPageR, fill );

    if ( controls & Slider ) {
	QPoint bo = p->brushOrigin();
	p->setBrushOrigin(sliderR.topLeft());
	if ( sliderR.isValid() )
	    drawButton( p, sliderR.x(), sliderR.y(),
			  sliderR.width(), sliderR.height(), g,
			  FALSE, &g.brush( QColorGroup::Button ) );
	p->setBrushOrigin(bo);
    }

}

/*!
  \reimp
*/
void QInterlaceStyle::drawSlider ( QPainter * p, int x, int y, int w, int h, const QColorGroup & g, Orientation orient, bool, bool)
{
    p->fillRect( x, y, w, h, g.brush( QColorGroup::Background ) );
    drawButton( p, x, y, w, h, g, FALSE, &g.brush( QColorGroup::Button ) );
    if ( orient == Horizontal ) {
	QCOORD mid = x + w / 2;
	qDrawShadeLine( p, mid,  y , mid,  y + h - 2, g, TRUE, 1);
    } else {
	QCOORD mid = y +h / 2;
	qDrawShadeLine( p, x, mid,  x + w - 2, mid, g, TRUE, 1);
    }
}

/*!
  \reimp
*/
void QInterlaceStyle::drawSliderMask ( QPainter * p, int x, int y, int w, int h, Orientation, bool, bool)
{
    drawButtonMask( p, x, y, w, h );
}

/*!
  \reimp
*/
void QInterlaceStyle::drawSliderGroove ( QPainter * p, int x, int y, int w, int h, const QColorGroup & g, QCOORD , Orientation o)
{
    p->setBrush( g.brush( QColorGroup::Dark ) );
    p->setPen( NoPen );

    if ( o == Horizontal )
	drawButton( p, x, y+h/2-3, w, 6, g, FALSE, &g.brush( QColorGroup::Mid ) );
    else
	drawButton( p, x+w/2-3, y, 6, h, g, FALSE, &g.brush( QColorGroup::Mid ) );
}

/*!
  \reimp
*/
void QInterlaceStyle::drawSliderGrooveMask( QPainter * p, int x, int y, int w, int h, QCOORD c, Orientation o)
{
    QColorGroup cg;
    cg.setBrush( QColorGroup::Dark, color1 );
    cg.setBrush( QColorGroup::Mid, color1 );
    drawSliderGroove( p, x, y, w, h, cg, c, o );
}

/*!
  \reimp
*/
int QInterlaceStyle::splitterWidth() const
{
    return QMAX( 12, QApplication::globalStrut().width() );
}

/*!
  \reimp
*/
void QInterlaceStyle::drawSplitter( QPainter *p, int x, int y, int w, int h,
  const QColorGroup &g, Orientation orient)
{
    const int motifOffset = 12;
    int sw = splitterWidth();
    if ( orient == Horizontal ) {
	    QCOORD xPos = x + w/2;
	    QCOORD kPos = motifOffset;
	    QCOORD kSize = sw - 4;

	    qDrawShadeLine( p, xPos, kPos + kSize - 1 ,
			    xPos, h, g );
	    drawPanel( p, xPos-sw/2+2, kPos,
			     kSize, kSize, g, FALSE, 2,
			     &g.brush( QColorGroup::Button ));
	    qDrawShadeLine( p, xPos, 0, xPos, kPos, g );
	} else {
	    QCOORD yPos = y + h/2;
	    QCOORD kPos = w - motifOffset - sw;
	    QCOORD kSize = sw - 4;

	    qDrawShadeLine( p, 0, yPos, kPos, yPos, g );
	    drawPanel( p, kPos, yPos-sw/2+2,
			     kSize, kSize, g, FALSE, 2,
			     &g.brush( QColorGroup::Button ));
	    qDrawShadeLine( p, kPos + kSize -1, yPos,
			    w, yPos, g );
	}

}

/*!
  \reimp
*/
void QInterlaceStyle::drawPanel( QPainter *p, int x, int y, int w, int h,
			    const QColorGroup &g, bool /*sunken*/,
			    int lineWidth, const QBrush *fill )
{
    if ( lineWidth < 2 )
	lineWidth = 2;

    p->setBrush( g.brush( QColorGroup::Dark ) );
    p->setPen( NoPen );

    p->drawRect( x, y, w, lineWidth );
    p->drawRect( x, y+h-lineWidth, w, lineWidth );
    p->drawRect( x, y, lineWidth, h );
    p->drawRect( x+w-lineWidth, y, lineWidth, h );

    if ( fill ) {
	x += lineWidth;
	y += lineWidth;
	w -= 2*lineWidth;
	h -= 2*lineWidth;
	p->setBrush( *fill );
	p->setPen( NoPen );
	p->drawRect( x, y, w, h );
    }
}

#endif // QT_NO_STYLE_INTERLACE

