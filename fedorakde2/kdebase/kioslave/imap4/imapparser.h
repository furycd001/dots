#ifndef _IMAPPARSER_H
#define _IMAPPARSER_H "$Id: imapparser.h,v 1.0 2000/12/04"
/**********************************************************************
 *
 *   imapparser.h  - IMAP4rev1 Parser
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

#include <qstringlist.h>
#include <qvaluelist.h>
#include <qlist.h>
#include <qdict.h>

#include "imaplist.h"
#include "imapcommand.h"
#include "imapinfo.h"

#include "mailheader.h"

class KURL;
class QString;
class mailAddress;
class mimeHeader;

class imapCache
{
public:
  imapCache ()
  {
    myHeader = NULL;
    mySize = 0;
    myFlags = 0;
    myDate.tm_year = 0;
    myUid = 0;
  }
   ~imapCache ()
  {;
  }

  mailHeader *getHeader ()
  {
    return myHeader;
  }
  void setHeader (mailHeader * inHeader)
  {
    myHeader = inHeader;
  }

  ulong getSize ()
  {
    return mySize;
  }
  void setSize (ulong inSize)
  {
    mySize = inSize;
  }

  ulong getUid ()
  {
    return myUid;
  }
  void setUid (ulong inUid)
  {
    myUid = inUid;
  }

  ulong getFlags ()
  {
    return myFlags;
  }
  void setFlags (ulong inFlags)
  {
    myFlags = inFlags;
  }

  const struct tm *getDate ()
  {
    return &myDate;
  }
  QString getDateStr ()
  {
    return mimeHdrLine::getDateStr (&myDate);
  }
  void setDateStr (const QString & _str)
  {
    mimeHdrLine::parseDate (_str.latin1 (), &myDate);
  }

protected:
  mailHeader * myHeader;
  ulong mySize;
  ulong myFlags;
  ulong myUid;
  struct tm myDate;
};


class imapParser
{

public:

  // the different states the client can be in
  enum IMAP_STATE
  {
    ISTATE_NO,
    ISTATE_CONNECT,
    ISTATE_LOGIN,
    ISTATE_SELECT
  };

public:
    imapParser ();
    virtual ~ imapParser ();

  virtual enum IMAP_STATE getState () { return currentState; }
  virtual void setState(enum IMAP_STATE state) { currentState = state; }

  const QString getCurrentBox ()
  {
    return rfcDecoder::fromIMAP(currentBox);
  };

  imapCommand *sendCommand (imapCommand *);
  imapCommand *doCommand (imapCommand *);


  bool clientLogin (const QString &, const QString &);
  bool clientAuthenticate (const QString &, const QString &, const QString &);

  // main loop for the parser
  // reads one line and dispatches it to the appropriate sub parser
  int parseLoop ();

  // parses all untagged responses and passes them on to the following parsers
  void parseUntagged (QString & result);

  void parseRecent (ulong value, QString & result);
  void parseResult (QString & result, QString & rest,
    const QString & command = QString::null);
  void parseCapability (QString & result);
  void parseFlags (QString & result);
  void parseList (QString & result);
  void parseLsub (QString & result);
  void parseSearch (QString & result);
  void parseStatus (QString & result);
  void parseExists (ulong value, QString & result);
  void parseExpunge (ulong value, QString & result);

  // parses the results of a fetch command
  // processes it with the following sub parsers
  void parseFetch (ulong value, QString & inWords);

  // read a envelope from imap and parse the adresses
  mailHeader *parseEnvelope (QString & inWords);
  QValueList < mailAddress > parseAdressList (QString & inWords);
  mailAddress parseAdress (QString & inWords);

  // parse the result of the body command
  void parseBody (QString & inWords);

  // parse the body structure recursively
  mimeHeader *parseBodyStructure (QString & inWords, const QString & section,
                                  mimeHeader * inHeader = NULL);

  // parse only one not nested part
  mimeHeader *parseSimplePart (QString & inWords, const QString & section);

  // parse a parameter list (name value pairs)
  QDict < QString > parseParameters (QString & inWords);

  // parse the disposition list (disposition (name value pairs))
  // the disposition has the key 'content-disposition'
  QDict < QString > parseDisposition (QString & inWords);

  // reimplement these

  // relay hook to send the fetched data directly to an upper level
  virtual void parseRelay (const QByteArray & buffer);

  // relay hook to announce the fetched data directly to an upper level
  virtual void parseRelay (ulong);

  // read at least len bytes
  virtual bool parseRead (QByteArray & buffer, ulong len, ulong relay = 0);

  // read at least a line (up to CRLF)
  virtual bool parseReadLine (QByteArray & buffer, ulong relay = 0);

  // write argument to server
  virtual void parseWriteLine (const QString &);

  // generic parser routines

  // parse a parenthesized list
  void parseSentence (QString & inWords);

  // parse a literal or word, may require more data
  QString parseLiteral (QString & inWords, bool relay = false);

  // static parser routines, can be used elsewhere

  // skip over whitespace
  static void skipWS (QString & inWords);

  // parse one word (maybe quoted) upto next space " ) ] }
  static QString parseOneWord (QString & inWords);

  // parse one number using parseOneWord
  static bool parseOneNumber (QString & inWords, ulong & num);

  // extract the box,section,list type, uid, uidvalidity from an url
  static void parseURL (const KURL & _url, QString & _box, QString & _section,
                        QString & _type, QString & _uid, QString & _validity);


  // access to the uid cache and other properties
  imapCache *getUid (const QString & _str)
  {
    return uidCache[_str];
  };

  QDictIterator < imapCache > getUidIterator ()
  {
    return QDictIterator < imapCache > (uidCache);
  };
  uint getCacheSize ()
  {
    return uidCache.count ();
  }
  imapCache *getLastHandled ()
  {
    return lastHandled;
  };

  const QStringList & getResults ()
  {
    return lastResults;
  };

  const imapInfo & getStatus ()
  {
    return lastStatus;
  };
  const imapInfo & getSelected ()
  {
    return selectInfo;
  };

  const QString & getContinuation ()
  {
    return continuation;
  };

  bool hasCapability (const QString &);

protected:

  // the current state we're in
  enum IMAP_STATE currentState;

  // the box selected
  QString currentBox;

  // here we store the result from select/examine and unsolicited updates
  imapInfo selectInfo;

  // the results from the last status command
  imapInfo lastStatus;

  // the results from the capabilities, split at ' '
  QStringList imapCapabilities;

  // the results from list/lsub commands
  QValueList < imapList > listResponses;

  // queues handling the running commands
  QList < imapCommand > sentQueue;  // no autodelete
  QList < imapCommand > completeQueue;  // autodelete !!

  // everything we didn't handle, everything but the greeting is bogus
  QStringList unhandled;

  // the last continuation request (there MUST not be more than one pending)
  QString continuation;

  // our own little message cache
  QDict < imapCache > uidCache;

  // the last uid seen while a fetch
  QString seenUid;
  imapCache *lastHandled, *preCache;

  ulong commandCounter;

  QStringList lastResults;

private:
  imapParser & operator = (const imapParser &); // hide the copy ctor

};
#endif
