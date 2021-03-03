/*****************************************************************

Copyright (c) 2000 Matthias Elter <elter@kde.org>

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

#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kiconloader.h>
#include <kwinmodule.h>
#include <netwm.h>
#include <qtimer.h>
#include <qimage.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "taskmanager.h"
#include "taskmanager.moc"

template class QList<Task>;
KWinModule* kwin_module = 0;

TaskManager::TaskManager(QObject *parent, const char *name)
    : QObject(parent, name), _active(0), _startup_info( NULL )
{
    // create and connect kwin module
    kwin_module = new KWinModule(this);

    connect(kwin_module, SIGNAL(windowAdded(WId)), SLOT(windowAdded(WId)));
    connect(kwin_module, SIGNAL(windowRemoved(WId)), SLOT(windowRemoved(WId)));
    connect(kwin_module, SIGNAL(activeWindowChanged(WId)), SLOT(activeWindowChanged(WId)));
    connect(kwin_module, SIGNAL(currentDesktopChanged(int)), SLOT(currentDesktopChanged(int)));
    connect(kwin_module, SIGNAL(windowChanged(WId,unsigned int)), SLOT(windowChanged(WId,unsigned int)));

    // register existing windows
    const QValueList<WId> windows = kwin_module->windows();
    for (QValueList<WId>::ConstIterator it = windows.begin(); it != windows.end(); ++it )
	windowAdded(*it);

    // set active window
    WId win = kwin_module->activeWindow();
    activeWindowChanged(win);
    
    configure_startup();
}

TaskManager::~TaskManager()
{
}

void TaskManager::configure_startup()
{
    KConfig c("klaunchrc", true);
    c.setGroup("FeedbackStyle");
    if (!c.readBoolEntry("TaskbarButton", true))
        return;
    _startup_info = new KStartupInfo( true, this );
    connect( _startup_info,
        SIGNAL( gotNewStartup( const KStartupInfoId&, const KStartupInfoData& )),
        SLOT( gotNewStartup( const KStartupInfoId&, const KStartupInfoData& )));
    connect( _startup_info,
        SIGNAL( gotStartupChange( const KStartupInfoId&, const KStartupInfoData& )),
        SLOT( gotStartupChange( const KStartupInfoId&, const KStartupInfoData& )));
    connect( _startup_info,
        SIGNAL( gotRemoveStartup( const KStartupInfoId&, const KStartupInfoData& )),
        SLOT( gotRemoveStartup( const KStartupInfoId& )));
    c.setGroup( "TaskbarButtonSettings" );
    _startup_info->setTimeout( c.readUnsignedNumEntry( "Timeout", 30 ));
}

Task* TaskManager::findTask(WId w)
{
    for (Task* t = _tasks.first(); t != 0; t = _tasks.next())
        if (t->window() == w  || t->hasTransient(w))
            return t;
    return 0;
}

void TaskManager::windowAdded(WId w )
{
    NETWinInfo info (qt_xdisplay(),  w, qt_xrootwin(),
		     NET::WMWindowType | NET::WMPid | NET::WMState );

    // ignore NET::Tool and other special window types
    if (info.windowType() != NET::Normal
        && info.windowType() != NET::Override
        && info.windowType() != NET::Unknown)
	return;

    // ignore windows that want to be ignored by the taskbar
    if ((info.state() & NET::SkipTaskbar) != 0)
	return;

    // lets see if this is a transient for an existing task
    Window transient_for;
    if (XGetTransientForHint( qt_xdisplay(), (Window) w, &transient_for )
        && (WId) transient_for != qt_xrootwin()
        && transient_for != 0 ) {

        Task* t = findTask((WId) transient_for);
	if (t) {
	    if (t->window() != w) {
		t->addTransient(w);
                // kdDebug() << "TM: Transient " << w << " added for Task: " << t->window() << endl;
            }
	    return;
	}
    }

    Task* t = new Task(w, this);
    _tasks.append(t);

    // kdDebug() << "TM: Task added for WId: " << w << endl;

    emit taskAdded(t);
}

void TaskManager::windowRemoved(WId w )
{
    // find task
    Task* t = findTask(w);
    if (!t) return;

    if (t->window() == w) {
        _tasks.removeRef(t);

        emit taskRemoved(t);

        if(t == _active) _active = 0;
        delete t;
        //kdDebug() << "TM: Task for WId " << w << " removed." << endl;
    }
    else {
        t->removeTransient( w );
        //kdDebug() << "TM: Transient " << w << " for Task " << t->window() << " removed." << endl;
    }
}

void TaskManager::windowChanged(WId w, unsigned int dirty)
{
    if( dirty & NET::WMState )
        {
        NETWinInfo info ( qt_xdisplay(),  w, qt_xrootwin(), NET::WMState );
        if ( (info.state() & NET::SkipTaskbar) != 0 )
            {
            windowRemoved( w );
            return;
            }
        else if( !findTask( w ))
            windowAdded( w ); // skipTaskBar state was removed, so add this window
        }

    // check if any state we are interested in is marked dirty
    if(!(dirty & (NET::WMVisibleName|NET::WMName|NET::WMState|NET::WMIcon|NET::XAWMState|NET::WMDesktop)) )
        return;

    // find task
    Task* t = findTask( w );
    if (!t) return;

    //kdDebug() << "TaskManager::windowChanged " << w << " " << dirty << endl;


    // refresh icon pixmap if necessary
    if (dirty & NET::WMIcon)
        t->refresh(true);
    else
        t->refresh();

    if(dirty & (NET::WMDesktop|NET::WMState))
        emit windowDesktopChanged(w); // moved to different desktop or is on all
}

void TaskManager::activeWindowChanged(WId w )
{
    //kdDebug() << "TaskManager::activeWindowChanged" << endl;

    Task* t = findTask( w );
    if (!t) {
        if (_active) {
            _active->setActive(false);
            _active = 0;
        }
    }
    else {
        if (_active)
            _active->setActive(false);

        _active = t;
        _active->setActive(true);
    }
}

void TaskManager::currentDesktopChanged(int desktop)
{
    emit desktopChanged(desktop);
}

void TaskManager::gotNewStartup( const KStartupInfoId& id, const KStartupInfoData& data )
{
    Startup* s = new Startup( id, data, this );
    _startups.append(s);

    emit startupAdded(s);
}

void TaskManager::gotStartupChange( const KStartupInfoId& id, const KStartupInfoData& data )
{
    for( Startup* s = _startups.first(); s != 0; s = _startups.next()) {
        if ( s->id() == id ) {
            s->update( data );
            return;
        }
    }
}

void TaskManager::gotRemoveStartup( const KStartupInfoId& id )
{
    killStartup( id );
}

void TaskManager::killStartup( const KStartupInfoId& id )
{
    Startup* s = 0;
    for(s = _startups.first(); s != 0; s = _startups.next()) {
        if (s->id() == id)
            break;
    }
    if (s == 0) return;

    _startups.removeRef(s);
    emit startupRemoved(s);
    delete s;
}

void TaskManager::killStartup(Startup* s)
{
    if (s == 0) return;

    _startups.removeRef(s);
    emit startupRemoved(s);
    delete s;
}

QString TaskManager::desktopName(int desk) const
{
    return kwin_module->desktopName(desk);
}

int TaskManager::numberOfDesktops() const
{
    return kwin_module->numberOfDesktops();
}

bool TaskManager::isOnTop(Task* task)
{
    if(!task) return false;

    Task* t = 0;

    for (QValueList<WId>::ConstIterator it = kwin_module->stackingOrder().fromLast();
         it != kwin_module->stackingOrder().end(); --it ) {

        t = findTask(*it);
        if ( t == task )
            return true;
        if ( t && !t->isIconified() && (t->isAlwaysOnTop() == task->isAlwaysOnTop()) )
            return false;
    }
    return false;
}


Task::Task(WId win, QObject * parent, const char *name)
  : QObject(parent, name),
    _active(false), _win(win),
    _lastWidth(0), _lastHeight(0), _lastResize(false), _lastIcon(),
    _thumbSize(0.2), _thumb(), _grab()
{
    _info = KWin::info(_win);

    // try to load icon via net_wm
    _pixmap = KWin::icon(_win, 16, 16, true);

    // try to guess the icon from the classhint
    if(_pixmap.isNull())
      KGlobal::instance()->iconLoader()->loadIcon(className().lower(),
						  KIcon::Small,KIcon::Small,
						  KIcon::DefaultState, 0, true);

    // load xapp icon
    if (_pixmap.isNull())
      _pixmap = SmallIcon("kcmx");
}

Task::~Task()
{
}

void Task::refresh(bool icon)
{
    _info = KWin::info(_win);
    if (icon) {

        // try to load icon via net_wm
        _pixmap = KWin::icon(_win, 16, 16, true);

        // try to guess the icon from the classhint
        if(_pixmap.isNull())
            KGlobal::instance()->iconLoader()->loadIcon(className().lower(), KIcon::Small,
                                                        KIcon::Small, KIcon::DefaultState, 0, true);

        // load xapp icon
        if (_pixmap.isNull())
            _pixmap = SmallIcon("kcmx");

	_lastIcon.resize(0,0);
	emit iconChanged();
    }
    emit changed();
}

void Task::setActive(bool a)
{
    _active = a;
    emit changed();
    if ( a )
      emit activated();
    else
      emit deactivated();
}

bool Task::isMaximized() const
{
    return(_info.state & NET::Max);
}

bool Task::isIconified() const
{
    return (_info.mappingState == NET::Iconic);
}

bool Task::isAlwaysOnTop() const
{
    return (_info.state & NET::StaysOnTop);
}

bool Task::isShaded() const
{
    return (_info.state & NET::Shaded);
}

bool Task::isOnCurrentDesktop() const
{
    return (_info.onAllDesktops || _info.desktop == kwin_module->currentDesktop());
}

bool Task::isOnAllDesktops() const
{
    return _info.onAllDesktops;
}

bool Task::isActive() const
{
    return _active;
}

bool Task::isModified() const
{
  static QString modStr = QString::fromUtf8("[") + i18n("modified") + QString::fromUtf8("]");
  int modStrPos = _info.visibleName.find(modStr);

  return ( modStrPos != -1 );
}

QString Task::iconName() const
{
    NETWinInfo ni( qt_xdisplay(),  _win, qt_xrootwin(), NET::WMIconName);
    return QString::fromUtf8(ni.iconName());
}
QString Task::visibleIconName() const
{
    NETWinInfo ni( qt_xdisplay(),  _win, qt_xrootwin(), NET::WMVisibleIconName);
    return QString::fromUtf8(ni.visibleIconName());
}

QString Task::className()
{
    XClassHint hint;
    if(XGetClassHint(qt_xdisplay(), _win, &hint))
        return QString(hint.res_name);
    return QString::null;
}

QPixmap Task::icon( int width, int height, bool allowResize )
{
  if ( (width == _lastWidth)
       && (height == _lastHeight)
       && (allowResize == _lastResize )
       && (!_lastIcon.isNull()) )
    return _lastIcon;

  QPixmap newIcon = KWin::icon( _win, width, height, allowResize );
  if ( !newIcon.isNull() ) {
    _lastIcon = newIcon;
    _lastWidth = width;
    _lastHeight = height;
    _lastResize = allowResize;
  }

  return newIcon;
}

QPixmap Task::bestIcon( int size, bool &isStaticIcon )
{
  QPixmap pixmap;
  isStaticIcon = false;

  switch( size ) {
  case KIcon::SizeSmall:
    {
      pixmap = icon( 16, 16, true  );
      
      // Icon of last resort
      if( pixmap.isNull() ) {
	pixmap = KGlobal::iconLoader()->loadIcon( "go",
						  KIcon::NoGroup,
						  KIcon::SizeSmall );
	isStaticIcon = true;
      }
    }
    break;
  case KIcon::SizeMedium:
    {
      //
      // Try 34x34 first for KDE 2.1 icons with shadows, if we don't
      // get one then try 32x32.
      //
      pixmap = icon( 34, 34, false  );

      if ( ( pixmap.width() != 34 ) || ( pixmap.height() != 34 ) ) {
	if ( ( pixmap.width() != 32 ) || ( pixmap.height() != 32 ) ) {
	  pixmap = icon( 32, 32, true  );
	}
      }

      // Icon of last resort
      if( pixmap.isNull() ) {
	pixmap = KGlobal::iconLoader()->loadIcon( "go",
						  KIcon::NoGroup,
						  KIcon::SizeMedium );
	isStaticIcon = true;
      }
    }
    break;
  case KIcon::SizeLarge:
    {
      // If there's a 48x48 icon in the hints then use it
      pixmap = icon( size, size, false  );
      
      // If not, try to get one from the classname
      if ( pixmap.isNull() || ( pixmap.width() != size ) || ( pixmap.height() != size ) ) {
	pixmap = KGlobal::iconLoader()->loadIcon( className(),
						  KIcon::NoGroup,
						  size,
						  KIcon::DefaultState,
						  0L,
						  true );
	isStaticIcon = true;
      }
      
      // If we still don't have an icon then scale the one in the hints
      if ( pixmap.isNull() || ( pixmap.width() != size ) || ( pixmap.height() != size ) ) {
	pixmap = icon( size, size, true  );
	isStaticIcon = false;
      }

      // Icon of last resort
      if( pixmap.isNull() ) {
	pixmap = KGlobal::iconLoader()->loadIcon( "go",
						  KIcon::NoGroup,
						  size );
	isStaticIcon = true;
      }
    }
  }

  return pixmap;
}

bool Task::idMatch( const QString& id1, const QString& id2 )
{
  if ( id1.isEmpty() || id2.isEmpty() )
    return false;

  if ( id1.contains( id2 ) > 0 )
    return true;
  
  if ( id2.contains( id1 ) > 0 )
    return true;
  
  // add hacks here ;-)
    if ( ( id1 == "navigator" && id2 == "netscape")
	 || ( id1 == "netscape" && id2 == "navigator")
	 || ( id1 == "kfmclient" && id2 == "konqueror")
	 || ( id1 == "konqueror" && id2 == "kfmclient")
	 || ( id1 == "command_shell" && id2 == "ddd" )
	 || ( id1 == "ddd" && id2 == "command_shell" )
	 || ( id1 == "gimp_startup" && id2 == "toolbox" )
	 || ( id1 == "toolbox" && id2 == "gimp_startup" )
	 || ( id1 == "gimp" && id2 == "toolbox" )
	 || ( id1 == "toolbox" && id2 == "gimp" )
	 || ( id1 == "xmms" && id2 == "xmms_player" )
	 || ( id1 == "xmms_player" && id2 == "xmms" )
	)
      return true;

    return false;
}


void Task::maximize()
{
    NETWinInfo ni( qt_xdisplay(),  _win, qt_xrootwin(), NET::WMState);
    ni.setState( NET::Max, NET::Max );

    if (_info.mappingState == NET::Iconic)
        activate();
}

void Task::restore()
{
    NETWinInfo ni( qt_xdisplay(),  _win, qt_xrootwin(), NET::WMState);
    ni.setState( 0, NET::Max );

    if (_info.mappingState == NET::Iconic)
        activate();
}

void Task::iconify()
{
    XIconifyWindow( qt_xdisplay(), _win, qt_xscreen() );
}

void Task::close()
{
    NETRootInfo ri( qt_xdisplay(),  NET::CloseWindow );
    ri.closeWindowRequest( _win );
}

void Task::raise()
{
    XRaiseWindow( qt_xdisplay(), _win );
}

void Task::activate()
{
    NETRootInfo ri( qt_xdisplay(), 0 );
    ri.setActiveWindow( _win );
}

void Task::toDesktop(int desk)
{
    NETWinInfo ni(qt_xdisplay(), _win, qt_xrootwin(), NET::WMDesktop);
    if (desk == 0) {
        if (_info.onAllDesktops)
            ni.setDesktop(kwin_module->currentDesktop());
        else
            ni.setDesktop(NETWinInfo::OnAllDesktops);
        return;
    }
    ni.setDesktop(desk);
}

void Task::toCurrentDesktop()
{
    toDesktop(kwin_module->currentDesktop());
}

void Task::setAlwaysOnTop(bool stay)
{
    NETWinInfo ni( qt_xdisplay(),  _win, qt_xrootwin(), NET::WMState);
    if(stay)
        ni.setState( NET::StaysOnTop, NET::StaysOnTop );
    else
        ni.setState( 0, NET::StaysOnTop );
}

void Task::setShaded(bool shade)
{
    NETWinInfo ni( qt_xdisplay(),  _win, qt_xrootwin(), NET::WMState);
    if(shade)
        ni.setState( NET::Shaded, NET::Shaded );
    else
        ni.setState( 0, NET::Shaded );
}

void Task::publishIconGeometry(QRect rect)
{
    NETWinInfo ni( qt_xdisplay(),  _win, qt_xrootwin(), NET::WMIconGeometry);
    NETRect r;
    r.pos.x = rect.x();
    r.pos.y = rect.y();
    r.size.width = rect.width();
    r.size.height = rect.height();
    ni.setIconGeometry(r);
}

void Task::updateThumbnail()
{
  if ( !isOnCurrentDesktop() )
    return;
  if ( !isActive() )
    return;
  if ( !_grab.isNull() ) // We're already processing one...
    return;

   //
   // We do this as a two stage process to remove the delay caused
   // by the thumbnail generation. This makes things much smoother
   // on slower machines.
   //
   _grab = QPixmap::grabWindow( _win );

   if ( !_grab.isNull() )
     QTimer::singleShot( 200, this, SLOT( generateThumbnail() ) );
}

void Task::generateThumbnail()
{
   if ( _grab.isNull() )
      return;

   QImage img = _grab.convertToImage();

   double width = img.width();
   double height = img.height();
   width = width * _thumbSize;
   height = height * _thumbSize;

   img = img.smoothScale( width, height );
   _thumb = img;
   _grab.resize( 0, 0 ); // Makes grab a null image.

   emit thumbnailChanged();
}

Startup::Startup( const KStartupInfoId& id, const KStartupInfoData& data,
    QObject * parent, const char *name)
    : QObject(parent, name), _id( id ), _data( data )
{
}

Startup::~Startup()
{

}

void Startup::update( const KStartupInfoData& data )
{
    _data.update( data );
    emit changed();
}

int TaskManager::currentDesktop() const
{
    return kwin_module->currentDesktop();
}
