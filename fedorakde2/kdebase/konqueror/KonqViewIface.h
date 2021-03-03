/* This file is part of the KDE project
   Copyright (C) 2000 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2000 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __KonqViewIface_h__
#define __KonqViewIface_h__

#include <dcopobject.h>
#include <qvaluelist.h>
#include <dcopref.h>

class KonqView;

/**
 * DCOP interface for a konqueror main window
 */
class KonqViewIface : virtual public DCOPObject
{
  K_DCOP
public:

  KonqViewIface( KonqView * view );
  ~KonqViewIface();

k_dcop:

  /**
   * Displays another URL, but without changing the view mode
   * (Make sure the part can display this URL)
   */
  void openURL( QString url,
                const QString & locationBarURL,
                const QString & nameFilter );

  /**
   * Change the type of view (i.e. loads a new konqueror view)
   * @param serviceType the service type we want to show
   * @param serviceName allows to enforce a particular service to be chosen,
   *        @see KonqFactory.
   */
  bool changeViewMode( const QString &serviceType,
                       const QString &serviceName );

  /**
   * Call this to prevent next openURL() call from changing history lists
   * Used when the same URL is reloaded (for instance with another view mode)
   */
  void lockHistory();

  /**
   * Stop loading
   */
  void stop();

  /**
   * Retrieve view's URL
   */
  QString url();

  /**
   * Get view's location bar URL, i.e. the one that the view signals
   * It can be different from url(), for instance if we display a index.html
   */
  QString locationBarURL();

  /**
   * @return the servicetype this view is currently displaying
   */
  QString serviceType();

  /**
   * @return the servicetypes this view is capable to display
   */
  QStringList serviceTypes();

  /**
   * @return the part embedded into this view
   */
  DCOPRef part();

  /**
   * Enable/Disable the context popup menu for this view.
   */
  void enablePopupMenu( bool b );

private:

  KonqView * m_pView;

};

#endif

