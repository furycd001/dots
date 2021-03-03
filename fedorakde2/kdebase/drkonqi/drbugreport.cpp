/*****************************************************************
 * drkonqi - The KDE Crash Handler
 *
 * $Id: drbugreport.cpp,v 1.4 2000/08/05 12:55:05 bieker Exp $
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

#include <qmultilineedit.h>

#include <kmessagebox.h>
#include <klocale.h>

#include "drbugreport.moc"
#include "drbugreport.h"

DrKBugReport::DrKBugReport(QWidget *parent, bool modal,
			   const KAboutData *aboutData)
  : KBugReport(parent, modal, aboutData)
{
}

void DrKBugReport::setText(const QString &str)
{
  m_lineedit->setText(str);
  startstring = str.simplifyWhiteSpace();
}

void DrKBugReport::slotOk()
{
  if (!startstring.isEmpty() &&
      m_lineedit->text().simplifyWhiteSpace() == startstring)
  {
    QString msg = i18n("You have to edit the description\n"
		       "before the report can be sent.");
    KMessageBox::error(this,msg);
    return;
  }                            
  KBugReport::slotOk();
}

