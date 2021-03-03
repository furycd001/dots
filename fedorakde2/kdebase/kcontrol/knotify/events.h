/*
    $Id: events.h,v 1.5 2000/12/14 07:07:03 waba Exp $

    Copyright (C) 2000 Carsten Pfeiffer <pfeiffer@kde.org>

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

#ifndef EVENTS_H
#define EVENTS_H

#include <qlist.h>
#include <qstring.h>

class KConfig;
class KNEvent;
class KNApplication;

typedef QList<KNEvent> EventList;
typedef QList<KNApplication> ApplicationList;
typedef QListIterator<KNEvent> KNEventListIterator;
typedef QListIterator<KNApplication> KNApplicationListIterator;

class Events
{
public:
    Events();

    void load();
    void save();

    ApplicationList& apps() { return m_apps; }

private:
    QString makeRelative( const QString& );

    ApplicationList m_apps;

};

class KNApplication
{
public:
    KNApplication( const QString &path );
    ~KNApplication();

    QString text() const { return m_description; }
    QString icon() const { return m_icon; }
    EventList * eventList();
    void save();

private:
    void loadEvents();

    QString m_icon;
    QString m_description;
    EventList *m_events;

    KConfig *kc; // The file that defines the events.
    KConfig *config; // The file that contains the settings for the events.
};


class KNEvent
{
    friend class KNApplication;

public:
    QString text() const { return description; }

    int presentation;
    int dontShow;
    QString logfile;
    QString soundfile;

private:
    KNEvent() {
	presentation = 0;
	dontShow = 0;
    }
    QString name;
    QString description;
    QString configGroup;
};

#endif // EVENTS_H
