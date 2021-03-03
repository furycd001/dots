/* This file is part of the KDE project
   Copyright (C) 1999 David Faure (maintainer)

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

#include <qdir.h>
#include <kapp.h>
#include <kuniqueapp.h>
#include <klocale.h>
#include <kglobalsettings.h>
#include <dcopclient.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <krun.h>
#include <kopenwith.h>
#include <kcrash.h>

#include "desktop.h"
#include "krootwm.h"
#include "lockeng.h"
#include "init.h"

#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <kstddirs.h>
#include <stdlib.h>

static const char *description =
        I18N_NOOP("The KDE Desktop.");

static const char *version = "v1.9.8";

static KCmdLineOptions options[] =
{
   { "x-root", I18N_NOOP("Use this if the desktop window appears as a real window"), 0 },
   { "noautostart", I18N_NOOP("Use this to disable the Autostart folder"), 0 },
   { "waitforkded", I18N_NOOP("Wait for kded to finish building database"), 0 },
   { 0, 0, 0 }
};

// -----------------------------------------------------------------------------

int kdesktop_screen_number = 0;

static DCOPClient * client = 0;
static void crashHandler(int)
{
    KCrash::setCrashHandler(0); // exit on next crash
    if (client)
	client->detach();  // unregister with dcop
    system("kdesktop&"); // try to restart
}

static void signalHandler(int sigId)
{
  fprintf(stderr, "*** kdesktop got signal %d (Exiting)\n", sigId);
  // try to cleanup all windows
	KCrash::setCrashHandler(0); // we're quitting now anyway
	KApplication::kApplication()->quit(); // turn catchable signals into clean shutdown
}

int main( int argc, char **argv )
{
    //setup signal handling
    signal(SIGTERM, signalHandler);
    signal(SIGHUP,  signalHandler);

    {
        QCString multiHead = getenv("KDE_MULTIHEAD");
        if (multiHead.lower() == "true")
        {
       	    Display *dpy = XOpenDisplay(NULL);
	    if (! dpy) {
		fprintf(stderr,
			"%s: FATAL ERROR: couldn't open display '%s'\n",
			argv[0], XDisplayName(NULL));
		exit(1);
	    }

	    int number_of_screens = ScreenCount(dpy);
	    kdesktop_screen_number = DefaultScreen(dpy);
	    int pos;
	    QCString display_name = XDisplayString(dpy);
	    XCloseDisplay(dpy);
	    dpy = 0;

	    if ((pos = display_name.findRev('.')) != -1)
		display_name.remove(pos, 10);

            QCString env;
	    if (number_of_screens != 1) {
		for (int i = 0; i < number_of_screens; i++) {
		    if (i != kdesktop_screen_number && fork() == 0) {
			kdesktop_screen_number = i;
			// break here because we are the child process, we don't
			// want to fork() anymore
			break;
		    }
		}

		env.sprintf("DISPLAY=%s.%d", display_name.data(),
			    kdesktop_screen_number);

		if (putenv(strdup(env.data()))) {
		    fprintf(stderr,
			    "%s: WARNING: unable to set DISPLAY environment vairable\n",
			    argv[0]);
		    perror("putenv()");
		}
	    }
	}
    }

    QCString appname;
    if (kdesktop_screen_number == 0)
	appname = "kdesktop";
    else
	appname.sprintf("kdesktop-screen-%d", kdesktop_screen_number);

    KAboutData aboutData( appname.data(), I18N_NOOP("KDesktop"),
			  version, description, KAboutData::License_GPL,
			  "(c) 1998-2000, The KDesktop Authors");
    aboutData.addAuthor("Torben Weis",0, "weis@kde.org");
    aboutData.addAuthor("Matthias Ettrich",0, "ettrich@kde.org");

    KCmdLineArgs::init( argc, argv, &aboutData );
    KCmdLineArgs::addCmdLineOptions( options );

    if (!KUniqueApplication::start()) {
        fprintf(stderr, "kdesktop is already running!\n");
        exit(0);
    }
    KUniqueApplication app;
    app.disableSessionManagement(); // Do SM, but don't restart.

    KCrash::setCrashHandler(crashHandler); // Try to restart on crash
    client = kapp->dcopClient();

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    bool x_root_hack = args->isSet("x-root");
    bool auto_start = args->isSet("autostart");
    bool wait_for_kded = args->isSet("waitforkded");

    // This MUST be created before any widgets are created
    SaverEngine saver;

    // Do this before forking so that if a dialog box appears it won't
    // be covered by other apps.
    // And before creating KDesktop too, of course.
    testLocalInstallation();

    // Open-with dialog handler
    KFileOpenWithHandler fowh;

    KDesktop desktop( x_root_hack, auto_start, wait_for_kded );

    args->clear();

    app.dcopClient()->setDefaultObject( "KDesktopIface" );

    return app.exec();
}

