/*
 *  This file is part of the KDE libraries
 *  Copyright (c) 2001 Michael Goffioul <goffioul@imec.be>
 *
 *  $Id$
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

#include "kmuimanager.h"
#include "kprintdialog.h"
#include "kprintdialogpage.h"
#include "kpcopiespage.h"
#include "kprinter.h"
#include "kprinterpropertydialog.h"
#include "kmfactory.h"
#include "kmmanager.h"
#include "kmprinter.h"
#include "kpdriverpage.h"
#include "kpqtpage.h"
#include "kpfilterpage.h"

#include <klocale.h>

KMUiManager::KMUiManager(QObject *parent, const char *name)
: QObject(parent,name)
{
	m_printdialogflags = (KMUiManager::PrintDialogAll & ~KMUiManager::Options);
	m_printdialogpages.setAutoDelete(false);
	m_copyflags = 0;
}

KMUiManager::~KMUiManager()
{
}

void KMUiManager::setupPropertyPages(KMPropertyPage*)
{
}

void KMUiManager::setupWizard(KMWizard*)
{
}

void KMUiManager::setupConfigDialog(KMConfigDialog*)
{
}

int KMUiManager::copyFlags(KPrinter *pr)
{
	int	fl(0);
	if (KMFactory::self()->settings()->pageSelection == KPrinter::ApplicationSide)
	{
		if (pr)
		{
			if (pr->currentPage() > 0) fl |= Current;
			if (pr->minPage() > 0 && pr->maxPage() > 0)
				fl |= (Range|PageSet|Order);
		}
		else fl = CopyAll;
		if (m_copyflags & NoAutoCollate) fl |= NoAutoCollate;
	}
	else fl = m_copyflags;
	return fl;
}

int KMUiManager::dialogFlags()
{
	int	f = m_printdialogflags;
	if (KMFactory::self()->settings()->application == KPrinter::StandAlone)
		f &= ~(KMUiManager::Preview);
	return f;
}

void KMUiManager::setupPrintDialog(KPrintDialog *dlg)
{
	// dialog flags
	dlg->setFlags(dialogFlags());

	// dialog pages
	if (KMFactory::self()->settings()->standardDialogPages & KPrinter::CopiesPage)
		m_printdialogpages.insert(0,new KPCopiesPage(dlg->printer(), 0, "CopiesPage"));
	dlg->setDialogPages(&m_printdialogpages);
}

void KMUiManager::setupPropertyDialog(KPrinterPropertyDialog *dlg)
{
	if (dlg->printer())
	{
		if (dlg->printer()->isSpecial())
		{ // special case
			dlg->addPage(new KPQtPage(dlg,"QtPage"));
			dlg->setDefaultButton(QString::null);
		}
		else
		{
			// check for a driver
			DrMain	*driver = KMFactory::self()->manager()->loadPrinterDriver(dlg->printer(), false);
			dlg->setDriver(driver);

			// add pages specific to print system
			setupPrinterPropertyDialog(dlg);

			// add driver page
			if (driver)
				dlg->addPage(new KPDriverPage(dlg->printer(),driver,dlg,"DriverPage"));

		}
		dlg->setCaption(i18n("Configuration of %1").arg(dlg->printer()->name()));
		dlg->addPage(new KPFilterPage(dlg,"FilterPage"));
		dlg->resize(100,100);
	}
}

void KMUiManager::setupPrinterPropertyDialog(KPrinterPropertyDialog *dlg)
{
	if (KMFactory::self()->settings()->application == KPrinter::Dialog)
		dlg->addPage(new KPQtPage(dlg,"QtPage"));
}
