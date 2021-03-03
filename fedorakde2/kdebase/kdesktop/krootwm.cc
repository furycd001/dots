/*
 * krootwm.cc Part of the KDE project.
 *
 * Copyright (C) 1997 Matthias Ettrich
 *           (C) 1997 Torben Weis, weis@kde.org
 *           (C) 1998 S.u.S.E. weis@suse.de

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qdir.h>

#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#include <qmessagebox.h>
#include <qtimer.h>
#include <kpopupmenu.h>
#include <kapp.h>
#include <kbookmark.h>
#include <kbookmarkmenu.h>
#include <kglobal.h>
#include <kio/job.h>
#include <kiconloader.h>
#include <klocale.h>
#include <knewmenu.h>
#include <kprocess.h>
#include <krun.h>
#include <ksimpleconfig.h>
#include <kwinmodule.h>
#include <kwin.h>
#include <kglobalsettings.h>
#include <kstddirs.h>
#include <dcopclient.h>
#include <khelpmenu.h>
#include <kstringhandler.h>
#include <kiconloader.h>
#include <kwin.h>
#include <netwm.h>
#include <kdebug.h>
#include <kwindowlistmenu.h>
#include <kdesktopwidget.h>

#include <X11/X.h>
#include <X11/Xlib.h>

#include "krootwm.h"
#include "kdiconview.h"
#include "desktop.h"
#include "kcustommenu.h"

#include <kmenubar.h>

KRootWm * KRootWm::s_rootWm = 0;


// for multihead
extern int kdesktop_screen_number;


KRootWm::KRootWm(KDesktop* _desktop) : QObject(_desktop)
{
  s_rootWm = this;
  m_actionCollection = new KActionCollection();
  m_pDesktop = _desktop;
  customMenu1 = 0;
  customMenu2 = 0;

  // Creates the new menu
  menuBar = 0; // no menubar yet
  menuNew = new KNewMenu( m_actionCollection, "new_menu" );
  connect(menuNew->popupMenu(), SIGNAL( aboutToShow() ),
          this, SLOT( slotFileNewAboutToShow() ) );

  bookmarks = new KActionMenu( i18n("Bookmarks"), "bookmark", m_actionCollection, "bookmarks" );
  bookmarkMenu = new KBookmarkMenu( new KBookmarkOwner(),
                                    bookmarks->popupMenu(),
                                    m_actionCollection,
                                    true, false );

  // The windowList and desktop menus can be part of a menubar (Mac style)
  // so we create them here
  desktopMenu = new QPopupMenu;
  windowListMenu = new KWindowListMenu;
  connect( windowListMenu, SIGNAL( aboutToShow() ),
           this, SLOT( slotWindowListAboutToShow() ) );

  // Create the actions

  m_actionCollection->insert( m_pDesktop->actionCollection()->action( "paste" ) );
  m_actionCollection->insert( m_pDesktop->actionCollection()->action( "undo" ) );
  new KAction(i18n("Help on desktop..."), "contents", 0, this, SLOT( slotHelp() ), m_actionCollection, "help" );
  new KAction(i18n("Run Command..."), "run", 0, m_pDesktop, SLOT( slotExecuteCommand() ), m_actionCollection, "exec" );
  new KAction(i18n("Configure Background..."), "background", 0, this, SLOT( slotConfigureBackground() ),
              m_actionCollection, "configbackground" );
  new KAction(i18n("Configure Desktop..."), "configure", 0, this, SLOT( slotConfigureDesktop() ),
              m_actionCollection, "configdesktop" );
  new KAction(i18n("Disable Desktop Menu"), 0, this, SLOT( slotToggleDesktopMenu() ),
              m_actionCollection, "togglemenubar" );

  new KAction(i18n("Unclutter Windows"), 0, this, SLOT( slotUnclutterWindows() ),
              m_actionCollection, "unclutter" );
  new KAction(i18n("Cascade Windows"), 0, this, SLOT( slotCascadeWindows() ),
              m_actionCollection, "cascade" );

	// arrange menu actions
	new KAction(i18n("by Name (Case Sensitive)"), 0, this, SLOT( slotArrangeByNameCS() ),
							m_actionCollection, "sort_nc");
	new KAction(i18n("by Name (Case Insensitive)"), 0, this, SLOT( slotArrangeByNameCI() ),
							m_actionCollection, "sort_nci");
	new KAction(i18n("by Size"), 0, this, SLOT( slotArrangeBySize() ),
							m_actionCollection, "sort_size");
	new KAction(i18n("by Type"), 0, this, SLOT( slotArrangeByType() ),
							m_actionCollection, "sort_type");

	KToggleAction *aSortDirsFirst = new KToggleAction( i18n("Directories first"), 0, m_actionCollection, "sort_directoriesfirst" );
	aSortDirsFirst->setChecked(true);
	//
  new KAction(i18n("Line up Icons"), 0, this, SLOT( slotLineupIcons() ),
              m_actionCollection, "lineup" );
  new KAction(i18n("Refresh Desktop"), 0, this, SLOT( slotRefreshDesktop() ),
              m_actionCollection, "refresh" );
  // Icons in sync with kicker
  new KAction(i18n("Lock Screen"), "lock", 0, this, SLOT( slotLock() ),
              m_actionCollection, "lock" );
  new KAction(i18n("Logout"), "exit", 0, this, SLOT( slotLogout() ),
              m_actionCollection, "logout" );

  initConfig();
}

KRootWm::~KRootWm()
{
    delete m_actionCollection;
}

void KRootWm::initConfig()
{
//  kdDebug() << "KRootWm::initConfig" << endl;
  KConfig *kconfig = KGlobal::config();

  // parse the configuration
  kconfig->setGroup(QString::fromLatin1("KDE"));
  globalMenuBar = kconfig->readBoolEntry(QString::fromLatin1("macStyle"), false);
  kconfig->setGroup(QString::fromLatin1("Menubar"));
  showMenuBar = globalMenuBar || kconfig->readBoolEntry(QString::fromLatin1("ShowMenubar"), false );

  // read configuration for clicks on root window
  const char * s_choices[6] = { "", "WindowListMenu", "DesktopMenu", "AppMenu", "CustomMenu1", "CustomMenu2" };
  leftButtonChoice = middleButtonChoice = rightButtonChoice = NOTHING;
  kconfig->setGroup("Mouse Buttons");
  QString s = kconfig->readEntry("Left", "");
  for ( int c = 0 ; c < 6 ; c ++ )
    if (s == s_choices[c])
      { leftButtonChoice = (menuChoice) c; break; }
  s = kconfig->readEntry("Middle", "WindowListMenu");
  for ( int c = 0 ; c < 6 ; c ++ )
    if (s == s_choices[c])
      { middleButtonChoice = (menuChoice) c; break; }
  s = kconfig->readEntry("Right", "DesktopMenu");
  for ( int c = 0 ; c < 6 ; c ++ )
    if (s == s_choices[c])
      { rightButtonChoice = (menuChoice) c; break; }

  buildMenus();
}


void KRootWm::buildMenus()
{
//    kdDebug() << "KRootWm::buildMenus" << endl;

    delete menuBar;
    menuBar = 0;

    delete customMenu1;
    customMenu1 = 0;
    delete customMenu2;
    customMenu2 = 0;

    if (showMenuBar)
    {
//        kdDebug() << "showMenuBar" << endl;
        QWidget* dummy = new QWidget;
        menuBar = new KMenuBar( dummy );
        disconnect( kapp, SIGNAL( appearanceChanged() ), menuBar, SLOT( slotReadConfig() ) );
        menuBar->setCaption("KDE Desktop");
    }

    // create Arrange menu
    QPopupMenu *pArrangeMenu = new QPopupMenu;
    m_actionCollection->action("sort_nc")->plug( pArrangeMenu );
    m_actionCollection->action("sort_nci")->plug( pArrangeMenu );
    m_actionCollection->action("sort_size")->plug( pArrangeMenu );
    m_actionCollection->action("sort_type")->plug( pArrangeMenu );
    pArrangeMenu->insertSeparator();
    m_actionCollection->action("sort_directoriesfirst")->plug( pArrangeMenu );

    if (menuBar) {
        file = new QPopupMenu;

        m_actionCollection->action("exec")->plug( file );

        file->insertSeparator();
        m_actionCollection->action("lock")->plug( file );
        m_actionCollection->action("logout")->plug( file );

        desk = new QPopupMenu;

        m_actionCollection->action("unclutter")->plug( desk );
        m_actionCollection->action("cascade")->plug( desk );
        desk->insertSeparator();
        m_actionCollection->action("lineup")->plug( desk );

        desk->insertItem(i18n("Arrange Icons"), pArrangeMenu);

        m_actionCollection->action("refresh")->plug( desk );
        desk->insertSeparator();
        m_actionCollection->action("configbackground")->plug( desk );
        m_actionCollection->action("configdesktop")->plug( desk );
        desk->insertSeparator();
        m_actionCollection->action("togglemenubar")->plug( desk );
        m_actionCollection->action("togglemenubar")->setText(i18n("Disable Desktop Menu"));

        help = new KHelpMenu(0, 0, false);
    }
    else
        m_actionCollection->action("togglemenubar")->setText(i18n("Enable Desktop Menu"));

    desktopMenu->clear();
    desktopMenu->disconnect( this );

    menuNew->plug( desktopMenu );
    bookmarks->plug( desktopMenu );
    desktopMenu->insertSeparator();

    m_actionCollection->action("undo")->plug( desktopMenu );
    m_actionCollection->action("paste")->plug( desktopMenu );
    m_actionCollection->action("help")->plug( desktopMenu );
    m_actionCollection->action("exec")->plug( desktopMenu );
    desktopMenu->insertSeparator();
    m_actionCollection->action("configbackground")->plug( desktopMenu );
    m_actionCollection->action("configdesktop")->plug( desktopMenu );
    desktopMenu->insertSeparator();
    if ( !globalMenuBar ) {
	m_actionCollection->action("togglemenubar")->plug( desktopMenu );
	desktopMenu->insertSeparator();
    }
    m_actionCollection->action("unclutter")->plug( desktopMenu );
    m_actionCollection->action("cascade")->plug( desktopMenu );
    desktopMenu->insertSeparator();
    m_actionCollection->action("lineup")->plug( desktopMenu );
		desktopMenu->insertItem(i18n("Arrange Icons"), pArrangeMenu);
    m_actionCollection->action("refresh")->plug( desktopMenu );

    desktopMenu->insertSeparator();
    m_actionCollection->action("lock")->plug( desktopMenu );
    m_actionCollection->action("logout")->plug( desktopMenu );
    connect( desktopMenu, SIGNAL( aboutToShow() ), this, SLOT( slotFileNewAboutToShow() ) );

    if (menuBar) {
        menuBar->insertItem(i18n("File"), file);
        menuBar->insertItem(i18n("New"), menuNew->popupMenu());
        menuBar->insertItem(i18n("Bookmarks"), bookmarks->popupMenu());
        menuBar->insertItem(i18n("Desktop"), desk);
        menuBar->insertItem(i18n("Windows"), windowListMenu);
        menuBar->insertItem(i18n("Help"), help->menu());
        help->menu()->removeItemAt( 4 ); // we don't need no aboutApplication

        menuBar->setTopLevelMenu( true );
        XSetTransientForHint( qt_xdisplay(), menuBar->winId(), m_pDesktop->winId() );
        menuBar->show(); // we need to call show() as we delayed the creation with the timer
    }
}

void KRootWm::slotFileNewAboutToShow()
{
//  kdDebug() << " KRootWm:: (" << this << ") slotFileNewAboutToShow() menuNew=" << menuNew << endl;
  // As requested by KNewMenu :
  menuNew->slotCheckUpToDate();
  // And set the files that the menu apply on :
  menuNew->setPopupFiles( m_pDesktop->url() );
}

void KRootWm::slotWindowListAboutToShow()
{
  windowListMenu->init();
}

void KRootWm::activateMenu( menuChoice choice, const QPoint& global )
{
  switch ( choice )
  {
    case WINDOWLISTMENU:
      windowListMenu->popup(global);
      break;
    case DESKTOPMENU:
      desktopMenu->popup(global);
      break;
    case APPMENU:
    {
      // This allows the menu to disappear when clicking on the background another time
      XUngrabPointer(qt_xdisplay(), CurrentTime);
      XSync(qt_xdisplay(), False);
      // Ask kicker to showup the menu
      QByteArray data;
      QDataStream stream( data, IO_WriteOnly );
      stream << global.x();
      stream << global.y();

      // make sure we send the message to the correct kicker
      QCString appname;
      if (kdesktop_screen_number == 0)
	  appname = "kicker";
      else
	  appname.sprintf("kicker-screen-%d", kdesktop_screen_number);

      kapp->dcopClient()->send( appname.data(), "kickerMenuManager",
				"popupKMenu(int,int)", data );
      break;
    }
    case CUSTOMMENU1:
      if (!customMenu1)
         customMenu1 = new KCustomMenu("kdesktop_custom_menu1");
      customMenu1->popup(global);
      break;
    case CUSTOMMENU2:
      if (!customMenu2)
         customMenu2 = new KCustomMenu("kdesktop_custom_menu2");
      customMenu2->popup(global);
      break;
    case NOTHING:
    default:
      break;
  }
}

void KRootWm::mousePressed( const QPoint& _global, int _button )
{
    if (!desktopMenu) return; // initialisation not yet done
    switch ( _button ) {
    case LeftButton:
        if ( showMenuBar && menuBar )
            menuBar->raise();
        activateMenu( leftButtonChoice, _global );
        break;
    case MidButton:
        activateMenu( middleButtonChoice, _global );
        break;
    case RightButton:
        activateMenu( rightButtonChoice, _global );
        break;
    default:
        // nothing
        break;
    }
}

void KRootWm::slotWindowList() {
//  kdDebug() << "KRootWm::slotWindowList" << endl;
  windowListMenu->popup(QPoint(50,50) /* QCursor::pos() */);
}

void KRootWm::slotArrangeByNameCS()
{
    bool b = static_cast<KToggleAction *>(m_actionCollection->action("sort_directoriesfirst"))->isChecked();
    m_pDesktop->iconView()->rearrangeIcons( KDIconView::NameCaseSensitive, b);
}

void KRootWm::slotArrangeByNameCI()
{
    bool b = static_cast<KToggleAction *>(m_actionCollection->action("sort_directoriesfirst"))->isChecked();
    m_pDesktop->iconView()->rearrangeIcons( KDIconView::NameCaseInsensitive, b);
}

void KRootWm::slotArrangeBySize()
{
    bool b = static_cast<KToggleAction *>(m_actionCollection->action("sort_directoriesfirst"))->isChecked();
    m_pDesktop->iconView()->rearrangeIcons( KDIconView::Size, b);
}

void KRootWm::slotArrangeByType()
{
    bool b = static_cast<KToggleAction *>(m_actionCollection->action("sort_directoriesfirst"))->isChecked();
    m_pDesktop->iconView()->rearrangeIcons( KDIconView::Type, b);
}

void KRootWm::slotLineupIcons() {
    m_pDesktop->iconView()->lineupIcons();
}

void KRootWm::slotRefreshDesktop() {
    m_pDesktop->refresh();
}

void KRootWm::slotConfigureBackground() {
  QStringList args;
  args.append(QString::fromLatin1("background"));
  KApplication::kdeinitExec(QString::fromLatin1("kcmshell"), args);
}

void KRootWm::slotConfigureDesktop() {
  QStringList args;
  args.append(QString::fromLatin1("desktop"));
  KApplication::kdeinitExec(QString::fromLatin1("kcmshell"), args);
}


void KRootWm::slotToggleDesktopMenu()
{
    KConfig *config = KGlobal::config();
    KConfigGroupSaver saver(config, QString::fromLatin1("Menubar"));
    if (showMenuBar && menuBar)
	config->writeEntry(QString::fromLatin1("ShowMenubar"), false);
    else
	config->writeEntry(QString::fromLatin1("ShowMenubar"), true);
    config->sync();

    // make sure we send the message to the correct screen
    QCString appname;
    if (kdesktop_screen_number == 0)
	appname = "kdesktop";
    else
	appname.sprintf("kdesktop-screen-%d", kdesktop_screen_number);

    kapp->dcopClient()->send( appname.data(), "KDesktopIface", "configure()", "");
}


void KRootWm::slotUnclutterWindows()
{
    QCString appname;
    if (kdesktop_screen_number == 0)
	appname = "kwin";
    else
	appname.sprintf("kwin-screen-%d", kdesktop_screen_number);

    kapp->dcopClient()->send(appname.data(), "KWinInterface", "unclutterDesktop()", "");
}


void KRootWm::slotCascadeWindows() {
    QCString appname;
    if (kdesktop_screen_number == 0)
	appname = "kwin";
    else
	appname.sprintf("kwin-screen-%d", kdesktop_screen_number);

    kapp->dcopClient()->send(appname.data(), "KWinInterface", "cascadeDesktop()", "");
}


void KRootWm::slotHelp() {
    KApplication::kdeinitExec(QString::fromLatin1("khelpcenter2"));
}


void KRootWm::slotLock() {
    QCString appname;
    if (kdesktop_screen_number == 0)
	appname = "kdesktop";
    else
	appname.sprintf("kdesktop-screen-%d", kdesktop_screen_number);

    kapp->dcopClient()->send(appname, "KScreensaverIface", "lock()", "");
}


void KRootWm::slotLogout() {
    m_pDesktop->logout();
}


void KRootWm::slotMenuItemActivated(int /* item */ )
{
}


#include "krootwm.moc"
