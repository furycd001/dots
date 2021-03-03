/*
 * bell.h
 *
 * Copyright (c) 1997 Patrick Dowler dowler@morgul.fsh.uvic.ca
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


#ifndef __KKDBCONFIG_H__
#define __KKDBCONFIG_H__

#include <kcmodule.h>

class QTabWidget;
class PluginConfig;
class ConnectionConfig;

class KDBModule : public KCModule
{
    Q_OBJECT

public:
    KDBModule( QWidget* parent = 0L, const char* name = 0L );
    ~KDBModule();

    void load();
    void save();
    void defaults();

private slots:
    void slotChanged();

private:
    QTabWidget       *m_tab;
    PluginConfig     *m_plugins;
    ConnectionConfig *m_connections;
};

#endif
