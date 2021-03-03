/***************************************************************************
                         chooser.cpp  -  description
                             -------------------
    begin                : Tue Nov 9 1999
    copyright            : (C) 1999 by Harald Hoyer
    email                : Harald.Hoyer@RedHat.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "DXdmcp.h"
#include <kapp.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdlib.h>

static const char *description = I18N_NOOP("Login chooser for Xdmcp");

static const char *version = "v0.1";

static ChooserDlg *kchooser = 0;


class MyApp:public KApplication {

  public:
    virtual bool x11EventFilter(XEvent *);
};


bool MyApp::x11EventFilter(XEvent * ev)
{
    // Hack to tell dialogs to take focus
    if (ev->type == ConfigureNotify) {
	QWidget *target = QWidget::find(((XConfigureEvent *) ev)->window);
	if (target) {
	    target = target->topLevelWidget();
	    if (target->isVisible() && !target->isPopup())
		XSetInputFocus(qt_xdisplay(), target->winId(),
			       RevertToParent, CurrentTime);
	}
    }

    return FALSE;
}

static KCmdLineOptions options[] = {
    /* XXX use I18N_NOOP !!!! */
    {"xdmaddress <addr>", "Specify the chooser socket (in hex)", 0},
    {"clientaddress <addr>", "Specify the client ip (in hex)", 0},
    {"connectionType <type>", "Specify the connection type (in dec)", 0},
    {"+[host]", "Specify the hosts to list or use BROADCAST", 0}
};

int main(int argc, char **argv)
{
    KCmdLineArgs::init(argc, argv, "chooser", description, version);
    KCmdLineArgs::addCmdLineOptions(options);

    CXdmcp *cxdmcp = new CXdmcp();

    MyApp app;

    kchooser = new ChooserDlg(cxdmcp);

    app.setMainWidget(kchooser);

    kchooser->show();
    kchooser->ping();
    app.exec();

    exit(0);
}
