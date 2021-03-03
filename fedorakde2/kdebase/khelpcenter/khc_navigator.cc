/*
 *  khc_navigator.cc - part of the KDE Help Center
 *
 *  Copyright (C) 1999 Matthias Elter (me@kde.org)
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

#include "khc_navigator.h"
#include "khc_navigatoritem.h"
// #include "khc_searchwidget.h"
#include "khc_factory.h"

#include <qdir.h>
#include <qfile.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qlabel.h>
#include <qtabbar.h>
#include <qheader.h>
#include <qdom.h>
#include <qtextstream.h>

#include <kaction.h>
#include <kapp.h>
#include <ksimpleconfig.h>
#include <kstddirs.h>
#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
#include <klistview.h>
#include <kservicegroup.h>
#include <ksycocaentry.h>
#include <kservice.h>
#include <kservicegroup.h>
#include <kmessagebox.h>
#include <kiconloader.h>
#include <kprocio.h>

template class QList<khcNavigatorItem>;

SectionItem::SectionItem(QListViewItem *parent, const QString &text)
	: QListViewItem(parent, text)
{
  setOpen(false);
}

void SectionItem::setOpen(bool open)
{
  if (open)
    setPixmap(0, KGlobal::iconLoader()->loadIcon(QString::fromLatin1("contents"), KIcon::Small));
  else
    setPixmap(0, KGlobal::iconLoader()->loadIcon(QString::fromLatin1("contents2"), KIcon::Small));

  QListViewItem::setOpen(open);
}

khcNavigator::khcNavigator(QWidget *parentWidget, QObject *parent,
                           const char *name)
    : KParts::ReadOnlyPart(parent,name)
{
    kdDebug(1400) << "khcNavigator::khcNavigator\n";
    setInstance( KHCFactory::instance() );

    setWidget( new khcNavigatorWidget( parentWidget ) );

    m_extension = new khcNavigatorExtension( this, "khcNavigatorExtension" );
    connect( widget(), SIGNAL( itemSelected(const QString&) ),
             m_extension, SLOT( slotItemSelected(const QString&) ) );
}

bool khcNavigator::openURL( const KURL & )
{
  emit started( 0 );
  emit completed();
  return true;
}

bool khcNavigator::openFile()
{
  return true; // easy one...
}

khcNavigator::~khcNavigator()
{
  // KParts deletes the widget. Cool.
}

void khcNavigatorExtension::slotItemSelected(const QString& url)
{
    KParts::URLArgs urlArgs(true, 0, 0);

    kdDebug(1400) << "request URL " << url << endl;

    emit openURLRequest( url, urlArgs );
}

khcNavigatorWidget::khcNavigatorWidget(QWidget *parent, const char *name)
   : QTabWidget(parent, name)
{
    KConfig *config = kapp->config();
    config->setGroup("ScrollKeeper");
    mScrollKeeperShowEmptyDirs = config->readBoolEntry("ShowEmptyDirs",false);

    setupContentsTab();
    // setupSearchTab();
    setupGlossaryTab();

    buildTree();
}

khcNavigatorWidget::~khcNavigatorWidget()
{
}

void khcNavigatorWidget::setupContentsTab()
{
    contentsTree = new KListView(this);
    contentsTree->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    contentsTree->addColumn(QString::null);
    contentsTree->setAllColumnsShowFocus(true);
    contentsTree->header()->hide();
    contentsTree->setRootIsDecorated(false);
    contentsTree->setSorting(-1, false);
    connect(contentsTree, SIGNAL(executed(QListViewItem*)), this,
	    SLOT(slotItemSelected(QListViewItem*)));

    addTab(contentsTree, i18n("Contents"));
}

void khcNavigatorWidget::setupSearchTab()
{
    /* search = new SearchWidget(this);
    connect(search, SIGNAL(searchResult(QString)),this,
	    SLOT(slotURLSelected(QString)));

    addTab(search, i18n("Search"));
 */
}

void khcNavigatorWidget::setupGlossaryTab()
{
    glossaryTree = new KListView(this);
    glossaryTree->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    glossaryTree->addColumn(QString::null);
    glossaryTree->header()->hide();
    glossaryTree->setAllColumnsShowFocus(true);
    glossaryTree->setRootIsDecorated(true);
    connect(glossaryTree, SIGNAL(executed(QListViewItem *)),
        SLOT(slotGlossaryItemSelected(QListViewItem *)));

    byTopicItem = new QListViewItem(glossaryTree, i18n("By topic"));
    byTopicItem->setPixmap(0, KGlobal::iconLoader()->loadIcon(QString::fromLatin1("help"), KIcon::Small));

    alphabItem = new QListViewItem(glossaryTree, i18n("Alphabetically"));
    alphabItem->setPixmap(0, KGlobal::iconLoader()->loadIcon(QString::fromLatin1("charset"), KIcon::Small));

    addTab(glossaryTree, i18n("Glossary"));

    meinproc = new KProcess();
    connect(meinproc, SIGNAL(receivedStdout(KProcess *, char *, int)),
        SLOT(gotMeinprocOutput(KProcess *, char *, int)));
    connect(meinproc, SIGNAL(processExited(KProcess *)),
        SLOT(meinprocExited(KProcess *)));

    *meinproc << locate("exe", QString::fromLatin1("meinproc"));
    *meinproc << QString::fromLatin1("--stdout");
    *meinproc << langLookup(QString::fromLatin1("khelpcenter/glossary/index.docbook"));

    meinproc->start(KProcess::NotifyOnExit, KProcess::Stdout);
}

void khcNavigatorWidget::gotMeinprocOutput(KProcess *, char *data, int len)
{
  htmlData += QString::fromLatin1(data, len);
}

void khcNavigatorWidget::meinprocExited(KProcess *)
{
  delete meinproc;

  QDomDocument doc;

  if (!doc.setContent(htmlData))
    return;

  QDomNodeList glossDivNodes = doc.documentElement().elementsByTagName(QString::fromLatin1("div"));
  for (unsigned int i = 0; i < glossDivNodes.count(); i++) {
    QDomNode glossDivNode = glossDivNodes.item(i);
    if (glossDivNode.toElement().attribute(QString::fromLatin1("class"), QString::null) != QString::fromLatin1("glossdiv"))
      continue;

    QString glossDiv = glossDivNode.namedItem(QString::fromLatin1("h3")).toElement().text().simplifyWhiteSpace();
    SectionItem *topicSection = new SectionItem(byTopicItem, glossDiv);

    QDomNodeList glossEntryNodes = glossDivNode.toElement().elementsByTagName(QString::fromLatin1("dt"));
    for (unsigned int j = 0; j < glossEntryNodes.count(); j++) {
      QDomNode glossEntryNode = glossEntryNodes.item(j);
      QString term = glossEntryNode.namedItem(QString::fromLatin1("a")).toElement().text().simplifyWhiteSpace();

      (void) new QListViewItem(topicSection, term);

      SectionItem *alphabSection = 0L;
      for (QListViewItemIterator it(alphabItem); it.current(); it++)
        if (it.current()->text(0) == term[0].upper()) {
          alphabSection = static_cast<SectionItem *>(it.current());
          break;
        }

      if (!alphabSection)
        alphabSection = new SectionItem(alphabItem, term[0].upper());

      (void) new QListViewItem(alphabSection, term);

      glossEntryNode = glossEntryNode.nextSibling();

      QString definition;
      QTextStream defStream(&definition, IO_WriteOnly);
      defStream << glossEntryNode.namedItem(QString::fromLatin1("p")).toElement();

      QStringList seeAlso;
      QDomNodeList seeAlsoNodes = glossEntryNode.toElement().elementsByTagName(QString::fromLatin1("p"));
      // Skip first <p> element as it contained the definition.
      for (unsigned int k = 1; k < seeAlsoNodes.count(); k++)
        seeAlso += seeAlsoNodes.item(k).namedItem(QString::fromLatin1("a")).toElement().text().simplifyWhiteSpace();

      glossEntries.insert(term, new GlossaryEntry(term, definition, seeAlso));
    }
  }
}

void khcNavigatorWidget::buildTree()
{
  // supporting KDE
  khcNavigatorItem *ti_support = new khcNavigatorItem(contentsTree, i18n("Supporting KDE"),"document2");
  ti_support->setURL(QString("help:/khelpcenter/index.html?anchor=support"));
  staticItems.append(ti_support);

  // kde contacts
  khcNavigatorItem *ti_contact = new khcNavigatorItem(contentsTree, i18n("Contact Information"),"document2");
  ti_contact->setURL(QString("help:/khelpcenter/index.html?anchor=contact"));
  staticItems.append(ti_contact);

  // kde links
  khcNavigatorItem *ti_links = new khcNavigatorItem(contentsTree, i18n("KDE on the web"),"document2");
  ti_links->setURL(QString("help:/khelpcenter/index.html?anchor=links"));
  staticItems.append(ti_links);

  // KDE FAQ
  khcNavigatorItem *ti_faq = new khcNavigatorItem(contentsTree, i18n("The KDE FAQ"),"document2");
  ti_faq->setURL(QString("help:/khelpcenter/faq/index.html"));
  staticItems.append(ti_faq);

  // scan plugin dir for plugins
  insertPlugins();

  insertScrollKeeperItems();

  // info browser
  khcNavigatorItem *ti_info = new khcNavigatorItem(contentsTree, i18n("Browse info pages"),"document2");
  ti_info->setURL(QString("info:/dir"));
  staticItems.append(ti_info);

  // unix man pages
  khcNavigatorItem *ti_man = new khcNavigatorItem(contentsTree, i18n("Unix manual pages"),"document2");
  ti_man->setURL(QString("man:/(index)"));
  staticItems.append(ti_man);

  // fill the man pages subcontentsTree
  buildManSubTree(ti_man);

  // application manuals
  khcNavigatorItem *ti_manual = new khcNavigatorItem(contentsTree, i18n("Application manuals"),"contents2");
  ti_manual->setURL("");
  staticItems.append(ti_manual);

  // fill the application manual subtree
  buildManualSubTree(ti_manual, "");

  // KDE user's manual
  khcNavigatorItem *ti_um = new khcNavigatorItem(contentsTree, i18n("KDE user's manual"),"document2");
  ti_um->setURL(QString("help:/khelpcenter/userguide/index.html"));
  staticItems.append(ti_um);

  // KDE quickstart guide
  khcNavigatorItem *ti_qs = new khcNavigatorItem(contentsTree, i18n("Introduction to KDE"),"document2");
  ti_qs->setURL(QString("help:/khelpcenter/quickstart/index.html"));
  staticItems.append(ti_qs);

  // introduction page
  khcNavigatorItem *ti_intro = new khcNavigatorItem(contentsTree, i18n("Introduction"),"document2");
  ti_intro->setURL(QString("help:/khelpcenter/index.html?anchor=welcome"));
  staticItems.append(ti_intro);

  contentsTree->setCurrentItem(ti_intro);
}

void khcNavigatorWidget::clearTree()
{
    contentsTree->clear();

    while(!staticItems.isEmpty())
	staticItems.removeFirst();

    while(!manualItems.isEmpty())
	manualItems.removeFirst();

    while(!pluginItems.isEmpty())
	pluginItems.removeFirst();

    while(!scrollKeeperItems.isEmpty())
	scrollKeeperItems.removeFirst();
}

void khcNavigatorWidget::buildManSubTree(khcNavigatorItem *parent)
{
  // man(n)
  khcNavigatorItem *ti_man_sn = new khcNavigatorItem(parent, i18n("(n) New"),"document2");
  ti_man_sn->setURL(QString("man:/(n)"));
  staticItems.append(ti_man_sn);

  // man(9)
  khcNavigatorItem *ti_man_s9 = new khcNavigatorItem(parent, i18n("(9) Kernel"),"document2");
  ti_man_s9->setURL(QString("man:/(9)"));
  staticItems.append(ti_man_s9);

  // man(8)
  khcNavigatorItem *ti_man_s8 = new khcNavigatorItem(parent, i18n("(8) Sys. Administration"),"document2");
  ti_man_s8->setURL(QString("man:/(8)"));
  staticItems.append(ti_man_s8);

  // man(7)
  khcNavigatorItem *ti_man_s7 = new khcNavigatorItem(parent, i18n("(7) Miscellaneous"),"document2");
  ti_man_s7->setURL(QString("man:/(7)"));
  staticItems.append(ti_man_s7);

  // man(6)
  khcNavigatorItem *ti_man_s6 = new khcNavigatorItem(parent, i18n("(6) Games"),"document2");
  ti_man_s6->setURL(QString("man:/(6)"));
  staticItems.append(ti_man_s6);

  // man(5)
  khcNavigatorItem *ti_man_s5 = new khcNavigatorItem(parent, i18n("(5) File Formats"),"document2");
  ti_man_s5->setURL(QString("man:/(5)"));
  staticItems.append(ti_man_s5);

  // man(4)
  khcNavigatorItem *ti_man_s4 = new khcNavigatorItem(parent, i18n("(4) Devices"),"document2");
  ti_man_s4->setURL(QString("man:/(4)"));
  staticItems.append(ti_man_s4);

  // man(3)
  khcNavigatorItem *ti_man_s3 = new khcNavigatorItem(parent, i18n("(3) Subroutines"),"document2");
  ti_man_s3->setURL(QString("man:/(3)"));
  staticItems.append(ti_man_s3);

  // man(2)
  khcNavigatorItem *ti_man_s2 = new khcNavigatorItem(parent, i18n("(2) System calls"),"document2");
  ti_man_s2->setURL(QString("man:/(2)"));
  staticItems.append(ti_man_s2);
  // man (1)
  khcNavigatorItem *ti_man_s1 = new khcNavigatorItem(parent, i18n("(1) User commands"),"document2");
  ti_man_s1->setURL(QString("man:/(1)"));
  staticItems.append(ti_man_s1);
}

void khcNavigatorWidget::buildManualSubTree(khcNavigatorItem *parent, QString relPath)
{
  KServiceGroup::Ptr root = KServiceGroup::group(relPath);
  KServiceGroup::List list = root->entries();


  for (KServiceGroup::List::ConstIterator it = list.begin(); it != list.end(); ++it)
    {
      KSycocaEntry * e = *it;
      KService::Ptr s;
      khcNavigatorItem *item;
      KServiceGroup::Ptr g;
      QString url;

      switch (e->sycocaType())
	{
	case KST_KService:
	  s = static_cast<KService*>(e);
	  url = documentationURL(s);
	  if (!url.isEmpty())
	    {
	      item = new khcNavigatorItem(parent, s->name(), s->icon());
	      item->setURL(url);
	      staticItems.append(item);
	    }
	  break;

	case KST_KServiceGroup:
	  g = static_cast<KServiceGroup*>(e);
          if ( g->caption().contains( ".hidden" ) )
              continue;
	  item = new khcNavigatorItem(parent, g->caption(), g->icon());
	  item->setURL("");
	  buildManualSubTree(item, g->relPath());
	  if (item->childCount() > 0)
	    staticItems.append(item);
	  else
	    delete item;
	  break;

	default:
	  break;
	}
    }
}

// derive a valid URL to the documentation
QString khcNavigatorWidget::documentationURL(KService *s)
{
  // if entry contains a DocPath, process it
  QString docPath = s->property("DocPath").toString();
  if (!docPath.isEmpty())
    {
      // see if it is part of our help system, or external
      // note that this test might be a bit too stupid
      if (docPath.left(5) == "file:" || docPath.left(5) == "http:")
          return docPath;

      return QString( "help:/" ) + docPath;
    }
  else
    return QString::null;

}

void khcNavigatorWidget::insertPlugins()
{
    // Scan plugin dir
    KStandardDirs* kstd = KGlobal::dirs();
    kstd->addResourceType("data", "share/apps/khelpcenter");
    QStringList list = kstd->findDirs("data", "plugins");
    for(QStringList::Iterator it=list.begin(); it!=list.end(); it++) {
      processDir(*it, 0, &pluginItems);
      appendEntries(*it, 0, &pluginItems);
    }
}

void khcNavigatorWidget::insertScrollKeeperItems()
{
    KProcIO proc;
    proc << "scrollkeeper-get-content-list";
    proc << KGlobal::locale()->language();
    connect(&proc,SIGNAL(readReady(KProcIO *)),SLOT(getScrollKeeperContentsList(KProcIO *)));
    if (!proc.start(KProcess::Block)) {
      kdDebug(1400) << "Could not execute scrollkeeper-get-content-list" << endl;
      return;
    }

    if (!QFile::exists(mScrollKeeperContentsList)) {
      kdDebug(1400) << "Scrollkeeper contents file '" << mScrollKeeperContentsList
                << "' does not exist." << endl;
      return;
    }

    QDomDocument doc("ScrollKeeperContentsList");
    QFile f(mScrollKeeperContentsList);
    if ( !f.open( IO_ReadOnly ) )
        return;
    if ( !doc.setContent( &f ) ) {
        f.close();
        return;
    }
    f.close();

    // Create top-level item
    khcNavigatorItem *topItem = new khcNavigatorItem(contentsTree, i18n("Scrollkeeper"),"contents2");
    topItem->setURL("");
    scrollKeeperItems.append(topItem);

    QDomElement docElem = doc.documentElement();

    QDomNode n = docElem.firstChild();
    while( !n.isNull() ) {
        QDomElement e = n.toElement();
        if( !e.isNull() ) {
            if (e.tagName() == "sect") {
              insertScrollKeeperSection(topItem,e);
            }
        }
        n = n.nextSibling();
    }
}

void khcNavigatorWidget::getScrollKeeperContentsList(KProcIO *proc)
{
    QString filename;
    proc->readln(filename,true);

    mScrollKeeperContentsList = filename;
}

int khcNavigatorWidget::insertScrollKeeperSection(khcNavigatorItem *parentItem,QDomNode sectNode)
{
    khcNavigatorItem *sectItem = new khcNavigatorItem(parentItem,"","contents2");
    sectItem->setURL("");
    scrollKeeperItems.append(sectItem);

    int numDocs = 0;  // Number of docs created in this section

    QDomNode n = sectNode.firstChild();
    while( !n.isNull() ) {
        QDomElement e = n.toElement();
        if( !e.isNull() ) {
            if (e.tagName() == "title") {
                sectItem->setText(0,e.text());
            } else if (e.tagName() == "sect") {
                numDocs += insertScrollKeeperSection(sectItem,e);
            } else if (e.tagName() == "doc") {
                insertScrollKeeperDoc(sectItem,e);
                ++numDocs;
            }
        }
        n = n.nextSibling();
    }

    // Remove empty sections
    if (!mScrollKeeperShowEmptyDirs && numDocs == 0) delete sectItem;

    return numDocs;
}

void khcNavigatorWidget::insertScrollKeeperDoc(khcNavigatorItem *parentItem,QDomNode docNode)
{
    khcNavigatorItem *docItem = new khcNavigatorItem(parentItem,"","document2");
    scrollKeeperItems.append(docItem);

    QString url;

    QDomNode n = docNode.firstChild();
    while( !n.isNull() ) {
        QDomElement e = n.toElement();
        if( !e.isNull() ) {
            if (e.tagName() == "doctitle") {
                docItem->setText(0,e.text());
            } else if (e.tagName() == "docsource") {
                url.append(e.text());
            } else if (e.tagName() == "docformat") {
                QString mimeType = e.text();
                if (mimeType == "text/html") {
                    // Let the HTML part figure out how to get the doc
                } else if (mimeType == "text/xml") {
                    // Should probably check for the DTD here
                    url.prepend("help:");
                } else if (mimeType == "text/sgml") {
                    // GNOME docs use this type. We don't have a real viewer for this.
                    url.prepend("file:");
                } else if (mimeType.left(5) == "text/") {
                    url.prepend("file:");
                }
            }
        }
        n = n.nextSibling();
    }

    docItem->setURL(url);
}

void khcNavigatorWidget::slotReloadTree()
{
    emit setBussy(true);
    clearTree();
    buildTree();
    emit setBussy(false);
}

void khcNavigatorWidget::slotURLSelected(QString url)
{
    emit itemSelected(url);
}

void khcNavigatorWidget::slotGlossaryItemSelected(QListViewItem *item)
{
  if (!item)
    return;

  if (dynamic_cast<SectionItem *>(item->parent())) {
    GlossaryEntry *entry = glossEntries[item->text(0)];
    kdDebug(1400) << "Emitting entry " << entry->term << endl;
    emit glossSelected(*entry);
  }

  item->setOpen(!item->isOpen());
}

void khcNavigatorWidget::slotItemSelected(QListViewItem* currentItem)
{
  if (!currentItem)
    return;
  khcNavigatorItem *item = static_cast<khcNavigatorItem*>(currentItem);

  if (item->childCount() > 0)
    {
      if (item->isOpen())
        item->setOpen(false);
      else
        item->setOpen(true);
    }

  // find the highlighted item in our lists
  for (item = staticItems.first(); item != 0; item = staticItems.next())
    {
      if (item == contentsTree->currentItem())
	{
	  if (!item->getURL().isEmpty())
	    emit itemSelected(item->getURL());
	  return;
	}
    }
  for (item = manualItems.first(); item != 0; item = manualItems.next())
    {
      if (item == contentsTree->currentItem())
	{
	  if (!item->getURL().isEmpty())
	    emit itemSelected(item->getURL());
	  return;
	}
    }
  for (item = pluginItems.first(); item != 0; item = pluginItems.next())
    {
      if (item == contentsTree->currentItem())
	{
	  if (!item->getURL().isEmpty()) {
            QString url = item->getURL();
            kdDebug(1400) << "plugin url: " << url << endl;
            int colonPos = url.find(':');
            if (colonPos < 0 || colonPos > url.find('/')) {
              url = "file:" + langLookup(url);
            }
	    emit itemSelected(url);
          }
	  return;
	}
    }
  for (item = scrollKeeperItems.first(); item != 0; item = scrollKeeperItems.next())
    {
      if (item == contentsTree->currentItem())
	{
	  if (!item->getURL().isEmpty())
	    emit itemSelected(item->getURL());
	  return;
	}
    }
}

bool khcNavigatorWidget::appendEntries(const QString &dirName, khcNavigatorItem *parent, QList<khcNavigatorItem> *appendList)
{
    QDir fileDir(dirName, "*.desktop", 0, QDir::Files | QDir::Hidden | QDir::Readable);

    if (!fileDir.exists())
	return false;

    QStringList fileList = fileDir.entryList();
    QStringList::Iterator itFile;

    for ( itFile = fileList.begin(); !(*itFile).isNull(); ++itFile )
    {
	QString filename = dirName;
	filename += "/";
	filename += *itFile;

    khcNavigatorItem *entry;
    if (parent)
        entry = new khcNavigatorItem(parent);
    else
        entry = new khcNavigatorItem(contentsTree);

	if (entry->readKDElnk(filename))
	    appendList->append(entry);
	else
	    delete entry;
    }

    return true;
}


bool khcNavigatorWidget::processDir( const QString &dirName, khcNavigatorItem *parent,  QList<khcNavigatorItem> *appendList)
{
    QString folderName;

    QDir dirDir( dirName, "*", 0, QDir::Dirs );

    if (!dirDir.exists()) return false;

    QStringList dirList = dirDir.entryList();
    QStringList::Iterator itDir;

    for ( itDir = dirList.begin(); !(*itDir).isNull(); ++itDir )
    {
	if ( (*itDir)[0] == '.' )
	    continue;


	QString filename = dirDir.path();
	filename += "/";
	filename += *itDir;

	QString dirFile = filename;
	dirFile += "/.directory";
	QString icon;

	if ( QFile::exists( dirFile ) )
	{
	    KSimpleConfig sc( dirFile, true );
            sc.setDesktopGroup();
	    folderName = sc.readEntry("Name");

	    //icon = sc.readEntry("MiniIcon");
	    //if (icon.isEmpty())
	    icon = "contents2";
	}
	else
	{
	    folderName = *itDir;
	    icon = "contents2";
	}

    khcNavigatorItem *dirItem;
    if (parent)
        dirItem = new khcNavigatorItem(parent, folderName, icon);
    else
        dirItem = new khcNavigatorItem(contentsTree, folderName, icon);
    appendList->append(dirItem);


	// read and append child items
	appendEntries(filename, dirItem, appendList);
	processDir(filename, dirItem, appendList);
    }
    return true;
}

QString khcNavigatorWidget::langLookup(const QString &fname)
{
    QStringList search;

    // assemble the local search paths
    const QStringList localDoc = KGlobal::dirs()->resourceDirs("html");

    // look up the different languages
    for (int id=localDoc.count()-1; id >= 0; --id)
    {
        QStringList langs = KGlobal::locale()->languageList();
        langs.append("default");
        langs.append("en");
        QStringList::ConstIterator lang;
        for (lang = langs.begin(); lang != langs.end(); ++lang)
            search.append(QString("%1%2/%3").arg(localDoc[id]).arg(*lang).arg(fname));
    }

    // try to locate the file
    QStringList::Iterator it;
    for (it = search.begin(); it != search.end(); ++it)
    {
        kdDebug(1400) << "Looking for help in: " << *it << endl;

        QFileInfo info(*it);
        if (info.exists() && info.isFile() && info.isReadable())
            return *it;

        QString file = (*it).left((*it).findRev('/')) + "/index.docbook";
        kdDebug(1400) << "Looking for help in: " << file << endl;
        info.setFile(file);
        if (info.exists() && info.isFile() && info.isReadable())
            return *it;
    }

    return QString::null;
}



#include "khc_navigator.moc"
