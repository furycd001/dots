/****************************************************************************

 KHotKeys -  (C) 2000 Lubos Lunak <l.lunak@email.cz>

 main.cpp  -

 $Id: main.cpp,v 1.4 2001/02/23 14:49:27 mkretz Exp $

****************************************************************************/

#define __main_CPP

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kcmdlineargs.h>

#include "khotkeys.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


// for multihead
int khotkeys_screen_number = 0;


int main( int argc, char** argv )
{                             // no need to i18n these, no GUI
    {
	// multiheaded hotkeys
	KInstance inst("khotkeys-multihead");
	KConfig config("kdeglobals", true);
	config.setGroup("X11");
	if (config.readBoolEntry("enableMultihead")) {
	    Display *dpy = XOpenDisplay(NULL);
	    if (! dpy) {
		fprintf(stderr, "%s: FATAL ERROR while trying to open display %s\n",
			argv[0], XDisplayName(NULL));
		exit(1);
	    }

	    int number_of_screens = ScreenCount(dpy);
	    khotkeys_screen_number = DefaultScreen(dpy);
	    int pos;
	    QCString displayname = XDisplayString(dpy);
	    XCloseDisplay(dpy);
	    dpy = 0;

	    if ((pos = displayname.findRev('.')) != -1)
		displayname.remove(pos, 10);

	    QCString env;
	    if (number_of_screens != 1) {
		for (int i = 0; i < number_of_screens; i++) {
		    if (i != khotkeys_screen_number && fork() == 0) {
			khotkeys_screen_number = i;
			// break here because we are the child process, we don't
			// want to fork() anymore
			break;
		    }
		}

		env.sprintf("DISPLAY=%s.%d", displayname.data(), khotkeys_screen_number);
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
    if (khotkeys_screen_number == 0)
	appname = "khotkeys";
    else
	appname.sprintf("khotkeys-screen-%d", khotkeys_screen_number);

    KCmdLineArgs::init( argc, argv, appname, "KHotKeys", "1.5" );
    KUniqueApplication::addCmdLineOptions();
    if( !KHotKeysApp::start()) // already running
        return 0;
    KHotKeysApp app;
    app.disableSessionManagement(); // started from startkde now
    return app.exec();
}
