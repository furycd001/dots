/*
  Copyright (c) 2000 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>

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

#ifndef __SCANNER_H__
#define __SCANNER_H__


#include <kdialog.h>


#include <qstringlist.h>


class KProgress;
class QLabel;
class QPushButton;


class Scanner : public KDialog
{
  Q_OBJECT

public:

  Scanner(QWidget *parent=0, const char *name=0);
  ~Scanner();


private slots:

  void startScan();
  void scanOneFile();


private:

  KProgress   *_progress;
  QLabel      *_appIcon, *_appName, *_summary;
  QPushButton *_scan, *_cancel1;
  QStringList _templates;
  int         found, count;

};


#endif
