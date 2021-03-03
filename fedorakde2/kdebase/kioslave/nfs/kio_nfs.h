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

#ifndef KIO_NFS_H
#define KIO_NFS_H

#include <kio/slavebase.h>
#include <kio/global.h>

#include <qmap.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qintdict.h>
#include <qtimer.h>

#define PORTMAP  //this seems to be required to compile on Solaris
#include <rpc/rpc.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#include <iostream>
using namespace std;

class NFSFileHandle
{
   public:
      NFSFileHandle();
      NFSFileHandle(const NFSFileHandle & handle);
      ~NFSFileHandle();
      NFSFileHandle& operator= (const NFSFileHandle& src);
      NFSFileHandle& operator= (const char* src);
      operator const char* () const {return m_handle;};
      bool isInvalid() const {return m_isInvalid;}
      void setInvalid() {m_isInvalid=TRUE;};
      time_t age() const;
   protected:
      char *m_handle;
      bool m_isInvalid;
      time_t m_detectTime;
};

ostream& operator<<(ostream&, const NFSFileHandle&);

typedef QMap<QString,NFSFileHandle> NFSFileHandleMap;


class NFSProtocol : public KIO::SlaveBase
{
   public:
      NFSProtocol (const QCString &pool, const QCString &app );
      virtual ~NFSProtocol();

      virtual void openConnection();
      virtual void closeConnection();

      virtual void setHost( const QString& host, int port, const QString& user, const QString& pass );

      virtual void put( const KURL& url, int _mode,bool _overwrite, bool _resume );
      virtual void get( const KURL& url );
      virtual void listDir( const KURL& url);
      virtual void symlink( const QString &target, const KURL &dest, bool );
      virtual void stat( const KURL & url);
      virtual void mkdir( const KURL& url, int permissions );
      virtual void del( const KURL& url, bool isfile);
      virtual void chmod(const KURL& url, int permissions );
      virtual void rename(const KURL &src, const KURL &dest, bool overwrite);
      virtual void copy( const KURL& src, const KURL &dest, int mode, bool overwrite );
   protected:
      void createVirtualDirEntry(KIO::UDSEntry & entry);
      bool checkForError(int clientStat, int nfsStat, const QString& text);
      bool isExportedDir(const QString& path);
      bool isRoot(const QString& path);
      void completeUDSEntry(KIO::UDSEntry& entry, fattr& attributes);
      void completeBadLinkUDSEntry(KIO::UDSEntry& entry, fattr& attributes);
      void completeAbsoluteLinkUDSEntry(KIO::UDSEntry& entry, const QCString& path);
      bool isValidLink(const QString& parentDir, const QString& linkDest);
      bool isAbsoluteLink(const QString& path);
      
      NFSFileHandle getFileHandle(QString path);

      NFSFileHandleMap m_handleCache;
      QIntDict<QString> usercache;      // maps long ==> QString *
      QIntDict<QString> groupcache;

      QStringList m_exportedDirs;
      QString m_currentHost;
      CLIENT *m_client;
      CLIENT *m_nfsClient;
      timeval total_timeout;
      timeval pertry_timeout;
      int m_sock;
      time_t m_lastCheck;
      void checkForOldFHs();
};

#endif
