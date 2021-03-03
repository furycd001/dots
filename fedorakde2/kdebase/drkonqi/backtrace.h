/*****************************************************************
 * drkonqi - The KDE Crash Handler
 *
 * $Id: backtrace.h,v 1.4 2000/09/11 17:39:09 faure Exp $
 *
 * Copyright (C) 2000 Hans Petter Bieker <bieker@kde.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************/

#ifndef BACKTRACE_H
#define BACKTRACE_H

class KProcess;
class KrashConfig;
class KTempFile;

#include <qobject.h>

class BackTrace : public QObject
{
  Q_OBJECT

 public:
  BackTrace(const KrashConfig *krashconf, QObject *parent,
            const char *name = 0);
  ~BackTrace();

  void start();

 signals:
  // Just the new text
  void append(const QString &str);

  void someError();
  void done();
  void done(const QString &);

 protected slots:
  void slotProcessExited(KProcess * proc);
  void slotReadInput(KProcess * proc, char * buf, int buflen);

 private:
  KProcess *m_proc;
  const KrashConfig *m_krashconf;
  KTempFile *m_temp;
  QString m_strBt;
};
#endif
