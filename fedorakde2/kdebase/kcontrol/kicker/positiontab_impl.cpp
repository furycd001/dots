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

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qwhatsthis.h>
#include <qslider.h>
#include <qspinbox.h>

#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <klineedit.h>

#include "positiontab_impl.h"
#include "positiontab_impl.moc"


extern int kickerconfig_screen_number;


PositionTab::PositionTab( QWidget *parent, const char* name )
  : PositionTabBase (parent, name)
{
    // connections
    connect(m_locationGroup, SIGNAL(clicked(int)), SIGNAL(changed()));
    connect(m_sizeGroup, SIGNAL(clicked(int)), SIGNAL(changed()));
    connect(m_percentSlider, SIGNAL(valueChanged(int)), SIGNAL(changed()));
    connect(m_percentSpinBox, SIGNAL(valueChanged(int)), SIGNAL(changed()));
    connect(m_expandCheckBox, SIGNAL(clicked()), SIGNAL(changed()));

    // whats this help
    QWhatsThis::add(m_locationGroup, i18n("This sets the position of the panel"
                                          " i.e. the screen border it is attached to. You can also change this"
                                          " position by left-clicking on some free space on the panel and"
                                          " dragging it to a screen border."));

    QWhatsThis::add(m_sizeGroup, i18n("This sets the size of the panel."
                                      " You can also access this option via the panel context menu, i.e."
                                      " by right-clicking on some free space on the panel."));

    load();
}

void PositionTab::load()
{
    QCString configname;
    if (kickerconfig_screen_number == 0)
	configname = "kickerrc";
    else
	configname.sprintf("kicker-screen-%drc", kickerconfig_screen_number);
    KConfig *c = new KConfig(configname, false, false);

    c->setGroup("General");

    m_sizeGroup->setButton(c->readNumEntry("Size", 2));
    m_locationGroup->setButton(c->readNumEntry("Position", 3));

    int sizepercentage = c->readNumEntry( "SizePercentage", 100 );
    m_percentSlider->setValue( sizepercentage );
    m_percentSpinBox->setValue( sizepercentage );

    m_expandCheckBox->setChecked( c->readBoolEntry( "ExpandSize", true ) );

    delete c;
}

void PositionTab::save()
{
    QCString configname;
    if (kickerconfig_screen_number == 0)
	configname = "kickerrc";
    else
	configname.sprintf("kicker-screen-%drc", kickerconfig_screen_number);
    KConfig *c = new KConfig(configname, false, false);

    c->setGroup("General");

    c->writeEntry("Size", m_sizeGroup->id(m_sizeGroup->selected()));
    c->writeEntry("Position", m_locationGroup->id(m_locationGroup->selected()));
    c->writeEntry( "SizePercentage", m_percentSlider->value() );
    c->writeEntry( "ExpandSize", m_expandCheckBox->isChecked() );
    c->sync();

    delete c;
}

void PositionTab::defaults()
{
    m_sizeGroup->setButton(2);
    m_locationGroup->setButton(3);
    m_expandCheckBox->setChecked( true );
    m_percentSlider->setValue( 100 );
    m_percentSpinBox->setValue( 100 );
}
