/*
 *   kdeprintfax - a small fax utility
 *   Copyright (C) 2001  Michael Goffioul
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
 */

#include "faxctrl.h"
#include "kdeprintfax.h"
#include "defcmds.h"

#include <qmultilineedit.h>
#include <qfile.h>

#include <kprocess.h>
#include <kglobal.h>
#include <kconfig.h>
#include <klocale.h>
#include <kdialogbase.h>
#include <kmimetype.h>
#include <kstddirs.h>
#include <kapp.h>

#include <stdlib.h>

FaxCtrl::FaxCtrl(QObject *parent, const char *name)
: QObject(parent, name)
{
	m_process = new KShellProcess();
	connect(m_process, SIGNAL(receivedStdout(KProcess*,char*,int)), SLOT(slotReceivedStdout(KProcess*,char*,int)));
	connect(m_process, SIGNAL(receivedStderr(KProcess*,char*,int)), SLOT(slotReceivedStdout(KProcess*,char*,int)));
	connect(m_process, SIGNAL(processExited(KProcess*)), SLOT(slotProcessExited(KProcess*)));
	connect(this, SIGNAL(faxSent(bool)), SLOT(cleanTempFiles()));
}

FaxCtrl::~FaxCtrl()
{
	delete m_process;
}

bool FaxCtrl::send(KdeprintFax *f)
{
	m_command = faxCommand();
	if (m_command.isEmpty())
		return false;
	KConfig	*conf = KGlobal::config();

	QString	v;
	// settings
	conf->setGroup("Fax");
	m_command.replace(QRegExp("%dev"), conf->readEntry("Device", "modem"));
	v = conf->readEntry("Server");
	if (v.isEmpty()) v = getenv("FAXSERVER");
	if (v.isEmpty()) v = QString::fromLatin1("localhost");
	m_command.replace(QRegExp("%server"), v);
	m_command.replace(QRegExp("%page"), conf->readEntry("Page", "a4"));
	v = conf->readEntry("Resolution", "High");
	m_command.replace(QRegExp("%res"), (v == "High" ? "" : "-l"));
	conf->setGroup("Personal");
	m_command.replace(QRegExp("%user"), conf->readEntry("Name", getenv("USER")));
	m_command.replace(QRegExp("%from"), conf->readEntry("Number", ""));

	// arguments
	m_command.replace(QRegExp("%number"), f->number());
	m_command.replace(QRegExp("%name"), f->name());
	m_command.replace(QRegExp("%comment"), f->comment());
	m_command.replace(QRegExp("%enterprise"), f->enterprise());

	m_log = QString::null;
	m_filteredfiles.clear();
	cleanTempFiles();
	m_files = f->files();

	filter();

	return true;
}

bool FaxCtrl::isExtended()
{
	KConfig	*conf = KGlobal::config();
	conf->setGroup("System");
	return (conf->readEntry("System", "efax") == "hylafax");
}

void FaxCtrl::slotReceivedStdout(KProcess*, char *buffer, int len)
{
	QCString	str(buffer, len);
	addLog(QString(str));
}

void FaxCtrl::slotProcessExited(KProcess*)
{
	// we exited a process: if there's still entries in m_files, this was a filter
	// process, else this was the fax process
	bool	ok = (m_process->normalExit() && ((m_process->exitStatus()&0x1) == 0));
	if (ok && m_files.count() > 0)
	{
		// remove first element
		m_files.remove(m_files.begin());
		if (m_files.count() > 0)
			filter();
		else
			sendFax();
	}
	else
		emit faxSent(ok);
}

QString FaxCtrl::faxCommand()
{
	KConfig	*conf = KGlobal::config();
	conf->setGroup("System");
	QString	sys = conf->readEntry("System", "efax");
	if (sys == "hylafax") return conf->readEntry("HylaFax", hylafax_default_cmd);
	else return conf->readEntry("EFax", hylafax_default_cmd);
}

void FaxCtrl::sendFax()
{
	// files
	m_command.replace(QRegExp("%files"), m_filteredfiles.join(" "));

	m_process->clearArguments();
	*m_process << m_command;
	addLog(i18n("Sending to fax using: %1\n").arg(m_command));
	if (!m_process->start(KProcess::NotifyOnExit, KProcess::AllOutput))
		emit faxSent(false);
	else emit message(i18n("Sending fax..."));
}

void FaxCtrl::filter()
{
	if (m_files.count() > 0)
	{
		QString	mimeType = KMimeType::findByURL(KURL(m_files[0]), 0, true)->name();
		if (mimeType == "application/postscript" || mimeType == "image/tiff")
		{
			emit message(i18n("Skipping %1...").arg(m_files[0]));
			m_filteredfiles.prepend(m_files[0]);
			m_files.remove(m_files.begin());
			filter();
		}
		else
		{
			QString	tmp = locateLocal("tmp","kdeprintfax_") + kapp->randomString(8);
			m_filteredfiles.prepend(tmp);
			m_tempfiles.append(tmp);
			m_process->clearArguments();
			*m_process << locate("data","kdeprintfax/anytops") << "-m" << locate("data","kdeprintfax/faxfilters") << QString::fromLatin1("--mime=%1").arg(mimeType) << m_files[0] << tmp;
			if (!m_process->start(KProcess::NotifyOnExit, KProcess::AllOutput))
				emit faxSent(false);
			else emit message(i18n("Filtering %1...").arg(m_files[0]));
		}
	}
	else
		sendFax();
}

bool FaxCtrl::abort()
{
	if (m_process->isRunning())
		return m_process->kill();
	else
		return false;
}

void FaxCtrl::viewLog(QWidget *parent)
{
	KDialogBase	dlg(parent, 0, true, i18n("Log message"), KDialogBase::Close, KDialogBase::Close);
	QMultiLineEdit	*m_edit = new QMultiLineEdit(&dlg);
	m_edit->setText(m_log);
	m_edit->setWordWrap(QMultiLineEdit::WidgetWidth);
	m_edit->setReadOnly(true);
	dlg.setMainWidget(m_edit);
	dlg.resize(450, 300);
	dlg.exec();
}

void FaxCtrl::addLog(const QString& s)
{
	m_log.append(s).append("\n");
}

QString FaxCtrl::faxSystem()
{
	KConfig	*conf = KGlobal::config();
	conf->setGroup("System");
	QString	s = conf->readEntry("System", "efax");
	s[0] = s[0].upper();
	return s;
}

void FaxCtrl::cleanTempFiles()
{
	for (QStringList::ConstIterator it=m_tempfiles.begin(); it!=m_tempfiles.end(); ++it)
		QFile::remove(*it);
	m_tempfiles.clear();
}

#include "faxctrl.moc"
