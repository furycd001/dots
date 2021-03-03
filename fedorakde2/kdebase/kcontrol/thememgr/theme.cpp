/*
 * theme.h
 *
 * Copyright (c) 1998 Stefan Taferner <taferner@kde.org>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
 
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qfileinfo.h>
#include <kapp.h>
#include <qdir.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <qwindowdefs.h>
#include <qimage.h>
#include <dcopclient.h>

#include <kapp.h>
#include <kconfigbackend.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kdesktopfile.h>
#include <kicontheme.h>
#include <kipc.h>
#include <kdebug.h>
#include "theme.h"

#include <kio/netaccess.h>

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

extern int dropError(Display *, XErrorEvent *);

//-----------------------------------------------------------------------------
Theme::Theme()
{
  int len;

  instOverwrite = false;
  mConfig = 0;
  mConfigDir = KGlobal::dirs()->saveLocation("config");
  len = mConfigDir.length();
  if (len > 0 && mConfigDir[len-1] != '/') mConfigDir += '/';
  mKwmCount = 0;

  mMappings = NULL;
  mValid = false;
  loadMappings();

  loadSettings();
}


//-----------------------------------------------------------------------------
Theme::~Theme()
{
  saveSettings();
  if (mMappings) delete mMappings;
}


//-----------------------------------------------------------------------------
void Theme::loadSettings(void)
{
  KConfig* cfg = kapp->config();

  cfg->setGroup("Install");
  mRestartCmd = cfg->readEntry("restart-cmd",
			       "kill `pidof %s`; %s >/dev/null 2>&1 &");
}


//-----------------------------------------------------------------------------
void Theme::saveSettings(void)
{
}

//-----------------------------------------------------------------------------
const QString Theme::workDir(void)
{
  static QString *str = 0;
  if (!str)
    str = new QString(locateLocal("data", "kthememgr/Work/"));
  return *str;
}

const QString Theme::baseDir()
{
  static QString *str = 0;
  if (!str)
  {
     str = new QString(KGlobal::dirs()->saveLocation("config"));
     str->truncate(str->length()-7);
  }
  return *str;
}

bool Theme::checkExtension(const QString &file)
{
  return ((file.right(4) == ".tgz") ||
	  (file.right(4) == ".zip") ||
          (file.right(7) == ".tar.gz") ||
          (file.right(7) == ".ktheme"));
}

QString Theme::removeExtension(const QString &file)
{
  QString result = file;
  if (file.right(4) == ".tgz")
     result.truncate(file.length()-4);
  else if (file.right(4) == ".zip")
     result.truncate(file.length()-4);
  else if (file.right(7) == ".tar.gz")
     result.truncate(file.length()-7);
  else if (file.right(7) == ".ktheme")
     result.truncate(file.length()-7);

  return result;
}

QString Theme::defaultExtension()
{
   return QString::fromLatin1(".ktheme");
}

QString Theme::allExtensions()
{
   return i18n("*.tgz *.zip *.tar.gz *.ktheme|Theme files");
}

//-----------------------------------------------------------------------------
void Theme::loadMappings()
{
  QFile file;

  file.setName(locate("data", "kthememgr/theme.mappings"));
  if (!file.exists())
  {
     kdFatal() << "Mappings file theme.mappings not found." << endl;
  }

  if (mMappings) delete mMappings;
  mMappings = new KSimpleConfig(file.name(), true);
}


//-----------------------------------------------------------------------------
void Theme::cleanupWorkDir(void)
{
  QString cmd;
  int rc;

  // cleanup work directory
  cmd = QString::fromLatin1( "rm -rf %1*" ).arg( workDir() );
  rc = system(cmd.local8Bit());
  if (rc) kdWarning() << "Error during cleanup of work directory: rc=" << rc << " " << cmd << endl;
}

void Theme::findThemerc(const QString &path, const QStringList &list)
{
  for(QStringList::ConstIterator it = list.begin();
      it != list.end(); ++it)
  {
     QString filename = (*it).lower();
     if (filename.right(8) == ".themerc")
     {
        mThemeType = Theme_KDE;
        mThemercFile = path + *it;
        break;
     }
     if (filename.right(6) == ".theme")
     {
        mThemeType = Theme_Windows;
        mThemercFile = path + *it;
        break;
     }
  }
}

//-----------------------------------------------------------------------------
bool Theme::load(const QString &aPath, QString &error)
{
  QString cmd, str;
  QFileInfo finfo(aPath);
  int rc, num, i;

  assert(!aPath.isEmpty());

  delete mConfig; mConfig = 0;
  cleanupWorkDir();
  mValid = false;

  mFileName = aPath;
  i = mFileName.findRev('/');
  if (i >= 0)
     mFileName = mFileName.mid(i+1);

  if (finfo.isDir())
  {
    // The theme given is a directory. Copy files over into work dir.

    i = aPath.findRev('/');
    if (i >= 0) str = workDir() + aPath.mid(i);
    else str = workDir();

    cmd = QString("cp -r \"%1\" \"%2\"").arg(aPath).arg(str);
    kdDebug() << cmd << endl;
    rc = system(QFile::encodeName(cmd).data());
    if (rc)
    {
      error = i18n("Theme contents could not be copied from\n%1\ninto\n%2")
		.arg(aPath).arg(str);
      return false;
    }
  }
  else if (aPath.right(4) == ".zip")
  {
    // The theme given is a zip archive. Unpack the archive.
    cmd = QString("cd \"%1\"; unzip -qq \"%2\"")
             .arg(workDir()).arg(aPath);
    kdDebug() << cmd << endl;
    rc = system(QFile::encodeName(cmd).data());
    if (rc)
    {
      error = i18n("Theme contents could not be extracted from\n%1\ninto\n%2")
                .arg(aPath).arg(workDir());
      return false;
    }
  }
  else
  {
    // The theme given is a tar package. Unpack theme package.
    cmd = QString("cd \"%1\"; gzip -c -d \"%2\" | tar xf -")
             .arg(workDir()).arg(aPath);
    kdDebug() << cmd << endl;
    rc = system(QFile::encodeName(cmd).data());
    if (rc)
    {
      error = i18n("Theme contents could not be extracted from\n%1\ninto\n%2")
                .arg(aPath).arg(workDir());
      return false;
    }
  }

  // Let's see if the theme is stored in a subdirectory.
  mThemercFile = QString::null;
  mThemePath = workDir();
  QDir dir(mThemePath, QString::null, QDir::Name, QDir::Files|QDir::Dirs);
  QStringList list = dir.entryList();
  findThemerc(mThemePath, list);

  if (mThemercFile.isEmpty())
  {
    bool hasDir = false;
    for(QStringList::ConstIterator it = list.begin();
         it != list.end(); ++it)
    {
       if ((*it)[0] == '.') continue;
       finfo.setFile(mThemePath + *it);
       if (finfo.isDir())
       {
          mThemePath += *it + '/';
          hasDir = true;
          break;
       }
    }

    if (hasDir)
    {
       dir.setPath(mThemePath);
       list = dir.entryList();
       findThemerc(mThemePath, list);
    }
  }
  mFileList = list;

  if (mThemercFile.isEmpty())
  {
    error = i18n("Theme does not contain a .themerc nor a .theme file.");
    return false;
  }

  // Search for the preview image
  dir.setNameFilter("*.preview.*");
  mPreviewFile = dir[0];
  mPreviewFile = mThemePath+mPreviewFile;

  if (mThemeType == Theme_Windows)
  {
    // Convert '\' to '/'. 
    // KSimpleConfig uses '\' for escaping, so we have to do that here!
    QFile file1(mThemercFile);
    QFile file2(mThemercFile+"_rc");
    if (file1.open(IO_ReadOnly) && file2.open(IO_WriteOnly))
    {
       mThemercFile += "_rc";
       QTextStream stream1(&file1);
       QTextStream stream2(&file2);
       QRegExp reg("\\\\");
       while(!stream1.atEnd())
       {
          QString line(stream1.readLine());
          line.replace(reg, "/");
          stream2 << line << endl;
       }
    }
  }

  // read theme config file
  mConfig = new KSimpleConfig(mThemercFile);
  loadGroupGeneral();

  mValid = true;
  emit changed();
  return true;
}


//-----------------------------------------------------------------------------
bool Theme::save(const QString &aPath)
{
  if (!mValid) return false;

  emit apply();

  mConfig->sync();

  QString path = aPath;
  if (!checkExtension(path))
  {
    path += defaultExtension();
  }

  QString cmd = QString("cd \"%1\";tar cf - *|gzip -c >\"%2\"").
		arg(workDir()).arg(path);

  kdDebug() << cmd << endl;
  int rc = system(QFile::encodeName(cmd).data());
  if (rc) kdDebug() << "Failed to save theme to " << aPath << " with command " << cmd << endl;

  return (rc==0);
}


//-----------------------------------------------------------------------------
void Theme::removeFile(const QString& aName, const QString &aDirName)
{
  if (aName.isEmpty()) return;

  if (aName[0] == '/' || aDirName.isEmpty())
    QFile::remove( aName );
  else if (aDirName[aDirName.length()-1] == '/')
    QFile::remove( aDirName + aName );
  else QFile::remove( (aDirName + '/' + aName) );
}

QString Theme::findFile(const QString &aSrc)
{
  QString src = mThemePath + aSrc;
  QFileInfo finfo(src);
  if (!finfo.exists())
  {
    kdDebug() << "File " << src << " not found." << endl;
    src = aSrc;
    int i = src.findRev('/');
    if (i == -1)
    {
       kdDebug() << "File " << aSrc << " is not in theme package." << endl;
       return QString::null;
    }
    src = src.mid(i+1).lower();
    for(QStringList::ConstIterator it = mFileList.begin();
        it != mFileList.end(); ++it)
    {
       if (src == (*it).lower())
       {
          src = mThemePath + *it;
          finfo.setFile(src);
          break;
       }
    }
    if (!finfo.exists())
    {
       kdDebug() << "File " << aSrc << " is not in theme package." << endl;
       return QString::null;
    }
  }

  if (finfo.isDir())
  {
    kdDebug() << src << " is a direcotry instead of a file." << endl;
    return QString::null;
  }
  return src;
}


//-----------------------------------------------------------------------------
bool Theme::installFile(const QString& aSrc, const QString& aDest)
{
  QString cmd, dest;
  QString src(aSrc);
  int len, i;
  char buf[32768];
  bool backupMade = false;

  if (src.isEmpty()) return true;

  assert(aDest[0] == '/');
  dest = aDest;

  src = findFile(src);
  if (src.isEmpty())
     return false;


  QFileInfo finfo(dest);
  if (finfo.isDir())  // destination is a directory
  {
    len = dest.length();
    if (dest[len-1]=='/') dest[len-1] = '\0';
    i = src.findRev('/');
    dest = dest + '/' + src.mid(i+1);
    finfo.setFile(dest);
  }

  if (!instOverwrite && finfo.exists()) // make backup copy
  {
    QFile::remove( dest+'~' );
    rename(dest.local8Bit(), (dest+'~').local8Bit());
    backupMade = true;
  }

  QFile srcFile(src);
  if (!srcFile.open(IO_ReadOnly))
  {
    kdWarning() << "Cannot open file " << src << " for reading" << endl;
    return false;
  }

  QFile destFile(dest);
  if (!destFile.open(IO_WriteOnly))
  {
    kdWarning() << "Cannot open file " << dest << " for writing" << endl;
    return false;
  }

  while (!srcFile.atEnd())
  {
    len = srcFile.readBlock(buf, 32768);
    if (len <= 0) break;
    if (destFile.writeBlock(buf, len) != len)
    {
      kdWarning() <<"Write error to " << dest << ": " << strerror(errno) << endl;
      return false;
    }
  }

  srcFile.close();
  destFile.close();

  addInstFile(dest);
//  kdDebug() << "Installed " << src << " to " << dest << ". Backup: backupMade" << endl;

  return true;
}

//-----------------------------------------------------------------------------
bool Theme::installDirectory(const QString& aSrc, const QString& aDest)
{
  bool backupMade = false; // Not used ??

  if (aSrc.isEmpty()) return true;

  assert(aDest[0] == '/');
  QString dest = aDest;

  QString src = mThemePath + aSrc;

  QFileInfo finfo(src);
  if (!finfo.exists())
  {
    kdDebug() << "Directory " << aSrc << " is not in theme package." << endl;
    return false;
  }
  if (!finfo.isDir())
  {
    kdDebug() << aSrc << " is not a directory." << endl;
    return false;
  }

  if (finfo.exists()) // delete and/or make backup copy
  {
    if (instOverwrite)
    {
       KURL url;
       url.setPath(dest);
       KIO::NetAccess::del(url);
    }
    else
    {
       KURL url;
       url.setPath(dest+'~');
       KIO::NetAccess::del(url);
       rename(QFile::encodeName(dest).data(), QFile::encodeName(dest+'~').data());
       backupMade = true;
    }
  }

  KURL url1;
  KURL url2;
  url1.setPath(src);
  url2.setPath(dest);

  KIO::NetAccess::dircopy(url1, url2);

  addInstFile(dest);
//  kdDebug() << "Installed " << src << " to " << dest << ". Backup: backupMade" << endl;

  return true;
}


//-----------------------------------------------------------------------------
int Theme::installGroup(const char* aGroupName)
{
  QString value, oldValue, cfgFile, cfgGroup, appDir, group;
  QString oldCfgFile, key, cfgKey, cfgValue, themeValue, instCmd;
  QString preInstCmd;
  bool absPath = false;
  KSimpleConfig* cfg = NULL;
  int len, i, installed = 0;
  const char* missing = 0;

  group = aGroupName;
  if (mThemeType == Theme_Windows)
  {
    if (group == "Colors")
      group = "Control Panel/Colors";
    else if (group == "Display")
      group = "Control Panel/Desktop";
    else if (group == "Sounds")
      group = "AppEvents/Schemes/Apps/.Default/Minimize/.Current";
  }
  mConfig->setGroup(group);

  if (!instOverwrite) uninstallFiles(aGroupName);
  else readInstFileList(aGroupName);

  while (!group.isEmpty())
  {
    mMappings->setGroup(group);

    // Read config settings
    value = mMappings->readEntry("ConfigThemeGroup");
    if (!value.isEmpty())
    {
      mConfig->setGroup(value);
    }

    value = mMappings->readEntry("ConfigFile");
    if (!value.isEmpty())
    {
      cfgFile = value;
      if (cfgFile == "KDERC") cfgFile = QDir::homeDirPath() + "/.kderc";
      else if (cfgFile[0] != '/') cfgFile = mConfigDir + cfgFile;
    }
    value = mMappings->readEntry("ConfigGroup");
    if (!value.isEmpty()) cfgGroup = value;
    value = mMappings->readEntry("ConfigAppDir");
    if (!value.isEmpty() && (value[0] != '/'))
    {
      appDir = value;
      appDir = baseDir() + appDir;

      len = appDir.length();
      if (len > 0 && appDir[len-1]!='/') appDir += '/';
    }
    absPath = mMappings->readBoolEntry("ConfigAbsolutePaths", absPath);
    QString emptyValue = mMappings->readEntry("ConfigEmpty");

    value = mMappings->readEntry("ConfigActivateCmd");
    if (!value.isEmpty() && (mCmdList.findIndex(value) < 0))
      mCmdList.append(value);
    instCmd = mMappings->readEntry("ConfigInstallCmd", instCmd).stripWhiteSpace();
    preInstCmd = mMappings->readEntry("ConfigPreInstallCmd", preInstCmd).stripWhiteSpace();

    // Some checks
    if (cfgFile.isEmpty()) missing = "ConfigFile";
    if (cfgGroup.isEmpty()) missing = "ConfigGroup";
    if (missing)
    {
      kdWarning() << "Internal error in theme mappings (file theme.mappings) in group " << group << ":" << endl
		   << "Entry `" << missing << "' is missing or has no value." << endl;
      break;
    }

    // Open config file and sync/close old one
    if (oldCfgFile != cfgFile)
    {
      if (cfg)
      {
	cfg->sync();
	delete cfg;
      }
      cfg = new KSimpleConfig(cfgFile);
      oldCfgFile = cfgFile;
    }

    // Set group in config file
    cfg->setGroup(cfgGroup);

    // Execute pre-install command (if given)
    if (!preInstCmd.isEmpty()) preInstallCmd(cfg, preInstCmd);

    // Process all mapping entries for the group
    QMap<QString, QString> aMap = mMappings->entryMap(group);
    QMap<QString, QString>::Iterator aIt(aMap.begin());

    for (; aIt != aMap.end(); ++aIt) {
      key = aIt.key();
      if ( key.left(6).lower() == "Config" ) continue;
      value = (*aIt).stripWhiteSpace();
      len = value.length();
      bool bInstallFile = false;
      bool bInstallDir = false;
      if (len>0 && value[len-1]=='!')
      {
	value.truncate(len - 1);
      }
      else if (len>0 && value[len-1]=='*')
      {
	value.truncate(len - 1);
        bInstallDir = true;
      }
      else
      {
        bInstallFile = true;
      }

      // parse mapping
      i = value.find(':');
      if (i >= 0)
      {
	cfgKey = value.left(i);
	cfgValue = value.mid(i+1);
      }
      else
      {
	cfgKey = value;
	cfgValue = QString::null;
      }
      if (cfgKey.isEmpty()) cfgKey = key;

      if (bInstallFile || bInstallDir)
      {
	oldValue = cfg->readEntry(cfgKey);
	if (!oldValue.isEmpty() && oldValue==emptyValue)
	  oldValue = QString::null;
      }
      else
      {
         oldValue = QString::null;
      }

      themeValue = mConfig->readEntry(key);
      if (group.left(20) == "Control Panel/Colors")
      {
        themeValue.replace(QRegExp("\\s"), QString::fromLatin1(","));
      }
      else if (group.left(21) == "Control Panel/Desktop")
      {
 	if (key == "WallpaperStyle")
        {
	  themeValue = "Scaled";
        }
      }
      if (bInstallFile && (mThemeType == Theme_Windows))
      {
         themeValue.replace(QRegExp("%.+%"), QString::null);
      }

      if (cfgValue.isEmpty()) cfgValue = themeValue;

      // Install file
      if (bInstallFile)
      {
        // Strip leading path
        i = cfgValue.findRev('/');
        if (i != -1)
           cfgValue = cfgValue.mid(i+1);
	if (!themeValue.isEmpty())
	{
          KStandardDirs::makeDir(appDir);
	  if (installFile(themeValue, appDir + cfgValue))
	    installed++;
	  else bInstallFile = false;
	}
      }
      // Install dir
      if (bInstallDir)
      {
	if (!themeValue.isEmpty())
	{
          KStandardDirs::makeDir(appDir);
	  if (installDirectory(themeValue, appDir + cfgValue))
	    installed++;
	  else bInstallDir = false;
	}
      }

      bool bDeleteKey = false;
      // Determine config value
      if (cfgValue.isEmpty())
      {
         cfgValue = emptyValue;
         if (cfgValue.isEmpty())
           bDeleteKey = true;
      }
      else if ((bInstallFile || bInstallDir) && absPath)
         cfgValue = appDir + cfgValue;

      // Set config entry
      if (bDeleteKey)
         cfg->deleteEntry(cfgKey, false);
      else
         cfg->writeEntry(cfgKey, cfgValue);
    }

    if (!instCmd.isEmpty())
       installCmd(cfg, instCmd, installed);
    group = mMappings->readEntry("ConfigNextGroup");
  }

  if (cfg)
  {
    cfg->sync();
    delete cfg;
  }

  writeInstFileList(aGroupName);
  return installed;
}


//-----------------------------------------------------------------------------
void Theme::preInstallCmd(KSimpleConfig* aCfg, const QString& aCmd)
{
  QString grp = aCfg->group();
  QString value, cmd;

  cmd = aCmd.stripWhiteSpace();

  if (cmd == "stretchBorders")
  {
    value = mConfig->readEntry("ShapePixmapBottom");
    if (!value.isEmpty()) stretchPixmap(mThemePath + value, false);
    value = mConfig->readEntry("ShapePixmapTop");
    if (!value.isEmpty()) stretchPixmap(mThemePath + value, false);
    value = mConfig->readEntry("ShapePixmapLeft");
    if (!value.isEmpty()) stretchPixmap(mThemePath + value, true);
    value = mConfig->readEntry("ShapePixmapRight");
    if (!value.isEmpty()) stretchPixmap(mThemePath + value, true);
  }
  else
  {
    kdWarning() << "Unknown pre-install command `" << aCmd << "' in theme.mappings file in group "
	    << mMappings->group() << endl;
  }
}

static void cleanKWMPixmapEntry(KSimpleConfig *aCfg, const char *entry)
{
  QString pixmap1 = QString::fromLatin1(entry) + ".png";
  QString pixmap2 = aCfg->readEntry(entry);
  if (pixmap2 != pixmap1)
  {
     QString file = locateLocal("data", "kwin/pics/"+pixmap1);
     ::unlink(QFile::encodeName(file));
  }
}

static int countKWMPixmapEntry(KSimpleConfig *aCfg, const char *entry)
{
  return aCfg->readEntry(entry).isEmpty() ? 0 : 1;
   
}

//-----------------------------------------------------------------------------
void Theme::installCmd(KSimpleConfig* aCfg, const QString& aCmd,
		       int &aInstalled)
{
  QString grp = aCfg->group();
  QString cmd = aCmd.stripWhiteSpace();

  if (cmd == "setColorScheme")
  {
    updateColorScheme(aCfg);
  }
  else if (cmd == "setWallpaperMode")
  {
    QString value = aCfg->readEntry("wallpaper",QString::null);
    aCfg->writeEntry("UseWallpaper", !value.isEmpty());
  }
  else if (cmd == "oneDesktopMode")
  {
    bool flag = (aInstalled == 1);
    bool value = aCfg->readBoolEntry("CommonDesktop", false);
    if (flag || value)
      aCfg->writeEntry("CommonDesktop", true);
    if (flag) 
      aCfg->writeEntry("DeskNum", 0);
  }
  else if (cmd == "setSound")
  {
    bool flag = (aInstalled > 0);
    aInstalled = 0;
    int presentation = aCfg->readNumEntry("presentation",0);
    if (flag)
       aCfg->writeEntry("presentation", presentation | 1);
    else
       aCfg->writeEntry("presentation", presentation & ~1);
  }
  else if (cmd == "setKWM")
  {
    mKwmCount = 0;
    mKwmCount += countKWMPixmapEntry(aCfg, "wm_top");
    mKwmCount += countKWMPixmapEntry(aCfg, "wm_bottom");
    mKwmCount += countKWMPixmapEntry(aCfg, "wm_left");
    mKwmCount += countKWMPixmapEntry(aCfg, "wm_right");
    mKwmCount += countKWMPixmapEntry(aCfg, "wm_topleft");
    mKwmCount += countKWMPixmapEntry(aCfg, "wm_topright");
    mKwmCount += countKWMPixmapEntry(aCfg, "wm_bottomleft");
    mKwmCount += countKWMPixmapEntry(aCfg, "wm_bottomright");
  }
  else if (cmd == "setKWM2")
  {
    // Clean up spurious theme pixmaps
    cleanKWMPixmapEntry(aCfg, "menu");
    cleanKWMPixmapEntry(aCfg, "iconify");
    cleanKWMPixmapEntry(aCfg, "maximize");
    cleanKWMPixmapEntry(aCfg, "maximizedown");
    cleanKWMPixmapEntry(aCfg, "close");
    cleanKWMPixmapEntry(aCfg, "pinup");
    cleanKWMPixmapEntry(aCfg, "pindown");
  }
  else if (cmd == "setKWM3")
  {
    QString plugin = aCfg->readEntry("PluginLib");
    kdDebug() << "KWin Plugin: " << plugin << endl;
    if ((mKwmCount == 8) && (plugin.isEmpty()))
       aCfg->writeEntry("PluginLib", "libkwinkwmtheme");
    else if (!plugin.isEmpty())  //an installed kwin client
       aCfg->writeEntry("PluginLib", plugin);
  }
  else if (cmd == "setKicker")
  {
    QString value = aCfg->readEntry("BackgroundTheme",QString::null);
    aCfg->writeEntry("UseBackgroundTheme", !value.isEmpty());
    if (!value.isEmpty())
        aCfg->writeEntry("RotateBackground", true);
  }
  else
  {
    kdWarning() << "Unknown command `" << aCmd << "' in theme.mappings "
		 << "file in group " << mMappings->group() << endl;
  }

  if ( aCfg->group() != grp )
    aCfg->setGroup(grp);
}

void Theme::applyIcons()
{
  QString theme = KIconTheme::current();

  KIconTheme icontheme(theme);

  const char *groups[] = { "Desktop", "Toolbar", "MainToolbar", "Small", 0L };

  KSimpleConfig *config = new KSimpleConfig("kdeglobals", false);
  for (int i=0; i<KIcon::LastGroup; i++)
  {
    if (groups[i] == 0L)
      break;
    config->setGroup(QString::fromLatin1(groups[i]) + "Icons");
    config->writeEntry("Size", icontheme.defaultSize(i));
  }
  delete config;

  for (int i=0; i<KIcon::LastGroup; i++)
  {
    KIPC::sendMessageAll(KIPC::IconChanged, i);
  }
}


//-----------------------------------------------------------------------------
void Theme::doCmdList(void)
{
  QString cmd, str, appName;
  //  int rc;
  for (QStringList::ConstIterator it = mCmdList.begin();
       it != mCmdList.end();
       ++it)
  {
    cmd = *it;
    kdDebug() << "do command: " << cmd << endl;
    if (cmd.startsWith("kfmclient"))
    {
      system(cmd.local8Bit());
    }
    else if (cmd == "applyColors")
    {
      runKrdb();
      colorSchemeApply();
    }
    else if (cmd == "applyWallpaper")
    {
       // reconfigure kdesktop. kdesktop will notify all clients
       DCOPClient *client = kapp->dcopClient();
       if (!client->isAttached())
          client->attach();
       client->send("kdesktop", "KBackgroundIface", "configure()", "");
    }
    else if (cmd == "applyIcons")
    {
       applyIcons();
    }
    else if (cmd == "applySound")
    {
       // reconfigure knotify
       DCOPClient *client = kapp->dcopClient();
       if (!client->isAttached())
          client->attach();
       client->send("knotify", "", "reconfigure()", "");
    }
    else if (cmd == "applyKWM")
    {
       // Clean up spurious theme pixmaps
       kapp->config()->reparseConfiguration();
       // reconfigure kwin
       DCOPClient *client = kapp->dcopClient();
       if (!client->isAttached())
          client->attach();
       client->send("kwin", "", "reconfigure()", "");
    }
    else if (cmd == "applyKicker")
    {
       // reconfigure kicker
       DCOPClient *client = kapp->dcopClient();
       if (!client->isAttached())
          client->attach();
       client->send("kicker", "", "configure()", "");
    }
    else if (cmd.startsWith("restart"))
    {
      appName = cmd.mid(7).stripWhiteSpace();
      str = i18n("Restart %1 to activate the new settings?").arg( appName );
      if (KMessageBox::questionYesNo(0, str) == KMessageBox::Yes) {
          str.sprintf(mRestartCmd.local8Bit().data(), appName.local8Bit().data(),
                      appName.local8Bit().data());
          system(str.local8Bit());
      }
    }
  }

  mCmdList.clear();
}


//-----------------------------------------------------------------------------
bool Theme::backupFile(const QString &fname) const
{
  QFileInfo fi(fname);
  QString cmd;
  int rc;

  if (!fi.exists()) return false;

  QFile::remove((fname + '~'));
  cmd.sprintf("mv \"%s\" \"%s~\"", fname.local8Bit().data(),
	      fname.local8Bit().data());
  rc = system(cmd.local8Bit());
  if (rc) kdWarning() << "Cannot make backup copy of "
          << fname << ": mv returned " << rc << endl;
  return (rc==0);
}

//-----------------------------------------------------------------------------
void Theme::addInstFile(const QString &aFileName)
{
  if (!aFileName.isEmpty() && (mInstFiles.findIndex(aFileName) < 0))
    mInstFiles.append(aFileName);
}


//-----------------------------------------------------------------------------
void Theme::readInstFileList(const char* aGroupName)
{
  KConfig* cfg = kapp->config();

  assert(aGroupName!=0);
  cfg->setGroup("Installed Files");
  mInstFiles = cfg->readListEntry(aGroupName, ':');
}


//-----------------------------------------------------------------------------
void Theme::writeInstFileList(const char* aGroupName)
{
  KConfig* cfg = kapp->config();

  assert(aGroupName!=0);
  cfg->setGroup("Installed Files");
  cfg->writeEntry(aGroupName, mInstFiles, ':');
}


//-----------------------------------------------------------------------------
void Theme::uninstallFiles(const char* aGroupName)
{
  QString cmd, fname, value;
  QFileInfo finfo;
  bool reverted = false;
  int processed = 0;

  readInstFileList(aGroupName);
  for (QStringList::ConstIterator it=mInstFiles.begin();
       it != mInstFiles.end();
       ++it)
  {
    fname = *it;
    reverted = false;
    if (unlink(QFile::encodeName(fname).data())==0)
       reverted = true;
    finfo.setFile(fname+'~');
    if (finfo.exists())
    {
      if (rename(QFile::encodeName(fname+'~').data(), QFile::encodeName(fname).data()))
	kdWarning() << "Failed to rename " << fname << " to "
        << fname << "~:" << strerror(errno) << endl;
      else reverted = true;
    }

    if (reverted)
    {
      processed++;
    }
  }
  mInstFiles.clear();
  writeInstFileList(aGroupName);
}


//-----------------------------------------------------------------------------
void Theme::install(void)
{
  if (!mValid) return;

  loadMappings();
  mCmdList.clear();

  if (instWallpapers) installGroup("Display");
  if (instSounds) installGroup("Sounds");
  if (instIcons) installGroup("Icons");

  // Colors & WM are installed behind each other to get a smoother update.
  if (instColors) installGroup("Colors"); 
  if (instWM) 
  { 
     installGroup("Window Border");
     installGroup("Window Titlebar");
  }
  if (instPanel) installGroup("Panel");

  doCmdList();

  saveSettings();
}


//-----------------------------------------------------------------------------
void Theme::readCurrent(void)
{
  emit changed();
}


//-----------------------------------------------------------------------------
KConfig* Theme::openConfig(const QString &aAppName) const
{
  return new KConfig(aAppName + "rc");
}


//-----------------------------------------------------------------------------
void Theme::loadGroupGeneral(void)
{
  QString fname;
  QColor col;
  col.setRgb(192,192,192);

  if (mThemeType == Theme_Windows) 
  {
    mName = mThemercFile;
    int i = mName.findRev('/');
    if (i != -1)
       mName = mName.mid(i+1);
    i = mName.findRev('.');
    if (i != -1)
       mName = mName.left(i);
    mDescription = mName + " Theme";
    mAuthor = "Divide by Zero's WinTheme patch imported";
    mEmail = "";
    mHomePage = "";
    mVersion = "";
  } 
  else 
  {
    mConfig->setGroup("General");
    mName = mConfig->readEntry("Name");
    if (mName.isEmpty())
       mName = mConfig->readEntry("name", "<unknown>");
    mDescription = mConfig->readEntry("Comment");
    if (mDescription.isEmpty())
       mDescription = mConfig->readEntry("description", i18n("%1 Theme").arg(mName));
    mAuthor = mConfig->readEntry("author");
    mEmail = mConfig->readEntry("email");
    mHomePage = mConfig->readEntry("homepage");
    mVersion = mConfig->readEntry("version");
  }
  mPreview.resize(0,0);
  if (!mPreviewFile.isEmpty())
  {
    mPreview.load(mPreviewFile);
  }
  if (mPreview.isNull())
  {
     if (mThemeType == Theme_Windows) 
     {
        mConfig->setGroup("Control Panel/Desktop");
        mPreviewFile = mConfig->readEntry("Wallpaper");
        mPreviewFile.replace(QRegExp("%.+%"), QString::null);
        mPreviewFile = findFile(mPreviewFile);
        if (!mPreviewFile.isEmpty())
        { 
           QImage img(mPreviewFile);
           if (!img.isNull())
           {
              mPreview = img.smoothScale(320,240);
           }
        }
     }
  }
}

//-----------------------------------------------------------------------------
bool Theme::hasGroup(const QString& aName, bool aNotEmpty)
{
  bool found;
  QString gName;
  
  if (mThemeType == Theme_Windows) 
  {
    if (aName == "Colors")
	gName = "Control Panel/Colors";
    else if (aName == "Display")
	gName = "Control Panel/Desktop";
    else if (aName == "Sounds")
        gName = "AppEvents/Schemes/Apps/.Default/Minimize/.Current";
  } else
    gName = aName;
  found = mConfig->hasGroup(gName);

  if (!aNotEmpty)
    return found;

  QMap<QString, QString> aMap = mConfig->entryMap(gName);
  if (found && aNotEmpty)
    found = !aMap.isEmpty();

  return found;
}

//-----------------------------------------------------------------------------
void Theme::stretchPixmap(const QString &aFname, bool aStretchVert)
{
  QPixmap src, dest;
  QBitmap *srcMask, *destMask;
  int w, h, w2, h2;
  QPainter p;

  src.load(aFname);
  if (src.isNull()) return;

  w = src.width();
  h = src.height();

  if (aStretchVert)
  {
    w2 = w;
    for (h2=h; h2<64; h2=h2<<1)
      ;
  }
  else
  {
    h2 = h;
    for (w2=w; w2<64; w2=w2<<1)
      ;
  }

  dest = src;
  dest.resize(w2, h2);

  p.begin(&dest);
  p.drawTiledPixmap(0, 0, w2, h2, src);
  p.end();

  srcMask = (QBitmap*)src.mask();
  if (srcMask)
  {
    destMask = (QBitmap*)dest.mask();
    p.begin(destMask);
    p.drawTiledPixmap(0, 0, w2, h2, *srcMask);
    p.end();
  }

  dest.save(aFname, QPixmap::imageFormat(aFname));
}

//-----------------------------------------------------------------------------
void Theme::runKrdb(void) const
{
  KSimpleConfig cfg("kcmdisplayrc", true);

  cfg.setGroup("X11");
  if (cfg.readBoolEntry("useResourceManager", true))
    system("krdb2");
}


//-----------------------------------------------------------------------------
void Theme::colorSchemeApply(void)
{
  KIPC::sendMessageAll(KIPC::PaletteChanged);
}


void Theme::updateColorScheme(KSimpleConfig *config)
{
    // define some KDE2 default colors
    QColor kde2Blue;
    if (QPixmap::defaultDepth() > 8)
      kde2Blue.setRgb(10, 95, 137);
    else
      kde2Blue.setRgb(0, 0, 192);

    QColor widget(220, 220, 220);

    QColor button;
    if (QPixmap::defaultDepth() > 8)
      button.setRgb(228, 228, 228);
    else
      button.setRgb(220, 220, 220);

    QColor link(0, 0, 192);
    QColor visitedLink(128, 0,128);

    // Current scheme
    config->setGroup("General");

    bool isDefault = config->readEntry( "background").isEmpty(); 
    if (isDefault)
    {
       QString sCurrentScheme = locateLocal("data", "kdisplay2/color-schemes/thememgr.kcsrc");
       unlink(QFile::encodeName(sCurrentScheme));
       config->setGroup("KDE");
       config->writeEntry("colorScheme", "<default>", true, true);
       return;
    }

    // note: defaults should be the same as the KDE default
    QColor txt = config->readColorEntry( "foreground", &black );
    QColor back = config->readColorEntry( "background", &widget );
    QColor select = config->readColorEntry( "selectBackground", &kde2Blue );
    QColor selectTxt = config->readColorEntry( "selectForeground", &white );
    QColor window = config->readColorEntry( "windowBackground", &white );
    QColor windowTxt = config->readColorEntry( "windowForeground", &black );
    button = config->readColorEntry( "buttonBackground", &button );
    QColor buttonTxt = config->readColorEntry( "buttonForeground", &black );
    link = config->readColorEntry( "linkColor", &link );
    visitedLink = config->readColorEntry( "visitedLinkColor", &visitedLink );

    config->setGroup( "WM" );

    QColor iaTitle = config->readColorEntry("inactiveBackground", &widget);
    QColor iaTxt = config->readColorEntry("inactiveForeground", &black);
    QColor iaBlend = config->readColorEntry("inactiveBlend", &widget);
    QColor aTitle = config->readColorEntry("activeBackground", &kde2Blue);
    QColor aTxt = config->readColorEntry("activeForeground", &white);
    QColor aBlend = config->readColorEntry("activeBlend", &kde2Blue);
    // hack - this is all going away. For now just set all to button bg
    QColor aTitleBtn = config->readColorEntry("activeTitleBtnBg", &back);
    QColor iTitleBtn = config->readColorEntry("inactiveTitleBtnBg", &back);

    config->setGroup( "KDE" );
    int contrast = config->readNumEntry( "contrast", 7 );

    QString sCurrentScheme = locateLocal("data", "kdisplay2/color-schemes/thememgr.kcsrc");
    KSimpleConfig *colorScheme = new KSimpleConfig(sCurrentScheme );
    int i = sCurrentScheme.findRev('/');
    if (i >= 0)
      sCurrentScheme = sCurrentScheme.mid(i+1);
    config->setGroup("KDE");
    config->writeEntry("colorScheme", sCurrentScheme, true, true);

    colorScheme->setGroup("Color Scheme" );
    colorScheme->writeEntry("Name", i18n("Theme Colors"));
    colorScheme->writeEntry("background", back );
    colorScheme->writeEntry("selectBackground", select );
    colorScheme->writeEntry("foreground", txt );
    colorScheme->writeEntry("activeForeground", aTxt );
    colorScheme->writeEntry("inactiveBackground", iaTitle );
    colorScheme->writeEntry("inactiveBlend", iaBlend );
    colorScheme->writeEntry("activeBackground", aTitle );
    colorScheme->writeEntry("activeBlend", aBlend );
    colorScheme->writeEntry("inactiveForeground", iaTxt );
    colorScheme->writeEntry("windowForeground", windowTxt );
    colorScheme->writeEntry("windowBackground", window );
    colorScheme->writeEntry("selectForeground", selectTxt );
    colorScheme->writeEntry("contrast", contrast );
    colorScheme->writeEntry("buttonForeground", buttonTxt );
    colorScheme->writeEntry("buttonBackground", button );
    colorScheme->writeEntry("activeTitleBtnBg", aTitleBtn);
    colorScheme->writeEntry("inactiveTitleBtnBg", iTitleBtn);
    colorScheme->writeEntry("linkColor", link);
    colorScheme->writeEntry("visitedLinkColor", visitedLink);

    delete colorScheme;
}

//-----------------------------------------------------------------------------
const QString Theme::fileOf(const QString& aName) const
{
  int i = aName.findRev('/');
  if (i >= 0) return aName.mid(i+1);
  return aName;
}


//-----------------------------------------------------------------------------
const QString Theme::pathOf(const QString& aName) const
{
  int i = aName.findRev('/');
  if (i >= 0) return aName.left(i);
  return aName;
}


//-----------------------------------------------------------------------------
#include "theme.moc"
