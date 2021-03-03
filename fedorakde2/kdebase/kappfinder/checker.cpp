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


#include <string.h>
#include <iostream>
using namespace std;


#include <qfile.h>
#include <qdir.h>


#include <kdesktopfile.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kdebug.h>


#include "checker.h"


void copyFile(QString infname, QString outfname)
{
  QFile inf(infname);
  if (inf.open(IO_ReadOnly))
    {
      QFile outf(outfname);
      if (outf.open(IO_WriteOnly))
	{
	  outf.writeBlock(inf.readAll());
	
	  outf.close();
	}

      inf.close();
    }
}


bool checkDesktopFile(const QString &templ, QString destDir)
{
  KDesktopFile desktop(templ, true);

  // find out where to put the .desktop files
  QString destName;
  if (destDir.isNull())
    destDir = KGlobal::dirs()->saveLocation("apps");

  // find out the name of the file to store
  destName = templ;
  int pos = templ.find("kappfinder/apps/");
  if (pos > 0)
    destName = destName.mid(pos+strlen("kappfinder/apps/"));

  // calculate real dir and filename
  destName = destDir+destName;
  pos = destName.findRev('/');
  if (pos > 0)
    {
      destDir = destName.left(pos);
      destName = destName.mid(pos+1);
    }

  // determine for which executable to look
  QString exec = desktop.readEntry("TryExec");
  if (exec.isEmpty())
    exec = desktop.readEntry("Exec");
  pos = exec.find(' ');
  if (pos > 0)
    exec = exec.left(pos);

  cout << "looking for " << exec.local8Bit() << "\t\t";

  // try to locate the binary
  QString pexec = KGlobal::dirs()->findExe(exec);
  if (pexec.isEmpty())
    {
      cout << "not found" << endl;
      return false;
    }

  cout << pexec.local8Bit() << endl;

  kdDebug() << "Writing " << destDir << "/./" << destName << endl;

  // create the directories
  destDir += "/";
  QDir d;
  pos = -1;
  while ((pos = destDir.find('/', pos+1)) >= 0)
    {
      QString path = destDir.left(pos+1);
      d = path;
      if (!d.exists())
	{
	  d.mkdir(path);
	}
    }

  // write out the desktop file
  copyFile(templ, destDir+"/"+destName);

  return true;
}


void decorateDirs(QString destDir)
{
  // find out where to put the .directory files
  QString destName;
  if (destDir.isNull())
    destDir = KGlobal::dirs()->saveLocation("apps");

  QStringList _dirs = KGlobal::dirs()->findAllResources("data", "kappfinder/apps/*.directory", true);

  QStringList::Iterator it;
  for (it = _dirs.begin(); it != _dirs.end(); ++it)
    {
      // find out the name of the file to store
      destName = *it;
      int pos = destName.find("kappfinder/apps/");
      if (pos > 0)
	destName = destName.mid(pos+strlen("kappfinder/apps/"));

      destName = destDir+"/"+destName;

      if (!QFile::exists(destName))
	{	
	  kdDebug() << "Copy " << *it << " to " << destName << endl;
	  copyFile(*it, destName);
	}
    }
}
