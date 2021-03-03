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

#include "confgeneral.h"

#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>

#include <kglobal.h>
#include <kconfig.h>
#include <klocale.h>

#include <stdlib.h>

ConfGeneral::ConfGeneral(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	m_name = new QLineEdit(this);
	m_company = new QLineEdit(this);
	m_number = new QLineEdit(this);
	QLabel	*m_namelabel = new QLabel(i18n("Name:"), this);
	QLabel	*m_companylabel = new QLabel(i18n("Company:"), this);
	QLabel	*m_numberlabel = new QLabel(i18n("Number:"), this);

	QGridLayout	*l0 = new QGridLayout(this, 4, 2, 10, 10);
	l0->setColStretch(1, 1);
	l0->setRowStretch(3, 1);
	l0->addWidget(m_namelabel, 0, 0);
	l0->addWidget(m_companylabel, 1, 0);
	l0->addWidget(m_numberlabel, 2, 0);
	l0->addWidget(m_name, 0, 1);
	l0->addWidget(m_company, 1, 1);
	l0->addWidget(m_number, 2, 1);
}

void ConfGeneral::load()
{
	KConfig	*conf = KGlobal::config();
	conf->setGroup("Personal");
	m_name->setText(conf->readEntry("Name", getenv("USER")));
	m_number->setText(conf->readEntry("Number"));
	m_company->setText(conf->readEntry("Company"));
}

void ConfGeneral::save()
{
	KConfig	*conf = KGlobal::config();
	conf->setGroup("Personal");
	conf->writeEntry("Name", m_name->text());
	conf->writeEntry("Number", m_number->text());
	conf->writeEntry("Company", m_company->text());
}
