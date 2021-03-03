/*
 * localemon.cpp
 *
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

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qobjectlist.h>
#include <qwhatsthis.h>
#include <qlayout.h>
#include <qvgroupbox.h>
#include <qvbox.h>

#include <kglobal.h>
#include <kdialog.h>
#include <kconfig.h>
#include <ksimpleconfig.h>
#include <kstddirs.h>
#include <knumvalidator.h>

#include "klocaleadv.h"
#include "klocalesample.h"
#include "toplevel.h"
#include "localemon.h"
#include "localemon.moc"

KLocaleConfigMoney::KLocaleConfigMoney(KLocaleAdvanced *_locale,
				       QWidget *parent, const char*name)
 : QWidget(parent, name),
   locale(_locale)
{
    // Money
    QGridLayout *lay = new QGridLayout(this, 5, 2,
				       KDialog::marginHint(),
				       KDialog::spacingHint());

    labMonCurSym = new QLabel(this, I18N_NOOP("Currency symbol:"));
    lay->addWidget(labMonCurSym, 0, 0);
    edMonCurSym = new QLineEdit(this);
    lay->addWidget(edMonCurSym, 0, 1);
    connect( edMonCurSym, SIGNAL( textChanged(const QString &) ), SLOT( slotMonCurSymChanged(const QString &) ) );

    labMonDecSym = new QLabel(this, I18N_NOOP("Decimal symbol:"));
    lay->addWidget(labMonDecSym, 1, 0);
    edMonDecSym = new QLineEdit(this);
    lay->addWidget(edMonDecSym, 1, 1);
    connect( edMonDecSym, SIGNAL( textChanged(const QString &) ), SLOT( slotMonDecSymChanged(const QString &) ) );

    labMonThoSep = new QLabel(this, I18N_NOOP("Thousands separator:"));
    lay->addWidget(labMonThoSep, 2, 0);
    edMonThoSep = new QLineEdit(this);
    lay->addWidget(edMonThoSep, 2, 1);
    connect( edMonThoSep, SIGNAL( textChanged(const QString &) ), SLOT( slotMonThoSepChanged(const QString &) ) );

    labMonFraDig = new QLabel(this, I18N_NOOP("Fract digits:"));
    lay->addWidget(labMonFraDig, 3, 0);
    edMonFraDig = new QLineEdit(this);
    edMonFraDig->setValidator( new KIntValidator( 0,10,edMonFraDig ) );
    lay->addWidget(edMonFraDig, 3, 1);
    connect( edMonFraDig, SIGNAL( textChanged(const QString &) ),
	     SLOT( slotMonFraDigChanged(const QString &) ) );

    QWidget *vbox = new QVBox(this);
    lay->addMultiCellWidget(vbox, 4, 4, 0, 1);
    QGroupBox *grp;
    grp = new QGroupBox( vbox, I18N_NOOP("Positive") );
    grp->setColumns(2);
    labMonPosPreCurSym = new QLabel(grp, I18N_NOOP("Prefix currency symbol:"));
    chMonPosPreCurSym = new QCheckBox(grp);
    connect( chMonPosPreCurSym, SIGNAL( clicked() ),
	     SLOT( slotMonPosPreCurSymChanged() ) );

    labMonPosMonSignPos = new QLabel(grp, I18N_NOOP("Sign position:"));
    cmbMonPosMonSignPos = new QComboBox(grp, "signpos");
    connect( cmbMonPosMonSignPos, SIGNAL( activated(int) ),
	     SLOT( slotMonPosMonSignPosChanged(int) ) );

    grp = new QGroupBox( vbox, I18N_NOOP("Negative") );
    grp->setColumns(2);
    labMonNegPreCurSym = new QLabel(grp, I18N_NOOP("Prefix currency symbol:"));
    chMonNegPreCurSym = new QCheckBox(grp);
    connect( chMonNegPreCurSym, SIGNAL( clicked() ),
	     SLOT( slotMonNegPreCurSymChanged() ) );

    labMonNegMonSignPos = new QLabel(grp, I18N_NOOP("Sign position:"));
    cmbMonNegMonSignPos = new QComboBox(grp, "signpos");
    connect( cmbMonNegMonSignPos, SIGNAL( activated(int) ),
	     SLOT( slotMonNegMonSignPosChanged(int) ) );

    // insert some items
    int i = 5;
    while (i--)
	{
	    cmbMonPosMonSignPos->insertItem(QString::null);
	    cmbMonNegMonSignPos->insertItem(QString::null);
	}

    lay->setColStretch(1, 1);
    lay->addRowSpacing(5, 0);

    adjustSize();
}

KLocaleConfigMoney::~KLocaleConfigMoney()
{
}

/**
 * Load stored configuration.
 */
void KLocaleConfigMoney::load()
{
  // temperary use of our locale as the global locale
  KLocale *lsave = KGlobal::_locale;
  KGlobal::_locale = locale;

  KConfig *config = KGlobal::config();
  KConfigGroupSaver saver(config, QString::fromLatin1("Locale"));

  KSimpleConfig ent(locate("locale",
			   QString::fromLatin1("l10n/%1/entry.desktop")
			   .arg(locale->country())), true);
  ent.setGroup(QString::fromLatin1("KCM Locale"));

  // different tmp variables
  QString str;
  int i;
  bool b;

  // Currency symbol
  str = config->readEntry(QString::fromLatin1("CurrencySymbol"));
  if (str.isNull())
    str = ent.readEntry(QString::fromLatin1("CurrencySymbol"), QString::fromLatin1("$"));
  locale->setCurrencySymbol(str);

  // Decimal symbol
  str = config->readEntry(QString::fromLatin1("MonetaryDecimalSymbol"));
  if (str.isNull())
    str = ent.readEntry(QString::fromLatin1("MonetaryDecimalSymbol"), QString::fromLatin1("."));
  locale->setMonetaryDecimalSymbol(str);

  // Thousends separator
  str = config->readEntry(QString::fromLatin1("MonetaryThousandsSeparator"));
  if (str.isNull())
    str = ent.readEntry(QString::fromLatin1("MonetaryThousandsSeparator"), QString::fromLatin1(","));
  str.replace(QRegExp(QString::fromLatin1("$0")), QString::null);
  locale->setMonetaryThousandsSeparator(str);

  // Fract digits
  i = config->readNumEntry(QString::fromLatin1("FractDigits"), -1);
  if (i == -1)
    i = ent.readNumEntry(QString::fromLatin1("FractDigits"), 2);
  locale->setFracDigits(i);

  // PositivePrefixCurrencySymbol
  b = ent.readBoolEntry(QString::fromLatin1("PositivePrefixCurrencySymbol"), true);
  b = config->readNumEntry(QString::fromLatin1("PositivePrefixCurrencySymbol"), b);
  locale->setPositivePrefixCurrencySymbol(b);

  // NegativePrefixCurrencySymbol
  b = ent.readBoolEntry(QString::fromLatin1("NegativePrefixCurrencySymbol"), true);
  b = config->readNumEntry(QString::fromLatin1("NegativePrefixCurrencySymbol"), b);
  locale->setNegativePrefixCurrencySymbol(b);

  // PositiveMonetarySignPosition
  i = config->readNumEntry(QString::fromLatin1("PositiveMonetarySignPosition"), -1);
  if (i == -1)
    i = ent.readNumEntry(QString::fromLatin1("PositiveMonetarySignPosition"), KLocale::BeforeQuantityMoney);
  locale->setPositiveMonetarySignPosition((KLocale::SignPosition)i);

  // NegativeMonetarySignPosition
  i = config->readNumEntry(QString::fromLatin1("NegativeMonetarySignPosition"), -1);
  if (i == -1)
    i = ent.readNumEntry(QString::fromLatin1("NegativeMonetarySignPosition"), KLocale::ParensAround);
  locale->setNegativeMonetarySignPosition((KLocale::SignPosition)i);

  // update the widgets
  edMonCurSym->setText(locale->currencySymbol());
  edMonDecSym->setText(locale->monetaryDecimalSymbol());
  edMonThoSep->setText(locale->monetaryThousandsSeparator());
  edMonFraDig->setText(locale->formatNumber(locale->fracDigits(), 0));
  chMonPosPreCurSym->setChecked(locale->positivePrefixCurrencySymbol());
  chMonNegPreCurSym->setChecked(locale->negativePrefixCurrencySymbol());
  cmbMonPosMonSignPos->setCurrentItem(locale->positiveMonetarySignPosition());
  cmbMonNegMonSignPos->setCurrentItem(locale->negativeMonetarySignPosition());

  // restore the old global locale
  KGlobal::_locale = lsave;
}

void KLocaleConfigMoney::save()
{
  KSimpleConfig *c = new KSimpleConfig(QString::fromLatin1("kdeglobals"), false);
  c->setGroup(QString::fromLatin1("Locale"));
  // Write something to the file to make it dirty
  c->writeEntry(QString::fromLatin1("CurrencySymbol"), QString::null);

  c->deleteEntry(QString::fromLatin1("CurrencySymbol"), false);
  c->deleteEntry(QString::fromLatin1("MonetaryDecimalSymbol"), false);
  c->deleteEntry(QString::fromLatin1("MonetaryThousandsSeparator"), false);
  c->deleteEntry(QString::fromLatin1("PositiveSign"), false);
  c->deleteEntry(QString::fromLatin1("NegativeSign"), false);
  c->deleteEntry(QString::fromLatin1("FractDigits"), false);
  c->deleteEntry(QString::fromLatin1("PositivePrefixCurrencySymbol"), false);
  c->deleteEntry(QString::fromLatin1("NegativePrefixCurrencySymbol"), false);
  c->deleteEntry(QString::fromLatin1("PositiveMonetarySignPosition"), false);
  c->deleteEntry(QString::fromLatin1("NegativeMonetarySignPosition"), false);
  delete c;

  KConfig *config = KGlobal::config();
  KConfigGroupSaver saver(config, QString::fromLatin1("Locale"));

  KSimpleConfig ent(locate("locale",
			   QString::fromLatin1("l10n/%1/entry.desktop")
			   .arg(locale->country())), true);
  ent.setGroup(QString::fromLatin1("KCM Locale"));

  QString str;
  int i;
  bool b;

  str = ent.readEntry(QString::fromLatin1("CurrencySymbol"), QString::fromLatin1("$"));
  if (str != locale->currencySymbol())
    config->writeEntry(QString::fromLatin1("CurrencySymbol"), locale->currencySymbol(), true, true);

  str = ent.readEntry(QString::fromLatin1("MonetaryDecimalSymbol"), QString::fromLatin1("."));
  if (str != locale->monetaryDecimalSymbol())
    config->writeEntry(QString::fromLatin1("MonetaryDecimalSymbol"), locale->monetaryDecimalSymbol(), true, true);

  str = ent.readEntry(QString::fromLatin1("MonetaryThousandsSeparator"), QString::fromLatin1(","));
  str.replace(QRegExp(QString::fromLatin1("$0")), QString::null);
  if (str != locale->monetaryThousandsSeparator())
    config->writeEntry(QString::fromLatin1("MonetaryThousandsSeparator"), QString::fromLatin1("$0%1$0").arg(locale->monetaryThousandsSeparator()), true, true);

  str = ent.readEntry(QString::fromLatin1("PositiveSign"));
  if (str != locale->positiveSign())
    config->writeEntry(QString::fromLatin1("PositiveSign"), locale->positiveSign(), true, true);

  str = ent.readEntry(QString::fromLatin1("NegativeSign"), QString::fromLatin1("-"));
  if (str != locale->negativeSign())
    config->writeEntry(QString::fromLatin1("NegativeSign"), locale->negativeSign(), true, true);

  i = ent.readNumEntry(QString::fromLatin1("FractDigits"), 2);
  if (i != locale->fracDigits())
    config->writeEntry(QString::fromLatin1("FractDigits"), locale->fracDigits(), true, true);

  b = ent.readNumEntry(QString::fromLatin1("PositivePrefixCurrencySymbol"), true);
  if (b != locale->positivePrefixCurrencySymbol())
    config->writeEntry(QString::fromLatin1("PositivePrefixCurrencySymbol"), locale->positivePrefixCurrencySymbol(), true, true);

  b = ent.readNumEntry(QString::fromLatin1("NegativePrefixCurrencySymbol"), true);
  if (b != locale->negativePrefixCurrencySymbol())
    config->writeEntry(QString::fromLatin1("NegativePrefixCurrencySymbol"), locale->negativePrefixCurrencySymbol(), true, true);

  i = ent.readNumEntry(QString::fromLatin1("PositiveMonetarySignPosition"), (int)KLocale::BeforeQuantityMoney);
  if (i != locale->positiveMonetarySignPosition())
    config->writeEntry(QString::fromLatin1("PositiveMonetarySignPosition"), (int)locale->positiveMonetarySignPosition(), true, true);

  i = ent.readNumEntry(QString::fromLatin1("NegativeMonetarySignPosition"), (int)KLocale::ParensAround);
  if (i != locale->negativeMonetarySignPosition())
    config->writeEntry(QString::fromLatin1("NegativeMonetarySignPosition"), (int)locale->negativeMonetarySignPosition(), true, true);

  config->sync();
}

void KLocaleConfigMoney::defaults()
{
  reset();
}

void KLocaleConfigMoney::slotMonCurSymChanged(const QString &t)
{
  locale->setCurrencySymbol(t);
  emit resample();
}

void KLocaleConfigMoney::slotMonDecSymChanged(const QString &t)
{
  locale->setMonetaryDecimalSymbol(t);
  emit resample();
}

void KLocaleConfigMoney::slotMonThoSepChanged(const QString &t)
{
  locale->setMonetaryThousandsSeparator(t);
  emit resample();
}

void KLocaleConfigMoney::slotMonFraDigChanged(const QString &t)
{
  locale->setFracDigits((int)locale->readNumber(t));
  emit resample();
}

void KLocaleConfigMoney::slotMonPosPreCurSymChanged()
{
  locale->setPositivePrefixCurrencySymbol(chMonPosPreCurSym->isChecked());
  emit resample();
}

void KLocaleConfigMoney::slotMonNegPreCurSymChanged()
{
  locale->setNegativePrefixCurrencySymbol(chMonNegPreCurSym->isChecked());
  emit resample();
}

void KLocaleConfigMoney::slotMonPosMonSignPosChanged(int i)
{
  locale->setPositiveMonetarySignPosition((KLocale::SignPosition)i);
  emit resample();
}

void KLocaleConfigMoney::slotMonNegMonSignPosChanged(int i)
{
  locale->setNegativeMonetarySignPosition((KLocale::SignPosition)i);
  emit resample();
}

/**
 * Reset to defaults. This will be ran when user e.g. changes country.
 */
void KLocaleConfigMoney::reset()
{
  // temperary use of our locale as the global locale
  KLocale *lsave = KGlobal::_locale;
  KGlobal::_locale = locale;

  KSimpleConfig ent(locate("locale",
			   QString::fromLatin1("l10n/%1/entry.desktop")
			   .arg(locale->country())), true);
  ent.setGroup(QString::fromLatin1("KCM Locale"));

  locale->setCurrencySymbol(ent.readEntry(QString::fromLatin1("CurrencySymbol"), QString::fromLatin1("$")));
  locale->setMonetaryDecimalSymbol(ent.readEntry(QString::fromLatin1("MonetaryDecimalSymbol"), QString::fromLatin1(".")));
  QString str = ent.readEntry(QString::fromLatin1("MonetaryThousandsSeparator"), QString::fromLatin1(","));
  str.replace(QRegExp(QString::fromLatin1("$0")), QString::null);
  locale->setMonetaryThousandsSeparator(str);
  locale->setFracDigits(ent.readNumEntry(QString::fromLatin1("FractDigits"), 2));
  locale->setPositivePrefixCurrencySymbol(ent.readBoolEntry(QString::fromLatin1("PositivePrefixCurrencySymbol"), true));
  locale->setNegativePrefixCurrencySymbol(ent.readBoolEntry(QString::fromLatin1("NegativePrefixCurrencySymbol"), true));
  locale->setPositiveMonetarySignPosition((KLocale::SignPosition)ent.readNumEntry(QString::fromLatin1("PositiveMonetarySignPosition"), KLocale::BeforeQuantityMoney));
  locale->setNegativeMonetarySignPosition((KLocale::SignPosition)ent.readNumEntry(QString::fromLatin1("NegativeMonetarySignPosition"), KLocale::ParensAround));

  edMonCurSym->setText(locale->currencySymbol());
  edMonDecSym->setText(locale->monetaryDecimalSymbol());
  edMonThoSep->setText(locale->monetaryThousandsSeparator());
  edMonFraDig->setText(locale->formatNumber(locale->fracDigits(), 0));
  chMonPosPreCurSym->setChecked(locale->positivePrefixCurrencySymbol());
  chMonNegPreCurSym->setChecked(locale->negativePrefixCurrencySymbol());
  cmbMonPosMonSignPos->setCurrentItem(locale->positiveMonetarySignPosition());
  cmbMonNegMonSignPos->setCurrentItem(locale->negativeMonetarySignPosition());

  // restore the old global locale
  KGlobal::_locale = lsave;
}

void KLocaleConfigMoney::reTranslate()
{
  QObjectList list;
  list.append(cmbMonPosMonSignPos);
  list.append(cmbMonNegMonSignPos);

  QComboBox *wc;
  for(QObjectListIt li(list) ; (wc = (QComboBox *)li.current()) != 0; ++li)
  {
    wc->changeItem(locale->translate("Parens around"), 0);
    wc->changeItem(locale->translate("Before quantity money"), 1);
    wc->changeItem(locale->translate("After quantity money"), 2);
    wc->changeItem(locale->translate("Before money"), 3);
    wc->changeItem(locale->translate("After money"), 4);
  }

  QString str;

  str = locale->translate( "Here you can enter your usual currency "
			   "symbol, e.g. $ or DM."
			   "<p>Please note that the Euro symbol may not be "
			   "available on your system, depending on the "
			   "distribution you use." );
  QWhatsThis::add( labMonCurSym, str );
  QWhatsThis::add( edMonCurSym, str );
  str = locale->translate( "Here you can define the decimal separator used "
			   "to display monetary values."
			   "<p>Note that the decimal separator used to "
			   "display other numbers has to be defined "
			   "separately (see the 'Numbers' tab)." );
  QWhatsThis::add( labMonDecSym, str );
  QWhatsThis::add( edMonDecSym, str );

  str = locale->translate( "Here you can define the thousands separator "
			   "used to display monetary values."
			   "<p>Note that the thousands separator used to "
			   "display other numbers has to be defined "
			   "separately (see the 'Numbers' tab)." );
  QWhatsThis::add( labMonThoSep, str );
  QWhatsThis::add( edMonThoSep, str );

  str = locale->translate( "This determines the number of fract digits for "
			   "monetary values, i.e. the number of digits you "
			   "find <em>behind</em> the decimal separator. "
			   "Correct value is 2 for almost all people." );
  QWhatsThis::add( labMonFraDig, str );
  QWhatsThis::add( edMonFraDig, str );

  str = locale->translate( "If this option is checked, the currency sign "
			   "will be prefixed (i.e. to the left of the "
			   "value) for all positive monetary values. If "
			   "not, it will be postfixed (i.e. to the right)." );
  QWhatsThis::add( labMonPosPreCurSym, str );
  QWhatsThis::add( chMonPosPreCurSym, str );

  str = locale->translate( "If this option is checked, the currency sign "
			   "will be prefixed (i.e. to the left of the "
			   "value) for all negative monetary values. If "
			   "not, it will be postfixed (i.e. to the right)." );
  QWhatsThis::add( labMonNegPreCurSym, str );
  QWhatsThis::add( chMonNegPreCurSym, str );

  str = locale->translate( "Here you can select how a positive sign will be "
			   "positioned. This only affects monetary values." );
  QWhatsThis::add( labMonPosMonSignPos, str );
  QWhatsThis::add( cmbMonPosMonSignPos, str );

  str = locale->translate( "Here you can select how a negative sign will "
			   "be positioned. This only affects monetary "
			   "values." );
  QWhatsThis::add( labMonNegMonSignPos, str );
  QWhatsThis::add( cmbMonNegMonSignPos, str );
}
