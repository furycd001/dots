#ifndef _IMAPLIST_H
#define _IMAPLIST_H "$Id: imaplist.h,v 1.2 2001/02/28 10:51:35 haeckel Exp $"
/**********************************************************************
 *
 *   imaplist.h  - IMAP4rev1 list response handler
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

//the class handling the responses from list
class imapList
{
public:

  imapList ();
  imapList (const QString &);
  imapList (const imapList &);
    imapList & operator = (const imapList &);

  QString hierarchyDelimiter () const
  {
    return hierarchyDelimiter_;
  }
  void setHierarchyDelimiter (const QString & _str)
  {
    hierarchyDelimiter_ = _str;
  }

  QString name () const
  {
    return name_;
  }
  void setName (const QString & _str)
  {
    name_ = _str;
  }

  bool noInferiors () const
  {
    return noInferiors_;
  }
  void setNoInferiors (bool _val)
  {
    noInferiors_ = _val;
  }

  bool noSelect () const
  {
    return noSelect_;
  }
  void setNoSelect (bool _val)
  {
    noSelect_ = _val;
  }

  bool marked () const
  {
    return marked_;
  }
  void setMarked (bool _val)
  {
    marked_ = _val;
  }

  bool unmarked () const
  {
    return unmarked_;
  }
  void setUnmarked (bool _val)
  {
    unmarked_ = _val;
  }

private:

  QString hierarchyDelimiter_;
  QString name_;
  bool noInferiors_;
  bool noSelect_;
  bool marked_;
  bool unmarked_;
};

#endif
