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

#include <kaboutkde.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kconfig.h>
#include <kwin.h>
#include <kapp.h>
#include <dcopclient.h>
#include <kaction.h>
#include <kbookmarkmenu.h>

#include "k_mnu.h"
#include "k_mnu.moc"

#include "global.h"
#include "panel.h"
#include "quickbrowser_mnu.h"
#include "panelop_mnu.h"
#include "recent_mnu.h"
#include "client_mnu.h"

PanelKMenu::PanelKMenu(QWidget *parent, const char *name)
  : PanelServiceMenu(QString::null, QString::null, parent, name)
    ,bookmarkMenu(0), bookmarkOwner(0)
{
    // set the first client id to some arbitrarily large value.
    client_id = 10000;
    // Don't automatically clear the main menu.
    disableAutoClear();
    actionCollection = new KActionCollection( this );
    setCaption( i18n( "K Menu" ) );
}

PanelKMenu::~PanelKMenu()
{
    delete bookmarkMenu;
    delete bookmarkOwner;
}

void PanelKMenu::initialize()
{
    updateRecent();
    if (init) return;

    // add services
    PanelServiceMenu::initialize();
    insertSeparator();

    // create recent menu section
    createRecentMenuItems();

    KConfig *config = KGlobal::config();
    config->setGroup("menus");

    bool need_separator = false;

    // insert bookmarks
    if(config->readBoolEntry("UseBookmarks", true))
    {
        // Need to create a new popup each time, it's deleted by subMenus.clear()
        QPopupMenu * bookmarkParent = new QPopupMenu( this, "bookmarks" );
        if(!bookmarkOwner)
            bookmarkOwner = new KBookmarkOwner;
        if(bookmarkMenu)
            delete bookmarkMenu; // can't reuse old one, the popup has been deleted
        bookmarkMenu = new KBookmarkMenu( bookmarkOwner, bookmarkParent, actionCollection, true, false );

        QString bookmarkstring = i18n( "&Bookmarks" );
        int p;
        while ( ( p = bookmarkstring.find( '&' ) ) >= 0 )
            bookmarkstring.remove( p, 1 );
        insertItem( SmallIconSet( "bookmark" ), bookmarkstring, bookmarkParent );
        subMenus.append(bookmarkParent);
        need_separator = true;
    }

    // insert recent documents menu
    if(config->readBoolEntry("UseRecent", true))
    {
        PanelRecentMenu *recentMnu = new PanelRecentMenu(this);
        insertItem(SmallIconSet("document"), i18n("Recent Documents"), recentMnu);
        subMenus.append(recentMnu);
        need_separator = true;
    }

    // insert quickbrowser
    if(config->readBoolEntry("UseBrowser", true))
    {
        PanelQuickBrowser *browserMnu = new PanelQuickBrowser(this);
        insertItem(SmallIconSet("kdisknav"), i18n("Quick Browser"), browserMnu);
        subMenus.append(browserMnu);
        need_separator = true;
    }

    if (need_separator)
        insertSeparator();

    // insert client menus, if any
    if (clients.count() > 0) {
        QIntDictIterator<KickerClientMenu> it(clients);
        while (it){
            if (it.current()->text.at(0) != '.')
                insertItem(
                    it.current()->icon,
                    it.current()->text,
                    it.current(),
                    it.currentKey()
                    );
            ++it;
        }
        insertSeparator();
    }

    // run command
    insertItem(SmallIconSet("run"), i18n("Run Command..."), this, SLOT( slotRunCommand() ) );
    insertSeparator();

    // insert panel menu
    PanelOpMenu *panelOpMenu = new PanelOpMenu(false, this);
    insertItem(SmallIconSet("panel"), i18n("Configure Panel"), panelOpMenu);
    subMenus.append(panelOpMenu);

    // lock and logout
    insertItem(SmallIconSet("lock"), i18n("Lock Screen"), this, SLOT(slotLock()));
    insertItem(SmallIconSet("exit"), i18n("Logout"), this, SLOT(slotLogout()));
    adjustSize();

    insertTearOffHandle();

    init = true;
}

int PanelKMenu::insertClientMenu(KickerClientMenu *p)
{
    int id = client_id;
    clients.insert(id, p);
    slotClear();
    return id;
}

void PanelKMenu::removeClientMenu(int id)
{
    clients.remove(id);
    removeItem(id);
    slotClear();
}

void PanelKMenu::slotLock()
{
    DCOPClient *client = kapp->dcopClient();
    client->send("kdesktop", "KScreensaverIface", "lock()", "");
}

void PanelKMenu::slotLogout()
{
    kapp->requestShutDown();
}

extern int kicker_screen_number;

void PanelKMenu::slotRunCommand()
{
    QByteArray data;
    QCString appname( "kdesktop" );
    if ( kicker_screen_number )
        appname.sprintf("kdesktop-screen-%d", kicker_screen_number);

    kapp->dcopClient()->send( appname, "KDesktopIface", "popupExecuteCommand()", data );
}
