/*****************************************************************
 * drkonqi - The KDE Crash Handler
 *
 * $Id: krashconf.h,v 1.10 2001/04/05 01:42:13 mueller Exp $
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

#ifndef KRASHCONF_H
#define KRASHCONF_H

#include <kaboutdata.h>
#include <qstring.h>

class KrashConfig
{
 public:
  KrashConfig();
  ~KrashConfig();

  QString programName() const { return m_aboutData->programName(); };
  const char* appName() const { return m_aboutData->appName(); };
  QString debugger() const { return m_debugger; }
  QString debuggerBatch() const { return m_debuggerBatch; }
  QString tryExec() const { return m_tryExec; }
  const KAboutData *aboutData() const { return m_aboutData; }
  int signalNumber() const { return m_signalnum; };
  int pid() const { return m_pid; };
  bool showBacktrace() const { return m_showbacktrace; };
  bool showDebugger() const { return m_showdebugger && !m_debugger.isNull(); };
  bool showBugReport() const { return m_showbugreport; };
  bool startedByKdeinit() const { return m_startedByKdeinit; };
  QString signalName() const { return m_signalName; };
  QString signalText() const { return m_signalText; };
  QString whatToDoText() const { return m_whatToDoText; }
  QString errorDescriptionText() const { return m_errorDescriptionText; };

  void expandString(QString &str, QString tempFile = QString::null) const;

 private:
  void readConfig();

 private:
  KAboutData *m_aboutData;
  int m_pid;
  int m_signalnum;
  bool m_showdebugger;
  bool m_showbacktrace;
  bool m_showbugreport;
  bool m_startedByKdeinit;
  QString m_signalName;
  QString m_signalText;
  QString m_whatToDoText;
  QString m_errorDescriptionText;
  QString m_execname;

  QString m_debugger;
  QString m_debuggerBatch;
  QString m_tryExec;
};

#endif
