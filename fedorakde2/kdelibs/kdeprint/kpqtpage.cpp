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

#include "kpqtpage.h"
#include "kprinter.h"

#include <qcombobox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <qlayout.h>

#include <kiconloader.h>
#include <klocale.h>

#define ORIENT_PORTRAIT_ID	0
#define ORIENT_LANDSCAPE_ID	1

#define COLORMODE_COLOR_ID	0
#define COLORMODE_GRAYSCALE_ID	1

void radioCursor(QButtonGroup*);

static struct pagesizestruct
{
	const char*	text;
	int 	ID;
} page_sizes[] =
{
	{ "A0", KPrinter::A0 },
	{ "A1", KPrinter::A1 },
	{ "A2", KPrinter::A2 },
	{ "A3", KPrinter::A3 },
	{ "A4", KPrinter::A4 },
	{ "A5", KPrinter::A5 },
	{ "A6", KPrinter::A6 },
	{ "A7", KPrinter::A7 },
	{ "A8", KPrinter::A8 },
	{ "A9", KPrinter::A9 },
	{ "B1", KPrinter::B1 },
	{ "B10", KPrinter::B10 },
	{ "B2", KPrinter::B2 },
	{ "B3", KPrinter::B3 },
	{ "B4", KPrinter::B4 },
	{ "B5", KPrinter::B5 },
	{ "B6", KPrinter::B6 },
	{ "B7", KPrinter::B7 },
	{ "B8", KPrinter::B8 },
	{ "B9", KPrinter::B9 },
	{ I18N_NOOP("Envelope C5"), KPrinter::C5E },
	{ I18N_NOOP("Envelope DL"), KPrinter::DLE },
	{ I18N_NOOP("Envelope US #10"), KPrinter::Comm10E },
	{ I18N_NOOP("Executive"), KPrinter::Executive },
	{ I18N_NOOP("Folio"), KPrinter::Folio },
	{ I18N_NOOP("Ledger"), KPrinter::Ledger },
	{ I18N_NOOP("Tabloid"), KPrinter::Tabloid },
	{ I18N_NOOP("US Legal"), KPrinter::Legal },
	{ I18N_NOOP("US Letter"), KPrinter::Letter }
};

int findIndex(int ID)
{
	for (int i=0; i<KPrinter::NPageSize-1; i++)
		if (page_sizes[i].ID == ID)
			return i;
	return 4;
}

//*****************************************************************************************************

KPQtPage::KPQtPage(QWidget *parent, const char *name)
: KPrintDialogPage(parent,name)
{
	setTitle(i18n("Print format"));

	// widget creation
	m_pagesize = new QComboBox(this);
	QLabel	*m_pagesizelabel = new QLabel(i18n("Page size:"), this);
	m_pagesizelabel->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
	m_orientbox = new QButtonGroup(0, Qt::Vertical, i18n("Orientation"), this);
	m_colorbox = new QButtonGroup(0, Qt::Vertical, i18n("Color mode"), this);
	QRadioButton	*m_portrait = new QRadioButton(i18n("Portrait"), m_orientbox);
	QRadioButton	*m_landscape = new QRadioButton(i18n("Landscape"), m_orientbox);
	m_orientpix = new QLabel(m_orientbox);
	m_orientpix->setAlignment(Qt::AlignCenter);
	QRadioButton	*m_color = new QRadioButton(i18n("Color"), m_colorbox);
	QRadioButton	*m_grayscale = new QRadioButton(i18n("Grayscale"), m_colorbox);
	m_colorpix = new QLabel(m_colorbox);
	m_colorpix->setAlignment(Qt::AlignCenter);

	// layout creation
	QGridLayout	*lay0 = new QGridLayout(this, 2, 2, 10, 10);
	lay0->setRowStretch(1,1);
	lay0->addWidget(m_pagesizelabel,0,0);
	lay0->addWidget(m_pagesize,0,1);
	lay0->addWidget(m_orientbox,1,0);
	lay0->addWidget(m_colorbox,1,1);
	QGridLayout	*lay1 = new QGridLayout(m_orientbox->layout(), 2, 2, 10);
	lay1->addWidget(m_portrait,0,0);
	lay1->addWidget(m_landscape,1,0);
	lay1->addMultiCellWidget(m_orientpix,0,1,1,1);
	QGridLayout	*lay2 = new QGridLayout(m_colorbox->layout(), 2, 2, 10);
	lay2->addWidget(m_color,0,0);
	lay2->addWidget(m_grayscale,1,0);
	lay2->addMultiCellWidget(m_colorpix,0,1,1,1);

	// initialization
	radioCursor(m_orientbox);
	radioCursor(m_colorbox);
	m_portrait->setChecked(true);
	slotOrientationChanged(0);
	m_color->setChecked(true);
	slotColorModeChanged(0);
	for (int i=0; i<KPrinter::NPageSize-1; i++)
		m_pagesize->insertItem(i18n(page_sizes[i].text));
	m_pagesize->setCurrentItem(findIndex(KPrinter::A4));	// default to A4

	// connections
	connect(m_orientbox,SIGNAL(clicked(int)),SLOT(slotOrientationChanged(int)));
	connect(m_colorbox,SIGNAL(clicked(int)),SLOT(slotColorModeChanged(int)));
}

KPQtPage::~KPQtPage()
{
}

void KPQtPage::slotOrientationChanged(int ID)
{
	m_orientpix->setPixmap(UserIcon((ID == ORIENT_PORTRAIT_ID ? "kdeprint_portrait" : "kdeprint_landscape")));
}

void KPQtPage::slotColorModeChanged(int ID)
{
	m_colorpix->setPixmap(UserIcon((ID == COLORMODE_COLOR_ID ? "kdeprint_color" : "kdeprint_grayscale")));
}

void KPQtPage::setOptions(const QMap<QString,QString>& opts)
{
	int 	ID = (opts["kde-orientation"] == "Landscape" ? ORIENT_LANDSCAPE_ID : ORIENT_PORTRAIT_ID);
	m_orientbox->setButton(ID);
	slotOrientationChanged(ID);
	ID = (opts["kde-colormode"] == "GrayScale" ? COLORMODE_GRAYSCALE_ID : COLORMODE_COLOR_ID);
	m_colorbox->setButton(ID);
	slotColorModeChanged(ID);
	if (!opts["kde-pagesize"].isEmpty())
		m_pagesize->setCurrentItem(findIndex(opts["kde-pagesize"].toInt()));
}

void KPQtPage::getOptions(QMap<QString,QString>& opts, bool)
{
	opts["kde-orientation"] = (m_orientbox->id(m_orientbox->selected()) == ORIENT_LANDSCAPE_ID ? "Landscape" : "Portrait");
	opts["kde-colormode"] = (m_colorbox->id(m_colorbox->selected()) == COLORMODE_GRAYSCALE_ID ? "GrayScale" : "Color");
	opts["kde-pagesize"] = QString::number(page_sizes[m_pagesize->currentItem()].ID);
}
#include "kpqtpage.moc"
