//-----------------------------------------------------------------------------
//
// Screen savers for KDE
//
// Copyright (c)  Martin R. Jones 1999
//

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <qcolor.h>

#include <klocale.h>
#include <kconfig.h>
#include <kstddirs.h>
#include <kdebug.h>
#include <kapp.h>
#include <kcmdlineargs.h>
#include <kcrash.h>

#include "demowin.h"
#include "saver.h"
#include <kscreensaver_vroot.h>

static const char *appName = "klock";

static const char *description = I18N_NOOP("KDE Screen Lock / Saver");

static const char *version = "2.0.0";

static const KCmdLineOptions options[] =
{
  { "setup", I18N_NOOP("Setup screen saver."), 0 },
  { "window-id wid", I18N_NOOP("Run in the specified XWindow."), 0 },
  { "root", I18N_NOOP("Run in the root XWindow."), 0 },
  { "demo", I18N_NOOP("Start screen saver in demo mode."), "default"},
  { 0,0,0 }
};

static void crashHandler( int sig )
{
#ifdef SIGABRT
    signal ( SIGABRT, SIG_DFL );
#endif
    abort();
}

//----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    KCmdLineArgs::init(argc, argv, appName, description, version);

    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app;

    KCrash::setCrashHandler( crashHandler );

    DemoWindow *demoWidget = 0;
    Window saveWin = 0;

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	
    if (args->isSet("setup"))
    {
       setupScreenSaver();
       exit(0);
    }

    if (args->isSet("window-id"))
    {
        saveWin = atol(args->getOption("window-id"));
    }

    if (args->isSet("root"))
    {
        saveWin = RootWindow(qt_xdisplay(), qt_xscreen());
    }

    if (args->isSet("demo"))
    {
        saveWin = 0;
    }

    if (saveWin == 0)
    {
        demoWidget = new DemoWindow();
        demoWidget->setBackgroundMode(QWidget::NoBackground);
//        demoWidget->setBackgroundColor(Qt::black);
        demoWidget->show();
        saveWin = demoWidget->winId();
        app.setMainWidget(demoWidget);
        app.processEvents();
    }

    startScreenSaver(saveWin);
    app.exec();
    stopScreenSaver();

    if (demoWidget)
    {
        delete demoWidget;
    }

    return 0;
}

