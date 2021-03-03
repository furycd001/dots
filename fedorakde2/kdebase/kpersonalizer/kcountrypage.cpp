/***************************************************************************
                          kcountrypage.cpp  -  description
                             -------------------
    begin                : Tue May 22 2001
    copyright            : (C) 2001 by Ralf Nolden
    email                : nolden@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <stdlib.h>

#include <qstringlist.h>
#include <qlabel.h>
#include <qcheckbox.h>

#include <kapp.h>
#include <ksimpleconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
#include <kstddirs.h>
#include <kconfig.h>
#include <dcopclient.h>

#include "klanguagebutton.h"
#include "klocaleadv.h"

#include "kcountrypage.h"

KCountryPage::KCountryPage(QWidget *parent, const char *name ) : KCountryPageDlg(parent,name) {

  // I want to have this in before message freeze starts
  // expect live behind this after beta1
  cb_enlargeFonts->hide();

  px_introSidebar->setPixmap(locate("data", "kpersonalizer/pics/step1.png"));
  locale = new KLocaleAdvanced("kpersonalizer");
  locale->setLanguage("C");
  languageSet = false;

  connect(cb_country, SIGNAL(activated(int)), SLOT(setLangForCountry(int)));

  // set appropriate KDE version (kapp.h)
  txt_welcome->setText(i18n("<h3>Welcome to KDE %1!</h3>").arg(KDE_VERSION_STRING));


  KConfig *config = KGlobal::config();
  config->setGroup(QString::fromLatin1("Locale"));

  QString lang = config->readEntry(QString::fromLatin1("Language"));
  lang = lang.lower();
  lang = lang.left(lang.find(':')); // only use the first lang

  QString country = config->readEntry(QString::fromLatin1("Country"),
                                      QString::fromLatin1("C"));
  if(country == QString::fromLatin1("C")) {
    country = QString::fromLatin1(getenv("LANG"));
    if(country.left(5) == "nn_NO") // glibc's nn_NO is KDE's no_NY
      country = "no";
    if(country.contains("_"))
      country = country.mid(country.find("_")+1);
    if(country.contains("."))
      country = country.left(country.find("."));
    if(country.contains("@"))
      country = country.left(country.find("@"));
    if(country != "C")
      country = country.lower();
    if(country == "en") // special-case "en" - should be "en_GB" or "en_US", but plain "en" is in use quite often
      country = "C";
  }

  KSimpleConfig ent(locate("locale",
               QString::fromLatin1("l10n/%1/entry.desktop")
               .arg(country)), true);
  ent.setGroup(QString::fromLatin1("KCM Locale"));
  langs = ent.readListEntry(QString::fromLatin1("Languages"));
  if (langs.isEmpty()) langs = QString::fromLatin1("C");

  loadCountryList(cb_country);
  loadLanguageList(cb_language, langs);

  QString compare = lang;
  if(lang.isEmpty())
  {
    compare = langs.first();
    for(QStringList::Iterator it = langs.begin(); it != langs.end(); ++it)
    {
	if(*it == QString::fromLatin1(getenv("LANG")).mid(3, 2).lower())
	    compare = *it;
    }
  }
  if(compare == "c")
    compare = "C";

  // Highlight the users's language
  int bestmatch = 0;
  int best = -1;
  for(int i = 0; i < cb_language->count(); i++)
  {
    int match=0;
    if(cb_language->tag(i) == "C")
	    match++;
    if(cb_language->tag(i).contains(compare))
	    match+=2;
    if(cb_language->tag(i).left(compare.length()) == compare)
	    match+=10;
    if(cb_language->tag(i) == compare)
	    match+=100;
    if(compare == "en_US" && cb_language->tag(i) == "C")
	    match+=50;
    if(match > bestmatch) {
	    bestmatch=match;
	    best=i;
    }
  }
  cb_language->setCurrentItem(best);
  locale->setLanguage(cb_language->tag(best));

  // Highlight the users's country
  for(int i = 0; i < cb_country->count(); i++)
  {
    if(cb_country->tag(i).contains(country))
    {
	cb_country->setCurrentItem(i);
	locale->setCountry(cb_country->tag(i));
    }
  }

  languageSet = true;

}

KCountryPage::~KCountryPage(){
}


void KCountryPage::loadCountryList(KLanguageButton *combo)
{

  // temperary use of our locale as the global locale
  KLocale *lsave = KGlobal::_locale;
//  KGlobal::_locale = locale;

  QString sub = QString::fromLatin1("l10n/");

  // clear the list
  combo->clear();

  QStringList regionlist = KGlobal::dirs()->findAllResources("locale",
                                 sub + QString::fromLatin1("*.desktop"));
  regionlist.sort();

  for ( QStringList::ConstIterator it = regionlist.begin(); it != regionlist.end(); ++it )
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

void KCountryPage::loadLanguageList(KLanguageButton *combo, const QStringList &first)
{
  // temperary use of our locale as the global locale
  KLocale *lsave = KGlobal::_locale;
  //KGlobal::_locale = locale;

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
//      combo->insertSubmenu(locale->translate("Other"), submenu, QString::null, -2);
      combo->insertSubmenu(i18n("Other"), submenu, QString::null, -2);
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

/** No descriptions */
void KCountryPage::save(KLanguageButton *comboCountry, KLanguageButton *comboLang){
  kdDebug() << "KCountryPage::save()" << endl;
  KConfigBase *config = KGlobal::config();

  config->setGroup(QString::fromLatin1("Locale"));
  config->writeEntry(QString::fromLatin1("Country"), comboCountry->currentTag(), true, true);
  config->writeEntry(QString::fromLatin1("Language"), comboLang->currentTag(), true, true);
  config->sync();
   // Tell kdesktop about the new language
  QCString replyType; QByteArray replyData;
  QByteArray data, da;
  QDataStream stream( data, IO_WriteOnly );
  stream << comboLang->currentTag();
  if ( !kapp->dcopClient()->isAttached() )
    kapp->dcopClient()->attach();
  kdDebug() << "KLocaleConfig::save : sending signal to kdesktop" << endl;

  // inform kicker and kdeskop about the new language
  kapp->dcopClient()->send( "kicker", "Panel", "restart()", QString::null);
  // call, not send, so that we know it's done before coming back
  // (we both access kdeglobals...)
  kapp->dcopClient()->call( "kdesktop", "KDesktopIface", "languageChanged(QString)", data, replyType, replyData );
  // kdesktop has changed the path to the trash, so we need to reparse
  // to avoid overriding the changes
  config->reparseConfiguration();
}

void KCountryPage::setLangForCountry(int i)
{

  QString country = cb_country->tag(i);

  KSimpleConfig ent(locate("locale", QString::fromLatin1("l10n/") + country + QString::fromLatin1("/entry.desktop")), true);
  ent.setGroup(QString::fromLatin1("KCM Locale"));
  langs = ent.readListEntry(QString::fromLatin1("Languages"));

  QString lang = QString::fromLatin1("C");
  // use the first INSTALLED langauge in the list, or default to C
  for ( QStringList::Iterator it = langs.begin(); it != langs.end(); ++it )
    if (cb_language->containsTag(*it))
      {
    lang = *it;
    break;
      }

  locale->setLanguage(lang);
  locale->setCountry(country);

  cb_language->setCurrentItem(lang);
}

#include "kcountrypage.moc"
