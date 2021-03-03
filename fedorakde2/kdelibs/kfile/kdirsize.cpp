/* This file is part of the KDE libraries
    Copyright (C) 2000 David Faure <faure@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include "kdirsize.h"
#include <kdebug.h>
#include <qapplication.h>
#include <qtimer.h>
#include <config-kfile.h>

using namespace KIO;

KDirSize::KDirSize( const KURL & directory )
    : KIO::Job(false /*No GUI*/), m_bAsync(true), m_totalSize(0L)
{
    startNextJob( directory );
}

KDirSize::KDirSize( const KFileItemList & lstItems )
    : KIO::Job(false /*No GUI*/), m_bAsync(true), m_totalSize(0L), m_lstItems(lstItems)
{
    QTimer::singleShot( 0, this, SLOT(processList()) );
}

void KDirSize::processList()
{
    while (!m_lstItems.isEmpty())
    {
        KFileItem * item = m_lstItems.first();
        m_lstItems.removeFirst();
        if ( item->isDir() && !item->isLink() )
        {
            kdDebug(kfile_area) << "KDirSize::processList dir -> listing" << endl;
            KURL url = item->url();
            startNextJob( url );
            return; // we'll come back later, when this one's finished
        }
        else
        {
            m_totalSize += (unsigned long)item->size();
// no long long with kdDebug()
//            kdDebug(kfile_area) << "KDirSize::processList file -> " << m_totalSize << endl;
        }
    }
    kdDebug(kfile_area) << "KDirSize::processList finished" << endl;
    if ( !m_bAsync )
        qApp->exit_loop();
    emitResult();
}

void KDirSize::startNextJob( const KURL & url )
{
    KIO::ListJob * listJob = KIO::listRecursive( url, false /* no GUI */ );
    connect( listJob, SIGNAL(entries( KIO::Job *,
                                      const KIO::UDSEntryList& )),
             SLOT( slotEntries( KIO::Job*,
                                const KIO::UDSEntryList& )));
    addSubjob( listJob );
}

void KDirSize::slotEntries( KIO::Job*, const KIO::UDSEntryList & list )
{
    KIO::UDSEntryListConstIterator it = list.begin();
    KIO::UDSEntryListConstIterator end = list.end();
    for (; it != end; ++it) {
        KIO::UDSEntry::ConstIterator it2 = (*it).begin();
        long long size = 0L;
        bool isLink = false;
        QString name;
        for( ; it2 != (*it).end(); it2++ ) {
          switch( (*it2).m_uds ) {
            case KIO::UDS_NAME:
              name = (*it2).m_str;
              break;
            case KIO::UDS_LINK_DEST:
              isLink = !(*it2).m_str.isEmpty();
              break;
            case KIO::UDS_SIZE:
              size = ((*it2).m_long);
              break;
            default:
              break;
          }
        }
        if ( !isLink && name != QString::fromLatin1("..") )
        {
            m_totalSize += size;
            //kdDebug(kfile_area) << name << ":" << size << endl;
        }
    }
}

//static
KDirSize * KDirSize::dirSizeJob( const KURL & directory )
{
    return new KDirSize( directory ); // useless - but consistent with other jobs
}

//static
KDirSize * KDirSize::dirSizeJob( const KFileItemList & lstItems )
{
    return new KDirSize( lstItems );
}

//static
unsigned long KDirSize::dirSize( const KURL & directory )
{
    return dirSize64( directory );
}

//static
long long KDirSize::dirSize64( const KURL & directory )
{
    KDirSize * dirSize = dirSizeJob( directory );
    dirSize->setSync();
    qApp->enter_loop();
    return dirSize->totalSize64();
}


void KDirSize::slotResult( KIO::Job * job )
{
    kdDebug(kfile_area) << " KDirSize::slotResult( KIO::Job * job ) m_lstItems:" << m_lstItems.count() << endl;
    if ( !m_lstItems.isEmpty() )
    {
        subjobs.remove(job); // Remove job, but don't kill this job.
        processList();
    }
    else
    {
        if ( !m_bAsync )
            qApp->exit_loop();
        KIO::Job::slotResult( job );
    }
}

#include "kdirsize.moc"
