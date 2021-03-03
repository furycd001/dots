/*
 * Copyright (c) 2000, 2001 Alex Zepeda <jazepeda@pacbell.net>
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
 *	$Id: smtp.h,v 1.23 2001/06/29 06:52:34 haeckel Exp $
 */

#ifndef _SMTP_H
#define _SMTP_H "$Id: smtp.h,v 1.23 2001/06/29 06:52:34 haeckel Exp $"

#include <qstring.h>
#include <kio/tcpslavebase.h>

class KDESasl;

class SMTPProtocol
	: public KIO::TCPSlaveBase {
public:
	SMTPProtocol (const QCString &pool, const QCString &app, bool useSSL);
	virtual ~SMTPProtocol ();

	virtual void setHost (const QString &host, int port, const QString &user, const QString &pass);

	virtual void put (const KURL &url, int permissions, bool overwrite, bool resume);
	virtual void stat (const KURL &url);

protected:

	bool smtp_open (const KURL &u);
	void smtp_close ();
	bool command (const QString &buf, char *r_buf = NULL, unsigned int r_len = 0);
	int getResponse (char *real_buf = NULL, unsigned int real_len = 0);
	bool Authenticate (const KURL &url);
	void HandleSMTPWriteError (const KURL &url);
	void ParseFeatures (const char *buf);
	void PutRecipients (QStringList &list, const KURL &url);

	unsigned short m_iOldPort;
	bool opened, haveTLS;
	struct timeval m_tTimeout;
	QString m_sServer, m_sOldServer;
	QString m_sUser, m_sOldUser;
	QString m_sPass, m_sOldPass;
	QString m_sError;

	KDESasl *m_pSASL;
	QString m_sAuthConfig;
};

#endif
