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
 * $Id: gopher.h,v 1.9 2001/05/22 08:23:47 garbanzo Exp $
 */

#ifndef _GOPHER_H
#define _GOPHER_H "$Id: gopher.h,v 1.9 2001/05/22 08:23:47 garbanzo Exp $"

#include <sys/types.h>
#include <sys/time.h>

#include <stdio.h>

#include <qstring.h>

#include <kio/tcpslavebase.h>

enum GopherType
{
  GOPHER_TEXT       = '0',
  GOPHER_MENU       = '1',
  GOPHER_PHONEBOOK  = '2',
  GOPHER_ERROR      = '3',
  GOPHER_BINHEX     = '4',
  GOPHER_PCBINARY   = '5',
  GOPHER_UUENCODE   = '6',
  GOPHER_INDEX      = '7',
  GOPHER_TELNET     = '8',
  GOPHER_BINARY     = '9',
  GOPHER_DUPLICATE  = '+',
  GOPHER_GIF        = 'g',
  GOPHER_IMAGE      = 'I',
  GOPHER_TN3270     = 'T',
  GOPHER_INFO       = 'i',
  GOPHER_HTML       = 'h',
  GOPHER_DOC        = 'd',
  GOPHER_SOUND      = 's',
  GOPHER_NONE       = '*'
};

class GopherProtocol : public KIO::TCPSlaveBase
{
public:
	GopherProtocol (const QCString &pool, const QCString &app );
	virtual ~GopherProtocol ();

	virtual void get (const KURL &);
	virtual void stat (const KURL &url);
	//virtual void del (const KURL& url, bool isfile);
	virtual void listDir (const KURL& url);

	void setHost (const QString &host, int port, const QString &user, const QString &pass);

protected:

	bool readRawData(const QString &_url, const char *mimetype=0);

	/**
	  *  Attempt to properly shut down the Gopher connection.
	*/
	void gopher_close ();

	/**
	  * Attempt to initiate a Gopher connection via a TCP socket.  If no
	  * port is passed, port 70 is assumed.
	*/
	bool gopher_open (const KURL &_url);

	QString m_sServer, m_sUser, m_sPass;
	int m_cmd;
	struct timeval m_tTimeout;
	GopherType current_type;
	static const char *abouttext;
};

#endif
