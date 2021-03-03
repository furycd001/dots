/*
    $Id: events.cpp,v 1.10 2001/01/14 21:57:41 pfeiffer Exp $

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

#include <qstringlist.h>

#include <kconfig.h>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstddirs.h>

#include "events.h"

// simple access to all knotify-handled applications
Events::Events()
{
    m_apps.setAutoDelete( true );
}


void Events::load()
{
    m_apps.clear();
    QStringList fullpaths(KGlobal::dirs()->findAllResources("data", "*/eventsrc", false, true));
    QString path;
    for (QStringList::Iterator it=fullpaths.begin(); it!=fullpaths.end(); ++it) {
        path = makeRelative( *it );
	if ( !path.isEmpty() ) {
	    m_apps.append( new KNApplication( path ));
	}
    }
}

void Events::save()
{
    kdDebug() << "save\n";

    KNApplicationListIterator it( m_apps );
    while ( it.current() ) {
	(*it)->save();
	++it;
    }
}

// returns e.g. "kwin/eventsrc" from a given path
// "/opt/kde2/share/apps/kwin/eventsrc"
QString Events::makeRelative( const QString& fullPath )
{
  int slash = fullPath.findRev( '/' ) - 1;
  slash = fullPath.findRev( '/', slash );

  if ( slash < 0 )
    return QString::null;

  return fullPath.mid( slash+1 );
}

//////////////////////////////////////////////////////////////////////



KNApplication::KNApplication( const QString &path )
{
    QString config_file = path;
    config_file[config_file.find('/')] = '.';
    m_events = 0L;
    config = new KConfig(config_file, false, false);
    kc = new KConfig(path, true, false, "data");
    kc->setGroup( QString::fromLatin1("!Global!") );
    m_icon = kc->readEntry(QString::fromLatin1("IconName"),
                           QString::fromLatin1("misc"));
    m_description = kc->readEntry( QString::fromLatin1("Comment"),
				   i18n("No description available") );
}

KNApplication::~KNApplication()
{
    delete config;
    delete kc;
    delete m_events;
}


EventList * KNApplication::eventList()
{
    if ( !m_events ) {
	m_events = new EventList;
	m_events->setAutoDelete( true );
	loadEvents();
    }

    return m_events;
}


void KNApplication::save()
{
    if ( !m_events )
	return;

    KNEventListIterator it( *m_events );
    KNEvent *e;
    while ( (e = it.current()) ) {
	config->setGroup( e->configGroup );
	config->writeEntry( "presentation", e->presentation );
	config->writeEntry( "soundfile", e->soundfile );
	config->writeEntry( "logfile", e->logfile );

	++it;
    }
    config->sync();
}


void KNApplication::loadEvents()
{
    KNEvent *e = 0L;

    QString global = QString::fromLatin1("!Global!");
    QString default_group = QString::fromLatin1("<default>");
    QString name = QString::fromLatin1("Name");
    QString comment = QString::fromLatin1("Comment");
    QString unknown = i18n("Unknown Title");
    QString nodesc = i18n("No Description");

    QStringList conflist = kc->groupList();
    QStringList::Iterator it = conflist.begin();

    while ( it != conflist.end() ) {
	if ( (*it) != global && (*it) != default_group ) { // event group
	    kc->setGroup( *it );

	    e = new KNEvent;
	    e->name = kc->readEntry( name, unknown );
	    e->description = kc->readEntry( comment, nodesc );
	    e->configGroup = *it;

	    if ( e->name.isEmpty() || e->description.isEmpty() )
		delete e;

	    else { // load the event
		int default_rep = kc->readNumEntry("default_presentation", 0 );
                QString default_logfile = kc->readEntry("default_logfile");
                QString default_soundfile = kc->readEntry("default_sound");
                config->setGroup(*it);
		e->presentation = config->readNumEntry("presentation", default_rep);
		e->dontShow = config->readNumEntry("nopresentation", 0 );
		e->logfile = config->readEntry("logfile", default_logfile);
		e->soundfile = config->readEntry("soundfile", default_soundfile);

		m_events->append( e );
	    }
	}

	++it;
    }

    return;
}
