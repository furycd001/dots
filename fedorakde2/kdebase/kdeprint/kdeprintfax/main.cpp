/*
 *   kdeprintfax - a interface to fax-packages
 *   Copyright (C) 2001  Michael Goffioul
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "kdeprintfax.h"

#include <qfile.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kapp.h>

QString debugFlag;
int oneShotFlag = false;

static const char *description =
	I18N_NOOP("A small fax utility to be used with kdeprint.");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE


static KCmdLineOptions options[] =
{
  { "+[file]", I18N_NOOP("File to fax (added to the file list)"), 0 },
  { 0, 0, 0 }
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};

int main(int argc, char *argv[])
{

  KAboutData aboutData( "kdeprintfax", I18N_NOOP("KdeprintFax"),
    "1.0", description, KAboutData::License_GPL,
    "(c) 2001 Michael Goffioul");
  aboutData.addAuthor("Michael Goffioul",0, "goffioul@imec.be");
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.
  KApplication::addCmdLineOptions();

  KApplication a;
  KCmdLineArgs	*args = KCmdLineArgs::parsedArgs();

  KdeprintFax	*w = new KdeprintFax;
  a.setMainWidget(w);
  w->show();
  for (int i=0;i<args->count();i++)
  	w->addURL(args->url(i));

  args->clear();
  return a.exec();
}
