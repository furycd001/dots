/* This file is part of the KDE project
   Copyright (C) 2000 Carsten Pfeiffer <pfeiffer@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KONQ_HISTORYCOMM_H
#define KONQ_HISTORYCOMM_H

#include <qdatetime.h>

#include <dcopobject.h>
#include <kurl.h>

class KonqHistoryEntry
{
public:
    KonqHistoryEntry()
	: numberOfTimesVisited(1) {}

    KURL url;
    QString typedURL;
    QString title;
    Q_UINT32 numberOfTimesVisited;
    QDateTime firstVisited;
    QDateTime lastVisited;
};


// QDataStream operators (read and write a KonqHistoryEntry
// from/into a QDataStream
QDataStream& operator<< (QDataStream& s, const KonqHistoryEntry& e);
QDataStream& operator>> (QDataStream& s, KonqHistoryEntry& e);

///////////////////////////////////////////////////////////////////


/**
 * DCOP Methods for KonqHistoryManager. Has to be in a separate file, because
 * dcopidl2cpp barfs on every second construct ;(
 * Implementations of the pure virtual methods are in KonqHistoryManager
 */
class KonqHistoryComm : public DCOPObject
{
    K_DCOP

protected:
    KonqHistoryComm( QCString objId ) : DCOPObject( objId ) {}

k_dcop:
    virtual ASYNC notifyHistoryEntry( KonqHistoryEntry e, QCString saveId) = 0;
    virtual ASYNC notifyMaxCount( Q_UINT32 count, QCString saveId ) = 0;
    virtual ASYNC notifyMaxAge( Q_UINT32 days, QCString saveId ) = 0;
    virtual ASYNC notifyClear( QCString saveId ) = 0;
    virtual ASYNC notifyRemove( KURL url, QCString saveId ) = 0;
    virtual ASYNC notifyRemove( KURL::List url, QCString saveId ) = 0;

};

#endif // KONQ_HISTORYCOMM_H
