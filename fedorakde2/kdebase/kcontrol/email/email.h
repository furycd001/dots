/* vi: ts=8 sts=4 sw=4
 *
 * $Id: email.h,v 1.24 2001/07/20 19:56:55 haeckel Exp $
 *
 * This file is part of the KDE project, module kcontrol.
 * Copyright (C) 1999-2001 by Alex Zepeda and Daniel Molkentin
 *
 * You can freely distribute this program under the following terms:
 *
 * 1.) If you use this program in a product outside of KDE, you must 
 * credit the authors, and must mention "kcmemail".
 *
 * 2.) If this program is used in a product other than KDE, this license 
 * must be reproduced in its entirety in the documentation and/or some 
 * other highly visible location (e.g. startup splash screen)
 * 
 * 3.) Use of this product implies the following terms:
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
 */

#ifndef KCMEMAIL_H
#define KCMEMAIL_H

#include <qvariant.h>
#include <qdialog.h>

#include <kcmodule.h>

#include "emailbase.h"

class QButtonGroup;
class QCheckBox;
class QGroupBox;
class QLabel;
class KLineEdit;
class QPushButton;
class QRadioButton;
class QVBox;

class KComboBox;
class KEMailSettings;

class topKCMEmail
	: public KCModule
{
	Q_OBJECT

public:
	topKCMEmail (QWidget *parent = 0, const char *name = 0);
	~topKCMEmail ();

	KCMEmailBase *m_email;

	void load();
	void load(const QString &);
	void save();
	void defaults();
	QString quickHelp() const;

public slots:

	void configChanged();
	void configChanged(bool);
	void selectEmailClient();
	void profileChanged(const QString &);


protected slots:

	void slotComboChanged(const QString &);
	void slotNewProfile();

protected:
	void clearData();
	KEMailSettings *pSettings;
	QString m_sICMPassword, m_sICMUsername, m_sICMPath, m_sICMHost;
	QString m_sOGMPassword, m_sOGMUsername, m_sOGMCommand, m_sOGMHost;
	unsigned int m_uOGMPort, m_uICMPort;
	bool m_bOGMSecure, m_bICMSecure;

	bool m_bChanged;
};

#endif // TOPKCMEMAIL_H
