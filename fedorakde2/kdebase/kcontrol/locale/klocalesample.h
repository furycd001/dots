/*
 * locale.cpp
 *
 * Copyright (c) 1998 Matthias Hoelzer (hoelzer@physik.uni-wuerzburg.de)
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

#ifndef __KLOCALESAMPLE_H__
#define __KLOCALESAMPLE_H__

#include <qwidget.h>

class QLabel;
class QResizeEvent;

class KLocaleAdvanced;

class KLocaleSample : public QWidget
{
  Q_OBJECT
public:
  KLocaleSample(KLocaleAdvanced *_locale,
		QWidget *parent=0, const char*name=0);
  ~KLocaleSample();

public slots:
  void update();

protected slots:
  void slotUpdateTime();

private:
  KLocaleAdvanced *locale;
  QLabel *numberSample, *labNumber;
  QLabel *moneySample, *labMoney;
  QLabel *timeSample, *labTime;
  QLabel *dateSample, *labDate;
  QLabel *dateShortSample, *labDateShort;
};

#endif
