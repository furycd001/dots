/*****************************************************************
 * drkonqi - The KDE Crash Handler
 *
 * $Id: backtrace.cpp,v 1.11 2000/11/04 14:42:41 faure Exp $
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

#include <qfile.h>

#include <kprocess.h>
#include <kdebug.h>
#include <kstddirs.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <ktempfile.h>

#include "krashconf.h"
#include "backtrace.h"
#include "backtrace.moc"

BackTrace::BackTrace(const KrashConfig *krashconf, QObject *parent,
                     const char *name)
  : QObject(parent, name),
    m_krashconf(krashconf), m_temp(0)
{
  m_proc = new KProcess;
}

BackTrace::~BackTrace()
{
  pid_t pid = m_proc ? m_proc->getPid() : 0;
  // we don't want the gdb process to hang around
  delete m_proc; // this will kill gdb (SIGKILL, signal 9)

  // continue the process we ran backtrace on. Gdb sends SIGSTOP to the
  // process. For some reason it doesn't work if we send the signal before
  // gdb has exited, so we better wait for it.
  // Do not touch it if we never ran backtrace.
  if (pid) {
    waitpid(pid, NULL, 0);
    kill(m_krashconf->pid(), SIGCONT);
  }

  delete m_temp;
}

void BackTrace::start()
{
  QString exec = m_krashconf->tryExec();
  if ( !exec.isEmpty() && KStandardDirs::findExe(exec).isEmpty() )
  {
    KMessageBox::error(0L,i18n("%1 not found. Check if it is installed, you need it to generate backtraces.\n"
                               "If you have another debugger, contact kde-devel@kde.org for help on how to set it up.").arg(exec));
    return;
  }
  m_temp = new KTempFile;
  m_temp->setAutoDelete(TRUE);
  int handle = m_temp->handle();
  ::write(handle, "bt\n", 3); // this is the command for a backtrace
  ::fsync(handle);

  // start the debugger
  m_proc = new KShellProcess;

  QString str = m_krashconf->debuggerBatch();
  m_krashconf->expandString(str, m_temp->name());

  *m_proc << str;

  connect(m_proc, SIGNAL(receivedStdout(KProcess*, char*, int)),
          SLOT(slotReadInput(KProcess*, char*, int)));
  connect(m_proc, SIGNAL(processExited(KProcess*)),
          SLOT(slotProcessExited(KProcess*)));

  m_proc->start ( KProcess::NotifyOnExit, KProcess::All );
}

void BackTrace::slotReadInput(KProcess *, char* buf, int buflen)
{
  QString newstr = QString::fromLocal8Bit(buf, buflen);
  m_strBt.append(newstr);

  emit append(newstr);
}

void BackTrace::slotProcessExited(KProcess *proc)
{
  // start it again
  kill(m_krashconf->pid(), SIGCONT);

  // remove crap
  m_strBt.replace(QRegExp(QString::fromLatin1("\\(no debugging symbols found\\)\\.\\.\\.\\n?")),QString::null);
  // how many " ?? " in the bt ?
  int unknown = m_strBt.contains(QString::fromLatin1(" ?? "));
  // how many lines in the bt ?
  int lines = m_strBt.contains('\n');
  bool tooShort = !m_strBt.find(QString::fromLatin1("#5"));
  if (proc->normalExit() && (proc->exitStatus() == 0) &&
      !m_strBt.isNull() && !tooShort && (unknown + 2 < lines)) {
    emit done();
    emit done(m_strBt);
  } else
    emit someError();
}
