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
AN ACTION OF CONTRACT, TORT OR OTDHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include <stdlib.h>

#include <qstring.h>
#include <qfile.h>
#include <qxembed.h>

#include <kapp.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <kpanelapplet.h>
#include <kaboutdata.h>
#include <qfileinfo.h>
#include <dcopclient.h>
#include <kwin.h>

#include "pluginmgr.h"
#include "appletinfo.h"
#include "appletproxy.h"
#include "appletproxy.moc"

#include <X11/Xlib.h>


static KCmdLineOptions options[] =
{
  { "+desktopfile", I18N_NOOP("The applets desktop file."), 0 },
  { "configfile <file>", I18N_NOOP("The config file to be used."), 0 },
  { "callbackid <id>", I18N_NOOP("DCOP callback id of the applet container."), 0 },
  { 0, 0, 0}
};

int main( int argc, char ** argv )
{
    KAboutData aboutData( "appletproxy", I18N_NOOP("Panel applet proxy.")
                          , "v0.1.0"
                          ,I18N_NOOP("Panel applet proxy.")
                          , KAboutData::License_BSD
                          , "(c) 2000, The KDE Developers");
    KCmdLineArgs::init(argc, argv, &aboutData );
    aboutData.addAuthor("Matthias Elter",0, "elter@kde.org");
    aboutData.addAuthor("Matthias Ettrich",0, "ettrich@kde.org");
    KApplication::addCmdLineOptions();
    KCmdLineArgs::addCmdLineOptions(options); // Add our own options.

    KApplication a;
    a.disableSessionManagement();

    KGlobal::dirs()->addResourceType("applets", KStandardDirs::kde_default("data") +
				     "kicker/applets");

    // setup proxy object
    AppletProxy proxy(0, "appletproxywidget");

    // parse cmdline args
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    if ( args->count() == 0 )
        KCmdLineArgs::usage(i18n("No desktop file specified") );

    // Perhaps we should use a konsole-like solution here (shell, list of args...)
    QCString desktopfile = QCString( args->arg(0) );

    // load applet DSO
    proxy.loadApplet( desktopfile, args->getOption("configfile"));

    // dock into our applet container
    QCString callbackid = args->getOption( "callbackid");
    if ( callbackid.isEmpty() )
	proxy.showStandalone();
    else
	proxy.dock(args->getOption("callbackid"));

    return a.exec();
}

AppletProxy::AppletProxy(QObject* parent, const char* name)
  : QObject(parent, name)
  , DCOPObject("AppletProxy")
  , _info(0)
  , _applet(0)
  , _loader(0)
{
    _loader = new KickerPluginManager;

    // try to attach to DCOP server
    if (!kapp->dcopClient()->attach()) {
	kdError() << "Failed to attach to DCOP server." << endl;
	exit(0);
    }

    if (!kapp->dcopClient()->registerAs("applet_proxy", true)) {
	kdError() << "Failed to register at DCOP server." << endl;
	exit(0);
    }
}

AppletProxy::~AppletProxy()
{
    kapp->dcopClient()->detach();
}

void AppletProxy::loadApplet(const QCString& desktopFile, const QCString& configFile)
{
    QString df;

    // try simple path first
    QFileInfo finfo( desktopFile );
    if ( finfo.exists() ) {
	df = finfo.absFilePath();
    } else {
	// locate desktop file
	df = KGlobal::dirs()->findResource("applets", QString(desktopFile));
    }

    QFile file(df);
    // does the config file exist?
    if (df.isNull() || !file.exists()) {
	kdError() << "Failed to locate applet desktop file: " << desktopFile << endl;
	exit(0);
    }

    // create AppletInfo instance
    _info = new AppletInfo(df);

    // set the config file
    if (!configFile.isNull())
	_info->setConfigFile(configFile);

    // load applet DSO
    if (_loader) _applet = _loader->loadApplet(df, _info->configFile(), 0);

    // sanity check
    if (!_applet) {
	kdError() << "Failed to load applet DSO: " << _info->library() << endl;
	exit(0);
    }

    // connect updateLayout signal
    connect(_applet, SIGNAL(updateLayout()), SLOT(slotUpdateLayout()));
    // connect requestFocus signal
    connect(_applet, SIGNAL(requestFocus()), SLOT(slotRequestFocus()));
}

void AppletProxy::dock(const QCString& callbackID)
{
    kdDebug(1210) << "Callback ID: " << callbackID << endl;

    _callbackID = callbackID;

    // try to attach to DCOP server
    DCOPClient* dcop = kapp->dcopClient();

    dcop->setNotifications(true);
    connect(dcop, SIGNAL(applicationRemoved(const QCString&)),
	    SLOT(slotApplicationRemoved(const QCString&)));

    WId win;

    // get docked
    {
	QCString replyType;
	QByteArray data, replyData;
	QDataStream dataStream( data, IO_WriteOnly );

	int actions = 0;
	if(_applet) actions = _applet->actions();
	dataStream << actions;

	int type = 0;
	if (_applet) type = static_cast<int>(_applet->type());
	dataStream << type;

	// we use "call" to know whether it was sucessful

	int screen_number = 0;
	if (qt_xdisplay())
	    screen_number = DefaultScreen(qt_xdisplay());
	QCString appname;
	if (screen_number == 0)
	    appname = "kicker";
	else
	    appname.sprintf("kicker-screen-%d", screen_number);

	if ( !dcop->call(appname, _callbackID, "dockRequest(int,int)",
			 data, replyType, replyData ) )
        {
            kdError() << "Failed to dock into the panel." << endl;
            exit(0);
        }

	QDataStream reply( replyData, IO_ReadOnly );
	reply >> win;

    }

    if (win) {
        _applet->hide();
        QXEmbed::initialize();
        QXEmbed::embedClientIntoWindow( _applet, win );
    }
    else {
        kdError() << "Failed to dock into the panel." << endl;
        if(_applet) delete _applet;
        exit(0);
    }

}

bool AppletProxy::process(const QCString &fun, const QByteArray &data,
                          QCString& replyType, QByteArray &replyData)
{
    if ( fun == "widthForHeight(int)" )
	{
	    QDataStream dataStream( data, IO_ReadOnly );
	    int height;
	    dataStream >> height;
	    QDataStream reply( replyData, IO_WriteOnly );
	    replyType = "int";

	    if (!_applet)
		reply << height;
	    else
		reply << _applet->widthForHeight(height);

	    return true;
	}
    else if ( fun == "heightForWidth(int)" )
	{
	    QDataStream dataStream( data, IO_ReadOnly );
	    int width;
	    dataStream >> width;
	    QDataStream reply( replyData, IO_WriteOnly );
	    replyType = "int";

	    if(!_applet)
		reply << width;
	    else
		reply << _applet->heightForWidth(width);

	    return true;
	}
    else if ( fun == "setDirection(int)" )
	{
	    QDataStream dataStream( data, IO_ReadOnly );
	    int dir;
	    dataStream >> dir;

	    if(_applet) {
		_applet->slotSetPopupDirection(static_cast<KPanelApplet::Direction>(dir));
	    }
	    return true;
	}
    else if ( fun == "setOrientation(int)" )
	{
	    QDataStream dataStream( data, IO_ReadOnly );
	    int orient;
	    dataStream >> orient;

	    if(_applet) {
		_applet->slotSetOrientation(static_cast<Qt::Orientation>(orient));
	    }
	    return true;
	}
    else if ( fun == "removedFromPanel()" )
	{
            if(_applet) delete _applet;
            exit(0);
	    return true;
	}
    else if ( fun == "about()" )
	{
	    if(_applet) _applet->action( KPanelApplet::About );
	    return true;
	}
    else if ( fun == "help()" )
	{
	    if(_applet) _applet->action( KPanelApplet::Help );
	    return true;
	}
    else if ( fun == "preferences()" )
	{
	    if(_applet) _applet->action( KPanelApplet::Preferences );
	    return true;
	}
    else if (fun == "reportBug()" )
  {
      if(_applet) _applet->action( KPanelApplet::ReportBug );
      return true;
  }
    else if ( fun == "actions()" )
	{
	    QDataStream reply( replyData, IO_WriteOnly );
	    int actions = 0;
	    if(_applet) actions = _applet->actions();
	    reply << actions;
	    replyType = "int";
	    return true;
	}
    else if ( fun == "type()" )
	{
	    QDataStream reply( replyData, IO_WriteOnly );
	    int type = 0;
	    if (_applet) type = static_cast<int>(_applet->type());
	    reply << type;
	    replyType = "int";
	    return true;
	}
    return false;
}

void AppletProxy::slotUpdateLayout()
{
    if(_callbackID.isNull()) return;

    QByteArray data;
    int screen_number = 0;
    if (qt_xdisplay())
	screen_number = DefaultScreen(qt_xdisplay());
    QCString appname;
    if (screen_number == 0)
	appname = "kicker";
    else
	appname.sprintf("kicker-screen-%d", screen_number);

    kapp->dcopClient()->send(appname, _callbackID, "updateLayout()", data);
}

void AppletProxy::slotRequestFocus()
{
    if(_callbackID.isNull()) return;

    QByteArray data;
    int screen_number = 0;
    if (qt_xdisplay())
	screen_number = DefaultScreen(qt_xdisplay());
    QCString appname;
    if (screen_number == 0)
	appname = "kicker";
    else
	appname.sprintf("kicker-screen-%d", screen_number);

    kapp->dcopClient()->send(appname, _callbackID, "requestFocus()", data);
}

void AppletProxy::slotApplicationRemoved(const QCString& appId)
{
    int screen_number = 0;
    if (qt_xdisplay())
	screen_number = DefaultScreen(qt_xdisplay());
    QCString appname;
    if (screen_number == 0)
	appname = "kicker";
    else
	appname.sprintf("kicker-screen-%d", screen_number);

    if(appId == appname) {
	kdDebug(1210) << "Connection to kicker lost, shutting down" << endl;
	kapp->quit();
    }
}

void AppletProxy::showStandalone()
{
    _applet->resize( _applet->widthForHeight( 48 ), 48 );
    _applet->setMinimumSize( _applet->size() );
    _applet->setCaption( _info->name() );
    KWin::setType( _applet->winId(), NET::Tool );
    kapp->setMainWidget( _applet );
    _applet->show();
}
