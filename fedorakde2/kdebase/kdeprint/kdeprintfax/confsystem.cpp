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

#include "confsystem.h"
#include "defcmds.h"

#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>

#include <kglobal.h>
#include <kconfig.h>
#include <klocale.h>

#define	EFAX_ID		0
#define	HYLAFAX_ID	1

ConfSystem::ConfSystem(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	m_system = new QButtonGroup(this);
	m_system->hide();
	m_efax = new QLineEdit(this);
	m_hfax = new QLineEdit(this);
	QRadioButton	*m_efaxbtn = new QRadioButton(i18n("Use EFax system"), this);
	QRadioButton	*m_hfaxbtn = new QRadioButton(i18n("Use HylaFax system"), this);
	connect(m_efaxbtn, SIGNAL(toggled(bool)), m_efax, SLOT(setEnabled(bool)));
	connect(m_efaxbtn, SIGNAL(toggled(bool)), m_hfax, SLOT(setDisabled(bool)));
	m_system->insert(m_efaxbtn, EFAX_ID);
	m_system->insert(m_hfaxbtn, HYLAFAX_ID);
	m_system->setButton(EFAX_ID);

	QVBoxLayout	*l0 = new QVBoxLayout(this, 10, 5);
	l0->addWidget(m_efaxbtn);
	QHBoxLayout	*l1 = new QHBoxLayout(0, 0, 0), *l2 = new QHBoxLayout(0, 0, 0);
	l0->addLayout(l1);
	l1->addSpacing(20);
	l1->addWidget(m_efax, 1);
	l0->addSpacing(20);
	l0->addWidget(m_hfaxbtn);
	l0->addLayout(l2);
	l2->addSpacing(20);
	l2->addWidget(m_hfax, 1);
	l0->addStretch(1);
}

void ConfSystem::load()
{
	KConfig	*conf = KGlobal::config();
	conf->setGroup("System");
	m_efax->setText(conf->readEntry("EFax", efax_default_cmd));
	m_hfax->setText(conf->readEntry("HylaFax", hylafax_default_cmd));
	QString	v = conf->readEntry("System", "efax");
	if (v == "efax") m_system->setButton(EFAX_ID);
	if (v == "hylafax") m_system->setButton(HYLAFAX_ID);
	else m_system->setButton(EFAX_ID);
}

void ConfSystem::save()
{
	KConfig	*conf = KGlobal::config();
	conf->setGroup("System");
	conf->writeEntry("EFax", m_efax->text());
	conf->writeEntry("HylaFax", m_hfax->text());
	int	ID = m_system->id(m_system->selected());
	switch (ID)
	{
		case EFAX_ID: conf->writeEntry("System", "efax"); break;
		case HYLAFAX_ID: conf->writeEntry("System", "hylafax"); break;
	}
}
