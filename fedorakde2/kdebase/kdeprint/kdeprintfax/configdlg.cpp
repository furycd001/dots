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

#include "configdlg.h"
#include "confgeneral.h"
#include "conffax.h"
#include "confsystem.h"
#include "conffilters.h"

#include <qvbox.h>
#include <klocale.h>
#include <kiconloader.h>

ConfigDlg::ConfigDlg(QWidget *parent, const char *name)
: KDialogBase(IconList, i18n("Configuration"), Ok|Cancel, Ok, parent, name, true)
{
	QVBox	*page1 = addVBoxPage(i18n("Personal"), i18n("Personal settings"), DesktopIcon("kdmconfig"));
	m_general = new ConfGeneral(page1, "Personal");

	QVBox	*page2 = addVBoxPage(i18n("Fax"), i18n("Fax settings"), DesktopIcon("kdeprintfax"));
	m_fax = new ConfFax(page2, "Fax");

	QVBox	*page3 = addVBoxPage(i18n("System"), i18n("Fax system selection"), DesktopIcon("gear"));
	m_system = new ConfSystem(page3, "System");

	QVBox	*page4 = addVBoxPage(i18n("Filters"), i18n("Filters configuration"), DesktopIcon("filter"));
	m_filters = new ConfFilters(page4, "Filters");

	resize(400, 300);
}

void ConfigDlg::load()
{
	m_general->load();
	m_fax->load();
	m_system->load();
	m_filters->load();
}

void ConfigDlg::save()
{
	m_general->save();
	m_fax->save();
	m_system->save();
	m_filters->save();
}

bool ConfigDlg::configure(QWidget *parent)
{
	ConfigDlg	dlg(parent);
	dlg.load();
	if (dlg.exec())
	{
		dlg.save();
		return true;
	}
	return false;
}
