/*
 * themecreator.cpp
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

#include "themecreator.h"
#include <kapp.h>
#include <klocale.h>
#include <kdebug.h>
#include <kconfigbackend.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kconfig.h>

#include <qstring.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qimage.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>


//-----------------------------------------------------------------------------
ThemeCreator::ThemeCreator(): ThemeCreatorInherited()
{
}


//-----------------------------------------------------------------------------
ThemeCreator::~ThemeCreator()
{
}


//-----------------------------------------------------------------------------
bool ThemeCreator::create(const QString aThemeName)
{
  if (aThemeName.isEmpty()) return false;

  kdDebug() << "Theme::create() started" << endl;

  delete mConfig; mConfig = 0;
  cleanupWorkDir();
  mValid = false;

  mThemePath = workDir() + aThemeName + '/';
  mFileName = aThemeName;
  if (!KStandardDirs::makeDir(mThemePath))
  {
    kdWarning() << "Failed to create directory " << mThemePath << ": " << strerror(errno) << endl;
    return false;
  }

  mThemercFile = mThemePath + aThemeName + ".themerc";
  mPreviewFile = QString::null;
  mPreview.resize(0,0);

  mConfig = new KSimpleConfig(mThemercFile);
  return true;
}

//-----------------------------------------------------------------------------
void ThemeCreator::savePreview(const QImage &image)
{
  image.save(mThemePath+fileName()+".preview.png", "PNG");
}


//-----------------------------------------------------------------------------
bool ThemeCreator::extract(void)
{
  kdDebug() << "Theme::extract() started" << endl;

  saveGroupGeneral();

  loadMappings();

  if (instWallpapers) extractGroup("Display");
  if (instColors) extractGroup("Colors");
  if (instSounds) extractGroup("Sounds");
  if (instWM) extractGroup("Window Border");

  kdDebug() << "Theme::extract() done" << endl;

  saveSettings();
  mValid = true;
  save(KGlobal::dirs()->saveLocation("themes") + fileName());

  return true;
}


//-----------------------------------------------------------------------------
int ThemeCreator::extractGroup(const char* aGroupName)
{
  QString value, cfgFile, cfgGroup, appDir, group, emptyValue, mapValue, str;
  QString oldCfgFile, key, cfgKey, cfgValue, themeValue, instCmd;
  bool absPath = false, doCopyFile;
  KSimpleConfig* cfg = 0;
  int len, i, extracted = 0;
  const char* missing = 0;

  kdDebug() << "*** beginning with " << aGroupName << endl;
  group = aGroupName;
  mConfig->setGroup(group);

  while (!group.isEmpty())
  {
    mMappings->setGroup(group);
    kdDebug() << "Mappings group: " << group << endl;

    // Read config settings
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
    if (!value.isEmpty())
    {
      appDir = baseDir() + value;
      len = appDir.length();
      if (len > 0 && appDir[len-1]!='/') appDir += '/';
    }
    absPath = mMappings->readBoolEntry("ConfigAbsolutePaths", absPath);
    value = mMappings->readEntry("ConfigEmpty");
    if (!value.isEmpty()) emptyValue = value;
    value = mMappings->readEntry("ConfigActivateCmd");
    if (!value.isEmpty() && (mCmdList.findIndex(value) < 0))
      mCmdList.append(value);

    instCmd = mMappings->readEntry("ConfigInstallCmd").stripWhiteSpace();

    // Some checks
    if (cfgFile.isEmpty()) missing = "ConfigFile";
    if (cfgGroup.isEmpty()) missing = "ConfigGroup";
    if (missing)
    {
      kdWarning() << "Internal error in theme mappings "
		   << "(file theme.mappings) in group " << group << ":" << endl
		   << "Entry `" << missing << "' is missing or has no value." << endl;
      break;
    }

    // Open config file and sync/close old one
    if (oldCfgFile != cfgFile)
    {
      if (cfg)
      {
	kdDebug() << "closing config file" << endl;
	cfg->sync();
	delete cfg;
      }
      kdDebug() << "opening config file " << cfgFile << endl;
      cfg = new KSimpleConfig(cfgFile);
      oldCfgFile = cfgFile;
    }

    // Set group in config file
    cfg->setGroup(cfgGroup);
    kdDebug() << cfgFile << ": " << cfgGroup << endl;
    // Process all mapping entries for the group

    QMap<QString, QString> aMap = mMappings->entryMap(group);
    QMap<QString, QString>::Iterator aIt(aMap.begin());
    for (; aIt != aMap.end(); ++aIt) {
      key = aIt.key();
      if (key.startsWith("Config")) continue;
      mapValue = (*aIt).stripWhiteSpace();
      len = mapValue.length();
      if (len>0 && mapValue[len-1]=='!')
      {
	doCopyFile = false;
	mapValue.truncate(len-1);
      }
      else doCopyFile = true;

      // parse mapping
      i = mapValue.find(':');
      if (i >= 0)
      {
	cfgKey = mapValue.left(i);
	cfgValue = mapValue.mid(i+1, 1024);
      }
      else
      {
	cfgKey = mapValue;
	cfgValue = QString::null;
      }
      if (cfgKey.isEmpty()) cfgKey = key;
      value = cfg->readEntry(cfgKey);

      if (doCopyFile)
      {
	if (!value.isEmpty())
	{
	  if (value[0] != '/') value = appDir + value;
	  str = extractFile(value);
	  if (!str.isEmpty())
	  {
	    extracted++;
	    value = str;
	  }
	  value = fileOf(value);
	}
      }

      // Set config entry
      if (value == emptyValue) value = "";
      kdDebug() << key << "=" << value << endl;
      if (value.isEmpty())
         mConfig->deleteEntry(key, false);
      else
         mConfig->writeEntry(key, value);
    }

    if (!instCmd.isEmpty()) extractCmd(cfg, instCmd, extracted);
    group = mMappings->readEntry("ConfigNextGroup");
  }

  if (cfg)
  {
    kdDebug() << "closing config file" << endl;
    cfg->sync();
    delete cfg;
  }

  kdDebug() << "*** done with " << aGroupName << endl;
  return extracted;
}


//-----------------------------------------------------------------------------
void ThemeCreator::extractIcons(void)
{
}


//-----------------------------------------------------------------------------
void ThemeCreator::extractCmd(KSimpleConfig* aCfg, const QString& aCmd,
			      int)
{
  QString grp = aCfg->group();
  QString value, cmd;

  cmd = aCmd.stripWhiteSpace();

  aCfg->setGroup(grp);
}


//-----------------------------------------------------------------------------
const QString ThemeCreator::extractFile(const QString& aFileName)
{
  QFileInfo finfo(aFileName);
  QFile srcFile, destFile;
  char buf[32768];
  QString fname, ext, str;
  int len, i;

  if (!finfo.exists() || !finfo.isFile())
  {
    kdDebug() << "File " << aFileName << " does not exist or is no file." << endl;
    return 0;
  }

  fname = fileOf(aFileName);
  while (1) // find a unique filename in the work directory
  {
    finfo.setFile(mThemePath + fname);
    kdDebug() << "Checking for " << mThemePath+fname << endl;
    if (!finfo.exists()) break;
    i = fname.findRev('.');
    if (i >= 0)
    {
      ext = fname.mid(i, 255);
      fname.truncate(i);
    }
    else ext = QString::null;
    int j;
    for (j=i-1; j>=0 && fname[j]>='0' && fname[j]<='9'; j--);

    int num = 0;
    j++;
    int h = j;
    while(j < i)
    {
       num = 10*num + (fname[j].latin1() - '0');
       j++;
    }

    num++;
    fname.truncate(h);
    fname = QString("%1%2%3").arg(fname).arg(num).arg(ext);
  }

  kdDebug() << "Extracting " << aFileName << " to " << mThemePath + fname << endl;

  srcFile.setName(aFileName);
  if (!srcFile.open(IO_ReadOnly))
  {
    kdWarning() << "Cannot open file " << aFileName << " for reading" << endl;
    return 0;
  }

  destFile.setName(mThemePath + fname);
  if (!destFile.open(IO_WriteOnly))
  {
    kdWarning() << "Cannot open file " << mThemePath << fname << " for writing" << endl;
    return 0;
  }

  while (!srcFile.atEnd())
  {
    len = srcFile.readBlock(buf, 32768);
    if (len <= 0) break;
    if (destFile.writeBlock(buf, len) != len)
    {
      kdWarning() << "Write error to " << mThemePath << fname
          << ": " << strerror(errno) << endl;
      return 0;
    }
  }

  srcFile.close();
  destFile.close();

  return fname;
}


//-----------------------------------------------------------------------------
void ThemeCreator::saveGroupGeneral(void)
{
  mConfig->setGroup("General");
  mConfig->writeEntry("name", mName);
  mConfig->writeEntry("author", mAuthor);
  mConfig->writeEntry("email", mEmail);
  mConfig->writeEntry("homepage", mHomePage);
  mConfig->writeEntry("version", mVersion);
}



