/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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


#include "desktop.h"
#include "krootwm.h"
#include "bgmanager.h"
#include "bgsettings.h"
#include "startupid.h"
#include "kdiconview.h"
#include "minicli.h"

#include <string.h>
#include <unistd.h>
#include <kcolordrag.h>

#include <qdragobject.h>
#include <qrect.h>
#include <qdir.h>

#include <netwm.h>
#include <dcopclient.h>
#include <kapp.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kstddirs.h>
#include <kimageio.h>
#include <kipc.h>
#include <klocale.h>
#include <kio/netaccess.h>
#include <kprocess.h>
#include <ksycoca.h>
#include <ktempfile.h>
#include <kurl.h>
#include <kmessagebox.h>
#include <kglobalaccel.h>
#include <kwinmodule.h>
#include <krun.h>
#include <ksimpleconfig.h>
#include <kwin.h>
#include <kglobalsettings.h>
#include <kpopupmenu.h>

// root window hack
#include <X11/X.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
// ----

// -----------------------------------------------------------------------------
#define DEFAULT_DELETEACTION 1

// for multihead - from main.cc
extern int kdesktop_screen_number;

KDesktop::KDesktop( bool x_root_hack, bool auto_start, bool wait_for_kded ) :
    QWidget( 0L, "desktop", WResizeNoErase | (x_root_hack ? (WStyle_Customize | WStyle_NoBorder) : 0) ),
    // those two WStyle_ break kdesktop when the root-hack isn't used (no Dnd)

    DCOPObject( "KDesktopIface" ), startup_id( NULL )
{
  m_bAutoStart = auto_start;
  m_bWaitForKded = wait_for_kded;
  m_miniCli = 0; // created on demand
  keys = 0; // created later
  KGlobal::locale()->insertCatalogue("kdesktop");
  KGlobal::locale()->insertCatalogue("libkonq"); // needed for apps using libkonq

  setCaption( "KDE Desktop");
  KWin::setType( winId(), NET::Desktop );

  setAcceptDrops(true); // WStyle_Customize seems to disable that
  m_pKwinmodule = new KWinModule( this );
  updateWorkAreaTimer = new QTimer( this );
  connect( updateWorkAreaTimer, SIGNAL( timeout() ),
           this, SLOT( updateWorkArea() ) );
  connect( m_pKwinmodule, SIGNAL( workAreaChanged() ),
           this, SLOT( workAreaChanged() ) );

  // Dont repaint on configuration changes during construction
  m_bInit = true;

  // It's the child widget that gets the focus, not us
  setFocusPolicy( NoFocus );

  if ( x_root_hack )
  {
    // this is a ugly hack to make Dnd work
    // Matthias told me that it won't be necessary with kwin
    // actually my first try with ICCCM (Dirk) :-)
    unsigned long data[2];
    data[0] = (unsigned long) 1;
    data[1] = (unsigned long) 0; // None; (Werner)
    Atom wm_state = XInternAtom(qt_xdisplay(), "WM_STATE", False);
    XChangeProperty(qt_xdisplay(), winId(), wm_state, wm_state, 32,
                    PropModeReplace, (unsigned char *)data, 2);

  }

  setGeometry( QApplication::desktop()->geometry() );
  lower();

  connect( kapp, SIGNAL( shutDown() ),
           this, SLOT( slotShutdown() ) );

  connect(kapp, SIGNAL(settingsChanged(int)),
          this, SLOT(slotSettingsChanged(int)));
  kapp->addKipcEventMask(KIPC::SettingsChanged);

  connect(KSycoca::self(), SIGNAL(databaseChanged()),
          this, SLOT(slotDatabaseChanged()));

  m_pIconView = new KDIconView( this, 0 );
  connect( m_pIconView, SIGNAL( imageDropEvent( QDropEvent * ) ),
           this, SLOT( handleImageDropEvent( QDropEvent * ) ) );
  connect( m_pIconView, SIGNAL( colorDropEvent( QDropEvent * ) ),
           this, SLOT( handleColorDropEvent( QDropEvent * ) ) );
  connect( m_pIconView, SIGNAL( newWallpaper( const KURL & ) ),
           this, SLOT( slotNewWallpaper( const KURL & ) ) );

  // All the QScrollView/QWidget-specific stuff should go here, so that we can use
  // another qscrollview/widget instead of the iconview and use the same code
  m_pIconView->setVScrollBarMode( QScrollView::AlwaysOff );
  m_pIconView->setHScrollBarMode( QScrollView::AlwaysOff );
  m_pIconView->setDragAutoScroll( false );
  m_pIconView->setFrameStyle( QFrame::NoFrame );
  m_pIconView->setBackgroundMode( NoBackground );
  m_pIconView->viewport()->setBackgroundMode( NoBackground );
  m_pIconView->setFocusPolicy( StrongFocus );
  m_pIconView->viewport()->setFocusPolicy( StrongFocus );
  m_pIconView->setGeometry( geometry() );

  // Geert Jansen: backgroundmanager belongs here
  // TODO tell KBackgroundManager if we change widget()
  bgMgr = new KBackgroundManager( widget(), m_pKwinmodule );
  connect( bgMgr, SIGNAL( initDone()), SLOT( backgroundInitDone()));

  //kdDebug(1204) << "KDesktop constructor -> workAreaChanged" << endl;
  workAreaChanged();

  QTimer::singleShot(0, this, SLOT( slotStart() ));
}

void
KDesktop::backgroundInitDone()
{
  //kdDebug(1204) << "KDesktop::backgroundInitDone" << endl;
  show();
  const QPixmap *bg = backgroundPixmap();
  if ( bg )
  {
     // Set the root window background now too.
     // This is entirely so that when the window manager starts
     // background flashing is kept to a minimum.
     kapp->desktop()->setBackgroundPixmap( *bg );
  }
}

void
KDesktop::slotStart()
{
  //kdDebug(1204) << "KDesktop::slotStart" << endl;
  if (!m_bInit) return;

  kapp->dcopClient()->send( "ksplash", "", "upAndRunning(QString)", QString("kdesktop"));

  // In case we started without database
  KImageIO::registerFormats();

  initConfig();

  // We need to be visible in order to insert icons, even if the background isn't ready yet...
  show();

  // Now we may react to configuration changes
  m_bInit = false;

  m_pIconView->start();

  // Global keys
  keys = new KGlobalAccel;
#include "kdesktopbindings.cpp"
  keys->connectItem("Execute command", this, SLOT(slotExecuteCommand()));
  keys->connectItem("Show taskmanager", this, SLOT(slotShowTaskManager()));
  keys->connectItem("Show window list", this, SLOT(slotShowWindowList()));
  keys->connectItem("Logout", this, SLOT(slotLogout()));
  keys->connectItem("Logout without Confirmation", this, SLOT(slotLogoutWithoutConfirmation()));

  KRootWm* krootwm = new KRootWm( this ); // handler for root menu (used by kdesktop on RMB click)
  keys->connectItem("Lock screen", krootwm, SLOT( slotLock()));

  keys->readSettings();

  if ( m_bAutoStart )
  {
     // now let's execute all the stuff in the autostart folder.
     // the stuff will actually be really executed when the event loop is
     // entered, since KRun internally uses a QTimer
     QDir dir( KGlobalSettings::autostartPath() );
     QStringList entries = dir.entryList( QDir::Files );
     QStringList::Iterator it = entries.begin();
     QStringList::Iterator end = entries.end();
     for (; it != end; ++it )
     {
            // Don't execute backup files
            if ( (*it).right(1) != "~" && (*it).right(4) != ".bak" &&
                 ( (*it)[0] != '%' || (*it).right(1) != "%" ) &&
                 ( (*it)[0] != '#' || (*it).right(1) != "#" ) )
            {
                KURL url;
                url.setPath( dir.absPath() + '/' + (*it) );
                (void) new KRun( url, 0, true );
            }
     }
   }

  connect(kapp, SIGNAL(appearanceChanged()), SLOT(slotConfigure()));
}

// -----------------------------------------------------------------------------

KDesktop::~KDesktop()
{
  delete m_miniCli;
  delete bgMgr;
  delete keys;
  delete startup_id;
}

// -----------------------------------------------------------------------------

void KDesktop::initConfig()
{
    m_pIconView->initConfig( m_bInit );

    if ( keys )
         keys->readSettings();

    KConfig c( "klaunchrc", true );
    c.setGroup( "FeedbackStyle" );
    if( !c.readBoolEntry( "BusyCursor", true ))
    {
        delete startup_id;
        startup_id = NULL;
    }
    else
    {
        if( startup_id == NULL )
            startup_id = new StartupId;
        startup_id->configure();
    }

    KConfig * config = KGlobal::config();
    config->setGroup( "General" );
    set_vroot = config->readBoolEntry( "SetVRoot", false );
    slotSetVRoot(); // start timer
}

// -----------------------------------------------------------------------------

void KDesktop::slotExecuteCommand()
{
    // this function needs to be duplicated since it appears that one
    // cannot have a 'slot' be a DCOP method.  if this changes in the
    // future, then 'slotExecuteCommand' and 'popupExecuteCommand' can
    // merge into one slot.
    popupExecuteCommand();
}

/*
  Shows minicli
 */
void KDesktop::popupExecuteCommand()
{
  if (m_bInit)
      return;

  // Created on demand
  if ( !m_miniCli )
  {
      m_miniCli = new Minicli;
      m_miniCli->adjustSize(); // for the centering below
  }

  // Move minicli to the current desktop
  NETWinInfo info( qt_xdisplay(), m_miniCli->winId(), qt_xrootwin(), NET::WMDesktop );
  int currentDesktop = kwinModule()->currentDesktop();
  if ( info.desktop() != currentDesktop )
      info.setDesktop( currentDesktop );

  if ( m_miniCli->isVisible() ) {
      m_miniCli->raise();
  } else {
      KDesktopWidget *desktop = KApplication::desktop();
      QRect rect = desktop->screenGeometry(desktop->screenNumber(QCursor::pos()));
      m_miniCli->move(rect.x() + (rect.width() - m_miniCli->width())/2,
                      rect.y() + (rect.height() - m_miniCli->height())/2);
      m_miniCli->show();
  }
  KWin::setActiveWindow( m_miniCli->winId() );
}

void KDesktop::slotShowWindowList()
{
     KWin::setActiveWindow( winId() );
     KRootWm::getRootWm()->slotWindowList();
}

void KDesktop::slotShowTaskManager()
{
    kdDebug(1204) << "Launching KSysGuard..." << endl;
    KProcess* p = new KProcess;
    CHECK_PTR(p);

    *p << "ksysguard2";
    *p << "--showprocesses";

    p->start(KProcess::DontCare);

    delete p;
}

// -----------------------------------------------------------------------------

void KDesktop::rearrangeIcons()
{
    m_pIconView->rearrangeIcons();
}

void KDesktop::lineupIcons()
{
    m_pIconView->lineupIcons();
}

void KDesktop::selectAll()
{
    m_pIconView->selectAll( true );
}

void KDesktop::unselectAll()
{
    m_pIconView->selectAll( false );
}

QStringList KDesktop::selectedURLs()
{
    return m_pIconView->selectedURLs();
}

void KDesktop::refreshIcons()
{
    m_pIconView->refreshIcons();
}

KActionCollection * KDesktop::actionCollection()
{
    return m_pIconView->actionCollection();
}

KURL KDesktop::url() const
{
    return m_pIconView->url();
}

QWidget * KDesktop::widget() const
{
     return m_pIconView;
}

// -----------------------------------------------------------------------------

void KDesktop::slotConfigure()
{
    configure();
}

void KDesktop::configure()
{
    // re-read configuration and apply it
    KGlobal::config()->reparseConfiguration();

    // If we have done start() already, then re-configure.
    // Otherwise, start() will call initConfig anyway
    if (!m_bInit)
    {
       initConfig();
       KRootWm::getRootWm()->initConfig();
    }
}

void KDesktop::slotSettingsChanged(int category)
{
    //kdDebug(1204) << "KDesktop::slotSettingsChanged" << endl;
    if (category == KApplication::SETTINGS_PATHS)
    {
        kdDebug(1204) << "KDesktop::slotSettingsChanged SETTINGS_PATHS" << endl;
        m_pIconView->recheckDesktopURL();
    }
}

void KDesktop::slotDatabaseChanged()
{
    //kdDebug(1204) << "KDesktop::slotDatabaseChanged" << endl;
    if (m_bInit) // kded is done, now we can "start" for real
        slotStart();
    if (KSycoca::isChanged("mimetypes"))
        m_pIconView->refreshMimeTypes();
}

void KDesktop::refresh()
{
  // George Staikos 3/14/01
  // This bit will just refresh the desktop and icons.  Now I have code
  // in KWin to do a complete refresh so this isn't really needed.
  // I'll leave it in here incase the plan is changed again
#if 0
  m_bNeedRepaint |= 1;
  updateWorkArea();
  refreshIcons();
#endif
  kapp->dcopClient()->send( "kwin", "", "refresh()", "");
}

// -----------------------------------------------------------------------------

void KDesktop::slotSetVRoot()
{
    if (KWin::info(winId()).mappingState == NET::Withdrawn) {
        QTimer::singleShot(100, this, SLOT(slotSetVRoot()));
        return;
    }

    unsigned long rw = RootWindowOfScreen(ScreenOfDisplay(qt_xdisplay(), qt_xscreen()));
    unsigned long vroot_data[1] = { m_pIconView->viewport()->winId() };
    Atom vroot = XInternAtom(qt_xdisplay(), "__SWM_VROOT", False);

    Window rootReturn, parentReturn, *children;
    unsigned int numChildren;
    Window top = winId();
    while (1) {
        /*int ret = */XQueryTree(qt_xdisplay(), top , &rootReturn, &parentReturn,
                                 &children, &numChildren);
        if (children)
            XFree((char *)children);
        if (parentReturn == rw) {
            break;
        } else
            top = parentReturn;
    }
    if ( set_vroot )
        XChangeProperty(qt_xdisplay(), top, vroot, XA_WINDOW, 32,
                        PropModeReplace, (unsigned char *)vroot_data, 1);
    else
        XDeleteProperty (qt_xdisplay(), top, vroot);
}

// -----------------------------------------------------------------------------

void KDesktop::slotShutdown()
{
    m_pIconView->slotSaveIconPositions();
    if ( m_miniCli )
        m_miniCli->saveConfig();
}

// don't hide when someone presses Alt-F4 on us
void KDesktop::closeEvent(QCloseEvent *e)
{
    e->ignore();
}

void KDesktop::workAreaChanged()
{
    //kdDebug(1204) << "KDesktop::workAreaChanged() -> starting timer" << endl;
    updateWorkAreaTimer->stop();
    updateWorkAreaTimer->start( 100, TRUE );
}

void KDesktop::updateWorkArea()
{
    QRect wr( kwinModule()->workArea( kwinModule()->currentDesktop() ) );
    m_pIconView->updateWorkArea( wr );
}

void KDesktop::handleColorDropEvent(QDropEvent * e)
{
    KPopupMenu popup;
    popup.insertItem(SmallIconSet("colors"),i18n("Set as primary background color"), 1);
    popup.insertItem(SmallIconSet("colors"),i18n("Set as secondary background color"), 2);
    int result = popup.exec(e->pos());

    QColor c;
    KColorDrag::decode(e, c);
    switch (result) {
      case 1: bgMgr->setColor(c, true); break;
      case 2: bgMgr->setColor(c, false); break;
      default: break;
    }
    bgMgr->setWallpaper(0,0);
}

void KDesktop::handleImageDropEvent(QDropEvent * e)
{
    KPopupMenu popup;
    popup.insertItem( SmallIconSet("background"),i18n("Set as &Wallpaper"), 1);
    int result = popup.exec(e->pos());

    if (result == 1)
    {
        QImage i;
        QImageDrag::decode(e, i);
        KTempFile tmpFile(KGlobal::dirs()->saveLocation("wallpaper"), ".png");
        i.save(tmpFile.name(), "PNG");
        kdDebug(1204) << "KDesktop::contentsDropEvent " << tmpFile.name() << endl;
        bgMgr->setWallpaper(tmpFile.name());
    }
}

void KDesktop::slotNewWallpaper(const KURL &url)
{
    // This is called when a file containing an image is dropped
    // (called by KonqOperations)
    QString tmpFile;
    KIO::NetAccess::download( url, tmpFile );
    bgMgr->setWallpaper(tmpFile);
}

void KDesktop::logout()
{
    if( !kapp->requestShutDown() )
        // this i18n string is also in kicker/applets/run/runapplet
        KMessageBox::error( this, i18n("Could not logout properly.  The session manager cannot\n"
                                        "be contacted.  You can try to force a shutdown by pressing\n"
                                        "the CTRL, ALT and BACKSPACE keys at the same time.  Note\n"
                                        "however that your current session will not be saved with a\n"
                                        "forced shutdown." ) );
}

void KDesktop::slotLogout()
{
    logout();
}

void KDesktop::slotLogoutWithoutConfirmation()
{
    kapp->requestShutDown( true );
}

void KDesktop::setVRoot( bool enable )
{
    if ( enable == set_vroot )
        return;

    set_vroot = enable;
    kdDebug(1204) << "setVRoot " << enable << endl;
    KConfig * config = KGlobal::config();
    KConfigGroupSaver gs( config, "General" );
    config->writeEntry( "SetVRoot", set_vroot );
    config->sync();
    slotSetVRoot();
}

#include "desktop.moc"
