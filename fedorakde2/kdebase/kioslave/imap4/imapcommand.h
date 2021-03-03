#ifndef _IMAPCOMMAND_H
#define _IMAPCOMMAND_H "$Id: imapcommand.h,v 1.6 2001/06/02 17:53:24 haeckel Exp $"
/**********************************************************************
 *
 *   imapcommand.h  - IMAP4rev1 command handler
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

class imapCommand
{
public:
  imapCommand ();
  imapCommand (const QString & command, const QString & parameter);
  bool isComplete ();
  const QString & result ();
  const QString & parameter ();
  const QString & command ();
  const QString & id ();

  void setId (const QString &);
  void setComplete ();
  void setResult (const QString &);
  void setCommand (const QString &);
  void setParameter (const QString &);
  const QString getStr ();

  static imapCommand *clientNoop ();
  static imapCommand *clientFetch (ulong uid, const QString & fields,
                                   bool nouid = false);
  static imapCommand *clientFetch (ulong fromUid, ulong toUid,
                                   const QString & fields, bool nouid =
                                   false);
  static imapCommand *clientFetch (const QString & sequence,
                                   const QString & fields, bool nouid =
                                   false);
  static imapCommand *clientList (const QString & reference,
                                  const QString & path, bool lsub = false);
  static imapCommand *clientSelect (const QString & path, bool examine =
                                    false);
  static imapCommand *clientClose();
  static imapCommand *clientStatus (const QString & path,
                                    const QString & parameters);
  static imapCommand *clientCopy (const QString & box,
                                  const QString & sequence, bool nouid =
                                  false);
  static imapCommand *clientAppend (const QString & box,
                                    const QString & extras, ulong size);
  static imapCommand *clientCreate (const QString & path);
  static imapCommand *clientDelete (const QString & path);
  static imapCommand *clientExpunge ();
  static imapCommand *clientRename (const QString & src,
                                    const QString & dest);
  static imapCommand *clientSearch (const QString & search, bool nouid =
                                    false);
  static imapCommand *clientStore (const QString & set, const QString & item,
                                   const QString & data, bool nouid = false);
  static imapCommand *clientLogout ();
  static imapCommand *clientStartTLS ();
protected:
    QString aCommand;
  QString mId;
  bool mComplete;
  QString aParameter;
  QString mResult;

private:
    imapCommand & operator = (const imapCommand &);
};

#endif
