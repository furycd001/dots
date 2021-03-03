/*****************************************************************

Copyright (c) 1996-2000 the kicker authors. See file AUTHORS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include <qtooltip.h>
#include <qcursor.h>
#include <qfile.h>

#include <kglobal.h>
#include <kconfig.h>
#include <kstddirs.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpropsdlg.h>
#include <krun.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kdesktopfile.h>
#include <kapp.h>
#include <kiconeffect.h>
#include <kwinmodule.h>
#include <kwin.h>
#include <kglobalaccel.h>
#include <kglobalsettings.h>
#include <konq_operations.h>
#include <kwindowlistmenu.h>
#include <kaction.h>
#include <kbookmarkmenu.h>

#include "browser_mnu.h"
#include "recent_mnu.h"
#include "k_mnu.h"
#include "konsole_mnu.h"
#include "panelbutton.h"
#include "panelbutton.moc"
#include "exe_dlg.h"
#include "browser_dlg.h"
#include "global.h"
#include "panel.h"

#include <netwm.h>
#include <kipc.h>

PanelButton::PanelButton( QWidget * parent, const char *name)
  : PanelButtonBase( parent, name )
{
    connect(kapp, SIGNAL(iconChanged(int)), SLOT(slotIconChanged(int)));
    kapp->addKipcEventMask(KIPC::IconChanged);
}

void PanelButton::slotIconChanged(int /*group*/)
{
    resizeEvent(0L);
}

QPoint PanelButton::getPopupPosition(QPopupMenu *menu)
{
    QPoint gpos = mapToGlobal(QPoint(0, 0));

    switch (dir)
    {
        case dDown:
            gpos = QPoint(gpos.x(), topLevelWidget()->height() + topLevelWidget()->pos().y());
            if (menu->sizeHint().height() > kapp->desktop()->height() - gpos.y())
            {
               // Move menu to the right of the mouse pointer.
               gpos.setX(gpos.x() + 40);
            }
            break;
        case dUp:
            gpos = QPoint(gpos.x(), topLevelWidget()->pos().y() - menu->sizeHint().height());
            if (menu->sizeHint().height() > topLevelWidget()->pos().y())
            {
               // Move menu to the right of the mouse pointer.
               gpos.setX(gpos.x() + 40);
            }
            break;
        case dLeft: return(QPoint(topLevelWidget()->pos().x() - menu->sizeHint().width(), gpos.y()));
        case dRight: return(QPoint(topLevelWidget()->width() + topLevelWidget()->pos().x(), gpos.y()));
    }
    return gpos;
}


PanelPopupButton::PanelPopupButton(QWidget *parent, const char *name)
  : PanelButton(parent, name)
{
    _pressedDuringPopup = false;
    _popup = 0;
    setDrawArrow(true);
    connect(this, SIGNAL(pressed()), SLOT(slotExecMenu()));
}

void PanelPopupButton::setPopup(QPopupMenu *popup)
{
    if(popup != 0) {
	_popup = popup;
	_popup->installEventFilter(this);
    }
}

QPopupMenu *PanelPopupButton::popup() const
{
    return _popup;
}

bool PanelPopupButton::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::MouseButtonPress
	    || e->type() == QEvent::MouseButtonDblClick) {
	QMouseEvent *me = static_cast<QMouseEvent *>(e);
	if (rect().contains(mapFromGlobal(me->globalPos()))) {
	    _pressedDuringPopup = true;
	    return true;
	}
    }
    else if (e->type() == QEvent::MouseButtonRelease) {
	QMouseEvent *me = static_cast<QMouseEvent *>(e);
	if (rect().contains(mapFromGlobal(me->globalPos()))) {
	    if (_pressedDuringPopup)
		_popup->hide();
	    return true;
	}
    }
    return false;
}

void PanelPopupButton::slotExecMenu()
{
    if (_popup != 0) {
	_pressedDuringPopup = false;
	initPopup();
	_popup->exec(getPopupPosition(_popup));
	setDown(false);
    }
}


PanelURLButton::PanelURLButton(const QString &url, QWidget *parent,
                               const char *name)
  : PanelButton(parent, name)
{
    configure();
    urlStr = url;
    KURL u(url);
    local = u.isLocalFile();
    setToolTip();
    setIcon(u);

    connect(this, SIGNAL(clicked()), SLOT(slotExec()));
    setAcceptDrops(true);
}

void PanelURLButton::saveConfiguration(KConfig* config, const QString& group)
{
    config->setGroup(group);

    config->writeEntry("URL", urlStr);
    config->sync();
}

void PanelURLButton::configure()
{
    PanelButton::configure();

    KConfig *config = KGlobal::config();
    config->setGroup("buttons");

    QString tile = QString::null;
    if(config->readBoolEntry("EnableTileBackground", false))
    {
        config->setGroup("button_tiles");
        if(config->readBoolEntry("EnableURLTiles", true))
            tile = config->readEntry("URLTile", QString::null);
    }
    setTile(tile);
}

void PanelURLButton::setToolTip()
{
    if (local)
    {
        KDesktopFile df(KURL(urlStr).path());
        if ( df.readComment().isEmpty() )
            QToolTip::add(this, df.readName());
        else
            QToolTip::add(this, df.readName() + " - " + df.readComment());
        setTitle(df.readName());
    }
    else
    {
        QToolTip::add(this, urlStr);
        setTitle(urlStr);
    }
}

void PanelURLButton::dragEnterEvent(QDragEnterEvent *ev)
{
    ev->accept(QUriDrag::canDecode(ev));
}

void PanelURLButton::dropEvent(QDropEvent *ev)
{
    kapp->propagateSessionManager();
    QStrList fileList;
    QStringList execList;
    if(QUriDrag::decode(ev, fileList)){
        QStrListIterator it(fileList);
        for(;it.current(); ++it)
            execList.append(it.current());
        KURL url(urlStr);
        if(!execList.isEmpty() && KDesktopFile::isDesktopFile(url.path())){
            KApplication::startServiceByDesktopPath(url.path(), execList);
        }
    }
}

void PanelURLButton::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == LeftButton)
	last_lmb_press = e->pos();
    QButton::mousePressEvent(e);
}

void PanelURLButton::mouseMoveEvent(QMouseEvent *e)
{
    if ((e->state() & LeftButton) == 0) return;

    QPoint p(e->pos() - last_lmb_press);
    if (p.manhattanLength() <= 16) // KGlobalSettings::dndEventDelay() is not enough!
	return;

    setDown(false);
    QStrList uris;
    uris.append(urlStr.local8Bit());
    QDragObject *dd = new QUriDrag(uris, this);

    KIcon::StdSizes sz = // Er, enum names should not be plural
	width() < 32 ?
	KIcon::SizeSmall : (width() < 48 ? KIcon::SizeMedium : KIcon::SizeLarge);

    QPixmap pm = KGlobal::iconLoader()->loadIcon(KMimeType::iconForURL(KURL(urlStr)),
                                                 KIcon::Panel, sz,
                                                 KIcon::DefaultState, 0L, true);
    dd->setPixmap(pm);
    dd->dragCopy();
}

void PanelURLButton::resizeEvent(QResizeEvent*e)
{
    PanelButtonBase::resizeEvent(e);

    setIcon(KURL(urlStr));
}

void PanelURLButton::slotExec()
{
    KIconEffect::visualActivate(this, rect());
    kapp->propagateSessionManager();
    new KRun(urlStr, 0, local);
}

void PanelURLButton::updateURL()
{
    if (pDlg->kurl().path() != urlStr) {
        urlStr = pDlg->kurl().path();
        setIcon( pDlg->kurl() );
    }

    pDlg = 0L;
    resizeEvent(0);
    setToolTip();
    emit requestSave();
}

void PanelURLButton::properties()
{
    if ( (local && !QFile::exists(KURL(urlStr).path()) )
         || KURL(urlStr).isMalformed())
    {
        KMessageBox::error( 0L, i18n("The file %1 doesn't exist").arg(urlStr) );
        return;
    }

    pDlg = new KPropertiesDialog(KURL(urlStr)); // will delete itself
    connect(pDlg, SIGNAL(applied()), SLOT(updateURL()));
}


PanelBrowserButton::PanelBrowserButton(const QString& icon, const QString &startDir,
				       QWidget *parent, const char *name)
    : PanelPopupButton(parent, name), _icon( icon )
{
    configure();

    topMenu = new PanelBrowserMenu(startDir);
    setPopup(topMenu);

    QToolTip::add(this, i18n("Browse: %1").arg(startDir));
    setTitle(startDir);
}

void PanelBrowserButton::saveConfiguration(KConfig* config, const QString& group)
{
    config->setGroup(group);

    config->writeEntry("Icon", _icon);
    config->writeEntry("Path", topMenu->path());
    config->sync();
}

void PanelBrowserButton::configure()
{
    PanelButton::configure();

    KConfig *config = KGlobal::config();
    config->setGroup("buttons");

    QString tile = QString::null;
    if(config->readBoolEntry("EnableTileBackground", false))
    {
        config->setGroup("button_tiles");
        if(config->readBoolEntry("EnableBrowserTiles", true))
            tile = config->readEntry("BrowserTile", QString::null);
    }
    setTile(tile);
}

void PanelBrowserButton::resizeEvent(QResizeEvent* e)
{
    PanelButtonBase::resizeEvent(e);
    setIcon(_icon, "kdisknav");
}

void PanelBrowserButton::initPopup()
{
    topMenu->initialize();
}

void PanelBrowserButton::properties()
{
    PanelBrowserDialog dlg( topMenu->path(), _icon, this );

    if( dlg.exec() == QDialog::Accepted ){
	_icon = dlg.icon();
	QString path = dlg.path();

	if ( path != topMenu->path() ) {
	    delete topMenu;
	    topMenu = new PanelBrowserMenu( path );
	    setTitle( path );
	}
	setIcon( _icon, "kdisknav" );
	emit requestSave();
    }
}


PanelServiceMenuButton::PanelServiceMenuButton(const QString& label, const QString &relPath,
					       QWidget *parent, const char *name)
  : PanelPopupButton(parent, name)
{
    configure();

    topMenu = new PanelServiceMenu(label, relPath);
    setPopup(topMenu);

    QToolTip::add(this, i18n("Browse: %1").arg(label));
    setTitle(label);
}

void PanelServiceMenuButton::saveConfiguration(KConfig* config, const QString& group)
{
    config->setGroup(group);

    config->writeEntry("Label", topMenu->path());
    config->writeEntry("RelPath", topMenu->relPath());
    config->sync();
}

void PanelServiceMenuButton::configure()
{
    PanelButton::configure();

    KConfig *config = KGlobal::config();
    config->setGroup("buttons");

    QString tile = QString::null;
    if(config->readBoolEntry("EnableTileBackground", false))
    {
        config->setGroup("button_tiles");
        if(config->readBoolEntry("EnableBrowserTiles", true))
            tile = config->readEntry("BrowserTile", QString::null);
    }
    setTile(tile);
}

void PanelServiceMenuButton::resizeEvent(QResizeEvent* e)
{
    PanelButtonBase::resizeEvent(e);

    QString dirFile(locate("apps", topMenu->relPath()+"/.directory"));
    QString iconFile;
    if(QFile::exists(dirFile)){
        KSimpleConfig config(dirFile);
        config.setDesktopGroup();
        iconFile = config.readEntry("Icon", "folder");
    }
    if(iconFile.isEmpty())
        iconFile = "folder";

    setIcon(iconFile, "folder");
}

void PanelServiceMenuButton::initPopup()
{
    topMenu->initialize();
}


PanelKButton::PanelKButton(QWidget *parent, const char *name)
    :PanelPopupButton(parent, name)
{
    QToolTip::add(this, i18n("Start Application"));
    setTitle(i18n("Start Application"));
    topMenu = new PanelKMenu(this);
    setPopup(topMenu);

    connect(topMenu, SIGNAL(aboutToHide()), this, SLOT(slotRelease()));

    // client menu stuff
    menuMgr = new KickerMenuManager( topMenu, this, "kickerMenuManager" );
    connect(menuMgr, SIGNAL(popupKMenu(int, int)), this,
            SLOT(slotExecMenuAt(int, int)));

    static bool initGlobal = true;
    if ( initGlobal ) {
        initGlobal = false;
        KGlobalAccel *keys = PGlobal::globalKeys;
#define LAUNCH_MENU
#include "kickerbindings.cpp"
#undef LAUNCH_MENU
        keys->connectItem( "Popup Launch Menu", this,
                           SLOT( slotAccelActivated() ) );
    }
}

PanelKButton::~PanelKButton()
{
}

void PanelKButton::configure()
{
    PGlobal::globalKeys->readSettings();
    if ( topMenu )
	topMenu->reinitialize();

    PanelButton::configure();

    KConfig *config = KGlobal::config();
    config->setGroup("buttons");

    QString tile = QString::null;
    if(config->readBoolEntry("EnableTileBackground", false))
    {
        config->setGroup("button_tiles");
        if(config->readBoolEntry("EnableKMenuTiles", true))
            tile = config->readEntry("KMenuTile", QString::null);
    }
    setTile(tile);
}

void PanelKButton::resizeEvent(QResizeEvent*e)
{
    PanelButtonBase::resizeEvent(e);

    setIcon("go", "unknown");
}

void PanelKButton::slotAccelActivated()
{
    if (topMenu->isVisible()) {
	setDown(false);
	topMenu->hide();
    }
    else {
	setDown(true);
	topMenu->initialize();
	topMenu->popup(getPopupPosition(topMenu));
	topMenu->setActiveItem(0);
    }
}

void PanelKButton::slotRelease()
{
    setDown(false);
}

void PanelKButton::initPopup()
{
    topMenu->initialize();
}

void PanelKButton::slotExecMenuAt(int x, int y)
{
    topMenu->hide();
    topMenu->initialize();
    topMenu->popup(QPoint(x,y));
}

void PanelKButton::properties()
{
    KApplication::startServiceByDesktopName("kmenuedit");
}


PanelDesktopButton::PanelDesktopButton(QWidget *parent, const char *name)
  :PanelButton(parent, name)
{
    setToggleButton( TRUE );

    QToolTip::add(this, i18n("Show Desktop"));
    setTitle(i18n("Show Desktop"));

    static bool initGlobal = true;
    if ( initGlobal ) {
        initGlobal = false;
        KGlobalAccel *keys = PGlobal::globalKeys;
#define SHOW_DESKTOP
#include "kickerbindings.cpp"
#undef SHOW_DESKTOP
        keys->connectItem("Toggle Show Desktop", this, SLOT(toggle()));
    }

    // on desktop changes or when a window is deiconified, we abort the show desktop mode
    connect(PGlobal::kwin_module, SIGNAL(currentDesktopChanged(int)), SLOT(slotCurrentDesktopChanged(int)));
    connect( PGlobal::kwin_module, SIGNAL( windowChanged(WId,unsigned int) ),
	     SLOT( slotWindowChanged(WId,unsigned int) ) );


    connect( this, SIGNAL( toggled(bool ) ),
	     SLOT(slotShowDesktop(bool)));

    setAcceptDrops( true );
}

void PanelDesktopButton::dragEnterEvent( QDragEnterEvent *ev )
{
    ev->accept( QUriDrag::canDecode( ev ) );
}

void PanelDesktopButton::dropEvent( QDropEvent *ev )
{
    KURL dPath = KGlobalSettings::desktopPath();
    KFileItem item( dPath, QString::fromLatin1( "inode/directory" ), -1 );
    KonqOperations::doDrop( &item, dPath, ev, this );
}

void PanelDesktopButton::resizeEvent(QResizeEvent*e)
{
  PanelButtonBase::resizeEvent(e);
  setIcon("desktop", "unknown");
}

void PanelDesktopButton::slotCurrentDesktopChanged(int)
{
    setOn( FALSE ); // abort desktop change, will invoke slotShowDesktop( FALSE ) below
}

void PanelDesktopButton::slotWindowChanged(WId w, unsigned int dirty)
{
    if ( !isOn() )
	return;

    if ( dirty & NET::XAWMState ) {
	NETWinInfo inf( qt_xdisplay(), w, qt_xrootwin(),
			NET::XAWMState | NET::WMWindowType );
	if ( (inf.windowType() == NET::Normal || inf.windowType() == NET::Unknown )
	     && inf.mappingState() == NET::Visible ) {
	    // a window was deiconified, abort the show desktop mode.
	    iconifiedList.clear();
	    setOn( FALSE );
	}
    }
}

void PanelDesktopButton::slotShowDesktop( bool b )
{
    if ( b ) {
	iconifiedList.clear();
	const QValueList<WId> windows = PGlobal::kwin_module->windows();
	QValueList<WId>::ConstIterator it;

	for ( it=windows.begin(); it!=windows.end(); ++it ) {
	    WId w = *it;
	    NETWinInfo info( qt_xdisplay(), w, qt_xrootwin(),
			    NET::XAWMState | NET::WMDesktop );
 	    if ( info.mappingState() == NET::Visible &&
 		 ( info.desktop() == NETWinInfo::OnAllDesktops
		   || info.desktop() == (int) PGlobal::kwin_module->currentDesktop() )
		 ) {
		KWin::iconifyWindow( w, false );
		iconifiedList.append( w );
	    }
	}
    } else {
	QValueList<WId>::ConstIterator it;
	for ( it=iconifiedList.begin(); it!=iconifiedList.end(); ++it ) {
	    KWin::deIconifyWindow( *it, false  );
	}
    }
}

void PanelDesktopButton::configure()
{
    PanelButton::configure();

    PGlobal::globalKeys->readSettings();

    KConfig *config = KGlobal::config();
    config->setGroup("buttons");

    QString tile = QString::null;
    if(config->readBoolEntry("EnableTileBackground", false))
	{
	    config->setGroup("button_tiles");
	    if(config->readBoolEntry("EnableDesktopButtonTiles", true))
		tile = config->readEntry("DesktopButtonTile", QString::null);
	}
    setTile(tile);
}


// Non-KDE application
PanelExeButton::PanelExeButton(const QString &filePath, const QString &icon,
                               const QString &cmdLine, bool inTerm,
                               QWidget *parent, const char *name)
  : PanelButton(parent, name)
{
  configure();
  pathStr = filePath;
  iconStr = icon;
  cmdStr = cmdLine;
  term = inTerm;

  QToolTip::add(this, filePath + " " + cmdLine);
  setTitle(filePath);

  connect(this, SIGNAL(clicked()), SLOT(slotExec()));
}

void PanelExeButton::saveConfiguration(KConfig* config, const QString& group)
{
  config->setGroup(group);

  config->writeEntry("RunInTerminal", term);
  config->writeEntry("Path", pathStr);
  config->writeEntry("Icon", iconStr);
  config->writeEntry("CommandLine", cmdStr);
  config->sync();
}

void PanelExeButton::configure()
{
  PanelButton::configure();

  KConfig *config = KGlobal::config();
  config->setGroup("buttons");

  QString tile = QString::null;
  if(config->readBoolEntry("EnableTileBackground", false))
    {
      config->setGroup("button_tiles");
      if(config->readBoolEntry("EnableExeTiles", true))
        tile = config->readEntry("ExeTile", QString::null);
    }
  setTile(tile);
}

PanelExeButton::PanelExeButton(const QString &configData, QWidget *parent,
                               const char *name)
  : PanelButton(parent, name)
{
  term = (configData[0] ==  '1') ? true : false;
  pathStr= configData.mid(1);
  int index = pathStr.find("::mossie::");
  pathStr.truncate(index);
  iconStr=configData.mid(index+11);
  int index2 = iconStr.find("::mossie::");
  iconStr.truncate(index2);
  cmdStr =configData.mid(index+11+index2+10);
  kdDebug(1210) << "term " << term << ", path " << pathStr << ", icon " << iconStr << ", cmd " << cmdStr << endl;

  connect(this, SIGNAL(clicked()), SLOT(slotExec()));
  setAcceptDrops(true);
}

void PanelExeButton::dragEnterEvent(QDragEnterEvent *ev)
{
  ev->accept(QUriDrag::canDecode(ev));
}

void PanelExeButton::dropEvent(QDropEvent *ev)
{
  QStrList fileList;
  QStringList blah;
  QString execStr;
  if(QUriDrag::decode(ev, fileList)){
    QStrListIterator it(fileList);
    for(;it.current(); ++it){
      KURL url(it.current());
      if(KDesktopFile::isDesktopFile(url.path())){
        KDesktopFile deskFile(url.path());
        deskFile.setDesktopGroup();
        execStr += deskFile.readURL() + " ";
      }
      else
        execStr += url.path() + " ";
    }
    bool result;
    kapp->propagateSessionManager();
    if(term){
      KConfig *config = KGlobal::config();
      config->setGroup("misc");
      QString termStr = config->readEntry("Terminal", "konsole2");
      result = KRun::run(termStr + " -e " + pathStr + " " +
                         cmdStr + " " + execStr, blah);

    }
    else
      result = KRun::run(pathStr + " " + cmdStr + " " + execStr, blah);

    if(!result)
      KMessageBox::error(this, i18n("Cannot execute non-KDE application!"),
                         i18n("Kicker Error!"));
  }
}

void PanelExeButton::resizeEvent(QResizeEvent*e)
{
  PanelButtonBase::resizeEvent(e);
  setIcon(iconStr, "exec");
}

void PanelExeButton::slotExec()
{
  KIconEffect::visualActivate(this, rect());
  QStringList blah;
  bool result;
  kapp->propagateSessionManager();
  if(term){
    KConfig *config = KGlobal::config();
    config->setGroup("misc");
    QString termStr = config->readEntry("Terminal", "konsole2");
    result = KRun::run(termStr + " -e " + pathStr + " " + cmdStr, blah);
  }
  else
    result = KRun::run(pathStr + " " + cmdStr, blah);
  if(!result)
    KMessageBox::error(this, i18n("Cannot execute non-KDE application!"),
                       i18n("Kicker Error!"));
}

void PanelExeButton::properties()
{
  PanelExeDialog dlg(pathStr, iconStr, cmdStr, term, this);
  if(dlg.exec() == QDialog::Accepted){
    iconStr = dlg.icon();
    cmdStr = dlg.commandLine();
    term = dlg.useTerminal();
    setIcon(iconStr, "exec");
    emit requestSave();
  }
}

PanelWindowListButton::PanelWindowListButton(QWidget *parent, const char *name)
  : PanelPopupButton(parent, name)
{
    configure();
    topMenu = new KWindowListMenu;
    setPopup(topMenu);

    QToolTip::add(this, i18n("Window List"));

    setTitle(i18n("Window List"));
}

PanelWindowListButton::~PanelWindowListButton()
{
    delete topMenu;
}

void PanelWindowListButton::configure()
{
    PanelButton::configure();

    KConfig *config = KGlobal::config();
    config->setGroup("buttons");

    QString tile = QString::null;
    if(config->readBoolEntry("EnableTileBackground", false))
    {
	config->setGroup("button_tiles");
	if(config->readBoolEntry("EnableWindowListTiles", true))
	    tile = config->readEntry("WindowListTile", QString::null);
    }
    setTile(tile);
}

void PanelWindowListButton::resizeEvent(QResizeEvent* e)
{
    PanelButtonBase::resizeEvent(e);
    setIcon("window_list", "unknown");
}

void PanelWindowListButton::initPopup()
{
    topMenu->init();
}

PanelBookmarksButton::PanelBookmarksButton(QWidget *parent, const char *name)
    : PanelPopupButton(parent, name), bookmarkMenu( 0 ), bookmarkOwner( 0 )
{
    configure();

    actionCollection = new KActionCollection( this );

    bookmarkParent = new QPopupMenu( this, "bookmarks" );
    bookmarkOwner = new KBookmarkOwner;
    bookmarkMenu = new KBookmarkMenu( bookmarkOwner, bookmarkParent, actionCollection, true, false );

    setPopup(bookmarkParent);

    QToolTip::add(this, i18n("Bookmarks"));
    setTitle(i18n("Bookmarks"));
}

PanelBookmarksButton::~PanelBookmarksButton()
{
    delete bookmarkMenu;
    delete bookmarkOwner;
}

void PanelBookmarksButton::configure()
{
    PanelButton::configure();

    KConfig *config = KGlobal::config();
    config->setGroup("buttons");

    QString tile = QString::null;
    if(config->readBoolEntry("EnableTileBackground", false))
    {
	config->setGroup("button_tiles");
	if(config->readBoolEntry("EnableWindowListTiles", true))
	    tile = config->readEntry("WindowListTile", QString::null);
    }
    setTile(tile);
}

void PanelBookmarksButton::resizeEvent(QResizeEvent* e)
{
    PanelButtonBase::resizeEvent(e);
    setIcon("bookmark", "unknown");
}

void PanelBookmarksButton::initPopup()
{
    bookmarkMenu->ensureUpToDate();
}


PanelRecentDocumentsButton::PanelRecentDocumentsButton(QWidget *parent, const char *name)
    : PanelPopupButton(parent, name), recentMenu( 0 )
{
    configure();

    recentMenu = new PanelRecentMenu( this, "recentdocuments" );
    setPopup(recentMenu);

    QToolTip::add(this, i18n("Recent Documents"));
    setTitle(i18n("Recent Documents"));
}

PanelRecentDocumentsButton::~PanelRecentDocumentsButton()
{
}

void PanelRecentDocumentsButton::configure()
{
    PanelButton::configure();

    KConfig *config = KGlobal::config();
    config->setGroup("buttons");

    QString tile = QString::null;
    if(config->readBoolEntry("EnableTileBackground", false))
    {
	config->setGroup("button_tiles");
	if(config->readBoolEntry("EnableWindowListTiles", true))
	    tile = config->readEntry("WindowListTile", QString::null);
    }
    setTile(tile);
}

void PanelRecentDocumentsButton::resizeEvent(QResizeEvent* e)
{
    PanelButtonBase::resizeEvent(e);
    setIcon("document", "unknown");
}

void PanelRecentDocumentsButton::initPopup()
{
    if ( recentMenu )
	recentMenu->initialize();
}

PanelKonsoleButton::PanelKonsoleButton(QWidget *parent, const char *name)
    : PanelButton(parent, name)
{
    configure();
    setDrawArrow(true);

    konsoleMenu = new PanelKonsoleMenu( this, "konsolesessions" );

    QToolTip::add(this, i18n("Terminal-Emulation"));
    setTitle(i18n("Terminal Session"));

    connect(this, SIGNAL(pressed()), SLOT(slotStartTimer()));
    connect(this, SIGNAL(released()), SLOT(slotStopTimer()));
    connect(this, SIGNAL(clicked()), SLOT(slotExec()));
    menuTimer = new QTimer(this);
    connect(menuTimer, SIGNAL(timeout()), SLOT(slotDelayedPopup()));
}

PanelKonsoleButton::~PanelKonsoleButton()
{
}

void PanelKonsoleButton::configure()
{
    PanelButton::configure();

    KConfig *config = KGlobal::config();
    config->setGroup("buttons");

    QString tile = QString::null;
    if(config->readBoolEntry("EnableTileBackground", false))
    {
	config->setGroup("button_tiles");
	if(config->readBoolEntry("EnableURLTiles", true)) // FIXME: get an own tile?
	    tile = config->readEntry("URLTile", QString::null);
    }
    setTile(tile);
}

void PanelKonsoleButton::resizeEvent(QResizeEvent* e)
{
    PanelButtonBase::resizeEvent(e);
    setIcon("konsole", "unknown");
}

void PanelKonsoleButton::slotStartTimer()
{
    menuTimer->start(500, true);
}

void PanelKonsoleButton::slotStopTimer()
{
    menuTimer->stop();
}

void PanelKonsoleButton::slotExec()
{
    kapp->kdeinitExec("konsole2");
}

void PanelKonsoleButton::slotDelayedPopup()
{
	konsoleMenu->initialize();
	konsoleMenu->exec(getPopupPosition(konsoleMenu));
    setDown(false);
}

