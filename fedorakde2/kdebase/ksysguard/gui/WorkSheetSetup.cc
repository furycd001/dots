/*
    KSysGuard, the KDE Task Manager and System Monitor
   
	Copyright (c) 1999, 2000 Chris Schlaeger <cs@kde.org>
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General Public
    License as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

	KSysGuard is currently maintained by Chris Schlaeger <cs@kde.org>. Please do
	not commit any changes without consulting me first. Thanks!

	$Id: WorkSheetSetup.cc,v 1.5 2001/05/25 14:46:14 cschlaeg Exp $
*/

#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>

#include <knuminput.h>
#include <klocale.h>

#include "WorkSheetSetup.h"
#include "WorkSheetSetup.moc"

WorkSheetSetup::WorkSheetSetup(const QString& defSheetName)
	: KDialogBase(0, 0, true, QString::null, Ok | Cancel)
{
	mainWidget = new QWidget(this);
	CHECK_PTR(mainWidget);
	QVBoxLayout* vLay = new QVBoxLayout(mainWidget, 0, spacingHint());
	CHECK_PTR(vLay);

	// create input field for sheet name
	QHBoxLayout* subLay = new QHBoxLayout();
	CHECK_PTR(subLay);
	vLay->addLayout(subLay);
	QLabel* sheetName = new QLabel(i18n("Name:"), mainWidget, "sheetName");
	CHECK_PTR(sheetName);
	subLay->addWidget(sheetName);
	sheetNameLE = new QLineEdit(defSheetName, mainWidget, "sheetNameLE");
	CHECK_PTR(sheetNameLE);
	sheetNameLE->setMinimumWidth(fontMetrics().maxWidth() * 10);
	subLay->addWidget(sheetNameLE);

	// create numeric input for number of columns
	subLay = new QHBoxLayout();
	CHECK_PTR(subLay);
	vLay->addLayout(subLay);
	QLabel* rowLB = new QLabel(i18n("Rows:"), mainWidget, "rowLB");
	CHECK_PTR(rowLB);
	subLay->addWidget(rowLB);
	rowNI = new KIntNumInput(1, mainWidget, 10, "rowNI");
	CHECK_PTR(rowNI);
	rowNI->setRange(1, 20);
	subLay->addWidget(rowNI);

	// create numeric input for number of columns
	subLay = new QHBoxLayout();
	CHECK_PTR(subLay);
	vLay->addLayout(subLay);
	QLabel* colLB = new QLabel(i18n("Columns:"), mainWidget, "colLB");
	CHECK_PTR(colLB);
	subLay->addWidget(colLB);
	colNI = new KIntNumInput(1, mainWidget, 10, "colNI");
	CHECK_PTR(colNI);
	colNI->setRange(1, 20);
	subLay->addWidget(colNI);

	vLay->addStretch(10);

	setMainWidget(mainWidget);
}

QString
WorkSheetSetup::getSheetName() const
{
	return (sheetNameLE->text());
}

int WorkSheetSetup::getRows() const
{
	return (rowNI->value());
}

int WorkSheetSetup::getColumns() const
{
	return (colNI->value());
}
