/*
 *  Copyright (c) 2001 John Firebaugh <jfirebaugh@kde.org>
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


#ifndef __extensionstab_impl_h__
#define __extensionstab_impl_h__

#include <qlistview.h>

#include "extensionstab.h"

class ExtensionInfo;

class ExtensionsTab : public ExtensionsTabBase
{
    Q_OBJECT

public:
    ExtensionsTab( QWidget *parent=0, const char* name=0 );

    void load();
    void save();
    void defaults();

signals:
    void changed();

private slots:
    void loadConfig( QListViewItem* item );
    void slotChanged();
};

class ExtensionInfo : public QListViewItem
{
public:
    ExtensionInfo( const QString& destopFile, const QString& configFile,
                   QListView* parent );

    void setDefaults();
    void save();

    QString _configFile;

    // Configuration settings
    int      _position;
    int      _HBwidth;
    bool     _showLeftHB;
    bool     _showRightHB;
    bool     _autoHide;
    bool     _autoHideSwitch;
    int      _autoHideDelay;
    bool     _hideAnim;
    bool     _autoHideAnim;
    int      _hideAnimSpeed;
    int      _autoHideAnimSpeed;
    bool     _showToolTips;
    int      _sizePercentage;
    bool     _expandSize;
};

#endif

