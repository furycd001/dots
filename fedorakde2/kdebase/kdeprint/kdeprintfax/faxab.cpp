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

#include "faxab.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <klistbox.h>
#include <klocale.h>
#include <addressbook.h>
#include <kmessagebox.h>
#include <kiconloader.h>

FaxAB::FaxAB(QWidget *parent, const char *name)
: KDialog(parent, name, true)
{
	m_name = new KListBox(this);
	m_fax = new KListBox(this);
	QLabel	*m_namelabel = new QLabel(i18n("Entries:"), this);
	QLabel	*m_faxlabel = new QLabel(i18n("Fax number:"), this);
	QPushButton	*m_ok = new QPushButton(i18n("OK"), this);
	QPushButton	*m_cancel = new QPushButton(i18n("Cancel"), this);
	connect(m_ok, SIGNAL(clicked()), SLOT(accept()));
	connect(m_cancel, SIGNAL(clicked()), SLOT(reject()));
	connect(m_name, SIGNAL(highlighted(const QString&)), SLOT(slotSelected(const QString&)));
	m_ok->setDefault(true);

	QVBoxLayout	*l0 = new QVBoxLayout(this, 10, 10);
	QGridLayout	*l1 = new QGridLayout(0, 2, 3, 0, 0);
	l0->addLayout(l1, 1);
	l1->setRowStretch(1, 1);
	l1->addColSpacing(1, 10);
	l1->addWidget(m_namelabel, 0, 0);
	l1->addWidget(m_faxlabel, 0, 2);
	l1->addWidget(m_name, 1, 0);
	l1->addWidget(m_fax, 1, 2);
	QHBoxLayout	*l2 = new QHBoxLayout(0, 0, 10);
	l0->addLayout(l2, 0);
	l2->addStretch(1);
	l2->addWidget(m_ok, 0);
	l2->addWidget(m_cancel, 0);

	resize(400, 200);

	AddressBook	bk(this);
	if (bk.noOfEntries() > 0)
	{
		std::list<AddressBook::Entry>	l;
		if (bk.getEntries(l) == AddressBook::NoError)
			for (std::list<AddressBook::Entry>::iterator it=l.begin(); it!= l.end(); ++it)
			{
				QStringList	n;
				for (uint i=0;i<(*it).telephone.count();i+=2)
					if ((*it).telephone[i] == "2")
						n.append((*it).telephone[i+1]);
				if (n.count() > 0)
				{
					QString	id = (*it).lastname + ", " + (*it).firstname;
					if (!(*it).middlename.isEmpty())
						id += " " + (*it).middlename;
					m_entries[id] = n;
				}
			}
	}

	if (m_entries.count() > 0)
	{
		for (QMap<QString,QStringList>::ConstIterator it=m_entries.begin(); it!=m_entries.end(); ++it)
			m_name->insertItem(SmallIcon("abentry"), it.key());
		m_name->sort();
		m_name->setCurrentItem(0);
	}
	else
		m_ok->setDisabled(true);
}

void FaxAB::slotSelected(const QString& text)
{
	if (m_entries.contains(text))
	{
		QStringList	l = m_entries[text];
		m_fax->clear();
		for (QStringList::ConstIterator it=l.begin(); it!=l.end(); ++it)
			m_fax->insertItem(SmallIcon("kdeprintfax"), *it);
		m_fax->sort();
		m_fax->setCurrentItem(0);
	}
}

bool FaxAB::getEntry(QString& number, QString& name, QString& enterprise, QWidget *parent)
{
	FaxAB	kab(parent);
	if (!kab.isValid())
	{
		KMessageBox::error(parent, i18n("No fax number found in your address book."));
		return false;
	}
	if (kab.exec())
	{
		number = kab.m_fax->currentText();
		name = kab.m_name->currentText();
		enterprise = i18n("Unknown");
		return true;
	}
	else return false;
}

bool FaxAB::isValid()
{
	return (m_name->count() > 0);
}

#include "faxab.moc"
