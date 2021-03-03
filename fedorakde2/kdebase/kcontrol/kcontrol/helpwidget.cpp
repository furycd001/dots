/*
  Copyright (c) 2000 Matthias Elter <elter@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <qlayout.h>

#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
#include <kprocess.h>
#include <kapp.h>

#include <qpushbutton.h>

#include "quickhelp.h"
#include "helpwidget.h"
#include "helpwidget.moc"

HelpWidget::HelpWidget(QWidget *parent , const char *name)
  : QWidget(parent, name)
{
  QVBoxLayout *l = new QVBoxLayout(this);

  _browser = new QuickHelp(this);
  connect(_browser, SIGNAL(urlClick(const QString &)),
	  SLOT(urlClicked(const QString &)));
  connect(_browser, SIGNAL(mailClick(const QString &,const QString &)),
	  SLOT(mailClicked(const QString &,const QString &)));

  l->addWidget(_browser);

  setBaseText();
}

void HelpWidget::setText( const QString& docPath, const QString& text)
{
  docpath = docPath;
  if (text.isEmpty() && docPath.isEmpty())
    setBaseText();
  else if (docPath.isEmpty())
    _browser->setText(text);
  else
    _browser->setText(text + i18n("<br><br>To read the full manual click <a href=\"%1\">here</a>.")
		      .arg(docPath.local8Bit()));
}

void HelpWidget::setBaseText()
{
  _browser->setText(i18n("<h1>KDE Control Center</h1>"
			 "Sorry, there is no quick help available for the active control module."
			 "<br><br>"
			 "Click <a href = \"kcontrol/index.html\">here</a> to read the general control center manual.") );
}

void HelpWidget::urlClicked(const QString & url)
{
  KProcess process;
  process << "khelpcenter2"
          << "help:/" + url;
  process.start(KProcess::DontCare);
}

void HelpWidget::mailClicked(const QString &,const QString & addr)
{
  kapp->invokeMailer(addr, QString::null);
}
