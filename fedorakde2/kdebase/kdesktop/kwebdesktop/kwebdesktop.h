/* This file is part of the KDE project
   Copyright (C) 2000 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KWEBDESKTOP_H
#define KWEBDESKTOP_H

#include <qobject.h>
#include <qcstring.h>
#include <kparts/browserextension.h>
#include <khtml_part.h>

namespace KIO { class Job; }

class KWebDesktop : public QObject
{
    Q_OBJECT
public:
    KWebDesktop( KHTMLPart *part, const QCString & imageFile ) : QObject( part )
    {
        m_part = part;
        m_imageFile = imageFile;
    }

private slots:
    void slotCompleted();

private:
    KHTMLPart *m_part;
    QCString m_imageFile;
};


class KWebDesktopRun : public QObject
{
    Q_OBJECT
public:
    KWebDesktopRun( KHTMLPart * part, const KURL & url );
    ~KWebDesktopRun() {}

protected slots:
    void slotMimetype( KIO::Job *job, const QString &_type );
    void slotFinished( KIO::Job * job );

private:
    KHTMLPart * m_part;
    KURL m_url;
};

#endif
