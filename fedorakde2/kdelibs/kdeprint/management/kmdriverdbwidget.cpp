/*
 *  This file is part of the KDE libraries
 *  Copyright (c) 2001 Michael Goffioul <goffioul@imec.be>
 *
 *  $Id:  $
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#include "kmdriverdbwidget.h"
#include "kmdriverdb.h"
#include "kmfactory.h"
#include "kmmanager.h"
#include "driver.h"

#include <klistbox.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qcursor.h>
#include <qapplication.h>
#include <kmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qstrlist.h>

#include <klocale.h>
#include <kcursor.h>
#include <kfiledialog.h>

KMDriverDbWidget::KMDriverDbWidget(QWidget *parent, const char *name)
: QWidget(parent,name)
{
	m_external = QString::null;

	// build widget
	m_manu = new KListBox(this);
	m_model = new KListBox(this);
	m_postscript = new QCheckBox(i18n("Postscript printer"),this);
	m_raw = new QCheckBox(i18n("Raw printer (no driver needed)"),this);
	m_postscript->setCursor(KCursor::handCursor());
	m_raw->setCursor(KCursor::handCursor());
	m_other = new QPushButton(i18n("Other..."),this);
	QLabel	*l1 = new QLabel(i18n("Manufacturer"), this);
	QLabel	*l2 = new QLabel(i18n("Model"), this);

	// build layout
	QVBoxLayout	*main_ = new QVBoxLayout(this, 0, 10);
	QGridLayout	*sub1_ = new QGridLayout(0, 2, 3, 0, 0);
	QHBoxLayout	*sub2_ = new QHBoxLayout(0, 0, 10);
	main_->addLayout(sub1_);
	main_->addLayout(sub2_);
	main_->addWidget(m_raw);
	sub1_->addWidget(l1,0,0);
	sub1_->addWidget(l2,0,2);
	sub1_->addWidget(m_manu,1,0);
	sub1_->addWidget(m_model,1,2);
	sub1_->addColSpacing(1,20);
	sub2_->addWidget(m_postscript,1);
	sub2_->addWidget(m_other,0);

	// build connections
	connect(KMDriverDB::self(),SIGNAL(dbLoaded(bool)),SLOT(slotDbLoaded(bool)));
	connect(m_manu,SIGNAL(highlighted(const QString&)),SLOT(slotManufacturerSelected(const QString&)));
	connect(m_raw,SIGNAL(toggled(bool)),m_manu,SLOT(setDisabled(bool)));
	connect(m_raw,SIGNAL(toggled(bool)),m_model,SLOT(setDisabled(bool)));
	connect(m_raw,SIGNAL(toggled(bool)),m_other,SLOT(setDisabled(bool)));
	connect(m_raw,SIGNAL(toggled(bool)),m_postscript,SLOT(setDisabled(bool)));
	connect(m_postscript,SIGNAL(toggled(bool)),m_manu,SLOT(setDisabled(bool)));
	connect(m_postscript,SIGNAL(toggled(bool)),m_model,SLOT(setDisabled(bool)));
	connect(m_postscript,SIGNAL(toggled(bool)),m_other,SLOT(setDisabled(bool)));
	connect(m_postscript,SIGNAL(toggled(bool)),m_raw,SLOT(setDisabled(bool)));
	connect(m_postscript,SIGNAL(toggled(bool)),SLOT(slotPostscriptToggled(bool)));
	connect(m_other,SIGNAL(clicked()),SLOT(slotOtherClicked()));
}

KMDriverDbWidget::~KMDriverDbWidget()
{
}

void KMDriverDbWidget::setDriver(const QString& manu, const QString& model)
{
	QListBoxItem	*item = m_manu->findItem(manu);
	if (item)
	{
		m_manu->setCurrentItem(item);
		item = m_model->findItem(model);
		if (item)
			m_model->setCurrentItem(item);
	}
}

void KMDriverDbWidget::setHaveRaw(bool on)
{
	if (on)
		m_raw->show();
	else
		m_raw->hide();
}

void KMDriverDbWidget::setHaveOther(bool on)
{
	if (on)
		m_other->show();
	else
		m_other->hide();
}

QString KMDriverDbWidget::manufacturer()
{
	return m_manu->currentText();
}

QString KMDriverDbWidget::model()
{
	return m_model->currentText();
}

KMDBEntryList* KMDriverDbWidget::drivers()
{
	return KMDriverDB::self()->findEntry(manufacturer(),model());
}

bool KMDriverDbWidget::isRaw()
{
	return m_raw->isChecked();
}

void KMDriverDbWidget::init()
{
	QApplication::setOverrideCursor(waitCursor);
	KMDriverDB::self()->init(this);
}

void KMDriverDbWidget::slotDbLoaded(bool reloaded)
{
	QApplication::restoreOverrideCursor();
	if (reloaded || m_manu->count() == 0)
	{ // do something only if DB reloaded
		m_manu->clear();
		m_model->clear();
		QDictIterator< QDict<KMDBEntryList> >	it(KMDriverDB::self()->manufacturers());
		for (;it.current();++it)
			m_manu->insertItem(it.currentKey());
		m_manu->sort();
		m_manu->setCurrentItem(0);
	}
}

void KMDriverDbWidget::slotManufacturerSelected(const QString& name)
{
	m_model->clear();
	QDict<KMDBEntryList>	*models = KMDriverDB::self()->findModels(name);
	if (models)
	{
		QStrIList	ilist(true);
		QDictIterator<KMDBEntryList>	it(*models);
		for (;it.current();++it)
			ilist.append(it.currentKey().latin1());
		ilist.sort();
		m_model->insertStrList(&ilist);
		m_model->setCurrentItem(0);
	}
}

void KMDriverDbWidget::slotPostscriptToggled(bool on)
{
	if (on)
	{
		QListBoxItem	*item = m_manu->findItem("POSTSCRIPT");
		if (item)
			m_manu->setCurrentItem(item);
		else
		{
			KMessageBox::error(this,i18n("Unable to find the Postscript driver."));
		}
	}
}

void KMDriverDbWidget::slotOtherClicked()
{
	if (m_external.isEmpty())
	{
		QString	filename = KFileDialog::getOpenFileName(QString::null,QString::null,this);
		if (!filename.isEmpty())
		{
			DrMain	*driver = KMFactory::self()->manager()->loadFileDriver(filename);
			if (driver)
			{
				m_external = filename;
				disconnect(m_manu,SIGNAL(highlighted(const QString&)),this,SLOT(slotManufacturerSelected(const QString&)));
				m_manu->clear();
				m_model->clear();
				QString	s = driver->get("manufacturer");
				m_manu->insertItem((s.isEmpty() ? i18n("<Unknown>") : s));
				s = driver->get("model");
				m_model->insertItem((s.isEmpty() ? i18n("<Unknown>") : s));
				m_manu->setCurrentItem(0);
				m_model->setCurrentItem(0);
				m_other->setText(i18n("Database"));
				m_desc = driver->get("description");
				delete driver;
			}
			else
				KMessageBox::error(this,i18n("Wrong driver format."));
		}
	}
	else
	{
		m_external = QString::null;
		connect(m_manu,SIGNAL(highlighted(const QString&)),this,SLOT(slotManufacturerSelected(const QString&)));
		m_other->setText(i18n("Other"));
		m_desc = QString::null;
		slotDbLoaded(true);
	}
}
#include "kmdriverdbwidget.moc"
