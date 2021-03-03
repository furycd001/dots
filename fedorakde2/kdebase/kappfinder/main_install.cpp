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

#include <stdio.h>


#include <kinstance.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kcmdlineargs.h>
#include <klocale.h>


#include <qstringlist.h>


#include "checker.h"


int main(int argc, char *argv[])
{
  KInstance instance("kappfinder_install");

  if (argc != 2)
    {
      fprintf(stderr, "Usage: kappfinder_install $directory\n");
      return -1;
    }

  QStringList _templates = KGlobal::dirs()->findAllResources("data", "kappfinder/apps/*.desktop", true);

  QString dir = QString(argv[1])+"/";

  QStringList::Iterator it;
  for (it = _templates.begin(); it != _templates.end(); ++it)
    checkDesktopFile(*it, dir);

  decorateDirs(dir);

  return 0;
}
