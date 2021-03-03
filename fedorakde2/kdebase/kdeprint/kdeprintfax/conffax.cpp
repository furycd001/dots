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

#include "conffax.h"

#include <qcombobox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>

#include <kglobal.h>
#include <kconfig.h>
#include <klocale.h>
#include <kseparator.h>

#include <stdlib.h>

ConfFax::ConfFax(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	m_device = new QComboBox(this);
	m_device->setMinimumHeight(25);
	m_resolution = new QComboBox(this);
	m_resolution->setMinimumHeight(25);
	m_pagesize = new QComboBox(this);
	m_pagesize->setMinimumHeight(25);
	m_device->insertItem(i18n("Standard Modem Port"));
	for (int i=0; i<10; i++)
		m_device->insertItem(i18n("Serial Port #%1").arg(i));
	m_resolution->insertItem(i18n("High (204x196 dpi)"));
	m_resolution->insertItem(i18n("Low (204x98 dpi)"));
	m_pagesize->insertItem(i18n("A4"));
	m_pagesize->insertItem(i18n("Letter"));
	m_pagesize->insertItem(i18n("Legal"));
	m_host = new QLineEdit(this);
	QLabel	*m_devicelabel = new QLabel(i18n("Fax/Modem device:"), this);
	QLabel	*m_resolutionlabel = new QLabel(i18n("Resolution:"), this);
	QLabel	*m_pagesizelabel = new QLabel(i18n("Paper size:"), this);
	QLabel	*m_hostlabel = new QLabel(i18n("Fax server (if any):"), this);
        KSeparator *m_line = new KSeparator( KSeparator::HLine, this);

	QGridLayout	*l0 = new QGridLayout(this, 6, 2, 10, 10);
	l0->setColStretch(1, 1);
	l0->setRowStretch(5, 1);
	l0->addWidget(m_devicelabel, 0, 0);
	l0->addWidget(m_hostlabel, 1, 0);
	l0->addWidget(m_resolutionlabel, 3, 0);
	l0->addWidget(m_pagesizelabel, 4, 0);
	l0->addWidget(m_device, 0, 1);
	l0->addWidget(m_host, 1, 1);
	l0->addWidget(m_resolution, 3, 1);
	l0->addWidget(m_pagesize, 4, 1);
	l0->addMultiCellWidget(m_line, 2, 2, 0, 1);
	l0->addRowSpacing(2, 5);
}

void ConfFax::load()
{
	KConfig	*conf = KGlobal::config();
	conf->setGroup("Fax");
	QString	v = conf->readEntry("Device", "modem");
	if (v.startsWith("ttyS")) m_device->setCurrentItem(v.right(v.length()-4).toInt()+1);
	else m_device->setCurrentItem(0);
	m_host->setText(conf->readEntry("Server", getenv("FAXSERVER")));
	v = conf->readEntry("Page", "a4");
	if (v == "letter") m_pagesize->setCurrentItem(1);
	else if (v == "legal") m_pagesize->setCurrentItem(2);
	else m_pagesize->setCurrentItem(0);
	v = conf->readEntry("Resolution", "High");
	m_resolution->setCurrentItem((v == "Low" ? 1 : 0));
}

void ConfFax::save()
{
	KConfig	*conf = KGlobal::config();
	conf->setGroup("Fax");
	if (m_device->currentItem() == 0)
		conf->writeEntry("Device", "modem");
	else
		conf->writeEntry("Device", QString::fromLatin1("ttyS%1").arg(m_device->currentItem()-1));
	conf->writeEntry("Server", m_host->text());
	conf->writeEntry("Resolution", (m_resolution->currentItem() == 0 ? "High" : "Low"));
	conf->writeEntry("Page", (m_pagesize->currentItem() == 0 ? "a4" : (m_pagesize->currentItem() == 1 ? "letter" : "legal")));
}
