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
#include <kpanelextension.h>
#include <kaboutdata.h>
#include <qfileinfo.h>
#include <dcopclient.h>

#include "pluginmgr.h"
#include "appletinfo.h"
#include "extensionproxy.h"
#include "extensionproxy.moc"

#include <X11/Xlib.h>


static KCmdLineOptions options[] =
{
  { "+desktopfile", I18N_NOOP("The extensions desktop file."), 0 },
  { "configfile <file>", I18N_NOOP("The config file to be used."), 0 },
  { "callbackid <id>", I18N_NOOP("DCOP callback id of the extension container."), 0 },
  { 0, 0, 0}
};

int main( int argc, char ** argv )
{
    KAboutData aboutData( "extensionproxy", I18N_NOOP("Panel extension proxy.")
                          , "v0.1.0"
                          ,I18N_NOOP("Panel extension proxy.")
                          , KAboutData::License_BSD
                          , "(c) 2000, The KDE Developers");
    KCmdLineArgs::init(argc, argv, &aboutData );
    aboutData.addAuthor("Matthias Elter",0, "elter@kde.org");
    aboutData.addAuthor("Matthias Ettrich",0, "ettrich@kde.org");
    KApplication::addCmdLineOptions();
    KCmdLineArgs::addCmdLineOptions(options); // Add our own options.

    KApplication a;
    a.disableSessionManagement();

    KGlobal::dirs()->addResourceType("extensions", KStandardDirs::kde_default("data") +
				     "kicker/extensions");

    // setup proxy object
    ExtensionProxy proxy(0, "extensionproxywidget");

    // parse cmdline args
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    // sanity check
    if ( args->count() == 0 )
        KCmdLineArgs::usage(i18n("No desktop file specified") );

    // do we have a callback id?
    if (args->getOption("callbackid").isNull()) {
	kdError() << "Callback ID is null. " << endl;
	exit(0);
    }

    // Perhaps we should use a konsole-like solution here (shell, list of args...)
    QCString desktopfile = QCString( args->arg(0) );

    // load extension DSO
    proxy.loadExtension( desktopfile, args->getOption("configfile"));

    // dock into our extension container
    proxy.dock(args->getOption("callbackid"));

    return a.exec();
}

ExtensionProxy::ExtensionProxy(QObject* parent, const char* name)
  : QObject(parent, name)
  , DCOPObject("ExtensionProxy")
  , _info(0)
  , _extension(0)
  , _loader(0)
{
    _loader = new KickerPluginManager;

    // try to attach to DCOP server
    if (!kapp->dcopClient()->attach()) {
	kdError() << "Failed to attach to DCOP server." << endl;
	exit(0);
    }

    if (!kapp->dcopClient()->registerAs("extension_proxy", true)) {
	kdError() << "Failed to register at DCOP server." << endl;
	exit(0);
    }
}

ExtensionProxy::~ExtensionProxy()
{
    kapp->dcopClient()->detach();
}

void ExtensionProxy::loadExtension(const QCString& desktopFile, const QCString& configFile)
{
    QString df;

    // try simple path first
    QFileInfo finfo( desktopFile );
    if ( finfo.exists() ) {
	df = finfo.absFilePath();
    } else {
	// locate desktop file
	df = KGlobal::dirs()->findResource("extensions", QString(desktopFile));
    }

    QFile file(df);
    // does the config file exist?
    if (df.isNull() || !file.exists()) {
	kdError() << "Failed to locate extension desktop file: " << desktopFile << endl;
	exit(0);
    }

    // create AppletInfo instance
    _info = new AppletInfo(df);

    // set the config file
    if (!configFile.isNull())
	_info->setConfigFile(configFile);

    // load extension DSO
    if (_loader) _extension = _loader->loadExtension(df, _info->configFile(), 0);

    // sanity check
    if (!_extension) {
	kdError() << "Failed to load extension DSO: " << _info->library() << endl;
	exit(0);
    }

    // connect updateLayout signal
    connect(_extension, SIGNAL(updateLayout()), SLOT(slotUpdateLayout()));
}

void ExtensionProxy::dock(const QCString& callbackID)
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
	if(_extension) actions = _extension->actions();
	dataStream << actions;

	int type = 0;
	if (_extension) type = static_cast<int>(_extension->type());
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
        _extension->hide();
        QXEmbed::initialize();
        QXEmbed::embedClientIntoWindow( _extension, win );
    }
    else {
        kdError() << "Failed to dock into the panel." << endl;
        if(_extension) delete _extension;
        exit(0);
    }
}

bool ExtensionProxy::process(const QCString &fun, const QByteArray &data,
                          QCString& replyType, QByteArray &replyData)
{
    if ( fun == "sizeHint(int,QSize)" )
	{
	    QDataStream dataStream( data, IO_ReadOnly );
	    int pos;
	    QSize maxSize;
	    dataStream >> pos;
	    dataStream >> maxSize;

	    QDataStream reply( replyData, IO_WriteOnly );
	    replyType = "QSize";

	    if(!_extension)
		reply << maxSize;
	    else
		reply << _extension->sizeHint((KPanelExtension::Position)pos, maxSize);

	    return true;
	}
    else if ( fun == "setPosition(int)" )
	{
	    QDataStream dataStream( data, IO_ReadOnly );
	    int pos;
	    dataStream >> pos;

	    if(_extension) {
		_extension->slotSetPosition(static_cast<KPanelExtension::Position>(pos));
	    }
	    return true;
	}
    else if ( fun == "removedFromPanel()" )
    {
        if(_extension) delete _extension;
        exit(0);
        return true;
    }
    else if ( fun == "about()" )
	{
	    if(_extension) _extension->action( KPanelExtension::About );
	    return true;
	}
    else if ( fun == "help()" )
	{
	    if(_extension) _extension->action( KPanelExtension::Help );
	    return true;
	}
    else if ( fun == "preferences()" )
    {
        if(_extension) _extension->action( KPanelExtension::Preferences );
        return true;
    }
    else if ( fun == "reportBug()" )
    {
        if(_extension) _extension->action( KPanelExtension::ReportBug );
        return true;
    }
    else if ( fun == "actions()" )
	{
	    QDataStream reply( replyData, IO_WriteOnly );
	    int actions = 0;
	    if(_extension) actions = _extension->actions();
	    reply << actions;
	    replyType = "int";
	    return true;
	}
    else if ( fun == "preferedPosition()" )
	{
	    QDataStream reply( replyData, IO_WriteOnly );
	    int pos = static_cast<int>(KPanelExtension::Bottom);
	    if(_extension) pos = static_cast<int>(_extension->preferedPosition());
	    reply << pos;
	    replyType = "int";
	    return true;
	}
    else if ( fun == "type()" )
	{
	    QDataStream reply( replyData, IO_WriteOnly );
	    int type = 0;
	    if (_extension) type = static_cast<int>(_extension->type());
	    reply << type;
	    replyType = "int";
	    return true;
	}
    return false;
}

void ExtensionProxy::slotUpdateLayout()
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

void ExtensionProxy::slotApplicationRemoved(const QCString& appId)
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
