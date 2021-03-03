/*****************************************************************

Copyright (c) 2001 the kicker authors. See file AUTHORS.

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

#include <stdlib.h>
#include <math.h>

#include <qlayout.h>
#include <qtooltip.h>
#include <qtimer.h>

#include <kwin.h>
#include <kconfig.h>
#include <klocale.h>
#include <kglobalsettings.h>
#include <kwinmodule.h>
#include <kapp.h>
#include <kdebug.h>
#include <karrowbutton.h>

#include "panelop_mnu.h"
#include "extensionmanager.h"
#include "userrectsel.h"

#include "container_panel.h"
#include "container_panel.moc"

#include <X11/Xlib.h>
const int XFocusOut = FocusOut;
const int XFocusIn = FocusIn;
#undef FocusOut
#undef FocusIn
#undef KeyPress
#undef KeyRelease

extern Time qt_x_time;
extern void qt_set_sane_enterleave( bool b );

QList<PanelContainer> PanelContainer::_containers;

PanelSettings::PanelSettings()
{
    // defaults
    _position          = Bottom;
    _HBwidth           = 14;
    _showLeftHB        = false;
    _showRightHB       = true;
    _autoHide          = false;
    _autoHideSwitch    = false;
    _autoHideDelay     = 3;
    _hideAnim          = true;
    _autoHideAnim      = true;
    _hideAnimSpeed     = 40;
    _autoHideAnimSpeed = 40;
    _showToolTips      = true;
    _sizePercentage    = 100;
    _expandSize        = true;
}

void PanelSettings::readConfig( KConfig *c )
{
    _position          = static_cast<Position>(c->readNumEntry("Position", _position));
    _HBwidth           = c->readNumEntry(  "HideButtonSize",         _HBwidth);
    _showLeftHB        = c->readBoolEntry( "ShowLeftHideButton",     _showLeftHB);
    _showRightHB       = c->readBoolEntry( "ShowRightHideButton",    _showRightHB);
    _autoHide          = c->readBoolEntry( "AutoHidePanel",          _autoHide);
    _autoHideSwitch    = c->readBoolEntry( "AutoHideSwitch",         _autoHideSwitch);
    _autoHideDelay     = c->readNumEntry(  "AutoHideDelay",          _autoHideDelay);
    _hideAnim          = c->readBoolEntry( "HideAnimation",          _hideAnim);
    _autoHideAnim      = c->readBoolEntry( "AutoHideAnimation",      _autoHideAnim);
    _hideAnimSpeed     = c->readNumEntry(  "HideAnimationSpeed",     _hideAnimSpeed);
    _autoHideAnimSpeed = c->readNumEntry(  "AutoHideAnimationSpeed", _autoHideAnimSpeed);
    _showToolTips      = c->readBoolEntry( "ShowToolTips",           _showToolTips );
    _sizePercentage    = c->readNumEntry(  "SizePercentage",         _sizePercentage );
    _expandSize        = c->readBoolEntry( "ExpandSize",             _expandSize );

    // sanitize
    if (_HBwidth < 3) _HBwidth = 3;
    if (_HBwidth > 24) _HBwidth = 24;

    if ( _sizePercentage < 1 ) _sizePercentage = 1;
    if ( _sizePercentage > 100 ) _sizePercentage = 100;
}

void PanelSettings::writeConfig( KConfig *c ) {
    c->writeEntry( "Position", static_cast<int>(_position));
    c->writeEntry( "HideButtonSize",         _HBwidth);
    c->writeEntry( "ShowLeftHideButton",     _showLeftHB);
    c->writeEntry( "ShowRightHideButton",    _showRightHB);
    c->writeEntry( "AutoHidePanel",          _autoHide);
    c->writeEntry( "AutoHideSwitch",         _autoHideSwitch);
    c->writeEntry( "AutoHideDelay",          _autoHideDelay);
    c->writeEntry( "HideAnimation",          _hideAnim);
    c->writeEntry( "AutoHideAnimation",      _autoHideAnim);
    c->writeEntry( "HideAnimationSpeed",     _hideAnimSpeed);
    c->writeEntry( "AutoHideAnimationSpeed", _autoHideAnimSpeed);
    c->writeEntry( "ShowToolTips",           _showToolTips );
    c->writeEntry( "SizePercentage",         _sizePercentage );
    c->writeEntry( "ExpandSize",             _expandSize );
}

PanelContainer::PanelContainer(QWidget *parent, const char *name)
   : QFrame(parent, name, WStyle_Customize | WStyle_NoBorderEx)
  , _autoHidden(false)
  , _userHidden(Unhidden)
  , _block_user_input(false)
  , _faked_activation(false)
  , _in_autohide(false)
  , _opMnu(0)
{
    qt_set_sane_enterleave( true ); // enable enter/leave propagation

    // panels live in the dock
    KWin::setType( winId(), NET::Dock );
    KWin::setState( winId(), NET::StaysOnTop | NET::Sticky );
    KWin::setOnAllDesktops( winId(), TRUE );

    connect( PGlobal::kwin_module, SIGNAL( strutChanged() ), this, SLOT( strutChanged() ) );
    connect( PGlobal::kwin_module, SIGNAL( currentDesktopChanged(int) ),
	     this, SLOT( currentDesktopChanged(int) ) );

    setFrameStyle( NoFrame );
    setLineWidth( 0 );
    setMargin(0);

    _popupWidgetFilter = new PopupWidgetFilter( this );
    connect( _popupWidgetFilter, SIGNAL(popupWidgetHiding()), SLOT(maybeStartAutoHideTimer()) );

    // layout
    _layout = new QBoxLayout( this, orientation() == Horizontal ? QBoxLayout::LeftToRight
			      : QBoxLayout::TopToBottom, 0, 0);
    _layout->setResizeMode( QLayout::FreeResize );

    // left/top hide button
    _ltHB = new KArrowButton(this);
    _ltHB->installEventFilter( this );
    connect( _ltHB, SIGNAL( clicked() ), this, SLOT( hideLeft() ) );

    _layout->addWidget( _ltHB );

    // applet area box
    _containerAreaBox = new ContainerAreaBox(this);
    _containerAreaBox->setFrameStyle( NoFrame );
    _containerAreaBox->setLineWidth( 0 );
    _containerAreaBox->installEventFilter( this );

    _layout->addWidget(_containerAreaBox);

    // left/up scroll button
    _luSB = new KArrowButton(this);
    _luSB->installEventFilter( this );
    _luSB->setAutoRepeat(true);
    connect(_luSB, SIGNAL(clicked()), SLOT(scrollLeftUp()));

    // right/ down scroll button
    _rdSB = new KArrowButton(this);
    _rdSB->installEventFilter( this );
    _rdSB->setAutoRepeat(true);
    connect(_rdSB, SIGNAL(clicked()), SLOT(scrollRightDown()));

    // scroll buttons
    _layout->addWidget(_luSB);
    _layout->addWidget(_rdSB);

    // right/bottom hide button
    _rbHB = new KArrowButton(this);
    _rbHB->installEventFilter( this );
    connect( _rbHB, SIGNAL( clicked() ), this, SLOT( hideRight() ) );

    _layout->addWidget( _rbHB );

    // hide scroll buttons
    showScrollButtons(false);

    // instantiate the autohide timer
    _autohideTimer = new QTimer(this);
    connect(_autohideTimer, SIGNAL(timeout()), SLOT(autoHideTimeout()));

    installEventFilter( this ); // for mouse event handling

    _containers.append( this );
}

PanelContainer::~PanelContainer()
{
//    kdDebug(1210) << "PanelContainer::~PanelContainer()" << endl;
    delete _opMnu;

    _containers.remove( this );
}

void PanelContainer::readContainerConfig()
{
    QListIterator<PanelContainer> it(_containers);
    for( ; it.current(); ++it ) {
	(*it)->readConfig();
    }
}

void PanelContainer::writeContainerConfig()
{
    QListIterator<PanelContainer> it(_containers);
    for( ; it.current(); ++it ) {
	(*it)->writeConfig();
    }
}

PanelSettings PanelContainer::defaultSettings()
{
    return PanelSettings();
}

void PanelContainer::readConfig( KConfig* config )
{
//    kdDebug(1210) << "PanelContainer::readConfig()" << endl;

    _settings = defaultSettings();
    _settings.readConfig( config );
    
    // we need to do this each time we reconfigure to ensure we respect Fitt's Law
    _containerAreaBox->enableX11EventFilter(true);

    emit positionChange( position() );
    updateLayout();

//    kdDebug(1210) << "tooltips " << ( _settings._showToolTips ? "enabled" : "disabled" ) << endl;
    QToolTip::setEnabled( _settings._showToolTips );

    if( !_settings._autoHide )
	autoHide(false);
    maybeStartAutoHideTimer();
}

void PanelContainer::writeConfig( KConfig* config )
{
//    kdDebug(1210) << "PanelContainer::writeConfig()" << endl;

    _settings.writeConfig( config );
}

void PanelContainer::setPosition(Position p)
{
//    kdDebug(1210) << "PanelContainer::setPosition()" << endl;
    if ( p != _settings._position ) {
	_settings._position = p;
	emit positionChange( p );
	updateLayout();
	writeConfig();
    }
}

Qt::Orientation PanelContainer::orientation() const
{
    if( position() == Top || position() == Bottom ) {
	return Horizontal;
    } else {
	return Vertical;
    }
}

void PanelContainer::currentDesktopChanged(int)
{
//    kdDebug(1210) << "PanelContainer::currentDesktopChanged" << endl;
    if (_settings._autoHideSwitch&&_settings._autoHide) {
	autoHide(false);
    }

    // For some reason we don't always get leave events when the user
    // changes desktops and moves the cursor out of the panel at the
    // same time. Maybe always calling this will help.
    maybeStartAutoHideTimer();
}

void PanelContainer::hideLeft()
{
    animatedHide(true);
}

void PanelContainer::hideRight()
{
    animatedHide(false);
}

QPoint PanelContainer::getPopupPosition(QPopupMenu *menu, QPoint globalPos)
{
    QPoint p(0,0);

    switch (position()) {
    case Top:
	p = mapToGlobal(QPoint(0, height()));
	p.setX(globalPos.x());
	break;
    case Bottom:
	p = mapToGlobal(QPoint(0, 0 - menu->height()));
	p.setX(globalPos.x());
	break;
    case Right:
	p = mapToGlobal(QPoint(0-menu->width(), 0));
	p.setY(globalPos.y());
	break;
    case Left:
	p = mapToGlobal(QPoint(width(), 0));
	p.setY(globalPos.y());
	break;
    }

    return p;
}

void PanelContainer::resetLayout()
{
//    kdDebug(1210) << "PanelContainer::resetLayout()" << endl;

    QRect g = initialGeometry( position(), autoHidden(), userHidden() );

    setGeometry( g );

    // layout
    if( orientation() == Horizontal )
	_layout->setDirection( QBoxLayout::LeftToRight );
    else
	_layout->setDirection( QBoxLayout::TopToBottom );

   // left/top hide button
    if ( orientation() == Horizontal ) {
	_ltHB->setArrowType(Qt::LeftArrow);
	_ltHB->setFixedSize(_settings._HBwidth, height());
    } else {
	_ltHB->setArrowType(Qt::UpArrow);
	_ltHB->setFixedSize(width(), _settings._HBwidth);
    }

    if ( _settings._showLeftHB || userHidden() == RightBottom ) {
	_ltHB->show();
    } else {
	_ltHB->hide();
    }

    if( orientation() == Horizontal ) {
        _luSB->setArrowType(Qt::LeftArrow);
        _rdSB->setArrowType(Qt::RightArrow);
        _luSB->setFixedSize(10, height());
        _rdSB->setFixedSize(10, height());
        QToolTip::add(_luSB, i18n("Scroll Left"));
        QToolTip::add(_rdSB, i18n("Scroll Right"));
    } else {
        _luSB->setArrowType(Qt::UpArrow);
        _rdSB->setArrowType(Qt::DownArrow);
        _luSB->setFixedSize(width(), 10);
        _rdSB->setFixedSize(width(), 10);
        QToolTip::add(_luSB, i18n("Scroll Up"));
        QToolTip::add(_rdSB, i18n("Scroll Down"));
    }

    // right/bottom hide button
    if ( orientation() == Horizontal ) {
	_rbHB->setArrowType(Qt::RightArrow);
	_rbHB->setFixedSize(_settings._HBwidth, height());
    } else {
	_rbHB->setArrowType(Qt::DownArrow);
	_rbHB->setFixedSize(width(), _settings._HBwidth);
    }

    if ( _settings._showRightHB || userHidden() == LeftTop ) {
	_rbHB->show();
    } else {
	_rbHB->hide();
    }

    if( userHidden() ) {
	QToolTip::add(_ltHB, i18n("Show Panel"));
	QToolTip::add(_rbHB, i18n("Show Panel"));
    } else {
	QToolTip::add(_ltHB, i18n("Hide Panel"));
	QToolTip::add(_rbHB, i18n("Hide Panel"));
    }

    _layout->activate();
    updateGeometry();

}

void PanelContainer::blockUserInput( bool block )
{
    if ( block == _block_user_input )
	return;

    // If we don't want any user input to be possible we should catch mouse
    // events and such. Therefore we install an eventfilter and let the
    // eventfilter discard those events.
    if ( block )
	qApp->installEventFilter( this );
    else
	qApp->removeEventFilter( this );

    _block_user_input = block;
}

// The autohide logic is pretty hacky.
//
// How it should work:
//
// The autohide timer is a single shot. It is started whenever the mouse
// leaves the panel and autohide is on. When the timeout occurs, which
// could be as soon as the event queue is clear if the timeout is zero,
// the panel is autohidden and the timer is stopped. When the mouse reenters
// the panel, it is un-autohidden.
//
// Why this won't work:
//
// 1) We don't want to hide the panel if there is a popup menu showing,
//    even if the mouse is outside the panel.
// 3) We don't get reliable enter/leave events during a virtual desktop
//    change.
// 2) The container area (and some applets) grab the mouse at various
//    times, notably during a button press or moving an applet. This
//    causes us not to receive enter and leave events.
//
// What we do instead:
//
// the autohide timer is continouously running whenever autohide is enabled
// and the panel is not hidden. When it times out, if the cursor is not inside
// the panel or there are no popup menus open, the panel is autohidden. If a
// popup is open, we stop the timer, and install an event filter on the
// popup what will restart the timer when it closes. If the cursor is inside the
// panel, the timer keeps running. If the user specified a zero timeout, run it at
// 10 ms instead to avoid taking 100% cpu. If we do get an enter or leave event,
// reset the timer so that the desired timout will be followed.
//
// Known problems with this approach:
//
// We take more cpu then we should. Still a few cases where panel
// should be autohid, but isn't.
//
// Very bizzare and unfathomable problem: Look below, in the
// autoHideTimeout method. You will see a call to autoHide(true)
// sandwiched between two debug statements. If you look at
// autoHide(), you will see that it prints lots of debug info.
// Here is the strange part: sometimes, autoHide(true) _DOES NOT_
// get called, even when the debug strings surrounding the call
// are printed! WTF?! This happens if you drag an applet handle outside
// the panel and wait for the timeout. I am completely mystified
// by this.
void PanelContainer::maybeStartAutoHideTimer()
{
    if ( _settings._autoHide && !_autoHidden && !_userHidden ) {


//	kdDebug(1210) << "starting auto hide timer for " << name() << endl;

	if( _settings._autoHideDelay == 0 )
	    _autohideTimer->start( 10 );
	else
	    _autohideTimer->start( _settings._autoHideDelay * 1000 );
    }
}

void PanelContainer::stopAutoHideTimer()
{
   if ( _autohideTimer->isActive() ) {
//	kdDebug(1210) << "stopping auto hide timer for " << name() << endl;
	_autohideTimer->stop();
   }
}

void PanelContainer::autoHideTimeout()
{
//    kdDebug(1210) << "PanelContainer::autoHideTimeout() " << name() << endl;

    // Hack: If there is a popup open, don't autohide until it closes.
    QWidget* popup = QApplication::activePopupWidget();
    if ( popup ) {

//	kdDebug(1210) << "popup detected" << endl;

	// Remove it first in case it was already installed.
	// Does nothing if it wasn't installed.
	popup->removeEventFilter( _popupWidgetFilter );

	// We will get a signal from the filter after the
	// popup is hidden. At that point, maybeStartAutoHideTimer()
	// will get called again.
	popup->installEventFilter( _popupWidgetFilter );

	// Stop the timer.
	stopAutoHideTimer();
	return;
    }

    if( _settings._autoHide && !_autoHidden && !_userHidden && !geometry().contains(QCursor::pos()) ) {
	stopAutoHideTimer();

//	kdDebug(1210) << "cursor is outside panel, hiding" << endl;
	// Sometimes, this call isn't made, even though the debug messages
	// on either side are printed!!!!!
	autoHide(true);
//	kdDebug(1210) << "autoHide(true) was just called" << endl;
    }
}

bool PanelContainer::eventFilter( QObject* o, QEvent * e)
{

    if (autoHidden()) {
	switch ( e->type() ) {
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
	case QEvent::MouseButtonDblClick:
	case QEvent::MouseMove:
	case QEvent::KeyPress:
	case QEvent::KeyRelease:
	    return true; // ignore;
	default:
	    break;
	}
    }

    if ( _block_user_input ) {
	switch ( e->type() ) {
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
	case QEvent::MouseButtonDblClick:
	case QEvent::MouseMove:
	case QEvent::KeyPress:
	case QEvent::KeyRelease:
	case QEvent::Enter:
	case QEvent::Leave:
	    return true; // ignore;
	default:
	    break;
	}
	return false;
    }

    switch ( e->type() ) {
// Why was this in here? It breaks the preferred focus handling (click, follows mouse, etc). -John
#if 0
    case QEvent::Enter:
	if  ( o == this && !isActiveWindow() ) {
	    XEvent ev;
	    memset(&ev, 0, sizeof(ev));
	    ev.xfocus.display = qt_xdisplay();
	    ev.xfocus.type = XFocusIn;
	    ev.xfocus.window = winId();
	    ev.xfocus.mode = NotifyNormal;
	    ev.xfocus.detail = NotifyAncestor;
	    Time time = qt_x_time;
	    qt_x_time = 1;
	    qApp->x11ProcessEvent( &ev );
	    qt_x_time = time;
	    _faked_activation = true;
	    setFocus(); // to avoid having a blinking cursor or a focus rectangle somewhere else
	}
	break;
    case QEvent::Leave:
	if ( o == this && _faked_activation && isActiveWindow() ) {
	    XEvent ev;
	    memset(&ev, 0, sizeof(ev));
	    ev.xfocus.display = qt_xdisplay();
	    ev.xfocus.type = XFocusIn;
	    ev.xfocus.window = winId();
	    ev.xfocus.mode = NotifyNormal;
	    ev.xfocus.detail = NotifyAncestor;
	    Time time = qt_x_time;
	    qt_x_time = 1;
	    qApp->x11ProcessEvent( &ev );
	    qt_x_time = time;
	    _faked_activation = false;
	}
	break;
    case QEvent::WindowDeactivate:
	_faked_activation = false;
	break;
#endif
    case QEvent::MouseButtonPress:
	{
	    QMouseEvent* me = (QMouseEvent*) e;
	    if ( me->button() == LeftButton )
		{
		    _last_lmb_press = me->pos();
		}
	    else if ( me->button() == RightButton )
		{
		    showPanelMenu( me->globalPos() );
		}
	}
	break;
    case QEvent::MouseMove:
	{
	    QMouseEvent* me = (QMouseEvent*) e;
	    if ( (me->state() & LeftButton) == LeftButton ) {
		QPoint p( me->pos() - _last_lmb_press );
		if ( p.manhattanLength() > KGlobalSettings::dndEventDelay() ) {
		    moveMe();
		    return true;
		}
	    }
	}
	break;
    case QEvent::Enter:
    case QEvent::DragEnter:
    case QEvent::Leave:
    case QEvent::DragLeave:
	// Un-autohide on all enter/leave events, because they are really screwey.
	if( autoHidden() )
	    autoHide(false);
	maybeStartAutoHideTimer();
	break;
    default:
	break;
    }
    return false;
}

QSize PanelContainer::initialSize( Position p )
{
    QRect a = workArea();

//    kdDebug(1210) << "     Work Area: (" << a.topLeft().x() << ", " << a.topLeft().y() << ") to ("
//                                 << a.bottomRight().x() << ", " << a.bottomRight().y() << ")" << endl;

    QSize hint = sizeHint( p, a.size() ).boundedTo( a.size() );
    int width = 0;
    int height = 0;

    if( p == Left || p == Right ) {
	width = hint.width();
	height = ( a.height() * _settings._sizePercentage ) / 100;

	if ( _settings._expandSize ) {
	    height = QMAX( height, hint.height() );
	}
    } else {
	width = ( a.width() * _settings._sizePercentage ) / 100;
	height = hint.height();

	if ( _settings._expandSize ) {
	    width = QMAX( width, hint.width() );
	}
    }

    return QSize( width, height );
}

QPoint PanelContainer::initialLocation( Position p, QSize s, bool autohidden, UserHidden userHidden )
{
    QRect a = workArea();

    int left;
    int top;

    // Get initial position
    switch( p ) {
    case Left:
    case Top:
	left = a.left();
	top  = a.top();
	break;
    case Right:
	left = a.right() - s.width() + 1;
	top  = a.top();
	break;
    case Bottom:
    default:
	left = a.left();
	top  = a.bottom() - s.height() + 1;
	break;
    }

    // Correct for auto hide
    if( autohidden ) {
#ifdef __osf__
#define OFF 2
#else
#define OFF 1
#endif
	switch( position() ) {
	case Left:
	    left -= s.width() - OFF;
	    break;
	case Right:
	    left += s.width() - OFF;
	    break;
	case Top:
	    top -= s.height() - OFF;
	    break;
	case Bottom:
	default:
	    top += s.height() - OFF;
	    break;
	}

    // Correct for user hide
    } else if ( userHidden == LeftTop ) {
	switch( position() ) {
	case Left:
	case Right:
	    top -= s.height() - _settings._HBwidth;
	    break;
	case Top:
	case Bottom:
	default:
	    left -= s.width() - _settings._HBwidth;
	    break;
	}
    } else if ( userHidden == RightBottom ) {
	switch( position() ) {
	case Left:
	case Right:
	    top = a.bottom() - _settings._HBwidth + 1;
	    break;
	case Top:
	case Bottom:
	default:
	    left = a.right() - _settings._HBwidth + 1;
	    break;
	}
    }

    return QPoint( left, top );
}

QRect PanelContainer::initialGeometry( Position p, bool autoHidden, UserHidden userHidden )
{
//    kdDebug(1210) << "   Computing geometry for " << name() << endl;

    QSize size = initialSize( p );
    QPoint point = initialLocation( p, size, autoHidden, userHidden );

//    kdDebug(1210) << "     Size: " << size.width() << " x " << size.height() << endl;
//    kdDebug(1210) << "     Pos: (" << point.x() << ", " << point.y() << ")" << endl;

    return QRect( point, size );
}

void PanelContainer::updateLayout()
{
//    kdDebug(1210) << "PanelContainer::updateLayout()" << endl;
    resetLayout();
    updateWindowManager();
}

void PanelContainer::strutChanged()
{
//    kdDebug(1210) << "PanelContainer::strutChanged()" << endl;
    if ( initialGeometry( position(), autoHidden(), userHidden() ) != geometry() ) {
	updateLayout();
    }
}

void PanelContainer::updateWindowManager()
{
//    kdDebug(1210) << "PanelContainer::updateWindowManager()" << endl;
    // Set the relevant properties on the window.
    int w = width();
    int h = height();

    QRect r(QApplication::desktop()->geometry());

    QRect geom = initialGeometry( position() );

    if( userHidden() || _settings._autoHide )
	w = h = 0;

    // only call KWin::setStrut when the strut is really changed
    NETStrut strut;
    switch (position()) {
    case Top:     strut.top = geom.y() + h; break;
    case Bottom:  strut.bottom = (r.bottom() - geom.y() - height()) + h + 1; break;
    case Right:   strut.right = (r.right() - geom.x() - width()) + w + 1; break;
    case Left:    strut.left = geom.x() + w; break;
    }

    if ( strut.left   != _strut.left  ||
         strut.right  != _strut.right ||
         strut.top    != _strut.top   ||
         strut.bottom != _strut.bottom ) {

	_strut = strut;
//	kdDebug(1210) << "Panel sets new strut: " << position() << endl;
//	kdDebug(1210) << strut.top << " " << strut.bottom << " " << strut.right << " " << strut.left << endl;
	switch (position()) {
	case Top:     KWin::setStrut( winId(), 0, 0,  strut.top, 0 ); break;
	case Bottom:  KWin::setStrut( winId(), 0, 0, 0, strut.bottom); break;
	case Right:   KWin::setStrut( winId(), 0, strut.right, 0, 0 ); break;
	case Left:    KWin::setStrut( winId(), strut.left, 0, 0, 0 ); break;
	}
    }
}

QSize PanelContainer::sizeHint( Position p, QSize maxSize )
{
    int width = _containerAreaBox->lineWidth() * 2;
    int height = _containerAreaBox->lineWidth() * 2;
    if( p == Top || p == Bottom ) {
	if ( _settings._showLeftHB )
	    width += _settings._HBwidth;
	if ( _settings._showRightHB )
	    width += _settings._HBwidth;
    } else {
	if ( _settings._showLeftHB )
	    height += _settings._HBwidth;
	if ( _settings._showRightHB )
	    height += _settings._HBwidth;
    }

    QSize size = QSize( width, height ).boundedTo( maxSize );

    //kdDebug(1210) << "     PanelContainer requests " << size.width() << " x " << size.height() << endl;

    return size;
}

void PanelContainer::showScrollButtons(bool show)
{
    if( show ) {
        _luSB->show();
        _rdSB->show();
    }
    else {
        _luSB->hide();
        _rdSB->hide();
    }
}

/* 1 is the initial speed, hide_show_animation is the top speed. */
#define PANEL_SPEED(x, c) (int)((1.0-2.0*fabs((x)-(c)/2.0)/c)*_settings._hideAnimSpeed+1.0)
#define PANEL_AUTOSPEED(x, c) (int)((1.0-2.0*fabs((x)-(c)/2.0)/c)*_settings._autoHideAnimSpeed+1.0)

void PanelContainer::animatedHide(bool left)
{
//    kdDebug(1210) << "PanelContainer::animatedHide()" << endl;

    blockUserInput(true);

    UserHidden newState;
    if( _userHidden != Unhidden )
        newState = Unhidden;
    else if( left )
        newState = LeftTop;
    else
        newState = RightBottom;

    QPoint oldpos = pos();
    QPoint newpos = initialGeometry( position(), false, newState ).topLeft();

    if( newState != Unhidden ) {
	_userHidden = newState;

	// So we don't cover the mac-style menubar
	lower();
    }

    if( _settings._hideAnim ) {
	switch( position() ) {
	case Left:
	case Right:
	    for (int i = 0; i < abs(newpos.y() - oldpos.y());
		 i += PANEL_SPEED(i,abs(newpos.y() - oldpos.y())))
		{
		    if (newpos.y() > oldpos.y())
			move(newpos.x(), oldpos.y() + i);
		    else
			move(newpos.x(), oldpos.y() - i);
		    qApp->syncX();
		    qApp->processEvents();
		}
	    break;
	case Top:
	case Bottom:
	default:
	    for (int i = 0; i < abs(newpos.x() - oldpos.x());
		 i += PANEL_SPEED(i,abs(newpos.x() - oldpos.x())))
		{
		    if (newpos.x() > oldpos.x())
			move(oldpos.x() + i, newpos.y());
		    else
			move(oldpos.x() - i, newpos.y());
		    qApp->syncX();
		    qApp->processEvents();
		}
	    break;
	}
    }

    blockUserInput( false );

    _userHidden = newState;
    
    updateLayout();
}

void PanelContainer::autoHide(bool hide)
{
//   kdDebug(1210) << "PanelContainer::autoHide( " << hide << " )" << endl;

   if ( _in_autohide ) {
//	kdDebug(1210) << "in autohide" << endl;
	return;
   }

    if ( hide == _autoHidden ) {
//	kdDebug(1210) << "already in that state" << endl;
	return; //nothing to do
    }

//    kdDebug(1210) << "entering autohide for real" << endl;

    _in_autohide = true;

// The following code breaks autohiding. I'm not sure what the problem it
// was meant to fix was, but I don't think it's worth it to break auto-hide. -John
#if 0
    // Check for foreign popup menus. Only check when we want to hide it.
    // Otherwise we won't be able to unhide the panel when a drag entered.
    if ( hide ) {
	if ( XGrabPointer( qt_xdisplay(), winId(), true, ButtonPressMask,
			   GrabModeAsync, GrabModeAsync, None, None, CurrentTime )
	     != Success ) {
	     kdDebug(1210) << "can't grab pointer" << endl;
	    _in_autohide = false;
	    return;
	}
	XUngrabPointer( qt_xdisplay(), CurrentTime );
    }
#endif

    _autoHidden = hide;

    blockUserInput(true);

    QPoint oldpos = pos();
    QPoint newpos = initialGeometry( position(), hide, Unhidden ).topLeft();

    _containerAreaBox->enableX11EventFilter(!hide);

    if( hide ) {
	// So we don't cover other panels
	lower();
    } else {
	// So we aren't covered by other panels
	raise();
    }

    if(_settings._autoHideAnim) {
	switch( position() ) {
	case Left:
	case Right:
	    for (int i = 0; i < abs(newpos.x() - oldpos.x());
		 i += PANEL_AUTOSPEED(i,abs(newpos.x() - oldpos.x())))
		{
		    if (newpos.x() > oldpos.x())
			move(oldpos.x() + i, newpos.y());
		    else
			move(oldpos.x() - i, newpos.y());
		    qApp->syncX();
		    qApp->processEvents();
		}
	    break;
	case Top:
	case Bottom:
	default:
	    for (int i = 0; i < abs(newpos.y() - oldpos.y());
		 i += PANEL_AUTOSPEED(i,abs(newpos.y() - oldpos.y())))
		{
		    if (newpos.y() > oldpos.y())
			move(newpos.x(), oldpos.y() + i);
		    else
			move(newpos.x(), oldpos.y() - i);
		    qApp->syncX();
		    qApp->processEvents();
		}
	    break;
	}
    }

    blockUserInput(false);

    updateLayout();

    // Sometimes tooltips don't get hidden.
    QToolTip::hide();

    _in_autohide = false;
}

void PanelContainer::showPanelMenu( QPoint globalPos )
{
    if(!_opMnu)
	_opMnu = new PanelOpMenu(true);
    _opMnu->exec(getPopupPosition(_opMnu, globalPos));
}

void PanelContainer::moveMe()
{
    stopAutoHideTimer();

    QApplication::syncX();
    QValueList<QRect> rects;

    QRect a = workArea();

    rects.append( initialGeometry( Left ) );
    rects.append( initialGeometry( Right ) );
    rects.append( initialGeometry( Top ) );
    rects.append( initialGeometry( Bottom ) );

    Position newpos = (Position) UserRectSel::select(rects, position());
    setPosition( newpos );

    // sometimes the HB's are not reset correctly
    _ltHB->setDown(false);
    _rbHB->setDown(false);

    maybeStartAutoHideTimer();
}

QRect PanelContainer::workArea()
{
    QValueList<WId> list;
    QListIterator<PanelContainer> it(_containers);
    for( ; it.current(); ++it ) {
	list.append((*it)->winId());
    }
    return PGlobal::kwin_module->workArea(list);
}

PopupWidgetFilter::PopupWidgetFilter( QObject *parent )
  : QObject( parent, "PopupWidgetFilter" )
{
}

bool PopupWidgetFilter::eventFilter( QObject *obj, QEvent* e )
{
    if( e->type() == QEvent::Hide ) {
	emit popupWidgetHiding();
    }
    return false;
}
