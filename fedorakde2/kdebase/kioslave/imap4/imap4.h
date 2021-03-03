#ifndef _IMAP4_H
#define _IMAP4_H "$Id: imap4.h,v 1.24 2001/06/04 18:47:07 haeckel Exp $"
/**********************************************************************
 *
 *   imap4.h  - IMAP4rev1 KIOSlave
 *   Copyright (C) 1999  John Corey
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
 *   Send comments and bug fixes to jcorey@fruity.ath.cx
 *
 *********************************************************************/

#include "imapparser.h"
#include "mimeio.h"

#include <kio/tcpslavebase.h>

#define IMAP_BUFFER 2048

enum IMAP_TYPE
{
  ITYPE_UNKNOWN,
  ITYPE_DIR,
  ITYPE_BOX,
  ITYPE_DIR_AND_BOX,
  ITYPE_MSG
};

class IMAP4Protocol:public
  KIO::TCPSlaveBase,
  public
  imapParser,
  public
  mimeIO
{

public:

  // reimplement the TCPSlave
  IMAP4Protocol (const QCString & pool, const QCString & app, bool isSSL);
  virtual ~IMAP4Protocol ();

  virtual void openConnection();
  virtual void closeConnection();

  virtual void setHost (const QString & _host, int _port, const QString & _user,
    const QString & _pass);
  virtual void get (const KURL & _url);
  virtual void stat (const KURL & _url);
  virtual void slave_status ();
  virtual void mimetype (const KURL & _url);
  virtual void del (const KURL & _url, bool isFile);
  /** Change the status. data = 'S' + URL + '\0' + Flags + '\0' 
   *  Copy a mail: data = 'C' + srcURL + '\0' + destURL + '\0' */
  virtual void special (const QByteArray & data);
  virtual void listDir (const KURL & _url);
  virtual void setSubURL (const KURL & _url);
  virtual void dispatch (int command, const QByteArray & data);
  virtual void mkdir (const KURL & url, int permissions);
  virtual void put (const KURL & url, int permissions, bool overwrite,
    bool resume);
  virtual void rename (const KURL & src, const KURL & dest, bool overwrite);
  virtual void copy (const KURL & src, const KURL & dest, int permissions,
    bool overwrite);

  // reimplement the parser
  // relay hook to send the fetched data directly to an upper level
  virtual void parseRelay (const QByteArray & buffer);

  // relay hook to announce the fetched data directly to an upper level
  virtual void parseRelay (ulong);

  // read at least len bytes
  virtual bool parseRead (QByteArray &buffer,ulong len,ulong relay=0);

  // read at least a line (up to CRLF)
  virtual bool parseReadLine (QByteArray & buffer, ulong relay = 0);

  // write argument to the server
  virtual void parseWriteLine (const QString &);

  // reimplement the mimeIO
  virtual int outputLine (const QCString & _str);

protected:

  // select or examine the box if needed
  bool assureBox (const QString & aBox, bool readonly);

  // our new ReadLine supports 0x00 within data
//  ssize_t ReadLine (char *data, ssize_t len);

  enum IMAP_TYPE
  parseURL (const KURL & _url, QString & _box, QString & _section,
            QString & _type, QString & _uid, QString & _validity,
            QString & _hierarchyDelimiter);
  QString getMimeType (enum IMAP_TYPE);

  bool makeLogin ();

  QString myHost, myUser, myPass, myAuth, myTLS;
  int myPort;

  bool relayEnabled;

  void outputLineStr (const QString & _str)
  {
    outputLine (_str.latin1 ());
  }
  void doListEntry (const KURL & _url, mailHeader * what, int stretch);
  void doListEntry (const KURL & _url, const QString & myBox,
                    const imapList & item);

  char readBuffer[IMAP_BUFFER];
  int readSize;
};

#endif
