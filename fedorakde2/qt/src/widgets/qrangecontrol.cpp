/****************************************************************************
** $Id: qt/src/widgets/qrangecontrol.cpp   2.3.2   edited 2001-09-21 $
**
** Implementation of QRangeControl class
**
** Created : 940427
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

#include "qrangecontrol.h"
#ifndef QT_NO_RANGECONTROL
#include "qglobal.h"
#include <limits.h>

// BEING REVISED: weis
/*!
  \class QRangeControl qrangecontrol.h
  \brief The QRangeControl class provides an integer value within a range.

  \ingroup misc

  It was originally designed for the QScrollBar widget, but it can
  also be practical for other purposes such as QSlider and QSpinBox.
  Here are the five main concepts in the class: <ul>

  <li> The current value.  This is the bounded integer that
  QRangeControl maintains.  value() returns this, and several
  functions including setValue() set it.

  <li> The minimum.  This is the lowest value value() can ever return.
  Returned by minValue(), set by setRange() or one of the
  constructors.

  <li> The maximum.  This is the highest value value() can ever return.
  Returned by maxValue(), set by setRange() or one of the
  constructors.

  <li> The line step.  This is the smaller of two natural steps
  QRangeControl provides, and typically corresponds to the user
  pressing an arrow key.  The line step is returned by lineStep()
  and set using setSteps(). The functions addLine() and subtractLine() allow easy
  movement of the current value by lineStep().

  <li> The page step.  This is the larger of two natural steps
  QRangeControl provides, and typically corresponds to the user
  pressing one of the PageUp and PageDown keys.  The page step is
  returned by pageStep() and set using setSteps().
  The functions addPage() and substractPage() allow easy movement
  of the current value by pageStep().

  </ul>

  Note that unity (1) may be viewed as a third step size.  setValue()
  lets you set the current value to any integer in the allowed range,
  not just minValue()+n*lineStep() for integer values of n.  Some
  widgets may allow the user to set any value at all, others may just
  provide multiples of lineStep()/pageStep().

  QRangeControl provides three virtual functions that are well-suited
  e.g. for updating the on-screen representation of range controls and
  emitting signals, namely valueChange(), rangeChange() and
  stepChange().

  Finally, QRangeControl provides a function called bound() which lets you
  force arbitrary integers to be within the allowed range of the range control.

  We recommend that all widgets, which inherit QRangeControl,
  provide at least a signal called
  valueChanged(), and many widgets will want to provide addStep(),
  addPage(), substractStep() and substractPage() as slots.
  
  Note that you have to use multiple inheritance if you plan to implement
  a widget using QRangeControl, since QRangeControl is not derived from
  QWidget.
*/


/*!
  Constructs a range control with min value 0, max value 99,
  line step 1, page step 10 and initial value 0.
*/

QRangeControl::QRangeControl()
{
    minVal  = 0;
    maxVal  = 99;
    line    = 1;
    page    = 10;
    val	    = 0;
    prevVal = -1;
    d	    = 0;
}

/*!
  Constructs a range control whose value can never be smaller than \a
  minValue or greater than \a maxValue, whose line step size is \a
  lineStep and page step size is \a pageStep, and whose value is
  initially \a value.

  \a value is forced to be within the legal range using the bound() method.
*/

QRangeControl::QRangeControl( int minValue, int maxValue,
			      int lineStep, int pageStep,
			      int value )
{
    minVal  = minValue;
    maxVal  = maxValue;
    line    = QABS( lineStep );
    page    = QABS( pageStep );
    prevVal = minVal - 1;
    val	    = bound( value );
    d	    = 0;
}


/*!
  \fn int QRangeControl::value() const

  Returns the current range control value.  This is guaranteed to be
  within the range [ minValue() ... maxValue() ].

  \sa setValue() prevValue()
*/

/*!
  \fn int QRangeControl::prevValue() const

  Returns the previous value of the range control. "Previous value"
  means the value before the last change occurred. Setting a new range
  may affect the value, too, since the value is forced to be inside
  the specified range. When the range
  control is initially created, this is the same as value().

  Note that prevValue() can be outside the current legal range if a call to
  setRange() causes the current value to change.  (For example if the
  range was 0-1000 and the current value 500, setRange( 0, 400 ) makes
  value() return 400 and prevValue() 500)

  \sa value() setRange()
*/

/*!
  Sets the range controls value to \e value and forces it to be within
  the legal range.

  Calls the virtual valueChange() function if the new value is
  different from the previous value. The old value can still be
  retrieved using prevValue().
  
   \sa value()
*/

void QRangeControl::setValue( int value )
{
    int lprevVal = prevVal;
    directSetValue( value );
    if ( lprevVal != val || prevVal != val )
	valueChange();
}

/*!
  Sets the range control value directly without calling valueChange().

  Forces the new value to be within the legal range.

  You will find few cases only where you have to call this function.
  However, if you want to change the range controls value inside
  the overloaded method valueChange() then setValue() would call the
  function valueChange() again. To avoid this recursion you must use
  directSetValue() instead.
  
  \sa setValue()
*/

void QRangeControl::directSetValue(int value)
{
    prevVal = val;
    val = bound( value );
}

/*!
  Equivalent to \code setValue( value()+pageStep() )\endcode plus a
  test for numerical overflow.

  If the value is changed, then valueChange() is called.
  
  \sa subtractPage() addLine() setValue()
*/

void QRangeControl::addPage()
{
    if ( value() + pageStep() < maxValue() )
	setValue( value() + pageStep() );
    else
	setValue( maxValue() );
}

/*!
  Equivalent to \code setValue( value()-pageStep() )\endcode  plus a
  test for numerical underflow
  
  If the value is changed, then valueChange() is called.
    
  \sa addPage() subtractLine() setValue()
*/

void QRangeControl::subtractPage()
{
    if ( value() - pageStep() > minValue() )
	setValue( value() - pageStep() );
    else
	setValue( minValue() );
}

/*!
  Equivalent to \code setValue( value()+lineStep() )\endcode  plus a
  test for numerical overflow
  
  If the value is changed, then valueChange() is called.
    
  \sa subtractLine() addPage() setValue()
*/

void QRangeControl::addLine()
{
    if ( value() + lineStep() < maxValue() )
	setValue( value() + lineStep() );
    else
	setValue( maxValue() );
}

/*!
  Equivalent to \code setValue( value()-lineStep() )\endcode plus a
  test for numerical underflow
  
  If the value is changed, then valueChange() is called.
    
  \sa addLine() subtractPage() setValue()
*/

void QRangeControl::subtractLine()
{
    if ( value() - lineStep() > minValue() )
	setValue( value() - lineStep() );
    else
	setValue( minValue() );
}


/*!
  \fn int QRangeControl::minValue() const

  Returns the current minimum value of the range.

  \sa setRange() maxValue()
*/

/*!
  \fn int QRangeControl::maxValue() const

  Returns the current maximum value of the range.

  \sa setRange() minValue()
*/

/*!
  Sets the range min value to \e minValue and the max value to \e
  maxValue.

  Calls the virtual rangeChange() function if one or both of the new
  min and max values are different from the previous setting.  Calls
  the virtual valueChange() function if the current value is adjusted
  because it was outside the new range.

  If \a maxValue is smaller than \a minValue, \a minValue becomes the
  only legal value.

  \sa minValue() maxValue()
*/

void QRangeControl::setRange( int minValue, int maxValue )
{
    if ( minValue == minVal && maxValue == maxVal )
	return;
    if ( minValue > maxValue ) {
#if defined(CHECK_RANGE)
	qWarning( "QRangeControl::setRange: minValue %d > maxValue %d",
		 minValue, maxValue );
#endif
	minVal = maxVal = minValue;
    } else {
	minVal = minValue;
	maxVal = maxValue;
    }
    int tmp = bound( val );
    rangeChange();
    if ( tmp != val ) {
	prevVal = val;
	val = tmp;
	valueChange();
    }
}


/*!
  \fn int QRangeControl::lineStep() const
  
  Returns the current line step.
  
  \sa setSteps() pageStep()
*/

/*!
  \fn int QRangeControl::pageStep() const

  Returns the current page step.
  
  \sa setSteps() lineStep()
*/

/*!
  Sets the range line step to \e lineStep and page step to \e pageStep.

  Calls the virtual stepChange() function if the new line step and/or
  page step are different from the previous settings.

  \sa lineStep() pageStep() setRange()
*/

void QRangeControl::setSteps(int lineStep,int pageStep)
{
    if (lineStep != line || pageStep != page) {
	line = QABS(lineStep);
	page = QABS(pageStep);
	stepChange();
    }
}


/*!
  This virtual function is called whenever the range control value
  changes.  You can reimplement it if you want to be notified when the
  value changes.  The default implementation does nothing.

  Note that this method is called after the value changed. The previous
  value can be retrieved using prevValue().
  
  \sa setValue(), addPage(), subtractPage(), addLine(), subtractLine()
  rangeChange(), stepChange()
*/

void QRangeControl::valueChange()
{
}


/*!
  This virtual function is called whenever the range controls range
  changes.  You can reimplement it if you want to be notified when the range
  changes.  The default implementation does nothing.

  Note that this method is called after the range changed.
    
  \sa setRange(), valueChange(), stepChange()
*/

void QRangeControl::rangeChange()
{
}


/*!
  This virtual function is called whenever the range controls line/page step
  settings changes.  You can reimplement it if you want to be notified
  when the step changes.  The default implementation does nothing.

  Note that this method is called after the step settings changed.
    
  \sa setSteps(), rangeChange(), valueChange()
*/

void QRangeControl::stepChange()
{
}


/*!  Forces \a v to be within the range from minValue() to maxValue()
  inclusive, and returns the result.

  This function is provided so that you can easily force other numbers
  than value() into the allowed range.  You do not need to call it in
  order to use QRangeControl itself.

  \sa setValue() value() minValue() maxValue()
*/

int QRangeControl::bound( int v ) const
{
    if ( v < minVal )
	return minVal;
    if ( v > maxVal )
	return maxVal;
    return v;
}


/*!
  Converts \a logical_val to a pixel position.  minValue() maps to 0, maxValue()
  maps to \a span, and other values are distributed evenly in between.

  This function can handle the entire integer range without overflow.
  
  Callings this method is useful when actually drawing a range control
  like a QScrollBar on the screen.
  
  \sa valueFromPosition()
*/

int QRangeControl::positionFromValue( int logical_val, int span ) const
{
    if ( span <= 0 || logical_val < minValue() || maxValue() <= minValue() )
	return 0;
    if ( logical_val > maxValue() )
	return span;

    uint range = maxValue() - minValue();
    uint p = logical_val - minValue();

    if ( range > (uint)INT_MAX/4096 ) {
	const int scale = 4096*2;
	return ( (p/scale) * span ) / (range/scale);
	// ### the above line is probably not 100% correct
	// ### but fixing it isn't worth the extreme pain...
    } else if ( range > (uint)span ) {
	return (2*p*span + range) / (2*range);
    } else {
	uint div = span / range;
	uint mod = span % range;
	return p*div + (2*p*mod + range) / (2*range);
    }
    //equiv. to (p*span)/range + 0.5
    // no overflow because of this implicit assumption:
    // span <= 4096
}


/*!
  Converts the pixel position \a pos to a value.  0 maps to minValue(),
  \a span maps to maxValue(), and other values are distributed evenly
  in between.

  This function can handle the entire integer range without overflow.
  
  Calling this method is useful if you actually implemented a range control
  widget like QScrollBar and want to handle mouse press events.  This function
  maps then screen coordinates to the logical values.
  
  \sa positionFromValue()
*/

int QRangeControl::valueFromPosition( int pos, int span ) const
{
    if ( span <= 0 || pos <= 0 )
	return minValue();
    if ( pos >= span )
	return maxValue();

    uint range = maxValue() - minValue();

    if ( (uint)span > range )
	return  minValue() + (2*pos*range + span) / (2*span);
    else {
	uint div = range / span;
	uint mod = range % span;
	return  minValue() + pos*div + (2*pos*mod + span) / (2*span);
    }
    // equiv. to minValue() + (pos*range)/span + 0.5
    // no overflow because of this implicit assumption:
    // pos <= span < sqrt(INT_MAX+0.0625)+0.25 ~ sqrt(INT_MAX)
}
#endif
