#ifndef RFCDECODER_H
#define RFCDECODER_H
/**********************************************************************
 *
 *   rfcdecoder.h  - handler for various rfc/mime encodings
 *   Copyright (C) 2000 s.carstens@gmx.de
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Send comments and bug fixes to s.carstens@gmx.de
 *
 *********************************************************************/

#include <qstring.h>

class QTextCodec;

// namespace for our rfc en/decoders
class rfcDecoder
{

public:

  static const QString fromIMAP (const QString & src);
  static const QString toIMAP (const QString & inSrc);

  static QString decodeQuoting(const QString &aStr);

  static QTextCodec *codecForName (const QString &);

  // decoder for RFC2047 and RFC1522
  static const QString decodeRFC2047String (const QString & _str,
                                            QString & charset,
                                            QString & language);
  static const QString decodeRFC2047String (const QString & _str,
                                            QString & charset);
  static const QString decodeRFC2047String (const QString & _str);

  // encoder for RFC2047 and RFC1522
  static const QString encodeRFC2047String (const QString & _str,
                                            QString & charset,
                                            QString & language);
  static const QString encodeRFC2047String (const QString & _str,
                                            QString & charset);
  static const QString encodeRFC2047String (const QString & _str);

  static const QString encodeRFC2231String (const QString & _str);
  static const QString decodeRFC2231String (const QString & _str);

  //for the authenticator cram-md5 (no decoder apparently)
  static const QCString encodeRFC2104 (const QCString & text,
                                       const QCString & key);
};

#endif
