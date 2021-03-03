/* vi: ts=8 sts=4 sw=4
 *
 * $Id: bgrender.cc,v 1.13.2.1 2001/08/26 19:01:01 pfeiffer Exp $
 *
 * This file is part of the KDE project, module kdesktop.
 * Copyright (C) 1999 Geert Jansen <g.t.jansen@stud.tue.nl>
 *
 * You can Freely distribute this program under the GNU Library General
 * Public License. See the file "COPYING.LIB" for the exact licensing terms.
 */

#include <time.h>
#include <stdlib.h>

#include <kconfig.h>

#include <qstring.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qobject.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qtimer.h>

#include <kapp.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kstddirs.h>
#include <kpixmap.h>
#include <kpixmapeffect.h>
#include <kimageeffect.h>
#include <kprocess.h>
#include <kimageio.h>
#include <ktempfile.h>

#include <bgdefaults.h>
#include <bgsettings.h>
#include <bgrender.h>

#include <X11/Xlib.h>


/**** KBackgroundRenderer ****/


KBackgroundRenderer::KBackgroundRenderer(int desk, KConfig *config)
    : KBackgroundSettings(desk, config)
{
    m_State = 0;

    if (! config) {
	int screen_number = 0;
	if (qt_xdisplay())
	    screen_number = DefaultScreen(qt_xdisplay());
	QCString configname;
	if (screen_number == 0)
	    configname = "kdesktoprc";
	else
	    configname.sprintf("kdesktop-screen-%drc", screen_number);

	m_pConfig = new KConfig(configname);
    } else
	m_pConfig = config;

    m_pDirs = KGlobal::dirs();
    m_rSize = m_Size = QApplication::desktop()->size();
    m_pBackground = 0L; m_pImage = 0L;
    m_pProc = 0L;
    m_Tempfile = 0L;
    m_bPreview = false;
    m_bTile = false;

    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), SLOT(render()));
}


KBackgroundRenderer::~KBackgroundRenderer()
{
    cleanup();
    delete m_Tempfile; m_Tempfile = 0;
}


void KBackgroundRenderer::tile(QImage *dest, QRect rect, QImage *src)
{
    rect &= dest->rect();

    int x, y;
    int h = rect.height(), w = rect.width();
    int offx = rect.x(), offy = rect.y();
    int sw = src->width(), sh = src->height();

    for (y=offy; y<offy+h; y++)
	for (x=offx; x<offx+w; x++)
	    dest->setPixel(x, y, src->pixel(x%sw, y%sh));
}


/*
 * Build a command line to run the program.
 */

QString KBackgroundRenderer::buildCommand()
{
    QString num;
    int pos = 0;

    QString cmd;
    if (m_bPreview)
        cmd = previewCommand();
    else
        cmd = command();

    if (cmd.isEmpty())
	return QString();

    while ((pos = cmd.find('%', pos)) != -1) {

        if (pos == (int) (cmd.length() - 1))
            break;

        switch (cmd.at(pos+1).latin1()) {
        case 'f':
            createTempFile();
            cmd.replace(pos, 2, m_Tempfile->name());
            pos += m_Tempfile->name().length() - 2;
            break;

        case 'x':
            num.setNum(m_Size.width());
            cmd.replace(pos, 2, num);
            pos += num.length() - 2;
            break;

        case 'y':
            num.setNum(m_Size.height());
            cmd.replace(pos, 2, num);
            pos += num.length() - 2;
            break;

        case '%':
            cmd.replace(pos, 2, "%");
            pos--;
            break;
        }

    }
    return cmd;
}


/*
 * Create a background tile. If the background mode is `Program',
 * this is asynchronous.
 */
int KBackgroundRenderer::doBackground(bool quit)
{
    if (m_State & BackgroundDone)
        return Done;

    int bgmode = backgroundMode();

    if (quit) {
	if (bgmode == Program && m_pProc)
	    m_pProc->kill();
        return Done;
    }

    int retval = Done;
    QString file;

    switch (bgmode) {

    case Flat:
	m_pBackground->create(10, 10, 32);
        m_pBackground->fill(colorA().rgb());
        break;

    case Pattern:
    {
        if (pattern().isEmpty())
            break;
        file = m_pDirs->findResource("dtop_pattern", pattern());
        if (file.isEmpty())
            break;

	m_pBackground->load(file);
	if (m_pBackground->isNull())
	    break;
	int w = m_pBackground->width();
	int h = m_pBackground->height();
	if ((w > m_Size.width()) || (h > m_Size.height())) {
	    w = QMIN(w, m_Size.width());
	    h = QMIN(h, m_Size.height());
	    *m_pBackground = m_pBackground->copy(0, 0, w, h);
	}
	KImageEffect::flatten(*m_pBackground, colorA(), colorB(), 0);
	break;
    }
    case Program:
        if (m_State & BackgroundStarted)
            break;
        m_State |= BackgroundStarted;
        createTempFile();        

	file = buildCommand();
	if (file.isEmpty())
	    break;

        m_pProc = new KShellProcess;
        *m_pProc << file;
        connect(m_pProc, SIGNAL(processExited(KProcess *)),
                SLOT(slotBackgroundDone(KProcess *)));
        m_pProc->start(KShellProcess::NotifyOnExit);
        retval = Wait;
        break;

    case HorizontalGradient:
    {
	QSize size = m_Size;
	size.setHeight(30);
	*m_pBackground = KImageEffect::gradient(size, colorA(), colorB(),
		KImageEffect::HorizontalGradient, 0);
        break;
    }
    case VerticalGradient:
    {
	QSize size = m_Size;
	size.setWidth(30);
        *m_pBackground = KImageEffect::gradient(size, colorA(), colorB(),
		KImageEffect::VerticalGradient, 0);
        break;
    }
    case PyramidGradient:
        *m_pBackground = KImageEffect::gradient(m_Size, colorA(), colorB(),
		KImageEffect::PyramidGradient, 0);
        break;

    case PipeCrossGradient:
        *m_pBackground = KImageEffect::gradient(m_Size, colorA(), colorB(),
		KImageEffect::PipeCrossGradient, 0);
        break;

    case EllipticGradient:
        *m_pBackground = KImageEffect::gradient(m_Size, colorA(), colorB(),
		KImageEffect::EllipticGradient, 0);
        break;
    }

    if (retval == Done)
        m_State |= BackgroundDone;

    return retval;
}


int KBackgroundRenderer::doWallpaper(bool quit)
{
    if (m_State & WallpaperDone)
        return Done;

    if (quit)
        // currently no asynch. wallpapers
        return Done;

    int wpmode = wallpaperMode();
    int blmode = blendMode();

    bool bTile = m_bTile;
    if (wpmode != NoWallpaper)
	bTile = false;

    QImage wp;
    if (wpmode != NoWallpaper) {
	if (currentWallpaper().isEmpty()) {
	    wpmode = NoWallpaper;
	    goto wp_out;
	}
	QString file = m_pDirs->findResource("wallpaper", currentWallpaper());
	if (file.isEmpty()) {
	    wpmode = NoWallpaper;
	    goto wp_out;
	}

	wp.load(file);
	if (wp.isNull()) {
	    wpmode = NoWallpaper;
	    goto wp_out;
	}
	wp = wp.convertDepth(32, DiffuseAlphaDither);

	// If we're previewing, scale the wallpaper down to make the preview
	// look more like the real desktop.
	if (m_bPreview) {
	    int xs = wp.width() * m_Size.width() / m_rSize.width();
	    int ys = wp.height() * m_Size.height() / m_rSize.height();
	    if ((xs < 1) || (ys < 1))
	    {
	       xs = ys = 1;
	    }
	    wp = wp.smoothScale(xs, ys);
	}
    }
wp_out:

    if (m_pBackground->isNull()) {
	m_pBackground->create(10, 10, 32);
	m_pBackground->fill(colorA().rgb());
    }

    int retval = Done;

    int w = m_Size.width();	// desktop width/height
    int h = m_Size.height();

    int ww = wp.width();	// wallpaper width/height
    int wh = wp.height();

    QRect d;	// to be filled destination rectangle; may exceed desktop!

    switch (wpmode)
    {
	case NoWallpaper:
	    break;
	case Centred:
	    d.setRect((w - ww) / 2, (h - wh) / 2, ww, wh);
	    break;
	case Tiled:
	    d.setRect(0, 0, w, h);
	    break;
	case CenterTiled:
	    d.setCoords(-ww + ((w - ww) / 2) % ww, -wh + ((h - wh) / 2) % wh, w, h);
	    break;
	case Scaled: 
	    wp = wp.smoothScale(ww = w, wh = h);
	    d.setRect(0, 0, w, h);
	    break;
        case CentredAutoFit:
            if( ww <= w && wh <= h ) {
    	        d.setRect((w - ww) / 2, (h - wh) / 2, ww, wh); // like Centred
	        break;
            }
            // fall through
	case CentredMaxpect:
            double sx = (double) w / ww;
            double sy = (double) h / wh;
            if (sx > sy) {
                ww = (int)(sy * ww);
                wh = h;
            } else {
                wh = (int)(sx * wh);
                ww = w;
            }
            wp = wp.smoothScale(ww, wh);
	    d.setRect((w - ww) / 2, (h - wh) / 2, ww, wh);
	    break;
    }


    // copy background to m_pImage
    if (m_pBackground->size() == m_Size) {
	*m_pImage = m_pBackground->copy();

	if (m_pImage->depth() < 32)
	    *m_pImage = m_pImage->convertDepth(32, DiffuseAlphaDither);

    } else {
	m_pImage->create(w, h, 32);
	tile(m_pImage, QRect(0, 0, w, h), m_pBackground);
    }

    // blend wallpaper to destination rectangle of m_pImage
    if (d.isValid())
        for (int y = d.top(); y < d.bottom(); y += wh) {
	    for (int x = d.left(); x < d.right(); x += ww) {
		blend(m_pImage, QRect(x, y, ww, wh), &wp,
			QPoint(-QMIN(x, 0), -QMIN(y, 0)));
	    }
	}


    // blend whole desktop
    if (wpmode != NoWallpaper) {
      int bal = blendBalance();

      switch( blmode ) {
      case HorizontalBlending:
	KImageEffect::blend( *m_pImage, *m_pBackground,
			     KImageEffect::HorizontalGradient,
			     bal, 100 );
	break;

      case VerticalBlending:
	KImageEffect::blend( *m_pImage, *m_pBackground,
			     KImageEffect::VerticalGradient,
			     100, bal );
	break;

      case PyramidBlending:
	KImageEffect::blend( *m_pImage, *m_pBackground,
			     KImageEffect::PyramidGradient,
			     bal, bal );
	break;

      case PipeCrossBlending:
	KImageEffect::blend( *m_pImage, *m_pBackground,
			     KImageEffect::PipeCrossGradient,
			     bal, bal );
	break;

      case EllipticBlending:
	KImageEffect::blend( *m_pImage, *m_pBackground,
			     KImageEffect::EllipticGradient,
			     bal, bal );
	break;

      case IntensityBlending:
	KImageEffect::modulate( *m_pImage, *m_pBackground, reverseBlending(),
		    KImageEffect::Intensity, bal, KImageEffect::All );
	break;

      case SaturateBlending:
	KImageEffect::modulate( *m_pImage, *m_pBackground, reverseBlending(),
		    KImageEffect::Saturation, bal, KImageEffect::Gray );
	break;

      case ContrastBlending:
	KImageEffect::modulate( *m_pImage, *m_pBackground, reverseBlending(),
		    KImageEffect::Contrast, bal, KImageEffect::All );
	break;

      case HueShiftBlending:
	KImageEffect::modulate( *m_pImage, *m_pBackground, reverseBlending(),
		    KImageEffect::HueShift, bal, KImageEffect::Gray );
	break;
      }
    }

    if (retval == Done)
        m_State |= WallpaperDone;

    return retval;
}



/* Alpha blend an area from <src> with offset <soffs> to rectangle <dr> of <dst>
 * Default offset is QPoint(0, 0).
 */
void KBackgroundRenderer::blend(QImage *dst, QRect dr, QImage *src, QPoint soffs)
{
    int x, y, a;
    dr &= dst->rect();

    for (y = 0; y < dr.height(); y++) {
	if (dst->scanLine(dr.y() + y) && src->scanLine(soffs.y() + y)) {
	    QRgb *b, *d;
	    for (x = 0; x < dr.width(); x++) {
		b = reinterpret_cast<QRgb*>(dst->scanLine(dr.y() + y)
			+ (dr.x() + x) * sizeof(QRgb));
                d = reinterpret_cast<QRgb*>(src->scanLine(soffs.y() + y)
			+ (soffs.x() + x) * sizeof(QRgb));
                a = qAlpha(*d);
                *b = qRgb(qRed(*b) - (((qRed(*b) - qRed(*d)) * a) >> 8),
                          qGreen(*b) - (((qGreen(*b) - qGreen(*d)) * a) >> 8),
                          qBlue(*b) - (((qBlue(*b) - qBlue(*d)) * a) >> 8));
            }
        }
    }
}



void KBackgroundRenderer::slotBackgroundDone(KProcess *)
{
    kdDebug() << "slotBackgroundDone" << endl;
    m_State |= BackgroundDone;

    if (m_pProc->normalExit() && !m_pProc->exitStatus())
        m_pBackground->load(m_Tempfile->name());

    m_Tempfile->unlink();
    delete m_Tempfile; m_Tempfile = 0;
    m_pTimer->start(0, true);
}



/*
 * Starts the rendering process.
 */
void KBackgroundRenderer::start()
{
    if (m_pBackground == 0L)
	m_pBackground = new QImage();
    if (m_pImage == 0L)
	m_pImage = new QImage();

    m_State = Rendering;
    m_pTimer->start(0, true);
}


/*
 * This slot is connected to a timer event. It is called repeatedly until
 * the rendering is done.
 */
void KBackgroundRenderer::render()
{
    if (!(m_State & Rendering))
        return;

    int ret;

    if (!(m_State & BackgroundDone)) {
        ret = doBackground();
        if (ret != Wait)
	    m_pTimer->start(0, true);
	return;
    }

    // No async wallpaper
    doWallpaper();

    done();
}


/*
 * Rendering is finished.
 */
void KBackgroundRenderer::done()
{
    m_State |= AllDone;
    emit imageDone(desk());
}


/*
 * Stop the rendering.
 */
void KBackgroundRenderer::stop()
{
    if (!(m_State & Rendering))
	return;

    doBackground(true);
    doWallpaper(true);

    m_State = 0;
}


/*
 * Cleanup after rendering.
 */
void KBackgroundRenderer::cleanup()
{
    delete m_pBackground; m_pBackground = 0L;
    delete m_pImage; m_pImage = 0L;
    delete m_pProc; m_pProc = 0L;
    m_State = 0;
}


void KBackgroundRenderer::setPreview(QSize size)
{
    if (size.isNull())
        m_bPreview = false;
    else {
        m_bPreview = true;
        m_Size = size;
    }
}


void KBackgroundRenderer::setTile(bool tile)
{
    m_bTile = tile;
}


QImage *KBackgroundRenderer::image()
{
    if (m_State & AllDone)
        return m_pImage;
    return 0L;
}


void KBackgroundRenderer::load(int desk, bool reparseConfig)
{
    if (m_State & Rendering)
        stop();
    cleanup();
    m_bPreview = false;
    m_bTile = false;
    m_Size = m_rSize;

    KBackgroundSettings::load(desk, reparseConfig);
}

void KBackgroundRenderer::createTempFile()
{
   if( !m_Tempfile )
     m_Tempfile = new KTempFile();
}


#include "bgrender.moc"
