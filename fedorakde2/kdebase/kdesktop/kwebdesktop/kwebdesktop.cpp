/* This file is part of the KDE project
   Copyright (C) 2000 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

// Idea by Gael Duval
// Implementation by David Faure

#include "kwebdesktop.h"

#include <kapp.h>
#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kurifilter.h>
#include <kio/job.h>

#include <qfile.h>
#include <qscrollview.h>

#include "kwebdesktop.moc"

static KCmdLineOptions options[] =
{
  { "+width", I18N_NOOP("Width of the image to create"), 0 },
  { "+height", I18N_NOOP("Height of the image to create"), 0 },
  { "+file", I18N_NOOP("Filename where to dump the output in png format."), 0 },
  { "+[URL]", I18N_NOOP("URL to open (if not specified, it is read from kwebdesktoprc)."), 0 },
  { 0, 0, 0 }
};

KWebDesktopRun::KWebDesktopRun( KHTMLPart * part, const KURL & url )
    : m_part(part), m_url(url)
{
    kdDebug() << "KWebDesktopRun::KWebDesktopRun starting get" << endl;
    KIO::Job * job = KIO::get(m_url, false, false);
    connect( job, SIGNAL( result( KIO::Job *)),
             this, SLOT( slotFinished(KIO::Job *)));
    connect( job, SIGNAL( mimetype( KIO::Job *, const QString &)),
             this, SLOT( slotMimetype(KIO::Job *, const QString &)));
}

void KWebDesktopRun::slotMimetype( KIO::Job *job, const QString &_type )
{
    KIO::SimpleJob *sjob = static_cast<KIO::SimpleJob *>(job);
    // Update our URL in case of a redirection
    m_url = sjob->url();
    QString type = _type; // necessary copy if we plan to use it
    sjob->putOnHold();
    // What to do if type is not text/html ??
    kdDebug() << "slotMimetype : " << type << endl;

    // Now open the URL in the part
    m_part->openURL( m_url );
}

void KWebDesktopRun::slotFinished( KIO::Job * job )
{
    // The whole point of all this is to abort silently on error
    if (job->error())
    {
        kdDebug() << job->errorString() << endl;
        kapp->quit();
    }
}


int main( int argc, char **argv )
{
    KAboutData data( "kwebdesktop", I18N_NOOP("KDE Web Desktop"),
                     "0.1",
                     I18N_NOOP("Displays an HTML page as the background of the desktop"),
                     KAboutData::License_GPL,
                     "(c) 2000, David Faure <faure@kde.org>" );
    data.addAuthor( "David Faure", I18N_NOOP("developer and maintainer"), "faure@kde.org" );

    KCmdLineArgs::init( argc, argv, &data );

    KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

    KApplication app;

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if ( args->count() != 3 && args->count() != 4 )
    {
       args->usage();
       return 1;
    }
    int width = QCString(args->arg(0)).toInt();
    int height = QCString(args->arg(1)).toInt();
    QCString imageFile = args->arg(2);
    QString url;
    if (args->count() == 4)
        url = QString::fromLocal8Bit(args->arg(3));

    KHTMLPart *part = new KHTMLPart;
    part->widget()->resize(width,height);

    part->enableJScript(false);
    part->enableJava(false);

    ((QScrollView *)part->widget())->setHScrollBarMode( QScrollView::AlwaysOff );
    ((QScrollView *)part->widget())->setVScrollBarMode( QScrollView::AlwaysOff );

    KWebDesktop *webDesktop = new KWebDesktop( part, imageFile );
    QObject::connect( part, SIGNAL( canceled(const QString &) ),
                      webDesktop, SLOT( slotCompleted() ) );
    QObject::connect( part, SIGNAL( completed() ),
                      webDesktop, SLOT( slotCompleted() ) );

    KConfig config( "kwebdesktoprc" );
    config.setGroup("Settings");
    if (url.isEmpty())
        url = config.readEntry("URL", "http://www.kde.org/");
    // Apply uri filter
    KURIFilterData uridata = url;
    KURIFilter::self()->filterURI( uridata );
    KURL u = uridata.uri();

    // Now start getting, to ensure mimetype and possible connection
    KWebDesktopRun * run = new KWebDesktopRun( part, u );

    int ret = app.exec();

    KIO::SimpleJob::removeOnHold(); // Kill any slave that was put on hold.
    delete part;
    delete run;
    //khtml::Cache::clear();

    return ret;
}

void KWebDesktop::slotCompleted()
{
    kdDebug() << "KWebDesktop::slotCompleted" << endl;
    // Dump image to m_imageFile
    QPixmap snapshot = QPixmap::grabWidget( m_part->widget() );
    snapshot.save( m_imageFile, "PNG" );
    // And terminate the app.
    kapp->quit();
}
