/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __desktop_h__
#define __desktop_h__

#include "KDesktopIface.h"

#include <qwidget.h>
#include <qstringlist.h>

class KURL;
class QCloseEvent;
class QDropEvent;
class QPopupMenu;
class KGlobalAccel;
class KWinModule;
class KBackgroundManager;
class QTimer;
class StartupId;
class KDIconView;
class Minicli;
class KActionCollection;

/**
 * KDesktop is the toplevel widget that is the desktop.
 * It handles the background, the screensaver and all the rest of the global stuff.
 * The icon view is a child widget of KDesktop.
 */
class KDesktop : public QWidget, virtual public KDesktopIface
{
  Q_OBJECT
public:

  KDesktop(bool x_root_hack, bool auto_start, bool wait_for_kded );
  ~KDesktop();

  // Implementation of the DCOP interface
  virtual void rearrangeIcons();
  virtual void lineupIcons();
  virtual void selectAll();
  virtual void unselectAll();
  virtual void refreshIcons();
  virtual QStringList selectedURLs();

  virtual void configure();
  virtual void popupExecuteCommand();
  virtual void refresh();
  virtual void logout();

  KWinModule* kwinModule() const { return m_pKwinmodule; }

  // The active widget (currently always the icon view)
  QWidget *widget() const;

  // The action collection of the active widget
  KActionCollection *actionCollection();

  // The URL (for the File/New menu)
  KURL url() const;

  // ## hack ##
  KDIconView *iconView() const { return m_pIconView; }

private slots:

  void workAreaChanged();

  /** Background is ready. */
  void backgroundInitDone();

  /** Activate the desktop. */
  void slotStart();

  /** Reconfigures */
  void slotConfigure();

  /** Show minicli,. the KDE command line interface */
  void slotExecuteCommand();

  /** Show taskmanger (calls KSysGuard with --showprocesses option) */
  void slotShowTaskManager();

  void slotShowWindowList();

  void slotLogout();
  void slotLogoutWithoutConfirmation();

  /** Connected to KSycoca */
  void slotDatabaseChanged();

  void slotShutdown();
  void slotSettingsChanged(int);

  /** set the vroot atom for e.g. xsnow */
  void slotSetVRoot();

  /** Connected to KDIconView */
  void handleImageDropEvent( QDropEvent * );
  void handleColorDropEvent( QDropEvent * );
  void slotNewWallpaper(const KURL &url);

  void updateWorkArea();

protected:
  void initConfig();

  virtual void closeEvent(QCloseEvent *e);

  virtual bool isVRoot() { return set_vroot; }
  virtual void setVRoot( bool enable );

private:

  KGlobalAccel *keys;

  KWinModule* m_pKwinmodule;

  KBackgroundManager* bgMgr;

  KDIconView *m_pIconView;

  QTimer *updateWorkAreaTimer;

  Minicli *m_miniCli;

  StartupId* startup_id;
  bool set_vroot;

  /** Set to true until start() has been called */
  bool m_bInit;

  /** Execute files from autoexec folder? */
  bool m_bAutoStart;

  /** Wait for kded to finish building database? */
  bool m_bWaitForKded;
};

#endif
