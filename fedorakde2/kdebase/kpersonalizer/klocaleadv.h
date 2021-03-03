/*
  klocaleadv.cpp - Advanced KLocale class

  Copyright 2000 Hans Petter Bieker <bieker@kde.org>
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   
  */

#ifndef KLOCALEADV_H
#define KLOCALEADV_H

#include <klocale.h>

class KLocaleAdvanced : public KLocale
{
public:
  KLocaleAdvanced( const QString& catalogue = QString::null );
  ~KLocaleAdvanced();

  void setChset(const QString &chrset);
  QString country() const;
  void setCountry(const QString &country);

  // dates
  void setDateFormat(const QString &fmt);
  void setDateFormatShort(const QString &fmt);
  void setTimeFormat(const QString &fmt);
  void setWeekStartsMonday(bool start);
  QString dateFormat() const;
  QString dateFormatShort() const;
  QString timeFormat() const;

  // numbers
  void setDecimalSymbol(const QString &symb);
  void setThousandsSeparator(const QString &sep);
  void setPositiveSign(const QString &sign);
  void setNegativeSign(const QString &sign);

  // money
  void setPositiveMonetarySignPosition(SignPosition signpos);
  void setNegativeMonetarySignPosition(SignPosition signpos);
  void setPositivePrefixCurrencySymbol(bool prefixcur);
  void setNegativePrefixCurrencySymbol(bool prefixcur);
  void setFracDigits(int digits);
  void setMonetaryThousandsSeparator(const QString &sep);
  void setMonetaryDecimalSymbol(const QString &symbol);
  void setCurrencySymbol(const QString &symbol);
private:
  QString _country;
};

#endif
