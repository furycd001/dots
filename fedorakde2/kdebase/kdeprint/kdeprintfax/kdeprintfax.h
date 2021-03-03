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

#ifndef KDEPRINTFAX_H
#define KDEPRINTFAX_H

#include <kmainwindow.h>
#include <kurl.h>

class KListBox;
class QLineEdit;
class QMultiLineEdit;
class FaxCtrl;
class QLabel;

class KdeprintFax : public KMainWindow
{
	Q_OBJECT
public:
	KdeprintFax(QWidget *parent = 0, const char *name = 0);

	void addURL(KURL url);
	QStringList files();
	QString number();
	QString name();
	QString enterprise();
	QString comment();

protected slots:
	void slotToggleToolBar();
	void slotToggleMenuBar();
	void slotKab();
	void slotAdd();
	void slotRemove();
	void slotFax();
	void slotAbort();
	void slotMessage(const QString&);
	void slotFaxSent(bool);
	void slotViewLog();
	void slotConfigure();
	void slotQuit();

protected:
	void initActions();
	void dragEnterEvent(QDragEnterEvent*);
	void dropEvent(QDropEvent*);
	void updateState();

private:
	KListBox	*m_files;
	QLineEdit	*m_number, *m_name, *m_enterprise;
	QMultiLineEdit	*m_comment;
	FaxCtrl		*m_faxctrl;
	QLabel		*m_msglabel;
};

#endif
