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

#ifndef CONFFILTERS_H
#define CONFFILTERS_H

#include <qwidget.h>

class KListView;

class ConfFilters : public QWidget
{
	Q_OBJECT
public:
	ConfFilters(QWidget *parent = 0, const char *name = 0);

	void load();
	void save();

protected slots:
	void slotAdd();
	void slotRemove();
	void slotChange();
	void slotUp();
	void slotDown();

private:
	KListView	*m_filters;
};

#endif
