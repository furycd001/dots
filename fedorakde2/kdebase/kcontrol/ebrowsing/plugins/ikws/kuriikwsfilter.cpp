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
#include <kurl.h>
#include <kinstance.h>
#include <kglobal.h>

#include "ikwsopts.h"
#include "kuriikwsfiltereng.h"
#include "kuriikwsfilter.h"

#define searcher KURISearchFilterEngine::self()

KInstance *KURIIKWSFilterFactory::s_instance = 0L;

KURIIKWSFilter::KURIIKWSFilter(QObject *parent, const char *name)
                 :KURIFilterPlugin(parent, name ? name : "kuriikwsfilter", 1.0),
                  DCOPObject("KURIIKWSFilterIface")
{
  KURISearchFilterEngine::incRef();
}

KURIIKWSFilter::~KURIIKWSFilter()
{
  KURISearchFilterEngine::decRef();
}

void KURIIKWSFilter::configure()
{
    kdDebug() << "(" << getpid() << ") " << "Internet Keywords: Sending a config reload request..." << endl;
    searcher->loadConfig();
}

bool KURIIKWSFilter::filterURI( KURIFilterData &data ) const
{
    if (searcher->verbose())
        kdDebug() << "KURIIKWSFilter: filtering " <<  data.uri().url() << endl;

    KURL u = data.uri();
    if ( u.pass().isEmpty() )
    {
      QString result = searcher->ikwsQuery( u );
      if( !result.isEmpty() )
      {
        setFilteredURI( data, result );
        setURIType( data, KURIFilterData::NET_PROTOCOL );
        return true;
      }
    }
    return false;
}

KURIIKWSFilterFactory::KURIIKWSFilterFactory(QObject *parent, const char *name)
                      :KLibFactory(parent, name)
{
    KURISearchFilterEngine::incRef();
    s_instance = new KInstance(searcher->name());
}

KURIIKWSFilterFactory::~KURIIKWSFilterFactory() {
    KURISearchFilterEngine::decRef();
    delete s_instance;
}

QObject *KURIIKWSFilterFactory::createObject( QObject* parent, const char* name,
                                              const char*, const QStringList& )
{
    return new KURIIKWSFilter( parent, name );
}

KInstance *KURIIKWSFilterFactory::instance() {
    return s_instance;
}

extern "C" {

void *init_libkuriikwsfilter() {
    return new KURIIKWSFilterFactory;
}

}

#include "kuriikwsfilter.moc"


