/*  This file is part of the KDE libraries
    Copyright (c) 2000 Matthias Hoelzer-Kluepfel <mhk@caldera.de>


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
#ifndef __kio_man_h__
#define __kio_man_h__


#include <qstring.h>
#include <qcstring.h>
#include <qstringlist.h>
#include <qdict.h>


#include <kio/global.h>
#include <kio/slavebase.h>


class MANProtocol : public QObject, public KIO::SlaveBase
{
    Q_OBJECT

public:

    MANProtocol(const QCString &pool_socket, const QCString &app_socket);
    virtual ~MANProtocol();

    virtual void get(const KURL& url);
    virtual void stat(const KURL& url);

    virtual void mimetype(const KURL &url);

    void outputError(const QString& errmsg);
    void outputMatchingPages(const QStringList &matchingPages);

    void showMainIndex();
    void showIndex(const QString& section);

    // the following two functions are the interface to man2html
    void output(const char *insert);
    char *readManPage(const char *filename);

    static MANProtocol *self();

private:
    void checkManPaths();
    QStringList findPages(const QString& section, const QString &title);

    void addToBuffer(const char *buffer, int buflen);
    QString pageName(const QString& page) const;
    QCString output_string;

    static MANProtocol *_self;
    QCString lastdir;
    QString common_dir;

};


#endif
