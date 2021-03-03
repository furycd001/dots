/*
  toplevel.cpp - A KControl Application

  Copyright 1998 Matthias Hoelzer
  Copyright 1999-2000 Hans Petter Bieker <bieker@kde.org>

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

#include <qlabel.h>
#include <qvgroupbox.h>
#include <qobjectlist.h>
#include <qtabwidget.h>
#include <qevent.h>
#include <qwidgetintdict.h>
#include <qlayout.h>

#include <kcharsets.h>
#include <kconfig.h>
#include <kcmodule.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <klocale.h>
#include <kprocess.h>

#include "klocaleadv.h"
#include "locale.h"
#include "localenum.h"
#include "localemon.h"
#include "localetime.h"
#include "klocalesample.h"
#include "toplevel.h"
#include "toplevel.moc"

KLocaleApplication::KLocaleApplication(QWidget *parent, const char *name)
  : KCModule(parent, name)
{
  locale = new KLocaleAdvanced(QString::fromLatin1("kcmlocale"));
  QVBoxLayout *l = new QVBoxLayout(this, 5);
  l->setAutoAdd(TRUE);

  tab = new QTabWidget(this);

  localemain = new KLocaleConfig(locale, this);
  tab->addTab( localemain, QString::null);
  localenum = new KLocaleConfigNumber(locale, this);
  tab->addTab( localenum, QString::null);
  localemon = new KLocaleConfigMoney(locale, this);
  tab->addTab( localemon, QString::null);
  localetime = new KLocaleConfigTime(locale, this);
  tab->addTab( localetime, QString::null);

  // Examples
  gbox = new QVGroupBox(this);
  sample = new KLocaleSample(locale, gbox);

  load();
  update();

  connect(localemain, SIGNAL(resample()),                    SLOT(update()  ));
  connect(localenum,  SIGNAL(resample()),                    SLOT(update()  ));
  connect(localemon,  SIGNAL(resample()),                    SLOT(update()  ));
  connect(localetime, SIGNAL(resample()),                    SLOT(update()  ));
  connect(localemain, SIGNAL(languageChanged()), localenum,  SLOT(reset()   ));
  connect(localemain, SIGNAL(languageChanged()), localemon,  SLOT(reset()   ));
  connect(localemain, SIGNAL(languageChanged()), localetime, SLOT(reset()   ));
  connect(localemain, SIGNAL(countryChanged()),              SLOT(reset   ()));
  connect(localemain, SIGNAL(chsetChanged()),                SLOT(newChset()));
}

KLocaleApplication::~KLocaleApplication()
{
    delete locale;
}

void KLocaleApplication::load()
{
    localemain->load();
    localenum->load();
    localemon->load();
    localetime->load();
}

void KLocaleApplication::save()
{
    // temperary use of our locale as the global locale
    KLocale *lsave = KGlobal::_locale;
    KGlobal::_locale = locale;
    KMessageBox::information(this, locale->translate
                 ("Changed language settings apply only to "
                  "newly started applications.\nTo change the "
                  "language of all programs, you will have to "
                  "logout first."),
            locale->translate("Applying language settings"),
            QString::fromLatin1("LanguageChangesApplyOnlyToNewlyStartedPrograms"));
    // restore the old global locale
    KGlobal::_locale = lsave;

    KConfig *config = KGlobal::config();
    KConfigGroupSaver saver(config, QString::fromLatin1("Locale"));

    bool langChanged = config->readEntry(QString::fromLatin1("Language"))
           != locale->language();

    localemain->save();
    localenum->save();
    localemon->save();
    localetime->save();

    // rebuild the date base if language was changed
    if (langChanged)
    {
      KProcess proc;
      proc << QString::fromLatin1("kbuildsycoca");
      proc.start(KProcess::DontCare);
    }
}

void KLocaleApplication::defaults()
{
    localemain->defaults();
    localenum->defaults();
    localemon->defaults();
    localetime->defaults();
}

void KLocaleApplication::moduleChanged(bool state)
{
  emit changed(state);
}

QString KLocaleApplication::quickHelp() const
{
  return locale->translate("<h1>Locale</h1>\n"
          "<p>From here you can configure language, numeric, and time \n"
          "settings for your particular region. In most cases it will be \n"
          "sufficient to choose the country you live in. For instance KDE \n"
          "will automatically choose \"German\" as language if you choose \n"
          "\"Germany\" from the list. It will also change the time format \n"
          "to use 24 hours and and use comma as decimal separator.</p>\n"
          "<p><b>Note:</b> The default charset is ISO 8859-1. That \n"
          "charset is used when choosing fonts. You will have to change \n"
          "it if you are using a non-Western European language.\n");
}

void KLocaleApplication::updateSample()
{
    sample->update();
    moduleChanged(true);
}

void KLocaleApplication::reTranslate()
{
  // The untranslated string for QLabel are stored in
  // the name() so we use that when retranslating
  QObject *wc;
  QObjectList *list = queryList("QWidget");
  QObjectListIt it(*list);
  while ( (wc = it.current()) != 0 ) {
    ++it;

    // unnamed labels will cause errors and should not be
    // retranslated. E.g. the example box should not be
    // retranslated from here.
    if (strcmp(wc->name(), "unnamed") == 0) continue;

    if (strcmp(wc->className(), "QLabel") == 0)
      ((QLabel *)wc)->setText( locale->translate( wc->name() ) );
    else if (strcmp(wc->className(), "QGroupBox") == 0)
      ((QGroupBox *)wc)->setTitle( locale->translate( wc->name() ) );
  }
  delete list;

  // Here we have the pointer
  gbox->setCaption(locale->translate("Examples"));
  tab->changeTab(localemain, locale->translate("&Locale"));
  tab->changeTab(localenum, locale->translate("&Numbers"));
  tab->changeTab(localemon, locale->translate("&Money"));
  tab->changeTab(localetime, locale->translate("&Time && dates"));

  // retranslate some other widgets
  localemain->reTranslate();
  localenum->reTranslate();
  localemon->reTranslate();
  localetime->reTranslate();
  sample->update();

  // FIXME: All widgets are done now. However, there are
  // still some problems. Popup menus from the QLabels are
  // not retranslated.
}

void KLocaleApplication::reset()
{
  localenum->reset();
  localemon->reset();
  localetime->reset();
}

void KLocaleApplication::newChset()
{
  // figure out which font to use
  KConfig *c = KGlobal::config();
  c->setGroup( QString::fromLatin1("General") );

  QFont font(QString::fromLatin1("helvetica"), 12, QFont::Normal);
  KGlobal::charsets()->setQFont(font, locale->charset());
  font = c->readFontEntry(QString::fromLatin1("font"), &font);

  // set the font for all subwidgets
  QObjectList *list = queryList("QWidget");
  QObjectListIt it(*list);

  QObject *w;
  while ( (w = it.current()) != 0 ) {
    ++it;
    ((QWidget *)w)->setFont(font);
  }
  delete list;

  // FIXME: the context helps are still using the old charset

  moduleChanged(true);
}
