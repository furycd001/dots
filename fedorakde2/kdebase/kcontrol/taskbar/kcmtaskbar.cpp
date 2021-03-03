/*
 *  Copyright (c) 2000 Kurt Granroth <granroth@kde.org>
 *  Copyright (c) 2000 Matthias Elter <elter@kde.org>
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
 */

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qwhatsthis.h>

#include <dcopclient.h>
#include <kapp.h>
#include <kconfig.h>
#include <kdialog.h>
#include <kglobal.h>
#include <klocale.h>

#include "kcmtaskbar.h"
#include "kcmtaskbar.moc"

extern "C"
{
    KCModule *create_taskbar(QWidget *parent, const char *name)
    {
	KGlobal::locale()->insertCatalogue("kcmtaskbar");
	return new TaskbarConfig(parent, name);
    };
}

TaskbarConfig::TaskbarConfig( QWidget *parent, const char* name )
  : KCModule (parent, name)
{
    ui = new TaskbarConfigUI(this);

    QVBoxLayout *vbox = new QVBoxLayout(this, KDialog::marginHint(),
                                        KDialog::spacingHint());
    vbox->addWidget(ui);
    connect(ui->showAllCheck, SIGNAL(clicked()), SLOT(configChanged()));
    connect(ui->showListBtnCheck, SIGNAL(clicked()), SLOT(configChanged()));
    connect(ui->groupCheck, SIGNAL(clicked()), SLOT(configChanged()));
    connect(ui->sortCheck, SIGNAL(clicked()), SLOT(configChanged()));
    connect(ui->iconCheck, SIGNAL(clicked()), SLOT(configChanged()));

    load();
}

TaskbarConfig::~TaskbarConfig()
{
}

void TaskbarConfig::configChanged()
{
    emit changed(true);
}

void TaskbarConfig::load()
{
    KConfig *c = new KConfig("ktaskbarrc", false, false);
    { // group for the benefit of the group saver
        KConfigGroupSaver saver(c, "General");

        ui->showAllCheck->setChecked(c->readBoolEntry("ShowAllWindows", true));
        ui->showListBtnCheck->setChecked(c->readBoolEntry("ShowWindowListBtn", true));
        ui->groupCheck->setChecked(c->readBoolEntry("GroupTasks", true));
	ui->sortCheck->setChecked(c->readBoolEntry("SortByDesktop", true));
	ui->iconCheck->setChecked(c->readBoolEntry("ShowIcon", true));
    }

    delete c;
    emit changed(false);
}

void TaskbarConfig::save()
{
    KConfig *c = new KConfig("ktaskbarrc", false, false);
    { // group for the benefit of the group saver
        KConfigGroupSaver saver(c, "General");

        c->writeEntry("ShowAllWindows", ui->showAllCheck->isChecked());
        c->writeEntry("ShowWindowListBtn", ui->showListBtnCheck->isChecked());
        c->writeEntry("GroupTasks", ui->groupCheck->isChecked());
	c->writeEntry("SortByDesktop", ui->sortCheck->isChecked());
	c->writeEntry("ShowIcon", ui->iconCheck->isChecked());
        c->sync();
    }

    delete c;

    emit changed(false);

    // Tell kicker about the new config file.
    if (!kapp->dcopClient()->isAttached())
        kapp->dcopClient()->attach();
    QByteArray data;
    kapp->dcopClient()->send( "kicker", "Panel", "restart()", data );
}

void TaskbarConfig::defaults()
{
    ui->showAllCheck->setChecked(true);
    ui->showListBtnCheck->setChecked(true);
    ui->groupCheck->setChecked(true);
    ui->sortCheck->setChecked(true);
    emit changed(true);
}

QString TaskbarConfig::quickHelp() const
{
    return i18n("<h1>Taskbar</h1> You can configure the taskbar here."
                " This includes options such as whether or not the taskbar should show all"
		" windows at once or only those on the current desktop."
                " You can also configure whether or not the Window List button will be displayed.");
}
