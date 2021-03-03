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
 *	$Id: smtp.cc,v 1.66 2001/06/29 06:52:34 haeckel Exp $
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#include <netinet/in.h>
#include <arpa/inet.h>

#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include <qbuffer.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qcstring.h>
#include <qglobal.h>

#include <kprotocolmanager.h>
#include <kemailsettings.h>
#include <ksock.h>
#include <kdebug.h>
#include <kinstance.h>
#include <kio/connection.h>
#include <kio/slaveinterface.h>
#include <kio/kdesasl.h>
#include <klocale.h>
#include <iostream>

#include "smtp.h"

#define ASCII(x) QString::fromLatin1(x)
#define WRITE_STRING(x) Write (static_cast<const char *>(x.local8Bit()), x.local8Bit().length())

#define DEFAULT_EMAIL "someuser@is.using.a.pre.release.kde.ioslave.compliments.of.kde.org"
#define DEFAULT_RESPONSE_BUFFER 512
#define DEFAULT_EHLO_BUFFER 5120

using namespace KIO;

extern "C" {
	int kdemain (int argc, char **argv);
}

void	GetAddresses (const QString &str, const QString &delim, QStringList &list);
int	GetVal (char *buf);

int kdemain (int argc, char **argv)
{
	KInstance instance("kio_smtp");

	if (argc != 4) {
		fprintf(stderr, "Usage: kio_smtp protocol domain-socket1 domain-socket2\n");
		exit(-1);
	}

        bool useSSL = (strcmp(argv[1], "smtps") == 0) ? true : false;
	// We might as well allocate it on the heap.  Since there's a heap o room there..
	SMTPProtocol *slave = new SMTPProtocol(argv[2], argv[3], useSSL);
	slave->dispatchLoop();

	delete slave;
	return 0;
}


SMTPProtocol::SMTPProtocol (const QCString &pool, const QCString &app, bool useSSL)
	: TCPSlaveBase (useSSL ? 465 : 25, useSSL ? "smtps" : "smtp", pool, app, useSSL)
{
	kdDebug() << "SMTPProtocol::SMTPProtocol" << endl;
	opened = false;
	haveTLS = false;
	m_iSock = 0;
	m_iOldPort = 0;
	m_sOldServer = QString::null;
	m_tTimeout.tv_sec = 10;
	m_tTimeout.tv_usec = 0;

	// Auth stuff
	m_pSASL = 0;
	m_sAuthConfig = QString::null;
}


SMTPProtocol::~SMTPProtocol ()
{
	kdDebug() << "SMTPProtocol::~SMTPProtocol" << endl;
	smtp_close();
}

void SMTPProtocol::HandleSMTPWriteError (const KURL &url)
{
	// Attempt to save face and call RSET, and if that works
	// try and shut down nicely.
	if (!command( ASCII("RSET") )) {
		error(ERR_SERVICE_NOT_AVAILABLE, i18n("RSET failed %1").arg(url.path()));
	} else {
		smtp_close();
		error(ERR_COULD_NOT_WRITE, i18n("RSET succeeded %1").arg(url.path()));
	}
}

// Usage: smtp://smtphost:port/send?to=user@host.com&subject=blah
// If smtphost is the name of a profile, it'll use the information 
// provided by that profile.  If it's not a profile name, it'll use it as
// nature intended.
// One can also specify in the query:
// to=emailaddress
// cc=emailaddress
// bcc=emailaddress
// subject=text
// profile=text (this will override the "host" setting
void SMTPProtocol::put (const KURL &url, int /*permissions*/, bool /*overwrite*/, bool /*resume*/)
{
	QString query = url.query();
	QString subject = ASCII("missing subject");
	QString profile = QString::null;
        QString from = QString::null;
	QStringList recip, bcc, cc, temp_list;

	GetAddresses(query, ASCII("to="), recip);
	GetAddresses(query, ASCII("cc="), cc);
	GetAddresses(query, ASCII("bcc="), bcc);

	// find the subject
	GetAddresses(query, ASCII("subject="), temp_list);
	if (temp_list.count()) {
		subject = temp_list.last();
		temp_list.clear();
	}

	GetAddresses(query, ASCII("from="), temp_list);
	if (temp_list.count()) {
		from = temp_list.last();
		temp_list.clear();
	}

	GetAddresses(query, ASCII("profile="), temp_list);
	if (temp_list.count()) {
		profile = temp_list.last();
		temp_list.clear();
	}

	KEMailSettings *mset = new KEMailSettings;
	KURL open_url = url;
	if (profile == QString::null) {
	kdDebug() << "kio_smtp: Profile is null" << endl;
		QStringList profiles = mset->profiles(); bool hasProfile=false;
		for (QStringList::Iterator it=profiles.begin(); it != profiles.end(); ++it) {
			if ((*it) == open_url.host())
				hasProfile=true;
		}
		if (hasProfile) {
			mset->setProfile(open_url.host());
			open_url.setHost(mset->getSetting(KEMailSettings::OutServer));
			m_sUser = mset->getSetting(KEMailSettings::OutServerLogin);
			m_sPass = mset->getSetting(KEMailSettings::OutServerPass);

			if (m_sUser.isEmpty())
				m_sUser = QString::null;
			if (m_sPass.isEmpty())
				m_sPass = QString::null;
			open_url.setUser(m_sUser);
			open_url.setPass(m_sPass);
			m_sServer = open_url.host();
			m_iPort = open_url.port();
		} else {
			mset->setProfile(mset->defaultProfileName());
		}
	} else {
		mset->setProfile(profile);
	}

	// Check KEMailSettings to see if we've specified an E-Mail address
	// if that worked, check to see if we've specified a real name
	// and then format accordingly (either: emailaddress@host.com or
	// Real Name <emailaddress@host.com>)
	if (from.isEmpty())
	{
		if (mset->getSetting(KEMailSettings::EmailAddress) !=
		  QString::null) {
			from = mset->getSetting(KEMailSettings::EmailAddress);
		} else {
			from = ASCII(DEFAULT_EMAIL);
		}
	}
	from.prepend(ASCII("MAIL FROM: "));

	if (!smtp_open(open_url))
	        error(ERR_SERVICE_NOT_AVAILABLE, i18n("SMTPProtocol::smtp_open failed (%1)").arg(open_url.path()));

	if (!command(from)) {
		HandleSMTPWriteError(open_url);
		return;
	}

	// Loop through our To and CC recipients, and send the proper
	// SMTP commands, for the benefit of the server.
	PutRecipients(recip, open_url);	// To
	PutRecipients(cc, open_url);	// Carbon Copy (CC)
	PutRecipients(bcc, open_url);	// Blind Carbon Copy (BCC)

	// Begin sending the actual message contents (most headers+body)
	if (!command(ASCII("DATA"))) {
		HandleSMTPWriteError(open_url);
	}

	if (mset->getSetting(KEMailSettings::EmailAddress) != QString::null) {
		if (mset->getSetting(KEMailSettings::RealName) != QString::null) {
			from = ASCII("From: %1 <%2>\r\n").arg(mset->getSetting(KEMailSettings::RealName)).arg(mset->getSetting(KEMailSettings::EmailAddress));
		} else {
			from = ASCII("From: %1\r\n").arg(mset->getSetting(KEMailSettings::EmailAddress));
		}
	} else {
		from = ASCII("From: %1\r\n").arg(DEFAULT_EMAIL);
	}
	WRITE_STRING(from);

	subject = ASCII("Subject: %1\r\n").arg(subject);
	WRITE_STRING(subject);

	// Write out the To header for the benefit of the mail clients
	query = ASCII("To: %1\r\n");
	for ( QStringList::Iterator it = recip.begin(); it != recip.end(); ++it ) {
		query = query.arg(*it);
		WRITE_STRING(query);
	}

	// Write out the CC header for the benefit of the mail clients
	query = ASCII("CC: %1\r\n");
	for ( QStringList::Iterator it = cc.begin(); it != cc.end(); ++it ) {
		query = query.arg(*it);
		WRITE_STRING(query);
	}

	delete mset;

	// Loop until we got 0 (end of data)
	int result;
	QByteArray buffer;

	do {
		dataReq(); // Request for data
		buffer.resize(0);
		result = readData(buffer);
		if (result > 0) {
			Write(buffer.data(), buffer.size());
		} else if (result < 0) {
			error(ERR_COULD_NOT_WRITE, open_url.path());
		}
	} while (result > 0);

	Write("\r\n.\r\n", 5);
	command(ASCII("RSET"));
	finished();
}

void SMTPProtocol::PutRecipients (QStringList &list, const KURL &url)
{
	QString formatted_recip = ASCII("RCPT TO: %1");
	for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
		if (!command(formatted_recip.arg(*it)))
			HandleSMTPWriteError(url);
	}
}

void SMTPProtocol::setHost (const QString &host, int port, const QString &user, const QString &pass)
{
	m_sServer = host;
	m_iPort = port;
	m_sUser = user;
	m_sPass = pass;
}

int SMTPProtocol::getResponse (char *r_buf, unsigned int r_len)
{
	char *buf = 0;
	unsigned int recv_len = 0, len;
	fd_set FDs;

	// Give the buffer the appropiate size
	// a buffer of less than 5 bytes will *not* work
	if (r_len) {
		buf = static_cast<char *>(malloc(r_len));
		len = r_len;
	} else {
		buf = static_cast<char *>(malloc(DEFAULT_RESPONSE_BUFFER));
		len = DEFAULT_RESPONSE_BUFFER;
	}

	// And keep waiting if it timed out
	unsigned int wait_time = 60; // Wait 60sec. max.
	do {
		// Wait for something to come from the server
		FD_ZERO(&FDs);
		FD_SET(m_iSock, &FDs);
		// Yes, it's true, Linux sucks.
		wait_time--;
		m_tTimeout.tv_sec = 1;
		m_tTimeout.tv_usec = 0;
	}
	while (wait_time && (::select(m_iSock+1, &FDs, 0, 0, &m_tTimeout) ==0));

	if (wait_time == 0) {
		kdDebug () << "kio_smtp: No response from SMTP server in 60 secs." << endl;
		return false;
	}

	// Clear out the buffer
	memset(buf, 0, len);
	// And grab the data
	ReadLine(buf, len-1);

	// This is really a funky crash waiting to happen if something isn't
	// null terminated.
	recv_len = strlen(buf);

	if (recv_len < 4)
		error(ERR_UNKNOWN, "Line too short");
	char *origbuf = buf;
	if (buf[3] == '-') { // Multiline response
		while ( (buf[3] == '-') && (len-recv_len > 3) ) { // Three is quite arbitrary
			buf += recv_len;
			len -= (recv_len+1);
			recv_len = ReadLine(buf, len-1);
			if (recv_len == 0)
				buf[0] = buf[1] = buf[2] = buf[3] = ' ';
		}
		buf = origbuf;
		memcpy(r_buf, buf, strlen(buf));
		r_buf[r_len-1] = 0;
		return GetVal(buf);
	} else {
		// Really crude, whee
		if (r_len) {
			r_len = recv_len-4;
			memcpy(r_buf, buf+4, r_len);
		}
		return GetVal(buf);
	}
}

bool SMTPProtocol::command (const QString &cmd, char *recv_buf, unsigned int len)
{
	QCString write_buf = cmd.local8Bit();

	// Write the command
	if ( Write(static_cast<const char *>(write_buf), write_buf.length() ) != static_cast<ssize_t>(write_buf.length()) ) {
		m_sError = i18n("Could not send to server.\n");
		return false;
	}

	if (Write("\r\n", 2) != 2) {
		m_sError = i18n("Could not send to server.\n");
		return false;
	}
	return (getResponse(recv_buf, len) < 400);
}

bool SMTPProtocol::smtp_open (const KURL &url)
{
	kdDebug () << "kio_smtp: IT WANTS " << url.host() << endl;
	if ( (m_iOldPort == GetPort(m_iPort)) && (m_sOldServer == m_sServer) && (m_sOldUser == m_sUser) ) {
		return true;
	} else {
		smtp_close();
		if( !ConnectToHost(m_sServer.latin1(), m_iPort))
			return false; // ConnectToHost has already send an error message.
		opened = true;
	}

	if (getResponse() >= 400) {
		return false;
	}

	QBuffer ehlobuf(QByteArray(DEFAULT_EHLO_BUFFER));
	memset(ehlobuf.buffer().data(), 0, DEFAULT_EHLO_BUFFER);

	// Yes, I *know* that this is not the way it should be done, but
	// for now there's no real need to complicate things by
	// determining our hostname.  AFAIK no servers really depend on this..
	if (!command(ASCII("EHLO kio_smtp"), ehlobuf.buffer().data(), 5119)) {
		if (!command(ASCII("HELO kio_smtp"))) { // Let's just check to see if it speaks plain ol' SMTP
			smtp_close();
			return false;
		}
	}

	// We parse the ESMTP extensions here... pipelining would be really cool
	char ehlo_line[DEFAULT_EHLO_BUFFER];
	if (ehlobuf.open(IO_ReadWrite)) {
		while (ehlobuf.readLine(ehlo_line, DEFAULT_EHLO_BUFFER) > 0)
			ParseFeatures(const_cast<const char *>(ehlo_line));
	}

	// we should also check kemailsettings as well..
	if (haveTLS && canUseTLS()) { 
                // For now we're gonna force it on.
                if (command(ASCII("STARTTLS"))) {
                   int tlsrc = startTLS();
                   if (tlsrc == 1) {
                      kdDebug() << "TLS has been enabled!" << endl;
                   } else {
                      if (tlsrc != -3) {
                        kdDebug() << "TLS negotiation failed!" << endl;
                        messageBox(Information,
                                   i18n("Your SMTP server claims to "
                                        "support TLS but negotiation "
                                        "was unsuccessful.  You can "
                                        "disable TLS in KDE using the "
                                        "crypto settings module."),
                                   i18n("Connection Failed"));
                      }
                      return false;
                   }
                } 
	}

	// Now we try and login
	if (!m_sUser.isNull()) {
		KURL auth_url=url;
		if (m_sPass.isNull()) {
			QString head=i18n("Username and password for your SMTP account:");
                        if (!openPassDlg(head, m_sUser, m_sPass)) {
				error(ERR_COULD_NOT_LOGIN, i18n("When prompted, you ran away."));
				return false;
			}
			auth_url.setPass(m_sPass);
			auth_url.setUser(m_sUser);
		}
		if (!Authenticate(auth_url)) {
			error(ERR_COULD_NOT_LOGIN, i18n("Authentication failed"));
			return false;
		}
	}

	m_iOldPort = m_iPort;
	m_sOldServer = m_sServer;
	m_sOldUser = m_sUser;
	m_sOldPass = m_sPass;

	return true;
}

bool SMTPProtocol::Authenticate (const KURL &url)
{
	bool ret;
	QString auth_method;

	// Generate a new context.. now this isn't the most efficient way of doing things, but for now this suffices
	if (m_pSASL)
		delete m_pSASL;
	m_pSASL = new KDESasl(url);

	// Choose available method from what the server has given us in  its greeting
	QStringList sl = QStringList::split(' ', m_sAuthConfig);
	QStrIList strList;
	for (int i = 0; i < sl.count(); i++) strList.append(sl[i].latin1());

	auth_method = m_pSASL->chooseMethod(strList);

	// If none are available, set it up so we can start over again
	if (auth_method == QString::null) {
		delete m_pSASL;
		m_pSASL = 0;
		kdDebug() << "kio_smtp: no authentication available" << endl;
		error(ERR_COULD_NOT_LOGIN, i18n("No compatible authentication methods found."));
		return false;
	} else {
		char *challenge = static_cast<char *>(malloc(2049));

 		// I've probably made some troll seek shelter somewhere else.. yay gov'nr and his matrix.. yes that one.. that looked like a  bird.. no not harvey milk
		if (!command(ASCII("AUTH ")+auth_method, challenge, 2049)) {
			free(challenge);
			delete m_pSASL;
			m_pSASL = 0;
			return false;
		}

		// Now for the stupid part
		// PLAIN auth has embedded null characters.  Ew.
		// so it's encoded in HEX as part of its spec IIRC.
		// so.. let that auth module do the b64 encoding itself..
		// it's easier than generating a byte array simply to pass
		// around null characters.  Stupid stupid stupid.
		QString cmd;
		QByteArray ba;
		ba.duplicate(challenge, strlen(challenge));
		cmd = m_pSASL->getResponse(ba);
		ret = command(cmd);
		free(challenge);

		if (ret)
			kdDebug() << "kio_smtp: auth worked" << endl;
		else
			kdDebug() << "kio_smtp: auth didn't work" << endl;
		return ret;
	}
	return false;
}

void SMTPProtocol::ParseFeatures (const char *_buf)
{
	QCString buf(_buf);

	// We want it to be between 250 and 259 inclusive, and it needs to be "nnn-blah" or "nnn blah"
	// So sez the SMTP spec..
	if ( (buf.left(2) != "25") || (!isdigit(buf[2])) || (!(buf.at(3) == '-') && !(buf.at(3) == ' ')) )
		return; // We got an invalid line..
	// Clop off the beginning, no need for it really
	buf = buf.mid(4, buf.length());

	if (buf.left(4) == "AUTH") { // Look for auth stuff
		if (m_sAuthConfig == QString::null) 
			m_sAuthConfig = buf.mid(5, buf.length());
	} else if (buf.left(8) == "STARTTLS") {
		haveTLS=true;
	}
}

void SMTPProtocol::smtp_close ()
{
	if (!opened) // We're already closed
		return;
	command(ASCII("QUIT"));
	CloseDescriptor();
	m_sOldServer = QString::null;
	m_pSASL = 0;
	m_sAuthConfig = QString::null;
	opened = false;
}

void SMTPProtocol::stat (const KURL &url)
{
	QString path = url.path();
        error (KIO::ERR_DOES_NOT_EXIST, url.path());
}

int GetVal (char *buf)
{
	int val;

	val = (100*(static_cast<int>(buf[0])-48));
	val += (10*(static_cast<int>(buf[1])-48));
	val += static_cast<int>(buf[2])-48;

	return val;
}

void GetAddresses (const QString &str, const QString &delim, QStringList &list)
{
	int curpos = 0;

	while ( (curpos = str.find(delim, curpos) ) != -1 ) {
		if ( (str.at(curpos-1) == "?") || (str.at(curpos-1) == "&") ) {
			curpos += delim.length();
			if (str.find("&", curpos) != -1)
				list += str.mid(curpos, str.find("&", curpos)-curpos);
			else
				list += str.mid(curpos, str.length());
		} else
			curpos += delim.length();
	}
}
