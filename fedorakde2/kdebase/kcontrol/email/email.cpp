/* vi: ts=8 sts=4 sw=4
 *
 * $Id: email.cpp,v 1.63 2001/07/23 20:27:21 haeckel Exp $
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

#include <sys/types.h>
#include <sys/stat.h>

#include <pwd.h>
#include <unistd.h>

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qfile.h>
#include <qfileinfo.h>

#include <kdebug.h>
#include <klineedit.h>
#include <kcombobox.h>
#include <kdialog.h>
#include <kglobal.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kstddirs.h>
#include <kmessagebox.h>
#include <kemailsettings.h>
#include <kurlrequester.h>

#include "email.h"


topKCMEmail::topKCMEmail (QWidget *parent,  const char *name)
	: KCModule (parent, name)
{
	QVBoxLayout *topLayout = new QVBoxLayout(this, 0, KDialog::spacingHint());
	m_email = new KCMEmailBase(this);
	topLayout->add(m_email);

 	connect(m_email->cmbCurProfile, SIGNAL(activated(const QString &)), this, SLOT(slotComboChanged(const QString &)) );
	connect(m_email->btnNewProfile, SIGNAL(clicked()), SLOT(slotNewProfile()) );

	connect(m_email->txtFullName,SIGNAL(textChanged(const QString&)), SLOT(configChanged()) );
	connect(m_email->txtOrganization, SIGNAL(textChanged(const QString&)), SLOT(configChanged()) );
	connect(m_email->txtEMailAddr, SIGNAL(textChanged(const QString&)), SLOT(configChanged()) );
	connect(m_email->txtReplyTo, SIGNAL(textChanged(const QString&)), SLOT(configChanged()) );
	connect(m_email->txtEMailClient, SIGNAL(textChanged(const QString&)), SLOT(configChanged()) );
	connect(m_email->chkRunTerminal, SIGNAL(clicked()), SLOT(configChanged()) );


	pSettings = new KEMailSettings();
	m_email->lblCurrentProfile->hide();
	m_email->cmbCurProfile->hide();
	m_email->btnNewProfile->hide();
	load();
}

topKCMEmail::~topKCMEmail()
{
}


void topKCMEmail::load()
{
	m_email->cmbCurProfile->clear();
	load(QString::null);
}

void topKCMEmail::load(const QString &s)
{
	if (s == QString::null) {
		m_email->cmbCurProfile->insertStringList(pSettings->profiles());
		if (pSettings->defaultProfileName() != QString::null) {
			kdDebug() << "prfile is: " << pSettings->defaultProfileName()<< "line is " << __LINE__ << endl;
			load(pSettings->defaultProfileName());
		} else {
			if (m_email->cmbCurProfile->count()) {
				//pSettings->setProfile(m_email->cmbCurProfile->text(0));
				kdDebug() << "prfile is: " << m_email->cmbCurProfile->text(0) << endl;
				load(m_email->cmbCurProfile->text(0));
				//pSettings->setDefault(m_email->cmbCurProfile->text(0));
			} else {
				m_email->cmbCurProfile->insertItem(i18n("Default"));
				pSettings->setProfile(i18n("Default"));
				pSettings->setDefault(i18n("Default"));
			}
		}
	} else {
		pSettings->setProfile(s);
		m_email->txtEMailAddr->setText(pSettings->getSetting(KEMailSettings::EmailAddress));
		m_email->txtReplyTo->setText(pSettings->getSetting(KEMailSettings::ReplyToAddress));
		m_email->txtOrganization->setText(pSettings->getSetting(KEMailSettings::Organization));
		m_email->txtFullName->setText(pSettings->getSetting(KEMailSettings::RealName));

		m_email->txtEMailClient->setURL(pSettings->getSetting(KEMailSettings::ClientProgram));
		m_email->chkRunTerminal->setChecked((pSettings->getSetting(KEMailSettings::ClientTerminal) == "true"));

		configChanged(false);
	}

}

void topKCMEmail::clearData()
{
	m_email->txtEMailAddr->setText(QString::null);
	m_email->txtReplyTo->setText(QString::null);
	m_email->txtOrganization->setText(QString::null);
	m_email->txtFullName->setText(QString::null);

	m_email->txtEMailClient->setURL(QString::null);
	m_email->chkRunTerminal->setChecked(false);

	configChanged(false);
}

void topKCMEmail::slotNewProfile()
{
	KDialog *dlgAskName = new KDialog(this, "noname", true);
	dlgAskName->setCaption(i18n("New Email Profile"));

	QVBoxLayout *vlayout = new QVBoxLayout(dlgAskName, KDialog::marginHint(), KDialog::spacingHint());

	QHBoxLayout *layout = new QHBoxLayout(vlayout);

	QLabel *lblName = new QLabel(dlgAskName);
	lblName->setText(i18n("Name:"));
	lblName->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
	KLineEdit *txtName = new KLineEdit(dlgAskName);
	lblName->setBuddy(txtName);
	layout->addWidget(lblName);
	layout->addWidget(txtName);

	layout = new QHBoxLayout(vlayout);
	QPushButton *btnOK = new QPushButton(dlgAskName);
	btnOK->setText(i18n("&OK"));
	btnOK->setFixedSize(btnOK->sizeHint());
	QPushButton *btnCancel = new QPushButton(dlgAskName);
	btnCancel->setText(i18n("&Cancel"));
	btnCancel->setFixedSize(btnCancel->sizeHint());
	layout->addWidget(btnOK);
	layout->addWidget(btnCancel);
	connect(btnOK, SIGNAL(clicked()), dlgAskName, SLOT(accept()));
	connect(btnCancel, SIGNAL(clicked()), dlgAskName, SLOT(reject()));
        connect(txtName, SIGNAL(returnPressed ()),dlgAskName, SLOT(accept()));
	txtName->setFocus();

	if (dlgAskName->exec() == QDialog::Accepted) {
		if (txtName->text().isEmpty()) {
			KMessageBox::sorry(this, i18n("Oops, you need to enter a name please. Thanks."));
		} else if (m_email->cmbCurProfile->currentText().contains(txtName->text()))
			KMessageBox::sorry(this, i18n("This email profile already exists, and cannot be created again"), i18n("Oops"));
		else {
			pSettings->setProfile(txtName->text());
			m_email->cmbCurProfile->insertItem(txtName->text());
			// should probbaly load defaults instead
			clearData();
                        //you add new profile so you change config
                        configChanged();
			m_email->cmbCurProfile->setCurrentItem(m_email->cmbCurProfile->count()-1);
		}
	} else { // rejected
	}

	delete dlgAskName;
}

void topKCMEmail::configChanged(bool c)
{
	emit changed(c);
	m_bChanged=c;
}

void topKCMEmail::configChanged()
{
	configChanged(true);
}

void topKCMEmail::save()
{
	pSettings->setProfile(m_email->cmbCurProfile->text(m_email->cmbCurProfile->currentItem()));
	pSettings->setSetting(KEMailSettings::RealName, m_email->txtFullName->text());
	pSettings->setSetting(KEMailSettings::EmailAddress, m_email->txtEMailAddr->text());
	pSettings->setSetting(KEMailSettings::Organization, m_email->txtOrganization->text());
	pSettings->setSetting(KEMailSettings::ReplyToAddress, m_email->txtReplyTo->text());

	pSettings->setSetting(KEMailSettings::ClientProgram, m_email->txtEMailClient->url());
	pSettings->setSetting(KEMailSettings::ClientTerminal, (m_email->chkRunTerminal->isChecked()) ? "true" : "false");

	// insure proper permissions -- contains sensitive data
	QString cfgName(KGlobal::dirs()->findResource("config", "emaildefaults"));
	if (!cfgName.isEmpty())
		::chmod(QFile::encodeName(cfgName), 0600);

	configChanged(false);
}

void topKCMEmail::defaults()
{
	char hostname[80];
	struct passwd *p;

	p = getpwuid(getuid());
	gethostname(hostname, 80);

	m_email->txtFullName->setText(QString::fromLocal8Bit(p->pw_gecos));
        m_email->txtOrganization->setText(QString::null);
        m_email->txtReplyTo->setText(QString::null);

	QString tmp = QString::fromLocal8Bit(p->pw_name);
	tmp += "@"; tmp += hostname;

	m_email->txtEMailAddr->setText(tmp);

/*	QString client = KGlobal::dirs()->findResource("exe", "kmail");

	if (client.isEmpty())
		client = "kmail"; */
	QString client = "kmail";

	m_email->txtEMailClient->setURL(client);

	m_email->chkRunTerminal->setChecked(false);

	configChanged();
}

QString topKCMEmail::quickHelp() const
{
	return i18n("<h1>email</h1> This module allows you to enter basic email"
	            " information for the current user. The information here is used,"
	            " among other things, for sending bug reports to the KDE developers"
	            " when you use the bug report dialog.<p>"
	            " Note that email programs like KMail and Empath offer many more"
	            " features, but they provide their own configuration facilities.");
}

void topKCMEmail::selectEmailClient()
{
	QString client = KFileDialog::getOpenFileName(QString::null, "*", this);

	QFileInfo *clientInfo = new QFileInfo(client);
	if (clientInfo->exists() && clientInfo->isExecutable() && clientInfo->filePath().contains(' ') == 0)
		m_email->txtEMailClient->setURL(client);
}

void topKCMEmail::profileChanged(const QString &s)
{
	save(); // Save our changes...
	load(s);
}

void topKCMEmail::slotComboChanged(const QString &name)
{
	if (m_bChanged) {
		if (KMessageBox::warningYesNo(this, i18n("Do you wish to discard changes to the current profile?")) == KMessageBox::No) {
			if (KMessageBox::warningYesNo(this, i18n("Do you wish to save changes to the current profile?")) == KMessageBox::Yes) {
				save();
			} else {
				int keep=-1;
				for (int i=0; i < m_email->cmbCurProfile->count(); i++) {
					if (m_email->cmbCurProfile->text(i) == pSettings->currentProfileName()) {
						keep=i; break;
					}
				}
				if (keep != -1)
					m_email->cmbCurProfile->setCurrentItem(keep);
				return;
			}
		}
	}
	load(name);
}


extern "C"
{
	KCModule *create_email(QWidget *parent, const char *name) {
		KGlobal::locale()->insertCatalogue("kcmemail");
		return new topKCMEmail(parent, name);
	};
}

#include "email.moc"
