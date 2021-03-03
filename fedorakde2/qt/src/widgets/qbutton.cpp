/****************************************************************************
** $Id: qt/src/widgets/qbutton.cpp   2.3.2   edited 2001-03-24 $
**
** Implementation of QButton widget class
**
** Created : 940206
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

#include "qbutton.h"
#ifndef QT_NO_BUTTON
#include "qbuttongroup.h"
#include "qbitmap.h"
#include "qpainter.h"
#include "qtimer.h"
#include "qaccel.h"
#include "qpixmapcache.h"
#include "qfocusdata.h"
#include "qapplication.h"
#include "qpushbutton.h"
#include <ctype.h>

static const int autoRepeatDelay  = 300;
static const int autoRepeatPeriod = 100;

#ifdef _WS_QWS_
// Small in Qt/Embedded - 5K on 32bpp
static const int drawingPixWidth  = 64;
static const int drawingPixHeight = 20;
#else
// 120K on 32bpp
static const int drawingPixWidth  = 300;
static const int drawingPixHeight = 100;
#endif


/*
  Returns a pixmap of dimension (drawingPixWidth x drawingPixHeight). The
  pixmap is used by paintEvent for flicker-free drawing.
 */

static QPixmap *drawpm = 0;


static void cleanupButtonPm()
{
    delete drawpm;
    drawpm = 0;
}


static inline void makeDrawingPixmap()
{
    if ( !drawpm ) {
	qAddPostRoutine( cleanupButtonPm );
	drawpm = new QPixmap( drawingPixWidth, drawingPixHeight );
	CHECK_PTR( drawpm );
    }
}


struct QButtonData
{
    QButtonData() {
#ifndef QT_NO_BUTTONGROUP
	group = 0;
#endif
#ifndef QT_NO_ACCEL
	a = 0;
#endif
    }
#ifndef QT_NO_BUTTONGROUP
    QButtonGroup *group;
#endif
    QTimer timer;
#ifndef QT_NO_ACCEL
    QAccel *a;
#endif
};


void QButton::ensureData()
{
    if ( !d ) {
	d = new QButtonData;
	CHECK_PTR( d );
	connect(&d->timer, SIGNAL(timeout()), this, SLOT(autoRepeatTimeout()));
    }
}


/*! Returns a pointer to the group of which this button is a member.

  If the button is not a member of any QButtonGroup, this function
  returns 0.

  \sa setGroup() QButtonGroup
*/

QButtonGroup *QButton::group() const
{
#ifndef QT_NO_BUTTONGROUP
    return d ? d->group : 0;
#else
    return 0;
#endif
}


void QButton::setGroup( QButtonGroup* g )
{
#ifndef QT_NO_BUTTONGROUP
    ensureData();
    d->group = g;
#endif
}


QTimer *QButton::timer()
{
    ensureData();
    return &d->timer;
}


// NOT REVISED
/*!
  \class QButton qbutton.h

  \brief The QButton class is the abstract base class of button
  widgets, providing functionality common to buttons.

  \ingroup abstractwidgets

  The QButton class implements an abstract button, and lets subclasses
  specify how to reply to user actions and how to draw the button.

  QButton provides both push and toggle buttons.  The QRadioButton and
  QCheckBox classes provide only toggle buttons, QPushButton and
  QToolButton provide both toggle and push buttons.

  Any button can have either a text or pixmap label.  setText() sets
  the button to be a text button and setPixmap() sets it to be a
  pixmap button.  The text/pixmap is manipulated as necessary to
  create "disabled" appearance when the button is disabled.

  QButton provides most of the states used for buttons:
  <ul>
  <li>isDown() determines whether the button is \e pressed down.
  <li>isOn() determines whether the button is \e on.
  Only toggle buttons can be switched on and off  (see below).
  <li>isEnabled() determines whether the button can be pressed by the
  user.
  <li>setAutoRepeat() determines whether the button will auto-repeat
  if the user holds it down.
  <li>setToggleButton() determines whether the button is a toggle
  button or not.
  </ul>

  The difference between isDown() and isOn() is as follows:
  When the user clicks a toggle button to toggle it on, the button is
  first \e pressed, then released into \e on state.  When the user
  clicks it again (to toggle it off) the button moves first to the \e
  pressed state, then to the \e off state (isOn() and isDown() are
  both FALSE).

  Default buttons (as used in many dialogs) are provided by
  QPushButton::setDefault() and QPushButton::setAutoDefault().

  QButton provides four signals:
  <ul>
  <li>pressed() is emitted when the left mouse button is pressed while
  the mouse cursor is inside the button.
  <li>released() is emitted when the left mouse button is released.
  <li>clicked() is emitted when the button is first pressed then
  released, or when the accelerator key is typed, or when animateClick()
  is called.
  <li>toggled(bool) is emitted when the state of a toggle button changes.
  <li>stateChanged(int) is emitted when the state of a tristate
			toggle button changes.
  </ul>

  If the button is a text button with "&" in its text, QButton creates
  an automatic accelerator key.  This code creates a push button
  labelled "Rock & Roll" (where the c is underscored).  The button
  gets an automatic accelerator key, Alt-C:

  \code
    QPushButton *p = new QPushButton( "Ro&ck && Roll", this );
  \endcode

  In this example, when the user presses Alt-C the button will
  call animateClick().

  You can also set a custom accelerator using the setAccel() function.
  This is useful mostly for pixmap buttons since they have no
  automatic accelerator.

  \code
    QPushButton *p;
    p->setPixmap( QPixmap("print.png") );
    p->setAccel( ALT+Key_F7 );
  \endcode

  All of the buttons provided by Qt (\l QPushButton, \l QToolButton,
  \l QCheckBox and \l QRadioButton) can display both text and pixmaps.

  To subclass QButton, you have to reimplement at least drawButton()
  (to draw the button's outskirts) and drawButtonLabel() (to draw its
  text or pixmap).  It is generally advisable to reimplement
  sizeHint() as well, and sometimes hitButton() (to determine whether
  a button press is within the button).

  To reduce flickering the QButton::paintEvent() sets up a pixmap that the
  drawButton() function draws in. You should not reimplement paintEvent()
  for a subclass of QButton unless you want to take over all drawing.

  \sa QButtonGroup
*/


/*! \enum QButton::ToggleType

  This enum type defines what a button can do in response to a
  mouse/keyboard press: <ul>

  <li> \c SingleShot - pressing the button causes an action, then the
  button returns to the unpressed state.

  <li> \c Toggle - pressing the button toggles it between an \c On and
  and \c Off state.

  <li> \c Tristate - pressing the button cycles between the three
  states \c On, \c Off and \c NoChange

  </ul>

*/

/*! \enum QButton::ToggleState

  This enum defines the state of a toggle button at any moment.  The possible values are: <ul>

  <li> \c Off - the button is in the "off" state
  <li> \c NoChange - the button is in the default/unchanged state
  <li> \c On - the button is in the "on" state

  </ul>
*/


/*!
  Constructs a standard button with a parent widget and a name.

  If \a parent is a QButtonGroup, this constructor calls
  QButtonGroup::insert().
*/

QButton::QButton( QWidget *parent, const char *name, WFlags f )
    : QWidget( parent, name, f )
{
    bpixmap    = 0;
    toggleTyp  = SingleShot;			// button is simple
    buttonDown = FALSE;				// button is up
    stat       = Off;				// button is off
    mlbDown    = FALSE;				// mouse left button up
    autoresize = FALSE;				// not auto resizing
    animation  = FALSE;				// no pending animateClick
    repeat     = FALSE;				// not in autorepeat mode
    d	       = 0;
#ifndef QT_NO_BUTTONGROUP
    if ( parent && parent->inherits("QButtonGroup") ) {
	setGroup((QButtonGroup*)parent);
	group()->insert( this );		// insert into button group
    }
#endif
    setFocusPolicy( TabFocus );
}

/*!
  Destructs the button, deleting all its child widgets.
*/

QButton::~QButton()
{
#ifndef QT_NO_BUTTONGROUP
    if ( group() )
	group()->remove( this );
#endif
    delete bpixmap;
    delete d;
}


/*!
  \fn void QButton::pressed()
  This signal is emitted when the button is pressed down.

  \sa released(), clicked()
*/

/*!
  \fn void QButton::released()
  This signal is emitted when the button is released.

  \sa pressed(), clicked(), toggled()
*/

/*!
  \fn void QButton::clicked() This signal is emitted when the button
  is activated, i.e. first pressed down and then released when the
  mouse cursor is inside the button, or when the accelerator key is
  typed, or when animateClick() is called.

  \sa pressed(), released(), toggled()
*/

/*!
  \fn void QButton::toggled( bool on )
  This signal is emitted whenever a toggle button changes status.
  \e on is TRUE if the button is on, or FALSE if the button is off.

  This may be the result of a user action, toggle() slot activation,
  or because setOn() was called.

  \sa clicked()
*/

/*!
  \fn void QButton::stateChanged( int state )
  This signal is emitted whenever a toggle button changes status.
  \e state is 2 if the button is on, 1 if it is in the
  \link QCheckBox::setTristate() "no change" state\endlink
  or 0 if the button is off.

  This may be the result of a user action, toggle() slot activation,
  setState(), or because setOn() was called.

  \sa clicked()
*/


/*!
  \fn QString QButton::text() const
  Returns the button text, or
  \link QString::operator!() null string\endlink
  if the button has no text.
  \sa setText()
*/

/*!
  Sets the button to display \e text.

  If the text contains an ampersand, QButton creates an automatic
  accelerator for it, such as  Alt-c for "&Cancel".

  \sa text(), setPixmap(), setAccel(), QPixmap::mask()
*/

void QButton::setText( const QString &text )
{
    if ( btext == text )
	return;
    btext = text;

    if ( bpixmap ) {
	delete bpixmap;
	bpixmap = 0;
    }

    if ( autoresize )
	adjustSize();

#ifndef QT_NO_ACCEL
    setAccel( QAccel::shortcutKey( btext ) );
#endif

    update();
    updateGeometry();
}


/*!
  \fn const QPixmap *QButton::pixmap() const
  Returns the button pixmap, or 0 if the button has no pixmap.
*/

/*!
  Sets the button to display \a pixmap

  If \a pixmap is monochrome (i.e. it is a QBitmap or its \link
  QPixmap::depth() depth\endlink is 1) and it does not have a mask,
  this function sets the pixmap to be its own mask. The purpose of
  this is to draw transparent bitmaps, which is important for
  e.g. toggle buttons.

  \sa pixmap(), setText(), setAccel(), QPixmap::mask()
*/

void QButton::setPixmap( const QPixmap &pixmap )
{
    if ( bpixmap && bpixmap->serialNumber() == pixmap.serialNumber() )
	return;

    bool newSize;
    if ( bpixmap ) {
	newSize = pixmap.width() != bpixmap->width() ||
		  pixmap.height() != bpixmap->height();
	*bpixmap = pixmap;
    } else {
	newSize = TRUE;
	bpixmap = new QPixmap( pixmap );
	CHECK_PTR( bpixmap );
    }
    if ( bpixmap->depth() == 1 && !bpixmap->mask() )
	bpixmap->setMask( *((QBitmap *)bpixmap) );
    if ( !btext.isNull() )
	btext = QString::null;
    if ( autoresize && newSize )
	adjustSize();
    setAccel( 0 );
    if ( autoMask() )
	updateMask();
    update();
    updateGeometry();
}


/*!
  Returns the accelerator key currently set for the button, or 0
  if no accelerator key has been set.
  \sa setAccel()
*/

int QButton::accel() const
{
#ifndef QT_NO_ACCEL
    return d && d->a ? d->a->key( 0 ) : 0;
#else
    return 0;
#endif
}

/*!
  Specifies an accelerator \a key for the button, or removes the
  accelerator if \a key is 0.

  Setting a button text containing a shortcut character (for
  example the 'x' in E&xit) automatically defines an ALT+letter
  accelerator for the button.
  You only need to call this function in order to specify a custom
  accelerator.

  Example:
  \code
    QPushButton *b1 = new QPushButton;
    b1->setText( "&OK" );		// sets accel ALT+'O'

    QPushButton *b2 = new QPushButton;
    b2->setPixmap( printIcon );		// pixmap instead of text
    b2->setAccel( CTRL+'P' );		// custom accel
  \endcode

  \sa accel(), setText(), QAccel
*/

void QButton::setAccel( int key )
{
#ifndef QT_NO_ACCEL
    if ( d && d->a )
	d->a->clear();
    if ( !key )
	return;
    ensureData();
    if ( !d->a )
	d->a = new QAccel( this, "buttonAccel" );
    d->a->connectItem( d->a->insertItem( key, 0 ),
		       this, SLOT(animateClick()) );
#endif
}


/*!
  \fn bool QButton::autoResize() const

  \obsolete

  Strange pre-layout stuff.

  Returns TRUE if auto-resizing is enabled, or FALSE if auto-resizing is
  disabled.

  Auto-resizing is disabled by default.

  \sa setAutoResize()
*/


/*!\obsolete

  Strange pre-layout stuff.

  Enables auto-resizing if \e enable is TRUE, or disables it if \e enable is
  FALSE.

  When auto-resizing is enabled, the button will resize itself whenever
  the contents change.

  \sa autoResize(), adjustSize()
*/

void QButton::setAutoResize( bool enable )
{
    if ( (bool)autoresize != enable ) {
	autoresize = enable;
	if ( autoresize )
	    adjustSize();			// calls resize which repaints
    }
}


/*!
  \fn bool QButton::autoRepeat() const

  Returns TRUE if the button is auto-repeating, else FALSE.

  The default is FALSE.

  \sa setAutoRepeat()
*/


/*!
  Turns on auto-repeat for the button if \a enable is TRUE, or
  turns it off if \a enable is FALSE.

  When auto-repeat is enabled, the clicked() signal is emitted at
  regular intervals while the buttons \link isDown() is down. \endlink

  setAutoRepeat() has no effect for \link setToggleButton() toggle
  buttons. \endlink

  \sa isDown(), autoRepeat(), clicked()
*/

void QButton::setAutoRepeat( bool enable )
{
    repeat = (uint)enable;
    if ( repeat && mlbDown )
	timer()->start( autoRepeatDelay, TRUE );
}


/*!
  Performs an animated click: The button is pressed and a short while
  later released.

  pressed(), released(), clicked(), toggled(), and stateChanged()
  signals are emitted as appropriate.

  This function does nothing if the button is \link setEnabled()
  disabled. \endlink

  \sa setAccel()
*/

void QButton::animateClick()
{
    if ( !isEnabled() || animation )
	return;
    animation  = TRUE;
    buttonDown = TRUE;
    repaint( FALSE );
    emit pressed();
    QTimer::singleShot( 100, this, SLOT(animateTimeout()) );
}


/*!
  \fn bool QButton::isDown() const
  Returns TRUE if the button pressed down, or FALSE if it is standing up.
  \sa setDown()
*/

/*!
  Sets the state of the button to pressed down if \e enable is TRUE
  or to standing up if \e enable is FALSE.

  If the button is a toggle button, it is \e not toggled.  Call
  toggle() as well if you need to do that.  The pressed() and
  released() signals are not emitted by this function.

  This method is provided in case you need to reimplement the mouse event
  handlers.

  \sa isDown(), setOn(), toggle(), toggled()
*/

void QButton::setDown( bool enable )
{
    if ( d )
	timer()->stop();
    mlbDown = FALSE;				// the safe setting
    if ( (bool)buttonDown != enable ) {
	buttonDown = enable;
	repaint( FALSE );
    }
}

/*!
  \fn QButton::ToggleType QButton::toggleType() const

  Returns the current toggle type.

  \sa setToggleType()
*/

/*!
  \fn void QButton::setState( ToggleState t)

  This protected function sets the button state into state t but does
  \e not cause repainting.

  \sa setToggleType()
*/

/*!
  \fn QButton::ToggleState QButton::state() const
  Returns the state of the button.

  \sa ToggleState ToggleType setState()
*/

/*!
  \fn bool QButton::isOn() const
  Returns TRUE if this toggle button is switched on, or FALSE if it is
  switched off.
  \sa setOn(), isToggleButton()
*/

/*!
  \fn void QButton::setOn( bool enable )

  Switches a toggle button on if \e enable is TRUE or off if \e enable is
  FALSE.  This function should be called only for toggle buttons.
  \sa isOn(), isToggleButton()
*/

void QButton::setState( ToggleState s )
{
    if ( !toggleTyp ) {
#if defined(CHECK_STATE)
	qWarning( "QButton::setState() / setOn: (%s) Only toggle buttons "
		 "may be switched", name( "unnamed" ) );
#endif
	return;
    }

    if ( (ToggleState)stat != s ) {		// changed state
	bool was = stat != Off;
	stat = s;
    if ( autoMask() )
        updateMask();
	repaint( FALSE );
	if ( was != (stat != Off) )
	    emit toggled( stat != Off );
	emit stateChanged( s );
    }
}


/*!
  \fn bool QButton::isToggleButton() const
  Returns TRUE if the button is a toggle button.
  \sa setToggleButton()
*/

/*!
  \fn void QButton::setToggleButton( bool enable )

  Makes the button a toggle button if \e enable is TRUE, or a normal button
  if \e enable is FALSE.

  Note that this function is protected. It is called from subclasses
  to enable the toggle functionality. QCheckBox and QRadioButton are
  toggle buttons. QPushButton is initially not a toggle button, but
  QPushButton::setToggleButton() can be called to create toggle buttons.

  \sa isToggleButton()
*/

/*!
  Returns TRUE if \e pos is inside the clickable button rectangle, or
  FALSE if it is outside.

  Per default, the clickable area is the entire widget. Subclasses may
  reimplement it, though.
*/
bool QButton::hitButton( const QPoint &pos ) const
{
    return rect().contains( pos );
}

/*!
  Draws the button.  The default implementation does nothing.

  This virtual function is reimplemented by subclasses to draw real
  buttons. At some point in time, these reimplementations are supposed
  to call drawButtonLabel().

  \sa drawButtonLabel(), paintEvent()
*/

void QButton::drawButton( QPainter * )
{
    return;
}

/*!
  Draws the button text or pixmap.

  This virtual function is reimplemented by subclasses to draw real
  buttons. It's invoked by drawButton().

  \sa drawButton(), paintEvent()
*/

void QButton::drawButtonLabel( QPainter * )
{
    return;
}


static bool got_a_release = FALSE; // ### binary compatibility trick, keyReleaseEvent is new
/*!\reimp
*/

void QButton::keyPressEvent( QKeyEvent *e )
{
    switch ( e->key() ) {
    case Key_Enter:
    case Key_Return:
	if ( inherits("QPushButton") )
	    emit clicked();
	else
	    e->ignore();
	break;
    case Key_Space:
	if ( !e->isAutoRepeat() ) {
	    if ( got_a_release )
		setDown( TRUE );
	    else {
		buttonDown = TRUE;
		repaint( FALSE );
	    }
	    if ( inherits("QPushButton") )
		emit pressed();
	    else
		e->ignore();
	}
	break;
    case Key_Up:
    case Key_Left:
#ifndef QT_NO_BUTTONGROUP
	if ( group() )
	    group()->moveFocus( e->key() );
	else
#endif
	    focusNextPrevChild( FALSE );
	break;
    case Key_Right:
    case Key_Down:
#ifndef QT_NO_BUTTONGROUP
	if ( group() )
	    group()->moveFocus( e->key() );
	else
#endif
	    focusNextPrevChild( TRUE );
	break;
    case Key_Escape:
	if ( buttonDown ) {
	    buttonDown = FALSE;
	    update();
	    break;
	}
	// fall through
    default:
	e->ignore();
    }
}

/*!
  \reimp
 */
void QButton::keyReleaseEvent( QKeyEvent * e)
{
    got_a_release = TRUE;
    switch ( e->key() ) {
    case Key_Space:
	if ( buttonDown && !e->isAutoRepeat() ) {
	    buttonDown = FALSE;
	    nextState();
	    emit released();
	    emit clicked();
	}
	break;
    default:
	e->ignore();
    }
}



/*! \reimp */

bool QButton::focusNextPrevChild( bool next )
{
    // we do not want this any more
    return QWidget::focusNextPrevChild( next );
}


/*!\reimp
*/

void QButton::mousePressEvent( QMouseEvent *e )
{
    if ( e->button() != LeftButton )
	return;
    bool hit = hitButton( e->pos() );
    if ( hit ) {				// mouse press on button
	mlbDown = TRUE;				// left mouse button down
	buttonDown = TRUE;
    if ( autoMask() )
        updateMask();

	repaint( FALSE );
	emit pressed();
	if ( repeat )
	    timer()->start( autoRepeatDelay, TRUE );
    }
}

/*!\reimp
*/

void QButton::mouseReleaseEvent( QMouseEvent *e)
{
    if ( e->button() != LeftButton || !mlbDown )
	return;
    if ( d )
	timer()->stop();
    mlbDown = FALSE;				// left mouse button up
    buttonDown = FALSE;
    if ( hitButton( e->pos() ) ) {		// mouse release on button
    nextState();
        emit released();
    emit clicked();
    } else {
	    repaint( FALSE );
	    emit released();
    }
}

/*!\reimp
*/

void QButton::mouseMoveEvent( QMouseEvent *e )
{
    if ( !((e->state() & LeftButton) && mlbDown) )
	return;					// left mouse button is up
    if ( hitButton( e->pos() ) ) {		// mouse move in button
	if ( !buttonDown ) {
	    buttonDown = TRUE;
	    repaint( FALSE );
	    emit pressed();
	}
    } else {					// mouse move outside button
	if ( buttonDown ) {
	    buttonDown = FALSE;
	    repaint( FALSE );
	    emit released();
	}
    }
}


/*!
  Handles paint events for buttons.  Small and typically complex
  buttons (less than 300x100 pixels) are painted double-buffered to
  reduce flicker. The actually drawing is done in the virtual functions
  drawButton() and drawButtonLabel().

  \sa drawButton(), drawButtonLabel()
*/
void QButton::paintEvent( QPaintEvent *event )
{
    if ( event &&
	 width() <= drawingPixWidth &&
	 height() <= drawingPixHeight &&
	 backgroundMode() != X11ParentRelative ) {
	makeDrawingPixmap(); // makes file-static drawpm variable
	if ( backgroundOrigin() == ParentOrigin && !isTopLevel() )
	    drawpm->fill( this, x(), y() );
	else
	    drawpm->fill( this, 0, 0 );
	QPainter paint;
	paint.begin( drawpm, this );
	drawButton( &paint );
	paint.end();

	paint.begin( this );
	paint.drawPixmap( 0, 0, *drawpm );
	paint.end();
    } else {
	erase( event->region() );
	QPainter paint( this );
	drawButton( &paint );
    }
}

/*!\reimp
*/

void QButton::focusInEvent( QFocusEvent * e)
{
    QWidget::focusInEvent( e );
}

/*!\reimp
*/

void QButton::focusOutEvent( QFocusEvent * e )
{
    buttonDown = FALSE;
    QWidget::focusOutEvent( e );
}


/*!
  Internal slot used for auto repeat.
*/

void QButton::autoRepeatTimeout()
{
    if ( mlbDown && isEnabled() && autoRepeat() ) {
	if ( buttonDown ) {
	    emit released();
	    emit clicked();
	    emit pressed();
	}
	timer()->start( autoRepeatPeriod, TRUE );
    }
}


/*!
  Internal slot used for the second stage of animateClick().
*/

void QButton::animateTimeout()
{
    if ( !animation )
	return;
    animation  = FALSE;
    buttonDown = FALSE;
    nextState();
    emit released();
    emit clicked();
}


void QButton::nextState()
{
    bool t = isToggleButton() && !( isOn() && isExclusiveToggle() );
    bool was = stat != Off;
    if ( t ) {
	if ( toggleTyp == Tristate )
	    stat = ( stat + 1 ) % 3;
	else
	    stat = stat ? Off : On;
    }
    if ( autoMask() )
        updateMask();
    repaint( FALSE );
    if ( t ) {
	if ( was != (stat != Off) )
	    emit toggled( stat != Off );
	emit stateChanged( stat );
    }
}


/*! \reimp */

void QButton::enabledChange( bool e )
{
    if ( !e )
	setDown( FALSE );
    QWidget::enabledChange( e );
}


/*!  if this is a toggle button, toggles it. */

void QButton::toggle()
{
    if ( isToggleButton() )
	 setOn( !isOn() );
}

/*!
  Sets the type of toggling behavior.  The default is \a SingleShot.

  Subclasses use this, and present it with a more comfortable interface.
*/
void QButton::setToggleType( ToggleType type )
{
    toggleTyp = type;
    if ( type != Tristate && stat == NoChange )
	setState( On );
}



/*!
  Returns TRUE if this button behaves exclusively inside a QButtonGroup.
  In that case, this button can only be toggled off by another button
  being toggled on.
*/

bool QButton::isExclusiveToggle() const
{
#ifndef QT_NO_BUTTONGROUP
    return group() && ( group()->isExclusive() ||
			group()->isRadioButtonExclusive() &&
			inherits( "QRadioButton" ) );
#else
    return FALSE;
#endif
}
#endif
