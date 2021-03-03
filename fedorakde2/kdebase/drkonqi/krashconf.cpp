/*****************************************************************
 * drkonqi - The KDE Crash Handler
 *
 * $Id: krashconf.cpp,v 1.18 2001/04/05 01:42:13 mueller Exp $
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

#include <kconfig.h>
#include <kglobal.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kdebug.h>

#include "krashconf.h"

KrashConfig :: KrashConfig()
{
  readConfig();
}

KrashConfig :: ~KrashConfig()
{
  delete m_aboutData;
}

void KrashConfig :: readConfig()
{
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  m_signalnum = args->getOption( "signal" ).toInt();
  m_pid = args->getOption( "pid" ).toInt();
  m_startedByKdeinit = args->isSet("kdeinit");
  m_execname = args->getOption( "appname" );
  if ( !args->getOption( "apppath" ).isEmpty() )
      m_execname.prepend( args->getOption( "apppath" )+"/" );

  QCString programname = args->getOption("programname");
  if (programname.isEmpty()) programname.setStr(I18N_NOOP("unknown"));
  // leak some memory... Well. It's only done once anyway :-)
  const char * progname = qstrdup(programname);
  m_aboutData = new KAboutData(args->getOption("appname"),
                    progname,
                    args->getOption("appversion"),
                    0, 0, 0, 0, 0,
                    args->getOption("bugaddress"));

  KConfig *config = KGlobal::config();
  config->setGroup(QString::fromLatin1("drkonqi"));

  // maybe we should check if it's relative?
  QString configname = config->readEntry(QString::fromLatin1("ConfigName"),
                                         QString::fromLatin1("enduser"));

  QString debuggername = config->readEntry(QString::fromLatin1("Debugger"),
                                           QString::fromLatin1("gdb"));

  KConfig debuggers(QString::fromLatin1("debuggers/%1rc").arg(debuggername),
                    true, false, "appdata");

  debuggers.setGroup(QString::fromLatin1("General"));
  m_debugger = debuggers.readEntry(QString::fromLatin1("Exec"));
  m_debuggerBatch = debuggers.readEntry(QString::fromLatin1("ExecBatch"));
  m_tryExec = debuggers.readEntry(QString::fromLatin1("TryExec"));

  KConfig preset(QString::fromLatin1("presets/%1rc").arg(configname),
                 true, false, "appdata");

  preset.setGroup(QString::fromLatin1("ErrorDescription"));
  if (preset.readBoolEntry(QString::fromLatin1("Enable"), true))
    m_errorDescriptionText = preset.readEntry(QString::fromLatin1("Name"));

  preset.setGroup(QString::fromLatin1("WhatToDoHint"));
  if (preset.readBoolEntry(QString::fromLatin1("Enable")))
    m_whatToDoText = preset.readEntry(QString::fromLatin1("Name"));

  preset.setGroup(QString::fromLatin1("General"));
  m_showbugreport = preset.readBoolEntry(QString::fromLatin1("ShowBugReportButton"), false);
  m_showdebugger = m_showbacktrace = m_pid != 0;
  if (m_showbacktrace) {
    m_showbacktrace = preset.readBoolEntry(QString::fromLatin1("ShowBacktraceButton"), true);
    m_showdebugger = preset.readBoolEntry(QString::fromLatin1("ShowDebugButton"), true);
  }

  bool b = preset.readBoolEntry(QString::fromLatin1("SignalDetails"), true);

  QString str = QString::number(m_signalnum);
  // use group unknown if signal not found
  if (!preset.hasGroup(str)) str = QString::fromLatin1("unknown");
  preset.setGroup(str);
  m_signalName = preset.readEntry(QString::fromLatin1("Name"));
  if (b)
    m_signalText = preset.readEntry(QString::fromLatin1("Comment"));
}

// replace some of the strings
void KrashConfig :: expandString(QString &str, QString tempFile) const
{
  int pos = -1;
  while ( (pos = str.findRev('%', pos)) != -1 ) {
    if (str.mid(pos, 8) == QString::fromLatin1("%appname"))
        str.replace(pos, 8, QString::fromLatin1(appName()));
    if (str.mid(pos, 9) == QString::fromLatin1("%execname"))
    {
      if (startedByKdeinit())
        str.replace(pos, 9, QString::fromLatin1("kdeinit"));
      else
        str.replace(pos, 9, m_execname);
    }
    else if (str.mid(pos, 7) == QString::fromLatin1("%signum"))
      str.replace(pos, 7, QString::number(signalNumber()));
    else if (str.mid(pos, 8) == QString::fromLatin1("%signame"))
      str.replace(pos, 8, signalName());
    else if (str.mid(pos, 9) == QString::fromLatin1("%progname"))
      str.replace(pos, 9, programName());
    else if (str.mid(pos, 4) == QString::fromLatin1("%pid"))
      str.replace(pos, 4, QString::number(pid()));
    else if (str.mid(pos, 9) == QString::fromLatin1("%tempfile"))
      str.replace(pos, 9, tempFile);
  }
}
