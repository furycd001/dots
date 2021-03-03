#ifndef _IMAPINFO_H
#define _IMAPINFO_H "$Id: selectinfo.h,v 1.2 2001/02/28 10:51:35 haeckel Exp $"
/**********************************************************************
 *
 *   imapinfo.h  - IMAP4rev1 SELECT / EXAMINE handler
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

#include <qstringlist.h>
#include <qstring.h>

//class handling the info we get on EXAMINE and SELECT
class imapInfo
{
public:

  imapInfo ();
  imapInfo (const QStringList &);
    imapInfo (const imapInfo &);
    imapInfo & operator = (const imapInfo &);

  ulong _flags (const QString &) const;

  void setCount (ulong l)
  {
    countAvailable_ = true;
    count_ = l;
  }

  void setRecent (ulong l)
  {
    recentAvailable_ = true;
    recent_ = l;
  }

  void setUnseen (ulong l)
  {
    unseenAvailable_ = true;
    unseen_ = l;
  }

  void setUidValidity (ulong l)
  {
    uidValidityAvailable_ = true;
    uidValidity_ = l;
  }

  void setUidNext (ulong l)
  {
    uidNextAvailable_ = true;
    uidNext_ = l;
  }

  void setFlags (ulong l)
  {
    flagsAvailable_ = true;
    flags_ = l;
  }

  void setFlags (const QString & inFlag)
  {
    flagsAvailable_ = true;
    flags_ = _flags (inFlag);
  }

  void setPermanentFlags (ulong l)
  {
    permanentFlagsAvailable_ = true;
    permanentFlags_ = l;
  }

  void setPermanentFlags (const QString & inFlag)
  {
    permanentFlagsAvailable_ = true;
    permanentFlags_ = _flags (inFlag);
  }

  void setReadWrite (bool b)
  {
    readWriteAvailable_ = true;
    readWrite_ = b;
  }

  ulong count () const
  {
    return count_;
  }

  ulong recent () const
  {
    return recent_;
  }

  ulong unseen () const
  {
    return unseen_;
  }

  ulong uidValidity () const
  {
    return uidValidity_;
  }

  ulong uidNext () const
  {
    return uidNext_;
  }

  ulong flags () const
  {
    return flags_;
  }

  ulong permanentFlags () const
  {
    return permanentFlags_;
  }

  bool readWrite () const
  {
    return readWrite_;
  }

  ulong countAvailable () const
  {
    return countAvailable_;
  }

  ulong recentAvailable () const
  {
    return recentAvailable_;
  }

  ulong unseenAvailable () const
  {
    return unseenAvailable_;
  }

  ulong uidValidityAvailable () const
  {
    return uidValidityAvailable_;
  }

  ulong uidNextAvailable () const
  {
    return uidNextAvailable_;
  }

  ulong flagsAvailable () const
  {
    return flagsAvailable_;
  }

  ulong permanentFlagsAvailable () const
  {
    return permanentFlagsAvailable_;
  }

  bool readWriteAvailable () const
  {
    return readWriteAvailable_;
  }

private:

    ulong count_;
  ulong recent_;
  ulong unseen_;
  ulong uidValidity_;
  ulong uidNext_;
  ulong flags_;
  ulong permanentFlags_;
  bool readWrite_;

  bool countAvailable_;
  bool recentAvailable_;
  bool unseenAvailable_;
  bool uidValidityAvailable_;
  bool uidNextAvailable_;
  bool flagsAvailable_;
  bool permanentFlagsAvailable_;
  bool readWriteAvailable_;
};

#endif
