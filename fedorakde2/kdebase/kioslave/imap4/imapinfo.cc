// $Id: imapinfo.cc,v 1.5 2001/03/21 23:30:03 waba Exp $
/**********************************************************************
 *
 *   imapinfo.cc  - IMAP4rev1 SELECT / EXAMINE handler
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

#include "imapinfo.h"
#include "imapparser.h"

#include <kdebug.h>

imapInfo::imapInfo ():count_ (0),
recent_ (0),
unseen_ (0),
uidValidity_ (0),
uidNext_ (0),
flags_ (0),
permanentFlags_ (0),
readWrite_ (false),
countAvailable_ (false),
recentAvailable_ (false),
unseenAvailable_ (false),
uidValidityAvailable_ (false),
uidNextAvailable_ (false),
flagsAvailable_ (false),
permanentFlagsAvailable_ (false), readWriteAvailable_ (false)
{
}

imapInfo::imapInfo (const imapInfo & mi):count_ (mi.count_),
recent_ (mi.recent_),
unseen_ (mi.unseen_),
uidValidity_ (mi.uidValidity_),
uidNext_ (mi.uidNext_),
flags_ (mi.flags_),
permanentFlags_ (mi.permanentFlags_),
readWrite_ (mi.readWrite_),
countAvailable_ (mi.countAvailable_),
recentAvailable_ (mi.recentAvailable_),
unseenAvailable_ (mi.unseenAvailable_),
uidValidityAvailable_ (mi.uidValidityAvailable_),
uidNextAvailable_ (mi.uidNextAvailable_),
flagsAvailable_ (mi.flagsAvailable_),
permanentFlagsAvailable_ (mi.permanentFlagsAvailable_),
readWriteAvailable_ (mi.readWriteAvailable_)
{
}

imapInfo & imapInfo::operator = (const imapInfo & mi)
{
  // Avoid a = a.
  if (this == &mi)
    return *this;

  count_ = mi.count_;
  recent_ = mi.recent_;
  unseen_ = mi.unseen_;
  uidValidity_ = mi.uidValidity_;
  uidNext_ = mi.uidNext_;
  flags_ = mi.flags_;
  permanentFlags_ = mi.permanentFlags_;
  readWrite_ = mi.readWrite_;
  countAvailable_ = mi.countAvailable_;
  recentAvailable_ = mi.recentAvailable_;
  unseenAvailable_ = mi.unseenAvailable_;
  uidValidityAvailable_ = mi.uidValidityAvailable_;
  uidNextAvailable_ = mi.uidNextAvailable_;
  flagsAvailable_ = mi.flagsAvailable_;
  permanentFlagsAvailable_ = mi.permanentFlagsAvailable_;
  readWriteAvailable_ = mi.readWriteAvailable_;

  return *this;
}

imapInfo::imapInfo (const QStringList & list):count_ (0),
recent_ (0),
unseen_ (0),
uidValidity_ (0),
uidNext_ (0),
flags_ (0),
permanentFlags_ (0),
readWrite_ (false),
countAvailable_ (false),
recentAvailable_ (false),
unseenAvailable_ (false),
uidValidityAvailable_ (false),
uidNextAvailable_ (false),
flagsAvailable_ (false),
permanentFlagsAvailable_ (false), readWriteAvailable_ (false)
{
  for (QStringList::ConstIterator it (list.begin ()); it != list.end (); ++it)
  {
    QString line (*it);

    line = line.left (line.length () - 2);
    QStringList
    tokens (QStringList::split (' ', line));

    kdDebug(7116) << "Processing: " << line << endl;
    if (tokens[0] != "*")
      continue;

    if (tokens[1] == "OK")
    {
      if (tokens[2] == "[UNSEEN")
        setUnseen (tokens[3].left (tokens[3].length () - 1).toULong ());

      else if (tokens[2] == "[UIDVALIDITY")
        setUidValidity (tokens[3].left (tokens[3].length () - 1).toULong ());

      else if (tokens[2] == "[UIDNEXT")
        setUidNext (tokens[3].left (tokens[3].length () - 1).toULong ());

      else if (tokens[2] == "[PERMANENTFLAGS")
      {
        int
          flagsStart =
          line.
          find ('(');
        int
          flagsEnd =
          line.
          find (')');

        kdDebug(7116) << "Checking permFlags from " << flagsStart << " to " << flagsEnd << endl;
        if ((-1 != flagsStart) && (-1 != flagsEnd) && flagsStart < flagsEnd)
          setPermanentFlags (_flags (line.mid (flagsStart, flagsEnd)));

      }
      else if (tokens[2] == "[READ-WRITE")
      {
        setReadWrite (true);
      }
      else if (tokens[2] == "[READ-ONLY")
      {
        setReadWrite (false);
      }
      else
      {
        kdDebug(7116) << "unknown token2: " << tokens[2] << endl;
      }
    }
    else if (tokens[1] == "FLAGS")
    {
      int
        flagsStart =
        line.
        find ('(');
      int
        flagsEnd =
        line.
        find (')');

      if ((-1 != flagsStart) && (-1 != flagsEnd) && flagsStart < flagsEnd)
        setFlags (_flags (line.mid (flagsStart, flagsEnd)));
    }
    else
    {
      if (tokens[2] == "EXISTS")
        setCount (tokens[1].toULong ());

      else if (tokens[2] == "RECENT")
        setRecent (tokens[1].toULong ());

      else
        kdDebug(7116) << "unknown token1/2: " << tokens[1] << " " << tokens[2] << endl;
    }
  }

}

ulong imapInfo::_flags (const QString & flagsString)
{
  QString
    ignore;

  return _flags (flagsString, ignore);
}

ulong imapInfo::_flags (const QString & inFlags, QString & userflags)
{
  ulong
    flags =
    0;
  QString
    entry;
  QString
    flagsString =
    inFlags;
  userflags = "";

  if (flagsString[0] == '(')
    flagsString = flagsString.right (flagsString.length () - 1);

  while (!flagsString.isEmpty () && flagsString[0] != ')')
  {
    entry = imapParser::parseOneWord (flagsString);

    if (0 != entry.contains ("\\Seen", false))
      flags ^= Seen;
    else if (0 != entry.contains ("\\Answered", false))
      flags ^= Answered;
    else if (0 != entry.contains ("\\Flagged", false))
      flags ^= Flagged;
    else if (0 != entry.contains ("\\Deleted", false))
      flags ^= Deleted;
    else if (0 != entry.contains ("\\Draft", false))
      flags ^= Draft;
    else if (0 != entry.contains ("\\Recent", false))
      flags ^= Recent;
    else if (0 != entry.contains ("\\*", false))
      flags ^= User;
    else if (entry.isEmpty ())
      flagsString = "";
    else
      userflags += entry + " ";
  }

  kdDebug(7116) << "imapInfo::_flags - " << inFlags << " -> " << flags << "d, " << userflags << endl;

  return flags;
}
