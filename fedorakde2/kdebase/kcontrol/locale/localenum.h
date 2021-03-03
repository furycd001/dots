/*
 * localenum.h
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


#ifndef __KLOCALECONFIGNUM_H__
#define __KLOCALECONFIGNUM_H__

#include <qwidget.h>

class QCheckBox;
class QComboBox;
class QLineEdit;

class KLocaleAdvanced;
class KLanguageCombo;

class KLocaleConfigNumber : public QWidget
{
  Q_OBJECT

public:
  KLocaleConfigNumber( KLocaleAdvanced *_locale,
		       QWidget *parent=0, const char *name=0);
  ~KLocaleConfigNumber( );

  void load();
  void save();
  void defaults();

public slots:
  void reset();
  void reTranslate();

private slots:
  // Numbers
  void slotMonPosSignChanged(const QString &t);
  void slotMonNegSignChanged(const QString &t);
  void slotDecSymChanged(const QString &t);
  void slotThoSepChanged(const QString &t);

signals:
  void translate();
  void resample();

private:
  KLocaleAdvanced *locale;

  // Numbers
  QLabel *labDecSym;
  QLineEdit *edDecSym;
  QLabel *labThoSep;
  QLineEdit *edThoSep;
  QLabel *labMonPosSign;
  QLineEdit *edMonPosSign;
  QLabel *labMonNegSign;
  QLineEdit *edMonNegSign;
};

#endif
