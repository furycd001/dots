/*  This file is part of the KDE project
    Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
    Copyright (C) 2000 Yves Arrouye <yves@realnames.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <unistd.h>

#include <kdebug.h>
#include <klocale.h>
#include <kinstance.h>
#include <kglobal.h>

#include "ikwsopts.h"
#include "kuriikwsfiltereng.h"
#include "kurisearchfilter.h"

#define searcher KURISearchFilterEngine::self()

KInstance *KURISearchFilterFactory::s_instance = 0L;

KURISearchFilter::KURISearchFilter(QObject *parent, const char *name)
                 :KURIFilterPlugin(parent, name ? name : "kurisearchfilter", 1.0),
                  DCOPObject("KURISearchFilterIface")
{
  KURISearchFilterEngine::incRef();
}

KURISearchFilter::~KURISearchFilter()
{
  KURISearchFilterEngine::decRef();
}

void KURISearchFilter::configure()
{
    kdDebug() << "(" << getpid() << ") " << "Search Engine Keywords: Sending a config reload request..." << endl;
    searcher->loadConfig();
}

bool KURISearchFilter::filterURI( KURIFilterData &data ) const
{
    if ( searcher->verbose() )
        kdDebug() << "KURISearchFilter: filtering " << data.uri().url() << endl;

    QString result = searcher->searchQuery( data.uri() );
    if ( !result.isEmpty() )
    {
        setFilteredURI( data, result );
        setURIType( data, KURIFilterData::NET_PROTOCOL );
        return true;
    }
    return false;
}

KCModule *KURISearchFilter::configModule(QWidget *parent, const char *name) const
{
    return new InternetKeywordsOptions(parent, name);
}

QString KURISearchFilter::configName() const
{
    return i18n("&Keywords");
}


KURISearchFilterFactory::KURISearchFilterFactory(QObject *parent, const char *name) : KLibFactory(parent, name)
{
    KURISearchFilterEngine::incRef();
    s_instance = new KInstance(searcher->name());
}

KURISearchFilterFactory::~KURISearchFilterFactory()
{
    KURISearchFilterEngine::decRef();
    delete s_instance;
}

QObject *KURISearchFilterFactory::create( QObject *parent, const char *name, const char*, const QStringList & )
{
    QObject *obj = new KURISearchFilter( parent, name );
    emit objectCreated( obj );
    return obj;
}

KInstance *KURISearchFilterFactory::instance()
{
    return s_instance;
}

extern "C"
{
  void *init_libkurisearchfilter()
  {
    return new KURISearchFilterFactory;
  }
}

#include "kurisearchfilter.moc"
