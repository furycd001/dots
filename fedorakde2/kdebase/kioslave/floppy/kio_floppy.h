/*  This file is part of the KDE project
    Copyright (C) 2000 Alexander Neundorf <neundorf@kde.org>

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

#ifndef KIO_FLOPPY_H
#define KIO_FLOPPY_H

#include <kio/slavebase.h>
#include <kio/global.h>

#include "program.h"

#include <iostream>

#include <qobject.h>
#include <qstring.h>

struct StatInfo
{
   StatInfo():name(""),time(0),size(0),mode(0),freeSpace(0),isDir(false),isValid(false) {;};
   QString name;
   time_t time;
   int size;
   int mode;
   int freeSpace;
   bool isDir:1;
   bool isValid:1;
};


class FloppyProtocol : public KIO::SlaveBase
{
   public:
      FloppyProtocol (const QCString &pool, const QCString &app );
      virtual ~FloppyProtocol();

      virtual void listDir( const KURL& url);
      virtual void stat( const KURL & url);
      virtual void mkdir( const KURL& url, int);
      virtual void del( const KURL& url, bool isfile);
      virtual void rename(const KURL &src, const KURL &dest, bool overwrite);
      virtual void get( const KURL& url );
      virtual void put( const KURL& url, int _mode,bool overwrite, bool _resume );
      //virtual void copy( const KURL& src, const KURL &dest, int, bool overwrite );
   protected:
      Program *m_mtool;
      int readStdout();
      int readStderr();

      StatInfo createStatInfo(const QString line, bool makeStat=false, const QString& dirName="");
      void createUDSEntry(const StatInfo& info, KIO::UDSEntry& entry);
      StatInfo _stat(const KURL& _url);
      int freeSpace(const KURL& url);

      bool stopAfterError(const KURL& url, const QString& drive);

      void clearBuffers();
      void terminateBuffers();
      char *m_stdoutBuffer;
      char *m_stderrBuffer;
      int m_stdoutSize;
      int m_stderrSize;
};

#endif
