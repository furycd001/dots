/***************************************************************************
                          mailheader.h  -  description
                             -------------------
    begin                : Tue Oct 24 2000
    copyright            : (C) 2000 by Sven Carstens
    email                : s.carstens@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MAILHEADER_H
#define MAILHEADER_H

#include <time.h>

#include "mimeheader.h"
#include "mailaddress.h"
#include "mimeio.h"
#include "rfcdecoder.h"

/**
  *@author Sven Carstens
  */

class mailHeader:public mimeHeader
{
public:
  mailHeader ();
  ~mailHeader ();

  virtual QString internalType ()
  {
    return QString ("mailHeader");
  };

  virtual void addHdrLine (mimeHdrLine *);
  virtual void outputHeader (mimeIO &);

  void addTo (const mailAddress & _adr)
  {
    toAdr.append (new mailAddress (_adr));
  };
  void addCC (const mailAddress & _adr)
  {
    ccAdr.append (new mailAddress (_adr));
  };
  void addBCC (const mailAddress & _adr)
  {
    bccAdr.append (new mailAddress (_adr));
  };

  void setFrom (const mailAddress & _adr)
  {
    fromAdr = _adr;
  };
  void setSender (const mailAddress & _adr)
  {
    senderAdr = _adr;
  };
  void setReturnPath (const mailAddress & _adr)
  {
    returnpathAdr = _adr;
  };
  void setReplyTo (const mailAddress & _adr)
  {
    replytoAdr = _adr;
  };

  QCString getMessageId ()
  {
    return messageID;
  };
  void setMessageId (const QCString & _str)
  {
    messageID = _str;
  };

  QCString getInReplyTo ()
  {
    return inReplyTo;
  };
  void setInReplyTo (const QCString & _str)
  {
    inReplyTo = _str;
  };

  // set a unicode subject
  void setSubject (const QString & _str)
  {
    _subject = rfcDecoder::encodeRFC2047String(_str).latin1();
  };
  // set a encoded subject
  void setSubjectEncoded (const QCString & _str)
  {
    _subject = _str.stripWhiteSpace().simplifyWhiteSpace();
  };

  // get the unicode subject
  const QString getSubject ()
  {
    return rfcDecoder::decodeRFC2047String(_subject);
  };
  // get the encoded subject
  QCString getSubjectEncoded ()
  {
    return _subject;
  };

  const struct tm *getDate ()
  {
    return &date;
  };
  void setDate (const QCString & _str)
  {
    mimeHdrLine::parseDate (_str, &date, &gmt_offset);
  }

  QCString dateStr ()
  {
    if (date.tm_year != 0)
      return mimeHdrLine::getDateStr (&date, gmt_offset);
    return QCString ();
  }
  QCString dateShortStr ()
  {
    if (date.tm_year != 0)
      return mimeHdrLine::getDateStr (&date, gmt_offset);
    return QCString ();
  }

  static int parseAddressList (const char *, QList < mailAddress > *);
  static QCString getAddressStr (QList < mailAddress > *);
#ifdef KMAIL_COMPATIBLE
  QString subject ()
  {
    return getSubject ();
  }
  const mailAddress & from ()
  {
    return fromAdr;
  }
  const mailAddress & replyTo ()
  {
    return replytoAdr;
  }
  const QList < mailAddress > &to ()
  {
    return toAdr;
  }
  const QList < mailAddress > &cc ()
  {
    return ccAdr;
  }
  const QList < mailAddress > &bcc ()
  {
    return bccAdr;
  }
  void readConfig (void)
  {;
  }
#endif

private:
  QList < mailAddress > toAdr;
  QList < mailAddress > ccAdr;
  QList < mailAddress > bccAdr;
  mailAddress fromAdr;
  mailAddress senderAdr;
  mailAddress returnpathAdr;
  mailAddress replytoAdr;
  QCString _subject;
  struct tm date;
  int gmt_offset;
  QCString messageID;
  QCString inReplyTo;
};

#endif
