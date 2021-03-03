/*
  klocaleadv.cpp - A KControl Application

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


#include "klocaleadv.h"

KLocaleAdvanced::KLocaleAdvanced( const QString& catalogue )
  : KLocale( catalogue )
{
}

KLocaleAdvanced::~KLocaleAdvanced()
{
}

void KLocaleAdvanced::setChset(const QString &chrset)
{
  chset = chrset;
}

void KLocaleAdvanced::setDateFormat(const QString &fmt)
{
  _datefmt = fmt.stripWhiteSpace();
}

void KLocaleAdvanced::setDateFormatShort(const QString &fmt)
{
  _datefmtshort = fmt.stripWhiteSpace();
}

void KLocaleAdvanced::setTimeFormat(const QString &fmt)
{
  _timefmt = fmt.stripWhiteSpace();
}

void KLocaleAdvanced::setWeekStartsMonday(bool start)
{
  m_weekStartsMonday = start;
}

QString KLocaleAdvanced::dateFormat() const
{
  return _datefmt;
}

QString KLocaleAdvanced::dateFormatShort() const
{
  return _datefmtshort;
}

QString KLocaleAdvanced::timeFormat() const
{
  return _timefmt;
}

QString KLocaleAdvanced::country() const
{
  return _country;
}

void KLocaleAdvanced::setDecimalSymbol(const QString &symb)
{
  _decimalSymbol = symb.stripWhiteSpace();
}

void KLocaleAdvanced::setThousandsSeparator(const QString &sep)
{
  // allow spaces here
  _thousandsSeparator = sep;
}

void KLocaleAdvanced::setPositiveSign(const QString &sign)
{
  _positiveSign = sign.stripWhiteSpace();
}

void KLocaleAdvanced::setNegativeSign(const QString &sign)
{
  _negativeSign = sign.stripWhiteSpace();
}

void KLocaleAdvanced::setPositiveMonetarySignPosition(SignPosition signpos)
{
  _positiveMonetarySignPosition = signpos;
}

void KLocaleAdvanced::setNegativeMonetarySignPosition(SignPosition signpos)
{
  _negativeMonetarySignPosition = signpos;
}

void KLocaleAdvanced::setPositivePrefixCurrencySymbol(bool prefixcur)
{
  _positivePrefixCurrencySymbol = prefixcur;
}

void KLocaleAdvanced::setNegativePrefixCurrencySymbol(bool prefixcur)
{
  _negativePrefixCurrencySymbol = prefixcur;
}

void KLocaleAdvanced::setFracDigits(int digits)
{
  _fracDigits = digits;
}

void KLocaleAdvanced::setMonetaryThousandsSeparator(const QString &sep)
{
  // allow spaces here
  _monetaryThousandsSeparator = sep;
}

void KLocaleAdvanced::setMonetaryDecimalSymbol(const QString &symbol)
{
  _monetaryDecimalSymbol = symbol.stripWhiteSpace();
}

void KLocaleAdvanced::setCurrencySymbol(const QString &symbol)
{
  _currencySymbol = symbol.stripWhiteSpace();
}

void KLocaleAdvanced::setCountry(const QString &country)
{
  _country = country.stripWhiteSpace();
}
