/*
 * Copyright (c) 1999,2000 Alex Zepeda
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
 *	$Id: pop3.h,v 1.36 2001/06/21 18:07:42 granroth Exp $
 */

#ifndef _POP3_H
#define _POP3_H "$Id: pop3.h,v 1.36 2001/06/21 18:07:42 granroth Exp $"

#include <sys/types.h>
#include <sys/time.h>

#include <stdio.h>

#include <qstring.h>

#include <kio/tcpslavebase.h>

class POP3Protocol 
	: public KIO::TCPSlaveBase
{
public:
	POP3Protocol (const QCString &pool, const QCString &app, bool SSL);
	virtual ~POP3Protocol ();

	virtual void setHost (const QString& host, int port, const QString& user, const QString& pass);

	virtual void get (const KURL& url);
	virtual void stat (const KURL& url);
	virtual void del (const KURL &url, bool isfile);
	virtual void listDir (const KURL &url);

protected:

	/**
	  * This returns the size of a message as a long integer.
	  * This is useful as an internal member, because the "other"
	  * getSize command will emit a signal, which would be harder
	  * to trap when doing something like listing a directory.
	  */
	size_t realGetSize (unsigned int msg_num);

	/**
	  *  Send a command to the server, and wait for the  one-line-status
	  *  reply via getResponse.  Similar rules apply.  If no buffer is
	  *  specified, no data is passed back.
	  */
	bool command (const char *buf, char *r_buf=0, unsigned int r_len=0);

	/**
	  *  All POP3 commands will generate a response.  Each response will
	  *  either be prefixed with a "+OK " or a "-ERR ".  The getResponse
	  *  fucntion will wait until there's data to be read, and then read in
	  *  the first line (the response), and copy the response sans +OK/-ERR
	  *  into a buffer (up to len bytes) if one was passed to it.  It will
	  *  return true if the response was affirmitave, or false otherwise.
	  */
	bool getResponse (char *buf, unsigned int len, const char *command);

	/** Call int pop3_open() and report an error, if if fails */
        void openConnection ();

	/**
	  *  Attempt to properly shut down the POP3 connection by sending
	  *  "QUIT\r\n" before closing the socket.
	  */
	void closeConnection ();

	/**
	  * Attempt to initiate a POP3 connection via a TCP socket.  If no port
	  * is passed, port 110 is assumed, if no user || password is
	  * specified, the user is prompted for them.
	  */
	bool pop3_open ();

	const QCString encodeRFC2104 (const QCString & text, const QCString & key);

	int m_cmd;
	unsigned short int m_iOldPort;
	struct timeval m_tTimeout;
	QString m_sOldServer, m_sOldPass, m_sOldUser;
	QString m_sServer, m_sPass, m_sUser;
	bool m_try_apop, m_try_sasl, opened;
	QString m_sError;
};

#endif
