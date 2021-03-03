/*
 * krootwm.h Part of the KDE project.
 *
 * Copyright (C) 1997 Matthias Ettrich
 *           (C) 1997 Torben Weis, weis@kde.org
 *           (C) 1998 S.u.S.E, weis@suse.de
 *
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

#ifndef __krootwm_h__
#define __krootwm_h__

#include <qpixmap.h>
#include <qobject.h>

// we need Window but do not want to include X.h since it
// #defines way too many constants
typedef unsigned long XID;
typedef XID Window;

class KMenuBar;
class KDesktop;
class QPopupMenu;
class KNewMenu;
class KWinModule;
class KBookmarkMenu;
class KHelpMenu;
class KActionCollection;
class KActionMenu;
class KWindowListMenu;

enum {
  ITEM_HELP=100,
  ITEM_PASTE,
  ITEM_EXECUTE,
  ITEM_CONFIGURE_BACKGROUND,
  ITEM_CONFIGURE_ICONS,
  ITEM_UNCLUTTER_WINDOWS,
  ITEM_CASCADE_WINDOWS,
  ITEM_ARRANGE_ICONS,
  ITEM_LOCK_SCREEN,
  ITEM_LOGOUT
};

/**
 * This class is the handler for the menus (root popup menu and desktop menubar)
 */
class KRootWm: public QObject {
  Q_OBJECT

public:
  KRootWm(KDesktop*);
  ~KRootWm();

  void mousePressed( const QPoint& _global, int _button );
  bool hasLeftButtonMenu() { return leftButtonChoice != NOTHING; }

  /**
   * Return the unique KRootWm instance
   */
  static KRootWm * getRootWm() { return s_rootWm; }

  /**
   * share this with desktop.cc
   */
  KNewMenu * newMenu() { return menuNew; }

  /**
   * Read and apply configuration
   */
  void initConfig();

public slots:
  void slotArrangeByNameCS();
  void slotArrangeByNameCI();
  void slotArrangeBySize();
  void slotArrangeByType();
  void slotLineupIcons();
  void slotRefreshDesktop();
  void slotConfigureBackground();
  void slotConfigureDesktop();
  void slotToggleDesktopMenu();
  void slotUnclutterWindows();
  void slotCascadeWindows();
  void slotWindowList();
  void slotHelp();
  void slotLock();
  void slotLogout();

private:
  KDesktop* m_pDesktop;

  // The five root menus :
  KWindowListMenu* windowListMenu;
  QPopupMenu* desktopMenu;
  // the appMenu is (will be) provided by kicker
  QPopupMenu* customMenu1;
  QPopupMenu* customMenu2;

  // Configuration for the root menus :
  typedef enum { NOTHING = 0, WINDOWLISTMENU, DESKTOPMENU, APPMENU, CUSTOMMENU1, CUSTOMMENU2 } menuChoice;
  menuChoice leftButtonChoice;
  menuChoice middleButtonChoice;
  menuChoice rightButtonChoice;

  KNewMenu* menuNew;
  KActionMenu* bookmarks;
  KBookmarkMenu* bookmarkMenu;
  KActionCollection * m_actionCollection;

  void activateMenu( menuChoice choice, const QPoint& global );
  void buildMenus();

  bool showMenuBar;
  bool globalMenuBar;
  KMenuBar *menuBar;

  QPopupMenu *file;
  QPopupMenu *desk;
  KHelpMenu *help;

  QPixmap defaultPixmap;

  static KRootWm * s_rootWm;

  bool m_bInit;

private slots:

  void slotMenuItemActivated(int);
  void slotFileNewAboutToShow();
  void slotWindowListAboutToShow();
};

#endif
