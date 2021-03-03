/****************************************************************************
** $Id: qt/src/widgets/qcdestyle.cpp   2.3.2   edited 2001-01-26 $
**
** Implementation of CDE-like style class
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

#include "qcdestyle.h"
#ifndef QT_NO_STYLE_CDE
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
  \class QCDEStyle qcdestyle.h
  \brief CDE Look and Feel
  
  \ingroup appearance

  This style provides a slightly improved Motif look similar to some
  versions of the Common Desktop Environment (CDE). The main
  difference are thinner frames and more modern radio buttons and
  check boxes. Together with a dark background and a bright
  text/foreground color, the style looks quite attractive (at least
  for Motif fans).
*/

/*!
    Constructs a QCDEStyle

    If useHighlightCols is FALSE (the default value), then the style
    will polish the application's color palette to emulate the Motif
    way of highlighting, which is a simple inversion between the base
    and the text color.
*/
QCDEStyle::QCDEStyle( bool useHighlightCols ) : QMotifStyle( useHighlightCols )
{
}

/*!
  Destructs the style.
*/
QCDEStyle::~QCDEStyle()
{
}

/*!\reimp
  */

int QCDEStyle::defaultFrameWidth() const
{
    return 1;
}


/*!\reimp
  */

void QCDEStyle::drawArrow( QPainter *p, ArrowType type, bool down,
		 int x, int y, int w, int h,
		 const QColorGroup &g, bool enabled, const QBrush * /* fill */ )
{
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
	bFill.resize( dim & 1 ? 3 : 4 );
	bTop.resize( 2 );
	bBot.resize( 2 );
	bLeft.resize( 2 );
	bLeft.putPoints( 0, 2, 0,0, 0,dim-1 );
	bTop.putPoints( 0, 2, 1,0, dim-1, dim/2);
	bBot.putPoints( 0, 2, 1,dim-1, dim-1, dim/2);

	if ( dim > 6 ) {			// dim>6: must fill interior
	    bFill.putPoints( 0, 2, 1,dim-1, 1,1 );
	    if ( dim & 1 )			// if size is an odd number
		bFill.setPoint( 2, dim - 2, dim / 2 );
	    else
		bFill.putPoints( 2, 2, dim-2,dim/2-1, dim-2,dim/2 );
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
    p->setPen( CBOT );
    p->drawLineSegments( bBot );
    p->setPen( CTOP );
    p->drawLineSegments( bTop );

    p->setWorldMatrix( wxm );
    p->setBrush( saveBrush );			// restore brush
    p->setPen( savePen );			// restore pen

#undef CMID
#undef CLEFT
#undef CTOP
#undef CBOT

}

/*!\reimp
  */
void QCDEStyle::drawIndicator( QPainter* p,
			       int x, int y, int w, int h, const QColorGroup &g,
			       int s, bool down, bool /* enabled */ )
{
    bool showUp = !down && s == QButton::Off;
    QBrush fill =  down ? g.brush( QColorGroup::Mid )   :
			  g.brush( QColorGroup::Button );
    qDrawShadePanel( p, x, y, w, h, g, !showUp, defaultFrameWidth(), &fill );

    if (s != QButton::Off) {
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
	if ( s == QButton::NoChange )
	    p->setPen( g.dark() );
	else
	    p->setPen( g.foreground() );
	p->drawLineSegments( a );
    }
}


#define QCOORDARRLEN(x) sizeof(x)/(sizeof(QCOORD)*2)

/*!\reimp
  */
void QCDEStyle::drawExclusiveIndicator( QPainter* p,
				   int x, int y, int w, int h, const QColorGroup &g,
				   bool on, bool down, bool /* enabled */ )
{
    static const QCOORD pts1[] = {		// up left  lines
	1,9, 1,8, 0,7, 0,4, 1,3, 1,2, 2,1, 3,1, 4,0, 7,0, 8,1, 9,1 };
    static const QCOORD pts4[] = {		// bottom right  lines
	2,10, 3,10, 4,11, 7,11, 8,10, 9,10, 10,9, 10,8, 11,7,
	11,4, 10,3, 10,2 };
    static const QCOORD pts5[] = {		// inner fill
	4,2, 7,2, 9,4, 9,7, 7,9, 4,9, 2,7, 2,4 };

    p->eraseRect( x, y, w, h );
    QPointArray a( QCOORDARRLEN(pts1), pts1 );
    a.translate( x, y );
    p->setPen( (down||on) ? g.dark() : g.light() );
    p->drawPolyline( a );
    a.setPoints( QCOORDARRLEN(pts4), pts4 );
    a.translate( x, y );
    p->setPen(  (down||on) ? g.light() : g.dark() );
    p->drawPolyline( a );
    a.setPoints( QCOORDARRLEN(pts5), pts5 );
    a.translate( x, y );
    QColor fillColor = on ? g.dark() : g.background();
    p->setPen( fillColor );
    p->setBrush( on ?  g.brush( QColorGroup::Dark )        :
		       g.brush( QColorGroup::Background ) );
    p->drawPolygon( a );
}

#endif
