/*
 * Copyright (c) 1999,2000,2001 Alex Zepeda <jazepeda@pacbell.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Redistributions of source code or in binary form must consent to
 *    future terms and conditions as set forth by the founding author(s).
 *    The founding author is defined as the creator of following code, or
 *    lacking a clearly defined creator, the founding author is defined as
 *    the first person to claim copyright to, and contribute significantly
 *    to the following code.
 * 4. The following code may be used without explicit consent in any
 *    product provided the previous three conditions are met, and that
 *    the following source code be made available at no cost to consumers
 *    of mentioned product and the founding author as defined above upon
 *    request.  This condition may at any time be waived by means of 
 *    explicit written consent from the founding author.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: gopher.cc,v 1.20 2001/05/22 08:23:47 garbanzo Exp $
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/param.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include <qcstring.h>
#include <qglobal.h>

#include <kprotocolmanager.h>
#include <ksock.h>
#include <kdebug.h>
#include <kinstance.h>
#include <kio/connection.h>
#include <kio/slaveinterface.h>
#include <kio/passdlg.h>

#include <klocale.h>

#include "gopher.h"


using namespace KIO;

extern "C" {
	int kdemain(int argc, char **argv);
}

int kdemain(int argc, char **argv)
{
  KInstance instance( "kio_gopher" );
  if (argc != 4) {
    kdDebug() << " Usage: kio_gopher protocol domain-socket1 domain-socket2" << endl;
    exit(-1);
  }
  GopherProtocol slave(argv[2], argv[3]);
  slave.dispatchLoop();
  return 0;
}

GopherProtocol::GopherProtocol(const QCString &pool, const QCString &app)
  : TCPSlaveBase( 70, "gopher", pool, app)
{
  m_cmd = CMD_NONE;
  m_tTimeout.tv_sec=10;
  m_tTimeout.tv_usec=0;
}

GopherProtocol::~GopherProtocol()
{
        gopher_close();
}

void GopherProtocol::gopher_close ()
{
  CloseDescriptor();
}

bool GopherProtocol::gopher_open( const KURL &_url )
{
  ConnectToHost(m_sServer.ascii() /*check if ok*/, static_cast<int>(_url.port()));
  QString path=_url.path();
  if (path.at(0)=='/') path.remove(0,1);
  if (path.isEmpty()) {
    // We just want the initial listing
    if (Write("\r\n", 2) != 2) {
      error(ERR_COULD_NOT_CONNECT, _url.host());
      return false;
    }
  } else {
    path.remove(0,1); // Remove the type identifier
    // It should not be empty here
    if (path.isEmpty()) {
      error(ERR_MALFORMED_URL, _url.host());
      gopher_close();
      return false;
    }
    // Otherwise we should send our request
    if (Write(path.ascii(), strlen(path.ascii())) != static_cast<ssize_t>(strlen(path.ascii()))) {
      error(ERR_COULD_NOT_CONNECT, _url.host());
      gopher_close();
      return false;
    }
    if (Write("\r\n", 2) != 2) {
      error(ERR_COULD_NOT_CONNECT, _url.host());
      gopher_close();
      return false;
    }
  }
  return true;

}

void GopherProtocol::setHost( const QString & _host, int _port, const QString &_user, const QString &_pass)
{
  m_sServer = _host;
  m_iPort = _port;
  m_sUser = _user;
  m_sPass = _pass;
}

void GopherProtocol::stat (const KURL &url)
{
  kdDebug() << "STAT CALLZ" << endl;
  QString _path = url.path();
  if (_path.at(0) == '/')
    _path.remove(0,1);
  UDSEntry entry;
  UDSAtom atom;
  atom.m_uds = KIO::UDS_NAME;
  atom.m_str = _path;
  entry.append( atom );

  atom.m_uds = KIO::UDS_FILE_TYPE;
  if ((!url.hasPath()) || (url.path() == "/") || (_path.at(0) == '1') ) {
   kdDebug() << "Is a DIR" << endl;
   atom.m_long = S_IFDIR;
  } else {
   kdDebug() << "Is a FILE" << endl;
   atom.m_long = S_IFREG;
  }
  entry.append( atom );

      atom.m_uds = KIO::UDS_ACCESS;
      atom.m_long = S_IRUSR | S_IRGRP | S_IROTH; // readable by everybody
      entry.append( atom );

#if 0
      atom.m_uds = KIO::UDS_SIZE;
      atom.m_long = m_iSize;
      entry.append( atom );
#endif

  // TODO: maybe get the size of the message?
  statEntry( entry );
  finished();
}


void GopherProtocol::listDir( const KURL &dest )
{
  QString path = dest.path();
  if ( dest.isMalformed() ) {
    error( ERR_MALFORMED_URL, dest.url() );
    m_cmd = CMD_NONE;
    return;
  }

  if (dest.protocol() != "gopher") {
    error( ERR_INTERNAL, "kio_gopher got non gopher url" );
    m_cmd = CMD_NONE;
    return;
  }
  if (!gopher_open(dest)) {
    gopher_close();
    return;
  }
  if (path.at(0) == '/') path.remove(0,1);

  UDSEntry entry;
  UDSAtom atom;
  QString line;
  char buf[1024];
  bzero(buf, sizeof buf);
  UDSEntryList entries;
  int infoNum = 0;
  while (ReadLine(buf, sizeof(buf) - 1)) {
      buf[sizeof(buf) - 1] = 0;
    line = buf+1;
    if (strcmp(buf, ".\r\n")==0) {
    puts("dot done");
        break;
    }
    entry.clear();

    atom.m_uds = UDS_FILE_TYPE;
    atom.m_str = "";
    switch ((GopherType)buf[0]) {
    case GOPHER_INFO:
    case GOPHER_MENU:{
      atom.m_long = S_IFDIR;
      break;
    }
    default: {
      atom.m_long = S_IFREG;
    }
    }
    entry.append(atom);

    atom.m_uds = UDS_NAME;
    atom.m_long = 0;
    atom.m_str = line.mid(0,line.find("\t"));

    if (buf[0] == GOPHER_INFO) {
        QString prefix;
        prefix.sprintf("%03d ", ++infoNum);
        atom.m_str.prepend(prefix);
    }
    entry.append(atom);

    atom.m_uds = UDS_MIME_TYPE;
    atom.m_long = 0;
    switch ((GopherType)buf[0]) {
    case GOPHER_MENU:{
      atom.m_str="inode/directory";
      break;
    }
    case GOPHER_GIF:{
      atom.m_str="image/gif";
      break;
    }
    case GOPHER_INFO: // idk
    case GOPHER_TEXT:{
      atom.m_str="text/plain";
      break;
    }
    case GOPHER_HTML:{
      atom.m_str="text/html";
      break;
    }
    default: {
      atom.m_str="application/octet-stream";
      break;
    }
    }
    entry.append(atom);

    atom.m_uds = UDS_URL;
    KURL uds;
    uds.setProtocol("gopher");
    QString path("/");
    path.append(buf[0]);
    line.remove(0, line.find("\t")+1);
    path.append(line.mid(0,line.find("\t")));
    if (path == "//") path="/";
    uds.setPath(path);
    line.remove(0, line.find("\t")+1);
    uds.setHost(line.mid(0,line.find("\t")));
    line.remove(0, line.find("\t")+1);
    uds.setPort(line.mid(0,line.find("\t")).toUShort());
    atom.m_long = 0;
    atom.m_str = uds.url();
    entry.append(atom);

    atom.m_uds = UDS_SIZE;
    atom.m_str = QString::null;
    atom.m_long = 0;
    entry.append(atom);

    atom.m_uds = KIO::UDS_ACCESS;
    atom.m_long = S_IRUSR | S_IRGRP | S_IROTH; // readable by everybody
    entry.append( atom );

    entries.append(entry);
    entry.clear();
    bzero(buf, sizeof buf);
  }
  listEntries(entries);
  finished();
  return;
}

void GopherProtocol::get(const KURL &usrc)
{
  QByteArray array;
  QString path, cmd;
  //KURL usrc(_url);
  if ( usrc.isMalformed() ) {
    error( ERR_MALFORMED_URL, usrc.url() );
    m_cmd = CMD_NONE;
    return;
  }

  if (usrc.protocol() != "gopher") {
    error( ERR_INTERNAL, "kio_gopher got non gopher url" );
    m_cmd = CMD_NONE;
    return;
  }

  path = usrc.path();

  if (path == "/aboutme.txt") {
    mimeType("text/plain");
    array.setRawData(GopherProtocol::abouttext, strlen(GopherProtocol::abouttext));
    data(array);
    array.resetRawData(GopherProtocol::abouttext, strlen(GopherProtocol::abouttext));
    data(QByteArray());
    processedSize(strlen(GopherProtocol::abouttext));
    finished();
  }
  if (path.at(0)=='/') path.remove(0,1);
  if (path.isEmpty()) {
      kdDebug() << "We should be a dir!!" << endl;
    error(ERR_IS_DIRECTORY, usrc.url());
    m_cmd=CMD_NONE; return;
  }
  if (path.length() < 2) {
    error(ERR_MALFORMED_URL, usrc.url());
    return;
  }
  char type = path.ascii()[0];
  //fprintf(stderr,"Type is:");
  current_type=(GopherType)type;
  gopher_open(usrc);
  switch ((GopherType)type) {
  case GOPHER_GIF:  {
    if(!readRawData(usrc.url(), "image/gif")) {
      error(ERR_INTERNAL, "rawReadData failed");
      return;
    }
    break;
  }
  case GOPHER_HTML: {
    if (!readRawData(usrc.url(), "text/html")) {
      error(ERR_INTERNAL, "rawReadData failed");
      return;
    }
    break;
  }
  case GOPHER_UUENCODE: {
    if (!readRawData(usrc.url(), "text/plain")) {
      error(ERR_INTERNAL, "rawReadData failed");
      return;
    }
    break;
  }
  case GOPHER_IMAGE:
  case GOPHER_DOC:
  case GOPHER_SOUND:
  case GOPHER_BINARY:
  case GOPHER_PCBINARY: {
    if(!readRawData(usrc.url(), "application/octet-stream")) {
      error(ERR_INTERNAL, "rawReadData failed");
      return;
    }
    break;
  }
  case GOPHER_TEXT: {
    if(!readRawData(usrc.url(), "text/plain")) {
      error(ERR_INTERNAL, "rawReadData failed");
      return;
    }
    break;
  }
  default:
    break;
  }
}

bool GopherProtocol::readRawData(const QString &/*_url*/, const char *mimetype)
{
  QByteArray array;
  char buf[1024];
  mimeType(mimetype);
  ssize_t read_ret=0;
  size_t total_size=0;
  while ((read_ret=Read(buf, 1024))>0) {
      total_size+=read_ret;
      array.setRawData(buf, read_ret);
      data( array );
      array.resetRawData(buf, read_ret);
      totalSize(total_size);
  }
  processedSize(total_size);
  finished();
  gopher_close();
  finished();
  return true;
}

const char *GopherProtocol::abouttext=
"gopher  n.  1. Any of various short tailed, burrowing mammals of the\n"
"family Geomyidae, of North America.  2. (Amer. colloq.) Native or\n"
"inhabitant of Minnesota: the Gopher State.  3. (Amer. colloq.) One\n"
"who runs errands, does odd-jobs, fetches or delivers documents for\n"
"office staff.  4. (computer tech.) software following a simple\n"
"protocol for burrowing through a TCP/IP internet.";
