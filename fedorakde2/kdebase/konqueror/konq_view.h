/*
 *  This file is part of the KDE project
 *  Copyright (C) 1998, 1999 David Faure <faure@kde.org>
 *
 *  $Id: konq_view.h,v 1.140 2001/06/13 20:39:59 faure Exp $
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
 **/

#ifndef __konq_view_h__
#define __konq_view_h__ "$Id: konq_view.h,v 1.140 2001/06/13 20:39:59 faure Exp $"

#include "konq_mainwindow.h"
#include "konq_factory.h"

#include <kparts/browserextension.h>

#include <qlist.h>
#include <qstring.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qguardedptr.h>
#include <qcstring.h>

#include <ktrader.h>

class KonqRun;
class KonqFrame;
class KonqViewIface;
class KonqBrowserInterface;

struct HistoryEntry
{
  KURL url;
  QString locationBarURL; // can be different from url when showing a index.html
  QString title;
  QByteArray buffer;
  QString strServiceType;
  QString strServiceName;
};

/* This class represents a child of the main view. The main view maintains
 * the list of children. A KonqView contains a Browser::View and
 * handles it. It's more or less the backend structure for the views.
 * The widget handling stuff is done by the KonqFrame.
 */
class KonqView : public QObject
{
  Q_OBJECT
public:

  /**
   * Create a konqueror view
   * @param viewFactory the factory to be used to create the part
   * @param viewFrame the frame where to create the view
   * @param mainWindow is the main window :-)
   * @param service the service implementing the part
   * @param partServiceOffers list of part offers found by the factory
   * @param appServiceOffers list of app offers found by the factory
   * @param serviceType the serviceType implemented by the part
   * @param passiveMode whether to initially make the view passive
   */
  KonqView( KonqViewFactory &viewFactory,
            KonqFrame* viewFrame,
            KonqMainWindow * mainWindow,
            const KService::Ptr &service,
            const KTrader::OfferList &partServiceOffers,
            const KTrader::OfferList &appServiceOffers,
            const QString &serviceType,
            bool passiveMode);

  ~KonqView();

  /**
   * Displays another URL, but without changing the view mode (caller has to
   * ensure that the call makes sense)
   * @param url the URL to open
   * @param locationBarURL the URL to set in the location bar (see @ref setLocationBarURL)
   * @param nameFilter e.g. *.cpp
   */
  void openURL( const KURL &url,
                const QString & locationBarURL,
                const QString &nameFilter = QString::null);

  /**
   * Change the type of view (i.e. loads a new konqueror view)
   * Contract: the caller should call stop() first,
   *
   * @param serviceType the service type we want to show
   * @param serviceName allows to enforce a particular service to be chosen,
   *        @see KonqFactory.
   */
  bool changeViewMode( const QString &serviceType,
                       const QString &serviceName = QString::null );

  /**
   * Call this to prevent next openURL() call from changing history lists
   * Used when the same URL is reloaded (for instance with another view mode)
   *
   * Calling with lock=false is a hack reserved to the "find" feature.
   */
  void lockHistory( bool lock = true ) { m_bLockHistory = lock; }

  /**
   * @return true if view can go back
   */
  bool canGoBack() { return m_lstHistory.at() > 0; }

  /**
   * @return true if view can go forward
   */
  bool canGoForward() { return m_lstHistory.at() != ((int)m_lstHistory.count())-1; }

  uint historyLength() { return m_lstHistory.count(); }

  /**
   * Move in history. +1 is "forward", -1 is "back", you can guess the rest.
   */
  void go( int steps );

  /**
   * @return the history of this view
   */
  const QList<HistoryEntry> & history() { return m_lstHistory; }

  /**
   * Creates a deep copy of the @p other view's history buffers.
   */
  void copyHistory( KonqView *other );

  /**
   * Set the KonqRun instance that is running something for this view
   * The main window uses this to store the KonqRun for each child view.
   */
  void setRun( KonqRun * run  );

  KonqRun *run() const { return m_pRun; }

  /**
   * Stop loading
   */
  void stop();

  /**
   * Retrieve view's URL
   */
  KURL url();

  /**
   * Get view's location bar URL, i.e. the one that the view signals
   * It can be different from url(), for instance if we display a index.html
   */
  const QString locationBarURL() { return m_sLocationBarURL; }

  /**
   * Get the URL that was typed to get the current URL.
   */
  const QString typedURL() { return m_sTypedURL; }
  /**
   * Set the URL that was typed to get the current URL.
   */
  void setTypedURL( const QString & u ) { m_sTypedURL = u; }

  /**
   * @return the part embedded into this view
   */
  KParts::ReadOnlyPart *part() const { return m_pPart; }

  /**
   * see KonqViewManager::removePart
   */
  void partDeleted() { m_pPart = 0L; }

  KParts::BrowserExtension *browserExtension() {
      return KParts::BrowserExtension::childObject( m_pPart );
  }

  /**
   * @return a pointer to the KonqFrame which the view lives in
   */
  KonqFrame* frame() const { return m_pKonqFrame; }

  /**
   * @return the servicetype this view is currently displaying
   */
  QString serviceType() const { return m_serviceType; }

  /**
   * @return the servicetypes this view is capable to display
   */
  QStringList serviceTypes() const { return m_service->serviceTypes(); }

  bool supportsServiceType( const QString &serviceType ) const
        { return serviceTypes().contains( serviceType ); }

  // True if "Use index.html" is set (->the view doesn't necessarily show HTML!)
  bool allowHTML() const { return m_bAllowHTML; }
  void setAllowHTML( bool allow ) { m_bAllowHTML = allow; }

  // True if currently loading
  bool isLoading() const { return m_bLoading; }
  void setLoading( bool b ) { m_bLoading = b; }

  // True if "locked to current location" (and their view mode, in fact)
  bool isLockedLocation() const { return m_bLockedLocation; }
  void setLockedLocation( bool b );

  // True if can't be made active (e.g. dirtree).
  bool isPassiveMode() const { return m_bPassiveMode; }
  void setPassiveMode( bool mode );

  // True if 'link' symbol set
  bool isLinkedView() const { return m_bLinkedView; }
  void setLinkedView( bool mode );

  // True if toggle view
  void setToggleView( bool b ) { m_bToggleView = b; }
  bool isToggleView() const { return m_bToggleView; }

  // True if locked to current view mode
  // Toggle views and passive views are locked to their view mode.
  bool isLockedViewMode() const { return m_bToggleView || m_bPassiveMode; }

  void setService( const KService::Ptr &s ) { m_service = s; }
  KService::Ptr service() { return m_service; }

  QString caption() const { return m_caption; }

  KTrader::OfferList partServiceOffers() { return m_partServiceOffers; }
  KTrader::OfferList appServiceOffers() { return m_appServiceOffers; }

  KonqMainWindow *mainWindow() const { return m_pMainWindow; }

  // return true if the method was found, false if the execution failed
  bool callExtensionMethod( const char *methodName );
  bool callExtensionBoolMethod( const char *methodName, bool value );
  bool callExtensionStringMethod( const char *methodName, QString value );

  void setViewName( const QString &name );
  QString viewName() const;

  // True to enable the context popup menu
  void enablePopupMenu( bool b );
  bool isPopupMenuEnabled() const { return m_bPopupMenuEnabled; }

  QStringList frameNames() const;

  KonqViewIface * dcopObject();

  void goHistory( int steps );

  static QStringList childFrameNames( KParts::ReadOnlyPart *part );

  static KParts::BrowserHostExtension *hostExtension( KParts::ReadOnlyPart *part, const QString &name );

signals:

  /**
   * Signal the main window that the embedded part changed (e.g. because of changeViewMode)
   */
  void sigPartChanged( KonqView *childView, KParts::ReadOnlyPart *oldPart, KParts::ReadOnlyPart *newPart );

  /**
   * Emitted in slotCompleted
   */
  void viewCompleted( KonqView * view );

public slots:
  /**
   * Store location-bar URL in the child view
   * and updates the main view if this view is the current one
   * May be different from url e.g. if using "allowHTML".
   */
  void setLocationBarURL( const QString & locationBarURL );
  /**
   * get an icon for the URL from the BrowserExtension
   */
  void setIconURL( const KURL &iconURL );

  void setCaption( const QString & caption ) { m_caption = caption; }

protected slots:
  // connected to the KROP's KIO::Job
  void slotStarted( KIO::Job * job );
  void slotCompleted();
  void slotCompleted( bool );
  void slotCanceled( const QString & errMsg );
  void slotPercent( KIO::Job *, unsigned long percent );
  void slotSpeed( KIO::Job *, unsigned long bytesPerSecond );
  void slotInfoMessage( KIO::Job *, const QString &msg );

  /**
   * Connected to the BrowserExtension
   */
  void slotSelectionInfo( const KFileItemList &items );
  void slotOpenURLNotify();
  void slotEnableAction( const char * name, bool enabled );

protected:
  /**
   * Replace the current view with a new view, created by @p viewFactory.
   */
  void switchView( KonqViewFactory &viewFactory );

  /**
   * Connects the internal part to the main window.
   * Do this after creating it and before inserting it.
   */
  void connectPart();

  /**
   * Creates a new entry in the history.
   */
  void createHistoryEntry();

  /**
   * Updates the current entry in the history.
   * @param saveLocationBarURL whether to save the location bar URL as part of it
   * (not done in openURL, to be able to revert if aborting)
   */
  void updateHistoryEntry(bool saveLocationBarURL);

  void sendOpenURLEvent( const KURL &url, const KParts::URLArgs &args = KParts::URLArgs() );

  void setServiceTypeInExtension();

  virtual bool eventFilter( QObject *obj, QEvent *e );

////////////////// protected members ///////////////

  KParts::ReadOnlyPart *m_pPart;

  QString m_sLocationBarURL;
  QString m_sTypedURL;

  /**
   * The full history (back + current + forward)
   * The current position in the history is m_lstHistory.current()
   */
  QList<HistoryEntry> m_lstHistory;

  KonqMainWindow *m_pMainWindow;
  QGuardedPtr<KonqRun> m_pRun;
  KonqFrame *m_pKonqFrame;
  uint m_bAllowHTML:1;
  uint m_bLoading:1;
  uint m_bLockedLocation:1;
  uint m_bPassiveMode:1;
  uint m_bLinkedView:1;
  uint m_bToggleView:1;
  uint m_bLockHistory:1;
  uint m_bAborted:1;
  uint m_bGotIconURL:1;
  uint m_bPopupMenuEnabled:1;
  KTrader::OfferList m_partServiceOffers;
  KTrader::OfferList m_appServiceOffers;
  KService::Ptr m_service;
  QString m_serviceType;
  QString m_caption;
  KonqViewIface * m_dcopObject;
  KonqBrowserInterface *m_browserIface;
};

#endif
