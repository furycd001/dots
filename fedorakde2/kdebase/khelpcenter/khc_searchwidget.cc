/*
 *  khc_searchwidget.cpp - part of the KDE Help Center
 *
 *  Copyright (C) 1999 Matthias Elter (me@kde.org)
 *            (C) 2000 Matthias Hoelzer-Kluepfel (hoelzer@kde.org)
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


#include <stdlib.h>


#include <qlabel.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qframe.h>
#include <klanguagebutton.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <ksimpleconfig.h>
#include <qwhatsthis.h>


#include <klocale.h>


#include "htmlsearch/htmlsearch.h"
#include "khc_searchwidget.moc"


SearchWidget::SearchWidget(QWidget *parent)
  : QWidget(parent)
{
  QVBoxLayout *vbox = new QVBoxLayout(this, 2,2);

  searchString = new QLineEdit(this);
  keyWordLabel = new QLabel(searchString, i18n("&Keywords:"), this);
  connect(searchString, SIGNAL(returnPressed()), this, SLOT(slotSearch()));
  vbox->addWidget(keyWordLabel);
  vbox->addWidget(searchString);

  searchButton = new QPushButton(i18n("&Search"), this);
  searchButton->setFixedSize(searchButton->sizeHint());
  vbox->addWidget(searchButton, 0, Qt::AlignRight);

  QFrame *f = new QFrame(this);
  f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
  vbox->addWidget(f);

  QLabel *l = new QLabel(i18n("Advanced options:"), this);
  vbox->addWidget(l);
  vbox->addSpacing(4);

  QHBoxLayout *hbox = new QHBoxLayout(vbox);

  language = new KLanguageButton(this);
  QWhatsThis::add(language, i18n("Here you can select the language you want to search in."));
  l = new QLabel(language, i18n("&Language"), this);
  hbox->addWidget(l);
  hbox->addWidget(language,1);

  loadLanguages();

  language->setCurrentItem(KGlobal::locale()->language());

  hbox = new QHBoxLayout(vbox);

  method = new QComboBox(this);
  method->insertItem(i18n("and"));
  method->insertItem(i18n("or"));
  method->insertItem(i18n("boolean"));
  
  l = new QLabel(method, i18n("Method"), this);
  hbox->addWidget(l);
  hbox->addWidget(method);

  hbox = new QHBoxLayout(vbox);

  pages = new QComboBox(this);
  pages->insertItem(i18n("10"));
  pages->insertItem(i18n("25"));
  pages->insertItem(i18n("1000"));
  
  l = new QLabel(pages, i18n("Max. &results"), this);
  hbox->addWidget(l);
  hbox->addWidget(pages);

  hbox = new QHBoxLayout(vbox);

  format = new QComboBox(this);
  format->insertItem(i18n("Long"));
  format->insertItem(i18n("Short"));
  
  l = new QLabel(format, i18n("&Format"), this);
  hbox->addWidget(l);
  hbox->addWidget(format);

  hbox = new QHBoxLayout(vbox);

  sort = new QComboBox(this);
  sort->insertItem(i18n("Score"));
  sort->insertItem(i18n("Title"));
  sort->insertItem(i18n("Date"));
  
  l = new QLabel(sort, i18n("&Sort"), this);
  hbox->addWidget(l);
  hbox->addWidget(sort);

  revSort = new QCheckBox(i18n("Reverse &order"), this);
  vbox->addWidget(revSort, 0, Qt::AlignRight);

  vbox->addStretch(1);

  indexButton = new QPushButton(i18n("&Update index"), this);
  indexButton->setFixedSize(indexButton->sizeHint());
  vbox->addWidget(indexButton, 0, Qt::AlignLeft);
 
  connect(searchButton, SIGNAL(clicked()), this, SLOT(slotSearch()));
  connect(indexButton, SIGNAL(clicked()), this, SLOT(slotIndex()));

  search = new HTMLSearch();
}


SearchWidget::~SearchWidget()
{
  delete search;
}


void SearchWidget::loadLanguages()
{
  // clear the list
  language->clear();
 
  // add all languages to the list
  QStringList langs = KGlobal::dirs()->findAllResources("locale",
							QString::fromLatin1("*/entry.desktop"));
  langs.sort();

  for (QStringList::ConstIterator it = langs.begin(); it != langs.end(); ++it)
    {
      KSimpleConfig entry(*it);
      entry.setGroup(QString::fromLatin1("KCM Locale"));
      QString name = entry.readEntry(QString::fromLatin1("Name"), KGlobal::locale()->translate("without name"));
      
      QString path = *it;
      int index = path.findRev('/');
      path = path.left(index);
      index = path.findRev('/');
      path = path.mid(index+1);
      language->insertLanguage(path, name);      
    }
}


void SearchWidget::slotSearch()
{
  QString words = searchString->text();
  QString m = "and";
  if (method->currentItem() == 1)
    m = "or";
  if (method->currentItem() == 2)
    m = "boolean";
  int p = 10;
  if (pages->currentItem() == 1)
    p = 25;
  if (pages->currentItem() == 2)
    p = 1000;
  QString f = "builtin-long";
  if (format->currentItem() == 1)
    f = "builtin-short";
  QString so = "score";
  if (sort->currentItem() == 1)
    so = "title";
  if (sort->currentItem() == 2)
    so = "date";
  if (revSort->isChecked())
    so = QString("rev%1").arg(so);
  
  QString url = search->search(language->currentTag(), words, m, p, f, so);
 
  if (!url.isEmpty())
    emit searchResult(url);
}


void SearchWidget::slotIndex()
{
  // TODO: fix this ugly system call
  system("kcmshell Help/htmlsearch &");
}
