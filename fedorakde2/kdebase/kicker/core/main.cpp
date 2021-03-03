/*****************************************************************

Copyright (c) 1996-2001 the kicker authors. See file AUTHORS.

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
#include <klocale.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <kaboutdata.h>
#include <kimageio.h>
#include <kcrash.h>
#include <kopenwith.h>
#include <kconfig.h>
#include <dcopclient.h>
#include <kwinmodule.h>
#include <kglobalaccel.h>

#include <X11/Xlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "main.h"
#include "panel.h"
#include "global.h"
#include "pluginmgr.h"
#include "extensionmanager.h"

int kicker_screen_number = 0;

static DCOPClient * client = 0;
//static KFileOpenWithHandler* owh = new KFileOpenWithHandler;

KickerApp::KickerApp() : KUniqueApplication()
{
    KGlobal::dirs()->addResourceType("mini", KStandardDirs::kde_default("data") +
				     "kicker/pics/mini");

    KGlobal::dirs()->addResourceType("icon", KStandardDirs::kde_default("data") +
				     "kicker/pics");

    KGlobal::dirs()->addResourceType("applets", KStandardDirs::kde_default("data") +
                                     "kicker/applets");

    KGlobal::dirs()->addResourceType("tiles", KStandardDirs::kde_default("data") +
				     "kicker/tiles");

    KGlobal::dirs()->addResourceType("extensions", KStandardDirs::kde_default("data") +
				     "kicker/extensions");

    KImageIO::registerFormats();

    KGlobal::locale()->insertCatalogue("libkonq");

    // instantiate plugin manager and kwin module
    PGlobal::pluginmgr = new KickerPluginManager();
    PGlobal::kwin_module = new KWinModule(this );
    PGlobal::globalKeys = new KGlobalAccel();

    p = new Panel();
    setMainWidget(p);
    p->show();

    // let PGlobal know we are there
    PGlobal::panel = p;

    // this must stay at the bottom
    PGlobal::extensionManager = new ExtensionManager(this);

}

KickerApp::~KickerApp()
{
    delete p;
    delete PGlobal::extensionManager;
}

static void crashHandler(int signal)
{
    KCrash::setCrashHandler(0); // exit on next crash
    fprintf(stderr, "kicker: crashHandler called\n");

    if (client)
	client->detach();  // unregister with dcop
    system("kicker --nocrashhandler &"); // try to restart
    // try to delete our main window gracefully, if this doesn't
    // crash again, it can save the system tray docks from being
    // destroyed.
    delete kapp->mainWidget();

    // Try calling the default crash handler
//    KCrash::defaultCrashHandler(signal);
}


static void sighandler(int)
{
    fprintf(stderr, "kicker: sighandler called\n");
    QApplication::exit();
}

int main( int argc, char ** argv )
{
    {
        QCString multiHead = getenv("KDE_MULTIHEAD");
        if (multiHead.lower() == "true") {
	    Display *dpy = XOpenDisplay(NULL);
	    if (! dpy) {
		fprintf(stderr, "%s: FATAL ERROR: couldn't open display %s\n",
			argv[0], XDisplayName(NULL));
		exit(1);
	    }

	    int number_of_screens = ScreenCount(dpy);
	    kicker_screen_number = DefaultScreen(dpy);
	    int pos;
	    QCString display_name = XDisplayString(dpy);
	    XCloseDisplay(dpy);
	    dpy = 0;

	    if ((pos = display_name.findRev('.')) != -1)
		display_name.remove(pos, 10);

            QCString env;
	    if (number_of_screens != 1) {
		for (int i = 0; i < number_of_screens; i++) {
		    if (i != kicker_screen_number && fork() == 0) {
			kicker_screen_number = i;
			// break here because we are the child process, we don't
			// want to fork() anymore
			break;
		    }
		}

		env.sprintf("DISPLAY=%s.%d", display_name.data(), kicker_screen_number);

		if (putenv(strdup(env.data()))) {
		    fprintf(stderr,
			    "%s: WARNING: unable to set DISPLAY environment variable\n",
			    argv[0]);
		    perror("putenv()");
		}
	    }
	}
    }

    QCString appname;
    if (kicker_screen_number == 0)
	appname = "kicker";
    else
	appname.sprintf("kicker-screen-%d", kicker_screen_number);

    KAboutData aboutData(appname.data(), I18N_NOOP("KDE Panel")
			 , "1.1"
			 , I18N_NOOP("The KDE desktop panel.")
			 , KAboutData::License_BSD
			 , "(c) 1999-2000, The KDE Team");

    aboutData.addAuthor("Matthias Elter",0, "elter@kde.org");
    aboutData.addAuthor("Matthias Ettrich",0, "ettrich@kde.org");
    aboutData.addAuthor("Wilco Greven",0, "greven@kde.org");
    aboutData.addAuthor("Rik Hemsley",0, "rik@kde.org");
    aboutData.addAuthor("Daniel M. Duley",0, "mosfet@kde.org");
    aboutData.addAuthor("Preston Brown",0, "pbrown@kde.org");
    aboutData.addAuthor("John Firebaugh",0, "jfirebaugh@kde.org");

    KCmdLineArgs::init( argc, argv, &aboutData );

    if (!KickerApp::start()) {
	kdError() << "kicker is already running!" << endl;
	return 0;
    }

    if (signal(SIGTERM, sighandler) == SIG_IGN)
	signal(SIGTERM, SIG_IGN);
    if (signal(SIGINT, sighandler) == SIG_IGN)
	signal(SIGINT, SIG_IGN);
    if (signal(SIGHUP, sighandler) == SIG_IGN)
	signal(SIGHUP, SIG_IGN);

    KickerApp a;
    a.disableSessionManagement();

    // See if a crash handler was installed. It was if the -nocrashhandler
    // argument was given, but the app eats the kde options so we can't
    // check that directly. If it wasn't, don't install our handler either.
    if( KCrash::crashHandler() != 0 ) {
//	kdDebug(1210) << "Installing crash handler" << endl;
	KCrash::setCrashHandler(crashHandler); // Try to restart on crash
    }

    client = kapp->dcopClient();
    client->setDefaultObject("Panel");
    client->send( "ksplash", "", "upAndRunning(QString)", QString(appname.data()));

    return a.exec();
}
