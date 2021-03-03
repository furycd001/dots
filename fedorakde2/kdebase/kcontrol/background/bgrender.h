/* vi: ts=8 sts=4 sw=4
 *
 * $Id: bgrender.h,v 1.4 2001/03/26 11:44:57 adawit Exp $
 *
 * This file is part of the KDE project, module kdesktop.
 * Copyright (C) 1999 Geert Jansen <g.t.jansen@stud.tue.nl>
 * 
 * You can Freely distribute this program under the GNU Library General
 * Public License. See the file "COPYING.LIB" for the exact licensing terms.
 */

#ifndef __BGRender_h_Included__
#define __BGRender_h_Included__

#include <qobject.h>
#include <qstring.h>

#include <bgsettings.h>

class QSize;
class QImage;
class QTimer;

class KConfig;
class KProcess;
class KTempFile;
class KShellProcess;
class KStandardDirs;

/**
 * This class renders a desktop background to a QImage. The operation is 
 * asynchronous: connect to the signal imageDone() to find out when the 
 * rendering is finished. It also has support for preview images, like 
 * the monitor in kcmdisplay.
 */
class KBackgroundRenderer: 
	public QObject,
	public KBackgroundSettings
{
    Q_OBJECT

public:
    KBackgroundRenderer(int desk, KConfig *config=0);
    ~KBackgroundRenderer();

    void load(int desk, bool reparseConfig=true);

    void setPreview(QSize size);
    void setTile(bool tile);

    QImage *image();
    bool isActive() { return m_State & Rendering; }
    void cleanup();

public slots:
    void start();
    void stop();
    
signals:
    void imageDone(int desk);

private slots:
    void slotBackgroundDone(KProcess *);
    void render();
    void done();

private:
    enum { Error, Wait, WaitUpdate, Done };
    enum { Rendering = 1, BackgroundStarted = 2, 
	BackgroundDone = 4, WallpaperStarted = 8, 
	WallpaperDone = 0x10, AllDone = 0x20 };

    QString buildCommand();
    void createTempFile();
    void tile(QImage *dst, QRect rect, QImage *src);
    void blend(QImage *dst, QRect dr, QImage *src, QPoint soffs = QPoint(0, 0));

    int doBackground(bool quit=false);
    int doWallpaper(bool quit=false);

    bool m_bPreview, m_bTile;
    int m_State;
    pid_t m_Pid;

    KTempFile* m_Tempfile;
    QSize m_Size, m_rSize;
    QImage *m_pImage, *m_pBackground;
    QTimer *m_pTimer;

    KConfig *m_pConfig;
    KStandardDirs *m_pDirs;
    KShellProcess *m_pProc;
};


#endif // __BGRender_h_Included__
