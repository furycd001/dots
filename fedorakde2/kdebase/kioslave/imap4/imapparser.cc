/**********************************************************************
 *
 *   imapparser.cc  - IMAP4rev1 Parser
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

#include "rfcdecoder.h"

#include "imapparser.h"

#include "imapinfo.h"

#include "mailheader.h"
#include "mimeheader.h"
#include "mailaddress.h"

#include <sys/types.h>

#include <stdlib.h>
#include <unistd.h>

#include <qregexp.h>
#include <qbuffer.h>
#include <qstring.h>
#include <qstringlist.h>

#include <kdebug.h>
#include <kmdcodec.h>
#include <kurl.h>

imapParser::imapParser ():
uidCache (17, false)
{
  uidCache.setAutoDelete (true);
  sentQueue.setAutoDelete (false);
  completeQueue.setAutoDelete (true);
  currentState = ISTATE_NO;
  commandCounter = 0;
  lastHandled = NULL;
  preCache = NULL;
}

imapParser::~imapParser ()
{
}

imapCommand *
imapParser::doCommand (imapCommand * aCmd)
{
  int pl = 0;
  sendCommand (aCmd);
  while (pl != -1 && !aCmd->isComplete ())
    while ((pl = parseLoop ()) == 0);

  return aCmd;
}

imapCommand *
imapParser::sendCommand (imapCommand * aCmd)
{
  aCmd->setId (QString ().setNum (commandCounter++));
  sentQueue.append (aCmd);

  continuation = QString::null;

  if (aCmd->command () == "SELECT" || aCmd->command () == "EXAMINE")
  {
    currentBox = aCmd->parameter ();
    currentBox = parseOneWord(currentBox);
    kdDebug(7116) << "imapParser::sendCommand - setting current box to " << currentBox << endl;
  }
  else if (aCmd->command ().find ("SEARCH") != -1)
  {
    lastResults.clear ();
  }
  else if (aCmd->command ().find ("LIST") != -1)
  {
    listResponses.clear ();
  }
  else if (aCmd->command ().find ("LSUB") != -1)
  {
    listResponses.clear ();
  }
  parseWriteLine (aCmd->getStr ());
  return aCmd;
}

bool
imapParser::clientLogin (const QString & aUser, const QString & aPass)
{
  imapCommand *cmd;
  bool retVal = false;

  cmd =
    doCommand (new
               imapCommand ("LOGIN", "\"" + aUser + "\" \"" + aPass + "\""));

  if (cmd->result () == "OK")
  {
    currentState = ISTATE_LOGIN;
    retVal = true;
  }
  completeQueue.removeRef (cmd);

  return retVal;
}


bool
imapParser::clientAuthenticate (const QString & aUser, const QString & aPass,
                                const QString & aAuth)
{
  imapCommand *cmd;
  bool retVal = false;

  // see if server supports this authenticator
  if (!hasCapability ("AUTH=" + aAuth))
    return false;

  // then lets try it
  cmd = sendCommand (new imapCommand ("AUTHENTICATE", aAuth));
  while (!cmd->isComplete ())
  {
    //read the next line
    while (parseLoop() == 0);

    if (!continuation.isEmpty ())
    {
      QString challenge = continuation;

      parseOneWord (challenge); // +
      challenge = challenge.left (challenge.length () - 2); // trim CRLF

      kdDebug(7116) << "IMAP4: authenticate key=" << challenge << endl;

      if (aAuth.upper () == "LOGIN")
      {
        challenge = KCodecs::base64Decode(challenge);
        if (challenge.find ("User", 0, false) != -1)
        {
          challenge = KCodecs::base64Encode(aUser.utf8());
        }
        else if (challenge.find ("Pass", 0, false) != -1)
        {
          challenge = KCodecs::base64Encode(aPass.utf8());
        }
      }
      else if (aAuth.upper () == "ANONYMOUS")
      {
        // we should present the challenge to the user and ask
        // him for a mail-adress or what ever
        challenge = KCodecs::base64Encode(aUser.utf8());
      }
      else if (aAuth.upper () == "PLAIN")
      {
        challenge = KCodecs::base64Encode(aUser + '\0' + aUser + '\0' + aPass);
      }
      else if (aAuth.upper () == "CRAM-MD5")
      {
        QCString password = aPass.latin1 ();
        QCString cchallenge = KCodecs::base64Decode(challenge).latin1();

        challenge = rfcDecoder::encodeRFC2104 (cchallenge, password);
        challenge = aUser + " " + challenge;
        challenge = KCodecs::base64Encode(challenge.utf8());
      }

      // we will ALWAYS write back a line to satisfiy the continuation
      parseWriteLine (challenge);

//      kdDebug(7116) << "Wrote: '" << rfcDecoder::decodeBase64(challenge.utf8()) << "'" << endl;
      continuation = QString::null;
    }
  }

  if (cmd->result () == "OK")
  {
    currentState = ISTATE_LOGIN;
    retVal = true;
  }
  completeQueue.removeRef (cmd);

  return retVal;
}

void
imapParser::parseUntagged (QString & result)
{
//  kdDebug(7116) << "imapParser::parseUntagged - '" << result << "'" << endl;

  parseOneWord (result);        // *
  QString what = parseOneWord (result); // see whats coming next

  switch (what[0].latin1 ())
  {
    //the status responses
  case 'B':                    // BAD or BYE
    if (what == "BAD")
    {
      parseResult (what, result);
    }
    else if (what == "BYE")
    {
      parseResult (what, result);
      currentState = ISTATE_NO;
    }
    else
    {
      kdDebug(7116) << "imapParser::parseUntagged - unknown response " << what << " " << result << endl;
    }
    break;

  case 'N':                    // NO
    if (what[1] == 'O' && what.length () == 2)
    {
      parseResult (what, result);
    }
    else
    {
      kdDebug(7116) << "imapParser::parseUntagged - unknown response " << what << " " << result << endl;
    }
    break;

  case 'O':                    // OK
    if (what[1] == 'K' && what.length () == 2)
    {
      parseResult (what, result);
    }
    else
    {
      kdDebug(7116) << "imapParser::parseUntagged - unknown response " << what << " " << result << endl;
    }
    break;

  case 'P':                    // PREAUTH
    if (what == "PREAUTH")
    {
      parseResult (what, result);
      currentState = ISTATE_LOGIN;
    }
    else
    {
      kdDebug(7116) << "imapParser::parseUntagged - unknown response " << what << " " << result << endl;
    }
    break;

    // parse the other responses
  case 'C':                    // CAPABILITY
    if (what == "CAPABILITY")
    {
      parseCapability (result);
    }
    else
    {
      kdDebug(7116) << "imapParser::parseUntagged - unknown response " << what << " " << result << endl;
    }
    break;

  case 'F':                    // FLAGS
    if (what == "FLAGS")
    {
      parseFlags (result);
    }
    else
    {
      kdDebug(7116) << "imapParser::parseUntagged - unknown response " << what << " " << result << endl;
    }
    break;

  case 'L':                    // LIST or LSUB
    if (what == "LIST")
    {
      parseList (result);
    }
    else if (what == "LSUB")
    {
      parseLsub (result);
    }
    else
    {
      kdDebug(7116) << "imapParser::parseUntagged - unknown response " << what << " " << result << endl;
    }
    break;

  case 'S':                    // SEARCH or STATUS
    if (what == "SEARCH")
    {
      parseSearch (result);
    }
    else if (what == "STATUS")
    {
      parseStatus (result);
    }
    else
    {
      kdDebug(7116) << "imapParser::parseUntagged - unknown response " << what << " " << result << endl;
    }
    break;

  default:
    //better be a number
    {
      ulong number;
      bool valid;

      number = what.toUInt (&valid);
      if (valid)
      {
        what = parseLiteral (result);
        switch (what[0].latin1 ())
        {
        case 'E':
          if (what == "EXISTS")
          {
            parseExists (number, result);
          }
          else if (what == "EXPUNGE")
          {
            parseExpunge (number, result);
          }
          else
          {
            kdDebug(7116) << "imapParser::parseUntagged - unknown response " << number << "d " << what << " " << result << endl;
          }
          break;

        case 'F':
          if (what == "FETCH")
          {
            seenUid = QString::null;
            if (preCache) delete preCache;
            preCache = new imapCache();
            parseFetch (number, result);
          }
          else
          {
            kdDebug(7116) << "imapParser::parseUntagged - unknown response " << number << "d " << what << " " << result << endl;
          }
          break;

        case 'S':
          if (what == "STORE")  // deprecated store
          {
            seenUid = QString::null;
            parseFetch (number, result);
          }
          else
          {
            kdDebug(7116) << "imapParser::parseUntagged - unknown response " << number << "d " << what << " " << result << endl;
          }
          break;

        case 'R':
          if (what == "RECENT")
          {
            parseRecent (number, result);
          }
          else
          {
            kdDebug(7116) << "imapParser::parseUntagged - unknown response " << number << "d " << what << " " << result << endl;
          }
          break;
        default:
          kdDebug(7116) << "imapParser::parseUntagged - unknown response " << number << "d " << what << " " << result << endl;
          break;
        }
      }
      else
      {
        kdDebug(7116) << "imapParser::parseUntagged - unknown response " << what << " " << result << endl;
      }
    }
    break;
  }                             //switch
}                               //func


void
imapParser::parseResult (QString & result, QString & rest,
  const QString & command)
{
  if (command == "SELECT") selectInfo.setReadWrite(true);

  if (rest[0] == '[')
  {
    rest = rest.right (rest.length () - 1); //tie off [
    QString option = parseOneWord (rest);

    switch (option[0].latin1 ())
    {
    case 'A':                  // ALERT
      if (option == "ALERT")
      {
        kdDebug(7116) << "imapParser::parseResult - " << result << " " << option << " " << rest << endl;
      }
      else
      {
        kdDebug(7116) << "imapParser::parseResult - unknown response " << result << " " << option << " " << rest << endl;
      }
      break;

    case 'N':                  // NEWNAME
      if (option == "NEWNAME")
      {
        kdDebug(7116) << "imapParser::parseResult - " << result << " " << option << " " << rest << endl;
      }
      else
      {
        kdDebug(7116) << "imapParser::parseResult - unknown response " << result << " " << option << " " << rest << endl;
      }
      break;

    case 'P':                  //PARSE or PERMANENTFLAGS
      if (option == "PARSE")
      {
        kdDebug(7116) << "imapParser::parseResult - " << result << " " << option << " " << rest << endl;
      }
      else if (option == "PERMANENTFLAGS")
      {
//          kdDebug(7116) << "imapParser::parseResult - " << result << " " << option << " " << rest << endl;
        QString flags = rest.left (rest.find (']'));
        selectInfo.setPermanentFlags (flags);
      }
      else
      {
        kdDebug(7116) << "imapParser::parseResult - unknown response " << result << " " << option << " " << rest << endl;
      }
      break;

    case 'R':                  //READ-ONLY or READ-WRITE
      if (option == "READ-ONLY")
      {
//          kdDebug(7116) << "imapParser::parseResult - " << result << " " << option << " " << rest << endl;
        selectInfo.setReadWrite (false);
      }
      else if (option == "READ-WRITE")
      {
//          kdDebug(7116) << "imapParser::parseResult - " << result << " " << option << " " << rest << endl;
        selectInfo.setReadWrite (true);
      }
      else
      {
        kdDebug(7116) << "imapParser::parseResult - unknown response " << result << " " << option << " " << rest << endl;
      }
      break;

    case 'T':                  //TRYCREATE
      if (option == "TRYCREATE")
      {
        kdDebug(7116) << "imapParser::parseResult - " << result << " " << option << " " << rest << endl;
      }
      else
      {
        kdDebug(7116) << "imapParser::parseResult - unknown response " << result << " " << option << " " << rest << endl;
      }
      break;

    case 'U':                  //UIDVALIDITY or UNSEEN
      if (option == "UIDVALIDITY")
      {
//          kdDebug(7116) << "imapParser::parseResult - " << result << " " << option << " " << rest << endl;
        ulong value;
        if (parseOneNumber (rest, value))
          selectInfo.setUidValidity (value);
      }
      else if (option == "UNSEEN")
      {
//          kdDebug(7116) << "imapParser::parseResult - " << result << " " << option << " " << rest << endl;
        ulong value;
        if (parseOneNumber (rest, value))
          selectInfo.setUnseen (value);
      }
      else if (option == "UIDNEXT")
      {
//          kdDebug(7116) << "imapParser::parseResult - " << result << " " << option << " " << rest << endl;
        ulong value;
        if (parseOneNumber (rest, value))
          selectInfo.setUidNext (value);
      }
      else
      {
        kdDebug(7116) << "imapParser::parseResult - unknown response " << result << " " << option << " " << rest << endl;
      }
      break;

    }
    if (rest[0] == ']')
      rest = rest.right (rest.length () - 1); //tie off ]
    skipWS (rest);
  }
  QString action = command;
  if (command.isEmpty())
  {
    QString action = parseOneWord (rest);
    if (action == "UID")
      action = parseOneWord (rest);
  }

  switch (action[0].latin1 ())
  {
  case 'A':
    if (action == "AUTHENTICATE")
      if (result == "OK")
        currentState = ISTATE_LOGIN;
    break;

  case 'L':
    if (action == "LOGIN")
      if (result == "OK")
        currentState = ISTATE_LOGIN;
    break;

  case 'E':
    if (action == "EXAMINE")
    {
      uidCache.clear ();
      if (result == "OK")
        currentState = ISTATE_SELECT;
      else
      {
        if (currentState == ISTATE_SELECT)
          currentState = ISTATE_LOGIN;
        currentBox = QString::null;
      }
      kdDebug(7116) << "imapParser::parseResult - current box is now " << currentBox << endl;
    }
    break;

  case 'S':
    if (action == "SELECT")
    {
      uidCache.clear ();
      if (result == "OK")
        currentState = ISTATE_SELECT;
      else
      {
        if (currentState == ISTATE_SELECT)
          currentState = ISTATE_LOGIN;
        currentBox = QString::null;
      }
      kdDebug(7116) << "imapParser::parseResult - current box is now " << currentBox << endl;
    }
    break;

  default:
    break;
  }

}

void
imapParser::parseCapability (QString & result)
{
  imapCapabilities = QStringList::split (" ", result);
}

void
imapParser::parseFlags (QString & result)
{
//  kdDebug(7116) << "imapParser::parseFlags - " << result << endl;
  selectInfo.setFlags (result);
}

void
imapParser::parseList (QString & result)
{
//  kdDebug(7116) << "imapParser::parseList - " << result << endl;
  imapList this_one;

  if (result[0] != '(')
    return;                     //not proper format for us

  result = result.right (result.length () - 1); // tie off (

  //process the attributes
  QString attribute;

  while (!result.isEmpty () && result[0] != ')')
  {
    attribute = imapParser::parseOneWord (result);
    if (-1 != attribute.find ("\\Noinferiors", 0, false)  // FIXME: different
      || -1 != attribute.find ("\\HasNoChildren", 0, false)) // case required
      this_one.setNoInferiors (true);
    else if (-1 != attribute.find ("\\Noselect", 0, false))
      this_one.setNoSelect (true);
    else if (-1 != attribute.find ("\\Marked", 0, false))
      this_one.setMarked (true);
    else if (-1 != attribute.find ("\\Unmarked", 0, false))
      this_one.setUnmarked (true);
    else
      kdDebug(7116) << "imapParser::parseList - unknown attribute " << attribute << endl;
  }

  result = result.right (result.length () - 1); // tie off )
  imapParser::skipWS (result);

  this_one.setHierarchyDelimiter(imapParser::parseLiteral(result));
  this_one.setName (rfcDecoder::fromIMAP (imapParser::parseLiteral (result)));  // decode modified UTF7

  listResponses.append (this_one);
}

void
imapParser::parseLsub (QString & result)
{
  kdDebug(7116) << "imapParser::parseLsub - " << result << endl;
  imapList this_one (result);
  listResponses.append (this_one);
}

void
imapParser::parseSearch (QString & result)
{
//  kdDebug(7116) << "imapParser::parseSearch - " << result << endl;
  QString entry;
  ulong value;

  while (parseOneNumber (result, value))
  {
    lastResults.append (QString ().setNum (value));
  }
//  kdDebug(7116) << "imapParser::parseSearch - " << result << endl;
}

void
imapParser::parseStatus (QString & inWords)
{
  kdDebug(7116) << "imapParser::parseStatus - " << inWords << endl;
  lastStatus = imapInfo ();

  parseOneWord (inWords);       // swallow the box
  if (inWords[0] != '(')
    return;

  inWords = inWords.right (inWords.length () - 1);
  skipWS (inWords);

  while (!inWords.isEmpty () && inWords[0] != ')')
  {
    QString label;
    ulong value;

    label = parseOneWord (inWords);
    if (parseOneNumber (inWords, value))
    {
      if (label == "MESSAGES")
        lastStatus.setCount (value);
      else if (label == "RECENT")
        lastStatus.setRecent (value);
      else if (label == "UIDVALIDITY")
        lastStatus.setUidValidity (value);
      else if (label == "UNSEEN")
        lastStatus.setUnseen (value);
      else if (label == "UIDNEXT")
        lastStatus.setUidNext (value);
    }
  }

  if (inWords[0] == ')')
    inWords = inWords.right (inWords.length () - 1);
  skipWS (inWords);
}

void
imapParser::parseExists (ulong value, QString & result)
{
//  kdDebug(7116) << "imapParser::parseExists - [" << value << "d] " << result << endl;
  selectInfo.setCount (value);
  result = QString::null;
}

void
imapParser::parseExpunge (ulong value, QString & result)
{
  kdDebug(7116) << "imapParser::parseExpunge - [" << value << "d] " << result << endl;
}

QValueList < mailAddress > imapParser::parseAdressList (QString & inWords)
{
  QValueList < mailAddress > retVal;

//  kdDebug(7116) << "imapParser::parseAdressList - " << inWords << endl;
  if (inWords[0] != '(')
  {
    parseOneWord (inWords);     // parse NIL
  }
  else
  {
    inWords = inWords.right (inWords.length () - 1);
    skipWS (inWords);

    while (!inWords.isEmpty () && inWords[0] != ')')
    {
      if (inWords[0] == '(')
        retVal.append (parseAdress (inWords));
      else
        break;
    }

    if (inWords[0] == ')')
      inWords = inWords.right (inWords.length () - 1);
    skipWS (inWords);
  }

  return retVal;
}

mailAddress
imapParser::parseAdress (QString & inWords)
{
  QString user, host, full, comment;
  mailAddress retVal;

//  kdDebug(7116) << "imapParser::parseAdress - " << inWords << endl;
  if (inWords[0] != '(')
    return retVal;
  inWords = inWords.right (inWords.length () - 1);
  skipWS (inWords);

  full = parseLiteral (inWords);
  comment = parseLiteral (inWords);
  user = parseLiteral (inWords);
  host = parseLiteral (inWords);

  retVal.setFullNameRaw (full.ascii ());
  retVal.setCommentRaw (comment.ascii ());
  retVal.setUser (user.ascii ());
  retVal.setHost (host.ascii ());

//  kdDebug(7116) << "imapParser::parseAdress - '" << full << "' '" << comment << "' '" << user << "' '" << host << "'" << endl;
  if (inWords[0] == ')')
    inWords = inWords.right (inWords.length () - 1);
  skipWS (inWords);
//  kdDebug(7116) << "imapParser::parseAdress - " << inWords << endl;

  return retVal;
}

mailHeader *
imapParser::parseEnvelope (QString & inWords)
{
  mailHeader *envelope = NULL;
  QValueList < mailAddress > list;

//  kdDebug(7116) << "imapParser::parseEnvelope - " << inWords << endl;

  if (inWords[0] != '(')
    return envelope;
  inWords = inWords.right (inWords.length () - 1);
  skipWS (inWords);


  envelope = new mailHeader;
  kdDebug(7116) << "imapParser::parseEnvelope - creating " << envelope << endl;

  //date
  QString date = parseLiteral (inWords);
  envelope->setDate (date.ascii ());

  //subject
  QString subject = parseLiteral (inWords);
  envelope->setSubjectEncoded (subject.ascii ());

  //from
  list = parseAdressList (inWords);
  for (QValueListIterator < mailAddress > it = list.begin ();
       it != list.end (); ++it)
  {
    envelope->setFrom ((*it));
  }

  //sender
  list = parseAdressList (inWords);
  for (QValueListIterator < mailAddress > it = list.begin ();
       it != list.end (); ++it)
  {
    envelope->setSender ((*it));
  }

  //reply-to
  list = parseAdressList (inWords);
  for (QValueListIterator < mailAddress > it = list.begin ();
       it != list.end (); ++it)
  {
    envelope->setReplyTo ((*it));
  }

  //to
  list = parseAdressList (inWords);
  for (QValueListIterator < mailAddress > it = list.begin ();
       it != list.end (); ++it)
  {
    envelope->addTo ((*it));
  }

  //cc
  list = parseAdressList (inWords);
  for (QValueListIterator < mailAddress > it = list.begin ();
       it != list.end (); ++it)
  {
    envelope->addCC ((*it));
  }

  //bcc
  list = parseAdressList (inWords);
  for (QValueListIterator < mailAddress > it = list.begin ();
       it != list.end (); ++it)
  {
    envelope->addBCC ((*it));
  }

  //in-reply-to
  QString reply = parseLiteral (inWords);
  envelope->setInReplyTo (reply.ascii ());

  //message-id
  QString message = parseLiteral (inWords);
  envelope->setMessageId (message.ascii ());

  // see if we have more to come
  while (!inWords.isEmpty () && inWords[0] != ')')
  {
    //eat the extensions to this part
    if (inWords[0] == '(')
      parseSentence (inWords);
    else
      parseLiteral (inWords);
  }

  if (inWords[0] == ')')
    inWords = inWords.right (inWords.length () - 1);
  skipWS (inWords);

  return envelope;
//  kdDebug(7116) << "imapParser::parseEnvelope - " << inWords << endl;
}

// parse parameter pairs into a dictionary
// caller must clean up the dictionary items
QDict < QString > imapParser::parseDisposition (QString & inWords)
{
  QString
    disposition;
  QDict < QString > retVal (17, false);

  // return value is a shallow copy
  retVal.setAutoDelete (false);

  if (inWords[0] != '(')
  {
    //disposition only
    disposition = parseOneWord (inWords);
  }
  else
  {
    inWords = inWords.right (inWords.length () - 1);
    skipWS (inWords);

    //disposition
    disposition = parseOneWord (inWords);

    retVal = parseParameters (inWords);
    if (inWords[0] != ')')
      return retVal;
    inWords = inWords.right (inWords.length () - 1);
    skipWS (inWords);
  }

  if (!disposition.isEmpty ())
    retVal.insert ("content-disposition", new QString (disposition));

  return retVal;
}

// parse parameter pairs into a dictionary
// caller must clean up the dictionary items
QDict < QString > imapParser::parseParameters (QString & inWords)
{
  QDict < QString > retVal (17, false);

  // return value is a shallow copy
  retVal.setAutoDelete (false);

  if (inWords[0] != '(')
  {
    //better be NIL
    parseOneWord (inWords);
  }
  else
  {
    inWords = inWords.right (inWords.length () - 1);
    skipWS (inWords);

    while (!inWords.isEmpty () && inWords[0] != ')')
    {
      QString
        label,
        value;

      label = parseLiteral (inWords);
      value = parseLiteral (inWords);
      retVal.insert (label, new QString (value));
//      kdDebug(7116) << "imapParser::parseParameters - " << label << " = '" << value << "'" << endl;
    }

    if (inWords[0] != ')')
      return retVal;
    inWords = inWords.right (inWords.length () - 1);
    skipWS (inWords);
  }

  return retVal;
}

mimeHeader *
imapParser::parseSimplePart (QString & inWords, const QString & inSection)
{
  QString type, subtype, id, description, encoding;
  QDict < QString > parameters (17, false);
  mimeHeader *localPart = NULL;
  ulong size;

  parameters.setAutoDelete (true);

  if (inWords[0] != '(')
    return NULL;

  localPart = new mimeHeader;

  kdDebug(7116) << "imapParser::parseSimplePart - section " << inSection.ascii () << endl;

  localPart->setPartSpecifier (inSection);

  inWords = inWords.right (inWords.length () - 1);
  skipWS (inWords);

  //body type
  type = parseLiteral (inWords);

  //body subtype
  subtype = parseLiteral (inWords);

  localPart->setType (QCString (type.ascii ()) + "/" + subtype.ascii ());

  //body parameter parenthesized list
  parameters = parseParameters (inWords);
  {
    QDictIterator < QString > it (parameters);

    while (it.current ())
    {
      localPart->setTypeParm (it.currentKey ().ascii (), *(it.current ()));
      ++it;
    }
    parameters.clear ();
  }

  //body id
  id = parseLiteral (inWords);
  localPart->setID (id.latin1 ());

  //body description
  description = parseLiteral (inWords);

  //body encoding
  encoding = parseLiteral (inWords);
  localPart->setEncoding (encoding.ascii ());

  //body size
  if (parseOneNumber (inWords, size));
  localPart->setLength (size);

  // type specific extensions
  if (type.upper () == "MESSAGE" && subtype.upper () == "RFC822")
  {
//    kdDebug(7116) << "imapParse::parseSimplePart - parse up message " << inWords << endl;

    //envelope structure
    mailHeader *envelope = parseEnvelope (inWords);
    if (envelope)
      envelope->setPartSpecifier (inSection + ".0");

    //body structure
    parseBodyStructure (inWords, inSection, envelope);

    localPart->setNestedMessage (envelope);

    //text lines
    ulong lines;
    parseOneNumber (inWords, lines);
  }
  else
  {
    if (type.upper () == "TEXT")
    {
      //text lines
      ulong lines;
      parseOneNumber (inWords, lines);
    }

    // md5
    parseLiteral (inWords);

    // body disposition
    parameters = parseDisposition (inWords);
    {
      QDictIterator < QString > it (parameters);
      QString *disposition = parameters[QString ("content-disposition")];

      if (disposition)
        localPart->setDisposition (disposition->ascii ());
      parameters.remove (QString ("content-disposition"));
      while (it.current ())
      {
        localPart->setDispositionParm (it.currentKey ().ascii (),
                                       *(it.current ()));
        ++it;
      }
      parameters.clear ();
    }

    // body language
    parseSentence (inWords);
  }

  // see if we have more to come
  while (!inWords.isEmpty () && inWords[0] != ')')
  {
    //eat the extensions to this part
    if (inWords[0] == '(')
      parseSentence (inWords);
    else
      parseLiteral (inWords);
  }

//  kdDebug(7116) << "imapParser::parseSimplePart - " << type << "/" << subtype << " - " << encoding << endl;

  if (inWords[0] == ')')
    inWords = inWords.right (inWords.length () - 1);
  skipWS (inWords);

  return localPart;
}

mimeHeader *
imapParser::parseBodyStructure (QString & inWords, const QString & inSection,
                                mimeHeader * localPart)
{
  int section = 0;
  QString outSection;

  if (inWords[0] != '(')
  {
    // skip ""
    parseOneWord (inWords);
    return NULL;
  }
  inWords = inWords.right (inWords.length () - 1);
  skipWS (inWords);

  if (inWords[0].latin1 () == '(')
  {
    QString subtype;
    QDict < QString > parameters (17, false);
    parameters.setAutoDelete (true);
    if (!localPart)
      localPart = new mimeHeader;
    else
    {
      // might be filled from an earlier run
      localPart->clearNestedParts ();
      localPart->clearTypeParameters ();
      localPart->clearDispositionParameters ();
    }

    // is multipart
    while (inWords[0].latin1 () == '(')
    {
      section++;
      outSection.setNum (section);
      outSection = inSection + "." + outSection;
      mimeHeader *subpart = parseBodyStructure (inWords, outSection);
      localPart->addNestedPart (subpart);
    }

    // fetch subtype
    subtype = parseOneWord (inWords);
//    kdDebug(7116) << "imapParser::parseBodyStructure - multipart/" << subtype << endl;

    localPart->setType (("MULTIPART/" + subtype).ascii ());

    // fetch parameters
    parameters = parseParameters (inWords);
    {
      QDictIterator < QString > it (parameters);

      while (it.current ())
      {
        localPart->setTypeParm (it.currentKey ().ascii (), *(it.current ()));
        ++it;
      }
      parameters.clear ();
    }

    // body disposition
    parameters = parseDisposition (inWords);
    {
      QDictIterator < QString > it (parameters);
      QString *disposition = parameters[QString ("content-disposition")];

      if (disposition)
        localPart->setDisposition (disposition->ascii ());
      parameters.remove (QString ("content-disposition"));
      while (it.current ())
      {
        localPart->setDispositionParm (it.currentKey ().ascii (),
                                       *(it.current ()));
        ++it;
      }
      parameters.clear ();
    }

    // body language
    parseSentence (inWords);

  }
  else
  {
    // is simple part
    inWords = "(" + inWords;    //fake a sentence
    localPart = parseSimplePart (inWords, inSection);
    inWords = ")" + inWords;    //remove fake
  }

  // see if we have more to come
  while (!inWords.isEmpty () && inWords[0] != ')')
  {
    //eat the extensions to this part
    if (inWords[0] == '(')
      parseSentence (inWords);
    else
      parseLiteral (inWords);
  }

  if (inWords[0] == ')')
    inWords = inWords.right (inWords.length () - 1);
  skipWS (inWords);

  return localPart;
}

void
imapParser::parseBody (QString & inWords)
{
  // see if we got a part specifier
  if (inWords[0] == '[')
  {
    QString specifier;
    inWords = inWords.right (inWords.length () - 1);  // eat it

    specifier = parseOneWord (inWords);
    kdDebug(7116) << "imapParser::parseBody : specifier [" << specifier << "]" << endl;

    if (inWords[0] == '(')
    {
      QString label;
      inWords = inWords.right (inWords.length () - 1);  // eat it

      while (!inWords.isEmpty () && inWords[0] != ')')
      {
        label = parseOneWord (inWords);
        kdDebug(7116) << "imapParser::parseBody - mimeLabel : " << label << endl;
      }

      if (inWords[0] == ')')
        inWords = inWords.right (inWords.length () - 1);  // eat it
    }
    if (inWords[0] == ']')
      inWords = inWords.right (inWords.length () - 1);  // eat it
    skipWS (inWords);

    // parse the header
    if (specifier == "0")
    {
      mailHeader *envelope = NULL;
      imapCache *cache = uidCache[seenUid];
      if (cache)
        envelope = cache->getHeader ();

      if (!envelope || seenUid.isEmpty ())
      {
        kdDebug(7116) << "imapParser::parseBody - discarding " << envelope << " " << seenUid.ascii () << endl;
        // don't know where to put it, throw it away
        parseLiteral (inWords, true);
      }
      else
      {
        kdDebug(7116) << "imapParser::parseBody - reading " << envelope << " " << seenUid.ascii () << endl;
        // fill it up with data
        QString theHeader = parseLiteral (inWords, true);
        mimeIOQString myIO;

        myIO.setString (theHeader);
        envelope->parseHeader (myIO);

      }
      lastHandled = cache;
    }
    else
    {
      // throw it away
      parseLiteral (inWords, true);
    }

  }
  else
  {
    mailHeader *envelope = NULL;
    imapCache *cache = uidCache[seenUid];
    if (cache)
      envelope = cache->getHeader ();

    if (!envelope || seenUid.isEmpty ())
    {
      kdDebug(7116) << "imapParser::parseBody - discarding " << envelope << " " << seenUid.ascii () << endl;
      // don't know where to put it, throw it away
      parseSentence (inWords);
    }
    else
    {
      kdDebug(7116) << "imapParser::parseBody - reading " << envelope << " " << seenUid.ascii () << endl;
      // fill it up with data
      mimeHeader *body = parseBodyStructure (inWords, seenUid, envelope);
      if (body != envelope)
        delete body;
    }
    lastHandled = cache;
  }
}

void
imapParser::parseFetch (ulong value, QString & inWords)
{
//  kdDebug(7116) << "imapParser::parseFetch - [" << value << "d] " << inWords << endl;

  // just the id
  if (value);

  if (inWords[0] != '(')
    return;
  inWords = inWords.right (inWords.length () - 1);
  skipWS (inWords);

  lastHandled = NULL;

  while (!inWords.isEmpty () && inWords[0] != ')')
  {
    if (inWords[0] == '(')
      parseSentence (inWords);
    else
    {
      QString word = parseLiteral (inWords);
      switch (word[0].latin1 ())
      {
      case 'E':
        if (word == "ENVELOPE")
        {
          mailHeader *envelope = NULL;
          imapCache *cache = (seenUid.isEmpty()) ? preCache : uidCache[seenUid];

          if (cache)
            envelope = cache->getHeader ();

          kdDebug(7116) << "imapParser::parseFetch - got " << envelope << " from Cache for " << seenUid << endl;
          if (envelope && !envelope->getMessageId ().isEmpty ())
          {
            // we have seen this one already
            // or don't know where to put it
            kdDebug(7116) << "imapParser::parseFetch - discarding " << envelope << " " << seenUid << endl;
            parseSentence (inWords);
          }
          else
          {
            kdDebug(7116) << "imapParser::parseFetch - reading " << envelope << " " << seenUid  << endl;
            envelope = parseEnvelope (inWords);
            if (envelope)
            {
              envelope->setPartSpecifier (seenUid + ".0");
              cache->setHeader (envelope);
              cache->setUid (seenUid.toULong ());
            }
          }
          lastHandled = cache;
        }
        break;

      case 'B':
        if (word == "BODY")
        {
          parseBody (inWords);
        }
        else if (word == "BODYSTRUCTURE")
        {
          mailHeader *envelope = NULL;
          imapCache *cache = (seenUid.isEmpty()) ? preCache : uidCache[seenUid];

          if (cache)
            envelope = cache->getHeader ();

          if (!envelope)
          {
            kdDebug(7116) << "imapParser::parseFetch - discarding " << envelope << " " << seenUid.ascii () << endl;
            // don't know where to put it, throw it away
            parseSentence (inWords);
          }
          else
          {
            kdDebug(7116) << "imapParser::parseFetch - reading " << envelope << " " << seenUid.ascii () << endl;
            // fill it up with data
            mimeHeader *body =
              parseBodyStructure (inWords, seenUid, envelope);
            if (body != envelope)
              delete body;
          }
          lastHandled = cache;
        }
        break;

      case 'U':
        if (word == "UID")
        {
          seenUid = parseOneWord (inWords);
          mailHeader *envelope = NULL;
          imapCache *cache = uidCache[seenUid];
          if (cache)
            envelope = cache->getHeader ();

          if (envelope || seenUid.isEmpty ())
          {
            // we have seen this one already
            // or don't know where to put it
          }
          else
          {
            // fill up the cache
            if (preCache)
            {
              cache = preCache;
              preCache = NULL;
            } else {
              cache = new imapCache ();
            }
            cache->setUid (seenUid.toULong ());
            uidCache.replace (seenUid, cache);
          }
          if (envelope)
            envelope->setPartSpecifier (seenUid);
          lastHandled = cache;
        }
        break;

      case 'R':
        if (word == "RFC822.SIZE")
        {
          ulong size;
          imapCache *cache = (seenUid.isEmpty()) ? preCache : uidCache[seenUid];
          parseOneNumber (inWords, size);

          if (cache)
            cache->setSize (size);
          lastHandled = cache;
        }
        else if (word.find ("RFC822") == 0)
        {
          // might be RFC822 RFC822.TEXT RFC822.HEADER
          parseLiteral (inWords, true);
        }
        break;

      case 'I':
        if (word == "INTERNALDATE")
        {
          QString date;
          date = parseOneWord (inWords);
          imapCache *cache = (seenUid.isEmpty()) ? preCache : uidCache[seenUid];
          if (cache)
            cache->setDateStr (date);
          lastHandled = cache;
        }
        break;

      case 'F':
        if (word == "FLAGS")
        {
          imapCache *cache = (seenUid.isEmpty()) ? preCache : uidCache[seenUid];
          if (cache)
            cache->setFlags (imapInfo::_flags (inWords));
          else
            parseSentence (inWords);
          lastHandled = cache;
        }
        break;

      default:
        kdDebug(7116) << "imapParser::parseFetch - ignoring " << inWords << endl;
        parseLiteral (inWords);
        break;
      }
    }
  }

  // see if we have more to come
  while (!inWords.isEmpty () && inWords[0] != ')')
  {
    //eat the extensions to this part
    if (inWords[0] == '(')
      parseSentence (inWords);
    else
      parseLiteral (inWords);
  }

  if (inWords[0] != ')')
    return;
  inWords = inWords.right (inWords.length () - 1);
  skipWS (inWords);
}


// default parser
void
imapParser::parseSentence (QString & inWords)
{
  QString stack;
  bool first = true;

//  kdDebug(7116) << "imapParser::parseSentence - " << inWords << endl;
  //find the first nesting parentheses

  while (!inWords.isEmpty () && (!stack.isEmpty () || first))
  {
    first = false;
    skipWS (inWords);

    unsigned char ch = inWords[0].latin1 ();
    switch (ch)
    {
    case '(':
      inWords = inWords.right (inWords.length () - 1);
      stack += ')';
      break;
    case ')':
      inWords = inWords.right (inWords.length () - 1);
      stack = stack.left (stack.length () - 1);
      break;
    case '[':
      inWords = inWords.right (inWords.length () - 1);
      stack += ']';
      break;
    case ']':
      inWords = inWords.right (inWords.length () - 1);
      stack = stack.left (stack.length () - 1);
      break;
    default:
      parseLiteral (inWords);
      skipWS (inWords);
      break;
    }
  }
  skipWS (inWords);
}

void
imapParser::parseRecent (ulong value, QString & result)
{
//  kdDebug(7116) << "imapParser::parseRecent - [" << value << "d] " << result << endl;
  selectInfo.setRecent (value);
  result = QString::null;
}

int imapParser::parseLoop ()
{
  QString result;
  QByteArray readBuffer;

  if (!parseReadLine(readBuffer)) return -1;
  result = QString::fromLatin1 (readBuffer.data (), readBuffer.size ());

  if (result.isNull ())
    return 0;
  if (!sentQueue.count ())
  {
    // maybe greeting or BYE everything else SHOULD not happen, use NOOP or IDLE
    kdDebug(7116) << "imapParser::parseLoop - unhandledResponse: \n" << result << endl;
    unhandled << result;
  }
  else
  {
    imapCommand *current = sentQueue.at (0);

    switch (result[0].latin1 ())
    {
    case '*':
      result = result.left (result.length () - 2);  // tie off CRLF
      parseUntagged (result);
      break;
    case '+':
      kdDebug(7116) << "imapParser::parseLoop - continue: \n" << result << endl;
      continuation = result;
      break;
    default:
      {
        QString tag, resultCode;

        tag = parseLiteral (result);
        if (tag == current->id ())
        {
          result = result.left (result.length () - 2);  // tie off CRLF
          resultCode = parseLiteral (result); //the result
          current->setResult (resultCode);
          current->setComplete ();

          sentQueue.removeRef (current);
          completeQueue.append (current);
          kdDebug(7116) << "imapParser::parseLoop -  completed " << resultCode << ": " << result << endl;
          parseResult (resultCode, result, current->command());
        }
        else
        {
          kdDebug(7116) << "imapParser::parseLoop - unknown tag '" << tag << "'" << endl;
          result = tag + " " + result;
        }
      }
      break;
    }
  }

  return 1;
}

void
imapParser::parseRelay (const QByteArray & buffer)
{
  qWarning
    ("imapParser::parseRelay - virtual function not reimplemented - data lost");
  if (&buffer);
}

void
imapParser::parseRelay (ulong len)
{
  qWarning
    ("imapParser::parseRelay - virtual function not reimplemented - announcement lost");
  if (len);
}

bool
imapParser::parseRead (QByteArray & buffer, ulong len, ulong relay)
{
  ulong localRelay = relay;
  while (buffer.size () < len)
  {
    // beware of wrap around
    if (buffer.size () < relay)
      localRelay = relay - buffer.size ();
    else
      localRelay = 0;

//    kdDebug(7116) << "imapParser::parseRead - remaining " << relay-buffer.length() << "d" << endl;
    kdDebug(7116) << "got now : " << buffer.size () << " needing still : " << localRelay << "d" << endl;
    parseReadLine (buffer, localRelay);
  }
  return (len <= buffer.size ());
}

bool imapParser::parseReadLine (QByteArray & buffer, ulong relay)
{
  qWarning
    ("imapParser::parseReadLine - virtual function not reimplemented - no data read");
  if (&buffer && relay);
  return FALSE;
}

void
imapParser::parseWriteLine (const QString & str)
{
  qWarning
    ("imapParser::parseWriteLine - virtual function not reimplemented - no data written");
  if (&str);
}

void
imapParser::parseURL (const KURL & _url, QString & _box, QString & _section,
                      QString & _type, QString & _uid, QString & _validity)
{
  kdDebug(7116) << "imapParser::parseURL - " << endl;
  QStringList parameters;

  _box = _url.path ();
  parameters = QStringList::split (";", _box);  //split parameters
  if (parameters.count () > 0)  //assertion failure otherwise
    parameters.remove (parameters.begin ());  //strip path
  _box = _box.left (_box.find (';')); // strip parameters
  for (QStringList::ConstIterator it (parameters.begin ());
       it != parameters.end (); ++it)
  {
    QString temp = (*it);

    // if we have a '/' separator we'll just nuke it
    if (temp.find ("/") > 0)
      temp = temp.left (temp.find ("/"));
//    if(temp[temp.length()-1] == '/')
//      temp = temp.left(temp.length()-1);
    if (temp.find ("section=", 0, false) == 0)
      _section = temp.right (temp.length () - 8);
    else if (temp.find ("type=", 0, false) == 0)
      _type = temp.right (temp.length () - 5);
    else if (temp.find ("uid=", 0, false) == 0)
      _uid = temp.right (temp.length () - 4);
    else if (temp.find ("uidvalidity=", 0, false) == 0)
      _validity = temp.right (temp.length () - 12);
  }
//  kdDebug(7116) << "URL: section= " << _section << ", type= " << _type << ", uid= " << _uid << endl;
//  kdDebug(7116) << "URL: user() " << _url.user() << endl;
//  kdDebug(7116) << "URL: path() " << _url.path() << endl;
//  kdDebug(7116) << "URL: encodedPathAndQuery() " << _url.encodedPathAndQuery() << endl;

  if (!_box.isEmpty ())
  {
    if (_box[0] == '/')
      _box = _box.right (_box.length () - 1);
    if (!_box.isEmpty () && _box[_box.length () - 1] == '/')
      _box = _box.left (_box.length () - 1);
  }
  kdDebug(7116) << "URL: box= " << _box << ", section= " << _section << ", type= " << _type << ", uid= " << _uid << ", validity= " << _validity << endl;
}

void
imapParser::skipWS (QString & inWords)
{
  int i = 0;

  while (inWords[i] == ' ' || inWords[i] == '\t' || inWords[i] == '\r'
         || inWords[i] == '\n')
  {
    i++;
  }
  inWords = inWords.right (inWords.length () - i);
}

QString
imapParser::parseLiteral (QString & inWords, bool relay)
{
  QString retVal;

  if (inWords[0] == '{')
  {
    ulong runLen;
    QString strLen;

    runLen = inWords.find ('}', 1);
    if (runLen > 0)
    {
      bool proper;
      strLen = inWords.left (runLen);
      strLen = strLen.right (strLen.length () - 1);
      inWords = inWords.right (inWords.length () - runLen - 1);
      runLen = strLen.toULong (&proper);
      if (proper)
      {
        //now get the literal from the server
        QByteArray fill;

        if (relay)
          parseRelay (runLen);
        parseRead (fill, runLen, relay ? runLen : 0);
//        kdDebug(7116) << "requested " << runLen << "d and got " << fill.size() << endl;
//        kdDebug(7116) << "last bytes " << fill[runLen-4] << " " << fill[runLen-3] << " " << fill[runLen-2] << " " << fill[runLen-1] << endl;
        retVal = QString::fromLatin1 (fill.data (), runLen);  // our data
        QByteArray prefetch;
        parseReadLine (prefetch); // must get more
        inWords = QString::fromLatin1 (prefetch.data (), prefetch.size ());
//          kdDebug(7116) << "prefetched [" << inWords.length() << "] - '" << inWords << "'" << endl;
//        kdDebug(7116) << "requested " << runLen << "d and got " << fill.length() << endl;
//        inWords = inWords.left(inWords.length()-2); // tie off CRLF
//        kdDebug(7116) << "|\n|\nV" << endl;
//        kdDebug(7116) << retVal << "^" << endl;
//        kdDebug(7116) << "|\n|\n'" << inWords << "'" << endl;

        // no duplicate data transfers
        relay = false;
      }
      else
      {
        kdDebug(7116) << "imapParser::parseLiteral - error parsing {} - " << strLen << endl;
      }
    }
    else
    {
      inWords = "";
      kdDebug(7116) << "imapParser::parseLiteral - error parsing unmatched {" << endl;
    }
  }
  else
  {
    retVal = parseOneWord (inWords);
  }
  skipWS (inWords);
  return retVal;
}

// does not know about literals ( {7} literal )

QString
imapParser::parseOneWord (QString & inWords)
{
  QString retVal;

  if (inWords[0] == '"')
  {
    int i = 1;
    bool quote = FALSE;
    while (i < inWords.length() && (inWords[i] != '"' || quote))
    {
      if (inWords[i] == '\\') quote = !quote;
      else quote = FALSE;
      i++;
    }
    if (i < inWords.length())
    {
      retVal = inWords.left (i);
      retVal = retVal.right (retVal.length () - 1);
      for (int j = 0; j < retVal.length(); j++)
        if (retVal[j] == '\\') retVal.remove(j, 1);
      inWords = inWords.right (inWords.length () - i - 1);
    }
    else
    {
      kdDebug(7116) << "imapParser::parseOneWord - error parsing unmatched \"" << endl;
      retVal = inWords;
      inWords = "";
    }
  }
  else
  {
    int i, j;
    i = inWords.find (' ');
    if (i == -1)
      i = inWords.length ();
    j = inWords.find ('(');
    if (j < i && j != -1)
      i = j;
    j = inWords.find (')');
    if (j < i && j != -1)
      i = j;
    j = inWords.find ('[');
    if (j < i && j != -1)
      i = j;
    j = inWords.find (']');
    if (j < i && j != -1)
      i = j;
    j = inWords.find ('\r');
    if (j < i && j != -1)
      i = j;
    j = inWords.find ('\n');
    if (j < i && j != -1)
      i = j;
    j = inWords.find ('\t');
    if (j < i && j != -1)
      i = j;
    if (i != -1)
    {
      retVal = inWords.left (i);
      inWords = inWords.right (inWords.length () - i);
    }
    else
    {
      retVal = inWords;
      inWords = "";
    }
    if (retVal == "NIL")
      retVal = QString::null;
  }
  skipWS (inWords);
  return retVal;
}

bool
imapParser::parseOneNumber (QString & inWords, ulong & num)
{
  bool valid;
  num = parseOneWord (inWords).toULong (&valid);
  return valid;
}

bool
imapParser::hasCapability (const QString & cap)
{
//  kdDebug(7116) << "imapParser::hasCapability - Looking for '" << cap << "'" << endl;
  for (QStringList::Iterator it = imapCapabilities.begin ();
       it != imapCapabilities.end (); ++it)
  {
//    kdDebug(7116) << "imapParser::hasCapability - Examining '" << (*it) << "'" << endl;
    if (cap.lower () == (*it).lower ())
    {
      return true;
    }
  }
  return false;
}

