/****************************************************************************
** $Id: qt/src/kernel/qsound.h   2.3.2   edited 2001-01-26 $
**
** Definition of QSound class and QAuServer internal class
**
** Created : 000117
**
** Copyright (C) 1999-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef QSOUND_H
#define QSOUND_H

#ifndef QT_H
#include "qobject.h"
#endif // QT_H

#ifndef QT_NO_SOUND

class QSoundData;
class QAuServer;
class QAuBucket;

class Q_EXPORT QSound : public QObject {
    Q_OBJECT
public:
    static bool available();
    static void play(const QString& filename);

    QSound(const QString& filename, QObject* parent=0, const char* name=0);
    ~QSound();

public slots:
    void play();

private:
    QSoundData* d;
};


/*
  QAuServer is an INTERNAL class.  If you wish to provide support for
  additional audio servers, you can make a subclass of QAuServer to do
  so, HOWEVER, your class may need to be re-engineered to some degree
  with each new Qt release, including minor releases.

  QAuBucket is whatever you want.
*/
class QAuServer : public QObject {
    Q_OBJECT

public:
    QAuServer(QObject* parent, const char* name);
    ~QAuServer();

    virtual void play(const QString& filename);
    virtual void play(QAuBucket* id)=0;
    virtual QAuBucket* newBucket(const QString& filename)=0;
    virtual void deleteBucket(QAuBucket* id)=0;
    virtual bool okay()=0;
};

#endif // QT_NO_SOUND

#endif
