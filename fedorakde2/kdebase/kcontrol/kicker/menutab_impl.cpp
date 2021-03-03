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

#include <qcheckbox.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qwhatsthis.h>
#include <qlineedit.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qvalidator.h>

#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>

#include "menutab_impl.h"
#include "menutab_impl.moc"


extern int kickerconfig_screen_number;


MenuTab::MenuTab( QWidget *parent, const char* name )
  : MenuTabBase (parent, name)
{
    // connections
    connect(m_clearCache, SIGNAL(clicked()), SIGNAL(changed()));
    connect(m_clearSlider, SIGNAL(valueChanged(int)), SIGNAL(changed()));
    connect(m_clearSpinBox, SIGNAL(valueChanged(int)), SIGNAL(changed()));
    connect(m_hiddenFiles, SIGNAL(clicked()), SIGNAL(changed()));
    connect(m_maxSlider, SIGNAL(valueChanged(int)), SIGNAL(changed()));
    connect(m_maxSpinBox, SIGNAL(valueChanged(int)), SIGNAL(changed()));
    connect(m_mergeLocations, SIGNAL(clicked()), SIGNAL(changed()));
    connect(m_showBookmarks, SIGNAL(clicked()), SIGNAL(changed()));
    connect(m_showRecent, SIGNAL(clicked()), SIGNAL(changed()));
    connect(m_showQuickBrowser, SIGNAL(clicked()), SIGNAL(changed()));
    connect(m_num2ShowSpinBox, SIGNAL(valueChanged(int)), SIGNAL(changed()));

    m_pRecentOrderGroup->setRadioButtonExclusive(true);
    connect(m_pRecentOrderGroup, SIGNAL(clicked(int)), SIGNAL(changed()));


    // whats this help
    QWhatsThis::add(m_clearCache, i18n("The panel can cache information about menu entries instead "
                                       "of reading it from disk every time you browse the menus. "
                                       "This makes the panel menus react faster. "
                                       "However, you might want to turn this off if you're short on memory."));

    QString clearstr = i18n("If menu caching is turned on, you can set a delay after which "
                            "the cache will be cleared.");

    QWhatsThis::add(m_clearSlider, clearstr);
    QWhatsThis::add(m_clearSpinBox, clearstr);

    QWhatsThis::add(m_hiddenFiles, i18n("If this option is enabled, hidden files (i.e. files beginning "
                                        "with a dot) will be shown in the QuickBrowser menus."));

    QString maxstr = i18n("When browsing directories that contain a lot of files, the QuickBrowser "
                          "can sometimes hide your whole desktop. Here you can limit the number of "
                          "entries shown at a time in the QuickBrowser. "
                          "This is particularly useful for low screen resolutions.");

    QWhatsThis::add(m_maxSlider, maxstr);
    QWhatsThis::add(m_maxSpinBox, maxstr);

    QWhatsThis::add(m_mergeLocations, i18n("KDE can support several different locations "
                                           "on the system for storing program "
                                           "information, including (but not limited to) "
                                           "a system-wide and a personal directory. "
                                           "Enabling this option makes the KDE panel "
                                           "merge these different locations into a "
                                           "single logical tree of programs."));

    QWhatsThis::add(m_showBookmarks, i18n("Enabling this option will make the panel show "
                                          "a bookmarks menu in your KDE menu"));

    QWhatsThis::add(m_showRecent, i18n("Enabling this option will make the panel show "
                                       "a recent documents menu in your KDE menu, containing shortcuts to "
                                       "your most recently edited documents. This assumes you've been "
                                       "using KDE applications to edit those documents, as other "
                                       "applications will not be able to take advantage of this feature."));

    QWhatsThis::add(m_showQuickBrowser, i18n("Enabling this option will show the 'Quick Browser' in your "
                                             "KDE menu, a fast way of accessing your files via submenus. "
                                             "You can also add a Quick Browser "
                                             "as a panel button, using the panel context menu."));

    load();
}

void MenuTab::load()
{
    QCString configname;
    if (kickerconfig_screen_number == 0)
        configname = "kickerrc";
    else
        configname.sprintf("kicker-screen-%drc", kickerconfig_screen_number);
    KConfig *c = new KConfig(configname, false, false);

    c->setGroup("menus");

    bool cc = c->readBoolEntry("ClearMenuCache", true);
    m_clearCache->setChecked(cc);
    m_clearSlider->setValue(c->readNumEntry("MenuCacheTime", 60000) / 1000);
    m_clearSlider->setEnabled(cc);
    m_clearSpinBox->setValue(c->readNumEntry("MenuCacheTime", 60000) / 1000);
    m_clearSpinBox->setEnabled(cc);

    m_maxSlider->setValue(c->readNumEntry("MaxEntries2", 30));
    m_maxSpinBox->setValue(c->readNumEntry("MaxEntries2", 30));

    m_mergeLocations->setChecked(c->readBoolEntry("MergeKDEDirs", true));
    m_showBookmarks->setChecked(c->readBoolEntry("UseBookmarks", true));
    m_showRecent->setChecked(c->readBoolEntry("UseRecent", true));
    m_showQuickBrowser->setChecked(c->readBoolEntry("UseBrowser", true));

    m_hiddenFiles->setChecked(c->readBoolEntry("ShowHiddenFiles", false));

    m_num2ShowSpinBox->setValue(c->readNumEntry("NumVisibleEntries", 5));

    bool bRecentVsOften = c->readBoolEntry("RecentVsOften", false);
    if (bRecentVsOften)
        m_pRecent->setChecked(true);
    else
        m_pOften->setChecked(true);
    delete c;
}

void MenuTab::save()
{
    QCString configname;
    if (kickerconfig_screen_number == 0)
        configname = "kickerrc";
    else
        configname.sprintf("kicker-screen-%drc", kickerconfig_screen_number);
    KConfig *c = new KConfig(configname, false, false);

    c->setGroup("menus");

    c->writeEntry("ClearMenuCache", m_clearCache->isChecked());
    c->writeEntry("MenuCacheTime", m_clearSlider->value() * 1000);
    c->writeEntry("MaxEntries2", m_maxSlider->value());
    c->writeEntry("MergeKDEDirs", m_mergeLocations->isChecked());
    c->writeEntry("UseBookmarks", m_showBookmarks->isChecked());
    c->writeEntry("UseRecent", m_showRecent->isChecked());
    c->writeEntry("UseBrowser", m_showQuickBrowser->isChecked());
    c->writeEntry("ShowHiddenFiles", m_hiddenFiles->isChecked());
    c->writeEntry("NumVisibleEntries", m_num2ShowSpinBox->value());

    bool bRecentVsOften = m_pRecent->isChecked();
    c->writeEntry("RecentVsOften", bRecentVsOften);

    c->sync();

    delete c;
}

void MenuTab::defaults()
{
  m_clearCache->setChecked(true);
  m_clearSlider->setValue(60);
  m_clearSlider->setEnabled(true);
  m_clearSpinBox->setValue(60);
  m_clearSpinBox->setEnabled(true);
  m_maxSlider->setValue(30);
  m_maxSpinBox->setValue(30);
  m_mergeLocations->setChecked(true);
  m_showRecent->setChecked(true);
  m_showQuickBrowser->setChecked(true);
  m_hiddenFiles->setChecked(false);
  m_showBookmarks->setChecked(true);

  m_pOften->setChecked(true);
  m_num2ShowSpinBox->setValue(5);
}
