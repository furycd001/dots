/****************************************************************************
** $Id: qt/src/widgets/qradiobutton.cpp   2.3.2   edited 2001-04-18 $
**
** Implementation of QRadioButton class
**
** Created : 940222
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

#include "qradiobutton.h"
#ifndef QT_NO_RADIOBUTTON
#include "qbuttongroup.h"
#include "qpainter.h"
#include "qdrawutil.h"
#include "qpixmap.h"
#include "qpixmapcache.h"
#include "qbitmap.h"
#include "qtextstream.h"
#include "qapplication.h"

// NOT REVISED
/*!
  \class QRadioButton qradiobutton.h
  \brief The QRadioButton widget provides a radio button with a text label.

  \ingroup basic

  QRadioButton and QCheckBox are both option buttons. That is, they
  can be switched on (checked) or off (unchecked). The classes differ
  in how the choices for the user are restricted. Check-boxes define
  "many of many" choices, while radio buttons provide a "one of many"
  choice. In a group of radio buttons, only one button at a time can
  be checked. If the user selects another button, the previously
  selected button is switched off.

  While it is technically possible to implement radio-behaviour with
  check boxes and vice versa, it's strongly recommended to stick with
  the well-known semantics. Otherwise your users would be pretty
  confused.

  The easiest way to implement a "one of many" choice, is to simply
  stick the radio buttons into QButtonGroup.

  Whenver a button is switched on or off, it emits the signal
  toggled(). Connect to this signal if you want to trigger an action
  each time the button changes state. Otherwise, use isChecked() to
  query whether or not a particular button is selected.

  <img src=qradiobt-m.png> <img src=qradiobt-w.png>

  \important text, setText, text, pixmap, setPixmap, accel, setAccel,
  isToggleButton, setDown, isDown, isOn, state, autoRepeat,
  isExclusiveToggle, group, setAutoRepeat, toggle, pressed, released,
  clicked, toggled, state stateChanged

  \sa QPushButton QToolButton
  <a href="guibooks.html#fowler">GUI Design Handbook: Radio Button</a>
*/


static const int gutter = 6; // between button and text
static const int margin = 2; // to right of text


/*!
  Constructs a radio button with no text.

  The \e parent and \e name arguments are sent to the QWidget constructor.
*/

QRadioButton::QRadioButton( QWidget *parent, const char *name )
	: QButton( parent, name, WRepaintNoErase | WResizeNoErase | WMouseNoMask )
{
    init();
}

/*!
  Constructs a radio button with a text.

  The \e parent and \e name arguments are sent to the QWidget constructor.
*/

QRadioButton::QRadioButton( const QString &text, QWidget *parent,
			    const char *name )
	: QButton( parent, name, WRepaintNoErase | WResizeNoErase | WMouseNoMask )
{
    init();
    setText( text );
}


/*!
  Initializes the radio button.
*/

void QRadioButton::init()
{
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
    setToggleButton( TRUE );
    if ( parentWidget() && parentWidget()->inherits("QButtonGroup") ) {
	QButtonGroup *bgrp = (QButtonGroup *)parentWidget();
	bgrp->setRadioButtonExclusive( TRUE );
    }
}


/*!
  \fn bool QRadioButton::isChecked() const
  Returns TRUE if the radio button is checked, or FALSE if it is not checked.
  \sa setChecked()
*/

/*!
  Checks the radio button if \e check is TRUE, or unchecks it if \e check
  is FALSE.

  Calling this function does not affect other radio buttons unless a radio
  button group has been defined using the QButtonGroup widget.

  \sa isChecked()
*/
void QRadioButton::setChecked( bool check )
{
    setOn( check );
}




/*!\reimp
*/
QSize QRadioButton::sizeHint() const
{
    // Any more complex, and we will use style().itemRect()
    // NB: QCheckBox::sizeHint() is similar
    constPolish();
    QSize sz;
    if (pixmap())
	sz = pixmap()->size();
    else
	sz = fontMetrics().size( ShowPrefix, text() );
    QSize bmsz = style().exclusiveIndicatorSize();
    if ( sz.height() < bmsz.height() )
	sz.setHeight( bmsz.height() );

    return sz + QSize( bmsz.width()
			+ (text().isEmpty() ? 0 : gutter+margin),
			4 ).expandedTo( QApplication::globalStrut() );
}


/*!\reimp
*/

QSizePolicy QRadioButton::sizePolicy() const
{
    //### removeme 3.0
    return QWidget::sizePolicy();
}


/*!\reimp
*/
bool QRadioButton::hitButton( const QPoint &pos ) const
{
    return rect().contains( pos );
}


/*!\reimp
*/
void QRadioButton::drawButton( QPainter *paint )
{
    QPainter	*p = paint;
    const QColorGroup & g = colorGroup();
    int		 x, y;

    QFontMetrics fm = fontMetrics();
    QSize lsz = fm.size(ShowPrefix, text());
    QSize sz = style().exclusiveIndicatorSize();
    x = text().isEmpty() ? 1 : 0;
    y = (height() - lsz.height() + fm.height() - sz.height())/2;

#ifndef QT_NO_TEXTSTREAM
#define SAVE_RADIOBUTTON_PIXMAPS
#endif

#if defined(SAVE_RADIOBUTTON_PIXMAPS)
    QString pmkey;				// pixmap key
    int kf = 0;
    if ( isDown() )
	kf |= 1;
    if ( isOn() )
	kf |= 2;
    if ( isEnabled() )
	kf |= 4;
    QTextOStream os(&pmkey);
    os << "$qt_radio_" << style().className() << "_"
			 << palette().serialNumber() << "_" << kf;
    QPixmap *pm = QPixmapCache::find( pmkey );
    if ( pm ) {					// pixmap exists
	drawButtonLabel( p );
	p->drawPixmap( x, y, *pm );
	return;
    }
    bool use_pm = TRUE;
    QPainter pmpaint;
    int wx, wy;
    if ( use_pm ) {
	pm = new QPixmap( sz );			// create new pixmap
	CHECK_PTR( pm );
	pmpaint.begin( pm );
	p = &pmpaint;				// draw in pixmap
	wx=x;  wy=y;				// save x,y coords
	x = y = 0;
	p->setBackgroundColor( g.background() );
    }
#endif

#define QCOORDARRLEN(x) sizeof(x)/(sizeof(QCOORD)*2)

    style().drawExclusiveIndicator(p, x, y, sz.width(), sz.height(), g, isOn(), isDown(), isEnabled() );

#if defined(SAVE_RADIOBUTTON_PIXMAPS)
    if ( use_pm ) {
	pmpaint.end();
	if ( backgroundPixmap() || backgroundMode() == X11ParentRelative ) {
	    QBitmap bm( pm->size() );
	    bm.fill( color0 );
	    pmpaint.begin( &bm );
	    style().drawExclusiveIndicatorMask( &pmpaint, 0, 0, bm.width(), bm.height(), isOn() );
	    pmpaint.end();
	    pm->setMask( bm );
	}
	p = paint;				// draw in default device
	p->drawPixmap( wx, wy, *pm );
	if (!QPixmapCache::insert(pmkey, pm) )	// save in cache
	    delete pm;
    }
#endif
    drawButtonLabel( p );
}



/*!\reimp
*/
void QRadioButton::drawButtonLabel( QPainter *p )
{
    int x, y, w, h;
    GUIStyle gs = style();
    QSize sz = style().exclusiveIndicatorSize();
    if ( gs == WindowsStyle )
	sz.setWidth(sz.width()+1);
    y = 0;
    x = sz.width() + gutter;
    w = width() - x;
    h = height();

    style().drawItem( p, x, y, w, h,
		      AlignLeft|AlignVCenter|ShowPrefix,
		      colorGroup(), isEnabled(),
		      pixmap(), text() );

    if ( hasFocus() ) {
	QRect br = style().itemRect( p, x, y, w, h,
				     AlignLeft|AlignVCenter|ShowPrefix,
				     isEnabled(),
				     pixmap(), text() );
	br.setLeft( br.left()-3 );
	br.setRight( br.right()+2 );
	br.setTop( br.top()-2 );
	br.setBottom( br.bottom()+2);
	br = br.intersect( QRect(0,0,width(),height()) );

	if ( !text().isEmpty() )
	    style().drawFocusRect( p, br, colorGroup() );
	else {
	    br.setRight( br.left()-1 );
	    br.setLeft( br.left()-16 );
	    br.setTop( br.top() );
	    br.setBottom( br.bottom() );
	    style().drawFocusRect( p, br, colorGroup() );
	}
    }
}


/*!
  \reimp
*/
void QRadioButton::resizeEvent( QResizeEvent* e )
{
    QButton::resizeEvent(e);
    int x, w, h;
    GUIStyle gs = style();
    QSize sz = style().exclusiveIndicatorSize();
    if ( gs == WindowsStyle )
	sz.setWidth(sz.width()+1);
    x = sz.width() + gutter;
    w = width() - x;
    h = height();

    QPainter p(this);
    QRect br = style().itemRect( &p, x, 0, w, h,
				 AlignLeft|AlignVCenter|ShowPrefix,
				 isEnabled(),
				 pixmap(), text() );
    if ( autoMask() )
	updateMask();
    update( br.right(), w, 0, h );
}


/*!\reimp
 */
void QRadioButton::updateMask()
{
    QBitmap bm(width(),height());
    {
	bm.fill(color0);
	QPainter p( &bm, this );
	int x, y, w, h;
	GUIStyle gs = style();
	QFontMetrics fm = fontMetrics();
	QSize lsz = fm.size(ShowPrefix, text());
	QSize sz = style().exclusiveIndicatorSize();
	if ( gs == WindowsStyle )
	    sz.setWidth(sz.width()+1);
	y = 0;
	x = sz.width() + gutter;
	w = width() - x;
	h = height();

	QColorGroup cg(color1,color1, color1,color1,color1,color1,color1,color1, color0);

	style().drawItem( &p, x, y, w, h,
			  AlignLeft|AlignVCenter|ShowPrefix,
			  cg, TRUE,
			  pixmap(), text() );
	x = 0;
	y = (height() - lsz.height() + fm.height() - sz.height())/2;

	style().drawExclusiveIndicatorMask(&p, x, y, sz.width(), sz.height(), isOn() );

	if ( hasFocus() ) {
 	    y = 0;
 	    x = sz.width() + gutter;
 	    w = width() - x;
 	    h = height();
	    QRect br = style().itemRect( &p, x, y, w, h,
					 AlignLeft|AlignVCenter|ShowPrefix,
					 isEnabled(),
					 pixmap(), text() );
	    br.setLeft( br.left()-3 );
	    br.setRight( br.right()+2 );
	    br.setTop( br.top()-2 );
	    br.setBottom( br.bottom()+2);
	    br = br.intersect( QRect(0,0,width(),height()) );

	    if ( !text().isEmpty() )
		style().drawFocusRect( &p, br, cg );
	    else {
		br.setRight( br.left()-1 );
		br.setLeft( br.left()-16 );
		br.setTop( br.top() );
		br.setBottom( br.bottom() );
		style().drawFocusRect( &p, br, cg );
	    }

	}
    }
    setMask(bm);
}


/*! \reimp */

void QRadioButton::focusInEvent( QFocusEvent *e  )
{
    QButton::focusInEvent( e ); // ### 3.0 remove
}
#endif
