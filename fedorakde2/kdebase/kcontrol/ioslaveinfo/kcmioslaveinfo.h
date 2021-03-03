/*
 * kcmioslaveinfo.h
 *
 * Copyright 2001 Alexander Neundorf <alexander.neundorf@rz.tu-ilmenau.de>
 * Copyright 2001 George Staikos  <staikos@kde.org>
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
#ifndef kcmioslaveinfo_h_included
#define kcmioslaveinfo_h_included

#include <kcmodule.h>

#include <qstring.h>
#include <ktextbrowser.h>
#include <qlistbox.h>
#include <klistbox.h>
#include <kio/job.h>

class KIOTimeoutControl;
class QTabWidget;
class QSpinBox;
class KConfig;

class KCMIOSlaveInfo : public KCModule
{
    Q_OBJECT
public:
    KCMIOSlaveInfo(QWidget *parent = 0L, const char *name = 0L);
    virtual ~KCMIOSlaveInfo();

    void load();
    void save();
    void defaults();
    int buttons();
    QString quickHelp() const;

protected:
    KListBox *m_ioslavesLb;
    KTextBrowser *m_info;
    QCString helpData;

protected slots:

    void showInfo(const QString& protocol);
    void showInfo(QListBoxItem *item);
    void slaveHelp( KIO::Job *, const QByteArray &data);

public slots:
    void configChanged();
    void childChanged(bool really);

};
#endif
