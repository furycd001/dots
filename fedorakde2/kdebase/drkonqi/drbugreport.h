/*****************************************************************
 * drkonqi - The KDE Crash Handler
 *
 * $Id: drbugreport.h,v 1.5 2000/08/05 12:55:05 bieker Exp $
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

#ifndef DRBUGREPORT_H
#define DRBUGREPORT_H

class KAboutData;

#include <kbugreport.h>

class DrKBugReport : public KBugReport
{
  Q_OBJECT

 public:
  /**
   * Constructor.
   */
  DrKBugReport(QWidget *parent = 0, bool modal = true,
	       const KAboutData *aboutData = 0);

 public:
  /**
   * Allows the debugger to set the default text in the editor.
   */
  void setText(const QString &str);

 protected slots:
  /**
   * OK has been clicked
   */
  virtual void slotOk( void );   

 private:
  QString startstring;
};

#endif
