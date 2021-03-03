// $Id: imapcommand.cc,v 1.9 2001/07/17 21:25:57 haeckel Exp $
/**********************************************************************
 *
 *   imapcommand.cc  - IMAP4rev1 command handler
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

#include "imapcommand.h"
#include "rfcdecoder.h"

/*#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <fcntl.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>

#include <qregexp.h>
#include <qbuffer.h>

#include <kprotocolmanager.h>
#include <ksock.h>
#include <kdebug.h>
#include <kinstance.h>
#include <kio/connection.h>
#include <kio/slaveinterface.h>
#include <kio/passdlg.h>
#include <klocale.h> */

imapCommand::imapCommand ()
{
  mComplete = false;
  mId = QString::null;
}

imapCommand::imapCommand (const QString & command, const QString & parameter)
//  aCommand(NULL),
//  mResult(NULL),
//  mParameter(NULL)
{
  mComplete = false;
  aCommand = command;
  aParameter = parameter;
  mId = QString::null;
}

bool
imapCommand::isComplete ()
{
  return mComplete;
}

const QString &
imapCommand::result ()
{
  return mResult;
}

const QString &
imapCommand::id ()
{
  return mId;
}

const QString &
imapCommand::parameter ()
{
  return aParameter;
}

const QString &
imapCommand::command ()
{
  return aCommand;
}

void
imapCommand::setId (const QString & id)
{
  if (mId.isEmpty ())
    mId = id;
}

void
imapCommand::setComplete ()
{
  mComplete = true;
}

void
imapCommand::setResult (const QString & result)
{
  mResult = result;
}

void
imapCommand::setCommand (const QString & command)
{
  aCommand = command;
}

void
imapCommand::setParameter (const QString & parameter)
{
  aParameter = parameter;
}

const QString
imapCommand::getStr ()
{
  if (parameter().isEmpty())
    return id() + " " + command() + "\r\n";
  else
    return id() + " " + command() + " " + parameter() + "\r\n";
}

imapCommand *
imapCommand::clientNoop ()
{
  return new imapCommand ("NOOP", "");
}

imapCommand *
imapCommand::clientFetch (ulong uid, const QString & fields, bool nouid)
{
  return clientFetch (uid, uid, fields, nouid);
}

imapCommand *
imapCommand::clientFetch (ulong fromUid, ulong toUid, const QString & fields,
                          bool nouid)
{
  QString uid;

  uid.setNum (fromUid);
  if (fromUid != toUid)
  {
    uid += ":";
    if (toUid < fromUid)
      uid += "*";
    else
      uid += QString ().setNum (toUid);
  }
  return clientFetch (uid, fields, nouid);
}

imapCommand *
imapCommand::clientFetch (const QString & sequence, const QString & fields,
                          bool nouid)
{
  return new imapCommand (nouid ? "FETCH" : "UID FETCH",
                          sequence + " (" + fields + ")");
}

imapCommand *
imapCommand::clientList (const QString & reference, const QString & path,
                         bool lsub)
{
  return new imapCommand (lsub ? "LSUB" : "LIST",
                          QString ("\"") + rfcDecoder::toIMAP (reference) +
                          "\" \"" + rfcDecoder::toIMAP (path) + "\"");
}

imapCommand *
imapCommand::clientSelect (const QString & path, bool examine)
{
  return new imapCommand ((examine && path != "INBOX") ? "EXAMINE" : "SELECT",
                          QString ("\"") + rfcDecoder::toIMAP (path) + "\"");
}

imapCommand *
imapCommand::clientClose()
{
  return new imapCommand("CLOSE", "");
}

imapCommand *
imapCommand::clientCopy (const QString & box, const QString & sequence,
                         bool nouid)
{
  return new imapCommand (nouid ? "COPY" : "UID COPY",
                          sequence + " \"" + rfcDecoder::toIMAP (box) + "\"");
}

imapCommand *
imapCommand::clientAppend (const QString & box, const QString &,
                           ulong size)
{
  return new imapCommand ("APPEND",
                          "\"" + rfcDecoder::toIMAP (box) + "\" {" +
                          QString ().setNum (size) + "}");
}

imapCommand *
imapCommand::clientStatus (const QString & path, const QString & parameters)
{
  return new imapCommand ("STATUS",
                          QString ("\"") + rfcDecoder::toIMAP (path) +
                          "\" (" + parameters + ")");
}

imapCommand *
imapCommand::clientCreate (const QString & path)
{
  return new imapCommand ("CREATE",
                          QString ("\"") + rfcDecoder::toIMAP (path) + "\"");
}

imapCommand *
imapCommand::clientDelete (const QString & path)
{
  return new imapCommand ("DELETE",
                          QString ("\"") + rfcDecoder::toIMAP (path) + "\"");
}

imapCommand *
imapCommand::clientExpunge ()
{
  return new imapCommand ("EXPUNGE", QString (""));
}

imapCommand *
imapCommand::clientRename (const QString & src, const QString & dest)
{
  return new imapCommand ("RENAME",
                          QString ("\"") + rfcDecoder::toIMAP (src) +
                          "\" \"" + rfcDecoder::toIMAP (dest) + "\"");
}

imapCommand *
imapCommand::clientSearch (const QString & search, bool nouid)
{
  return new imapCommand (nouid ? "SEARCH" : "UID SEARCH", search);
}

imapCommand *
imapCommand::clientStore (const QString & set, const QString & item,
                          const QString & data, bool nouid)
{
  return new imapCommand (nouid ? "STORE" : "UID STORE",
                          set + " " + item + " (" + data + ")");
}

imapCommand *
imapCommand::clientLogout ()
{
  return new imapCommand ("LOGOUT", "");
}

imapCommand *
imapCommand::clientStartTLS ()
{
  return new imapCommand ("STARTTLS", "");
}



