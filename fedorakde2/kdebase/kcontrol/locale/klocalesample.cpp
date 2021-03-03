/*
 * klocalesample.cpp
 *
 * Copyright (c) 1998 Matthias Hoelzer (hoelzer@physik.uni-wuerzburg.de)
 * Copyright (c) 1999 Preston Brown <pbrown@kde.org>
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

#include <qdatetime.h>
#include <qlabel.h>
#include <qwhatsthis.h>
#include <qlayout.h>
#include <qtimer.h>

#include <kglobal.h>
#include <kstddirs.h>
#include <kdebug.h>
#include <klocale.h>

#include "klocaleadv.h"
#include "klocalesample.h"
#include "klocalesample.moc"

KLocaleSample::KLocaleSample(KLocaleAdvanced *_locale,
			     QWidget *parent, const char*name)
  : QWidget(parent, name),
    locale(_locale)
{
  QGridLayout *lay = new QGridLayout(this, 5, 2);
  lay->setAutoAdd(TRUE);

  // Whatever the color scheme is, we want black text
  QColorGroup a = palette().active();
  a.setColor(QColorGroup::Foreground,Qt::black);
  QPalette pal(a,a,a);

  labNumber = new QLabel(this, I18N_NOOP("Numbers:"));
  labNumber->setPalette(pal);
  numberSample = new QLabel(this);
  numberSample->setPalette(pal);

  labMoney = new QLabel(this, I18N_NOOP("Money:"));
  labMoney->setPalette(pal);
  moneySample = new QLabel(this);
  moneySample->setPalette(pal);

  labDate = new QLabel(this, I18N_NOOP("Date:"));
  labDate->setPalette(pal);
  dateSample = new QLabel(this);
  dateSample->setPalette(pal);

  labDateShort = new QLabel(this, I18N_NOOP("Short date:"));
  labDateShort->setPalette(pal);
  dateShortSample = new QLabel(this);
  dateShortSample->setPalette(pal);

  labTime = new QLabel(this, I18N_NOOP("Time:"));
  labTime->setPalette(pal);
  timeSample = new QLabel(this);
  timeSample->setPalette(pal);

  lay->setColStretch(0, 1);
  lay->setColStretch(1, 3);

  QTimer *timer = new QTimer(this, "clock_timer");
  connect(timer, SIGNAL(timeout()), this, SLOT(slotUpdateTime()));
  timer->start(1000);
}

KLocaleSample::~KLocaleSample()
{
}

void KLocaleSample::slotUpdateTime()
{
  QDateTime dt = QDateTime::currentDateTime();

  dateSample->setText(locale->formatDate(dt.date(), false));
  dateShortSample->setText(locale->formatDate(dt.date(), true));
  timeSample->setText(locale->formatTime(dt.time(), true));
}

void KLocaleSample::update()
{
  numberSample->setText(locale->formatNumber(1234567.89) +
			QString::fromLatin1(" / ") +
			locale->formatNumber(-1234567.89));

  moneySample->setText(locale->formatMoney(123456789.00) +
		       QString::fromLatin1(" / ") +
		       locale->formatMoney(-123456789.00));

  slotUpdateTime();

  QString str;

  str = locale->translate("This is how numbers will be displayed.");
  QWhatsThis::add( labNumber,  str );
  QWhatsThis::add( numberSample, str );

  str = locale->translate("This is how monetary values will be displayed.");
  QWhatsThis::add( labMoney,    str );
  QWhatsThis::add( moneySample, str );

  str = locale->translate("This is how date values will be displayed.");
  QWhatsThis::add( labDate,    str );
  QWhatsThis::add( dateSample, str );

  str = locale->translate("This is how date values will be displayed using "
			  "a short notation.");
  QWhatsThis::add( labDateShort, str );
  QWhatsThis::add( dateShortSample, str );

  str = locale->translate("This is how the time will be displayed.");
  QWhatsThis::add( labTime,    str );
  QWhatsThis::add( timeSample, str );
}

