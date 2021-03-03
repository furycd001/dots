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

#ifndef CONFIGDLG_H
#define CONFIGDLG_H

#include <kdialogbase.h>

class ConfGeneral;
class ConfFax;
class ConfSystem;
class ConfFilters;

class ConfigDlg : public KDialogBase
{
public:
	static bool configure(QWidget *parent = 0);

protected:
	ConfigDlg(QWidget *parent = 0, const char *name = 0);
	void load();
	void save();

private:
	ConfGeneral	*m_general;
	ConfFax		*m_fax;
	ConfSystem	*m_system;
	ConfFilters	*m_filters;
};

#endif
