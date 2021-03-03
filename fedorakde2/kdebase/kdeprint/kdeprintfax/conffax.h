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

#ifndef CONFFAX_H
#define CONFFAX_H

#include <qwidget.h>

class QComboBox;
class QLineEdit;

class ConfFax : public QWidget
{
public:
	ConfFax(QWidget *parent = 0, const char *name = 0);

	void load();
	void save();

private:
	QComboBox	*m_device, *m_resolution, *m_pagesize;
	QLineEdit	*m_host;
};

#endif
