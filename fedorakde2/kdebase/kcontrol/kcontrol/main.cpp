/*
  Copyright (c) 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
  Copyright (c) 2000 Matthias Elter <elter@kde.org>

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


/**
 * Howto debug:
 *    start "kcontrol --nofork" in a debugger.
 *
 * If you want to test with command line arguments you need
 * -after you have started kcontrol in the debugger-
 * open another shell and run kcontrol with the desired
 * command line arguments.
 *
 * The command line arguments will be passed to the version of
 * kcontrol in the debugger via DCOP and will cause a call
 * to newInstance().
 */

#include <kcmdlineargs.h>
#include <dcopclient.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kdebug.h>

#include "main.h"
#include "main.moc"
#include "toplevel.h"
#include "global.h"

KControlApp::KControlApp()
  : KUniqueApplication()
  , toplevel(0)
{
  toplevel = new TopLevel();

  setMainWidget(toplevel);

  KConfig *config = KGlobal::config();
  config->setGroup("General");
  QWidget *desk = QApplication::desktop();
  int x = config->readNumEntry(QString::fromLatin1("InitialWidth %1").arg(desk->width()), 740);
  int y = config->readNumEntry(QString::fromLatin1("InitialHeight %1").arg(desk->height()), 540);
  toplevel->resize(x,y);
}

KControlApp::~KControlApp()
{
  if (toplevel)
    {
      KConfig *config = KGlobal::config();
      config->setGroup("General");
      QWidget *desk = QApplication::desktop();
      config->writeEntry(QString::fromLatin1("InitialWidth %1").arg(desk->width()), toplevel->width());
      config->writeEntry(QString::fromLatin1("InitialHeight %1").arg(desk->height()), toplevel->height());
      config->sync();
    }
  delete toplevel;
}

int main(int argc, char *argv[])
{
  KAboutData aboutData( "kcontrol", I18N_NOOP("KDE Control Center"),
    "v2.0", I18N_NOOP("The KDE Control Center"), KAboutData::License_GPL,
    I18N_NOOP("(c) 1998-2000, The KDE Control Center Developers"));
  aboutData.addAuthor("Matthias Hoelzer-Kluepfel",0, "hoelzer@kde.org");
  aboutData.addAuthor("Matthias Elter",0, "elter@kde.org");

  KCmdLineArgs::init( argc, argv, &aboutData );
  KUniqueApplication::addCmdLineOptions();

  KCGlobal::init();

  if (!KControlApp::start()) {
	kdDebug() << "kcontrol is already running!\n" << endl;
	return (0);
  }

  KControlApp app;

  // show the whole stuff
  app.mainWidget()->show();

  return app.exec();
}
