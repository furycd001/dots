/* This file is part of the KDE Project
   Copyright (c) 2001 Malte Starostik <malte.starostik@t-online.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

// $Id: favicons.h,v 1.3 2001/06/20 05:19:51 malte Exp $

#ifndef _FAVICONS_H_
#define _FAVICONS_H_

#include <kdedmodule.h>
#include <kurl.h>

namespace KIO { class Job; }

/**
 * KDED Module to handle shortcut icons ("favicons")
 * FaviconsModule implements a KDED Module that handles the association of
 * URLs and hosts with shortcut icons and the icons' downloads in a central
 * place.
 *
 * After a successful download, the DCOP signal iconChanged() is emitted.
 * It has the signature void iconChanged(bool, QString, QString);
 * The first parameter is true if the icon is a "host" icon, that is it is
 * the default icon for all URLs on the given host. In this case, the
 * second parameter is a host name, otherwise the second parameter is the
 * URL which is associated with the icon. The third parameter is the
 * @ref KIconLoader friendly name of the downloaded icon, the same as
 * @ref iconForURL will from now on return for any matching URL.
 *
 * @short KDED Module for favicons
 * @author Malte Starostik <malte.starostik@t-online.de>
 * @version $Id: favicons.h,v 1.3 2001/06/20 05:19:51 malte Exp $
 */
class FaviconsModule : public KDEDModule
{
    Q_OBJECT
    K_DCOP
public:
    FaviconsModule(const QCString &obj);
    virtual ~FaviconsModule();

k_dcop:
    /**
     * Looks up an icon name for a given URL. This function does not
     * initiate any download. If no icon for the URL or its host has
     * been downloaded yet, QString::null is returned.
     *
     * @param url the URL for which the icon is queried
     * @return the icon name suitable to pass to @ref KIconLoader or
     *         QString::null if no icon for this URL was found.
     */
    QString iconForURL(const KURL &url);
    /**
     * Assiciates an icon with the given URL. If the icon was not
     * downloaded before or the downloaded was too long ago, a
     * download attempt will be started and the iconChanged() DCOP
     * signal is emitted after the download finished successfully.
     *
     * @param url the URL which will be associated with the icon
     * @param iconURL the URL of the icon to be downloaded
     */
    ASYNC setIconForURL(const KURL &url, const KURL &iconURL);
    /**
     * Downloads the icon for a given host if it was not downloaded before
     * or the download was too long ago. If the download finishes
     * successfully, the iconChanged() DCOP signal is emitted.
     *
     * @param url any URL on the host for which the icon is to be downloaded
     */
    ASYNC downloadHostIcon(const KURL &url);

private:
    void startDownload(const QString &, bool, const KURL &);
    QString simplifyURL(const KURL &);
    QString iconNameFromURL(const KURL &);
    bool isIconOld(const QString &);

private slots:
    void slotData(KIO::Job *, const QByteArray &);
    void slotResult(KIO::Job *);
    void slotInfoMessage(KIO::Job *, const QString &);
    void slotKill();

private:
    struct FaviconsModulePrivate *d;
};

#endif

// vim: ts=4 sw=4 et
