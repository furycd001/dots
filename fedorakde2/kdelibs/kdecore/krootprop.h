/* This file is part of the KDE libraries
    Copyright (C) 1997 Mark Donohoe (donohoe@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/
#ifndef _KROOTPROP_H
#define _KROOTPROP_H

typedef unsigned long Atom;

#include <qcolor.h>
#include <qfont.h>
#include <qstring.h>
#include <qmap.h>
#include <qstringlist.h>
#include <kapp.h>

class KRootPropPrivate;

/**
* Access KDE desktop resources stored on the root window.
*
* A companion to the @ref KConfig class.
*
* The @ref KRootProp class is used for reading and writing configuration entries
* to properties on the root window.
*
* All configuration entries are of the form "key=value".
*
* @see  KConfig::KConfig
* @author Mark Donohoe (donohe@kde.org)
* @version $Id$
*/
class KRootProp
{
private:	
  Atom atom;
  QMap<QString,QString> propDict;
  QString property_;
  bool dirty;
  KRootPropPrivate *d;

protected:

public:
  /**
   * Construct a @ref KRootProp object for the property @p rProp.
   *
   **/
   KRootProp( const QString& rProp = QString::null );

/**
 * Destructor.
 *
 * Writes back any dirty configuration entries.
 **/
   ~KRootProp();
   
/**
 * Specify the property in which keys will be searched.
 *
 **/	
   void setProp(const QString& rProp="");
   
   
   /**
    * Retrieve the name of the property under which keys are searched.
    **/
   QString prop() const;
   
   /**
    * Destroy the property completely.
    *
    * I.e. all entries will be cleared
    * and the property will be removed from the root window.
    **/
 void destroy();

 /**
  * Read the value of an entry specified by @p rKey in the current property
  *
  * @param rKey	The key to search for.
  * @param pDefault A default value returned if the key was not found.
  * @return The value for this key or the default if no value
  *	  was found.
  **/	
 QString readEntry( const QString& rKey,
		    const QString& pDefault = QString::null ) const ;
					
 /**
  * Read a numerical value.
  *
  * Read the value of an entry specified by @p rKey in the current property
  * and interpret it numerically.
  *
  * @param rKey The key to search for.
  * @param nDefault A default value returned if the key was not found.
  * @return The value for this key or the default if no value was found.
  */
 int readNumEntry( const QString& rKey, int nDefault = 0 ) const;
 
 /**
  * Read a @ref QFont.
  *
  * Read the value of an entry specified by @p rKey in the current property
  * and interpret it as a font object.
  *
  * @param rKey		The key to search for.
  * @param pDefault	A default value returned if the key was not found.
  * @return The value for this key or a default font if no value was found.
  */
 QFont readFontEntry( const QString& rKey,
		      const QFont* pDefault = 0 ) const;
 
 /**
  * Read a @ref QColor.
  *
  * Read the value of an entry specified by @p rKey in the current property
  * and interpret it as a color.
  *
  * @param rKey		The key to search for.
  * @param pDefault	A default value returned if the key was not found.
  * @return The value for this key or a default color if no value
  * was found.
  */					
 QColor readColorEntry( const QString& rKey,
			const QColor* pDefault = 0 ) const;
							
	
 /**
  * @ref writeEntry() overridden to accept a const @ref QString& argument.
  *
  * This is stored to the current property when destroying the
  * config object or when calling @ref sync().
  *
  * @param rKey		The key to write.
  * @param rValue		The value to write.
  * @return The old value for this key. If this key did not exist,
  *	  a null string is returned.	
  *
  **/				
 QString writeEntry( const QString& rKey, const QString& rValue );

 /** Write the key value pair.
  * Same as above, but write a numerical value.
  * @param rKey The key to write.
  * @param nValue The value to write.
  * @return The old value for this key. If this key did not
  * exist, a null string is returned.	
  **/
 QString writeEntry( const QString& rKey, int nValue );

 /** Write the key value pair.
  * Same as above, but write a font.
  * @param rKey The key to write.
  * @param rValue The value to write.
  * @return The old value for this key. If this key did not
  * exist, a null string is returned.	
  **/
  QString writeEntry( const QString& rKey, const QFont& rFont );
 
  /** Write the key value pair.
   * Same as above, but write a color.
   * @param rKey The key to write.
   * @param rValue The value to write.
   * @return The old value for this key. If this key did not
   *  exist, a null string is returned.	
   **/
  QString writeEntry( const QString& rKey, const QColor& rColor );
  
  /**
   * Remove an entry.
   * @param rKey The key to remove.
   * @return The old value for this key. If this key did not
   *  exist, a null string is returned.
   **/
  QString removeEntry(const QString& rKey);

  /**
   * Get a list of all keys.
   * @return A @ref QStringList containing all the keys.
   **/
  QStringList listEntries() const;

  /** Flush the entry cache.
   * Write back dirty configuration entries to the current property,
   *  This is called automatically from the destructor.
   **/	
  void sync();
};

#endif
