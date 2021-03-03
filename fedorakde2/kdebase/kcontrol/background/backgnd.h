/* vi: ts=8 sts=4 sw=4
 *
 * $Id: backgnd.h,v 1.5 2001/06/29 19:23:42 granroth Exp $
 *
 * This file is part of the KDE project, module kcmdisplay.
 * Copyright (C) 1999 Geert Jansen <g.t.jansen@stud.tue.nl>
 * 
 * You can Freely distribute this program under the GNU General Public
 * License. See the file "COPYING" for the exact licensing terms.
 */

#ifndef __Bgnd_h_Included__
#define __Bgnd_h_Included__

#include <qobject.h>
#include <qstring.h>
#include <qcolor.h>
#include <qmap.h>
#include <qevent.h>
#include <qwidget.h>
#include <qvector.h>

#include <kcmodule.h>
#include <bgdefaults.h>

class QCheckBox;
class QListBox;
class QComboBox;
class QStringList;
class QHButtonGroup;
class QPalette;
class QLabel;
class QSlider;
class QTabWidget;
class QSpinBox;

class KColorButton;
class KBackgroundRenderer;
class KGlobalBackgroundSettings;
class KConfig;
class KStandardDirs;


/**
 * This class handles drops on the preview monitor.
 */
class KBGMonitor : public QWidget
{
    Q_OBJECT
public:

    KBGMonitor(QWidget *parent, const char *name=0L);
	     
signals:
    void imageDropped(QString);

protected:
    virtual void dropEvent(QDropEvent *);
    virtual void dragEnterEvent(QDragEnterEvent *);
};


/**
 * The Desktop/Background tab in kcontrol.
 */
class KBackground: public KCModule
{
    Q_OBJECT

public:
    KBackground(QWidget *parent, const char *name);

    virtual void load();
    virtual void save();
    virtual void defaults();

    QString quickHelp() const;

signals:
    void changed(bool);

private slots:
    void slotSelectDesk(int desk);
    void slotCommonDesk(bool common);
    void slotBGMode(int mode);
    void slotBGSetup();
    void slotColor1(const QColor &);
    void slotColor2(const QColor &);
    void slotImageDropped(QString);
    void slotWallpaperType(int);
    void slotWPMode(int);
    void slotWallpaper(const QString &);
    void slotBrowseWallpaper();
    void slotSetupMulti();
    void slotPreviewDone(int);
    void slotBlendMode(int mode);
    void slotBlendBalance(int value);
    void slotReverseBlending(bool value);
    void slotLimitCache(bool);
    void slotCacheSize(int);

private:
    void init();
    void apply();

    int m_Desk, m_Max;
    int m_oldMode;

    QListBox *m_pDeskList;
    QCheckBox *m_pCBCommon;
    QCheckBox *m_pReverseBlending, *m_pCBLimit;
    QComboBox *m_pBackgroundBox, *m_pWallpaperBox;
    QComboBox *m_pArrangementBox, *m_pBlendBox;
    QHButtonGroup *m_WallpaperType;
    QSlider *m_pBlendSlider;
    QPushButton *m_pBGSetupBut, *m_pMSetupBut;
    QPushButton *m_pBrowseBut;
    QTabWidget *m_pTabWidget;
    QWidget *m_pTab1, *m_pTab2, *m_pTab3, *m_pTab4;
    QSpinBox *m_pCacheBox;
    QMap<QString,int> m_Wallpaper;

    QVector<KBackgroundRenderer> m_Renderer;
    KGlobalBackgroundSettings *m_pGlobals;
    KColorButton *m_pColor1But, *m_pColor2But;
    KBGMonitor *m_pMonitor;

    KConfig *m_pConfig;
    KStandardDirs *m_pDirs;
};


#endif // __Bgnd_h_Included__
