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

#include <qstringlist.h>
#include <qfile.h>

#include <kapp.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <klibloader.h>

#include <stdlib.h>

#include "config.h"
#include "global.h"
#include "modloader.h"

KCModule *ModuleLoader::loadModule(const ModuleInfo &mod, bool withfallback)
{
  /*
   * Simple libraries as modules are the easiest case:
   *  We just have to load the library and get the module
   *  from the factory.
   */

    /* Corel's distro builds some .desktop files at boot time. In the previous version
     * of KDE, each module was a separate executable which could be passed parameters. Sinnce
     * KDE 2 now uses libraries for the modules, this is needed to pass some info to Corel's
     * hardware control panel. Essentially it sets an environment variable to be the name of
     * the .desktop file name of the module selected from the list view.
     */
  setenv("KDE_CONTROL_CENTER_ENV1", mod.fileName().latin1(), 1);

  if (!mod.library().isEmpty())
  {
      // get the library loader instance

      KLibLoader *loader = KLibLoader::self();

      // try to load the library
      QString libname("libkcm_%1");
      KLibrary *lib = loader->library(QFile::encodeName(libname.arg(mod.library())));
      if (lib)
	{
	  // get the create_ function
	  QString factory("create_%1");
	  void *create = lib->symbol(QFile::encodeName(factory.arg(mod.handle())));

	  if (create)
	    {
	      // create the module
	      KCModule* (*func)(QWidget *, const char *);
	      func = (KCModule* (*)(QWidget *, const char *)) create;
	      return  func(0,"");
	    }
	}
    }
    else
      kdWarning() << "Module " << mod.fileName() << " doesn't specify a library!" << endl;

  /*
   * Ok, we could not load the library.
   * Try to run it as an executable.
   * This must not be done when calling from kcmshell, or you'll
   * have infinite recursion
   * (startService calls kcmshell which calls modloader which calls startService...)
   *
   */
  if(withfallback)
      kapp->startServiceByDesktopPath(mod.fileName(), QString::null);
  return 0;
}

void ModuleLoader::unloadModule(const ModuleInfo &mod)
{
  unsetenv("KDE_CONTROL_CENTER_ENV1");

	// get the library loader instance
  KLibLoader *loader = KLibLoader::self();

  // try to unload the library
  QString libname("libkcm_%1");
  loader->unloadLibrary(QFile::encodeName(libname.arg(mod.library())));
}
