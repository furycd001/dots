/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Die Mai 22 17:24:18 CEST 2001
    copyright            : (C) 2001 by Ralf Nolden
    email                : nolden@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <dcopclient.h>

#include "kpersonalizer.h"

static const char *description =
	I18N_NOOP("KPersonalizer");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE
	
	
static KCmdLineOptions options[] =
{
  { 0, 0, 0 }
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};

int main(int argc, char *argv[])
{

  KAboutData aboutData( "kpersonalizer", I18N_NOOP("KPersonalizer"),
    VERSION, description, KAboutData::License_GPL,
    "(c) 2001, Ralf Nolden", 0, 0, "nolden@kde.org");
  aboutData.addAuthor("Ralf Nolden",0, "nolden@kde.org");
  aboutData.addAuthor("Carsten Wolff",0, "AirWulf666@gmx.net");
  aboutData.addAuthor("qwertz",0, "kraftw@gmx.de");
  aboutData.addAuthor("Bernhard Rosenkraenzer", 0, "bero@redhat.com");
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  KLocale::setMainCatalogue("kpersonalizer");

  KApplication a;
  if ( !kapp->dcopClient()->isAttached() )
    kapp->dcopClient()->attach();

  KPersonalizer *kpersonalizer = new KPersonalizer();
  a.setMainWidget(kpersonalizer);
  kpersonalizer->show();  

  return a.exec();
}
