/* This file is part of the KDE Display Manager Configuration package

    Copyright (C) 2000 Oswald Buddenhagen <ossi@kde.org>
    Based on several other files.

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

#ifndef __KDMCONV_H__
#define __KDMCONV_H__

#include <qlist.h>
#include <qstring.h>
#include <qimage.h>
#include <qgroupbox.h>
//#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>

#include <klistbox.h>
#include <kcombobox.h>
#include <kcolorbtn.h>
#include <kurl.h>

#include <pwd.h>

#include <kcmodule.h>


class KDMConvenienceWidget : public KCModule
{
	Q_OBJECT

public:
	KDMConvenienceWidget(QWidget *parent=0, const char *name=0, QStringList *show_users=0);

        void load(QStringList *show_users=0);
        void save();
	void defaults();

private slots:
        void addShowUser(const QString &user);
        void removeShowUser(const QString &user);
	void slotWpToNp();
	void slotNpToWp();
        void slotEnALChanged();
        void slotPresChanged();
        void slotEnPLChanged();
	void slotChanged();

private:
	void removeText(QListBox *lb, const QString &user);
	void updateButton();

	QGroupBox	*alGroup, *puGroup, *npGroup, *btGroup;
	QCheckBox	*cbalen, *cbal1st, *cbplen, *cbarlen, *cbjumppw;
	QRadioButton	*npRadio, *ppRadio, *spRadio;
	KComboBox	*userlb, *puserlb;
	KListBox	*wpuserlb, *npuserlb;
	QPushButton	*np_to_wp, *wp_to_np;
	QLabel		*u_label, *pu_label, *w_label, *n_label;
};

#endif


