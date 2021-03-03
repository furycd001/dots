/*
  toplevel.h - A KControl Application

  written 1998 by Matthias Hoelzer

  Copyright 1998 Matthias Hoelzer.
  Copyright 1999-2000 Hans Petter Bieker <bieker@kde.org>.
  
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

#ifndef __TOPLEVEL_H__
#define __TOPLEVEL_H__

#include <kcmodule.h>

class QTabWidget;
class QGroupBox;

class KLocaleAdvanced;
class KLocaleConfig;
class KLocaleConfigMoney;
class KLocaleConfigNumber;
class KLocaleConfigTime;
class KLocaleSample;

class KLocaleApplication : public KCModule
{
  Q_OBJECT

public:
  KLocaleApplication(QWidget *parent, const char *name);
  ~KLocaleApplication();

  void load();
  void save();
  void defaults();
  QString quickHelp() const;

public slots:
  void reTranslate();
  void reset();
  void newChset();
  void moduleChanged(bool state);
  void updateSample();
  void update() { reTranslate(); updateSample(); };

private:
  KLocaleAdvanced *locale;

  QTabWidget          *tab;
  KLocaleConfig       *localemain;
  KLocaleConfigNumber *localenum;
  KLocaleConfigMoney  *localemon;
  KLocaleConfigTime   *localetime;

  QGroupBox           *gbox;
  KLocaleSample       *sample;
};

#endif
