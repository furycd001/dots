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

#include "kcupsprinterimpl.h"
#include "kprinter.h"
#include "driver.h"
#include "kmfactory.h"
#include "kmmanager.h"
#include "cupsinfos.h"

#include <qfile.h>
#include <cups/cups.h>
#include <stdlib.h>

static void mapToCupsOptions(const QMap<QString,QString>& opts, QString& cmd);

//******************************************************************************************************

KCupsPrinterImpl::KCupsPrinterImpl(QObject *parent, const char *name)
: KPrinterImpl(parent,name)
{
}

KCupsPrinterImpl::~KCupsPrinterImpl()
{
}

bool KCupsPrinterImpl::setupCommand(QString& cmd, KPrinter *printer)
{
	// check printer object
	if (!printer) return false;

	cmd = QString::fromLatin1("cupsdoprint -P '%1' -J '%4' -H '%2:%3'").arg(printer->printerName()).arg(CupsInfos::self()->host()).arg(CupsInfos::self()->port()).arg(printer->docName());
	if (!CupsInfos::self()->login().isEmpty())
	{
		QString	s(" -U '"+CupsInfos::self()->login());
		if (!CupsInfos::self()->password().isEmpty())
			s += (":"+CupsInfos::self()->password());
		s.append('\'');
		cmd.append(s);
	}
	mapToCupsOptions(printer->options(),cmd);
	return true;
}

void KCupsPrinterImpl::preparePrinting(KPrinter *printer)
{
	// process orientation
	QString	o = printer->option("orientation-requested");
	printer->setOption("kde-orientation",(o == "4" || o == "5" ? "Landscape" : "Portrait"));
	// if it's a Qt application, then convert orientation as it will be handled by Qt directly
	if (printer->applicationType() == KPrinter::Dialog)
		printer->setOption("orientation-requested",(o == "5" || o == "6" ? "6" : "3"));

	// translate copies number
	if (!printer->option("kde-copies").isEmpty()) printer->setOption("copies",printer->option("kde-copies"));

	// page ranges are handled by CUPS, so application should print all pages
	if (printer->pageSelection() == KPrinter::SystemSide)
	{ // Qt => CUPS
		// translations
		if (!printer->option("kde-range").isEmpty())
			printer->setOption("page-ranges",printer->option("kde-range"));
		if (printer->option("kde-pageorder") == "Reverse")
			printer->setOption("OutputOrder",printer->option("kde-pageorder"));
		o = printer->option("kde-pageset");
		if (!o.isEmpty() && o != "0")
			printer->setOption("page-set",(o == "1" ? "odd" : "even"));
		printer->setOption("multiple-document-handling",(printer->option("kde-collate") == "Collate" ? "separate-documents-collated-copies" : "separate-documents-uncollated-copies"));
	}
	else
	{ // No translation needed (but range => (from,to))
		QString range = printer->option("kde-range");
		if (!range.isEmpty())
		{
			QSize	s = rangeToSize(range);
			printer->setOption("kde-from",QString::number(s.width()));
			printer->setOption("kde-to",QString::number(s.height()));
		}
	}

	// page size -> try to find page size from driver file
	KMManager	*mgr = KMFactory::self()->manager();
	DrMain	*driver = mgr->loadPrinterDriver(mgr->findPrinter(printer->printerName()), false);
	if (driver)
	{
		QString	psname = printer->option("PageSize");
		if (psname.isEmpty())
		{
			DrListOption	*opt = (DrListOption*)driver->findOption("PageSize");
			if (opt) psname = opt->get("default");
		}
		if (!psname.isEmpty())
		{
			printer->setOption("kde-pagesize",QString::number((int)pageNameToPageSize(psname)));
			DrPageSize	*ps = driver->findPageSize(psname);
			if (ps)
			{
				printer->setRealPageSize(ps->pageSize());
				printer->setMargins(ps->margins());
			}
		}
		delete driver;
	}

	KPrinterImpl::preparePrinting(printer);
}

void KCupsPrinterImpl::broadcastOption(const QString& key, const QString& value)
{
	KPrinterImpl::broadcastOption(key,value);
	if (key == "kde-orientation")
		KPrinterImpl::broadcastOption("orientation-requested",(value == "Landscape" ? "4" : "3"));
	else if (key == "kde-pagesize")
	{
		QString	pagename = QString::fromLatin1(pageSizeToPageName((KPrinter::PageSize)value.toInt()));
		KPrinterImpl::broadcastOption("PageSize",pagename);
		// simple hack for classes
		KPrinterImpl::broadcastOption("media",pagename);
	}
}

//******************************************************************************************************

static void mapToCupsOptions(const QMap<QString,QString>& opts, QString& cmd)
{
	QString	optstr;
	for (QMap<QString,QString>::ConstIterator it=opts.begin(); it!=opts.end(); ++it)
	{
		// only encode those options that doesn't start with "kde-" or "app-".
		if (!it.key().startsWith("kde-") && !it.key().startsWith("app-"))
		{
			optstr.append(" ").append(it.key());
			if (!it.data().isEmpty())
				optstr.append("=").append(it.data());
		}
	}
	if (!optstr.isEmpty())
		cmd.append(" -o '").append(optstr).append("'");
}
