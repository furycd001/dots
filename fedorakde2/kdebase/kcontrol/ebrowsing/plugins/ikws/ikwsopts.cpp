/*
 * Copyright (c) 2000 Yves Arrouye <yves@realnames.com>
 * Copyright (c) 2001 Dawit Alemayehu <adawit@kde.org>
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

#include <assert.h>

#include <qvbox.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <qwhatsthis.h>
#include <qfile.h>

#include <dcopclient.h>

#include <kapp.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <ksimpleconfig.h>
#include <ktrader.h>
#include <kstddirs.h>
#include <kdebug.h>

#include "ikwsopts.h"
#include "kuriikwsfiltereng.h"
#include "searchprovider.h"
#include "searchproviderdlg.h"

#define ITEM_NONE   (i18n("None"))
#define searcher    KURISearchFilterEngine::self()

class SearchProviderItem : public QListViewItem
{
public:
    SearchProviderItem(QListView *parent, SearchProvider *provider)
        : QListViewItem(parent),
          m_provider(provider)
    {
        update();
    };
    virtual ~SearchProviderItem()
    {
        delete m_provider;
    }

    void update()
    {
        setText(0, m_provider->name());
        setText(1, m_provider->keys().join(","));
    }
    SearchProvider *provider() const { return m_provider; }

private:
    SearchProvider *m_provider;
};

InternetKeywordsOptions::InternetKeywordsOptions(QWidget *parent, const char *name)
                        :KCModule(parent, name)
{
    QVBoxLayout *lay = new QVBoxLayout( this, KDialog::marginHint(), KDialog::spacingHint() );
    lay->setAutoAdd( true );

    // Deals with Internet keywords feature...
    gb_keywords = new QGroupBox( this );
    QVBoxLayout *i_vbox = new QVBoxLayout( gb_keywords, KDialog::marginHint(), KDialog::spacingHint() );
    cb_enableInternetKeywords = new QCheckBox(i18n("Enable Int&ernet Keywords"), gb_keywords);
    connect(cb_enableInternetKeywords, SIGNAL(clicked()), this, SLOT(changeInternetKeywordsEnabled()));
    QWhatsThis::add(cb_enableInternetKeywords, i18n("If this box is checked, KDE will let you use <em>Internet Keywords</em> "
                                                    "in its browser's address bar. This means you can simply type normal "
                                                    "words and phrases, such as \"KDE\", to automatically get redirected "
                                                    "to the appropriate site. For further details on this feature visit "
                                                    "<b>http://www.internetkeywords.org</b> or simply type <b>IKW Dev</b> "
                                                    "in the browser location bar."));
    i_vbox->addWidget(cb_enableInternetKeywords);
    QHBoxLayout *igbopts_lay = new QHBoxLayout( i_vbox, KDialog::spacingHint() );
    lb_searchFallback = new QLabel(i18n("&Fallback Search Engine:"), gb_keywords);
    igbopts_lay->addWidget( lb_searchFallback );
    cmb_searchFallback = new QComboBox(false, gb_keywords);
    igbopts_lay->addWidget( cmb_searchFallback, 1 );
    lb_searchFallback->setBuddy(cmb_searchFallback);
    connect(cmb_searchFallback, SIGNAL(activated(const QString &)), this, SLOT(moduleChanged()));
    QString wtstr = i18n("Allows you to select a search provider that will be used in case what you "
                         "typed is not an <em>Internet Keyword</em>.  Select \"None\" if you do not "
                         "want to do a search in this case, and you will get a directory listing of "
                         "relevant keywords.");
    QWhatsThis::add(lb_searchFallback, wtstr);
    QWhatsThis::add(cmb_searchFallback, wtstr);

    // Deals with the web short cut features...
    gb_search = new QGroupBox( this );
    lay->setStretchFactor( gb_search, 10 );
    QGridLayout *w_grid = new QGridLayout( gb_search, 3, 2,
                                           KDialog::marginHint(),
                                           KDialog::spacingHint() );
    w_grid->setColStretch(0, 2);
    w_grid->setRowStretch(2, 2);

    cb_enableSearchKeywords = new QCheckBox(i18n("Enable &Web Shortcuts"), gb_search);
    connect(cb_enableSearchKeywords, SIGNAL(clicked()), this, SLOT(changeSearchKeywordsEnabled()));
    QWhatsThis::add(cb_enableSearchKeywords, i18n("If this box is checked, KDE will let you use the shortcuts "
                                                  "defined below to quickly search the Internet. For example, "
                                                  "typing the words <em>shortcut</em>:<em>KDE</em> will result in "
                                                  "the word <em>KDE</em> being searched using the URI defined by "
                                                  "the <em>shortcut</em>."));
    w_grid->addMultiCellWidget(cb_enableSearchKeywords, 0, 0, 0, 1);
    lv_searchProviders = new QListView( gb_search );
    lv_searchProviders->setMultiSelection(false);
    lv_searchProviders->addColumn(i18n("Name"));
    lv_searchProviders->addColumn(i18n("Shortcuts"));
    lv_searchProviders->setSorting(0);
    wtstr = i18n("This list contains the search providers that KDE knows "
                 "about, and their associated pseudo-URI schemes, or shortcuts.");
    QWhatsThis::add(lv_searchProviders, wtstr);

    connect(lv_searchProviders, SIGNAL(selectionChanged(QListViewItem *)),
           this, SLOT(updateSearchProvider()));
    connect(lv_searchProviders, SIGNAL(doubleClicked(QListViewItem *)),
           this, SLOT(changeSearchProvider()));

    w_grid->addMultiCellWidget(lv_searchProviders, 1, 2, 0, 0);

    QVBox* vbox = new QVBox( gb_search );
    vbox->setSpacing( KDialog::spacingHint() );
    pb_addSearchProvider = new QPushButton( i18n("Add..."), vbox );
    QWhatsThis::add(pb_addSearchProvider, i18n("Click here to add a search provider."));
    connect(pb_addSearchProvider, SIGNAL(clicked()), this, SLOT(addSearchProvider()));

    pb_chgSearchProvider = new QPushButton( i18n("Change..."), vbox );
    QWhatsThis::add(pb_chgSearchProvider, i18n("Click here to change a search provider."));
    pb_chgSearchProvider->setEnabled(false);
    connect(pb_chgSearchProvider, SIGNAL(clicked()), this, SLOT(changeSearchProvider()));

    pb_delSearchProvider = new QPushButton( i18n("Delete"), vbox );
    QWhatsThis::add(pb_delSearchProvider, i18n("Click here to delete the currently selected search provider from the list."));
    pb_delSearchProvider->setEnabled(false);
    connect(pb_delSearchProvider, SIGNAL(clicked()), this, SLOT(deleteSearchProvider()));

    /*  //lukas: hide these as it's not implemented
    pb_impSearchProvider = new QPushButton( i18n("Import..."), vbox );
    QWhatsThis::add(pb_delSearchProvider, i18n("Click here to import a search provider from a file."));
    connect(pb_impSearchProvider, SIGNAL(clicked()), this, SLOT(importSearchProvider()));

    pb_expSearchProvider = new QPushButton(i18n("Export..."), vbox );
    QWhatsThis::add(pb_expSearchProvider, i18n("Click here to export a search provider to a file."));
    pb_expSearchProvider->setEnabled(false);
    connect(pb_expSearchProvider, SIGNAL(clicked()), this, SLOT(exportSearchProvider()));
    */

    w_grid->addWidget(vbox, 1, 1);
    // Load the options
    load();
}

QString InternetKeywordsOptions::quickHelp() const
{
    return i18n("In this module, you can choose whether to use Internet Keywords or not. "
                "You can also configure various Internet search engines "
                "that KDE can use for keyword search. This allows you, for example, "
                "to enter a pseudo-URI like gg:smetana to search the Google search "
                "engine for web pages about the Czech composer Bedrich Smetana.");
}

void InternetKeywordsOptions::load()
{
    // Clear state first.
    lv_searchProviders->clear();
    cmb_searchFallback->clear();
    cmb_searchFallback->insertItem(ITEM_NONE);

    KConfig config( searcher->name() + "rc", false, false );
    config.setGroup("General");

    QString searchFallback = config.readEntry("InternetKeywordsSearchFallback");
    const KTrader::OfferList services = KTrader::self()->query("SearchProvider");
    for (KTrader::OfferList::ConstIterator it = services.begin(); it != services.end(); ++it)
    {
        displaySearchProvider(new SearchProvider(*it),
            (*it)->desktopEntryName() == searchFallback);
    }

    // Enable/Disable widgets accordingly.
    bool ikwsEnabled = config.readBoolEntry("InternetKeywordsEnabled", true);
    cb_enableInternetKeywords->setChecked( ikwsEnabled );
    cmb_searchFallback->setEnabled( ikwsEnabled );
    lb_searchFallback->setEnabled( ikwsEnabled );

    bool searchEnabled = config.readBoolEntry("SearchEngineShortcutsEnabled", true);
    cb_enableSearchKeywords->setChecked( searchEnabled );
    changeSearchKeywordsEnabled();

    if (lv_searchProviders->childCount())
      lv_searchProviders->setSelected(lv_searchProviders->firstChild(), true);
}

void InternetKeywordsOptions::save()
{
    KConfig config( searcher->name() + "rc", false, false );
    config.setGroup("General");
    config.writeEntry("InternetKeywordsEnabled", cb_enableInternetKeywords->isChecked());
    config.writeEntry("SearchEngineShortcutsEnabled", cb_enableSearchKeywords->isChecked());
    QString fallback = cmb_searchFallback->currentText();
    if (fallback == ITEM_NONE)
        config.writeEntry("InternetKeywordsSearchFallback", QString::null);

    QString path = kapp->dirs()->saveLocation("services", "searchproviders/");
    for (QListViewItemIterator it(lv_searchProviders); it.current(); ++it)
    {
        SearchProviderItem *item = dynamic_cast<SearchProviderItem *>(it.current());
        assert(item);
        SearchProvider *provider = item->provider();
        QString name = provider->desktopEntryName();
        if (provider->isDirty())
        {
            if (name.isEmpty())
            {
                // New provider
                // Take the longest search shortcut as filename,
                // if such a file already exists, append a number and increase it
                // until the name is unique
                for (QStringList::ConstIterator it = provider->keys().begin(); it != provider->keys().end(); ++it)
                {
                    if ((*it).length() > name.length())
                        name = (*it).lower();
                }
                for (int suffix = 0; ; ++suffix)
                {
                    QString located, check = name;
                    if (suffix)
                        check += QString().setNum(suffix);
                    if ((located = locate("services", "searchproviders/" + check + ".desktop")).isEmpty())
                    {
                        name = check;
                        break;
                    }
                    else if (located.left(path.length()) == path)
                    {
                        // If it's a deleted (hidden) entry, overwrite it
                        if (KService(located).isDeleted())
                            break;
                    }
                }
            }
            KSimpleConfig service(path + name + ".desktop");
            service.setGroup("Desktop Entry");
            service.writeEntry("Type", "Service");
            service.writeEntry("ServiceTypes", "SearchProvider");
            service.writeEntry("Name", provider->name());
            service.writeEntry("Query", provider->query());
            service.writeEntry("Keys", provider->keys());
            service.writeEntry("Charset", provider->charset());
            // we might be overwriting a hidden entry
            service.writeEntry("Hidden", false);
        }
        if (fallback == provider->name())
            config.writeEntry("InternetKeywordsSearchFallback", name);
    }
    for (QStringList::ConstIterator it = m_deletedProviders.begin(); it != m_deletedProviders.end(); ++it)
    {
        QStringList matches = kapp->dirs()->findAllResources("services", "searchproviders/" + *it + ".desktop");
        // Shouldn't happen
        if (!matches.count())
            continue;

        if (matches.count() == 1 && matches[0].left(path.length()) == path)
        {
            // If only the local copy existed, unlink it
            // TODO: error handling
            QFile::remove(matches[0]);
            continue;
        }
        KSimpleConfig service(path + *it + ".desktop");
        service.setGroup("Desktop Entry");
        service.writeEntry("Type", "Service");
        service.writeEntry("ServiceTypes", "SearchProvider");
        service.writeEntry("Hidden", true);
    }
    config.sync();

    QByteArray data;
    kapp->dcopClient()->send("*", "KURIIKWSFilterIface", "configure()", data);
    kapp->dcopClient()->send("*", "KURISearchFilterIface", "configure()", data);
    kapp->dcopClient()->send( "kded", "kbuildsycoca", "recreate()", data);
}

void InternetKeywordsOptions::defaults()
{
    load();
}

void InternetKeywordsOptions::moduleChanged()
{
    // Removed the bool parameter, this way this can be directly connected
    // as it was alwayw called with true as argument anyway (malte)
    emit changed(true);
}

void InternetKeywordsOptions::changeInternetKeywordsEnabled()
{
    bool use_keywords = cb_enableInternetKeywords->isChecked();
    cmb_searchFallback->setEnabled(use_keywords);
    lb_searchFallback->setEnabled(use_keywords);
    moduleChanged();
}

void InternetKeywordsOptions::changeSearchKeywordsEnabled()
{
    bool use_keywords = cb_enableSearchKeywords->isChecked();
    lv_searchProviders->setEnabled(use_keywords);
    pb_addSearchProvider->setEnabled(use_keywords);
    pb_chgSearchProvider->setEnabled(use_keywords);
    pb_delSearchProvider->setEnabled(use_keywords);
    //pb_impSearchProvider->setEnabled(use_keywords);
    //pb_expSearchProvider->setEnabled(use_keywords);
    moduleChanged();
}

void InternetKeywordsOptions::addSearchProvider()
{
    SearchProviderDialog dlg(0, this);
    if (dlg.exec())
    {
        lv_searchProviders->setSelected(displaySearchProvider(dlg.provider()), true);
        moduleChanged();
    }
}

void InternetKeywordsOptions::changeSearchProvider()
{
    SearchProviderItem *item = dynamic_cast<SearchProviderItem *>(lv_searchProviders->currentItem());
    assert(item);
    SearchProviderDialog dlg(item->provider(), this);
    if (dlg.exec())
    {
        lv_searchProviders->setSelected(displaySearchProvider(dlg.provider()), true);
        moduleChanged();
    }
}

void InternetKeywordsOptions::deleteSearchProvider()
{
    SearchProviderItem *item = dynamic_cast<SearchProviderItem *>(lv_searchProviders->currentItem());
    assert(item);
    // Update the combo box to go to None if the fallback was deleted.
    int current = cmb_searchFallback->currentItem();
    for (int i = 1, count = cmb_searchFallback->count(); i < count; ++i)
    {
      if (cmb_searchFallback->text(i) == item->provider()->name())
      {
        cmb_searchFallback->removeItem(i);
        if (i == current)
          cmb_searchFallback->setCurrentItem(0);
        else if (current > i)
          cmb_searchFallback->setCurrentItem(current - 1);

        break;
      }
    }

    if (item->nextSibling())
        lv_searchProviders->setSelected(item->nextSibling(), true);
    else if (item->itemAbove())
        lv_searchProviders->setSelected(item->itemAbove(), true);

    if (!item->provider()->desktopEntryName().isEmpty())
        m_deletedProviders.append(item->provider()->desktopEntryName());
    delete item;
    updateSearchProvider();
    moduleChanged();
}

void InternetKeywordsOptions::importSearchProvider()
{
    KMessageBox::sorry(this, i18n("Importing Search Providers is not implemented yet."));
}

void InternetKeywordsOptions::exportSearchProvider()
{
    KMessageBox::sorry(this, i18n("Exporting Search Providers is not implemented yet."));
}

void InternetKeywordsOptions::updateSearchProvider()
{
    pb_chgSearchProvider->setEnabled(lv_searchProviders->currentItem());
    pb_delSearchProvider->setEnabled(lv_searchProviders->currentItem());
    //pb_expSearchProvider->setEnabled(lv_searchProviders->currentItem());
}

SearchProviderItem *InternetKeywordsOptions::displaySearchProvider(SearchProvider *p, bool fallback)
{

    // Show the provider in the list.
    SearchProviderItem *item = 0L;

    QListViewItemIterator it(lv_searchProviders);
    for (; it.current(); ++it)
    {
      if (it.current()->text(0) == p->name())
      {
        item = dynamic_cast<SearchProviderItem *>(it.current());
        assert(item);
        break;
      }
    }

    if (!item)
    {
      item = new SearchProviderItem(lv_searchProviders, p);
      // Put the name in the combo box.
      int i, count = cmb_searchFallback->count();
      for (i = 1; i < count; ++i)
      {
        if (cmb_searchFallback->text(i) > p->name())
        {
          int current = cmb_searchFallback->currentItem();
          cmb_searchFallback->insertItem(p->name(), i);
          if (current >= i)
            cmb_searchFallback->setCurrentItem(current + 1);
          break;
        }
      }
      if (i == count)
        cmb_searchFallback->insertItem(p->name());

      if (fallback)
        cmb_searchFallback->setCurrentItem(i);
    }
    else
        item->update();

    if (!it.current())
        lv_searchProviders->sort();

    return item;
}

#include "ikwsopts.moc"
