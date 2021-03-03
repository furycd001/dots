/* vi: ts=8 sts=4 sw=4
 *
 * $Id: bgmanager.cc,v 1.60.2.1 2001/10/22 21:58:11 waba Exp $
 *
 * This file is part of the KDE project, module kdesktop.
 * Copyright (C) 1999,2000 Geert Jansen <jansen@kde.org>
 *
 * You can Freely distribute this program under the GNU General Public
 * License. See the file "COPYING" for the exact licensing terms.
 */

#include <assert.h>

#include <qwidget.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qdragobject.h>
#include <qtimer.h>
#include <qwindowdefs.h>
#include <qlabel.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qpainter.h>
#include <qscrollview.h>
#include <qarray.h>

#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kconfig.h>
#include <kprocess.h>
#include <kwin.h>
#include <kapp.h>
#include <kdebug.h>
#include <kipc.h>
#include <kpixmap.h>
#include <kpopupmenu.h>
#include <kwinmodule.h>

#include "bgrender.h"
#include "bgmanager.h"
#include "bgdefaults.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#ifndef None
#define None 0L
#endif

#include "pixmapserver.h"

template class QVector<KBackgroundRenderer>;
template class QVector<KBackgroundCacheEntry>;
template class QArray<int>;

/**** KBackgroundManager ****/

KBackgroundManager::KBackgroundManager(QWidget *desktop, KWinModule* kwinModule)
    : DCOPObject("KBackgroundIface")
{
    m_bBgInitDone = false;
    if (desktop == 0L)
        m_pDesktop = QApplication::desktop();
    else
        m_pDesktop = desktop;

    m_X = m_pDesktop->width();
    m_Y = m_pDesktop->height();

    // Preallocate 4 desktops, higher ones are allocated when needed.
    m_Renderer.resize( 4 );
    m_Cache.resize( 4 );

    m_Serial = 0; m_Hash = 0;
    m_pConfig = KGlobal::config();
    m_bExport = m_bCommon = m_bInit = false;
    m_pKwinmodule = kwinModule;
    m_pPixmapServer = new KPixmapServer();

    for (unsigned i=0; i<m_Renderer.size(); i++)
    {
	m_Cache.insert(i, new KBackgroundCacheEntry);
        m_Cache[i]->pixmap = 0L;
        m_Cache[i]->hash = 0;
        m_Cache[i]->exp_from = -1;
        m_Renderer.insert (i, new KBackgroundRenderer(i,m_pConfig));
        connect(m_Renderer[i], SIGNAL(imageDone(int)), SLOT(slotImageDone(int)));
    }

    configure();

    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), SLOT(slotTimeout()));
    m_pTimer->start( 60000 );

    connect(m_pKwinmodule, SIGNAL(currentDesktopChanged(int)),
	    SLOT(slotChangeDesktop(int)));
    connect(m_pKwinmodule, SIGNAL(numberOfDesktopsChanged(int)),
	    SLOT(slotChangeNumberOfDesktops(int)));
}


KBackgroundManager::~KBackgroundManager()
{
    for (unsigned i=0; i<m_Renderer.size(); i++)
        delete m_Renderer[i];

    //delete m_pConfig; Very bad idea, this is KGlobal::config !
    delete m_pPixmapServer;
    delete m_pTimer;

    if (m_bExport)
	return;

	// clear the Esetroot properties, as the pixmaps they refer to are going away...
	 Atom prop_root, prop_esetroot;
	 prop_root = XInternAtom(qt_xdisplay(), "_XROOTPMAP_ID", True);
	 prop_esetroot = XInternAtom(qt_xdisplay(), "ESETROOT_PMAP_ID", True);
	 if (prop_root != None || prop_esetroot != None) // if they exist, we need to cleanup so GTK apps don't crash
	 {
		 XDeleteProperty(qt_xdisplay(), qt_xrootwin(), prop_root);
		 XDeleteProperty(qt_xdisplay(), qt_xrootwin(), prop_esetroot);
	 }

    for (unsigned i=0; i<m_Cache.size(); i++)
    {
	if (m_Cache[i]->pixmap)
	    delete m_Cache[i]->pixmap;
    }
}


void KBackgroundManager::applyExport(bool exp)
{
    if (exp == m_bExport)
	return;

    // If export mode changed from true -> false, remove all shared pixmaps.
    // If it changed false -> true force a redraw because the current screen
    // image might not have an associated pixmap in the cache.
    if (!exp)
    {
	for (unsigned i=0; i<m_Cache.size(); i++)
	    removeCache(i);
    } else
	m_Hash = 0;

    m_bExport = exp;
}


void KBackgroundManager::applyCommon(bool common)
{
    if (common == m_bCommon)
	return;
    m_bCommon = common;

    // If common changed from false -> true, remove all cache entries, except
    // at index 0 if exports are on.
    if (m_bCommon)
    {
	if (!m_bExport)
	    removeCache(0);
	for (unsigned i=1; i<m_Cache.size(); i++)
	    removeCache(i);
    }
}


void KBackgroundManager::applyCache(bool limit, int size)
{
    m_bLimitCache = limit;
    m_CacheLimit = size;
    freeCache(0);
}


/*
 * Call this when the configuration has changed.
 * This method is exported with DCOP.
 */
void KBackgroundManager::configure()
{
    // Global settings
    m_pConfig->reparseConfiguration();

    // Read individual settings
    KBackgroundRenderer *r;
    for (unsigned i=0; i<m_Renderer.size(); i++)
    {
        r = m_Renderer[i];
        int ohash = r->hash();
        r->load(i,false);
        if ((r->hash() != ohash))
            removeCache(i);
    }

    m_pConfig->setGroup("Background Common");
    applyCommon(m_pConfig->readBoolEntry("CommonDesktop", _defCommon));

    bool limit = m_pConfig->readBoolEntry("LimitCache", _defLimitCache);
    int size = m_pConfig->readNumEntry("CacheSize", _defCacheSize) * 1024;
    applyCache(limit, size);

    // Repaint desktop
    slotChangeDesktop(0);
}


int KBackgroundManager::realDesktop()
{
    int desk = m_pKwinmodule->currentDesktop();
    if (desk) desk--;
    return desk;
}


int KBackgroundManager::effectiveDesktop()
{
    return m_bCommon ? 0 : realDesktop();
}


/*
 * Number of desktops changed
 */
void KBackgroundManager::slotChangeNumberOfDesktops(int num)
{
    if (m_Renderer.size() == (unsigned) num)
	return;

    if (m_Renderer.size() > (unsigned) num)
    {
	for (unsigned i=num; i<m_Renderer.size(); i++)
	{
	    if (m_Renderer[i]->isActive())
		m_Renderer[i]->stop();
	    delete m_Renderer[i];
	    removeCache(i);
	}
	for (unsigned i=num; i<m_Renderer.size(); i++)
	    delete m_Cache[i];
	m_Renderer.resize(num);
	m_Cache.resize(num);
    } else
    {
	// allocate new renderers and caches
	int oldsz = m_Renderer.size();
	m_Renderer.resize(num);
	m_Cache.resize(num);
	for (int i=oldsz; i<num; i++)
	{
	    m_Cache.insert(i, new KBackgroundCacheEntry);
	    m_Cache[i]->pixmap = 0L;
	    m_Cache[i]->hash = 0;
	    m_Cache[i]->exp_from = -1;
	    m_Renderer.insert(i, new KBackgroundRenderer(i, m_pConfig));
	    connect(m_Renderer[i], SIGNAL(imageDone(int)), SLOT(slotImageDone(int)));
	}
    }
}

/*
 * Call this when the desktop has been changed.
 * Desk is in KWin convention: [1..desks], instead of [0..desks-1].
 * 0 repaints the current desktop.
 */
void KBackgroundManager::slotChangeDesktop(int desk)
{
    if (desk == 0)
	desk = realDesktop();
    else
	desk--;

    // Lazy initialisation of # of desktops
    if ((unsigned) desk >= m_Renderer.size())
	slotChangeNumberOfDesktops( m_pKwinmodule->numberOfDesktops() );

    int edesk = effectiveDesktop();
    m_Serial++;

    // If the background is the same: do nothing
    if (m_Hash == m_Renderer[edesk]->hash())
    {
	exportBackground(m_Current, desk);
        return;
    }

    // If we have the background already rendered: set it
    for (unsigned i=0; i<m_Cache.size(); i++)
    {
	if (!m_Cache[i]->pixmap)
	    continue;
	if (m_Cache[i]->hash != m_Renderer[edesk]->hash())
	    continue;
//        kdDebug() << "slotChangeDesktop i=" << i << endl;
	setPixmap(m_Cache[i]->pixmap, m_Cache[i]->hash, i);
	m_Cache[i]->atime = m_Serial;
	exportBackground(i, desk);
	return;
    }

    // Do we have this or an indentical config already running?
    for (unsigned i=0; i<m_Renderer.size(); i++)
    {
        if ((m_Renderer[i]->hash() == m_Renderer[edesk]->hash()) &&
            (m_Renderer[i]->isActive()))
            return;
    }

    renderBackground(edesk);
}


/*
 * Share a desktop pixmap.
 */
void KBackgroundManager::exportBackground(int pixmap, int desk)
{
    if (!m_bExport || (m_Cache[desk]->exp_from == pixmap))
        return;

    m_Cache[desk]->exp_from = pixmap;
    m_pPixmapServer->add(QString("DESKTOP%1").arg(desk+1),
	    m_Cache[pixmap]->pixmap);
    KIPC::sendMessageAll(KIPC::BackgroundChanged, desk+1);
}


/*
 * Paint the pixmap to the root window.
 */
void KBackgroundManager::setPixmap(KPixmap *pm, int hash, int desk)
{
    m_pDesktop->setBackgroundPixmap(*pm);
    m_pDesktop->repaint();
    if ( m_pDesktop->inherits( "QScrollView" ) ) {
	 XSetWindowBackgroundPixmap( qt_xdisplay(), ((QScrollView*)m_pDesktop )->viewport()->winId(), ParentRelative );
    }

// and export it via Esetroot-style for gnome/GTK apps to share in the pretties
	 Atom prop_root, prop_esetroot;
	 prop_root = XInternAtom(qt_xdisplay(), "_XROOTPMAP_ID", False);
	 prop_esetroot = XInternAtom(qt_xdisplay(), "ESETROOT_PMAP_ID", False);
	 Pixmap bgPm = pm->handle(); // fetch the actual X handle to it
	 kdDebug() << "Esetroot compat:  setting pixmap to " << bgPm << endl;

	 //Esetroot does a bunch of work to track down and kill pixmaps that are old here...
	 // I don't think I need to, as qt should be the one owning the pixmap

	 /* The call above should have created it.  If that failed, oh well - Esetroot is just cake anyway. */
	 if (prop_root != None && prop_esetroot != None) {
		 XChangeProperty(qt_xdisplay(), qt_xrootwin(), prop_root, XA_PIXMAP, 32, PropModeReplace,
				 (unsigned char *) &bgPm, 1);
		 XChangeProperty(qt_xdisplay(), qt_xrootwin(), prop_esetroot, XA_PIXMAP, 32, PropModeReplace,
				 (unsigned char *) &bgPm, 1);
	 } else {
		 kdDebug() << "Esetroot compat:  creation of pixmap property failed." << endl;
	 }

    m_Hash = hash;
    m_Current = desk;
}


/*
 * Start the render of a desktop background.
 */
void KBackgroundManager::renderBackground(int desk)
{
    KBackgroundRenderer *r = m_Renderer[desk];
    if (r->isActive())
    {
        kdDebug() << "renderer " << desk << " already active" << endl;
        return;
    }

    // Allow tiles!
    r->setTile(true);
    r->start();
}


/*
 * This slot is called when a renderer is done.
 */
void KBackgroundManager::slotImageDone(int desk)
{
    KPixmap *pm = new KPixmap();
    KBackgroundRenderer *r = m_Renderer[desk];

    pm->convertFromImage(*r->image());
    r->cleanup();

    // If current: paint it
    bool current = (r->hash() == m_Renderer[effectiveDesktop()]->hash());
    if (current)
    {
        setPixmap(pm, r->hash(), desk);
        if (!m_bBgInitDone)
        {
            m_bBgInitDone = true;
            emit initDone();
        }
    }
    if (m_bExport || !m_bCommon)
	addCache(pm, r->hash(), desk);
    else
        delete pm;

    if (current)
        exportBackground(desk, realDesktop());
}


/*
 * Size in bytes of a QPixmap. For use in the pixmap cache.
 */
int KBackgroundManager::pixmapSize(QPixmap *pm)
{
    return (pm->width() * pm->height()) * ((pm->depth() + 7) / 8);
}


/*
 * Total size of the pixmap cache.
 */
int KBackgroundManager::cacheSize()
{
    int total = 0;
    for (unsigned i=0; i<m_Cache.size(); i++)
    {
        if (m_Cache[i]->pixmap)
            total += pixmapSize(m_Cache[i]->pixmap);
    }
    return total;
}


/*
 * Remove an entry from the pixmap cache.
 */
void KBackgroundManager::removeCache(int desk)
{
    if (m_bExport)
	m_pPixmapServer->remove(QString("DESKTOP%1").arg(desk+1));
    else
        delete m_Cache[desk]->pixmap;
    m_Cache[desk]->pixmap = 0L;
    m_Cache[desk]->hash = 0;
    m_Cache[desk]->exp_from = -1;
    m_Cache[desk]->atime = 0;

    // Remove cache entries pointing to the removed entry
    for (unsigned i=0; i<m_Cache.size(); i++)
    {
	if (m_Cache[i]->exp_from == desk)
	{
	    assert(m_bExport);
	    m_Cache[i]->exp_from = -1;
	    m_pPixmapServer->remove(QString("DESKTOP%1").arg(i+1));
	}
    }
}


/*
 * Try to free up to size bytes from the cache.
 */
bool KBackgroundManager::freeCache(int size)
{
    if (m_bExport || !m_bLimitCache)
	return true;

    // If it doesn't fit at all, return now.
    if (size > m_CacheLimit)
	return false;

    // If cache is too full, purge it (LRU)
    while (size+cacheSize() > m_CacheLimit)
    {
	int j, min;
	min = m_Serial+1; j = 0;
	for (unsigned i=0; i<m_Cache.size(); i++)
	{
	    if (m_Cache[i]->pixmap && (m_Cache[i]->atime < min))
	    {
		min = m_Cache[i]->atime;
		j = i;
	    }
	}
	removeCache(j);
    }
    return true;
}


/*
 * Try to add a pixmap to the pixmap cache. We don't use QPixmapCache here
 * because if we're exporting pixmaps, this needs special care.
 */
void KBackgroundManager::addCache(KPixmap *pm, int hash, int desk)
{
    if (m_Cache[desk]->pixmap)
	removeCache(desk);

    if (m_bLimitCache && !m_bExport && !freeCache(pixmapSize(pm)))
    {
	// pixmap does not fit in cache
	delete pm;
	return;
    }

    m_Cache[desk]->pixmap = pm;
    m_Cache[desk]->hash = hash;
    m_Cache[desk]->atime = m_Serial;
    m_Cache[desk]->exp_from = -1;
    exportBackground(desk, desk);
}

/*
 * Called every minute to check if we need to rerun a background program.
 * or change a wallpaper.
 */
void KBackgroundManager::slotTimeout()
{
    QArray<int> running(m_Renderer.size());
    running.fill(0);

    int NumDesks = m_Renderer.size();
    if (m_bCommon) 
       NumDesks = 1;

    for (unsigned i=0; i<NumDesks; i++)
    {
        KBackgroundRenderer *r = m_Renderer[i];
        bool change = false;

        if ((r->backgroundMode() == KBackgroundSettings::Program) &&
	    (r->KBackgroundProgram::needUpdate()) &&
            (!running.contains(r->hash()))
	   )
        {
	    r->KBackgroundProgram::update();
            change = true;
        }

        if (r->needWallpaperChange())
	{
            r->changeWallpaper();
            change = true;
        }

        if (change)
	{
	    running[i] = r->hash();
            r->start();
	}
    }
}


// DCOP exported
void KBackgroundManager::changeWallpaper()
{
    KBackgroundRenderer *r = m_Renderer[effectiveDesktop()];
    if (r->multiWallpaperMode() == KBackgroundSettings::NoMulti)
	return;

    r->changeWallpaper();
    slotChangeDesktop(0);
}

// DCOP exported
void KBackgroundManager::setExport(int _export)
{
    kdDebug() << "KBackgroundManager enabling exports.\n";
    applyExport(_export);
    slotChangeDesktop(0);
}

// DCOP exported
void KBackgroundManager::setCommon(int common)
{
    applyCommon(common);
    m_pConfig->setGroup("Background Common");
    m_pConfig->writeEntry("CommonDesktop", m_bCommon);
    m_pConfig->sync();
    slotChangeDesktop(0);
}

// DCOP exported
void KBackgroundManager::setWallpaper(QString wallpaper, int mode)
{
    KBackgroundRenderer *r = m_Renderer[effectiveDesktop()];
    r->stop();
    r->setWallpaperMode(mode);
    r->setMultiWallpaperMode(KBackgroundSettings::NoMulti);
    r->setWallpaper(wallpaper);
    r->writeSettings();
    slotChangeDesktop(0);
}

void KBackgroundManager::setWallpaper(QString wallpaper)
{
    KBackgroundRenderer *r = m_Renderer[effectiveDesktop()];
    int mode = r->wallpaperMode();
    if (mode == KBackgroundSettings::NoWallpaper)
       mode = KBackgroundSettings::Tiled;
    setWallpaper(wallpaper, mode);   
}


// DCOP exported
void KBackgroundManager::setCache( int bLimit, int size )
{
    applyCache( bLimit, size*1024 );
    m_pConfig->setGroup( "Background Common" );
    m_pConfig->writeEntry( "LimitCache", (bool) bLimit );
    m_pConfig->writeEntry( "CacheSize", size );
    m_pConfig->sync();
}


void KBackgroundManager::repaintBackground()
{
    m_pDesktop->repaint();
}

void KBackgroundManager::setColor(const QColor & c, bool isColorA)
{
    KBackgroundRenderer *r = m_Renderer[effectiveDesktop()];
    r->stop();

    if (isColorA)
        r->setColorA(c);
    else
        r->setColorB(c);

    int mode = r->backgroundMode();
    if (mode == KBackgroundSettings::Program)
       mode = KBackgroundSettings::Flat;
       
    if (!isColorA && (mode == KBackgroundSettings::Flat))
       mode = KBackgroundSettings::VerticalGradient;
    r->setBackgroundMode(mode);

    r->writeSettings();
    slotChangeDesktop(0);
}

#include "bgmanager.moc"
