/*-
 * Copyright (c) 2000 Alex Zepeda
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
 *	$Id$
 */

#ifndef _KEMAILSETTINGS_H
#define _KEMAILSETTINGS_H "$Id$"

#include <qstring.h>
#include <qstringlist.h>

class KEMailSettingsPrivate;


/**
  * This is just a small class to facilitate accessing e-mail settings in 
  * a sane way, and allowing any program to manage multiple e-mail 
  * profiles effortlessly
  *
  * @author Alex Zepeda jazepeda@pacbell.net
  **/
class KEMailSettings {
public:
	/**
	  * The list of settings that I thought of when I wrote this 
	  * class.  Any extra settings thought of later can be accessed 
	  * easily with getExtendedSetting and setExtendedSetting.
	  * @see getExtendedSetting.
	  * @see setExtendedSetting.
	  **/
	enum Setting {
		ClientProgram,
		ClientTerminal,
		RealName,
		EmailAddress,
		ReplyToAddress,
		Organization,
		OutServer,
		OutServerLogin,
		OutServerPass,
		OutServerType,
		OutServerCommand,
		OutServerTLS,
		InServer,
		InServerLogin,
		InServerPass,
		InServerType,
		InServerMBXType,
		InServerTLS
	};

	/**
	  * The various extensions allowed.
	  **/
	enum Extension {
		POP3,
		SMTP,
		OTHER
	};

	/**
	  * Default constructor, just sets things up.
	  **/
	KEMailSettings();

	/**
	  * Default destructor, nothing to see here.
	  **/
	~KEMailSettings();

	/**
	  * List of profiles available.
	  **/
	QStringList profiles() const;

	/**
	  * @returns what profile we're currently using
	  **/
	QString currentProfileName() const;

	/**
	  * Change the current profile.
	  **/
	void setProfile (const QString &);

	/**
	  * @returns the name of the one that's currently default QString::null if none
	  **/
	QString defaultProfileName() const;

	/**
	  * New default..
	  **/
	void setDefault(const QString &);

	/**
	  * Get a "basic" setting, one that I've already thought of..
	  **/
	QString getSetting(KEMailSettings::Setting s);
	void setSetting(KEMailSettings::Setting s, const QString &v);

	/**
	  * Use this when trying to get at currently unimplemented settings
	  * such as POP3 authentication methods, or mail specific TLS settings
	  * or something I haven't already thought of.
	  **/
	QString getExtendedSetting(KEMailSettings::Extension e, const QString &s );

	/**
	  * Use this when trying to get at currently unimplemented settings
	  * such as POP3 authentication methods, or mail specific TLS settings
	  * or something I haven't already thought of.
	  **/
	void setExtendedSetting(KEMailSettings::Extension e, const QString &s, const QString &v );

private:
	KEMailSettingsPrivate *p;
};

#endif
