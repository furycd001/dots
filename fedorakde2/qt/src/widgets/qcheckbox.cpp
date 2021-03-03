/****************************************************************************
** $Id: qt/src/widgets/qcheckbox.cpp   2.3.2   edited 2001-04-18 $
**
** Implementation of QCheckBox class
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

#include "qcheckbox.h"
#ifndef QT_NO_CHECKBOX
#include "qpainter.h"
#include "qdrawutil.h"
#include "qpixmap.h"
#include "qpixmapcache.h"
#include "qbitmap.h"
#include "qtextstream.h"
#include "qapplication.h"

// NOT REVISED
/*!
  \class QCheckBox qcheckbox.h
  \brief The QCheckBox widget provides a check box with a text label.

  \ingroup basic

  QCheckBox and QRadioButton are both option buttons. That is, they
  can be switched on (checked) or off (unchecked). The classes differ
  in how the choices for the user are restricted. Radio buttons define
  a "one of many" choice, while check-boxes provide "many of many"
  choices.

  While it is technically possible to implement radio-behaviour with
  check boxes and vice versa, it's strongly recommended to stick with
  the well-known semantics. Otherwise your users would be pretty
  confused.

  Use QButtonGroup to group check-buttons visually.

  Whenver a check box is checked or cleared, it emits the signal
  toggled(). Connect to this signal if you want to trigger an action
  each time the box changes state. Otherwise, use isChecked() to query
  whether or not a particular check box is selected.

  In addition to the usual checked and unchecked states, QCheckBox
  optionally provides a third state to indicate "no change".  This is
  useful whenever you need to give the user the option of neither
  setting nor unsetting an option. If you need that third state,
  enable it with setTristate() and use state() to query the current
  toggle state. When a tristate box changes state, it emits the
  stateChanged() signal.

  \important text, setText, text, pixmap, setPixmap, accel, setAccel,
  isToggleButton, setDown, isDown, isOn, state, autoRepeat,
  isExclusiveToggle, group, setAutoRepeat, toggle, pressed, released,
  clicked, toggled, state stateChanged

  <img src=qchkbox-m.png> <img src=qchkbox-w.png>

  \sa QButton QRadioButton
  <a href="guibooks.html#fowler">Fowler: Check Box.</a>
*/



/*!
  Constructs a check box with no text.

  The \e parent and \e name arguments are sent to the QWidget constructor.
*/

QCheckBox::QCheckBox( QWidget *parent, const char *name )
	: QButton( parent, name, WRepaintNoErase | WResizeNoErase | WMouseNoMask )
{
    setToggleButton( TRUE );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
}

/*!
  Constructs a check box with a text.

  The \e parent and \e name arguments are sent to the QWidget constructor.
*/

QCheckBox::QCheckBox( const QString &text, QWidget *parent, const char *name )
	: QButton( parent, name, WRepaintNoErase | WResizeNoErase | WMouseNoMask )
{
    setText( text );
    setToggleButton( TRUE );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
}


/*!
  \fn bool QCheckBox::isChecked() const
  Returns TRUE if the check box is checked, or FALSE if it is not checked.
  \sa setChecked()
*/

/*!
  \fn void QCheckBox::setChecked( bool check )
  Checks the check box if \e check is TRUE, or unchecks it if \e check
  is FALSE.
  \sa isChecked()
*/

/*!
  Sets the checkbox into the "no change" state.

  \sa setTristate()
*/
void QCheckBox::setNoChange()
{
    setTristate(TRUE);
    setState( NoChange );
}

/*!
  Makes the check box a tristate check box if \a y is TRUE.  A tristate
  check box provides an additional state NoChange.

  Use tristate check boxes whenever you need to give the user the
  option of neither setting nor unsetting an option. A typical example
  is the "Italic" check box in the font dialog of a word processor
  when the marked text is partially Italic and partially not.

  \sa isTristate(), setNoChange() stateChanged(), state()
*/
void QCheckBox::setTristate(bool y)
{
    setToggleType( y ? Tristate : Toggle );
}

/*!
  Returns TRUE if the checkbox is a tristate checkbox. Otherwise returns
  FALSE.

  \sa setTristate()
*/
bool QCheckBox::isTristate() const
{
    return toggleType() == Tristate;
}

static int extraWidth( int gs )
{
    if ( gs == Qt::MotifStyle )
	return 8;
    else
	return 6;
}


/*!\reimp
*/
QSize QCheckBox::sizeHint() const
{
    // Any more complex, and we will use style().itemRect()
    // NB: QCheckBox::sizeHint() is similar
    constPolish();
    QSize sz;
    if (pixmap()) {
	sz = pixmap()->size();
    } else {
	sz = fontMetrics().size( ShowPrefix, text() );
    }
    GUIStyle gs = style().guiStyle();
    QSize bmsz = style().indicatorSize();
    if ( sz.height() < bmsz.height() )
	sz.setHeight( bmsz.height() );

    return sz + QSize( bmsz.width() + (style()==MotifStyle ? 1 : 0)
	+ (text().isEmpty() ? 0 : 4 + extraWidth(gs)),
	4 ).expandedTo( QApplication::globalStrut() );
}


/*!\reimp
*/

void QCheckBox::drawButton( QPainter *paint )
{
    QPainter	*p = paint;
    GUIStyle	 gs = style().guiStyle();
    const QColorGroup & g = colorGroup();
    int		 x, y;

    QFontMetrics fm = fontMetrics();
    QSize lsz = fm.size(ShowPrefix, text());
    QSize sz = style().indicatorSize();
    x = gs == MotifStyle ? 1 : 0;
    if ( text().isEmpty() )
	x += 1;
    y = (height() - lsz.height() + fm.height() - sz.height())/2;

#ifndef QT_NO_TEXTSTREAM
#define SAVE_CHECKBOX_PIXMAPS
#endif

#if defined(SAVE_CHECKBOX_PIXMAPS)
    QString pmkey;				// pixmap key
    int kf = 0;
    if ( isDown() )
	kf |= 1;
    if ( isEnabled() )
	kf |= 2;
    if ( hasFocus() )
	kf |= 4;				// active vs. normal colorgroup
    kf |= state() << 3;
    QTextOStream os(&pmkey);
    os << "$qt_check_" << style().className() << "_"
			 << palette().serialNumber() << "_" << kf;
    QPixmap *pm = QPixmapCache::find( pmkey );
    if ( pm ) {					// pixmap exists
	p->drawPixmap( x, y, *pm );
	drawButtonLabel( p );
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

    style().drawIndicator(p, x, y, sz.width(), sz.height(), colorGroup(), state(), isDown(), isEnabled());

#if defined(SAVE_CHECKBOX_PIXMAPS)
    if ( use_pm ) {
	pmpaint.end();
	if ( backgroundPixmap() || backgroundMode() == X11ParentRelative ) {
	    QBitmap bm( pm->size() );
	    bm.fill( color0 );
	    pmpaint.begin( &bm );
	    style().drawIndicatorMask( &pmpaint, 0, 0, bm.width(), bm.height(), isOn() );
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
void QCheckBox::drawButtonLabel( QPainter *p )
{
    int x, y, w, h;
    GUIStyle gs = style().guiStyle();
    QSize sz = style().indicatorSize();
    y = 0;
    x = sz.width() + extraWidth( gs ); //###
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
	    style().drawFocusRect(p, br, colorGroup());
	else {
	    br.setRight( br.left() );
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
void QCheckBox::resizeEvent( QResizeEvent* )
{
    int x, w, h;
    GUIStyle gs = style().guiStyle();
    QSize sz = style().indicatorSize();
    x = sz.width() + extraWidth( gs );
    w = width() - x;
    h = height();

    QPainter p(this);
    QRect br = style().itemRect( &p, x, 0, w, h,
				 AlignLeft|AlignVCenter|ShowPrefix,
				 isEnabled(),
				 pixmap(), text() );
    update( br.right(), w, 0, h );
    if ( autoMask() )
	updateMask();
}

/*!
  \reimp
*/
void QCheckBox::updateMask()
{
    QBitmap bm(width(),height());
    bm.fill(color0);
    QPainter p( &bm, this );
    int x, y, w, h;
    GUIStyle gs = style().guiStyle();
    QFontMetrics fm = fontMetrics();
    QSize lsz = fm.size(ShowPrefix, text());
    QSize sz = style().indicatorSize();
    y = 0;
    x = sz.width() + extraWidth(gs);
    w = width() - x;
    h = height();

    QColorGroup cg(color1,color1,color1,color1,color1,color1,color1,color1,color0);

    style().drawItem( &p, x, y, w, h,
		      AlignLeft|AlignVCenter|ShowPrefix,
		      cg, TRUE,
		      pixmap(), text() );
    x = 0;
    y = (height() - lsz.height() + fm.height() - sz.height())/2;
	
    style().drawIndicatorMask(&p, x, y, sz.width(), sz.height(), state() );

    if ( hasFocus() ) {
	y = 0;
	x = sz.width() + extraWidth(gs);
	QRect br = style().itemRect( &p, x, y, w, h,
				     AlignLeft|AlignVCenter|ShowPrefix,
				     isEnabled(),
				     pixmap(), text() );
	br.setLeft( br.left()-3 );
	br.setRight( br.right()+2 );
	br.setTop( br.top()-2 );
	br.setBottom( br.bottom()+2 );
	br = br.intersect( QRect(0,0,width(),height()) );

	if ( !text().isEmpty() )
	    style().drawFocusRect( &p, br, cg );
	else {
	    br.setRight( br.left() );
	    br.setLeft( br.left()-17 );
	    br.setTop( br.top() );
	    br.setBottom( br.bottom() );
	    style().drawFocusRect( &p, br, cg );
	}
    }
    setMask(bm);
}


/*!\reimp
*/
QSizePolicy QCheckBox::sizePolicy() const
{
    //### removeme 3.0
    return QWidget::sizePolicy();
}
#endif
