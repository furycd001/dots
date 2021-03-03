/*
 * locale.h
 *
 * Copyright (c) 1998 Matthias Hoelzer <hoelzer@physik.uni-wuerzburg.de>
 * Copyright (c) 1999-2000 Hans Petter Bieker <bieker@kde.org>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#ifndef __LOCALE_H__
#define __LOCALE_H__

#include <qwidget.h>

class KLocaleAdvanced;
class KLanguageButton;
class KLocaleSample;

class KLocaleConfig : public QWidget
{
  Q_OBJECT

public:
  KLocaleConfig( KLocaleAdvanced *_locale,
		 QWidget *parent=0, const char *name=0);
  ~KLocaleConfig( );

  void loadLanguageList(KLanguageButton *combo, const QStringList &first);
  void loadCountryList(KLanguageButton *combo);

  void load();
  void save();
  void defaults();
  QString quickHelp() const;

public slots:
  void reTranslate();

signals:
  void translate();
  void resample();
  void languageChanged();
  void countryChanged();
  void chsetChanged();

private:
  KLocaleAdvanced *locale;
  QStringList langs;

  KLanguageButton *comboCountry,
    *comboLang,
    *comboChset;

  QLabel *labCountry,
    *labLang,
    *labChset;

private slots:
  void changedCountry(int);
  void changedLanguage(int);
  void changedCharset(int);
  void readLocale(const QString &path, QString &name, const QString &sub) const;
};

#endif
