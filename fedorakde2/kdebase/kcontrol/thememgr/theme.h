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
#ifndef THEME_H
#define THEME_H

#include <qstring.h>
#include <ksimpleconfig.h>
#include <qpixmap.h>
#include <qstringlist.h>

class KConfig;

class Theme: public QObject
{
  Q_OBJECT
public:
  /** Construct a theme object */
  Theme();
  virtual ~Theme();

  /** Load theme and prepare it for installation or modification. Returns
   true on success. */
  virtual bool load(const QString &path, QString &error);
  
  bool isValid() { return mValid; }
  void setValid(bool valid) { mValid = valid; }

  /** Apply current theme to the users Kde configuration. This updates several
      config files and executes the initialization programs. */
  virtual void install(void);

  /** Write theme file. If no path is given the theme is stored in the default
      kde location with the given theme name. Returns true on success. */
  virtual bool save(const QString &path);

  /** Read current Kde configuration from several config files. */
  virtual void readCurrent(void);

  /** Returns the filename of the theme. */
  const QString fileName(void) { return mFileName; }

  /** Get preview pixmap. */
  const QPixmap& preview(void) const { return mPreview; }


  /** Theme packet installation options */
  bool instColors;
  bool instWallpapers;  
  bool instSounds;
  bool instIcons;
  bool instWM;
  bool instPanel;
  bool instOverwrite;

  /** Test if group with given name exists. If notEmpty is true the
      method also tests if the group is not empty. */
  virtual bool hasGroup(const QString& name, bool notEmpty=false);

  /** Working directory. */
  static const QString workDir(void);

  /** Base directory. */
  static const QString baseDir(void);
  
  /** returns true when the file has a supported extension **/
  static bool checkExtension(const QString &file);
  
  /** remove extension part from the file **/
  static QString removeExtension(const QString &file);
  
  /** default extension for the file **/
  static QString defaultExtension();

  /** filter expression for KFileDialog **/
  static QString allExtensions();

  /** Uninstall files of last theme installation for given group */
  virtual void uninstallFiles(const char* groupName);

  const QString themeName(void) const { return mName; } virtual
  void setThemeName(const QString &name) { mName = name; }
  const QString description(void) const { return mDescription; } virtual
  void setDescription(const QString&description) { mDescription = description; }
  QString author() const { return mAuthor; }
  void setAuthor(const QString &author) { mAuthor = author; }
  QString email() const { return mEmail; }
  void setEmail(const QString &email) { mEmail = email; }
  QString homepage() const { return mHomePage; }
  void setHomepage(const QString &homepage) { mHomePage = homepage; }
  QString version() const { return mVersion; }
  void setVersion(const QString &version) { mVersion = version; }

signals:
  /** This signal is emitted after import() or load() */
  void changed();

  /** This signal is emitted to ask for apply of all changes. */
  void apply();

protected:
  /** Read information from config file. */
  virtual void loadGroupGeneral(void);

  /** Create KConfig object and load fitting data. */
  virtual KConfig* openConfig(const QString &appName) const;

  /** Installs file by calling cp. The source name is prepended
      with the theme work directory, the dest name is prepended
      with KApp::localkdedir(). Returns true on success. */
  virtual bool installFile(const QString& name, const QString& dest);

  /** Search for a file in the theme.
      returns the full path, or empty if not found. */
  QString findFile(const QString& name);

  /** Installs directory. The source name is prepended
      with the theme work directory, the dest name is prepended
      with KApp::localkdedir(). Returns true on success. */
  virtual bool installDirectory(const QString& name, const QString& dest);

  /** Removes given file. If dirName is given and name is
      no absolute path, then dirName+"/"+name is removed. Does
      nothing if name is empty or null. */
  virtual void removeFile(const QString& name, const QString &dirName);

  /** Install theme group. Returns number of installed files. */
  virtual int installGroup(const char* groupName);

  /** Apply color scheme change to all open windows. Taken from
      kdisplay / colorscm.cpp */
  virtual void colorSchemeApply(void);
  /** Create color scheme from current settings. **/
  void updateColorScheme(KSimpleConfig *);

  /** Load mappings file. */
  virtual void loadMappings(void);

  /** Execute all commands in the command list mCmdList. called from
      install(). */
  virtual void doCmdList(void);

  /** Called from installGroup() with the value of the ConfigInstallCmd
      value. */
  virtual void installCmd(KSimpleConfig* cfg, const QString& cmd,
			  int &installedFiles);

  /** Called from installGroup() with the value of the ConfigPreInstallCmd
      value. */
  virtual void preInstallCmd(KSimpleConfig* cfg, const QString& cmd);

  /** Delete all files in work directory. */
  virtual void cleanupWorkDir(void);

  /** Rename file by adding a tilde (~) to the filename. Returns
      true if file exists. */
  virtual bool backupFile(const QString &filename) const;

  /** Load/save config settings */
  virtual void loadSettings(void);
  virtual void saveSettings(void);

  /** Stretch pixmap. Drops "None" color when there is no pixel
    using it. This crashes kwm. */
  void stretchPixmap(const QString &filename, bool verticalStretch);

  /** Add file to list of installed files. */
  virtual void addInstFile(const QString &filename);

  /** Read list of installled files. */
  virtual void readInstFileList(const char* groupName);

  /** Write list of installled files. */
  virtual void writeInstFileList(const char* groupName);

  /** Run krdb if krdb-usage is enabled. */
  virtual void runKrdb(void) const;

  /** Activate new icon theme. */
  virtual void applyIcons(void);

  /** Returns filename of given file+path (name up to the last slash) */
  virtual const QString fileOf(const QString&) const;

  /** Returns path of given file+path (up to the last slash) */
  virtual const QString pathOf(const QString&) const;

  /** Find .theme file in a list of files. **/
  void findThemerc(const QString &path, const QStringList &list);

protected:
  bool mValid; 		   // Whether the theme can be applied.

  enum ThemeType { Theme_KDE, Theme_Windows };

  ThemeType mThemeType;	   
  QString mFileName;       // Name+path
  QString mThemePath;      // Path to dir where theme files are stored
  QString mThemercFile;    // Name of the .themerc file
  QString mPreviewFile;    // Name of the preview image
  QString mRestartCmd;     // Shell command that restarts an app
  QPixmap mPreview;
  QString mConfigDir;

  QString mName;
  QString mDescription;
  QString mAuthor;
  QString mEmail;
  QString mHomePage;
  QString mVersion;
  
  QStringList mFileList;
  
  KSimpleConfig* mMappings;
  KSimpleConfig* mConfig;
  QStringList mCmdList;
  QStringList mInstFiles;	// List of installed files
  int mInstIcons;		// Number of installed icons
  int mKwmCount;		// Number of kwm theme pixmaps
};

#endif /*THEME_H*/
