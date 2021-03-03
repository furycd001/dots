/* This file is part of the KDE project
   Copyright (C) 1999 David Faure <faure@kde.org>

   $Id: init.cc,v 1.18.2.1 2001/08/15 10:18:14 faure Exp $

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qdir.h>
#include <qstring.h>
#include <qfile.h>

#include <kstddirs.h>
#include <kconfig.h>
#include <kdesktopfile.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kglobalsettings.h>
#include <kapp.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>


// for multihead
extern int kdesktop_screen_number;


/**
 * Test if a directory exists, create otherwise
 * @param _name full path of the directory
 * @param _showMsg show a message box if we created the dir
 * @return true if the dir was created
 */
bool testDir( const QString &_name )
{
  DIR *dp;
  dp = opendir( QFile::encodeName(_name) );
  if ( dp == NULL )
  {
    QString m = _name;
    if ( m.right(1) == "/" )
      m.truncate( m.length() - 1 );

    //      KMessageBox::information( 0, i18n("Creating directory:\n") + m );
    ::mkdir( QFile::encodeName(m), S_IRWXU );
    return true;
  }
  else
  {
    closedir( dp );
    return false;
  }
}

/**
 * Copy a standard .directory file to a user's directory
 * @param fileName destination file name
 * @param dir destination directory
 * @param force if false, don't copy if destination file already exists
 */
void copyDirectoryFile(const char *fileName, const QString& dir, bool force)
{
  if (force || !QFile::exists(dir + "/.directory")) {
    QCString cmd;
    cmd.sprintf( "cp %s %s/.directory",
     QFile::encodeName(locate("data", QString("kdesktop/") + fileName)).data(),
     QFile::encodeName(dir).data() );
    system( cmd );
  }
}

/**
 * Copy all links from DesktopLinks/ to the desktop, appropriately renamed
 * (to the contents of the translated Name field)
 */
void copyDesktopLinks()
{
    QStringList list =
	KGlobal::dirs()->findAllResources("appdata", "DesktopLinks/*", false, true);

    QString desktopPath = KGlobalSettings::desktopPath();
    if (kdesktop_screen_number != 0) {
	QString dn = "Desktop";
	dn += QString::number(kdesktop_screen_number);
	desktopPath.replace(QRegExp("Desktop"), dn);
    }

    for (QStringList::ConstIterator it = list.begin(); it != list.end(); it++) {
	QCString cmd = "cp '";
	KDesktopFile desk( *it );
	cmd += QFile::encodeName(*it);
	cmd += "' '";
	cmd += QFile::encodeName(desktopPath + desk.readName());
	cmd += "'";
	system( cmd );
    }
}

/**
 * @return true if this is the first time
 * kdesktop is run for the current release
 * WARNING : call only once !!! (second call will always return false !)
 */
bool isNewRelease()
{
    bool bNewRelease = false;

    KConfig* config = KGlobal::config(); // open kdesktoprc
    config->setGroup("Version");
    int versionMajor = config->readNumEntry("KDEVersionMajor", 0);
    int versionMinor = config->readNumEntry("KDEVersionMinor", 0);
    int versionRelease = config->readNumEntry("KDEVersionRelease", 0);

    if( versionMajor < KDE_VERSION_MAJOR )
        bNewRelease = true;
    else if( versionMinor < KDE_VERSION_MINOR )
             bNewRelease = true;
         else if( versionRelease < KDE_VERSION_RELEASE )
                  bNewRelease = true;

    if( bNewRelease ) {
      config->writeEntry("KDEVersionMajor", KDE_VERSION_MAJOR );
      config->writeEntry("KDEVersionMinor", KDE_VERSION_MINOR );
      config->writeEntry("KDEVersionRelease", KDE_VERSION_RELEASE );
      config->sync();
    }

    return bNewRelease;
}

/**
 * Create, if necessary, some directories in user's .kde/,
 * copy default .directory files there, as well as templates files.
 * Called by kdesktop on startup.
 */
void testLocalInstallation()
{
    bool newRelease = isNewRelease();

    QString desktopPath = KGlobalSettings::desktopPath();
    if (kdesktop_screen_number != 0) {
	QString dn = "Desktop";
	dn += QString::number(kdesktop_screen_number);
	desktopPath.replace(QRegExp("Desktop"), dn);
    }

    bool emptyDesktop = testDir( desktopPath );

    // Do not force copying that one (it would lose the icon positions)
    copyDirectoryFile("directory.desktop", desktopPath, false);

    QString trashPath = KGlobalSettings::trashPath();
    if (kdesktop_screen_number != 0) {
	QString dn = "Desktop";
	dn += QString::number(kdesktop_screen_number);
	trashPath.replace(QRegExp("Desktop"), dn);
    }

    testDir( trashPath );
    copyDirectoryFile("directory.trash", trashPath, newRelease); // for trash icon

    testDir( KGlobalSettings::autostartPath() );
    // we force the copying in case of a new release, to install new translations....
    copyDirectoryFile("directory.autostart", KGlobalSettings::autostartPath(), newRelease);

    if (emptyDesktop)
	copyDesktopLinks();
}

