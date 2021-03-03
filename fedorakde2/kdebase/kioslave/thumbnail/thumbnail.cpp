/*  This file is part of the KDE libraries
    Copyright (C) 2000 Malte Starostik <malte@kde.org>
                  2000 Carsten Pfeiffer <pfeiffer@kde.org>

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

// $Id: thumbnail.cpp,v 1.18.2.1 2001/08/21 20:55:45 malte Exp $

#include <stdlib.h>
#include <unistd.h>
#ifdef __FreeBSD__
    #include <machine/param.h>
#endif
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <qpixmap.h>
#include <qpainter.h>
#include <qimage.h>
#include <qbuffer.h>

#include <kdatastream.h> // Do not remove, needed for correct bool serialization
#include <kurl.h>
#include <kapp.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kimageeffect.h>
#include <kmimetype.h>
#include <klibloader.h>
#include <kdebug.h>

#include "thumbnail.h"
#include "thumbcreator.h"

// Recognized metadata entries:
// mimeType     - the mime type of the file, used for the overlay icon if any
// width        - maximum width for the thumbnail
// height       - maximum height for the thumbnail
// iconSize     - the size of the overlay icon to use if any
// iconAlpha    - the transparency value used for icon overlays
// plugin       - the name of the plugin library to be used for thumbnail creation.
//                Provided by the application to save an addition KTrader
//                query here.
// shmid        - the shared memory segment id to write the image's data to.
//                The segment is assumed to provide enough space for a 32-bit
//                image sized width x height pixels.
//                If this is given, the data returned by the slave will be:
//                    int width
//                    int height
//                    int depth
//                Otherwise, the data returned is the image in PNG format.
 
using namespace KIO;

extern "C"
{
    int kdemain(int argc, char **argv);
}

int kdemain(int argc, char **argv)
{
    KApplication app(argc, argv, "kio_thumbnail", false, true);

    if (argc != 4)
    {
        kdError() << "Usage: kio_thumbnail protocol domain-socket1 domain-socket2" << endl;
        exit(-1);
    }

    ThumbnailProtocol slave(argv[2], argv[3]);
    slave.dispatchLoop();

    return 0;
}

ThumbnailProtocol::ThumbnailProtocol(const QCString &pool, const QCString &app)
    : SlaveBase("thumbnail", pool, app)
{
    m_creators.setAutoDelete(true);
    m_iconDict.setAutoDelete(true);
    m_iconSize = 0;
}

ThumbnailProtocol::~ThumbnailProtocol()
{
}

void ThumbnailProtocol::get(const KURL &url)
{
    m_mimeType = metaData("mimeType");
    if (m_mimeType.isEmpty())
    {
        error(KIO::ERR_INTERNAL, "No MIME Type specified.");
        return;
    }

    m_width = metaData("width").toInt();
    m_height = metaData("height").toInt();
    if (m_width <= 0 || m_height <= 0)
    {
        error(KIO::ERR_INTERNAL, "No or invalid size specified.");
        return;
    }

    int iconSize = metaData("iconSize").toInt();
    if (!iconSize)
        iconSize = KGlobal::iconLoader()->currentSize(KIcon::Desktop);
	if (iconSize != m_iconSize)
        m_iconDict.clear();
    m_iconSize = iconSize;

    m_iconAlpha = metaData("iconAlpha").toInt();
    if (m_iconAlpha)
        m_iconAlpha = (m_iconAlpha << 24) | 0xffffff;
    QString plugin = metaData("plugin");
    if (plugin.isEmpty())
    {
        error(KIO::ERR_INTERNAL, "No plugin specified.");
        return;
    }
    ThumbCreator *creator = m_creators[plugin];
    if (!creator)
    {
        // Don't use KLibFactory here, this is not a QObject and
        // neither is ThumbCreator
        KLibrary *library = KLibLoader::self()->library(plugin.latin1());
        if (library)
        {
            newCreator create = (newCreator)library->symbol("new_creator");
            if (create)
                creator = create();
        }
        if (!creator)
        {
            error(KIO::ERR_INTERNAL, "Cannot load ThumbCreator " + plugin);
            return;
        }
        m_creators.insert(plugin, creator);
    }

    QImage img;
    if (!creator->create(url.path(), m_width, m_height, img))
    {
        error(KIO::ERR_INTERNAL, "Cannot create thumbnail for " + url.path());
        return;
    }
    if (img.width() > m_width || img.height() > m_height)
    {
        double imgRatio = (double)img.height() / (double)img.width();
        if (imgRatio > (double)m_height / (double)m_width)
            img = img.smoothScale(QMAX((double)m_height / imgRatio, 1), m_height);
        else
            img = img.smoothScale(m_width, QMAX((double)m_width * imgRatio, 1));
    }
    ThumbCreator::Flags flags = creator->flags();

    if (flags & ThumbCreator::DrawFrame)
    {
        QPixmap pix;
        pix.convertFromImage(img);
        int x2 = pix.width() - 1;
        int y2 = pix.height() - 1;
        // paint a black rectangle around the "page"
        QPainter p;
        p.begin( &pix );
        p.setPen( QColor( 48, 48, 48 ));
        p.drawLine( x2, 0, x2, y2 );
        p.drawLine( 0, y2, x2, y2 );
        p.setPen( QColor( 215, 215, 215 ));
        p.drawLine( 0, 0, x2, 0 );
        p.drawLine( 0, 0, 0, y2 );
        p.end();
        img = pix.convertToImage();
    }

    if ((flags & ThumbCreator::BlendIcon) && KGlobal::iconLoader()->alphaBlending(KIcon::Desktop))
    {
        // blending the mimetype icon in
        QImage icon = getIcon();

        int x = img.width() - icon.width() - 4;
        x = QMAX( x, 0 );
        int y = img.height() - icon.height() - 6;
        y = QMAX( y, 0 );
        KImageEffect::blendOnLower( x, y, icon, img );
    }

    QByteArray imgData;
    QDataStream stream( imgData, IO_WriteOnly );
	QString shmid = metaData("shmid");
	if (shmid.isEmpty())
		stream << img;
	else
	{
		void *shmaddr = shmat(shmid.toInt(), 0, 0);
		if (shmaddr == (void *)-1)
		{
			error(KIO::ERR_INTERNAL, "Failed to attach to shared memory segment " + shmid);
			return;
		}
		if (img.width() * img.height() > m_width * m_height)
		{
			error(KIO::ERR_INTERNAL, "Image is too big for the shared memory segment");
			shmdt((char*)shmaddr);
			return;
		}
		stream << img.width() << img.height() << img.depth()
               << img.hasAlphaBuffer();
		memcpy(shmaddr, img.bits(), img.numBytes());
		shmdt((char*)shmaddr);
	}
    data(imgData);
    finished();
}

const QImage& ThumbnailProtocol::getIcon()
{
    QImage* icon = m_iconDict.find(m_mimeType);
    if ( !icon ) // generate it!
    {
        icon = new QImage( KMimeType::mimeType(m_mimeType)->pixmap( KIcon::Desktop, m_iconSize ).convertToImage() );
        icon->setAlphaBuffer( true );

        int w = icon->width();
        int h = icon->height();
        for ( int y = 0; y < h; y++ )
        {
            QRgb *line = (QRgb *) icon->scanLine( y );
            for ( int x = 0; x < w; x++ )
            line[x] &= m_iconAlpha; // transparency
        }

        m_iconDict.insert( m_mimeType, icon );
    }

    return *icon;
}

