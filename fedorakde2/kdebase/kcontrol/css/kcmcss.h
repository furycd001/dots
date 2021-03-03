/*
 *  Copyright (c) 2000 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 */

#ifndef __KCMCSS_H__
#define __KCMCSS_H__


#include <qmap.h>

#include <kcmodule.h>


class CSSConfigDialog;


class CSSConfig : public KCModule
{
  Q_OBJECT

public:
	  
  CSSConfig(QWidget *parent = 0L, const char *name = 0L);

  void load();
  void save();
  void defaults();
  QString quickHelp() const;

  
public slots:
 
  void configChanged();

  void preview();


private:

  QMap<QString,QString> cssDict();

  CSSConfigDialog *dialog;

};


#endif
