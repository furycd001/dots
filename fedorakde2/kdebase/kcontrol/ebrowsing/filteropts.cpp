/*
 * Copyright (c) 2000 Yves Arrouye <yves@realnames.com>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <unistd.h>

#include <qlayout.h>
#include <qgroupbox.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qtabwidget.h>

#include <kapp.h>
#include <kbuttonbox.h>
#include <dcopclient.h>
#include <klocale.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kconfig.h>

#include "filteropts.h"

FilterOptions::FilterOptions(QWidget *parent, const char *name)
    		  :KCModule(parent, name)
{

    QGridLayout *lay = new QGridLayout(this, 1, 1, 10, 5);
    //QGroupBox =

    lay->addWidget(new QLabel(i18n("Under Construction..."), this), 0, 0);
    lay->activate();

    // load();
}

void FilterOptions::load()
{
	
}

void FilterOptions::save()
{
}

void FilterOptions::defaults()
{
    load();
}

void FilterOptions::moduleChanged(bool state)
{
    emit changed(state);
}

#include "filteropts.moc"
