/* This file is part of the KDE Display Manager Configuration package
    Copyright (C) 1997 Thomas Tanghus (tanghus@earthling.net)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/  

#ifndef __KDMSESS_H__
#define __KDMSESS_H__


#include <qlist.h>
#include <qstring.h>
#include <qlistbox.h>
#include <kdbtn.h>

#include <kcmodule.h>


class QLineEdit;
class KURLRequester;
class QComboBox;
class QCheckBox;

class MyListBox : public QListBox
{

public:
	MyListBox(QWidget *parent) : QListBox(parent) {}
	bool isItemVisible(int id) { return itemVisible(id); }
};

class KDMSessionsWidget : public KCModule
{
	Q_OBJECT

public:
	KDMSessionsWidget(QWidget *parent=0, const char *name=0);

	void load();
	void save();
	void defaults();

	enum SdModes { SdAll, SdRoot, SdNone };
	
protected:
	void moveSession(int);

protected slots:
	void slotLiloCheckToggled(bool);
	void slotAddSessionType();
	void slotRemoveSessionType();
	void slotSessionHighlighted(int);
	void slotCheckNewSession(const QString&);
	void slotSessionUp();
	void slotSessionDown();
	void changed();

private:
	void readSD (QComboBox *, QString);
	void writeSD (QComboBox *);

	KIconLoader	*iconloader;
	QComboBox	*sdlcombo, *sdrcombo;
	QLabel		*sdllabel, *sdrlabel;
	KURLRequester   *restart_lined, *shutdown_lined;
	QLineEdit	*session_lined;
	MyListBox	*sessionslb;
	KDirectionButton	*btnup, *btndown;
	QButton		*btnrm, *btnadd;
#ifdef __linux__
	QCheckBox	*lilo_check;
	QLabel		*lilocmd_label, *lilomap_label;
	KURLRequester	*lilocmd_lined, *lilomap_lined;
#endif
};


#endif


