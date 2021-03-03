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

#include "printwrapper.h"

#include <qtimer.h>
#include <kapp.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static KCmdLineOptions options[] =
{
	{ "d <printer>",      I18N_NOOP("Printer/destination to print on"),      0},
	{ "t <title>",        I18N_NOOP("Title for the print job" ),              0},
	{ "o <option=value>", I18N_NOOP("Printer option" ),                       0},
	{ "j <mode>",         I18N_NOOP("Job output mode (gui, console, none)" ), "gui"},
	{ "system <printsys>",I18N_NOOP("Print system to use (lpd, cups)" ), 0},
	{ "nostdin",          I18N_NOOP("Forbid impression from STDIN" ),         0},
	{ "+file(s)",	      I18N_NOOP("Files to load" ),                        0},
	{ 0,                  0,                                      0}
};

int main(int argc, char *argv[])
{
	KCmdLineArgs::init(argc,argv,"kprinter",I18N_NOOP("A printer tool for KDE" ),"0.0.1");
	KCmdLineArgs::addCmdLineOptions(options);
	KApplication	app;
	PrintWrapper	wrap;
	app.setMainWidget(&wrap);
	QTimer::singleShot(10,&wrap,SLOT(slotPrint()));

	return app.exec();
}
