// $Id: imaplist.cc,v 1.3 2001/03/21 23:30:03 waba Exp $
/**********************************************************************
 *
 *   imapinfo.cc  - IMAP4rev1 EXAMINE / SELECT handler
 *   Copyright (C) 2000
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
 *   Send comments and bug fixes to
 *
 *********************************************************************/

/*
  References:
    RFC 2060 - Internet Message Access Protocol - Version 4rev1 - December 1996
    RFC 2192 - IMAP URL Scheme - September 1997
    RFC 1731 - IMAP Authentication Mechanisms - December 1994
               (Discusses KERBEROSv4, GSSAPI, and S/Key)
    RFC 2195 - IMAP/POP AUTHorize Extension for Simple Challenge/Response
             - September 1997 (CRAM-MD5 authentication method)
    RFC 2104 - HMAC: Keyed-Hashing for Message Authentication - February 1997

  Supported URLs:
    imap://server/ - Prompt for user/pass, list all folders in home directory
    imap://user:pass@server/ - Uses LOGIN to log in
    imap://user;AUTH=method:pass@server/ - Uses AUTHENTICATE to log in

    imap://server/folder/ - List messages in folder
 */

#include "rfcdecoder.h"
#include "imaplist.h"
#include "imapparser.h"

#include <kdebug.h>

imapList::imapList ():noInferiors_ (false),
noSelect_ (false), marked_ (false), unmarked_ (false)
{
}

imapList::imapList (const imapList & lr):hierarchyDelimiter_ (lr.hierarchyDelimiter_),
name_ (lr.name_),
noInferiors_ (lr.noInferiors_),
noSelect_ (lr.noSelect_), marked_ (lr.marked_), unmarked_ (lr.unmarked_)
{
}

imapList & imapList::operator = (const imapList & lr)
{
  // Avoid a = a.
  if (this == &lr)
    return *this;

  hierarchyDelimiter_ = lr.hierarchyDelimiter_;
  name_ = lr.name_;
  noInferiors_ = lr.noInferiors_;
  noSelect_ = lr.noSelect_;
  marked_ = lr.marked_;
  unmarked_ = lr.unmarked_;

  return *this;
}

imapList::imapList (const QString & inStr):noInferiors_ (false),
noSelect_ (false),
marked_ (false), unmarked_ (false)
{
  QString
    s =
    inStr;

  if (s[0] != '(')
    return;                     //not proper format for us

  s = s.right (s.length () - 1);  // tie off (

  //process the attributes
  QString
    attribute;

  while (!s.isEmpty () && s[0] != ')')
  {
    attribute = imapParser::parseOneWord (s);
    if (-1 != attribute.find ("\\Noinferiors", 0, false))
      noInferiors_ = true;
    else if (-1 != attribute.find ("\\Noselect", 0, false))
      noSelect_ = true;
    else if (-1 != attribute.find ("\\Marked", 0, false))
      marked_ = true;
    else if (-1 != attribute.find ("\\Unmarked", 0, false))
      unmarked_ = true;
    else
      kdDebug(7116) << "imapList::imapList: bogus attribute " << attribute << endl;
  }

  s = s.right (s.length () - 1);  // tie off )
  imapParser::skipWS (s);

  hierarchyDelimiter_ = imapParser::parseOneWord (s);
  if (hierarchyDelimiter_ == "NIL")
    hierarchyDelimiter_ = QString::null;
  name_ = rfcDecoder::fromIMAP (imapParser::parseOneWord (s));  // decode modified UTF7
}
