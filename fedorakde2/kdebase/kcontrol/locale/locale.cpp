/*
 * locale.cpp
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

#include <qdir.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qtooltip.h>
#include <qiconset.h>
#include <qwhatsthis.h>

#include <kapp.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <ksimpleconfig.h>
#include <kcharsets.h>
#include <kdialog.h>

#include "klocaleadv.h"
#include "klanguagebutton.h"
#include "klocalesample.h"
#include "locale.h"
#include "locale.moc"
#include "toplevel.h"

KLocaleConfig::KLocaleConfig(KLocaleAdvanced *_locale,
                 QWidget *parent, const char *name)
  : QWidget (parent, name),
    locale(_locale)
{
    QGridLayout *lay = new QGridLayout(this, 7, 2,
                       KDialog::marginHint(),
                       KDialog::spacingHint());
    lay->setAutoAdd(TRUE);

    labCountry = new QLabel(this, I18N_NOOP("Country:"));
    comboCountry = new KLanguageButton( this );
    comboCountry->setFixedHeight(comboCountry->sizeHint().height());
    labCountry->setBuddy(comboCountry);
    connect( comboCountry, SIGNAL(activated(int)),
         this, SLOT(changedCountry(int)) );

    labLang = new QLabel(this, I18N_NOOP("Language:"));
    comboLang = new KLanguageButton( this );
    comboLang->setFixedHeight(comboLang->sizeHint().height());
    connect( comboLang, SIGNAL(activated(int)),
         this, SLOT(changedLanguage(int)) );

    labChset = new QLabel(this, I18N_NOOP("Charset:"));
    comboChset = new KLanguageButton( this );
    comboChset->setFixedHeight(comboChset->sizeHint().height());
    connect( comboChset, SIGNAL(activated(int)),
         this, SLOT(changedCharset(int)) );

    QStringList list = KGlobal::charsets()->availableCharsetNames();
    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
       comboChset->insertItem(*it, *it);

    lay->setColStretch(1, 1);
}

KLocaleConfig::~KLocaleConfig ()
{
}

void KLocaleConfig::loadLanguageList(KLanguageButton *combo, const QStringList &first)
{
  // temperary use of our locale as the global locale
  KLocale *lsave = KGlobal::_locale;
  KGlobal::_locale = locale;

  // clear the list
  combo->clear();

  QStringList prilang;
  // add the primary languages for the country to the list
  for ( QStringList::ConstIterator it = first.begin(); it != first.end(); ++it )
    {
        QString str = locate("locale", *it + QString::fromLatin1("/entry.desktop"));
        if (!str.isNull())
          prilang << str;
    }

  // add all languages to the list
  QStringList alllang = KGlobal::dirs()->findAllResources("locale",
                               QString::fromLatin1("*/entry.desktop"));
  alllang.sort();
  QStringList langlist = prilang;
  if (langlist.count() > 0)
    langlist << QString::null; // separator
  langlist += alllang;

  int menu_index = -2;
  QString submenu; // we are working on this menu
  for ( QStringList::ConstIterator it = langlist.begin();
    it != langlist.end(); ++it )
    {
        if ((*it).isNull())
        {
      combo->insertSeparator();
      submenu = QString::fromLatin1("other");
      combo->insertSubmenu(locale->translate("Other"), submenu, QString::null, -2);
          menu_index = -1; // first entries should _not_ be sorted
          continue;
        }
    KSimpleConfig entry(*it);
    entry.setGroup(QString::fromLatin1("KCM Locale"));
    QString name = entry.readEntry(QString::fromLatin1("Name"), locale->translate("without name"));

    QString path = *it;
    int index = path.findRev('/');
    path = path.left(index);
    index = path.findRev('/');
    path = path.mid(index+1);
    combo->insertLanguage(path, name, QString::null, submenu, menu_index);
    }
  // restore the old global locale
  KGlobal::_locale = lsave;
}

void KLocaleConfig::loadCountryList(KLanguageButton *combo)
{
  // temperary use of our locale as the global locale
  KLocale *lsave = KGlobal::_locale;
  KGlobal::_locale = locale;

  QString sub = QString::fromLatin1("l10n/");

  // clear the list
  combo->clear();

  QStringList regionlist = KGlobal::dirs()->findAllResources("locale",
                                 sub + QString::fromLatin1("*.desktop"));
  regionlist.sort();

  for ( QStringList::ConstIterator it = regionlist.begin();
    it != regionlist.end();
    ++it )
  {
    QString tag = *it;
    int index;

    index = tag.findRev('/');
    if (index != -1) tag = tag.mid(index + 1);

    index = tag.findRev('.');
    if (index != -1) tag.truncate(index);

    KSimpleConfig entry(*it);
    entry.setGroup(QString::fromLatin1("KCM Locale"));
    QString name = entry.readEntry(QString::fromLatin1("Name"),
                   locale->translate("without name"));

    combo->insertSubmenu( name, '-' + tag, sub );
  }

  // add all languages to the list
  QStringList countrylist = KGlobal::dirs()->findAllResources("locale",
                               sub + QString::fromLatin1("*/entry.desktop"));
  countrylist.sort();

  for ( QStringList::ConstIterator it = countrylist.begin();
    it != countrylist.end(); ++it )
    {
    KSimpleConfig entry(*it);
    entry.setGroup(QString::fromLatin1("KCM Locale"));
    QString name = entry.readEntry(QString::fromLatin1("Name"),
                       locale->translate("without name"));
    QString submenu = '-' + entry.readEntry(QString::fromLatin1("Region"));

    QString tag = *it;
    int index = tag.findRev('/');
    tag.truncate(index);
    index = tag.findRev('/');
    tag = tag.mid(index+1);
        int menu_index = combo->containsTag(tag) ? -1 : -2;
    combo->insertLanguage(tag, name, sub, submenu, menu_index);
    }
  // restore the old global locale
  KGlobal::_locale = lsave;
}

void KLocaleConfig::load()
{
  KConfig *config = KGlobal::config();
  config->setGroup(QString::fromLatin1("Locale"));

  QString lang = config->readEntry(QString::fromLatin1("Language"));
  lang = lang.left(lang.find(':')); // only use  the first lang
  locale->setLanguage(lang);

  QString country = config->readEntry(QString::fromLatin1("Country"),
                                      QString::fromLatin1("C"));
  locale->setCountry(country);

  QString charset = config->readEntry(QString::fromLatin1("Charset"),
                      QString::fromLatin1("iso8859-1"));
  emit chsetChanged();

  KSimpleConfig ent(locate("locale",
               QString::fromLatin1("l10n/%1/entry.desktop")
               .arg(country)), true);
  ent.setGroup(QString::fromLatin1("KCM Locale"));
  langs = ent.readListEntry(QString::fromLatin1("Languages"));
  if (langs.isEmpty()) langs = QString::fromLatin1("C");

  // load lists into widgets
  loadLanguageList(comboLang, langs);
  loadCountryList(comboCountry);

  // update widgets
  comboLang->setCurrentItem(locale->language());
  comboCountry->setCurrentItem(country);
  comboChset->setCurrentItem(charset);
}

void KLocaleConfig::readLocale(const QString &path, QString &name, const QString &sub) const
{
  // temperary use of our locale as the global locale
  KLocale *lsave = KGlobal::_locale;
  KGlobal::_locale = locale;

  // read the name
  QString filepath = sub;
  if (path.at(0) == '-')
    filepath += path.mid(1) + QString::fromLatin1(".desktop");
  else
    filepath += path + QString::fromLatin1("/entry.desktop");

  KSimpleConfig entry(locate("locale", filepath));
  entry.setGroup(QString::fromLatin1("KCM Locale"));
  name = entry.readEntry(QString::fromLatin1("Name"), locale->translate("without name"));

  // restore the old global locale
  KGlobal::_locale = lsave;
}

void KLocaleConfig::save()
{
  KConfigBase *config = KGlobal::config();

  config->setGroup(QString::fromLatin1("Locale"));

  config->writeEntry(QString::fromLatin1("Country"), comboCountry->currentTag(), true, true);
  config->writeEntry(QString::fromLatin1("Language"), comboLang->currentTag(), true, true);
  config->writeEntry(QString::fromLatin1("Charset"), comboChset->currentTag(), true, true);

  config->sync();
}

void KLocaleConfig::defaults()
{
  QString C = QString::fromLatin1("C");
  locale->setLanguage(C);
  locale->setCountry(C);

  loadLanguageList(comboLang, QStringList());
  loadCountryList(comboCountry);

  comboCountry->setCurrentItem(C);
  comboLang->setCurrentItem(C);
  comboChset->setCurrentItem(QString::fromLatin1("iso-8859-1"));

  emit resample();
  emit countryChanged();
}

QString KLocaleConfig::quickHelp() const
{
  return locale->translate("<h1>Locale</h1> Here you can select from several predefined"
    " national settings, i.e. your country, the language that will be used by the"
    " KDE desktop, the way numbers and dates are displayed etc. In most cases it will be"
    " sufficient to choose the country you live in. For instance KDE"
    " will automatically choose \"German\" as language if you choose"
    " \"Germany\" from the list. It will also change the time format"
    " to use 24 hours and and use comma as decimal separator.<p>"
    " Please note that you can still customize all this to suit your taste"
    " using the other tabs. However, the national settings for your country"
    " should be a good start. <p> If your country is not listed here, you"
    " have to adjust all settings to your needs manually.");
}

void KLocaleConfig::changedCountry(int i)
{
  QString country = comboCountry->tag(i);

  KSimpleConfig ent(locate("locale", QString::fromLatin1("l10n/") + country + QString::fromLatin1("/entry.desktop")), true);
  ent.setGroup(QString::fromLatin1("KCM Locale"));
  langs = ent.readListEntry(QString::fromLatin1("Languages"));

  QString lang = QString::fromLatin1("C");
  // use the first INSTALLED langauge in the list, or default to C
  for ( QStringList::Iterator it = langs.begin(); it != langs.end(); ++it )
    if (comboLang->containsTag(*it))
      {
    lang = *it;
    break;
      }

  locale->setLanguage(lang);
  locale->setCountry(country);

  comboLang->setCurrentItem(lang);

  emit countryChanged();
  emit resample();
}

void KLocaleConfig::changedLanguage(int i)
{
  locale->setLanguage(comboLang->tag(i));

  emit languageChanged();
  emit resample();
}

void KLocaleConfig::changedCharset(int)
{
  locale->setChset(comboChset->currentTag());

  emit chsetChanged();
}

void KLocaleConfig::reTranslate()
{
  QToolTip::add(comboCountry, locale->translate
        ( "This is were you live. KDE will use the defaults for "
          "this country.") );
  QToolTip::add(comboLang, locale->translate
        ( "All KDE programs will be displayed in this language (if "
          "available).") );
  QToolTip::add(comboChset, locale->translate
        ( "The preferred charset for fonts.") );

  QString str;

  str = locale->translate
    ( "Here you can choose your country. The settings "
      "for language, numbers etc. will automatically switch to the "
      "corresponding values." );
  QWhatsThis::add( labCountry, str );
  QWhatsThis::add( comboCountry, str );

  str = locale->translate
    ( "Here you can choose the language that will be used "
      "by KDE. If only US English is available, no translations have been "
      "installed. You can get translations packages for many languages from "
      "the place you got KDE from. <p> Note that some applications may not "
      "be translated to your language; in this case, they will automatically "
      "fall back to the default language, i.e. US English." );
  QWhatsThis::add( labLang, str );
  QWhatsThis::add( comboLang, str );

  str = locale->translate
    ( "Here you can choose the charset KDE uses to display "
      "text. ISO 8859-1 is the default and should work for you if you use a "
      "Western European language. If not, you may have to choose a different "
      "charset." );
  QWhatsThis::add( labChset, str );
  QWhatsThis::add( comboChset, str );
}
