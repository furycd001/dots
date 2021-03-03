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

#include "conffilters.h"
#include "filterdlg.h"

#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qfile.h>
#include <qtextstream.h>

#include <klocale.h>
#include <klistview.h>
#include <kiconloader.h>
#include <kstddirs.h>
#include <kmessagebox.h>

ConfFilters::ConfFilters(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	m_filters = new KListView(this);
	m_filters->addColumn(i18n("Mime Type"));
	m_filters->addColumn(i18n("Command"));
	m_filters->setFrameStyle(QFrame::WinPanel|QFrame::Sunken);
	m_filters->setLineWidth(1);
	m_filters->setSorting(-1);
	connect(m_filters, SIGNAL(doubleClicked(QListViewItem*)), SLOT(slotChange()));

	QPushButton	*m_add = new QPushButton(this);
	m_add->setPixmap(BarIcon("filenew"));
	QPushButton	*m_remove = new QPushButton(this);
	m_remove->setPixmap(BarIcon("remove"));
	QPushButton	*m_change = new QPushButton(this);
	m_change->setPixmap(BarIcon("filter"));
	QPushButton	*m_up = new QPushButton(this);
	m_up->setPixmap(BarIcon("up"));
	QPushButton	*m_down = new QPushButton(this);
	m_down->setPixmap(BarIcon("down"));
	connect(m_add, SIGNAL(clicked()), SLOT(slotAdd()));
	connect(m_change, SIGNAL(clicked()), SLOT(slotChange()));
	connect(m_remove, SIGNAL(clicked()), SLOT(slotRemove()));
	connect(m_up, SIGNAL(clicked()), SLOT(slotUp()));
	connect(m_down, SIGNAL(clicked()), SLOT(slotDown()));
	QToolTip::add(m_add, i18n("Add filter"));
	QToolTip::add(m_change, i18n("Modify filter"));
	QToolTip::add(m_remove, i18n("Remove filter"));
	QToolTip::add(m_up, i18n("Move filter up"));
	QToolTip::add(m_down, i18n("Move filter down"));

	QHBoxLayout	*l0 = new QHBoxLayout(this, 10, 10);
	QVBoxLayout	*l1 = new QVBoxLayout(0, 0, 0);
	l0->addWidget(m_filters, 1);
	l0->addLayout(l1, 0);
	l1->addWidget(m_add);
	l1->addWidget(m_change);
	l1->addWidget(m_remove);
	l1->addSpacing(10);
	l1->addWidget(m_up);
	l1->addWidget(m_down);
	l1->addStretch(1);
}

void ConfFilters::load()
{
	QFile	f(locate("data","kdeprintfax/faxfilters"));
	if (f.exists() && f.open(IO_ReadOnly))
	{
		QTextStream	t(&f);
		QString		line;
		int		p(-1);
		QListViewItem	*item(0);
		while (!t.eof())
		{
			line = t.readLine().stripWhiteSpace();
			if ((p=line.find(QRegExp("\\s"))) != -1)
			{
				QString	mime(line.left(p)), cmd(line.right(line.length()-p-1).stripWhiteSpace());
				if (!mime.isEmpty() && !cmd.isEmpty())
					item = new QListViewItem(m_filters, item, mime, cmd);
			}
		}
	}
}

void ConfFilters::save()
{
	QListViewItem	*item = m_filters->firstChild();
	QFile	f(locateLocal("data","kdeprintfax/faxfilters"));
	if (f.open(IO_WriteOnly))
	{
		QTextStream	t(&f);
		while (item)
		{
			t << item->text(0) << ' ' << item->text(1) << endl;
			item = item->nextSibling();
		}
	}
}

void ConfFilters::slotAdd()
{
	QString	mime, cmd;
	if (FilterDlg::doIt(this, &mime, &cmd))
		if (!mime.isEmpty() && !cmd.isEmpty())
			new QListViewItem(m_filters, m_filters->currentItem(), mime, cmd);
		else
			KMessageBox::error(this, i18n("Empty parameters."));
}

void ConfFilters::slotRemove()
{
	QListViewItem	*item = m_filters->currentItem();
	if (item)
		delete item;
}

void ConfFilters::slotChange()
{
	QListViewItem	*item = m_filters->currentItem();
	if (item)
	{
		QString	mime(item->text(0)), cmd(item->text(1));
		if (FilterDlg::doIt(this, &mime, &cmd))
		{
			item->setText(0, mime);
			item->setText(1, cmd);
		}
	}
}

void ConfFilters::slotUp()
{
	QListViewItem	*item = m_filters->currentItem();
	if (item && item->itemAbove())
	{
		m_filters->moveItem(item, 0, item->itemAbove()->itemAbove());
		m_filters->setCurrentItem(item);
	}
}

void ConfFilters::slotDown()
{
	QListViewItem	*item = m_filters->currentItem();
	if (item && item->itemBelow())
	{
		m_filters->moveItem(item, 0, item->itemBelow());
		m_filters->setCurrentItem(item);
	}
}

#include "conffilters.moc"
