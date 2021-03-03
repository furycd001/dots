/****************************************************************************
** $Id: qt/src/widgets/qspinbox.cpp   2.3.2   edited 2001-10-22 $
**
** Implementation of QSpinBox widget class
**
** Created : 1997
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

#include "qspinbox.h"
#ifndef QT_NO_SPINBOX

#include "qspinbox.h"
#include "qpushbutton.h"
#include "qpainter.h"
#include "qbitmap.h"
#include "qlineedit.h"
#include "qvalidator.h"
#include "qpixmapcache.h"
#include "qapplication.h"

struct QSpinBoxPrivate
{
    QSpinBoxPrivate(): buttonSymbols( QSpinBox::UpDownArrows ) {}

    QSpinBox::ButtonSymbols buttonSymbols;
};


// REVISED: warwick
/*!
  \class QSpinBox qspinbox.h

  \brief The QSpinBox class provides a spin box widget, sometimes called
	  up-down widget, little arrows widget or spin button.

  \ingroup basic

  QSpinBox allows the user to choose a value, either by
  clicking the up/down buttons to increase/decrease the value
  currently displayed, or by typing the value directly into the spin
  box. Usually the value is an integer.

  Every time the value changes, QSpinBox emits the valueChanged()
  signal.  The current value can be fetched with value() and set with
  setValue().

  The spin box clamps the value within a numeric range, see
  QRangeControl for details.  Clicking the up/down down buttons (or
  using the keyboard accelerators: Up-arrow and Down-arrow) will
  increase or decrease the current value in steps of size lineStep().

  Most spin boxes are directional, but QSpinBox can also operate as a
  circular spin box, i.e. if the range is 0-99 and the current value
  is 99, clicking Up will give 0.  Use setWrapping() if you want
  circular behavior.

  The displayed value can be prepended and/or appended with an
  arbitrary string indicating for example the unit of measurement.
  See setPrefix() and setSuffix().

  Normally, the spin box displays up and down arrows in the buttons.
  You can use setButtonSymbols() to change the display to
  show + and - symbols, if this is clearer for your intended purpose.
  In either case, the up and down arrow keys always work.

  It is often desirable to give the user a special, often default,
  choice in addition to the range of numeric values.  See
  setSpecialValueText() for how to do this with QSpinBox.

  The default \l QWidget::focusPolicy() is StrongFocus.

  QSpinBox can easily be subclassed to allow the user to input other
  things than an integer value, as long as the allowed input can be
  mapped down to a range of integers.  This can be done by overriding
  the virtual functions mapValueToText() and mapTextToValue() and
  setting another, suitable validator using setValidator(). For example,
  these function could be changed so that the user provided values
  from 0.0 to 10.0 while the range of integers used inside the program
  would be 0 to 100:

  \code
  class MySpinBox : public QSpinBox {
  public:
    ...

    QString	mapValueToText( int value )
    {
      return QString("%1.%2").arg(value/10).arg(value%10);
    }

    int		mapTextToValue( bool* ok )
    {
      return int(text().toFloat()*10);
    }

  };
  \endcode

  <img src=qspinbox-m.png> <img src=qspinbox-w.png>

  \sa QScrollBar QSlider
  <a href="guibooks.html#fowler">GUI Design Handbook: Spin Box</a>
*/


/*!
  Constructs a spin box with the default QRangeControl range and step
  values.

  \sa minValue(), maxValue(), setRange(), lineStep(), setSteps()
*/

QSpinBox::QSpinBox( QWidget * parent , const char *name )
    : QFrame( parent, name ),
      QRangeControl()
{
    initSpinBox();
}


/*!
  Constructs a spin box with range from \a minValue to \a maxValue
  inclusive, with step amount \a step.  The value is initially
  set to \a minValue.

  \sa minValue(), maxValue(), setRange(), lineStep(), setSteps()
*/

QSpinBox::QSpinBox( int minValue, int maxValue, int step, QWidget* parent,
		    const char* name )
    : QFrame( parent, name ),
      QRangeControl( minValue, maxValue, step, step, minValue )
{
    initSpinBox();
}

/*!
  \internal Initialization.
*/

void QSpinBox::initSpinBox()
{
    d = 0;
    wrap = FALSE;
    edited = FALSE;

    up = new QPushButton( this, "up" );
    up->setFocusPolicy( QWidget::NoFocus );
    up->setAutoDefault( FALSE );
    up->setAutoRepeat( TRUE );

    down = new QPushButton( this, "down" );
    down->setFocusPolicy( QWidget::NoFocus );
    down->setAutoDefault( FALSE );
    down->setAutoRepeat( TRUE );

    validate = new QIntValidator( minValue(), maxValue(), this, "validator" );
    vi = new QLineEdit( this, "line editor" );
    vi->setFrame( FALSE );
    setFocusProxy( vi );
    setFocusPolicy( StrongFocus );
    vi->setValidator( validate );
    vi->installEventFilter( this );

    if ( style() == WindowsStyle )
	setFrameStyle( WinPanel | Sunken );
    else
	setFrameStyle( Panel | Sunken );
    setLineWidth( 2 );

    setPalettePropagation( AllChildren );
    setFontPropagation( AllChildren );

    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
    updateDisplay();

    connect( up, SIGNAL(pressed()), SLOT(stepUp()) );
    connect( down, SIGNAL(pressed()), SLOT(stepDown()) );
    connect( vi, SIGNAL(textChanged(const QString&)), SLOT(textChanged()) );
}

/*!
  Destroys the spin box, freeing all memory and other resources.
*/

QSpinBox::~QSpinBox()
{
    delete d;
}


/*!
  Returns the current text of the spin box, including any prefix() and suffix().

  \sa value()
*/

QString QSpinBox::text() const
{
    return vi->text();
}



/*!
  Returns a copy of the current text of the spin box with any prefix
  and/or suffix and white space at the start and end removed.

  \sa text(), setPrefix(), setSuffix()
*/

QString QSpinBox::cleanText() const
{
    QString s = QString(text()).stripWhiteSpace();
    if ( !prefix().isEmpty() ) {
	QString px = QString(prefix()).stripWhiteSpace();
	int len = px.length();
	if ( len && s.left(len) == px )  // Remove _only_ if it is the prefix
	    s.remove( 0, len );
    }
    if ( !suffix().isEmpty() ) {
	QString sx = QString(suffix()).stripWhiteSpace();
	int len = sx.length();
	if ( len && s.right(len) == sx )  // Remove _only_ if it is the suffix
	    s.truncate( s.length() - len );
    }
    return s.stripWhiteSpace();
}


/*!
  Sets the special-value text to \a text.  If set, the spin box will
  display this text instead of a numeric value whenever the current
  value is equal to minVal().  Typically used for indicating that this
  choice has a special (default) meaning.

  For example, if your spin box allows the user to choose the
  margin width in a print dialog, and your application is able to
  automatically choose a good margin width, you can set up the spin
  box like this:
  \code
    QSpinBox marginBox( -1, 20, 1, parent, "marginBox" );
    marginBox->setSuffix( " mm" );
    marginBox->setSpecialValueText( "Auto" );
  \endcode
  The user will then be able to choose a margin width from 0-20
  millimeters, or select "Auto" to leave it to the application to
  choose.  Your code must then interpret the spin box value of -1 as
  the user requesting automatic margin width.

  Neither \link setPrefix prefix\endlink nor \link setSuffix
  suffix,\endlink if set, are added to the special-value text when
  displayed.

  To turn off the special-value text display, call this function with
  an empty string as parameter. The default is no special-value text,
  i.e. the numeric value is shown as usual.

  \sa specialValueText()
*/

void QSpinBox::setSpecialValueText( const QString &text )
{
    specText = text;
    updateDisplay();
}


/*!
  Returns the currently special-value text, or a null string if no
  special-value text is currently set.

  \sa setSpecialValueText()
*/

QString QSpinBox::specialValueText() const
{
    if ( specText.isEmpty() )
	return QString::null;
    else
	return specText;
}


/*!
  Sets the prefix to \a text.  The prefix is prepended to the start of
  the displayed value.  Typical use is to indicate the unit of
  measurement to the user. eg.

  \code
    sb->setPrefix("$");
  \endcode

  To turn off the prefix display, call this function with an empty
  string as parameter.  The default is no prefix.

  \sa prefix(), setSuffix(), suffix()
*/

void QSpinBox::setPrefix( const QString &text )
{
    pfix = text;
    updateDisplay();
}


/*!
  Sets the suffix to \a text.  The suffix is appended to the end of the
  displayed value.  Typical use is to indicate the unit of measurement
  to the user. eg.

  \code
    sb->setSuffix("cm");
  \endcode

  To turn off the suffix display, call this function with an empty
  string as parameter.  The default is no suffix.

  \sa suffix(), setPrefix(), prefix()
*/

void QSpinBox::setSuffix( const QString &text )
{
    sfix = text;
    updateDisplay();
}


/*!
  Returns the currently set prefix, or a null string if no prefix is
  set.

  \sa setPrefix(), setSuffix(), suffix()
*/

QString QSpinBox::prefix() const
{
    if ( pfix.isEmpty() )
	return QString::null;
    else
	return pfix;
}


/*!
  Returns the currently set suffix, or a null string if no suffix is
  set.

  \sa setSuffix(), setPrefix(), suffix()
*/

QString QSpinBox::suffix() const
{
    if ( sfix.isEmpty() )
	return QString::null;
    else
	return sfix;
}


/*!
  Setting wrapping to TRUE will allow the value to be stepped from the
  highest value to the lowest, and vice versa.  By default, wrapping is
  turned off.

  \sa wrapping(), minValue(), maxValue(), setRange()
*/

void QSpinBox::setWrapping( bool on )
{
    wrap = on;
    updateDisplay();
}


/*!
  Returns the current setWrapping() value.
*/

bool QSpinBox::wrapping() const
{
    return wrap;
}



/*!\reimp
*/
QSize QSpinBox::sizeHint() const
{
    constPolish();
    QFontMetrics fm = fontMetrics();
    int h = fm.height();
    if ( h < 12 ) 	// ensure enough space for the button pixmaps
	h = 12;
    int w = 35; 	// minimum width for the value
    int wx = fm.width( ' ' )*2;
    QString s;
    s = prefix() + ( (QSpinBox*)this )->mapValueToText( minValue() ) + suffix();
    w = QMAX( w, fm.width( s ) + wx);
    s = prefix() + ( (QSpinBox*)this )->mapValueToText( maxValue() ) + suffix();
    w = QMAX(w, fm.width( s ) + wx );
    if ( !specialValueText().isEmpty() ) {
	s = specialValueText();
	w = QMAX( w, fm.width( s ) + wx );
    }
    QSize r( h // buttons AND frame both sides - see resizeevent()
	     + 6 // right/left margins
	     + w, // widest value
	     frameWidth() * 2 // top/bottom frame
	     + 4 // top/bottom margins
	     + h // font height
	     );
    return r.expandedTo( QApplication::globalStrut() );
}


// Does the layout of the lineedit and the buttons

void QSpinBox::arrangeWidgets()
{
    if ( !up || !down ) // may happen if the application has a pointer error
	return;

    QSize bs; // no, it's short for 'button size'
    if ( style() == WindowsStyle )
	bs.setHeight( height()/2 - frameWidth() );
    else
	bs.setHeight( height()/2 );
    if ( bs.height() < 8 )
	bs.setHeight( 8 );
    bs.setWidth( bs.height() * 8 / 5 ); // 1.6 - approximate golden mean
    if ( style() == WindowsStyle )
	setFrameRect( QRect( 0, 0, 0, 0 ) );
    else
	setFrameRect( QRect( 0, 0, width() - bs.width(), height() ) );

    if ( up->size() != bs || down->size() != bs ) {
	up->resize( bs );
	down->resize( bs );
	updateButtonSymbols();
    }

    int y = style() == WindowsStyle ? frameWidth() : 0;
    int x = width() - y - bs.width();
    up->move( x, y );
    down->move( x, height() - y - up->height() );
    if ( style() == WindowsStyle )
	vi->setGeometry( frameWidth(), frameWidth(),
			 x - frameWidth(), height() - 2*frameWidth() );
    else
	vi->setGeometry( contentsRect() );
}

/*!\reimp
*/
QSizePolicy QSpinBox::sizePolicy() const
{
    //### removeme 3.0
    return QWidget::sizePolicy();
}


/*!
  Sets the current value of the spin box to \a value.  This is
  QRangeControl::setValue() made available as a slot.
*/

void QSpinBox::setValue( int value )
{
    QRangeControl::setValue( value );
}


/*!
  Increases the current value one step, wrapping as necessary.  This is
  the same as clicking on the pointing-up button, and can be used for
  e.g.  keyboard accelerators.

  \sa stepDown(), addLine(), lineStep(), setSteps(), setValue(), value()
*/

void QSpinBox::stepUp()
{
    if ( edited )
	interpretText();
    if ( wrapping() && ( value()+lineStep() > maxValue() ) )
	setValue( minValue() );
    else
	addLine();
}


/*!
  Decreases the current value one step, wrapping as necessary.  This is
  the same as clicking on the pointing-down button, and can be used
  for e.g.  keyboard accelerators.

  \sa stepUp(), subtractLine(), lineStep(), setSteps(), setValue(), value()
*/

void QSpinBox::stepDown()
{
    if ( edited )
	interpretText();
    if ( wrapping() && ( value()-lineStep() < minValue() ) )
	setValue( maxValue() );
    else
	subtractLine();
}


/*!
  \fn void QSpinBox::valueChanged( int value )

  This signal is emitted every time the value of the spin box changes
  (whatever the cause - by setValue(), by a keyboard accelerator, by
  mouse clicks etc.).

  Note that it is emitted \e every time, not just for the "final" step
  - if the user clicks 'up' three times, this signal is emitted three
  times.

  \sa value()
*/


/*!
  \fn void QSpinBox::valueChanged( const QString& valueText )

  This signal is emitted whenever the valueChanged( int ) signal is
  emitted, i.e.  every time the value of the spin box changes (whatever
  the cause - by setValue(), by a keyboard accelerator, by mouse
  clicks etc.).

  The \a valueText parameter is the same string that is
  displayed in the edit field of the spin box.

  \sa value()
*/



/*!
  Intercepts and handles those events coming to the embedded QLineEdit
  which have special meaning for the QSpinBox.
*/

bool QSpinBox::eventFilter( QObject* obj, QEvent* ev )
{
    if ( obj != vi )
	return FALSE;

    if ( ev->type() == QEvent::FocusOut || ev->type() == QEvent::Leave || ev->type() == QEvent::Hide ) {
	if ( edited )
	    interpretText();
    } else if ( ev->type() == QEvent::KeyPress ) {
	QKeyEvent* k = (QKeyEvent*)ev;
	if ( k->key() == Key_Up ) {
	    stepUp();
	    return TRUE;
	} else if ( k->key() == Key_Down ) {
	    stepDown();
	    return TRUE;
	} else if ( k->key() == Key_Return ) {
	    interpretText();
	    return FALSE;
	}
    }
    return FALSE;
}


/*!\reimp
*/
void QSpinBox::leaveEvent( QEvent* )
{
    if ( edited )
	interpretText();
}


/*!\reimp
*/
void QSpinBox::resizeEvent( QResizeEvent* )
{
    arrangeWidgets();
}

/*!\reimp
*/
void QSpinBox::wheelEvent( QWheelEvent * e )
{
    e->accept();
    static float offset = 0;
    static QSpinBox* offset_owner = 0;
    if (offset_owner != this) {
	offset_owner = this;
	offset = 0;
    }
    offset += -e->delta()/120;
    if (QABS(offset) < 1)
	return;
    int ioff = int(offset);
    int i;
    for (i=0; i<QABS(ioff); i++)
	offset > 0 ? stepDown() : stepUp();
    offset -= ioff;
}



/*!
  This method gets called by QRangeControl whenever the value has changed.
  Updates the display and emits the valueChanged() signals.
*/

void QSpinBox::valueChange()
{
    updateDisplay();
    emit valueChanged( value() );
    emit valueChanged( currentValueText() );
}


/*!
  This method gets called by QRangeControl whenever the range has
  changed.  It adjusts the default validator and updates the display.
*/

void QSpinBox::rangeChange()
{
#if QT_VERSION >= 300
#error "Use a virtual function."
#endif
    if ( validate->inherits( "QIntValidator" ) )
	((QIntValidator*)validate)->setRange( minValue(), maxValue() );
    updateDisplay();
}


/*!
  Sets the validator to \a v.  The validator controls what keyboard
  input is accepted when the user is editing in the value field.  The
  default is to use a suitable QIntValidator.

  Use setValidator(0) to turn off input validation (entered input will
  still be clamped to the range of the spinbox).
*/

void QSpinBox::setValidator( const QValidator* v )
{
    if ( vi )
	vi->setValidator( v );
}


/*!
  Returns the validator which constrains editing for this spin box if
  there is any, or else 0.

  \sa setValidator() QValidator
*/

const QValidator * QSpinBox::validator() const
{
    return vi ? vi->validator() : 0;
}

/*!
  Updates the contents of the embedded QLineEdit to reflect current
  value, using mapValueToText().  Also enables/disables the push
  buttons accordingly.

  \sa mapValueToText()
*/

void QSpinBox::updateDisplay()
{
    vi->setText( currentValueText() );
    edited = FALSE;
    up->setEnabled( isEnabled() && (wrapping() || value() < maxValue()) );
    down->setEnabled( isEnabled() && (wrapping() || value() > minValue()) );
}


/*!
  QSpinBox calls this after the user has manually edited the contents
  of the spin box (not using the up/down buttons/keys).

  The default implementation of this function interprets the new text
  using mapTextToValue().  If mapTextToValue() is successful, it
  changes the spin box' value.  If not the value if left unchanged.
*/

void QSpinBox::interpretText()
{
    bool ok = TRUE;
    bool done = FALSE;
    int newVal = 0;
    if ( !specialValueText().isEmpty() ) {
	QString s = QString(text()).stripWhiteSpace();
	QString t = QString(specialValueText()).stripWhiteSpace();
	if ( s == t ) {
	    newVal = minValue();
	    done = TRUE;
	}
    }
    if ( !done )
	newVal = mapTextToValue( &ok );
    if ( ok )
	setValue( newVal );
    updateDisplay();		// Sometimes redundant
}


/*!
  Returns a pointer to the embedded 'up' button.
*/

QPushButton* QSpinBox::upButton() const
{
    return up;
}


/*!
  Returns a pointer to the embedded 'down' button.
*/

QPushButton* QSpinBox::downButton() const
{
    return down;
}


/*!
  Returns a pointer to the embedded QLineEdit.
*/

QLineEdit* QSpinBox::editor() const
{
    return vi;
}


/*!
  This slot gets called whenever the user edits the text of the spin box.
*/

void QSpinBox::textChanged()
{
    edited = TRUE;	// This flag is cleared in updateDisplay()
};




/*!
  This virtual function is used by the spin box whenever it needs to
  display value \a v.  The default implementation returns a string
  containing \a v printed in the standard way.

  Reimplement this function in in a subclass if you want a specialized
  spin box, handling something else than integers.  This function need
  not be concerned with \link setPrefix() prefix \endlink or \link
  setSuffix() suffix \endlink or \link setSpecialValueText()
  special-value text, \endlink the QSpinBox handles that
  automatically.

  \sa updateDisplay(), mapTextToValue()
*/

QString QSpinBox::mapValueToText( int v )
{
    QString s;
    s.setNum( v );
    return s;
}


/*!
  This virtual function is used by the spin box whenever it needs to
  interpret the text entered by the user as a value.  The default
  implementation tries to interpret it as an integer in the standard
  way, and returns the integer value.

  Reimplement this function in in a subclass if you want a specialized
  spin box, handling something else than integers.  It should call
  text() (or cleanText() ) and return the value corresponding to that
  text.  If the text does not represent a legal value
  (uninterpretable), the bool pointed to by \a ok should be set to
  FALSE.

  This function need not be concerned with \link setSpecialValueText()
  special-value text, \endlink the QSpinBox handles that
  automatically.

  \sa interpretText(), mapValueToText()
*/

int QSpinBox::mapTextToValue( bool* ok )
{
    QString s = text();
    int newVal = s.toInt( ok );
    if ( !(*ok) && !( !prefix() && !suffix() ) ) {// Try removing any pre/suffix
	s = cleanText();
	newVal = s.toInt( ok );
    }
    return newVal;
}


/*!
  Returns the full text calculated from the current value, including any
  prefix, suffix or special-value text.
*/

QString QSpinBox::currentValueText()
{
    QString s;
    if ( (value() == minValue()) && !specialValueText().isEmpty() ) {
	s = specialValueText();
    } else {
	s = prefix();
	s.append( mapValueToText( value() ) );
	s.append( suffix() );
    }
    return s;
}



/*!
  \reimp
*/

void QSpinBox::setEnabled( bool on )
{
    bool b = isEnabled();
    QFrame::setEnabled( on );
    if ( isEnabled() != b ) {
	// ## enabledChange() might have been a better choice
	updateDisplay();
    }
}



/*! \reimp */

void QSpinBox::styleChange( QStyle& old )
{
    if ( style() == WindowsStyle )
	setFrameStyle( WinPanel | Sunken );
    else
	setFrameStyle( Panel | Sunken );
    arrangeWidgets();
    QWidget::styleChange( old );
}


/*! \enum QSpinBox::ButtonSymbols
  This enum type determines what the buttons in a spin box show.  The
  currently defined values are: <ul>

  <li> \c UpDownArrows - the buttons show little arrows, in the
  classic style.  This is the default.

  <li> \c PlusMinus - the buttons show '+' and '-' symbols.  This is
  often considered to be more meaningful than \c UpDownArrows.

  </ul>
*/

/*!  Sets the spin box to display \a newSymbols on its buttons.  \a
  newSymbols can be either \c UpDownArrows (the default) or \c PlusMinus.

  \sa buttonSymbols() ButtonSymbols
*/

void QSpinBox::setButtonSymbols( ButtonSymbols newSymbols )
{
    if ( buttonSymbols() == newSymbols )
	return;

    if ( !d )
	d = new QSpinBoxPrivate;
    d->buttonSymbols = newSymbols;
    updateButtonSymbols();
}


/*!  Returns the current button symbol mode.  The default is \c
  UpDownArrows.

  \sa setButtonSymbols() ButtonSymbols
*/

QSpinBox::ButtonSymbols QSpinBox::buttonSymbols() const
{
    return d ? d->buttonSymbols : UpDownArrows;
}



// this function uses the pixmap cache for a Different Reason: the
// pixmap cache also preserves QPixmap::serialNumber().  by doing
// this, QButton::setPixmap() is able to avoid flicker e.g. when the
// spin box is resized in such a way that the height of the buttons
// does not change (common the default size policy).

void QSpinBox::updateButtonSymbols()
{
    QString key( QString::fromLatin1( "$qt$qspinbox$" ) );
    bool pmSym = buttonSymbols() == PlusMinus;
    key += QString::fromLatin1( pmSym ? "+-" : "^v" );
    key += QString::number( down->height() );
    QString upKey = key + QString::fromLatin1( "$up" );
    QString dnKey = key + QString::fromLatin1( "$down" );
    QBitmap upBm;
    QBitmap dnBm;

    bool found = QPixmapCache::find( dnKey, dnBm )
		 && QPixmapCache::find( upKey, upBm );

    if ( !found ) {
	QPainter p;
	if ( pmSym ) {
	    int h = down->height()-4;
	    if ( h < 3 )
		return;
	    else if ( h == 4 )
		h = 3;
	    else if ( (h > 6) && (h & 1) )
		h--;
	    h -= ( h / 8 ) * 2;		// Empty border
	    dnBm.resize( h, h );
	    p.begin( &dnBm );
	    p.eraseRect( 0, 0, h, h );
	    p.setBrush( color1 );
	    int c = h/2;
	    p.drawLine( 0, c, h, c );
	    if ( !(h & 1) )
		p.drawLine( 0, c-1, h, c-1 );
	    p.end();
	    upBm = dnBm;
	    p.begin( &upBm );
	    p.drawLine( c, 0, c, h );
	    if ( !(h & 1) )
		p.drawLine( c-1, 0, c-1, h );
	    p.end();
	}
	else {
	    int w = down->width()-4;
	    if ( w < 3 )
		return;
	    else if ( !(w & 1) )
		w--;
	    w -= ( w / 7 ) * 2;		// Empty border
	    int h = w/2 + 2;        // Must have empty row at foot of arrow
	    dnBm.resize( w, h );
	    p.begin( &dnBm );
	    p.eraseRect( 0, 0, w, h );
	    QPointArray a;
	    a.setPoints( 3,  0, 1,  w-1, 1,  h-2, h-1 );
	    p.setBrush( color1 );
	    p.drawPolygon( a );
	    p.end();
#ifndef QT_NO_TRANSFORMATIONS
	    QWMatrix wm;
	    wm.scale( 1, -1 );
	    upBm = dnBm.xForm( wm );
#else
	    upBm.resize( w, h );
	    p.begin( &upBm );
	    p.eraseRect( 0, 0, w, h );
	    a.setPoints( 3,  0, h-2,  w-1, h-2,  h-2, 0 );
	    p.setBrush( color1 );
	    p.drawPolygon( a );
	    p.end();
#endif
	}
	QPixmapCache::insert( dnKey, dnBm );
	QPixmapCache::insert( upKey, upBm );
    }
    down->setPixmap( dnBm );
    up->setPixmap( upBm );
}

/*!
  \reimp
*/
int QSpinBox::minValue() const
{
    return QRangeControl::minValue();
}

/*!
  \reimp
*/
int QSpinBox::maxValue() const
{
    return QRangeControl::maxValue();
}

/*!
  A convenience function which just calls
  setRange( i, maxValue() )

  \sa setRange()
*/
void QSpinBox::setMinValue( int i )
{
    setRange( i, maxValue() );
}

/*!
  A convenience function which just calls
  setRange( minValue(), i )

  \sa setRange()
*/
void QSpinBox::setMaxValue( int i )
{
    setRange( minValue(), i );
}

/*!
  \reimp
*/
int QSpinBox::lineStep() const
{
    return QRangeControl::lineStep();
}

/*!
  Sets the line step to \e i.

  Calls the virtual stepChange() function if the new line step is
  different from the previous setting.

  \sa lineStep() QRangeControl::setSteps() setRange()
*/
void QSpinBox::setLineStep( int i )
{
    setSteps( i, pageStep() );
}

/*!
  \reimp
*/
int QSpinBox::value() const
{
    if ( edited ) {
	QSpinBox *that = (QSpinBox*)this;
	that->edited = FALSE;
	that->interpretText();
    }
    return QRangeControl::value();
}
#endif
