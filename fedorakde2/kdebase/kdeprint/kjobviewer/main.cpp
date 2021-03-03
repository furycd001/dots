/*
 *  This file is part of the KDE libraries
 *  Copyright (c) 2001 Michael Goffioul <goffioul@imec.be>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#include <kapp.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include <kjobviewer.h>
#include <klocale.h>
#include <stdlib.h>

static KCmdLineOptions options[] = {
	{ "d <printer-name>", I18N_NOOP("The printer for which jobs are requested"), 0 },
	{ "show", I18N_NOOP("Show job viewer at startup"), 0},
	{ "all", I18N_NOOP("Show jobs for all printers"), 0},
	{ 0, 0, 0}
};


int main(int argc, char *argv[])
{
	KAboutData	aboutData("kjobviewer","KJobViewer","0.1","A print job viewer",KAboutData::License_GPL,"(c) 2001, Michael Goffioul");
	aboutData.addAuthor("Michael Goffioul",0,"goffioul@imec.be");
	KCmdLineArgs::init(argc,argv,&aboutData);
	KCmdLineArgs::addCmdLineOptions(options);
	KJobViewerApp::addCmdLineOptions();

	if (!KJobViewerApp::start())
		exit(0);

	KJobViewerApp	a;
	return a.exec();
}
