
/***************************************************************************
                          kio_finger.h  -  description
                             -------------------
    begin                : Sun Aug 12 2000
    copyright            : (C) 2000 by Andreas Schlapbach
    email                : schlpbch@iam.unibe.ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef __kio_finger_h__
#define __kio_finger_h__

#include <qstring.h>
#include <qcstring.h>

#include <kurl.h>
#include <kprocess.h>
#include <kio/global.h>
#include <kio/slavebase.h>

class FingerProtocol : public QObject, public KIO::SlaveBase
{
  Q_OBJECT

public:

  FingerProtocol(const QCString &pool_socket, const QCString &app_socket);
  virtual ~FingerProtocol();

  virtual void mimetype(const KURL& url);
  virtual void get(const KURL& url);

private slots:
  void       slotGetStdOutput(KProcess*, char*, int);

private: 
  KURL                  *myURL;

  QString	        *myPerlPath; 		
  QString               *myFingerPath;
  QString               *myFingerPerlScript;  
  QString               *myFingerCSSFile;
  
  QString		*myStdStream;  

 
  KShellProcess	        *myKProcess;

  void       getProgramPath();
  void       parseCommandLine(const KURL& url);
};


#endif
