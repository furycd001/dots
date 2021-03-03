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

#ifndef FAXAB_H
#define FAXAB_H

#include <kdialog.h>
#include <qmap.h>
#include <qstringlist.h>

class KListBox;

class FaxAB : public KDialog
{
	Q_OBJECT;
public:
	FaxAB(QWidget *parent = 0, const char *name = 0);
	bool isValid();

	static bool getEntry(QString& number, QString& name, QString& enterprise, QWidget *parent = 0);

protected slots:
	void slotSelected(const QString&);

private:
	KListBox	*m_name, *m_fax;
	QMap<QString,QStringList>	m_entries;
};

#endif
