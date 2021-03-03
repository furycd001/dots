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

#include <math.h>
#include <unistd.h>

#include <qdragobject.h>
#include <qpixmap.h>
#include <qfileinfo.h>
#include <qbitmap.h>
#include <qtooltip.h>
#include <qtimer.h>
#include <qfile.h>
#include <qtextstream.h>

#include <kapp.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kurl.h>
#include <kdebug.h>
#include <kdesktopfile.h>
#include <kmimetype.h>
#include <kprocess.h>
#include <kpixmap.h>
#include <klocale.h>
#include <kpixmapeffect.h>
#include <kio/netaccess.h>

#include "containerarea.h"
#include "containerarea.moc"
#include "appletinfo.h"
#include "dirdrop_mnu.h"
#include "exe_dlg.h"
#include "panel.h"

#include "container_applet.h"
#include "container_button.h"


// for multihead
extern int kicker_screen_number;


ContainerArea::ContainerArea( Orientation orient, bool mainArea, KConfig* _c,
			      QWidget* parent, const char* name)
    : Panner( orient, parent, name )
, _block_relayout(false)
, _movingAC(false)
, _moveAC(0)
, _moveOffset(QPoint(0,0))
, _mainArea(mainArea)
, _config(_c)
, _dragIndicator(0)
, _dragMoveAC(0)
, _dragMoveOffset(QPoint(0,0))
{
    setAcceptDrops(true);
    _containers.setAutoDelete(false);
    connect(&_autoScrollTimer, SIGNAL(timeout()), SLOT(autoScroll()));
}

void ContainerArea::init()
{
    // restore applet layout or load a default panel layout
    KConfig* c = config();
    c->setGroup("General");

    if(c->hasKey("Applets"))
	loadContainerConfig();
    else
	defaultContainerConfig();
}

ContainerArea::~ContainerArea()
{
    // don't emit signals from destructor
    blockSignals( true );
    // clear applets
    removeAllContainers();
}

void ContainerArea::defaultContainerConfig()
{
    // only the main area has a default container config
    if (!_mainArea) {
	removeAllContainers();
	layoutChildren();
	saveContainerConfig();
	return;
    }

    // clear applets
    removeAllContainers();

    // kmenu
    KMenuButtonContainer *kmenu = new KMenuButtonContainer(viewport());
    addContainer(kmenu);

    // the desktop button
    DesktopButtonContainer *desktop = new DesktopButtonContainer(viewport());
    addContainer(desktop);

    // some url buttons
    URLButtonContainer *url;

    QRect r = PGlobal::panel->initialGeometry( PGlobal::panel->position() );
    int dsize;
    if (orientation() == Horizontal)
	dsize = r.width();
    else
	dsize = r.height();

    dsize -= 300;

    QStringList buttons;

    QFile f(locate("data", "kicker/default-apps"));
    if (f.open(IO_ReadOnly))
      {
        QTextStream is(&f);

	while (!is.eof())
	  buttons << is.readLine();

	f.close();
      }
    else
      {
	 buttons << "System/konsole.desktop";
	 buttons << "KControl.desktop";
	 buttons << "Help.desktop";
	 buttons << "Home.desktop";
	 buttons << "Internet/konqbrowser.desktop";
	 buttons << "Internet/KMail.desktop";
	 buttons << "Office/kword.desktop";
	 buttons << "Office/kspread.desktop";
	 buttons << "Office/kpresenter.desktop";
	 buttons << "Office/kontour.desktop";
	 buttons << "Editors/kwrite.desktop";
      }

    int size = dsize;
    for (QStringList::ConstIterator it = buttons.begin(); it != buttons.end(); ++it) {
	size -= 42;
	if (size <= 0)
	    break;
	QString s = locate("apps", *it);
	if (s.isEmpty()) continue;

	url = new URLButtonContainer(viewport(), s);
	addContainer(url);
	size -= 42;
    }

    // pager applet
    QString df = KGlobal::dirs()->findResource("applets", "kminipagerapplet.desktop");
    InternalAppletContainer *pager = new InternalAppletContainer(AppletInfo(df) ,viewport());
    addContainer(pager);

    // taskbar applet
    df = KGlobal::dirs()->findResource("applets", "ktaskbarapplet.desktop");
    InternalAppletContainer *taskbar = new InternalAppletContainer(AppletInfo(df), viewport());
    addContainer(taskbar);

    // system tray applet
    df = KGlobal::dirs()->findResource("applets", "ksystemtrayapplet.desktop");
    InternalAppletContainer *systemTray = new InternalAppletContainer(AppletInfo(df), viewport());
    systemTray->setFreeSpace(1);
    addContainer(systemTray);

    // date applet
    df = KGlobal::dirs()->findResource("applets", "clockapplet.desktop");
    InternalAppletContainer *date = new InternalAppletContainer(AppletInfo(df), viewport());
    date->setFreeSpace(1);
    addContainer(date);

    layoutChildren();
    saveContainerConfig();
}

void ContainerArea::saveContainerConfig(bool layoutOnly)
{
//    kdDebug(1210) << "ContainerArea::saveContainerConfig()" << endl;

    KConfig *c = config();
    c->setGroup("General");

    // build the applet list
    QStringList alist;

    QListIterator<BaseContainer> it(_containers);
    for(; it.current() ; ++it)
	alist.append( it.current()->appletId() );

    // write applet list (group 'panel', key 'applets')
    c->writeEntry("Applets", alist);

    // write applet config
    it.toFirst();
    for(; it.current() ; ++it)
	{
	    BaseContainer* a = it.current();

	    // set group to the applet id
	    c->setGroup(a->appletId());

	    // write positioning info
	    c->writeEntry("FreeSpace", QString("%1").arg(a->freeSpace()));

	    // write size hint
	    if(a->inherits("AppletContainer")) {
		if(orientation() == Horizontal)
		    c->writeEntry("WidthForHeightHint", QString("%1").arg(a->widthForHeight(height())));
		else
		    c->writeEntry("HeightForWidthHint", QString("%1").arg(a->heightForWidth(width())));
	    }

	    // let the applet container write custom data fields
	    if(!layoutOnly)
	    a->saveConfiguration(config(), a->appletId());
	}

    c->sync();
}

void ContainerArea::loadContainerConfig()
{
//    kdDebug(1210) << "ContainerArea::loadContainerConfig()" << endl;

    KConfig *c = config();

    // clear panel
    removeAllContainers();

    // read applet list
    c->setGroup("General");
    QStringList alist = c->readListEntry("Applets");
    QStringList trusted = c->readListEntry("TrustedApplets");

    // now restore the applets
    QStringList::Iterator it = alist.begin();
    while(it != alist.end())
	{
	    // applet id
	    QString appletId(*it);
	    QString group = appletId;

	    // is there a config group for this applet?
	    if(!c->hasGroup(group))
		continue;

	    // set config group
	    c->setGroup(group);

	    // read free space
	    float fspace = (float) c->readDoubleNumEntry("FreeSpace", 0);

            if(fspace > 1) fspace = 1;

	    BaseContainer* a = 0;

	    // create a matching applet container
	    if (appletId.contains("KMenuButton") > 0)
		a = new KMenuButtonContainer(viewport());
	    if (appletId.contains("DesktopButton") > 0)
		a = new DesktopButtonContainer(viewport());
	    else if (appletId.contains("WindowListButton") > 0)
		a = new WindowListButtonContainer(viewport());
	    else if (appletId.contains("BookmarksButton") > 0)
		a = new BookmarksButtonContainer(viewport());
	    else if (appletId.contains("RecentDocumentsButton") > 0)
		a = new RecentDocumentsButtonContainer(viewport());
	    else if (appletId.contains("URLButton") > 0)
		a = new URLButtonContainer(config(), group, viewport());
	    else if (appletId.contains("BrowserButton") > 0)
		a = new BrowserButtonContainer(config(), group, viewport());
	    else if (appletId.contains("ServiceMenuButton") > 0)
		a = new ServiceMenuButtonContainer(config(), group, viewport());
	    else if (appletId.contains("ExeButton") > 0)
		a = new ExeButtonContainer(config(), group, viewport());
        else if (appletId.contains("KonsoleButton") > 0)
        a = new KonsoleButtonContainer(viewport());
	    else if (appletId.contains("Applet") > 0)
		{
		    int whint = c->readNumEntry("WidthForHeightHint", 0);
		    int hhint = c->readNumEntry("HeightForWidthHint", 0);

		    c->setGroup(group);

		    QString df = KGlobal::dirs()->findResource("applets", c->readEntry("DesktopFile"));
		    AppletInfo info(df);

		    if ((info.isUniqueApplet() && hasInstance(&info)) || df.isEmpty()) {
			it++;
			continue;
		    }

		    QString configFile = c->readEntry("ConfigFile");
		    if (!configFile.isNull()) info.setConfigFile(configFile);

		    c->setGroup("General");

		    if(c->readNumEntry("SecurityLevel", 1) == 0)
			{
			    QString dfile = info.desktopFile();
                            int dotDesktopPos = dfile.findRev( '.' );
                            if ( dotDesktopPos != -1 )
                                dfile = dfile.left( dotDesktopPos );
			    bool trustedapplet = false;
			    for ( QStringList::Iterator it = trusted.begin(); it != trusted.end(); ++it )
				{
				    if ((*it) == dfile)
					trustedapplet = true;
				}

			    if (trustedapplet == true)
				a = new InternalAppletContainer(info, viewport());
			    else
				a = new ExternalAppletContainer(info, viewport());
			}
		    else
			a = new InternalAppletContainer(info, viewport());
		    ((AppletContainer*)a)->setWidthForHeightHint(whint);
		    ((AppletContainer*)a)->setHeightForWidthHint(hhint);
		}

	    if (a) {
		a->setFreeSpace(fspace);
		addContainer(a);
	    }
	    it++;
	}

    layoutChildren();
}

bool ContainerArea::hasInstance(AppletInfo* info) const
{
    bool found = false;

    for (QListIterator<BaseContainer> it(_containers); it.current(); ++it )
	{
	    BaseContainer *a = static_cast<BaseContainer*>(it.current());
	    if (a->inherits("AppletContainer")) {
		if (static_cast<AppletContainer*>(a)->info().library() == info->library()) {
		    found = true;
		    break;
		}
	    }
	}
    return found;
}

void ContainerArea::removeAllContainers()
{
    while ( !_containers.isEmpty() ) {
	BaseContainer* b = _containers.first();
	_containers.removeRef( b );
	delete b;
    }
    emit sizeHintChanged();
}

void ContainerArea::configure()
{
//    kdDebug(1210) << "ContainerArea::configure()" << endl;

    setBackgroundTheme();

    for (QListIterator<BaseContainer> it(_containers); it.current(); ++it )
	{
	    BaseContainer *a = it.current();
	    if (a)
		a->configure();
	}
}

void ContainerArea::addKMenuButton()
{
    KMenuButtonContainer *b = new KMenuButtonContainer(viewport());
    addContainer(b);
    moveToFirstFreePosition(b);
    scrollTo(b);
    saveContainerConfig();
}

void ContainerArea::addDesktopButton()
{
    DesktopButtonContainer *b = new DesktopButtonContainer(viewport());
    addContainer(b);
    moveToFirstFreePosition(b);
    scrollTo(b);
    saveContainerConfig();
}

void ContainerArea::addWindowListButton()
{
    WindowListButtonContainer *b = new WindowListButtonContainer(viewport());
    addContainer(b);
    moveToFirstFreePosition(b);
    scrollTo(b);
    saveContainerConfig();
}

void ContainerArea::addBookmarksButton()
{
    BookmarksButtonContainer *b = new BookmarksButtonContainer(viewport());
    addContainer(b);
    moveToFirstFreePosition(b);
    scrollTo(b);
    saveContainerConfig();
}

void ContainerArea::addRecentDocumentsButton()
{
    RecentDocumentsButtonContainer *b = new RecentDocumentsButtonContainer(viewport());
    addContainer(b);
    moveToFirstFreePosition(b);
    scrollTo(b);
    saveContainerConfig();
}

void ContainerArea::addURLButton(const QString &url)
{
    URLButtonContainer *b = new URLButtonContainer(viewport(), url);
    addContainer(b);
    moveToFirstFreePosition(b);
    scrollTo(b);
    saveContainerConfig();
}

void ContainerArea::addBrowserButton( const QString &startDir, const QString& icon )
{
    BrowserButtonContainer *b = new BrowserButtonContainer( viewport(), startDir, icon );
    addContainer(b);
    moveToFirstFreePosition(b);
    scrollTo(b);
    saveContainerConfig();
}

void ContainerArea::addServiceMenuButton(const QString &name, const QString& relPath)
{
    ServiceMenuButtonContainer *b = new ServiceMenuButtonContainer( viewport(), name, relPath );
    addContainer(b);
    moveToFirstFreePosition(b);
    scrollTo(b);
    saveContainerConfig();
}

void ContainerArea::addExeButton(const QString &filePath, const QString &icon,
				 const QString &cmdLine, bool inTerm)
{
    ExeButtonContainer *b = new ExeButtonContainer(viewport(), filePath, icon, cmdLine, inTerm);
    addContainer(b);
    moveToFirstFreePosition(b);
    scrollTo(b);
    saveContainerConfig();
}

void ContainerArea::addKonsoleButton()
{
    KonsoleButtonContainer *b = new KonsoleButtonContainer(viewport());
    addContainer(b);
    moveToFirstFreePosition(b);
    scrollTo(b);
    saveContainerConfig();
}

void ContainerArea::addApplet( const QString& desktopFile )
{
    bool internal = false;
    KConfig *c = KGlobal::config();
    c->setGroup("General");
    if(c->readNumEntry("SecurityLevel", 1) > 1)
	internal = true;

    addApplet( desktopFile, internal );
}

void ContainerArea::addApplet(const QString &desktopFile, bool internal)
{
    QString df = KGlobal::dirs()->findResource("applets", desktopFile);
    AppletInfo info(df);

    if (info.isUniqueApplet() && hasInstance(&info))
	return;

    AppletContainer *a;

    if (internal)
	{
	    a = new InternalAppletContainer(info, viewport());
	    addContainer(a);
	    moveToFirstFreePosition(a);
            scrollTo(a);
	    saveContainerConfig();
	}
    else
	{
	    a = new ExternalAppletContainer(info, viewport());
	    connect(a, SIGNAL(docked(ExternalAppletContainer*)),
		    SLOT(slotAddExternal(ExternalAppletContainer*)));
	}
}

void ContainerArea::slotAddExternal(ExternalAppletContainer* a)
{
    addContainer(a);
    moveToFirstFreePosition(a);
    scrollTo(a);
    saveContainerConfig();
}

void ContainerArea::addContainer(BaseContainer* a)
{
    if (!a) return;

    setUniqueId(a);

    _containers.append(a);

    emit sizeHintChanged();

    connect(a, SIGNAL(moveme(BaseContainer*) ),
	    SLOT( startContainerMove(BaseContainer*)));
    connect(a, SIGNAL(removeme(BaseContainer*) ),
	    SLOT( slotRemoveContainer(BaseContainer*)));
    connect(a, SIGNAL(requestSave()),
	    SLOT(slotSaveContainerConfig()));

    if (a->inherits("ExternalAppletContainer"))
	connect(a, SIGNAL(embeddedWindowDestroyed() ), this,
		SLOT( embeddedWindowDestroyed()));
    if (a->inherits("InternalAppletContainer")
	|| a->inherits("ExternalAppletContainer"))
	connect(a, SIGNAL(updateLayout() ), this,
		SLOT( slotLayoutChildren()));

    a->slotSetOrientation( orientation() );
    a->slotSetPopupDirection( popupDirection() );
    a->configure();
    addChild(a);
    a->show();
}

void ContainerArea::removeContainer(BaseContainer *a)
{
    if (a) {
	if (a->inherits("AppletContainer"))
	    static_cast<AppletContainer*>(a)->removeSessionConfigFile();

	removeChild(a);
	delete a;
	_containers.removeRef(a);
    }

    updateContainerList();
    emit sizeHintChanged();
    layoutChildren();
    saveContainerConfig(true);
    updateArrows();
}

void ContainerArea::setUniqueId(BaseContainer* a)
{
    QString idBase = a->appletType() + "_%1";
    QString newId;
    int i = 0;
    bool unique = false;

    while(!unique)
	{
	    i++;
	    newId = idBase.arg(i);

	    unique = true;
	    QListIterator<BaseContainer> it(_containers);
	    for(; it.current() ; ++it)
		{
		    BaseContainer* b = static_cast<BaseContainer*>(it.current());
		    if (b->appletId() == newId)
			{
			    unique = false;
			    break;
			}
		}
	}
    a->setAppletId(newId);
}

bool ContainerArea::isStretch(BaseContainer* a) const
{
    if (!a->inherits("AppletContainer"))
	return false;
    return (static_cast<AppletContainer*>(a)->type() ==  KPanelApplet::Stretch);
}

void ContainerArea::disableStretch()
{
    QListIterator<BaseContainer> it(_containers);
    for(; it.current() ; ++it)
	{
	    BaseContainer* b = static_cast<BaseContainer*>(it.current());

	    if (orientation() == Horizontal)
		b->resize(b->widthForHeight(height()), height());
	    else
		b->resize(width(), b->heightForWidth(width()));
	}
}

void ContainerArea::restoreStretch()
{
    BaseContainer* next = 0;
    QListIterator<BaseContainer> it(_containers);
    it.toLast();
    for(; it.current(); --it)
	{
	    BaseContainer* b = static_cast<BaseContainer*>(it.current());
	    if (isStretch(b))
		if (orientation() == Horizontal)
		    if (next)
			b->resize(next->x() - b->x(), height());
		    else
			b->resize(width() - b->x(), height());
		else
		    if (next)
			b->resize(width(), next->y() - b->y());
		    else
			b->resize(width(), height() - b->y());
	    next = b;
	}
}

void ContainerArea::startContainerMove(BaseContainer *a)
{
    if (!a) return;

    _moveAC = a;
    _movingAC = true;
    setMouseTracking(true);
    QCursor::setPos(mapToGlobal(QPoint(a->x() + a->moveOffset().x(), a->y() + a->moveOffset().y())));
    grabMouse(sizeAllCursor);

    _block_relayout = true;
    disableStretch();
    a->raise();
}

void ContainerArea::stopContainerMove(BaseContainer *b)
{
    if (_moveAC != b) return;

    _autoScrollTimer.stop();
    releaseMouse();
    setCursor(arrowCursor);
    _movingAC = false;
    setMouseTracking(false);

    if(_moveAC->inherits("ButtonContainer"))
	static_cast<ButtonContainer*>(_moveAC)->completeMoveOperation();

    _moveAC = 0;
    _block_relayout = false;

    updateContainerList();
    restoreStretch();
    saveContainerConfig(true);
}

void ContainerArea::mouseReleaseEvent(QMouseEvent *)
{
    if (_movingAC && _moveAC)
	stopContainerMove(_moveAC);
}

void ContainerArea::mouseMoveEvent(QMouseEvent *ev)
{
    if (!(_movingAC && _moveAC)) {
	Panner::mouseMoveEvent(ev);
	return;
    }

    int s;
    if (orientation() == Horizontal)
	s = width();
    else
	s = height();

    if (ev->state() & ShiftButton && s >= minimumUsedSpace( orientation(), width(), height() )) {

	if (orientation() == Horizontal) {
	    int oldX = _moveAC->x() + _moveAC->moveOffset().x();
	    int x = ev->pos().x();
	    moveContainerPush(_moveAC, x - oldX);
	}
	else if (orientation() == Vertical) {
	    int oldY = _moveAC->y() + _moveAC->moveOffset().y();
	    int y = ev->pos().y();
	    moveContainerPush(_moveAC, y - oldY);
	}
    }
    else {

	if (orientation() == Horizontal) {
	    int oldX = _moveAC->x() + _moveAC->moveOffset().x();
	    int x = ev->pos().x();
	    moveContainerSwitch(_moveAC, x - oldX);
	}
	else if (orientation() == Vertical) {
	    int oldY = _moveAC->y() + _moveAC->moveOffset().y();
	    int y = ev->pos().y();
	    moveContainerSwitch(_moveAC, y - oldY);
	}
    }
}

void ContainerArea::moveContainerSwitch(BaseContainer* moving, int distance)
{
    int nx =0;
    int ny = 0;
    bool scroll = false;

    // horizontal panel
    if (orientation() == Horizontal) {

	if (distance > 0) { // left to right

	    _containers.findRef(moving);
	    BaseContainer *next = _containers.next();
	    BaseContainer *last = moving;

	    while (next) {

		// 'moving' has completely passed applet 'next'.
		if ( next->x() + next->width() <= moving->x() + distance ) {
                    viewportToContents(next->x() - moving->width(), next->y(), nx, ny);
		    moveChild(next, nx, ny);
		    last = next;
		    next = _containers.next();
		    continue;
		}

		// 'next' has not been completely passed by 'moving', but
		// still may be covered by it.
		int switchMargin = 0;

		// calculate the position and width of the 'virtual' container
		// containing 'moving' and 'next'.
		int tx = next->x() - moving->width();
		int twidth = moving->width() + next->width();

		// determine the middle of the containers.
		int tmiddle = tx + twidth/2;
		int movingMiddle = moving->x() + distance + moving->width()/2;

		// move 'next' from the right side of the virtual container to
		// the left side if the middle of 'moving' has moved far enough
		// to the left, i.e. past the middle of the virtual container
		// plus the switchMargin. The switchMargin prevents rapidly
		// switching when 'moving' and 'next' have the same size.
		if (movingMiddle >= tmiddle + switchMargin) {
                    viewportToContents(next->x() - moving->width(), next->y(), nx, ny);
		    moveChild(next, nx, ny);
		    // store 'next', because it may become null in the next
		    // step.
		    last = next;
		    next = _containers.next();
		    continue;
		}

		// 'moving' doesn't cover 'next', and hasn't passed it. Then
		// we know that this also yields for the rest of the applets,
		// so leave the loop.
		break;
	    }

	    int newX;

	    if (last != moving) {
		newX = QMAX(last->x() + last->width(), moving->x() + distance);
                viewportToContents(newX, moving->y(), nx, ny);
		moveChild(moving, nx, ny);

		// Move 'moving' to its new position in the container list.
		_containers.removeRef(moving);
		_containers.insert( _containers.findRef(last) + 1, moving );
	    }
	    else
		if (next && moving->x() + distance >= next->x() - moving->width())
		    newX = next->x() - moving->width();
		else
		    newX = moving->x() + distance;

            if(newX >= width() - moving->width() - 80)
                scroll = true;

	    // Make sure the container isn't moved outside of the panel.
            viewportToContents(newX, moving->y(), nx, ny);
	    nx = QMIN(nx, contentsWidth() - moving->width());
	    moveChild(moving, nx, ny);

            if(scroll) {
                if(!_autoScrollTimer.isActive())
                    _autoScrollTimer.start(50);
                scrollBy(10, 0);
            }
	}

	else if (distance < 0) { // right to left

	    _containers.findRef(moving);
	    BaseContainer *prev = _containers.prev();
	    BaseContainer *last = moving;

	    while (prev) {

		if ( moving->x() + distance + moving->width() <= prev->x() ) {
                    viewportToContents(prev->x() + moving->width(), prev->y(), nx, ny);
		    moveChild(prev, nx, ny);
		    last = prev;
		    prev = _containers.prev();
		    continue;
		}

		int switchMargin = 0;

		// calculate the position and width of the 'virtual' container
		// containing 'moving' and 'prev'.
		int tx = prev->x();
		int twidth = moving->width() + prev->width();

		// determine the middle of the containers.
		int tmiddle = tx + twidth/2;
		int movingMiddle = moving->x() + distance + moving->width()/2;

		// move a from the left side of the virtual container to the
		// right side if the middle of 'moving' has moved past the
		// middle of the virtual container plus the switchMargin. The
		// switchMargin prevents rapidly switching when 'moving' and
		// 'prev' have the same size.
		if (movingMiddle <= tmiddle + switchMargin) {
                    viewportToContents(prev->x() + moving->width(), prev->y(), nx, ny);
		    moveChild(prev, nx, ny);
		    last = prev;
		    prev = _containers.prev();
		    continue;
		}

		break;
	    }

	    int newX;

	    if (last != moving) {
		newX = QMIN(last->x() - moving->width(), moving->x() + distance);
		// Move 'moving' to its new position in the container list.
		_containers.removeRef(moving);
		_containers.insert( _containers.findRef(last), moving );
	    }
	    else
		if (prev && moving->x() + distance < prev->x() + prev->width())
		    newX = prev->x() + prev->width();
		else
		    newX = moving->x() + distance;

            if(newX <= 80)
                scroll = true;

	    // Make sure the container isn't moved outside of the panel.
            viewportToContents(newX, moving->y(), nx, ny);
	    nx = QMAX(nx, 0);
            moveChild(moving, nx, ny);

            if(scroll) {
                if(!_autoScrollTimer.isActive())
                    _autoScrollTimer.start(50);
                scrollBy(-10, 0);
            }
	}
    }

    // vertical panel
    else if (orientation() == Vertical) {

	if (distance > 0) { // top to bottom

	    _containers.findRef(moving);
	    BaseContainer *next = _containers.next();
	    BaseContainer *last = moving;

	    while (next) {

		if ( next->y() + next->height() <= moving->y() + distance) {
                    viewportToContents(next->x(), next->y() - moving->height(), nx, ny);
		    moveChild(next, nx, ny);
		    last = next;
		    next = _containers.next();
		    continue;
		}

		int switchMargin = 0;

		// calculate the position and height of the 'virtual' container
		// containing 'moving' and 'next'.
		int ty = next->y() - moving->height();
		int theight = moving->height() + next->height();

		// determine the middle of the containers.
		int tmiddle = ty + theight/2;
		int movingMiddle = moving->y() + distance + moving->height()/2;

		// move 'next' from the bottom of the virtual container to the
		// top side if the middle of 'moving' has moved past the middle
		// of the virtual container plus the switchMargin. The
		// switchMargin prevents rapidly switching when 'moving' and
		// 'next' have the same size.
		if (movingMiddle >= tmiddle + switchMargin) {
                    viewportToContents(next->x(), next->y() - moving->height(), nx, ny);
		    moveChild(next, nx, ny);
		    last = next;
		    next = _containers.next();
		    continue;
		}

		break;
	    }

	    int newY;

	    if (last != moving) {
		newY = QMAX(last->y() + last->height(), moving->y() + distance);

		// Move 'moving' to its new position in the container list.
		_containers.removeRef(moving);
		_containers.insert( _containers.findRef(last) + 1, moving );
	    }
	    else
		if (next && moving->y() + distance >= next->y() - moving->height())
		    newY = next->y() - moving->height();
		else
		    newY = moving->y() + distance;

            if(newY >= height() - moving->height() - 80)
                scroll = true;

	    // Make sure the container isn't moved outside of the panel.
            viewportToContents(moving->x(), newY, nx, ny);
	    ny = QMIN(ny, contentsHeight() - moving->height());
	    moveChild(moving, nx, ny);

            if(scroll) {
                if(!_autoScrollTimer.isActive())
                    _autoScrollTimer.start(50);
                scrollBy(0, 10);
            }
	}

	else if (distance < 0) { // bottom to top

	    _containers.findRef(moving);
	    BaseContainer *prev = _containers.prev();
	    BaseContainer *last = moving;

	    while (prev) {

		if ( moving->y() + moving->height() <= prev->y() ) {
                    viewportToContents(prev->x(), prev->y() + moving->height(), nx, ny);
		    moveChild(prev, nx, ny);
		    last = prev;
		    prev = _containers.prev();
		    continue;
		}

		int switchMargin = 0;

		// calculate the position and height of the 'virtual' container
		// containing 'moving' and 'prev'.
		int ty = prev->y();
		int theight = moving->height() + prev->height();

		// determine the middle of the containers.
		int tmiddle = ty + theight/2;
		int movingMiddle = moving->y() + distance + moving->height()/2;

		// move 'prev' from the top of the virtual container to the
		// bottom if the middle of 'moving' has moved past the middle
		// of the virtual container plus the switchMargin. The
		// switchMargin prevents rapidly switching when 'moving' and
		// 'prev' have the same size.
		if (movingMiddle <= tmiddle + switchMargin) {
                    viewportToContents(prev->x(), prev->y() + moving->height(), nx, ny);
		    moveChild(prev, nx, ny);
		    last = prev;
		    prev = _containers.prev();
		    continue;
		}

		break;
	    }

	    int newY;

	    if (last != moving) {
		newY = QMIN(last->y() - moving->height(), moving->y() + distance);
                viewportToContents(moving->x(), newY, nx, ny);
		moveChild(moving, nx, ny);

		// Move 'moving' to its new position in the container list.
		_containers.removeRef(moving);
		_containers.insert( _containers.findRef(last), moving );
	    }
	    else
		if (prev && moving->y() + distance < prev->y() + prev->height())
		    newY = prev->y() + prev->height();
		else
		    newY = moving->y() + distance;

            if(newY <= 80)
                scroll = true;

	    // Make sure the container isn't moved outside of the panel.
            viewportToContents(moving->x(), newY, nx, ny);
	    ny = QMAX(ny, 0);
	    moveChild(moving, nx, ny);

            if(scroll) {
                if(!_autoScrollTimer.isActive())
                    _autoScrollTimer.start(50);
                scrollBy(0, - 10);
            }
	}
    }
}

int ContainerArea::moveContainerPush(BaseContainer* a, int distance)
{
    // Point the iterator 'it' to 'a'.
    QListIterator<BaseContainer> it(_containers);
    while (it.current() && it.current() != a)
	++it;

    return moveContainerPushRecursive(it, distance);
}

int ContainerArea::moveContainerPushRecursive(QListIterator<BaseContainer> it, int distanceRequest)
{
    int distanceAvailable, distanceMoved;
    BaseContainer* a = it.current();
    BaseContainer* b;

    if (orientation() == Horizontal) {

	if (distanceRequest < 0) {
	    // Find the previous container. If it exists, determine the
	    // distance between the two containers.
	    b = --it;
	    if (!b)
		distanceAvailable = -a->x();
	    else {
		distanceAvailable = b->x() - a->x() + b->width();
		if (distanceRequest - distanceAvailable < 0)
		    distanceAvailable += moveContainerPushRecursive(it, distanceRequest - distanceAvailable);
	    }
	    distanceMoved = QMAX(distanceRequest, distanceAvailable);
	}
	else if (distanceRequest > 0) {
	    // Find the next container. If it exists, determine the distance
	    // between the two containers.
	    b = ++it;
	    if (!b)
		distanceAvailable = width() - a->x() - a->width();
	    else {
		distanceAvailable = b->x() - a->x() - a->width();
		if (distanceRequest - distanceAvailable > 0)
		    distanceAvailable += moveContainerPushRecursive(it, distanceRequest - distanceAvailable);
	    }
	    distanceMoved = QMIN(distanceRequest, distanceAvailable);
	}
	else
	    return 0;

	moveChild(a, a->x() + distanceMoved, a->y());
	return distanceMoved;
    }
    else if (orientation() == Vertical) {

	if (distanceRequest < 0) {
	    // Find the previous container. If it exists, determine the
	    // distance between the current and the previous container.
	    // If this distance is not enough to comply to the request,
	    // try to push the previous container.
	    b = --it;
	    if (!b)
		distanceAvailable = -a->y();
	    else {
		distanceAvailable = b->y() - a->y() + b->height();
		if (distanceRequest - distanceAvailable < 0)
		    distanceAvailable += moveContainerPushRecursive(it, distanceRequest - distanceAvailable);
	    }
	    distanceMoved = QMAX(distanceRequest, distanceAvailable);
	}
	else if (distanceRequest > 0) {
	    // Find the next container. If it exists, determine the distance
	    // between the current and the next container. If this distance is
	    // not enough to comply to the request, try to push the next
	    // container.
	    b = ++it;
	    if (!b)
		distanceAvailable = height() - a->y() - a->height();
	    else {
		distanceAvailable = b->y() - a->y() - a->height();
		if (distanceRequest - distanceAvailable > 0)
		    distanceAvailable += moveContainerPushRecursive(it, distanceRequest - distanceAvailable);
	    }
	    distanceMoved = QMIN(distanceRequest, distanceAvailable);
	}
	else
	    return 0;

	moveChild(a, a->x(), a->y() + distanceMoved);
	return distanceMoved;
    }

    return 0;
}

int ContainerArea::position() const
{
    return static_cast<int>(_pos);
}

Direction ContainerArea::popupDirection() const
{
    Direction dir;
    switch (_pos)
	{
	case Left:
	    dir = dRight;
	    break;
	case Right:
	    dir = dLeft;
	    break;
	case Top:
	    dir = dDown;
	    break;
	case Bottom:
	default:
	    dir = dUp;
	    break;
	}
    return dir;
}

void ContainerArea::slotLayoutChildren()
{
    layoutChildren();
}

void ContainerArea::embeddedWindowDestroyed()
{
    if (sender() && sender()->inherits("ExternalAppletContainer"))
	removeContainer((ExternalAppletContainer*)sender());
}

void ContainerArea::layoutChildren()
{
    if (_block_relayout) return;

//    kdDebug(1210) << "ContainerArea::layoutChildren()" << endl;

    QSize newsize = size();
    int mus = minimumUsedSpace( orientation(), width(), height() );

    if (orientation() == Horizontal) {
	if (newsize.width() < mus)
	    newsize.setWidth(mus);
    }
    else {
	if (newsize.height() < mus)
	    newsize.setHeight(mus);
    }
    resizeContents(newsize.width(), newsize.height());

    int pos = 0;

    int occupiedspace = 0;
    int freespace = totalFreeSpace();

    QListIterator<BaseContainer> it(_containers);
    for ( ; it.current(); ++it )
	{
	    BaseContainer* a = (*it);
	
	    // get pointer to the nextapplet
	    ++it;
	    BaseContainer *next = (*it);
	    --it;

            float fs = a->freeSpace();
            if(fs > 1) fs = 1;

            float nfs = 0;
            if(next) {
                nfs = next->freeSpace();
                if(nfs > 1) nfs = 1;
            }

	    double fspace = fs * freespace;

	    if ((fspace - floor(fspace)) > 0.5)
		fspace += 1;
	    pos = static_cast<int>(fspace) + occupiedspace;

	    if (orientation() == Horizontal) {
		moveChild(a, pos, 0);
		int w = a->widthForHeight(height());
		if (isStretch(a)) {
		    if (next)
			a->resize(w + int((nfs - fs)*freespace), height());
		    else
			a->resize(width() - a->x(), height()); // FIXME
		}
		else
		    a->resize(w, height());
		occupiedspace += w;
	    }
	    else {
		moveChild(a, 0, pos);
		int h = a->heightForWidth(width());
		if (isStretch(a)) {
		    if (next)
			a->resize(width(), h + int((nfs - fs)*freespace));
		    else
			a->resize(width(), height() - a->y());
		}
		else
		    a->resize(width(), h);
		occupiedspace += h;
	    }
	}
}

void ContainerArea::dragEnterEvent(QDragEnterEvent *ev)
{
    ev->accept(QUriDrag::canDecode(ev));
    disableStretch();

    if (!_dragIndicator)
	_dragIndicator = new DragIndicator(this);
    if (orientation() == Horizontal)
	_dragIndicator->setPreferredSize(QSize(height(), height()));
    else
	_dragIndicator->setPreferredSize(QSize(width(), width()));

    _dragMoveOffset =
	QPoint(_dragIndicator->width()/2, _dragIndicator->height()/2);

    // Find the container before the position of the dragindicator.
    QListIterator<BaseContainer> it(_containers);
    it.toLast();
    while (it.current())
	{
	    BaseContainer* a = static_cast<BaseContainer*>(it.current());

	    if (orientation() == Horizontal && a->x() <
		    ev->pos().x() - _dragMoveOffset.x()
	      || orientation() == Vertical && a->y() <
		    ev->pos().y() - _dragMoveOffset.y() ) {
		_dragMoveAC = a;
		break;
	    }

	    --it;
	}

    if (orientation() == Horizontal)
	moveDragIndicator((ev->pos() - _dragMoveOffset).x());
    else
	moveDragIndicator((ev->pos() - _dragMoveOffset).y());

    _dragIndicator->show();
    QTimer::singleShot(30000, _dragIndicator, SLOT(hide()));
}

void ContainerArea::dragMoveEvent(QDragMoveEvent* ev)
{
    if (orientation() == Horizontal)
	moveDragIndicator((ev->pos() - _dragMoveOffset).x());
    else
	moveDragIndicator((ev->pos() - _dragMoveOffset).y());
}

void ContainerArea::dragLeaveEvent(QDragLeaveEvent*)
{
    _dragIndicator->hide();
    restoreStretch();
}

void ContainerArea::dropEvent(QDropEvent *ev)
{
    QStringList uriList;

    if (QUriDrag::decodeToUnicodeUris(ev, uriList)) {

        QStringList::ConstIterator it(uriList.begin());
        for (; it != uriList.end(); ++it) {

            BaseContainer* a;
            KURL url(*it);

            // see if it's a executable or directory
            if(url.isLocalFile() && !KDesktopFile::isDesktopFile(url.path()))
            {
                QFileInfo fi(url.path());
                if(fi.isDir())  // directory
                {
                    PanelDirDropMenu mnu;
                    switch(mnu.exec(mapToGlobal(ev->pos()))){
                        case PanelDirDropMenu::Browser:
                            a = new BrowserButtonContainer(viewport(), url.path(),
                                                           KMimeType::iconForURL(url));
                            break;
                        case PanelDirDropMenu::Url:
                            a = new URLButtonContainer(viewport(), *it);
                            break;
                        default:
                            _dragIndicator->hide();
                            restoreStretch();
                            return;
                    }
                }
                else if(fi.isExecutable())  // non-KDE executable
                {
                    QString pixmapFile;
                    KMimeType::pixmapForURL(url.path(), 0, KIcon::Panel, 0,
                                            KIcon::DefaultState, &pixmapFile);
                    PanelExeDialog dlg(url.path(), pixmapFile,
                                       QString::null, false, 0);
                    if(dlg.exec() == QDialog::Accepted){
				// KIconloader returns a full path, we only want name
                        QFileInfo iconfi(dlg.icon());
                        a = new ExeButtonContainer(viewport(), url.path(), iconfi.fileName(),
                                                   dlg.commandLine(),
                                                   dlg.useTerminal());
                    }
                    else
                        break;
                }
                else { // some unknown local file
                    a = new URLButtonContainer(viewport(), *it);
                }
            }
            else if (url.isLocalFile() && !ev->source())
            { // a local desktop file being dragged from an external program.
                // Make a copy first.
                QString file = locateLocal("appdata",url.fileName());
                KURL dest;
                dest.setPath(file);
                KIO::NetAccess::upload(url.path(), dest); // Copy
                a = new URLButtonContainer(viewport(), file);
            }
            else
            { // a internet URL
                a= new URLButtonContainer(viewport(), *it);
            }

	    // Move the neighbour containers if there isn't enough space
	    if (_dragIndicator->size() != _dragIndicator->preferredSize()) {
		int neededSpace;
		int distanceMoved;
		BaseContainer* next;

		if (_dragMoveAC) {
		    _containers.findRef(_dragMoveAC);
		    next = _containers.next();
		}
		else
		    next = _containers.first();

		if (orientation() == Horizontal) {
		    neededSpace = _dragIndicator->preferredSize().width()
                                  - _dragIndicator->width();
		    if (_dragMoveAC) {
			distanceMoved =
			    moveContainerPush(_dragMoveAC, -neededSpace/2);
			_dragIndicator->move(
			    _dragIndicator->x() + distanceMoved,
			    _dragIndicator->y());
			neededSpace += distanceMoved;
		    }
		    if (next)
			neededSpace -= moveContainerPush(next, neededSpace);
		    if (_dragMoveAC) {
			distanceMoved =
			    moveContainerPush(_dragMoveAC, -neededSpace);
			_dragIndicator->move(
			    _dragIndicator->x() + distanceMoved,
			    _dragIndicator->y());
		    }
		}
		else {
		    neededSpace = _dragIndicator->preferredSize().height()
                                  - _dragIndicator->height();
		    if (_dragMoveAC) {
			distanceMoved =
			    moveContainerPush(_dragMoveAC, -neededSpace/2);
			_dragIndicator->move(
			    _dragIndicator->x(),
			    _dragIndicator->y() + distanceMoved);
			neededSpace += distanceMoved;
		    }
		    if (next)
			neededSpace -= moveContainerPush(next, neededSpace);
		    if (_dragMoveAC) {
			distanceMoved =
			    moveContainerPush(_dragMoveAC, -neededSpace);
			_dragIndicator->move(
			    _dragIndicator->x(),
			    _dragIndicator->y() + distanceMoved);
		    }
		}
	    }

            addContainer(a);
	    moveChild(a, _dragIndicator->x(), _dragIndicator->y());
	    updateContainerList();
	    saveContainerConfig();
        }
    }
    _dragIndicator->hide();
    restoreStretch();
}

void ContainerArea::resizeEvent(QResizeEvent *ev)
{
    Panner::resizeEvent(ev);

    setBackgroundTheme();
}

void ContainerArea::setBackgroundTheme()
{
    // set background pixmap
    KConfigGroupSaver saver(_config, "General");
    if (_config->readBoolEntry("UseBackgroundTheme", false)) {
	QString bgStr = _config->readEntry("BackgroundTheme", "");
        bgStr = locate("appdata", bgStr);
	if(!bgStr.isEmpty()){
	    QPixmap bgPix(bgStr);
	    if(!bgPix.isNull()){
		// Do we need to rotate the image
		QPixmap bgPixNew;

		if ( _config->readBoolEntry("RotateBackground", false) &&
		     orientation() == Vertical )
		    {
				// Rotate the pixmap before scaling
			QWMatrix m;
			m.rotate( -90.0 );
			bgPixNew = bgPix.xForm( m );
		    }
		else
		    {
				// Don't rotate the image - just copy it
			bgPixNew = bgPix;
		    }

		// Scale the image but keep the same aspect ratio
		QImage bgImage = bgPixNew.convertToImage();
		double dAspect = (double)bgPixNew.width() / (double)bgPixNew.height();

		int nNewWidth = width();
		int nNewHeight = height();
		if (orientation() == Vertical )
		    nNewHeight = (int)( (double)nNewWidth / dAspect );
		else
		    nNewWidth  = (int)( (double)nNewHeight * dAspect);
		QImage bgImageNew = bgImage.smoothScale( nNewWidth, nNewHeight );

		// Convert back to a QPixmap
		bgPixNew.convertFromImage( bgImageNew );

		QBrush bgBrush(colorGroup().background(), bgPixNew);
		QPalette pal = kapp->palette();
		pal.setBrush(QColorGroup::Background, bgBrush);
		setPalette(pal);
	    }
	    else {
		unsetPalette();
		kdWarning() << "Kicker: Error loading background theme pixmap\n";
	    }
	}
    }
    else {
	unsetPalette();
    }
}

QRect ContainerArea::availableSpaceFollowing(BaseContainer* a)
{
    QRect availableSpace = rect();
    BaseContainer* b;

    if (a) {
	_containers.findRef(a);
	b = _containers.next();
    }
    else
	b = _containers.first();

    if (orientation() == Horizontal) {
	if (a)
	    availableSpace.setLeft(a->x() + a->width());
	if (b)
	    availableSpace.setRight(b->x() - 1);
    }
    else {
	if (a)
	    availableSpace.setTop(a->y() + a->height());
	if (b)
	    availableSpace.setBottom(b->y() - 1);
    }

    return availableSpace;
}

void ContainerArea::moveDragIndicator(int pos)
{
    QRect availableSpace = availableSpaceFollowing(_dragMoveAC);

    // Move _dragIndicator to position pos, restricted by availableSpace.
    // Resize _dragIndicator if necessary.
    if (orientation() == Horizontal) {
	if (availableSpace.size().width() <
	      _dragIndicator->preferredSize().width()) {
	    _dragIndicator->resize(availableSpace.size());
	    _dragIndicator->move(availableSpace.topLeft());
	}
	else {
	    int newX = pos;
	    _dragIndicator->resize(_dragIndicator->preferredSize());
	    newX = QMAX(newX, availableSpace.left());
	    newX = QMIN(newX,
	            availableSpace.right() + 1 - _dragIndicator->width() );
	    _dragIndicator->move(newX, availableSpace.top());
	}
    }
    else {
	if (availableSpace.size().height() <
	      _dragIndicator->preferredSize().height()) {
	    _dragIndicator->resize(availableSpace.size());
	    _dragIndicator->move(availableSpace.topLeft());
	}
	else {
	    int newY = pos;
	    _dragIndicator->resize(_dragIndicator->preferredSize());
	    newY = QMAX(newY, availableSpace.top());
	    newY = QMIN(newY,
	            availableSpace.bottom() + 1 - _dragIndicator->height() );
	    _dragIndicator->move(availableSpace.left(), newY);
	}
    }
}

void ContainerArea::moveToFirstFreePosition(BaseContainer* a)
{
    Orientation orient = orientation();

    int w = a->widthForHeight(height());
    int h = a->heightForWidth(width());

    bool stretch = false;
    bool found = false;

    QListIterator<BaseContainer> it(_containers);
    for(; it.current() ; ++it)
	{
	    BaseContainer* b = static_cast<BaseContainer*>(it.current());

	    int space = relativeContainerPos(b);

	    if (orient == Horizontal) {
		if (space >= w)
		    {
			if(stretch)
			    moveChild(a, b->x() - w, a->y());
			else
			    moveChild(a, b->x() - space, a->y());
			found = true;
			break;
		    }
	    }
	    else {
		if (space >= h)
		    {
			if(stretch)
			    moveChild(a, a->x(), b->y() - h);
			else
			    moveChild(a, a->x(), b->y() - space);
			found = true;
			break;
		    }
	    }
	    stretch = isStretch(b);
	}

    if (found)
	updateContainerList();
    else {
//        kdDebug() << "ContainerArea::moveToFirstFreePosition: trail" << endl;
        BaseContainer* last = _containers.last();

        if(orient == Horizontal)
            moveChild(a, last->x() + last->width() + 1, a->y());
        else
            moveChild(a, a->x(), last->y() + last->height() + 1);
    }
    layoutChildren();
}

BaseContainer* ContainerArea::coversContainer(BaseContainer *a, bool strict)
{
    BaseContainer *b;
    QListIterator<BaseContainer> it(_containers);

    for(; it.current() ; ++it)
	{
	    b = (BaseContainer*)it.current();

	    if (b == a) continue;

	    if ( orientation() == Horizontal ) {
		int bl, br;
		if (strict) {
		    bl = b->x();
		    br = b->x() + b->width();
		}
		else {
		    bl = b->x() + 10;
		    br = b->x() + b->width() - 10;
		}

		int btnl = a->x();
		int btnr = btnl + a->width();

		if ((btnl >= bl) && (btnl <= br))
		    return b;
		else if ((btnr >= bl) && (btnr <= br))
		    return b;
	    }
	    else {
		int bt, bb;
		if (strict) {
		    bt = b->y();
		    bb = b->y() + b->height();
		}
		else {
		    bt = b->y() + 10;
		    bb = b->y() + b->height() - 10;
		}
		int btnt = a->y();
		int btnb = btnt + a->height();

		if ((btnt >= bt) && (btnt <= bb))
		    return b;
		else if ((btnb >= bt) && (btnb <= bb))
		    return b;
	    }
	}
    return 0;
}

void ContainerArea::updateContainerList()
{
    QList<BaseContainer> sorted;

    while(!_containers.isEmpty())
	{
	    BaseContainer *b = 0;
	    int pos = 9999;

	    QListIterator<BaseContainer> it(_containers);

	    for(; it.current() ; ++it)
		{
		    BaseContainer* a = static_cast<BaseContainer*>(it.current());

		    if(orientation() == Horizontal)
			{
			    if (a->x() < pos) {
				b = a;
				pos = a->x();
			    }
			}
		    else
			{
			    if (a->y() < pos) {
				b = a;
				pos = a->y();
			    }
			}
		}

	    if (b) {
		sorted.append(b);
		_containers.remove(b);
	    }
	}
    _containers = sorted;

    float freespace = totalFreeSpace();
    float fspace = 0;

    QListIterator<BaseContainer> it(_containers);
    for(; it.current() ; ++it)
	{
	    fspace += relativeContainerPos(it.current());
	    if (fspace < 0) fspace = 0;
            double ssf = fspace/freespace;
            if (ssf > 1) ssf = 1;
            if(ssf < 0) ssf = 0;
	    it.current()->setFreeSpace(ssf);
	}
}

int ContainerArea::totalFreeSpace() const
{
    int availablespace;

    if(orientation() == Horizontal) {
	if(contentsWidth() > width())
	    availablespace = contentsWidth();
	else
	    availablespace = width();
    }
    else {
	if (contentsHeight() > height())
	    availablespace = contentsHeight();
	else
	    availablespace = height();
    }

    int freespace = availablespace - minimumUsedSpace( orientation(), width(), height() );
    if (freespace < 0) freespace = 0;

    return freespace;
}

int ContainerArea::minimumUsedSpace( Orientation o, int w, int h ) const
{
    int usedspace = 0;

    QListIterator<BaseContainer> it(_containers);
    for(; it.current() ; ++it)
	{
	    BaseContainer* a = static_cast<BaseContainer*>(it.current());

	    int space;
	    if(o == Horizontal) {
		space = a->widthForHeight(h);
	    } else
		space = a->heightForWidth(w);

	    if (space > 0)
		usedspace += space;
	}
    return usedspace;
}

int ContainerArea::relativeContainerPos(BaseContainer* b) const
{
    if (!b) return 0;
    if (!_containers.contains(b)) return 0;

    uint pos = 0;

    QListIterator<BaseContainer> it(_containers);
    for(; it.current() ; ++it) {
	BaseContainer* a = static_cast<BaseContainer*>(it.current());

	if (orientation() == Horizontal) {
	    if (a == b)  {
		int p = b->x() - pos;
		if (b < 0) return 0;
		return p;
	    } else
		pos = a->x() + a->widthForHeight(height());
	} else  {
	    if (a == b) {
		int p = b->y() - pos;
		if (b < 0) return 0;
		return p;
	    }
	    else
		pos = a->y() + a->heightForWidth(width());
	}
    }
    return 0;
}

void ContainerArea::slotSaveContainerConfig()
{
    saveContainerConfig();
}

void ContainerArea::slotRemoveContainer(BaseContainer* a)
{
    removeContainer(a);
}

void ContainerArea::setOrientation(Orientation o)
{
    Panner::setOrientation(o);

//    kdDebug(1210) << "ContainerArea::setOrientation()" << endl;

    QListIterator<BaseContainer> it(_containers);
    for ( ; it.current(); ++it )
        (*it)->slotSetOrientation(o);
}

void ContainerArea::setPosition(Position p)
{
    _pos = p;

    QListIterator<BaseContainer> it(_containers);
    for ( ; it.current(); ++it )
        (*it)->slotSetPopupDirection( popupDirection() );
}

void ContainerArea::autoScroll()
{
    if(!_moveAC) return;

    if(orientation() == Horizontal) {
        if(_moveAC->pos().x() <= 80)
            scrollBy(-10, 0);
        else if(_moveAC->pos().x() >= width() - _moveAC->width() - 80)
            scrollBy(10, 0);
    }
    else {
        if(_moveAC->pos().y() <= 80)
            scrollBy(0, -10);
        else if(_moveAC->pos().y() >= height() - _moveAC->height() - 80)
            scrollBy(0, 10);
    }
}

void DragIndicator::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QRect rect(0, 0, width(), height());
    style().drawFocusRect(&painter, rect, colorGroup(), &colorGroup().base());
}

void DragIndicator::mousePressEvent(QMouseEvent*)
{
    hide();
}

void ContainerArea::scrollTo(BaseContainer* b)
{
    if(!b) return;

    int x, y;
    viewportToContents(b->pos().x(), b->pos().y(), x, y);
    ensureVisible(x, y);
}
