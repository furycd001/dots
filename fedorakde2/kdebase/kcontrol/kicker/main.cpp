/*
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

#include <qtabwidget.h>
#include <qlayout.h>
#include <qradiobutton.h>

#include <kapp.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <klocale.h>
#include <kimageio.h>

#include <dcopclient.h>

#include "main.h"
#include "main.moc"
#include "positiontab_impl.h"
#include "hidingtab_impl.h"
#include "menutab_impl.h"
#include "lookandfeeltab_impl.h"
#include "applettab.h"
#include "extensionstab_impl.h"

#include <X11/Xlib.h>


// for multihead
int kickerconfig_screen_number = 0;


KickerConfig::KickerConfig(QWidget *parent, const char *name)
  : KCModule(parent, name)
{
    if (qt_xdisplay())
	kickerconfig_screen_number = DefaultScreen(qt_xdisplay());

    QVBoxLayout *layout = new QVBoxLayout(this);
    tab = new QTabWidget(this);
    layout->addWidget(tab);

    positiontab = new PositionTab(this);
    tab->addTab(positiontab, i18n("&Position"));
    connect(positiontab, SIGNAL(changed()), this, SLOT(configChanged()));

    hidingtab = new HidingTab(this);
    tab->addTab(hidingtab, i18n("&Hiding"));
    connect(hidingtab, SIGNAL(changed()), this, SLOT(configChanged()));

    lookandfeeltab = new LookAndFeelTab(this);
    tab->addTab(lookandfeeltab, i18n("&Look && Feel"));
    connect(lookandfeeltab, SIGNAL(changed()), this, SLOT(configChanged()));

    menutab = new MenuTab(this);
    tab->addTab(menutab, i18n("&Menus"));
    connect(menutab, SIGNAL(changed()), this, SLOT(configChanged()));

    applettab = new AppletTab(this);
    tab->addTab(applettab, i18n("&Applets"));
    connect(applettab, SIGNAL(changed()), this, SLOT(configChanged()));

    extensionstab = new ExtensionsTab(this);
    tab->addTab(extensionstab, i18n("&Extensions"));
    connect(extensionstab, SIGNAL(changed()), this, SLOT(configChanged()));

    load();
}

void KickerConfig::configChanged()
{
    emit changed(true);
}

void KickerConfig::load()
{
    positiontab->load();
    hidingtab->load();
    menutab->load();
    lookandfeeltab->load();
    applettab->load();
    extensionstab->load();
    emit changed(false);
}

void KickerConfig::save()
{
    positiontab->save();
    hidingtab->save();
    menutab->save();
    lookandfeeltab->save();
    applettab->save();
    extensionstab->save();

    emit changed(false);

    // Tell kicker about the new config file.
    if (!kapp->dcopClient()->isAttached())
        kapp->dcopClient()->attach();
    QByteArray data;

    QCString appname;
    if (kickerconfig_screen_number == 0)
	appname = "kicker";
    else
	appname.sprintf("kicker-screen-%d", kickerconfig_screen_number);
    kapp->dcopClient()->send( appname, "Panel", "configure()", data );
}

void KickerConfig::defaults()
{
    positiontab->defaults();
    hidingtab->defaults();
    menutab->defaults();
    lookandfeeltab->defaults();
    applettab->defaults();
    extensionstab->defaults();

    emit changed(true);
}

QString KickerConfig::quickHelp() const
{
    return i18n("<h1>Panel</h1> Here you can configure the KDE panel (also"
                " referred to as 'kicker'). This includes options like the position and"
                " size of the panel, as well as its hiding behavior and its looks.<p>"
                " Note that you can also access some of these options directly by clicking"
                " on the panel, e.g. dragging it with the left mouse button or using the"
                " context menu on right mouse button click. This context menu also offers you"
                " manipulation of the panel's buttons and applets.");
}

bool KickerConfig::horizontal()
{
    return (positiontab->m_topButton->isChecked() ||
            positiontab->m_bottomButton->isChecked());
}

extern "C"
{
    KCModule *create_kicker(QWidget *parent, const char *name)
    {
        KImageIO::registerFormats();
        KGlobal::locale()->insertCatalogue("kcmkicker");
        KGlobal::dirs()->addResourceType("tiles", KStandardDirs::kde_default("data") +
                                         "kicker/tiles");
        KGlobal::dirs()->addResourceType("hb_pics", KStandardDirs::kde_default("data") +
                                         "kcmkicker/pics");
        KGlobal::dirs()->addResourceType("applets", KStandardDirs::kde_default("data") +
                                         "kicker/applets");
        KGlobal::dirs()->addResourceType("extensions", KStandardDirs::kde_default("data") +
                                         "kicker/extensions");
        return new KickerConfig(parent, name);
    };
}
