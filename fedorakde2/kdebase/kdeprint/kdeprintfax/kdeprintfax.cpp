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

#include "kdeprintfax.h"
#include "faxab.h"
#include "faxctrl.h"
#include "configdlg.h"

#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qdragobject.h>
#include <qmultilineedit.h>

#include <kapp.h>
#include <kstdaction.h>
#include <kmenubar.h>
#include <ktoolbar.h>
#include <kaction.h>
#include <klistbox.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kio/netaccess.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kmimetype.h>
#include <kseparator.h>
#include <ksystemtray.h>
#include <kstatusbar.h>
#include <ksqueezedtextlabel.h>

KdeprintFax::KdeprintFax(QWidget *parent, const char *name)
: KMainWindow(parent, name)
{
	m_faxctrl = new FaxCtrl(this);
	connect(m_faxctrl, SIGNAL(message(const QString&)), SLOT(slotMessage(const QString&)));
	connect(m_faxctrl, SIGNAL(faxSent(bool)), SLOT(slotFaxSent(bool)));

	QWidget	*mainw = new QWidget(this);
	setCentralWidget(mainw);
	m_files = new KListBox(mainw);
	QLabel	*m_filelabel = new QLabel(i18n("Files:"), mainw);
        KSeparator*m_line = new KSeparator( KSeparator::HLine, mainw);
	m_number = new QLineEdit(mainw);
	m_name = new QLineEdit(mainw);
	m_enterprise = new QLineEdit(mainw);
	QLabel	*m_numberlabel = new QLabel(i18n("Fax Number:"), mainw);
	QLabel	*m_namelabel = new QLabel(i18n("Name:"), mainw);
	QLabel	*m_enterpriselabel = new QLabel(i18n("Enterprise:"), mainw);
	QLabel	*m_commentlabel = new QLabel(i18n("Comment:"), mainw);
	KSystemTray	*m_tray = new KSystemTray(this);
	m_tray->setPixmap(SmallIcon("kdeprintfax"));
	m_tray->show();
	m_comment = new QMultiLineEdit(mainw);
	m_comment->setLineWidth(1);

	QGridLayout	*l0 = new QGridLayout(mainw, 7, 2, 10, 5);
	l0->setColStretch(1,1);
	l0->addRowSpacing(5, 10);
	l0->addWidget(m_filelabel, 0, 0, Qt::AlignLeft|Qt::AlignTop);
	l0->addWidget(m_files, 0, 1);
	l0->addMultiCellWidget(m_line, 1, 1, 0, 1);
        l0->addRowSpacing(1, 10);
	l0->addWidget(m_numberlabel, 2, 0);
	l0->addWidget(m_namelabel, 3, 0);
	l0->addWidget(m_enterpriselabel, 4, 0);
	l0->addWidget(m_number, 2, 1);
	l0->addWidget(m_name, 3, 1);
	l0->addWidget(m_enterprise, 4, 1);
	l0->addWidget(m_commentlabel, 6, 0, Qt::AlignTop|Qt::AlignLeft);
	l0->addWidget(m_comment, 6, 1);

	m_msglabel = new KSqueezedTextLabel(statusBar());
	statusBar()->addWidget(m_msglabel, 1);
	statusBar()->insertFixedItem(i18n("Processing..."), 1);
	statusBar()->changeItem(i18n("Idle"), 1);
	statusBar()->insertFixedItem("hylafax/efax", 2);
	initActions();
	setAcceptDrops(true);
	setCaption(i18n("Send To Fax"));
	updateState();

	resize(450,400);
	QWidget	*d = kapp->desktop();
	move((d->width()-width())/2, (d->height()-height())/2);
}

void KdeprintFax::initActions()
{
	new KAction(i18n("Add File..."), "filenew", Qt::Key_Insert, this, SLOT(slotAdd()), actionCollection(), "file_add");
	new KAction(i18n("Remove File"), "remove", Qt::Key_Delete, this, SLOT(slotRemove()), actionCollection(), "file_remove");
	new KAction(i18n("Send Fax"), "connect_established", Qt::Key_Return, this, SLOT(slotFax()), actionCollection(), "fax_send");
	new KAction(i18n("Abort"), "stop", Qt::Key_Escape, this, SLOT(slotAbort()), actionCollection(), "fax_stop");
	new KAction(i18n("Address Book..."), "contents2", Qt::CTRL+Qt::Key_A, this, SLOT(slotKab()), actionCollection(), "fax_ab");
	new KAction(i18n("View Log..."), "contents", Qt::CTRL+Qt::Key_L, this, SLOT(slotViewLog()), actionCollection(), "fax_log");

	KStdAction::quit(this, SLOT(slotQuit()), actionCollection());
	KStdAction::showToolbar(this, SLOT(slotToggleToolBar()), actionCollection());
	KStdAction::showMenubar(this, SLOT(slotToggleMenuBar()), actionCollection());
	KStdAction::preferences(this, SLOT(slotConfigure()), actionCollection());

	actionCollection()->action("fax_stop")->setEnabled(false);

	createGUI();
	toolBar()->setIconText(KToolBar::IconTextBottom);
}

void KdeprintFax::slotToggleToolBar()
{
	if (toolBar()->isVisible()) toolBar()->hide();
	else toolBar()->show();
}

void KdeprintFax::slotToggleMenuBar()
{
	if (menuBar()->isVisible()) menuBar()->hide();
	else menuBar()->show();
}

void KdeprintFax::slotAdd()
{
	KURL	url = KFileDialog::getOpenURL(QString::null, QString::null, this);
	if (!url.isEmpty())
		addURL(url);
}

void KdeprintFax::slotRemove()
{
	if (m_files->currentItem() >= 0)
		m_files->removeItem(m_files->currentItem());
}

void KdeprintFax::slotFax()
{
	if (m_files->count() == 0)
		KMessageBox::error(this, i18n("No file to fax."));
	else if (m_number->text().isEmpty())
		KMessageBox::error(this, i18n("No fax number specified."));
	else if (m_faxctrl->send(this))
	{
		actionCollection()->action("fax_send")->setEnabled(false);
		actionCollection()->action("fax_stop")->setEnabled(true);
		statusBar()->changeItem(i18n("Processing..."), 1);
	}
	else
		KMessageBox::error(this, i18n("Unable to start Fax process."));
}

void KdeprintFax::slotAbort()
{
	if (!m_faxctrl->abort())
		KMessageBox::error(this, i18n("Unable to stop Fax process."));
}

void KdeprintFax::slotKab()
{
	QString	number, name, enterprise;
	if (FaxAB::getEntry(number, name, enterprise, this))
	{
		m_number->setText(number);
		m_name->setText(name);
		m_enterprise->setText(enterprise);
	}
}

void KdeprintFax::addURL(KURL url)
{
	QString	target;
	if (KIO::NetAccess::download(url,target))
		m_files->insertItem(KMimeType::pixmapForURL(url,0,KIcon::Small),target);
	else
		KMessageBox::error(this, i18n("Unable to retrieve %1.").arg(url.prettyURL()));
}

void KdeprintFax::dragEnterEvent(QDragEnterEvent *e)
{
	e->accept(QUriDrag::canDecode(e));
}

void KdeprintFax::dropEvent(QDropEvent *e)
{
	QStrList	l;
	if (QUriDrag::decode(e, l))
	{
		QStrListIterator	it(l);
		for (;it.current();++it)
			addURL(KURL(it.current()));
	}
}

QStringList KdeprintFax::files()
{
	QStringList	l;
	for (uint i=0; i<m_files->count(); i++)
		l.append(m_files->text(i));
	return l;
}

QString KdeprintFax::number()
{
	return m_number->text();
}

QString KdeprintFax::name()
{
	return m_name->text();
}

QString KdeprintFax::enterprise()
{
	return m_enterprise->text();
}

QString KdeprintFax::comment()
{
	return m_comment->text();
}

void KdeprintFax::slotMessage(const QString& msg)
{
	m_msglabel->setText(msg);
}

void KdeprintFax::slotFaxSent(bool status)
{
	actionCollection()->action("fax_send")->setEnabled(true);
	actionCollection()->action("fax_stop")->setEnabled(false);
	statusBar()->changeItem(i18n("Idle"), 1);
	if (!status)
		KMessageBox::error(this, i18n("Fax error: see log message for more information."));
	slotMessage(QString::null);
}

void KdeprintFax::slotViewLog()
{
	m_faxctrl->viewLog(this);
}

void KdeprintFax::slotConfigure()
{
	if (ConfigDlg::configure(this))
		updateState();
}

void KdeprintFax::updateState()
{
	m_comment->setEnabled(m_faxctrl->isExtended());
	m_enterprise->setEnabled(m_faxctrl->isExtended());
	statusBar()->changeItem(m_faxctrl->faxSystem(), 2);
}

void KdeprintFax::slotQuit()
{
	close(true);
}

#include "kdeprintfax.moc"
