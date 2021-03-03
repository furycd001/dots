/****************************************************************************
** $Id: qt/src/widgets/qvalidator.cpp   2.3.2   edited 2001-01-26 $
**
** Implementation of validator classes.
**
** Created : 970610
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

#include "qvalidator.h"
#ifndef QT_NO_VALIDATOR
#include "qwidget.h"
#include "qregexp.h"

#include <math.h> // HUGE_VAL
#include <limits.h> // *_MIN, *_MAX
#include <ctype.h> // isdigit

// NOT REVISED
/*!
  \class QValidator qvalidator.h

  \brief The QValidator class provides validation of input text.

  \ingroup misc

  The class itself is abstract; two subclasses provide rudimentary
  numeric range checking.

  The class includes two virtual functions, validate() and fixup().

  validate() is pure virtual, so it must be implemented by every
  subclass.  It returns \c Invalid, \c Intermediate or \c Acceptable
  depending on whether its argument is valid (for the class'
  definition of valid).

  The three states require some explanation.  An \c Invalid string is
  \e clearly invalid.  \c Intermediate is less obvious - the concept
  of validity is slippery when the string is incomplete (still being
  edited).  QValidator defines \c Intermediate as the property of a
  string that it is neither clearly invalid or acceptable as a final
  result.  \c Acceptable means that the string is acceptable as a
  final result.  One might say that any string that is a plausible
  intermediate state during entry of an \c Acceptable string is \c
  Intermediate.

  Here are some examples:
  <ol>

  <li>For a line edit that accepts integers from 0 to 999 inclusive,
  42 and 666 are \c Acceptable, the empty string and 1114 are \c
  Intermediate and asdf is \c Invalid.

  <li>For an editable combo box that accepts URLs, any well-formed URL
  is \c Acceptable, "http://www.trolltech.com/," is \c Intermediate (it can
  be a cut-and-paste job that accidentally took in a comma at the
  end), the empty string is valid (the user might select and delete
  all of the text in preparation to entering a new URL), and
  "http:///./" is \c Invalid.

  <li>For a spin box that accepts lengths, "11cm" and "1in" are \c
  Acceptable, "11" and the empty string are \c Intermediate, and
  "http://www.trolltech.com" and "hour" are \c Invalid.

  </ol>

  fixup() is provided for validators that can repair some or all user
  errors.  The default does nothing.  QLineEdit, for example, will
  call fixup() if the user presses Return and the content is not
  currently valid, in case fixup() can do magic.  This allows some \c
  Invalid strings to be made \c Acceptable, too, spoiling the muddy
  definition above even more.

  QValidator is generally used with QLineEdit, QSpinBox and QComboBox.
*/


/*! \enum QValidator::State

  This enum type defines the states in which a validated string can
  be.  There are currently three states: <ul>

  <li> \c Invalid - the string is \e clearly invalid.

  <li> \c Intermediate - the string is a plausible intermediate value
  during editing.

  <li> \c Acceptable - acceptable as a final result.

  </ul>

  The state \c Valid has been renamed \c Intermediate.  The old name
  confused too many people and is now obsolete.
*/


/*!
  Sets up the internal data structures used by the validator.  At
  the moment there aren't any.
*/

QValidator::QValidator( QWidget * parent, const char *name )
    : QObject( parent, name )
{
}


/*!
  Destroys the validator, freeing any storage and other resources
  used.
*/

QValidator::~QValidator()
{
}


/*!
  \fn QValidator::State QValidator::validate( QString& input, int& pos ) const

  This pure virtual function returns \c Invalid if \a input is invalid
  according to this validator's rules, \c Intermediate if it is likely that a
  little more editing will make the input acceptable (e.g. the user
  types '4' into a widget which accepts 10-99) and \c Acceptable if
  the input is completely acceptable.

  The function can change \a input and \a pos (the cursor position) if
  it wants to.
*/


/*!
  \fn void QValidator::fixup( QString & input ) const

  Attempts to change \a input to be valid according to this validator's
  rules.  Need not result in a valid string - callers of this function
  must re-test afterwards.  The default does nothing.

  Reimplementations of this function can change \a input even if they
  do not produce a valid string.  For example an ISBN validator might
  want to delete every character except digits and "-", even if the
  result is not a valid ISBN, and a last-name validator might want to
  remove white space from the start and end of the string, even if the
  resulting string is not in the list of known last names.
*/

void QValidator::fixup( QString & ) const
{
}


/*!
  \class QIntValidator qvalidator.h

  \brief The QIntValidator class provides range checking of integers.

  \ingroup misc

  QIntValidator provides a lower and an upper bound.  It does not
  provide a fixup() function.

  \sa QDoubleValidator
*/


/*!
  Constructs a validator object which accepts all integers.
*/

QIntValidator::QIntValidator( QWidget * parent, const char *name )
    : QValidator( parent, name )
{
    b = INT_MIN;
    t = INT_MAX;
}


/*!
  Constructs a validator object which accepts all integers from \a
  bottom up to and including \a top.
*/

QIntValidator::QIntValidator( int bottom, int top,
			      QWidget * parent, const char* name )
    : QValidator( parent, name )
{
    b = bottom;
    t = top;
}


/*!
  Destroys the validator, freeing any storage and other resources
  used.
*/

QIntValidator::~QIntValidator()
{
    // nothing
}


/*!  Returns \a Acceptable if \a input contains a number in the legal
  range, \a Intermediate if it contains another integer or is empty,
  and \a Invalid if \a input is not an integer.
*/

QValidator::State QIntValidator::validate( QString & input, int & ) const
{
    QRegExp empty( QString::fromLatin1("^ *-? *$") );
    if ( empty.match( input ) >= 0 )
	return QValidator::Intermediate;
    bool ok;
    long int tmp = input.toLong( &ok );
    if ( !ok )
	return QValidator::Invalid;
    else if ( tmp < b || tmp > t )
	return QValidator::Intermediate;
    else
	return QValidator::Acceptable;
}


/*!
  Sets the validator to accept only number from \a bottom up to an
  including \a top.
*/

void QIntValidator::setRange( int bottom, int top )
{
    b = bottom;
    t = top;
}

/*!
  Sets the validator to accept no numbers smaller than \a bottom.

  \sa setRange()
*/
void QIntValidator::setBottom( int bottom )
{
    setRange( bottom, top() );
}

/*!
  Sets the validator to accept no numbers bigger than \a top.

  \sa setRange()
*/
void QIntValidator::setTop( int top )
{
    setRange( bottom(), top );
}

/*!
  \fn int QIntValidator::bottom() const

  Returns the lowest valid number according to this validator.

  \sa top() setRange()
*/


/*!
  \fn int QIntValidator::top() const

  Returns the highest valid number according to this validator.

  \sa bottom() setRange()
*/


/*!
  \class QDoubleValidator qvalidator.h

  \brief The QDoubleValidator class provides range checking of
  floating-point numbers.

  \ingroup misc

  QDoubleValidator provides an upper bound, a lower bound, and a limit
  on the number of digits after the decimal point.  It does not
  provide a fixup() function.

  \sa QIntValidator
*/

/*!
  Constructs a validator object which accepts all doubles.
*/

QDoubleValidator::QDoubleValidator( QWidget * parent, const char *name )
    : QValidator( parent, name )
{
    b = -HUGE_VAL;
    t = HUGE_VAL;
    d = 1000;
}


/*!
  Constructs a validator object which accepts all doubles from \a
  bottom up to and including \a top with at most \a decimals digits
  after the decimal point.
*/

QDoubleValidator::QDoubleValidator( double bottom, double top, int decimals,
				    QWidget * parent, const char* name )
    : QValidator( parent, name )
{
    b = bottom;
    t = top;
    d = decimals;
}


/*!
  Destroys the validator, freeing any storage and other resources
  used.
*/

QDoubleValidator::~QDoubleValidator()
{
    // nothing
}


/*!  Returns \a Acceptable if \a input contains a number in the legal
  range and format, \a Intermediate if it contains another number, a
  number with too many digits after the decimal point or is empty, and
  \a Invalid if \a input is not a number.
*/

QValidator::State QDoubleValidator::validate( QString & input, int & ) const
{
    QRegExp empty( QString::fromLatin1("^ *-?\\.? *$") );
    if ( empty.match( input ) >= 0 )
	return QValidator::Intermediate;
    bool ok = TRUE;
    double tmp = input.toDouble( &ok );
    if ( !ok ) {
	QRegExp expexpexp( QString::fromLatin1("e-?\\d*$"), FALSE );
	int eeePos = expexpexp.match( input ); // EXPlicit EXPonent regEXP!
	int nume = input.contains( 'e', FALSE );
	if ( eeePos > 0 && nume < 2 ) {
	    QString mantissa = input.left( eeePos );
	    tmp = mantissa.toDouble( &ok );
	    if ( !ok )
		return QValidator::Invalid;
	}
	else if ( eeePos == 0 ) {
	    return QValidator::Intermediate;
	}
	else {
	    return QValidator::Invalid;
	}
    }

    int i = input.find( '.' );
    if ( i >= 0 ) {
	// has decimal point, now count digits after that
	i++;
	int j = i;
	while( input[j].isDigit() )
	    j++;
	if ( j - i > d )
	    return QValidator::Intermediate;
    }

    if ( tmp < b || tmp > t )
	return QValidator::Intermediate;
    else
	return QValidator::Acceptable;
}


/*!
  Sets the validator to accept numbers from \a bottom up to and
  including \a top with at most \a decimals digits after the decimal
  point.
*/

void QDoubleValidator::setRange( double bottom, double top, int decimals )
{
    b = bottom;
    t = top;
    d = decimals;
}

/*!
  Sets the validator to accept no numbers smaller than \a bottom.

  \sa setRange()
*/

void QDoubleValidator::setBottom( double bottom )
{
    setRange( bottom, top(), decimals() );
}

/*!
  Sets the validator to accept no numbers bigger than \a top.

  \sa setRange()
*/

void QDoubleValidator::setTop( double top )
{
    setRange( bottom(), top, decimals() );
}

/*!
  Sets the maximum number of digits after the decimal point.
*/

void QDoubleValidator::setDecimals( int decimals )
{
    setRange( bottom(), top(), decimals );
}

/*!
  \fn double QDoubleValidator::bottom() const

  Returns the lowest valid number according to this validator.

  \sa top() decimals() setRange()
*/


/*!
  \fn double QDoubleValidator::top() const

  Returns the highest valid number according to this validator.

  \sa bottom() decimals() setRange()
*/


/*!
  \fn int QDoubleValidator::decimals() const

  Returns the largest number of digits a valid number can have after
  its decimal point.

  \sa bottom() top() setRange()
*/
#endif
