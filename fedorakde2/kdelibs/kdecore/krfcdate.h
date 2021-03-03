/*
   This file is part of the KDE libraries
   Copyright (c) 2000 Waldo Bastian <bastian@kde.org>
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef _KRFCDATE_H_
#define _KRFCDATE_H_

#include <qstring.h>
#include <time.h>

/**
 * The KDate class contains functions related to the parsing of dates.
 *
 * @author Waldo Bastian <bastian@kde.org>
 */
class KRFCDate
{
public:
   /**
    * This function tries to parse a string containing a date/time in any
    * of the formats specified by RFC822, RFC850, RFC1036, RFC1123 and RFC2822.
    *
    * If the date/time could not be parsed, 0 is returned.  If the
    * parsed date is epoch, then epoch+1 is returned so that a valid
    * date will not be confused with an improper date string.
    *
    * The date/time returned is converted to UTC.
    *
    */
   static time_t parseDate(const QString &);


  /**
   * Returns the local timezone offset to UTC in seconds
   *
   */
   static int localUTCOffset();


  /**
   * Returns a string representation of the given date and time formated
   * in conformance to RFC2822.
   *
   * @param utcTime    a date and time in UTC
   * @param utcOffset  the offset to UTC in seconds
   *
   */

   static QCString rfc2822DateString(time_t utcTime, int utcOffset);


  /**
   * Returns a string representation of the given date and time formated
   * in conformance to RFC2822.
   *
   * Provided for convenience, the function is equivalent to
   * rfc2822DateString(t, localUTCOffset()).
   *
   * @param utcTime    a date and time in UTC
   *
   */

   static QCString rfc2822DateString(time_t utcTime);

};

#endif
