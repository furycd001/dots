/* This file is part of the KDE project
   Copyright (C) 2001 Malte Starostik <malte.starostik@t-online.de>

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

#include <string.h>
#include <time.h>

#include <qbuffer.h>
#include <qfile.h>
#include <qimage.h>
#include <qmap.h>
#include <qtimer.h>

#include <kdatastream.h> // DO NOT REMOVE, otherwise bool marshalling breaks
#include <kglobal.h>
#include <kicontheme.h>
#include <kimageio.h>
#include <ksimpleconfig.h>
#include <kstddirs.h>
#include <kio/job.h>

#include "favicons.moc"

struct FaviconsModulePrivate
{
    virtual ~FaviconsModulePrivate() { delete config; }

    struct DownloadInfo
    {
        QString hostOrURL;
        bool isHost;
        QByteArray iconData;
    };
    QMap<KIO::Job *, DownloadInfo> downloads;
    QStringList failedDownloads;
    KSimpleConfig *config;
	QList<KIO::Job> killJobs;
};

FaviconsModule::FaviconsModule(const QCString &obj)
    : KDEDModule(obj)
{
    // create our favicons folder so that KIconLoader knows about it
    (void) KGlobal::dirs()->saveLocation( "icon", "favicons" );
    d = new FaviconsModulePrivate;
    d->config = new KSimpleConfig(locateLocal("data", "konqueror/faviconrc"));
	d->killJobs.setAutoDelete(true);
}

FaviconsModule::~FaviconsModule()
{
    delete d;
}

QString FaviconsModule::iconForURL(const KURL &url)
{
    if (url.host().isEmpty())
        return QString::null;

    QString iconURL = d->config->readEntry(simplifyURL(url));
    if (!iconURL.isEmpty())
        return iconNameFromURL(iconURL);

    QString icon = "favicons/" + url.host();
    if (!locate("icon", icon + ".png").isEmpty())
        return icon;

    return QString::null;
}

QString FaviconsModule::simplifyURL(const KURL &url)
{
    // splat any = in the URL so it can be safely used as a config key
    QString result = url.host() + url.path();
    for (unsigned int i = 0; i < result.length(); ++i)
        if (result[i] == '=')
            result[i] = '_';

    return result;
}

QString FaviconsModule::iconNameFromURL(const KURL &iconURL)
{
    QString result = iconURL.host() + iconURL.path();
    // splat = and / so it can be safely used as a file name
    for (unsigned int i = 0; i < result.length(); ++i)
        if (result[i] == '=' || result[i] == '/')
            result[i] = '_';

    QString ext = result.right(4);
    if (ext == ".ico" || ext == ".png" || ext == ".xpm")
        result.remove(result.length() - 4, 4);

    return "favicons/" + result;
}

bool FaviconsModule::isIconOld(const QString &icon)
{
    struct stat st;
    if (stat(QFile::encodeName(icon), &st) != 0)
        return true; // Trigger a new download on error

    return (time(0) - st.st_mtime) > 604800; // arbitrary value (one week)
}

void FaviconsModule::setIconForURL(const KURL &url, const KURL &iconURL)
{
    // Ignore pages that explicitly specify /favicon.ico, like kmail.kde.org
    if (iconURL.host() == url.host() && iconURL.path() == "/favicon.ico")
        return;

    QString iconFile = locate("icon", iconNameFromURL(iconURL) + ".png");
    if (!(iconFile.isEmpty() || isIconOld(iconFile)))
        return;

    startDownload(simplifyURL(url), false, iconURL);
}

void FaviconsModule::downloadHostIcon(const KURL &url)
{
    QString iconFile = locate("icon", "favicons/" + url.host() + ".png");
    if (!(iconFile.isEmpty() || isIconOld(iconFile)))
        return;

    KURL iconURL(url, "/favicon.ico");
    startDownload(url.host(), true, iconURL);
}

void FaviconsModule::startDownload(const QString &hostOrURL, bool isHost, const KURL &iconURL)
{
    if (d->failedDownloads.contains(iconURL.url()))
        return;

    KIO::MetaData md;
    md.insert("ssl_militant", "TRUE");
    md.insert("ssl_no_client_cert", "TRUE");
    KIO::TransferJob *job = KIO::get(iconURL, false, false);
    job->addMetaData(md);
    connect(job, SIGNAL(data(KIO::Job *, const QByteArray &)), SLOT(slotData(KIO::Job *, const QByteArray &)));
    connect(job, SIGNAL(result(KIO::Job *)), SLOT(slotResult(KIO::Job *)));
    connect(job, SIGNAL(infoMessage(KIO::Job *, const QString &)), SLOT(slotInfoMessage(KIO::Job *, const QString &)));
    FaviconsModulePrivate::DownloadInfo download;
    download.hostOrURL = hostOrURL;
    download.isHost = isHost;
    d->downloads.insert(job, download);
}

void FaviconsModule::slotData(KIO::Job *job, const QByteArray &data)
{
    FaviconsModulePrivate::DownloadInfo &download = d->downloads[job];
    unsigned int oldSize = download.iconData.size();
    if (oldSize > 0x10000)
    {
        d->killJobs.append(job);
        QTimer::singleShot(0, this, SLOT(slotKill()));
    }
    download.iconData.resize(oldSize + data.size());
    memcpy(download.iconData.data() + oldSize, data.data(), data.size());
}

void FaviconsModule::slotResult(KIO::Job *job)
{
    FaviconsModulePrivate::DownloadInfo download = d->downloads[job];
    d->downloads.remove(job);
    QString iconURL = static_cast<KIO::TransferJob *>(job)->url().url();
    QString iconName;
    if (!job->error())
    {
        QBuffer buffer(download.iconData);
        buffer.open(IO_ReadOnly);
        QImageIO io;
        io.setIODevice(&buffer);
        io.setParameters("16");
        // Check here too, the job might have had no error, but the downloaded
        // file contains just a 404 message sent with a 200 status.
        // microsoft.com does that... (malte)
        if (io.read())
        {
            // Some sites have nasty 32x32 icons, according to the MS docs
            // IE ignores them, well, we scale them, otherwise the location
            // combo / menu will look quite ugly
            if (io.image().width() != KIcon::SizeSmall || io.image().height() != KIcon::SizeSmall)
                io.setImage(io.image().smoothScale(KIcon::SizeSmall, KIcon::SizeSmall));

            if (download.isHost)
                iconName = "favicons/" + download.hostOrURL;
            else
                iconName = iconNameFromURL(iconURL);

            io.setIODevice(0);
            io.setFileName(locateLocal("icon", iconName + ".png"));
            io.setFormat("PNG");
            if (!io.write())
                iconName = QString::null;
            else if (!download.isHost)
                d->config->writeEntry(download.hostOrURL, iconURL);
        }
    }
    if (iconName.isEmpty())
        d->failedDownloads.append(iconURL);

    QByteArray data;
    QDataStream stream( data, IO_WriteOnly );
    stream << download.isHost << download.hostOrURL << iconName;
    emitDCOPSignal("iconChanged(bool,QString,QString)", data);
}

void FaviconsModule::slotInfoMessage(KIO::Job *job, const QString &msg)
{
  FaviconsModulePrivate::DownloadInfo download = d->downloads[job];
  QString iconURL = static_cast<KIO::TransferJob *>(job)->url().url();
  QByteArray data;
  QDataStream stream( data, IO_WriteOnly );
  stream << iconURL << msg;
  emitDCOPSignal("infoMessage(KURL,QString)", data);
}

void FaviconsModule::slotKill()
{
    d->killJobs.clear();
}

extern "C" {
    KDEDModule *create_favicons(const QCString &obj)
    {
        KImageIO::registerFormats();
        return new FaviconsModule(obj);
    }
};

// vim: ts=4 sw=4 et
