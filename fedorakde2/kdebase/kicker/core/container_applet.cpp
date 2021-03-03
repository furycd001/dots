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

#include <qxembed.h>
#include <qframe.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qfile.h>

#include <kapp.h>
#include <kpopupmenu.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kpanelapplet.h>
#include <kprocess.h>
#include <kstddirs.h>
#include <kdebug.h>
#include <dcopclient.h>

#include "global.h"
#include "panel.h"
#include "appletop_mnu.h"
#include "pluginmgr.h"
#include "applethandle.h"
#include "appletinfo.h"
#include "container_applet.h"
#include "container_applet.moc"

#define HANDLE_SIZE 7

AppletContainer::AppletContainer(const AppletInfo& info, QWidget* parent )
  : BaseContainer(parent)
  , _info(info)
  , _layout(0)
  , _type(KPanelApplet::Normal)
  , _widthForHeightHint(0)
  , _heightForWidthHint(0)
{
    _dir = dDown;
    // setup handle
    _handle = new AppletHandle(this);
    _handle->installEventFilter(this);
    _handle->setOrientation(orientation());

    //setup appletframe
    _appletframe = new QHBox(this);
    _appletframe->setFrameStyle(QFrame::NoFrame);
    _appletframe->installEventFilter(this);

    if (orientation() == Horizontal)
	{
	    _handle->setFixedWidth(HANDLE_SIZE);
	    _handle->setMaximumHeight(128);
	    _layout = new QBoxLayout(this, QBoxLayout::LeftToRight, 0, 0);
	}
    else
	{
	    _handle->setFixedHeight(HANDLE_SIZE);
	    _handle->setMaximumWidth(128);
	    _layout = new QBoxLayout(this, QBoxLayout::TopToBottom, 0, 0);
	}

    _layout->setResizeMode( QLayout::FreeResize );

    _layout->addWidget(_handle);
    _layout->addWidget(_appletframe);
    _layout->activate();
}

void AppletContainer::configure()
{
    KConfig *config = KGlobal::config();
    config->setGroup("General");
    _handle->setFadeOutHandle(config->readBoolEntry("FadeOutAppletHandles", false));
}

bool AppletContainer::eventFilter (QObject *o, QEvent *e)
{
    switch (e->type())
	{
	case QEvent::MouseButtonPress:
	    {
		QMouseEvent* ev = (QMouseEvent*) e;
		if ( ev->button() == RightButton )
		    {
			if (!_opMnu)
			    _opMnu = new PanelAppletOpMenu(_actions, _info.name(), _info.icon());

			switch(_opMnu->exec(getPopupPosition(_opMnu, ev->pos())))
			    {
			    case PanelAppletOpMenu::Move:
				_moveOffset = QPoint(_handle->width()/2, _handle->height()/2);
				emit moveme(this);
				break;
			    case PanelAppletOpMenu::Remove:
				emit removeme(this);
				break;
			    case PanelAppletOpMenu::Help:
                                help();
                                break;
			    case PanelAppletOpMenu::About:
                                about();
                                break;
			    case PanelAppletOpMenu::Preferences:
                                preferences();
                                break;
          case PanelAppletOpMenu::ReportBug:
                                reportBug();
                                break;
			    default:
                                break;
			    }
			return true;
		    }
		else if ( ev->button() == MidButton
			  || ev->button() == LeftButton )
		    {
			_moveOffset = ev->pos();
			emit moveme(this);
		    }
		return false;
	    }
	default:
	    return QWidget::eventFilter(o, e);    // standard event processing
	}
    return false;
}

void AppletContainer::slotSetPopupDirection(Direction d)
{
    if (_dir == d) return;

    BaseContainer::slotSetPopupDirection(d);

    resetLayout();
}

void AppletContainer::slotSetOrientation(Orientation o)
{
    if (_orient == o) return;

    BaseContainer::slotSetOrientation(o);

    resetLayout();
}

void AppletContainer::resetLayout()
{
    _handle->setOrientation(orientation());

    if (orientation() == Horizontal)
	{
	    _layout->setDirection(QBoxLayout::LeftToRight);
	    _handle->setFixedWidth(HANDLE_SIZE);
	    _handle->setMaximumHeight(128);
	}
    else
	{
	    _layout->setDirection(QBoxLayout::TopToBottom);
	    _handle->setFixedHeight(HANDLE_SIZE);
	    _handle->setMaximumWidth(128);
	}
    _layout->activate();
}

void AppletContainer::removeSessionConfigFile()
{
    if (_configFile.isEmpty()) return;
    if (_info.isUniqueApplet()) return;
    QString path = locate("config", _configFile);

    QFile f (path);
    if (f.exists()) {
	kdDebug(1210) << "Removing session config file: " << path << endl;
	f.remove();
    }
}

InternalAppletContainer::InternalAppletContainer( const AppletInfo& info, QWidget *parent)
  : AppletContainer(info, parent)
{
    _deskFile = info.desktopFile();
    _configFile = info.configFile();
    _applet = PGlobal::pluginmgr->loadApplet(_deskFile, _configFile, _appletframe);


    if (!_applet) return;

    _applet->slotSetOrientation(_orient);
    _actions = _applet->actions();
    _type = _applet->type();

    connect(_applet, SIGNAL(updateLayout()), SIGNAL(updateLayout()));
    connect(_applet, SIGNAL(requestFocus()), SLOT(activateWindow()));
}


InternalAppletContainer::~InternalAppletContainer()
{
}

void InternalAppletContainer::saveConfiguration(KConfig* config, const QString& g)
{
    QString group = g;
    if (group.isNull()) group = appletId();

    config->setGroup(group);
    config->writeEntry("ConfigFile", _configFile);
    config->writeEntry("DesktopFile", _deskFile);
    config->sync();
}

void InternalAppletContainer::slotSetPopupDirection(Direction d)
{
    if (_dir == d) return;

    AppletContainer::slotSetPopupDirection(d);

    if (!_applet) return;
    _applet->slotSetPopupDirection((KPanelApplet::Direction)(d));
}

void InternalAppletContainer::slotSetOrientation(Orientation o)
{
    if (_orient == o) return;

    AppletContainer::slotSetOrientation(o);

    if (!_applet) return;
    _applet->slotSetOrientation(o);
}

int InternalAppletContainer::widthForHeight(int h)
{
    if (!_applet) {
	if (_widthForHeightHint > 0)
	    return _widthForHeightHint + HANDLE_SIZE;
	else
	    return h + HANDLE_SIZE;
    }
    return _applet->widthForHeight(h) + HANDLE_SIZE;
}

int InternalAppletContainer::heightForWidth(int w)
{
    if (!_applet) {
	if (_heightForWidthHint > 0)
	    return _heightForWidthHint + HANDLE_SIZE;
	else
	    return w + HANDLE_SIZE;
    }
    return _applet->heightForWidth(w) + HANDLE_SIZE;
}

void InternalAppletContainer::about()
{
    if (!_applet) return;
    _applet->action( KPanelApplet::About );
}

void InternalAppletContainer::help()
{
    if (!_applet) return;
    _applet->action( KPanelApplet::Help );
}

void InternalAppletContainer::preferences()
{
    if (!_applet) return;
    _applet->action( KPanelApplet::Preferences );
}

void InternalAppletContainer::reportBug()
{
    if (!_applet) return;
    _applet->action( KPanelApplet::ReportBug );
}

ExternalAppletContainer::ExternalAppletContainer( const AppletInfo& info, QWidget *parent)
  : AppletContainer(info, parent)
, DCOPObject(QCString("ExternalAppletContainer_") + QString::number( (ulong) this ).latin1() )
, _isdocked(false)
{
    _deskFile = info.desktopFile();
    _configFile = info.configFile();

    // init QXEmbed
    _embed = new QXEmbed( _appletframe );
    connect ( _embed, SIGNAL( embeddedWindowDestroyed() ),
	      this, SIGNAL( embeddedWindowDestroyed() ) );

    KProcess process;
    process << "appletproxy"
	    << QCString("--configfile")
	    << info.configFile()
	    << QCString("--callbackid")
	    << objId()
	    << info.desktopFile();
    process.start(KProcess::DontCare);
}

ExternalAppletContainer::~ExternalAppletContainer()
{
    QByteArray data;
    kapp->dcopClient()->send( _app, "AppletProxy", "removedFromPanel()", data);
}

void ExternalAppletContainer::saveConfiguration(KConfig* config, const QString& g)
{
    QString group = g;
    if (group.isNull()) group = appletId();

    config->setGroup(group);
    config->writeEntry("ConfigFile", _configFile);
    config->writeEntry("DesktopFile", _deskFile);
    config->sync();
}

void ExternalAppletContainer::slotSetPopupDirection(Direction d)
{
    if (_dir == d) return;

    AppletContainer::slotSetPopupDirection(d);

    if (!_isdocked) return;

    QByteArray data;
    QDataStream dataStream( data, IO_WriteOnly );
    dataStream << static_cast<int>(d);

    kapp->dcopClient()->send( _app, "AppletProxy", "setDirection(int)", data);
}

void ExternalAppletContainer::slotSetOrientation(Orientation o)
{
    if (_orient == o) return;

    AppletContainer::slotSetOrientation(o);

    if (!_isdocked) return;

    QByteArray data;
    QDataStream dataStream( data, IO_WriteOnly );
    dataStream << static_cast<int>(o);

    kapp->dcopClient()->send( _app, "AppletProxy", "setOrientation(int)", data );
}

void ExternalAppletContainer::about()
{
    if (!_isdocked) return;

    QByteArray data;
    kapp->dcopClient()->send( _app, "AppletProxy", "about()", data );
}

void ExternalAppletContainer::help()
{
    if (!_isdocked) return;

    QByteArray data;
    kapp->dcopClient()->send( _app, "AppletProxy", "help()", data );
}

void ExternalAppletContainer::preferences()
{
    if (!_isdocked) return;

    QByteArray data;
    kapp->dcopClient()->send( _app, "AppletProxy", "preferences()", data );
}

void ExternalAppletContainer::reportBug()
{
    if (!_isdocked) return;

    QByteArray data;
    kapp->dcopClient()->send( _app, "AppletProxy", "reportBug()", data );
}

int ExternalAppletContainer::widthForHeight(int h)
{
    int w = h;
    if (_widthForHeightHint > 0)
	w = _widthForHeightHint;

    if (!_isdocked) return w;

    DCOPClient* dcop = kapp->dcopClient();

    QByteArray data;
    QCString replyType;
    QByteArray replyData;
    QDataStream dataStream( data, IO_WriteOnly );
    dataStream << h;

    if (dcop->call( _app, "AppletProxy", "widthForHeight(int)", data, replyType, replyData ) )
	{
	    QDataStream reply( replyData, IO_ReadOnly );
	    reply >> w;
	}
    return w + HANDLE_SIZE;
}

int ExternalAppletContainer::heightForWidth(int w)
{
    int h = w;
    if (_heightForWidthHint > 0)
	h = _heightForWidthHint;

    if (!_isdocked) return h;

    DCOPClient* dcop = kapp->dcopClient();

    QByteArray data;
    QCString replyType;
    QByteArray replyData;
    QDataStream dataStream( data, IO_WriteOnly );
    dataStream << w;

    if (dcop->call( _app, "AppletProxy", "heightForWidth(int)", data, replyType, replyData ) )
	{
	    QDataStream reply( replyData, IO_ReadOnly );
	    reply >> h;
	}
    return h + HANDLE_SIZE;
}

bool ExternalAppletContainer::process(const QCString &fun, const QByteArray &data,
				      QCString& replyType, QByteArray & replyData)
{
    if ( fun == "dockRequest(int,int)" )
	{
	    QDataStream reply( replyData, IO_WriteOnly );
	    replyType = "WId";
	    reply << _embed->winId();

	    QDataStream sdata( data, IO_ReadOnly );
	    int actions, type;
	    sdata >> actions;
	    sdata >> type;

	    dockRequest(kapp->dcopClient()->senderId(), actions, type);
	    return true;
	}
    else if(fun == "updateLayout()")
	{
	    emit updateLayout();
	    return(true);
	}
    else if(fun == "requestFocus()")
	{
	    activateWindow();
	    return(true);
	}
    return true;
}

void ExternalAppletContainer::dockRequest(QCString app, int actions, int type)
{
    _app = app;

    kdDebug(1210) << "ExternalAppletContainer::dockRequest: " << app << endl;

    _type = static_cast<KPanelApplet::Type>(type);
    _actions = actions;

    // set orientation
    {
	QByteArray data;
	QDataStream dataStream( data, IO_WriteOnly );
	dataStream << static_cast<int>(_orient);

	kapp->dcopClient()->send( _app, "AppletProxy", "setOrientation(int)", data );
    }
    // set popup menu direction
    {
	QByteArray data;
	QDataStream dataStream( data, IO_WriteOnly );
	dataStream << static_cast<int>(_dir);

	kapp->dcopClient()->send( _app, "AppletProxy", "setDirection(int)", data);
    }

    _isdocked = true;
    emit docked(this);
    kdDebug(1210) << "dockRequest done" << endl;
}
