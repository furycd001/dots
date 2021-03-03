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

#ifndef KIO_SMBRO_H
#define KIO_SMBRO_H

#include <kio/slavebase.h>
#include <kio/global.h>

#include "my_process.h"

#include <sys/types.h>

#include <qobject.h>
#include <qstring.h>
#include <qdict.h>
#include <qmap.h>
#include <qcstring.h>

struct StatInfo
{
   StatInfo():name(""),time(0),size(0),mode(0),isDir(false),isValid(false) {;};
   QString name;
   time_t time;
   int size;
   int mode;
   bool isDir:1;
   bool isValid:1;
};

class SmbProtocol : public KIO::SlaveBase
{
   public:
      SmbProtocol (const QCString &pool, const QCString &app );
      virtual ~SmbProtocol();

      enum SmbReturnCode {SMB_OK, SMB_ERROR, SMB_WRONGPASSWORD, SMB_NOTHING};
      virtual void listDir( const KURL& url);
      virtual void stat( const KURL & url);
      virtual void get( const KURL& url );
      virtual void setHost(const QString& host, int port, const QString& user, const QString& pass );
   protected:
      SmbReturnCode waitUntilStarted(ClientProcess *proc,const QString& password);
      SmbReturnCode getShareInfo(ClientProcess* shareLister,const QString& password);
      //bool waitUntilStarted(ClientProcess *proc,const QString& password);
      //bool getShareInfo(ClientProcess* shareLister,const QString& password);
      ClientProcess* getProcess(const QString& host, const QString& share);

      int readOutput(int fd);

      StatInfo createStatInfo(const QString line);
      void createUDSEntry(const StatInfo& info, KIO::UDSEntry& entry);
      StatInfo _stat(const KURL& _url);
      void listShares();

      bool stopAfterError(const KURL& url, bool notSureWhetherErrorOccurred);

      void clearBuffer();
      void terminateBuffer();
      char *m_stdoutBuffer;
      int m_stdoutSize;
      QString m_currentHost;
      QCString m_nmbName;
      QCString m_ip;
      QDict<ClientProcess> m_processes;
      QMap<QString,int> m_months;

      //configuration data
      bool m_showHiddenShares;
      QString m_password;
      QString m_user;
      /*QString m_shareListingPassword;
      QString m_shareListingUser;

      QString m_shareAccessingPassword;
      QString m_shareAccessingUser;*/

      QString m_workgroup;
};

#endif
