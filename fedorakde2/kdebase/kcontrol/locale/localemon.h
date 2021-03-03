/*
 * localemon.h
 *
 * Copyright (c) 1999 Hans Petter Bieker <bieker@kde.org>
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


#ifndef __KLOCALECONFIGMON_H__
#define __KLOCALECONFIGMON_H__

#include <qwidget.h>

class QCheckBox;
class QComboBox;
class QLineEdit;

class KLocaleAdvanced;
class KLanguageCombo;

class KLocaleConfigMoney : public QWidget
{
  Q_OBJECT

public:
  KLocaleConfigMoney( KLocaleAdvanced *_locale, QWidget *parent=0, const char *name=0);
  ~KLocaleConfigMoney( );

  void load();
  void save();
  void defaults();

public slots:
  void reset();
  void reTranslate();

private slots:
  // Money
  void slotMonCurSymChanged(const QString &t);
  void slotMonDecSymChanged(const QString &t);
  void slotMonThoSepChanged(const QString &t);
  void slotMonFraDigChanged(const QString &t);
  void slotMonPosPreCurSymChanged();
  void slotMonNegPreCurSymChanged();
  void slotMonPosMonSignPosChanged(int i);
  void slotMonNegMonSignPosChanged(int i);

signals:
  void translate();
  void resample();

private:
  KLocaleAdvanced *locale;
  KLocaleSample *sample;

  // Money
  QLabel *labMonCurSym;
  QLineEdit *edMonCurSym;
  QLabel *labMonDecSym;
  QLineEdit *edMonDecSym;
  QLabel *labMonThoSep;
  QLineEdit *edMonThoSep;
  QLabel *labMonFraDig;
  QLineEdit *edMonFraDig;

  QLabel *labMonPosPreCurSym;
  QCheckBox *chMonPosPreCurSym;
  QLabel *labMonNegPreCurSym;
  QCheckBox *chMonNegPreCurSym;
  QLabel *labMonPosMonSignPos;
  QComboBox *cmbMonPosMonSignPos;
  QLabel *labMonNegMonSignPos;
  QComboBox *cmbMonNegMonSignPos;
};

#endif
