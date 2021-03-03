/*
  Copyright (c) 2000 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <kapp.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kaboutdata.h>


#include "scanner.h"


static const char* description=I18N_NOOP("KDE's application finder");


int main(int argc, char *argv[])
{
  KAboutData aboutData("kappfinder", I18N_NOOP("KAppfinder"),
    "1.0", description, KAboutData::License_GPL,
    "(c) 1998-2000, Matthias Hoelzer-Kluepfel");
  aboutData.addAuthor("Matthias Hoelzer-Kluepfel",0, "hoelzer@kde.org");

  KCmdLineArgs::init( argc, argv, &aboutData );

  KApplication app;

  Scanner *dlg = new Scanner;
  app.setMainWidget(dlg);

  dlg->exec();
}
