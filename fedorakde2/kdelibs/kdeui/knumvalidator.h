/**********************************************************************
**
** $Id$
**
** Copyright (C) 1999 Glen Parker <glenebob@nwlink.com>
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the Free
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
**
*****************************************************************************/

#ifndef __KNUMVALIDATOR_H
#define __KNUMVALIDATOR_H

#include <qvalidator.h>

/**
  @ref QValidator for integers.

  This can be used by @ref QLineEdit or subclass to provide validated
  text entry.  Can be provided with a base value (default is 10), to allow
  the proper entry of hexadecimal, octal, or any other base numeric data.

  @author Glen Parker <glenebob@nwlink.com>
  @version 0.0.1
*/
class KIntValidator : public QValidator {

  public:
    /**
      Constuctor.  Also sets the base value.
    */
    KIntValidator ( QWidget * parent, int base = 10, const char * name = 0 );
    /**
      Constuctor.  Also sets the minimum, maximum, and numeric base values.
    */
    KIntValidator ( int bottom, int top, QWidget * parent, int base = 10, const char * name = 0 );
    /**
      Destructor.
    */
    virtual ~KIntValidator ();
    /**
      Validate the text, and return the result.  Does not modify the paramaters.
    */
    virtual State validate ( QString &, int & ) const;
    /**
      Fix the text if possible, providing a valid string.  The parameter may be modified.
    */
    virtual void fixup ( QString & ) const;
    /**
      Set the minimum and maximum values allowed.
    */
    virtual void setRange ( int bottom, int top );
    /**
      Set the numeric base value.
    */
    virtual void setBase ( int base );
    /**
      Return the current minimum value allowed.
    */
    virtual int bottom () const;
    /**
      Return the current maximum value allowed.
    */
    virtual int top () const;
    /**
      Return the current numeric base.
    */
    virtual int base () const;

  protected:
    int _base;
    int _min;
    int _max;

};

/**
  @ref QValidator for floating point entry.
  Extends the QValidator class to properly validate double numeric data.
  This can be used by @ref QLineEdit or subclass to provide validated
  text entry.

  @author Glen Parker <glenebob@nwlink.com>
  @version 0.0.1
*/

// Just a marker for a feature that was added after 2.2-beta2.
// When KOffice depends on kdelibs-2.2, this can be removed.
#define KFLOATVALIDATOR_HAS_USEDLOCALPARAMETER
class KFloatValidatorPrivate;
class KFloatValidator : public QValidator {

  public:
    /**
      Constuctor.
    */
    KFloatValidator ( QWidget * parent, const char * name = 0 );
    /**
      Constuctor.  Also sets the minimum and maximum values.
    */
    KFloatValidator ( double bottom, double top, QWidget * parent, const char * name = 0 );
    /**
      Destructor.
    */
    virtual ~KFloatValidator ();
    /**
      Validate the text, and return the result.  Does not modify the paramaters.
    */
    virtual State validate ( QString &, int & ) const;
    /**
      Fix the text if possible, providing a valid string.  The parameter may be modified.
    */
    virtual void fixup ( QString & ) const;
    /**
      Set the minimum and maximum value allowed.
    */
    virtual void setRange ( double bottom, double top );
    /**
      Return the current minimum value allowed.
    */
    virtual double bottom () const;
    /**
      Return the current maximum value allowed.
    */
    virtual double top () const;
    
    void setAcceptLocalizedNumbers(bool _b);


  protected:
    double _min;
    double _max;
 private:
    KFloatValidatorPrivate *d;
};


#endif
