/*****************************************************************
 * drkonqi - The KDE Crash Handler
 * 
 * $Id: debugger.cpp,v 1.11 2001/03/08 13:33:11 bero Exp $
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

#include <qtextview.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qhbox.h>
#include <qlabel.h>

#include <klocale.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kfiledialog.h>
#include <kmessagebox.h>

#include "backtrace.h"
#include "krashconf.h"
#include "debugger.h"
#include "debugger.moc"

KrashDebugger :: KrashDebugger (const KrashConfig *krashconf, QWidget *parent, const char *name)
  : QWidget( parent, name ),
    m_krashconf(krashconf),
    m_proctrace(0)
{
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setAutoAdd(TRUE);

  m_backtrace = new QTextView(this);
  m_backtrace->setTextFormat(Qt::PlainText);
  m_backtrace->setFont(KGlobalSettings::fixedFont());

  QHBox *hbox = new QHBox(this);
  m_status = new QLabel(hbox);
  hbox->setStretchFactor(m_status,10);
  m_copyButton = new QPushButton(i18n("Copy"),hbox);
  connect( m_copyButton, SIGNAL( clicked() ), this, SLOT( slotCopy() ) );
  m_copyButton->hide();
  m_saveButton = new QPushButton(i18n("Save to disk"),hbox);
  connect( m_saveButton, SIGNAL( clicked() ), this, SLOT( slotSave() ) );
  m_saveButton->hide();
}

KrashDebugger :: ~KrashDebugger()
{
  // This will SIGKILL gdb and SIGCONT program which crashed.
  //  delete m_proctrace;
}

void KrashDebugger :: slotDone()
{
  m_status->setText(i18n("Done."));
  m_copyButton->show();
  m_saveButton->show();
}

void KrashDebugger :: slotCopy()
{
  m_backtrace->selectAll();
  m_backtrace->copy();
}

void KrashDebugger :: slotSave()
{
  QString filename=KFileDialog::getSaveFileName(QString::null, QString::null, this, i18n("Select filename"));
  if(!filename.isEmpty()) {
    QFile f(filename);
    if(f.open(IO_WriteOnly)) {
      f.writeBlock(m_backtrace->text().local8Bit());
      f.close();
    } else {
      KMessageBox::sorry(this, i18n("Can't open file %1 for writing").arg(filename));
    }
  }
}

void KrashDebugger :: slotSomeError()
{
  m_status->setText(i18n("Unable to create backtrace."));
}

void KrashDebugger :: slotAppend(const QString &str)
{
  m_status->setText(i18n("Loading backtrace..."));

  // append doesn't work here because it will add a newline as well
  m_backtrace->setText(m_backtrace->text() + str);
}

void KrashDebugger :: showEvent(QShowEvent *e)
{
  QWidget::showEvent(e);
  startDebugger();
}

void KrashDebugger :: startDebugger()
{
  // Only start one copy
  if (m_proctrace) return;

  m_status->setText(i18n("Loading symbols..."));

  m_proctrace = new BackTrace(m_krashconf, this);

  connect(m_proctrace, SIGNAL(append(const QString &)),
	  SLOT(slotAppend(const QString &)));
  connect(m_proctrace, SIGNAL(done()), SLOT(slotDone()));
  connect(m_proctrace, SIGNAL(someError()), SLOT(slotSomeError()));

  m_proctrace->start();
}
