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

#include <qwhatsthis.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qstring.h>
#include <qslider.h>
#include <qspinbox.h>

#include <kconfig.h>
#include <kcombobox.h>
#include <kglobal.h>
#include <klocale.h>

#include "main.h"
#include "hidingtab_impl.h"
#include "hidingtab_impl.moc"


extern int kickerconfig_screen_number;


HidingTab::HidingTab( KickerConfig *parent, const char* name )
  : HidingTabBase (parent, name)
{
    kconf = parent;
    // connections
    connect(m_manualHideAnimation, SIGNAL(clicked()), SIGNAL(changed()));
    connect(m_manualHideSlider, SIGNAL(valueChanged(int)), SIGNAL(changed()));
    connect(m_autoHide, SIGNAL(clicked()), SIGNAL(changed()));
    connect(m_autoHideSwitch, SIGNAL(clicked()), SIGNAL(changed()));
    connect(m_delaySlider, SIGNAL(valueChanged(int)), SIGNAL(changed()));
    connect(m_delaySpinBox, SIGNAL(valueChanged(int)), SIGNAL(changed()));
    connect(m_autoHideAnimation, SIGNAL(clicked()), SIGNAL(changed()));
    connect(m_autoHideSlider, SIGNAL(valueChanged(int)), SIGNAL(changed()));
    connect(m_lHB, SIGNAL(clicked()), SLOT(hideButtonsClicked()));
    connect(m_rHB, SIGNAL(clicked()), SLOT(hideButtonsClicked()));
    connect(m_hideButtonSlider, SIGNAL(valueChanged(int)), SIGNAL(changed()));

    // whats this help
    QWhatsThis::add(m_manualHideAnimation, i18n("If hide buttons are enabled, check this option to make the "
                                                "panel softly slide away when you click on the hide buttons. "
                                                "Otherwise it will just disappear."));

    QWhatsThis::add(m_manualHideSlider, i18n("Determines the speed of the hide animation, i.e. the "
                                             "animation shown when you click on the panel's hide buttons."));

    QWhatsThis::add(m_autoHide, i18n("If this option is enabled, the panel will automatically hide "
                                     "after some time and reappear when you move the mouse to the "
                                     "screen edge the panel is attached to. "
                                     "This is particularly useful for small screen resolutions, "
                                     "for example, on laptops.") );

    QWhatsThis::add(m_autoHideSwitch, i18n("If this option is enabled, the panel will automatically show "
					   "itself for a brief period of time when the desktop is switched "
					   "so you can see which desktop you are on.") );

    QString delaystr = i18n("Here you can change the delay after which the panel will disappear"
                            " if not used.");

    QWhatsThis::add(m_delaySlider, delaystr);
    QWhatsThis::add(m_delaySpinBox, delaystr);

    QWhatsThis::add(m_autoHideAnimation, i18n("If auto-hide panel is enabled, check this option to make "
                                              "the panel softly slide down after a certain amount of time. "
                                              "Otherwise it will just disappear."));

    QWhatsThis::add(m_autoHideSlider, i18n("Determines the speed of the auto-hide animation, "
                                           "i.e. the animation shown when the panel disappears after "
                                           "a certain amount of time."));

    QWhatsThis::add(m_hideButtonSlider, i18n("Here you can change the size of the hide buttons."));

    load();
}

void HidingTab::load()
{
    QCString configname;
    if (kickerconfig_screen_number == 0)
	configname = "kickerrc";
    else
	configname.sprintf("kicker-screen-%drc", kickerconfig_screen_number);
    KConfig *c = new KConfig(configname, false, false);

    c->setGroup("General");

    bool hideanim = c->readBoolEntry("HideAnimation", true);
    bool autohideanim = c->readBoolEntry("AutoHideAnimation", true);

    m_manualHideSlider->setValue(c->readNumEntry("HideAnimationSpeed", 40));
    m_autoHideSlider->setValue(c->readNumEntry("AutoHideAnimationSpeed", 40));

    m_manualHideSlider->setEnabled(hideanim);
    m_autoHideSlider->setEnabled(autohideanim);

    m_manualHideAnimation->setChecked(hideanim);
    m_autoHideAnimation->setChecked(autohideanim);

    bool showLHB = c->readBoolEntry("ShowLeftHideButton", false);
    bool showRHB = c->readBoolEntry("ShowRightHideButton", true);

    m_lHB->setChecked( showLHB );
    m_rHB->setChecked( showRHB );

    m_hideButtonSlider->setValue(c->readNumEntry("HideButtonSize", 14));
    m_hideButtonSlider->setEnabled(showLHB || showRHB);

    bool ah = c->readBoolEntry("AutoHidePanel", false);
    bool ahs = c->readBoolEntry("AutoHideSwitch", false);
    int delay = c->readNumEntry("AutoHideDelay", 3);

    m_autoHide->setChecked(ah);
    m_autoHideSwitch->setChecked(ahs);
    m_delaySlider->setValue(delay);
    m_delaySpinBox->setValue(delay);
    m_delaySlider->setEnabled(ah);
    m_delaySpinBox->setEnabled(ah);

    delete c;
}

void HidingTab::save()
{
    QCString configname;
    if (kickerconfig_screen_number == 0)
	configname = "kickerrc";
    else
	configname.sprintf("kicker-screen-%drc", kickerconfig_screen_number);
    KConfig *c = new KConfig(configname, false, false);

    c->setGroup("General");

    c->writeEntry("HideAnimation", m_manualHideAnimation->isChecked());
    c->writeEntry("AutoHidePanel", m_autoHide->isChecked());
    c->writeEntry("AutoHideSwitch", m_autoHideSwitch->isChecked());
    c->writeEntry("AutoHideDelay", m_delaySlider->value());
    c->writeEntry("AutoHideAnimation", m_autoHideAnimation->isChecked());
    c->writeEntry("HideAnimationSpeed", m_manualHideSlider->value());
    c->writeEntry("AutoHideAnimationSpeed", m_autoHideSlider->value());
    c->writeEntry("ShowLeftHideButton", m_lHB->isChecked());
    c->writeEntry("ShowRightHideButton", m_rHB->isChecked());

    c->writeEntry("HideButtonSize", m_hideButtonSlider->value());
    c->sync();

    delete c;
}

void HidingTab::defaults()
{
    m_manualHideAnimation->setChecked(true);
    m_autoHideAnimation->setChecked(true);

    m_manualHideSlider->setEnabled(true);
    m_autoHideSlider->setEnabled(true);

    m_manualHideSlider->setValue(100);
    m_autoHideSlider->setValue(25);

    m_autoHide->setChecked(false);
    m_delaySlider->setValue(3);
    m_delaySpinBox->setValue(3);
    m_delaySlider->setEnabled(false);
    m_delaySpinBox->setEnabled(false);

    m_lHB->setChecked( false );
    m_rHB->setChecked( true );
    m_hideButtonSlider->setValue(10);
}

void HidingTab::hideButtonsClicked()
{
    m_hideButtonSlider->setEnabled( m_lHB->isChecked() || m_rHB->isChecked() );
    emit changed();
}
