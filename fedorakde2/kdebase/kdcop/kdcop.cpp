/*
 * $Id: kdcop.cpp,v 1.1 2000/07/18 13:36:08 kalle Exp $
 *
 * Copyright (C) 2000 by Matthias Kalle Dalheimer <kalle@kde.org>
 *
 * Licensed under the Artistic License.
 */
#include <kapp.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>

#include "kdcopwindow.h"

static const KCmdLineOptions options[] =
{
	{ 0, 0, 0}
};

int main( int argc, char ** argv )
{
  KAboutData aboutData( "kdcop", I18N_NOOP("KDCOP"),
			"0.1", I18N_NOOP( "A graphical DCOP browser/client" ), 
			KAboutData::License_Artistic,
			"(c) 2000, Matthias Kalle Dalheimer");
  aboutData.addAuthor("Matthias Kalle Dalheimer",0, "kalle@kde.org");
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options );

  KApplication a;

  KDCOPWindow* kdcopwindow = new KDCOPWindow;
  a.setMainWidget( kdcopwindow );
  kdcopwindow->show();

  return a.exec();
}
