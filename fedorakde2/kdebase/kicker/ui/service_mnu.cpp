/*****************************************************************

Copyright (c) 1996-2000 the kicker authors. See file AUTHORS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include <qdragobject.h>
#include <qpixmap.h>
#include <qimage.h>

#include <kapp.h>
#include <kstddirs.h>
#include <kdebug.h>
#include <kdesktopfile.h>
#include <kglobalsettings.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmimetype.h>
#include <krun.h>
#include <kservicegroup.h>
#include <ksycoca.h>
#include <ksycocaentry.h>
#include <kservice.h>

#include "recentapps.h"
#include "service_mnu.h"
#include "service_mnu.moc"

const int idStart = 4242;
const int kRecentMenusOffset = 1000;

// static object that stores "recent" history
static RecentlyLaunchedApps s_RecentApps;

PanelServiceMenu::PanelServiceMenu(const QString & label, const QString & relPath, QWidget * parent,
                                   const char * name, bool addmenumode)
    : PanelMenu(label, parent, name), relPath_(relPath), clearOnClose_(false),addmenumode_(addmenumode)
{
    merge_ = KGlobal::config()->readBoolEntry("MergeKDEDirs", true);
    setAcceptDrops(true);
    subMenus.setAutoDelete(true);

    connect(KSycoca::self(),  SIGNAL(databaseChanged()),  SLOT(slotClearOnClose()));
}

PanelServiceMenu::~PanelServiceMenu()
{
}

// create and fill "recent" section at first
void PanelServiceMenu::createRecentMenuItems()
{
    s_RecentApps.init();

    QStringList RecentApps;
    s_RecentApps.getRecentApps(RecentApps);

    if (RecentApps.count() > 0) {
        bool bSeparator = false;
        int nId = idStart + kRecentMenusOffset;

        for (QValueList<QString>::ConstIterator it = RecentApps.fromLast(); ; --it) {
            KService::Ptr s = KService::serviceByDesktopPath(*it);
            if (!s)
                s_RecentApps.removeItem(*it);
            else
            {
                if (!bSeparator) {
                    bSeparator = true;
                    insertSeparator(0);
                }
                insertMenuItem(s, nId++, 0);
                s_RecentApps.m_nNumMenuItems++;
            }

            if (it == RecentApps.begin())
                break;
        }
    }
}

// updates "recent" section of KMenu
void PanelServiceMenu::updateRecentMenuItems(KService::Ptr & service)
{
    QString strItem(service->desktopEntryPath());

    // don't add an item from root kmenu level
    if (!strItem.contains('/'))
        return;

    // add it into recent apps list
    s_RecentApps.appLaunched(strItem);
    s_RecentApps.save();

    s_RecentApps.m_bNeedToUpdate = true;
}

void PanelServiceMenu::updateRecent()
{
    if (!s_RecentApps.m_bNeedToUpdate)
        return;

    s_RecentApps.m_bNeedToUpdate = false;

    bool bNeedSeparator = (s_RecentApps.m_nNumMenuItems <= 0);
    int nId = idStart + kRecentMenusOffset;

    // remove previous items
    if (s_RecentApps.m_nNumMenuItems > 0)
    {
        for (int i = 0; i < s_RecentApps.m_nNumMenuItems; i++)
        {
            removeItem(nId + i);
            entryMap_.remove(nId + i);
        }
        s_RecentApps.m_nNumMenuItems = 0;
    }

    // insert new items
    QStringList RecentApps;
    s_RecentApps.getRecentApps(RecentApps);

    if (RecentApps.count() > 0)
    {
        for (QValueList<QString>::ConstIterator it = RecentApps.fromLast(); ; --it)
        {
            KService::Ptr s = KService::serviceByDesktopPath(*it);
            if (!s)
            {
                s_RecentApps.removeItem(*it);
            }
            else
            {
                if (bNeedSeparator)
                {
                    bNeedSeparator = false;
                    insertSeparator(0);
                }
                insertMenuItem(s, nId++, 0);
                s_RecentApps.m_nNumMenuItems++;
            }

            if (it == RecentApps.begin())
                break;
        }
    }
}

void PanelServiceMenu::initialize()
{
    if (init) return;

    init = true;
    entryMap_.clear();
    clear();
    subMenus.clear();

    // Set the startposition outside the panel, so there is no drag initiated
    // when we use drag and click to select items. A drag is only initiated when
    // you click to open the menu, and then press and drag an item.
    startPos_ = QPoint(-1,-1);

    // We ask KSycoca to give us all services (sorted).
    KServiceGroup::Ptr root = KServiceGroup::group(relPath_);

    if (!root || !root->isValid())
        return;

    KServiceGroup::List list = root->entries(true);

    if (list.isEmpty()) {
        setItemEnabled(insertItem(i18n("No entries")), false);
        return;
    }

    int id = idStart;

    if (addmenumode_) {
        int mid = insertItem(SmallIconSet("ok"), i18n("Add this menu"), id++);
        entryMap_.insert(mid, static_cast<KSharedPtr<KSycocaEntry> >(root));

        if (list.count() > 0) {
            insertSeparator();
            id++;
        }
    }

    KServiceGroup::List::ConstIterator it = list.begin();
    for (; it != list.end(); ++it) {

        KSycocaEntry * e = *it;

        if (e->isType(KST_KServiceGroup)) {

            KServiceGroup::Ptr g(static_cast<KServiceGroup *>(e));
            QString groupCaption = g->caption();

            // Avoid adding empty groups.
            KServiceGroup::Ptr subMenuRoot = KServiceGroup::group(g->relPath());
            if (subMenuRoot->childCount() == 0)
                continue;

            // Ignore dotfiles.
            if ((g->name().at(0) == '.'))
                continue;

            // Item names may contain ampersands. To avoid them being converted
            // to accelators, replace them with two ampersands.
            groupCaption.replace(QRegExp("&"), "&&");

            PanelServiceMenu * m =
                newSubMenu(g->name(), g->relPath(), this, g->name().utf8());
	    m->setCaption( groupCaption );
            int newId = insertItem(SmallIconSet(g->icon()), groupCaption, m, id++);
            entryMap_.insert(newId, static_cast<KSharedPtr<KSycocaEntry> >(g));
            // We have to delete the sub menu our selves! (See Qt docs.)
            subMenus.append(m);
        }
        else {
            KService::Ptr s(static_cast<KService *>(e));
            insertMenuItem(s, id++);
        }
    }
    if ( count() > 0  && !relPath_.isEmpty() )
	insertTearOffHandle();
}

void PanelServiceMenu::insertMenuItem(KService::Ptr & s, int nId, int nIndex/*= -1*/)
{
    QString serviceName = s->name();

    // check for NoDisplay
    if ((s->property("NoDisplay")).asBool())
        return;

    // ignore dotfiles.
    if ((serviceName.at(0) == '.'))
        return;

    // item names may contain ampersands. To avoid them being converted
    // to accelators, replace them with two ampersands.
    serviceName.replace(QRegExp("&"), "&&");

    QPixmap normal = KGlobal::instance()->iconLoader()->loadIcon(s->icon(), KIcon::Small,
                                                                 0, KIcon::DefaultState, 0L, true);
    QPixmap active = KGlobal::instance()->iconLoader()->loadIcon(s->icon(), KIcon::Small,
                                                                 0, KIcon::ActiveState, 0L, true);
    // make sure they are not larger than 16x16
    if (normal.width() > 16 || normal.height() > 16) {
        QImage tmp = normal.convertToImage();
        tmp = tmp.smoothScale(16, 16);
        normal.convertFromImage(tmp);
    }
    if (active.width() > 16 || active.height() > 16) {
        QImage tmp = active.convertToImage();
        tmp = tmp.smoothScale(16, 16);
        active.convertFromImage(tmp);
    }

    QIconSet iconset;
    iconset.setPixmap(normal, QIconSet::Small, QIconSet::Normal);
    iconset.setPixmap(active, QIconSet::Small, QIconSet::Active);

    int newId = insertItem(iconset, serviceName, nId, nIndex);
    entryMap_.insert(newId, static_cast<KSharedPtr<KSycocaEntry> >(s));
}

void PanelServiceMenu::slotExec(int id)
{
    if (!entryMap_.contains(id)) return;

    KSycocaEntry * e = entryMap_[id];

    kapp->propagateSessionManager();

    KService::Ptr service = static_cast<KService *>(e);
    KRun::run(*service, KURL::List());

    updateRecentMenuItems(service);
    startPos_ = QPoint(-1,-1);
}

void PanelServiceMenu::mousePressEvent(QMouseEvent * ev)
{
    startPos_ = ev->pos();
    PanelMenu::mousePressEvent(ev);
}

void PanelServiceMenu::mouseMoveEvent(QMouseEvent * ev)
{
    PanelMenu::mouseMoveEvent(ev);

    if ( (ev->state() & LeftButton ) != LeftButton )
        return;

    QPoint p = ev->pos() - startPos_;
    if (p.manhattanLength() <= QApplication::startDragDistance() )
        return;

    int id = idAt(startPos_);

    // Don't drag items we didn't create.
    if (id < idStart)
        return;

    if (!entryMap_.contains(id)) {
        kdDebug(1210) << "Cannot find service with menu id " << id << endl;
        return;
    }

    KSycocaEntry * e = entryMap_[id];

    KService::Ptr service = static_cast<KService *>(e);

    QString filePath;

    QPixmap icon;

    switch (e->sycocaType()) {

        case KST_KService:
            icon     = static_cast<KService *>(e)->pixmap(KIcon::Small);
            filePath = static_cast<KService *>(e)->desktopEntryPath();
            break;

        case KST_KServiceGroup:
            icon = KGlobal::iconLoader()
                   ->loadIcon(static_cast<KServiceGroup *>(e)->icon(), KIcon::Small);
            filePath = static_cast<KServiceGroup *>(e)->relPath();
            break;

        default:
            return;
            break;
    }

    // If the path to the desktop file is relative, try to get the full
    // path from KStdDirs.

    QString path = (filePath[0] == '/') ? filePath : locate("apps", filePath);
    QUriDrag * d(new QUriDrag(this));

    d->setPixmap(icon);
    d->setFilenames(QStringList(path));
    d->dragCopy();
    close();

    // Set the startposition outside the panel, so there is no drag initiated
    // when we use drag and click to select items. A drag is only initiated when
    // you click to open the menu, and then press and drag an item.
    startPos_ = QPoint(-1,-1);
}

void PanelServiceMenu::dragEnterEvent(QDragEnterEvent *e)
{
    e->accept();
}

void PanelServiceMenu::dragMoveEvent(QDragMoveEvent *e)
{
    e->accept();
}

PanelServiceMenu *PanelServiceMenu::newSubMenu(const QString & label, const QString & relPath,
                                               QWidget * parent, const char * name)
{
    return new PanelServiceMenu(label, relPath, parent, name);
}

void PanelServiceMenu::slotClearOnClose()
{
    if (!init) return;

    if (!isVisible()){
        clearOnClose_ = false;
        slotClear();
    }
    else
        clearOnClose_ = true;
}

void PanelServiceMenu::closeEvent(QCloseEvent *)
{
    if (clearOnClose_) {
        clearOnClose_ = false;
        slotClear();
    }
}

void PanelServiceMenu::slotClear()
{
    entryMap_.clear();
    PanelMenu::slotClear();
    subMenus.clear();
}

