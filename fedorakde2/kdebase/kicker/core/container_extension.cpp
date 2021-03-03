/*****************************************************************

Copyright (c) 2000-2001 the kicker authors. See file AUTHORS.

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
#include <qfile.h>

#include <kglobal.h>
#include <kglobalsettings.h>
#include <kconfig.h>
#include <kapp.h>
#include <kprocess.h>
#include <kdebug.h>
#include <kstddirs.h>
#include <dcopclient.h>
#include <kwinmodule.h>

#include "extensionop_mnu.h"
#include "global.h"
#include "pluginmgr.h"

#include "container_extension.h"
#include "container_extension.moc"

ExtensionContainer::ExtensionContainer(const AppletInfo& info, QWidget *parent)
  : PanelContainer(parent, "ExtensionContainer")
  , _id(QString::null)
  , _opMnu(0)
  , _info(info)
  , _type(KPanelExtension::Normal)
  , _actions(0)
{
}

ExtensionContainer::~ExtensionContainer()
{
    delete _opMnu;
    writeConfig();
}

PanelSettings ExtensionContainer::defaultSettings()
{
//    kdDebug(1210) << "ExtensionContainer::defaultSettings()" << endl;

    // get defaults
    PanelSettings settings = PanelContainer::defaultSettings();

    // override some defaults
    settings._showLeftHB     = true;
    settings._showRightHB    = false;
    settings._sizePercentage = 1;
    settings._expandSize     = true;

    return settings;
}

QRect ExtensionContainer::workArea()
{
    QValueList<WId> list;
    QListIterator<PanelContainer> it(_containers);
    for( ; it.current(); ++it ) {
	if( !(*it)->inherits( "Panel" ) )
	    list.append((*it)->winId());
    }
    return PGlobal::kwin_module->workArea(list);
}

void ExtensionContainer::readConfig()
{
//    kdDebug(1210) << "ExtensionContainer::readConfig()" << endl;

    KConfig* config = new KConfig(_info.configFile());
    config->setGroup("General");

    PanelContainer::readConfig( config );

    delete config;
}

void ExtensionContainer::writeConfig()
{
//    kdDebug(1210) << "ExtensionContainer::writeConfig()" << endl;

    KConfig *config = KGlobal::config();
    config->setGroup(extensionId());

    config->writeEntry("ConfigFile", _info.configFile());
    config->writeEntry("DesktopFile", _info.desktopFile());
    config->sync();

    config = new KConfig(_info.configFile());
    config->setGroup("General");

    PanelContainer::writeConfig( config );
    config->sync();

    delete config;
}

bool ExtensionContainer::eventFilter (QObject *o, QEvent *e)
{
    if( e->type() == QEvent::MouseButtonPress )
    {
	QMouseEvent* ev = (QMouseEvent*) e;
	if ( ev->button() == RightButton )
	{
	    if (!_opMnu)
	    _opMnu = new PanelExtensionOpMenu(_actions);

	    switch(_opMnu->exec(getPopupPosition(_opMnu, ev->pos())))
	    {
		case PanelExtensionOpMenu::Move:
		    moveMe();
		    break;
		case PanelExtensionOpMenu::Remove:
		    emit removeme(this);
		    break;
		case PanelExtensionOpMenu::About:
		    about();
		    break;
		case PanelExtensionOpMenu::Help:
		    help();
		    break;
		case PanelExtensionOpMenu::Preferences:
		    preferences();
		    break;
		case PanelExtensionOpMenu::ReportBug:
		    reportBug();
		    break;
		default:
		    break;
	    }
	    return true;
	}
    }
    return PanelContainer::eventFilter(o, e);
}

void ExtensionContainer::removeSessionConfigFile()
{
    if (_info.configFile().isEmpty()) return;
    if (_info.isUniqueApplet()) return;
    QString path = locate("config", _info.configFile());

    QFile f (path);
    if (f.exists()) {
        kdDebug(1210) << "Removing session config file: " << path << endl;
        f.remove();
    }
}

InternalExtensionContainer::InternalExtensionContainer(const AppletInfo& info, QWidget *parent)
    : ExtensionContainer(info, parent)
{
    _extension = PGlobal::pluginmgr->loadExtension( info.desktopFile(),
                           info.configFile(), containerAreaBox());

    if (!_extension) return;

    _type = _extension->type();
    _actions = _extension->actions();

    connect(_extension, SIGNAL(updateLayout()), SLOT(updateLayout()));

    connect(this, SIGNAL(positionChange(Position)), _extension, SLOT(slotSetPosition(Position)));

    // initialise
    readConfig();
}

InternalExtensionContainer::~InternalExtensionContainer()
{
}

PanelSettings InternalExtensionContainer::defaultSettings()
{
//    kdDebug(1210) << "InternalExtensionContainer::defaultSettings()" << endl;

    // get defaults
    PanelSettings settings = ExtensionContainer::defaultSettings();

    // override some defaults
    if(_extension)
	settings._position = static_cast<Position>(static_cast<int>(_extension->preferedPosition()));

    return settings;
}

QSize InternalExtensionContainer::sizeHint(Position p, QSize maxSize)
{
    QSize size = PanelContainer::sizeHint( p, maxSize );

    if (_extension)
        size = _extension->sizeHint((KPanelExtension::Position)p, maxSize - size) + size;

    return size.boundedTo( maxSize );
}

void InternalExtensionContainer::slotSetPosition(Position p)
{
    if (!_extension) return;
    _extension->slotSetPosition((KPanelExtension::Position)(p));
}

void InternalExtensionContainer::about()
{
    if (!_extension) return;
    _extension->action(KPanelExtension::About);
}

void InternalExtensionContainer::help()
{
    if (!_extension) return;
    _extension->action(KPanelExtension::Help);
}

void InternalExtensionContainer::preferences()
{
    if (!_extension) return;
    _extension->action(KPanelExtension::Preferences);
}

void InternalExtensionContainer::reportBug()
{
    if (!_extension) return;
    _extension->action(KPanelExtension::ReportBug);
}

ExternalExtensionContainer::ExternalExtensionContainer(const AppletInfo& info, QWidget *parent)
    : ExtensionContainer(info, parent)
    , DCOPObject(QCString("ExternalExtensionContainer_") + kapp->randomString(20).lower().local8Bit())
    , _isdocked(false)
{

    // init QXEmbed
    _embed = new QXEmbed( containerAreaBox() );
    connect (_embed, SIGNAL(embeddedWindowDestroyed()),
             this, SIGNAL(embeddedWindowDestroyed()));

    KProcess process;
    process << "extensionproxy"
            << QCString("--configfile")
            << info.configFile()
            << QCString("--callbackid")
            << objId()
                << info.desktopFile();
    process.start(KProcess::DontCare);

    connect(this, SIGNAL(positionChange(Position)), SLOT(slotSetPosition(Position)));

    // initialise
    readConfig();
}

ExternalExtensionContainer::~ExternalExtensionContainer()
{
    QByteArray data;
    kapp->dcopClient()->send( _app, "ExtensionProxy", "removedFromPanel()", data);
}

void ExternalExtensionContainer::slotSetPosition(Position p)
{
    if (!_isdocked) return;

    QByteArray data;
    QDataStream dataStream( data, IO_WriteOnly );
    dataStream << static_cast<int>(p);

    kapp->dcopClient()->send( _app, "ExtensionProxy", "setPosition(int)", data );
}

void ExternalExtensionContainer::about()
{
    if (!_isdocked) return;

    QByteArray data;
    kapp->dcopClient()->send( _app, "ExtensionProxy", "about()", data );
}

void ExternalExtensionContainer::help()
{
    if (!_isdocked) return;

    QByteArray data;
    kapp->dcopClient()->send( _app, "ExtensionProxy", "help()", data );
}

void ExternalExtensionContainer::preferences()
{
    if (!_isdocked) return;

    QByteArray data;
    kapp->dcopClient()->send( _app, "ExtensionProxy", "preferences()", data );
}

void ExternalExtensionContainer::reportBug()
{
    if (!_isdocked) return;

    QByteArray data;
    kapp->dcopClient()->send( _app, "ExtensionProxy", "reportBug()", data );
}

QSize ExternalExtensionContainer::sizeHint(Position p, QSize maxSize)
{
    QSize size = PanelContainer::sizeHint( p, maxSize );

    if (!_isdocked)
        return size;

    QSize ms = maxSize;

    DCOPClient* dcop = kapp->dcopClient();

    QByteArray data;
    QCString replyType;
    QByteArray replyData;
    QDataStream dataStream( data, IO_WriteOnly );
    dataStream << static_cast<int>(p);
    dataStream << ms;

    if (dcop->call( _app, "ExtensionProxy", "sizeHint(int,QSize)", data, replyType, replyData ) )
        {
            QDataStream reply( replyData, IO_ReadOnly );
            reply >> ms;
        }
    return size + ms;
}

bool ExternalExtensionContainer::process(const QCString &fun, const QByteArray &data,
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
            updateLayout();
            return true;
        }
    return true;
}

void ExternalExtensionContainer::dockRequest(QCString app, int actions, int type)
{
    _app = app;
    _type = static_cast<KPanelExtension::Type>(type);
    _actions = actions;

    kdDebug(1210) << "ExternalExtensionContainer::dockRequest: " << app << endl;

    // get prefered position
    {
        QByteArray data;
        QCString replyType;
        QByteArray replyData;
        int pos;

        if (kapp->dcopClient()->call(_app, "ExtensionProxy", "preferedPosition()",
                                     data, replyType, replyData))
        {
            QDataStream reply( replyData, IO_ReadOnly );
            reply >> pos;
            ExtensionContainer::setPosition(static_cast<Position>(pos));
        }
    }

    // set position
    {
        QByteArray data;
        QDataStream dataStream( data, IO_WriteOnly );
        dataStream << static_cast<int>(position());

        kapp->dcopClient()->send( _app, "ExtensionProxy", "setPosition(int)", data );
    }

    _isdocked = true;
    updateLayout();
    emit docked(this);
}
