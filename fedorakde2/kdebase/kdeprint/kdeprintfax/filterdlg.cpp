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

#include "filterdlg.h"

#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>

#include <klocale.h>

FilterDlg::FilterDlg(QWidget *parent, const char *name)
: KDialogBase(parent, name, true, i18n("Filter parameters"), Ok|Cancel, Ok)
{
	QWidget	*w = new QWidget(this);

	m_mime = new QLineEdit(w);
	m_cmd = new QLineEdit(w);
	QLabel	*m_mimelabel = new QLabel(i18n("Mime Type:"), w);
	QLabel	*m_cmdlabel = new QLabel(i18n("Command:"), w);

	QGridLayout	*l0 = new QGridLayout(w, 2, 2, 10, 5);
	l0->setColStretch(1, 1);
	l0->addWidget(m_mimelabel, 0, 0);
	l0->addWidget(m_cmdlabel, 1, 0);
	l0->addWidget(m_mime, 0, 1);
	l0->addWidget(m_cmd, 1, 1);

	setMainWidget(w);

	resize(300, 100);
}

bool FilterDlg::doIt(QWidget *parent, QString *mime, QString *cmd)
{
	FilterDlg	dlg(parent);
	if (mime) dlg.m_mime->setText(*mime);
	if (cmd) dlg.m_cmd->setText(*cmd);
	if (dlg.exec())
	{
		if (mime) *mime = dlg.m_mime->text();
		if (cmd) *cmd = dlg.m_cmd->text();
		return true;
	}
	return false;
}
