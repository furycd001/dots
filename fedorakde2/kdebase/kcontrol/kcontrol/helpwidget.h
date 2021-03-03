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

#ifndef __helpwidget_h__
#define __helpwidget_h__

#include <qwidget.h>

class QuickHelp;

class HelpWidget : public QWidget
{
  Q_OBJECT

public:
  HelpWidget(QWidget *parent, const char *name=0);

  void setText( const QString& docPath, const QString& text);
  void setBaseText();

protected slots:
  void urlClicked(const QString &);
  void mailClicked(const QString &,const QString &);

private:
  QuickHelp *_browser;
  QString docpath;
};

#endif
