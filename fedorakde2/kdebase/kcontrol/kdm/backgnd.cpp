/* vi: ts=8 sts=4 sw=4
 *
 * $Id: backgnd.cpp,v 1.15 2001/06/11 02:21:54 rahn Exp $
 *
 * This file is part of the KDE project, module kcmdisplay.
 * Copyright (C) 1999 Geert Jansen <g.t.jansen@stud.tue.nl>
 *
 * Modified 2000.07.14 by Brad Hughes <bhughes@trolltech.com>
 * Improve layout and consistency with KDesktop's background selection
 *
 * Based on old backgnd.cpp:
 *
 * Copyright (c)  Martin R. Jones 1996
 * Converted to a kcc module by Matthias Hoelzer 1997
 * Gradient backgrounds by Mark Donohoe 1997
 * Pattern backgrounds by Stephan Kulow 1998
 * Randomizing & dnd & new display modes by Matej Koss 1998
 *
 * You can Freely distribute this program under the GNU General Public
 * License. See the file "COPYING" for the exact licensing terms.
 */

#include <unistd.h>
#include <sys/types.h>


#include <qobject.h>
#include <qlayout.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qdragobject.h>
#include <qhbox.h>
#include <qevent.h>
#include <qwhatsthis.h>
#include <qtabwidget.h>

#include <kapp.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kiconloader.h>
#include <kcolorbtn.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kpixmap.h>
#include <dcopclient.h>
#include <ksimpleconfig.h>
#include <kmessagebox.h>

#include <bgdefaults.h>
#include <bgsettings.h>
#include <bgrender.h>
#include <bgdialogs.h>
#include <backgnd.h>


extern KSimpleConfig *c;

/**** DLL Interface ****/

extern "C" {
    KCModule *create_background(QWidget *parent, const char *name) {
    return new KBackground(parent, name);
    }
}


/**** KBGMonitor ****/

void KBGMonitor::dropEvent(QDropEvent *e)
{
    if (!QUriDrag::canDecode(e))
    return;

    QStringList uris;
    if (QUriDrag::decodeLocalFiles(e, uris) && (uris.count() > 0)) {
    QString uri = *uris.begin();
    uri.prepend('/');
    emit imageDropped(uri);
    }
}


void KBGMonitor::dragEnterEvent(QDragEnterEvent *e)
{
    e->accept(QImageDrag::canDecode(e)|| QUriDrag::canDecode(e));
}


/**** KBackground ****/

KBackground::KBackground(QWidget *parent, const char *name)
    : KCModule(parent, name)
{
    QString wtstr;
    m_pDirs = KGlobal::dirs();

    // Top layout
    QGridLayout *top = new QGridLayout(this, 2, 1, 10, 10);
    top->setColStretch(0, 1);
    // top->setColStretch(1, 2);

    // A nice button size. Translators can adapt this
    QPushButton *pbut = new QPushButton(i18n("abcdefgh"), this);
    QSize bsize = pbut->sizeHint();
    delete pbut;

    // Preview monitor at (0,0)
    QLabel *lbl = new QLabel(this);
    lbl->setPixmap(locate("data", "kcontrol/pics/monitor.png"));
    lbl->setFixedSize(lbl->sizeHint());
    top->addWidget(lbl, 0,0); // 0,1, AlignCenter);
    m_pMonitor = new KBGMonitor(lbl);
    m_pMonitor->setGeometry(23, 14, 151, 115);
    connect(m_pMonitor, SIGNAL(imageDropped(QString)), SLOT(slotImageDropped(QString)));

    QWhatsThis::add( m_pMonitor,
             i18n("Here you can see a preview of how KDM's background will look "
              "like using the current settings. You can even set a background "
              "picture by dragging it onto the preview (e.g. from Konqueror).") );

    // Tabwidget at (1,0)
    m_pTabWidget = new QTabWidget(this);
    top->addWidget(m_pTabWidget, 1, 0);

    // Background settings on Tab 1
    m_pTab1 = new QWidget(0, "Background Tab");
    m_pTabWidget->addTab(m_pTab1, i18n("Back&ground"));

    // QGroupBox *group = new QGroupBox(i18n("Background"), this);
    // top->addWidget(group, 1, 0);

    QGridLayout *grid = new QGridLayout(m_pTab1, 4, 3, 10, 10);
    grid->setColStretch(1, 1);
    grid->setColStretch(2, 1);

    lbl = new QLabel(i18n("&Mode:"), m_pTab1);
    lbl->setFixedSize(lbl->sizeHint());
    grid->addWidget(lbl, 0, 0, Qt::AlignLeft);
    m_pBackgroundBox = new QComboBox(m_pTab1);
    lbl->setBuddy( m_pBackgroundBox );
    connect(m_pBackgroundBox, SIGNAL(activated(int)), SLOT(slotBGMode(int)));
    lbl->setBuddy(m_pBackgroundBox);
    grid->addWidget(m_pBackgroundBox, 0, 1);

    wtstr = i18n("Here you can change the way colors are applied to KDM's background."
         " Apart of just a plain color you can choose gradients, custom patterns or a"
         " background program (e.g. xearth).");
    QWhatsThis::add( lbl, wtstr );
    QWhatsThis::add( m_pBackgroundBox, wtstr );

    lbl = new QLabel(i18n("Color &1:"), m_pTab1);
    lbl->setFixedSize(lbl->sizeHint());
    grid->addWidget(lbl, 1, 0, Qt::AlignLeft);
    m_pColor1But = new KColorButton(m_pTab1);
    lbl->setBuddy( m_pColor1But );
    connect(m_pColor1But, SIGNAL(changed(const QColor &)),
        SLOT(slotColor1(const QColor &)));
    grid->addWidget(m_pColor1But, 1, 1);
    lbl->setBuddy(m_pColor1But);

    wtstr = i18n("By clicking on these buttons you can choose the colors KDM will use to "
         "paint the background. If you selected a gradient or a pattern, you can "
         "choose two colors.");
    QWhatsThis::add( lbl, wtstr );
    QWhatsThis::add( m_pColor1But, wtstr );

    lbl = new QLabel(i18n("Color &2:"), m_pTab1);
    lbl->setFixedSize(lbl->sizeHint());
    grid->addWidget(lbl, 2, 0, Qt::AlignLeft);
    m_pColor2But = new KColorButton(m_pTab1);
    lbl->setBuddy( m_pColor2But );
    connect(m_pColor2But, SIGNAL(changed(const QColor &)),
        SLOT(slotColor2(const QColor &)));
    grid->addWidget(m_pColor2But, 2, 1);
    lbl->setBuddy(m_pColor2But);

    QWhatsThis::add( lbl, wtstr );
    QWhatsThis::add( m_pColor2But, wtstr );

    QHBoxLayout *hbox = new QHBoxLayout();
    grid->addLayout(hbox, 3, 1);
    m_pBGSetupBut = new QPushButton(i18n("S&etup"), m_pTab1);
    m_pBGSetupBut->setFixedSize(bsize);
    connect(m_pBGSetupBut, SIGNAL(clicked()), SLOT(slotBGSetup()));
    hbox->addWidget(m_pBGSetupBut);
    hbox->addStretch();

    QWhatsThis::add( m_pBGSetupBut, i18n("Click here to setup a pattern or a background"
                     " program."));

    // Wallpaper at Tab2
    m_pTab2 = new QWidget(0, "Wallpaper Tab");
    m_pTabWidget->addTab(m_pTab2, i18n("Wa&llpaper"));

    // group = new QGroupBox(i18n("Wallpaper"), this);
    // top->addWidget(group, 1, 1);

    grid = new QGridLayout(m_pTab2, 4, 3, 10, 10);
    grid->setColStretch(1, 1);
    grid->setColStretch(2, 1);

    lbl = new QLabel(i18n("&Mode:"), m_pTab2);
    lbl->setFixedSize(lbl->sizeHint());
    grid->addWidget(lbl, 0, 0, Qt::AlignLeft);
    m_pArrangementBox = new QComboBox(m_pTab2);
    lbl->setBuddy( m_pArrangementBox );
    connect(m_pArrangementBox, SIGNAL(activated(int)), SLOT(slotWPMode(int)));
    lbl->setBuddy(m_pArrangementBox);
    grid->addWidget(m_pArrangementBox, 0, 1);

    wtstr = i18n("Here you can choose the way the selected picture will be used to cover"
         " KDM's background. If you select \"No Wallpaper\", the colors settings will be used.");
    QWhatsThis::add( lbl, wtstr );
    QWhatsThis::add( m_pArrangementBox, wtstr );

    lbl = new QLabel(i18n("&Wallpaper"), m_pTab2);
    lbl->setFixedSize(lbl->sizeHint());
    grid->addWidget(lbl, 1, 0, Qt::AlignLeft);
    m_pWallpaperBox = new QComboBox(m_pTab2);
    lbl->setBuddy( m_pWallpaperBox );
    lbl->setBuddy(m_pWallpaperBox);
    connect(m_pWallpaperBox, SIGNAL(activated(const QString &)),
        SLOT(slotWallpaper(const QString &)));
    grid->addWidget(m_pWallpaperBox, 1, 1);

    wtstr = i18n("Here you can choose from several wallpaper pictures, i.e. pictures"
         " that have been installed in the system's or your wallpaper directory. If you"
         " want to use other pictures, you can either click on the \"Browse\" button or"
         " drag a picture (e.g. from Konqueror) onto the preview.");
    QWhatsThis::add( lbl, wtstr );
    QWhatsThis::add( m_pWallpaperBox, wtstr );

    hbox = new QHBoxLayout();
    grid->addLayout(hbox, 2, 1);
    m_pBrowseBut = new QPushButton(i18n("B&rowse"), m_pTab2);
    m_pBrowseBut->setFixedSize(bsize);
    connect(m_pBrowseBut, SIGNAL(clicked()), SLOT(slotBrowseWallpaper()));
    hbox->addWidget(m_pBrowseBut);
    hbox->addStretch();

    QWhatsThis::add( m_pBrowseBut, i18n("Click here to choose a wallpaper using a file dialog.") );

    m_pCBMulti = new QCheckBox(i18n("Mul&tiple:"), m_pTab2);
    m_pCBMulti->setFixedSize(m_pCBMulti->sizeHint());
    connect(m_pCBMulti, SIGNAL(toggled(bool)), SLOT(slotMultiMode(bool)));
    grid->addWidget(m_pCBMulti, 3, 0);
    hbox = new QHBoxLayout();
    grid->addLayout(hbox, 3, 1);
    m_pMSetupBut = new QPushButton(i18n("S&etup"), m_pTab2);
    m_pMSetupBut->setFixedSize(bsize);
    connect(m_pMSetupBut, SIGNAL(clicked()), SLOT(slotSetupMulti()));
    hbox->addWidget(m_pMSetupBut);
    hbox->addStretch();

    m_pCBMulti->hide();
    m_pMSetupBut->hide();

    m_Renderer = new KBackgroundRenderer(0, c);
    connect(m_Renderer, SIGNAL(imageDone(int)), SLOT(slotPreviewDone(int)));

    init();
    apply();

    // read only mode
    if (getuid() != 0)
    {
        m_pCBMulti->setEnabled(false);
        m_pBackgroundBox->setEnabled(false);
        m_pWallpaperBox->setEnabled(false);
        m_pArrangementBox->setEnabled(false);
        m_pBGSetupBut->setEnabled(false);
        m_pMSetupBut->setEnabled(false);
        m_pBrowseBut->setEnabled(false);
        m_pColor1But->setEnabled(false);
        m_pColor2But->setEnabled(false);
    }
}


/*
 * Fill all check/listboxen
 */
void KBackground::init()
{
    int i;

    /*
    // Desktop names
    for (i=0; i<KWin::numberOfDesktops(); i++)
    m_pDeskList->insertItem(m_pGlobals->deskName(i));
    */

    // Background modes: make sure these match with kdesktop/bgrender.cc !!
    m_pBackgroundBox->insertItem(i18n("Flat"));
    m_pBackgroundBox->insertItem(i18n("Pattern"));
    m_pBackgroundBox->insertItem(i18n("Background Program"));
    m_pBackgroundBox->insertItem(i18n("Horizontal Gradient"));
    m_pBackgroundBox->insertItem(i18n("Vertical Gradient"));
    m_pBackgroundBox->insertItem(i18n("Pyramid Gradient"));
    m_pBackgroundBox->insertItem(i18n("Pipecross Gradient"));
    m_pBackgroundBox->insertItem(i18n("Elliptic Gradient"));

    // Wallpapers
    QStringList lst = m_pDirs->findAllResources("wallpaper", "*", false, true);
    for (i=0; i<(int)lst.count(); i++) {
    int n = lst[i].findRev('/');
    QString s = lst[i].mid(n+1);
    m_pWallpaperBox->insertItem(s);
    m_Wallpaper[s] = i;
    }

    // Wallpaper tilings: again they must match the ones from bgrender.cc
    m_pArrangementBox->insertItem(i18n("No Wallpaper"));
    m_pArrangementBox->insertItem(i18n("Centred"));
    m_pArrangementBox->insertItem(i18n("Tiled"));
    m_pArrangementBox->insertItem(i18n("Center Tiled"));
    m_pArrangementBox->insertItem(i18n("Centred Maxpect"));
    m_pArrangementBox->insertItem(i18n("Scaled"));
}


void KBackground::apply()
{
    KBackgroundRenderer *r = m_Renderer;

    /*
    // Desktop names
    if (m_pGlobals->commonBackground()) {
    m_pCBCommon->setChecked(true);
    m_pDeskList->setEnabled(false);
    } else  {
    m_pCBCommon->setChecked(false);
    m_pDeskList->setEnabled(true);
    m_pDeskList->setCurrentItem(m_Desk);
    }
    */

    // Background mode
    m_pBackgroundBox->setCurrentItem(r->backgroundMode());
    m_pColor1But->setColor(r->colorA());
    m_pColor2But->setColor(r->colorB());
    switch (r->backgroundMode()) {
    case KBackgroundSettings::Program:
    m_pColor1But->setEnabled(false);
    m_pColor2But->setEnabled(false);
    m_pBGSetupBut->setEnabled(true);
    break;
    case KBackgroundSettings::Flat:
    m_pColor1But->setEnabled(true);
    m_pColor2But->setEnabled(false);
    m_pBGSetupBut->setEnabled(false);
    break;
    case KBackgroundSettings::Pattern:
    m_pColor1But->setEnabled(true);
    m_pColor2But->setEnabled(true);
    m_pBGSetupBut->setEnabled(true);
    break;
    default:
    m_pColor1But->setEnabled(true);
    m_pColor2But->setEnabled(true);
    m_pBGSetupBut->setEnabled(false);
    break;
    }

    // Wallpaper mode
    QString wp = r->wallpaper();
    if (wp.isEmpty())
    wp = QString(" ");
    if (!m_Wallpaper.contains(wp)) {
    int count = m_Wallpaper.count();
    m_Wallpaper[wp] = count;
    m_pWallpaperBox->insertItem(wp);
    m_pWallpaperBox->setCurrentItem(count);
    }
    m_pWallpaperBox->setCurrentItem(m_Wallpaper[wp]);
    m_pArrangementBox->setCurrentItem(r->wallpaperMode());

    // Multi mode
    if (r->multiWallpaperMode() == KBackgroundSettings::NoMulti) {
    m_pCBMulti->setChecked(false);
    m_pWallpaperBox->setEnabled(true);
    m_pBrowseBut->setEnabled(true);
    m_pMSetupBut->setEnabled(false);
    } else {
    m_pCBMulti->setChecked(true);
    m_pWallpaperBox->setEnabled(false);
    m_pBrowseBut->setEnabled(false);
    m_pMSetupBut->setEnabled(true);
    }

    // Start preview render
    r->setPreview(m_pMonitor->size());
    r->start();
}


void KBackground::load()
{
    m_Renderer->load(0);

    apply();
    emit changed(false);
}


void KBackground::save()
{
    kdDebug() << "Saving stuff..." << endl;
    m_Renderer->writeSettings();

    emit changed(false);
}


void KBackground::defaults()
{
    KBackgroundRenderer *r = m_Renderer;

    if (r->isActive())
    r->stop();
    r->setBackgroundMode(KBackgroundSettings::Flat);
    r->setColorA(_defColorA);
    r->setColorB(_defColorB);
    r->setWallpaperMode(KBackgroundSettings::NoWallpaper);
    r->setMultiWallpaperMode(KBackgroundSettings::NoMulti);
    apply();
    emit changed(true);
}



/*
 * Called from the "Background Mode" combobox.
 */
void KBackground::slotBGMode(int mode)
{
    KBackgroundRenderer *r = m_Renderer;

    if (mode == r->backgroundMode())
    return;

    r->stop();
    r->setBackgroundMode(mode);
    apply();
    emit changed(true);
}


/*
 * Called from the "Background Setup" pushbutton.
 */
void KBackground::slotBGSetup()
{
    KBackgroundRenderer *r = m_Renderer;

    switch (r->backgroundMode()) {
    case KBackgroundSettings::Pattern:
    {
    KPatternSelectDialog dlg;
    QString cur = r->KBackgroundPattern::name();
    dlg.setCurrent(cur);
    if ((dlg.exec() == QDialog::Accepted) && !dlg.pattern().isEmpty()) {
        r->stop();
        r->setPattern(dlg.pattern());
        r->start();
        emit changed(true);
    }
    break;
    }
    case KBackgroundSettings::Program:
    {
    KProgramSelectDialog dlg;
    QString cur = r->KBackgroundProgram::name();
    dlg.setCurrent(cur);
    if ((dlg.exec() == QDialog::Accepted) && !dlg.program().isEmpty()) {
        r->stop();
        r->setProgram(dlg.program());
        r->start();
        emit changed(true);
    }
    break;
    }
    default:
    break;
    }
}


void KBackground::slotColor1(const QColor &color)
{
    KBackgroundRenderer *r = m_Renderer;

    if (color == r->colorA())
    return;

    r->stop();
    r->setColorA(color);
    r->start();
    emit changed(true);
}


void KBackground::slotColor2(const QColor &color)
{
    KBackgroundRenderer *r = m_Renderer;

    if (color == r->colorB())
    return;

    r->stop();
    r->setColorB(color);
    r->start();
    emit changed(true);
}


void KBackground::slotImageDropped(QString uri)
{
    KBackgroundRenderer *r = m_Renderer;
    if (uri == r->wallpaper())
    return;

    if (!m_Wallpaper.contains(uri)) {
    int count = m_Wallpaper.count();
    m_Wallpaper[uri] = count;
    m_pWallpaperBox->insertItem(uri);
    m_pWallpaperBox->setCurrentItem(count);
    }

    r->stop();
    r->setWallpaper(uri);
    r->start();
    emit changed(true);
}


void KBackground::slotMultiMode(bool multi)
{
    KBackgroundRenderer *r = m_Renderer;
    if (multi == (r->multiWallpaperMode() != KBackgroundSettings::NoMulti))
    return;

    r->stop();
    r->setMultiWallpaperMode(multi ? 1 : 0);
    r->start();

    if (multi) {
    m_pWallpaperBox->setEnabled(false);
    m_pBrowseBut->setEnabled(false);
    m_pMSetupBut->setEnabled(true);
    } else {
    m_pWallpaperBox->setEnabled(true);
    m_pBrowseBut->setEnabled(true);
    m_pMSetupBut->setEnabled(false);
    }
    emit changed(true);
}


void KBackground::slotWallpaper(const QString &wallpaper)
{
    KBackgroundRenderer *r = m_Renderer;

    if (wallpaper == r->wallpaper())
    return;

    r->stop();
    r->setWallpaper(wallpaper);
    r->start();
    emit changed(true);
}


void KBackground::slotBrowseWallpaper()
{
    KBackgroundRenderer *r = m_Renderer;

    KURL url = KFileDialog::getOpenURL();
    if (url.isEmpty())
    return;
    if (!url.isLocalFile()) {
      KMessageBox::sorry(this, i18n("Currently are only local wallpapers allowed."));
      return;
    }
    QString file = url.path();
    if (file == r->wallpaper())
    return;

    if (!m_Wallpaper.contains(file)) {
    int count = m_Wallpaper.count();
    m_Wallpaper[file] = count;
    m_pWallpaperBox->insertItem(file);
    m_pWallpaperBox->setCurrentItem(count);
    }

    r->stop();
    r->setWallpaper(file);
    r->start();
    emit changed(true);
}


/*
 * Called from the "Wallpaper Arrangement" combobox.
 */
void KBackground::slotWPMode(int mode)
{
    KBackgroundRenderer *r = m_Renderer;

    if (mode == r->wallpaperMode())
    return;

    r->stop();
    r->setWallpaperMode(mode);
    r->start();
    emit changed(true);
}


void KBackground::slotSetupMulti()
{
    KBackgroundRenderer *r = m_Renderer;

    KMultiWallpaperDialog dlg(r);
    if (dlg.exec() == QDialog::Accepted) {
    r->stop();
    r->start();
    emit changed(true);
    }
}


void KBackground::slotPreviewDone(int desk_done)
{
    kdDebug() << "Preview for desktop " << desk_done << " done" << endl;

    KBackgroundRenderer *r = m_Renderer;

    KPixmap pm;
    if (QPixmap::defaultDepth() < 15)
    pm.convertFromImage(*r->image(), KPixmap::LowColor);
    else
    pm.convertFromImage(*r->image());

    m_pMonitor->setBackgroundPixmap(pm);
}


#include "backgnd.moc"
