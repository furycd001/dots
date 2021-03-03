/*
 *  khc_navigator.h - part of the KDE Help Center
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

#ifndef __khc_navigator_h__
#define __khc_navigator_h__


#include <qlist.h>
#include <kparts/browserextension.h>
#include <kparts/part.h>
#include <qtabwidget.h>
#include <qlistview.h>
#include <kprocess.h>

class SearchWidget;
class khcNavigatorItem;
class khcNavigator;
class KListView;
class KService;
class KProcIO;

class SectionItem : public QListViewItem
{
  public:
    SectionItem(QListViewItem *, const QString &);

    virtual void setOpen(bool);
};

class khcNavigatorExtension : public KParts::BrowserExtension
{
    Q_OBJECT
 public:
    khcNavigatorExtension(KParts::ReadOnlyPart *part, const char *name=0) :
      KParts::BrowserExtension( part, name ) {}
    virtual ~khcNavigatorExtension() {}

 public slots:
    void slotItemSelected(const QString&);
};

class khcNavigator : public KParts::ReadOnlyPart
{
    Q_OBJECT

 public:
    khcNavigator(QWidget *parentWidget, QObject *widget, const char *name=0);
    virtual ~khcNavigator();

    virtual bool openURL( const KURL &url );

 protected:
    bool openFile();
    khcNavigatorExtension * m_extension;

};

class khcNavigatorWidget : public QTabWidget
{
    Q_OBJECT

 public:
    struct GlossaryEntry {
      GlossaryEntry() {}
      GlossaryEntry(const QString &t, const QString &d, const QStringList &sa)
      {
        term = t;
        definition = d;
        seeAlso = sa;
      }

     QString term;
      QString definition;
      QStringList seeAlso;
    };
    
    khcNavigatorWidget(QWidget *parent=0, const char *name=0);
    virtual ~khcNavigatorWidget();

		GlossaryEntry glossEntry(const QString &term) const { return *glossEntries[term]; }

 public slots:
    void slotURLSelected(QString url);
    void slotItemSelected(QListViewItem* index);
    void slotGlossaryItemSelected(QListViewItem* item);
    void slotReloadTree();

 signals:
    void itemSelected(const QString& itemURL);
    void glossSelected(const khcNavigatorWidget::GlossaryEntry& entry);
    void setBussy(bool bussy);

 private slots:
    void getScrollKeeperContentsList(KProcIO *proc);
    void gotMeinprocOutput(KProcess *, char *data, int len);
    void meinprocExited(KProcess *);

 private:
    void setupContentsTab();
    void setupIndexTab();
    void setupSearchTab();
    void setupGlossaryTab();
    void buildGlossaryCache();
    void buildTree();
    void clearTree();
    QString langLookup(const QString &);

    void buildManSubTree(khcNavigatorItem *parent);

    void buildManualSubTree(khcNavigatorItem *parent, QString relPath);
    QString documentationURL(KService *s);

    void insertPlugins();
    void insertScrollKeeperItems();
    int insertScrollKeeperSection(khcNavigatorItem *parentItem,QDomNode sectNode);
    void insertScrollKeeperDoc(khcNavigatorItem *parentItem,QDomNode docNode);

    bool appendEntries (const QString &dirName,  khcNavigatorItem *parent, QList<khcNavigatorItem> *appendList);
    bool processDir(const QString &dirName, khcNavigatorItem *parent,  QList<khcNavigatorItem> *appendList);

    QListViewItem *byTopicItem, *alphabItem;
    KListView *contentsTree, *glossaryTree;
    // SearchWidget *search;

    QList<khcNavigatorItem> staticItems, manualItems, pluginItems, scrollKeeperItems;

    bool mScrollKeeperShowEmptyDirs;
    QString mScrollKeeperContentsList;
    
    QDict<GlossaryEntry> glossEntries;
    KProcess *meinproc;
    QString htmlData;
};

inline QDataStream &operator<<( QDataStream &stream, const khcNavigatorWidget::GlossaryEntry &e )
{ return stream << e.term << e.definition << e.seeAlso; }

inline QDataStream &operator>>( QDataStream &stream, khcNavigatorWidget::GlossaryEntry &e )
{ return stream >> e.term >> e.definition >> e.seeAlso; }
 
#endif
