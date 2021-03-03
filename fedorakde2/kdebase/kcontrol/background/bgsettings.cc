/* vi: ts=8 sts=4 sw=4
 *
 * $Id: bgsettings.cc,v 1.11.2.2 2001/10/14 17:25:06 binner Exp $
 *
 * This file is part of the KDE project, module kdesktop.
 * Copyright (C) 1999 Geert Jansen <g.t.jansen@stud.tue.nl>
 *
 * You can Freely distribute this program under the GNU Library General
 * Public License. See the file "COPYING.LIB" for the exact licensing terms.
 */

#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include <kconfig.h>

#include <qstring.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qobject.h>
#include <qfileinfo.h>
#include <qdir.h>

#include <netwm.h>
#include <kapp.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kstddirs.h>
#include <klocale.h>
#include <ksimpleconfig.h>

#include <bgdefaults.h>
#include <bgsettings.h>


/*
 * QString -> int hash. From Qt's QGDict::hashKeyString().
 */

static int QHash(QString key)
{
    int g, h = 0;
    const QChar *p = key.unicode();
    for (unsigned i=0; i < key.length(); i++) {
        h = (h << 4) + p[i].cell();
        if ((g = (h & 0xf0000000)))
            h ^= (g >> 24);
        h &= ~g;
    }
    return h;
}


/**** KBackgroundPattern ****/


KBackgroundPattern::KBackgroundPattern(QString name)
{
    dirty = false;
    hashdirty = true;

    m_pDirs = KGlobal::dirs();
    m_pDirs->addResourceType("dtop_pattern", m_pDirs->kde_default("data") +
                             "kdesktop/patterns");
    m_pConfig = 0L;

    m_Name = name;
    if (m_Name.isEmpty())
        return;

    init();
    readSettings();
}


KBackgroundPattern::~KBackgroundPattern()
{
    delete m_pConfig;
}


void KBackgroundPattern::load(QString name)
{
    m_Name = name;
    init();
    readSettings();
}


void KBackgroundPattern::init(bool force_rw)
{
    delete m_pConfig;

    m_File = m_pDirs->findResource("dtop_pattern", m_Name + ".desktop");
    if (force_rw || m_File.isEmpty()) {
        m_File = m_pDirs->saveLocation("dtop_pattern") + m_Name + ".desktop";
        m_pConfig = new KSimpleConfig(m_File);
    } else
        m_pConfig = new KSimpleConfig(m_File);

    m_pConfig->setGroup("KDE Desktop Pattern");

    QFileInfo fi(m_File);
    m_bReadOnly = !fi.isWritable();
}


void KBackgroundPattern::setComment(QString comment)
{
    if (m_Comment == comment)
        return;
    dirty = true;
    m_Comment = comment;
}


void KBackgroundPattern::setPattern(QString pattern)
{
    if (m_Pattern == pattern)
        return;
    dirty = hashdirty = true;
    m_Pattern = pattern;
}


void KBackgroundPattern::readSettings()
{
    dirty = false;
    hashdirty = true;

    m_Pattern = m_pConfig->readEntry("File");
    m_Comment = m_pConfig->readEntry("Comment");
}


void KBackgroundPattern::writeSettings()
{
    if (!dirty)
        return;
    if (m_bReadOnly)
        init(true);

    m_pConfig->writeEntry("File", m_Pattern);
    m_pConfig->writeEntry("Comment", m_Comment);
    m_pConfig->sync();
    dirty = false;
}


bool KBackgroundPattern::isAvailable()
{
    QString file = m_Pattern;
    if (file.at(0) != '/')
        file = m_pDirs->findResource("dtop_pattern", file);
    QFileInfo fi(file);
    return (fi.exists());
}


bool KBackgroundPattern::remove()
{
    if (m_bReadOnly)
        return false;
    return !unlink(m_File.latin1());
}


QString KBackgroundPattern::fingerprint()
{
    return m_File;
}


int KBackgroundPattern::hash()
{
    if (hashdirty) {
	m_Hash = QHash(fingerprint());
	hashdirty = false;
    }
    return m_Hash;
}


/* static */
QStringList KBackgroundPattern::list()
{
    KStandardDirs *dirs = KGlobal::dirs();
    dirs->addResourceType("dtop_pattern", dirs->kde_default("data") +
                          "kdesktop/patterns");
    QStringList lst = dirs->findAllResources("dtop_pattern", "*.desktop",
                                             false, true);
    QStringList::Iterator it;
    for (it=lst.begin(); it!=lst.end(); it++) {
        // Strip path and suffix
        int pos = (*it).findRev('/');
        if (pos != -1)
            (*it) = (*it).mid(pos+1);
        pos = (*it).findRev('.');
        if (pos != -1)
            (*it) = (*it).left(pos);
    }
    return lst;
}


/**** KBackgroundProgram ****/


KBackgroundProgram::KBackgroundProgram(QString name)
{
    dirty = false;
    hashdirty = true;

    m_pDirs = KGlobal::dirs();
    m_pDirs->addResourceType("dtop_program", m_pDirs->kde_default("data") +
                             "kdesktop/programs");
    m_pConfig = 0L;

    // prevent updates when just constructed.
    m_LastChange = (int) time(0L);

    m_Name = name;
    if (m_Name.isEmpty())
        return;

    init();
    readSettings();
}


KBackgroundProgram::~KBackgroundProgram()
{
    delete m_pConfig;
}


void KBackgroundProgram::init(bool force_rw)
{
    delete m_pConfig;

    m_File = m_pDirs->findResource("dtop_program", m_Name + ".desktop");
    if (force_rw || m_File.isEmpty()) {
        m_File = m_pDirs->saveLocation("dtop_program") + m_Name + ".desktop";
        m_pConfig = new KSimpleConfig(m_File);
    } else
        m_pConfig = new KSimpleConfig(m_File);
    m_pConfig->setGroup("KDE Desktop Program");

    QFileInfo fi(m_File);
    m_bReadOnly = !fi.isWritable();
}


void KBackgroundProgram::load(QString name)
{
    m_Name = name;
    init();
    readSettings();
}


void KBackgroundProgram::setComment(QString comment)
{
    if (m_Comment == comment)
        return;
    dirty = true;
    m_Comment = comment;
}


void KBackgroundProgram::setExecutable(QString executable)
{
    if (m_Executable == executable)
        return;
    dirty = true;
    m_Executable = executable;
}


void KBackgroundProgram::setCommand(QString command)
{
    if (m_Command == command)
        return;
    dirty = hashdirty = true;
    m_Command = command;
}


void KBackgroundProgram::setPreviewCommand(QString command)
{
    if (m_PreviewCommand == command)
        return;
    dirty = true;
    m_PreviewCommand = command;
}


void KBackgroundProgram::setRefresh(int refresh)
{
    if (m_Refresh == refresh)
        return;
    dirty = hashdirty = true;
    m_Refresh = refresh;
}


void KBackgroundProgram::readSettings()
{
    dirty = false;
    hashdirty = true;

    m_Comment = m_pConfig->readEntry("Comment");
    m_Executable = m_pConfig->readEntry("Executable");
    m_Command = m_pConfig->readEntry("Command");
    m_PreviewCommand = m_pConfig->readEntry("PreviewCommand", m_Command);
    m_Refresh = m_pConfig->readNumEntry("Refresh", 300);
}


void KBackgroundProgram::writeSettings()
{
    if (!dirty)
        return;
    if (m_bReadOnly)
        init(true);

    m_pConfig->writeEntry("Comment", m_Comment);
    m_pConfig->writeEntry("Executable", m_Executable);
    m_pConfig->writeEntry("Command", m_Command);
    m_pConfig->writeEntry("PreviewCommand", m_PreviewCommand);
    m_pConfig->writeEntry("Refresh", m_Refresh);
    m_pConfig->sync();
    dirty = false;
}


bool KBackgroundProgram::isAvailable()
{
    return !m_pDirs->findExe(m_Executable).isEmpty();
}


bool KBackgroundProgram::remove()
{
    if (m_bReadOnly)
        return false;
    return !unlink(m_File.latin1());
}


bool KBackgroundProgram::needUpdate()
{
    return (m_LastChange + 60*m_Refresh <= time(0L));
}


void KBackgroundProgram::update()
{
    m_LastChange = (int) time(0L);
}


QString KBackgroundProgram::fingerprint()
{
    return QString("co:%1;re:%2").arg(m_Command).arg(m_Refresh);
}


int KBackgroundProgram::hash()
{
    if (hashdirty) {
	m_Hash = QHash(fingerprint());
	hashdirty = false;
    }
    return m_Hash;
}


/* static */
QStringList KBackgroundProgram::list()
{
    KStandardDirs *dirs = KGlobal::dirs();
    dirs->addResourceType("dtop_program", dirs->kde_default("data") +
                          "kdesktop/programs");
    QStringList lst = dirs->findAllResources("dtop_program", "*.desktop",
                                             false, true);
    QStringList::Iterator it;
    for (it=lst.begin(); it!=lst.end(); it++) {
        // Strip path and suffix
        int pos = (*it).findRev('/');
        if (pos != -1)
            (*it) = (*it).mid(pos+1);
        pos = (*it).findRev('.');
        if (pos != -1)
            (*it) = (*it).left(pos);
    }
    return lst;
}


/**** KBackgroundSettings ****/


KBackgroundSettings::KBackgroundSettings(int desk, KConfig *config)
{
    dirty = false; hashdirty = true;
    m_Desk = desk;

    // Default values.
    defColorA = _defColorA;
    defColorB = _defColorB;
    if (QPixmap::defaultDepth() > 8)
        defBackgroundMode = _defBackgroundMode;
    else
        defBackgroundMode = Flat;
    defWallpaperMode = _defWallpaperMode;
    defMultiMode = _defMultiMode;
    defBlendMode = _defBlendMode;
    defBlendBalance = _defBlendBalance;
    defReverseBlending = _defReverseBlending;

    // Background modes
    #define ADD_STRING(ID) m_BMMap[#ID] = ID; m_BMRevMap[ID] = (char *) #ID;
    ADD_STRING(Flat)
    ADD_STRING(Pattern)
    ADD_STRING(Program)
    ADD_STRING(HorizontalGradient)
    ADD_STRING(VerticalGradient)
    ADD_STRING(PyramidGradient)
    ADD_STRING(PipeCrossGradient)
    ADD_STRING(EllipticGradient)
    #undef ADD_STRING

    // Blend modes
    #define ADD_STRING(ID) m_BlMMap[#ID] = ID; m_BlMRevMap[ID] = (char *) #ID;
    ADD_STRING(NoBlending)
    ADD_STRING(HorizontalBlending)
    ADD_STRING(VerticalBlending)
    ADD_STRING(PyramidBlending)
    ADD_STRING(PipeCrossBlending)
    ADD_STRING(EllipticBlending)
    ADD_STRING(IntensityBlending)
    ADD_STRING(SaturateBlending)
    ADD_STRING(ContrastBlending)
    ADD_STRING(HueShiftBlending)
    #undef ADD_STRING

    // Wallpaper modes
    #define ADD_STRING(ID) m_WMMap[#ID] = ID; m_WMRevMap[ID] = (char *) #ID;
    ADD_STRING(NoWallpaper)
    ADD_STRING(Centred)
    ADD_STRING(Tiled)
    ADD_STRING(CenterTiled)
    ADD_STRING(CentredMaxpect)
    ADD_STRING(Scaled)
    ADD_STRING(CentredAutoFit)
    #undef ADD_STRING

    // Multiple wallpaper modes
    #define ADD_STRING(ID) m_MMMap[#ID] = ID; m_MMRevMap[ID] = (char *) #ID;
    ADD_STRING(NoMulti)
    ADD_STRING(InOrder)
    ADD_STRING(Random)
    #undef ADD_STRING

    m_pDirs = KGlobal::dirs();
    if (! config) {
	int screen_number = 0;
	if (qt_xdisplay())
	    screen_number = DefaultScreen(qt_xdisplay());
	QCString configname;
	if (screen_number == 0)
	    configname = "kdesktoprc";
	else
	    configname.sprintf("kdesktop-screen-%drc", screen_number);

	m_pConfig = new KConfig(configname);
    } else
	m_pConfig = config;

    srand((unsigned int) time(0L));

    if (m_Desk == -1)
	return;

    readSettings();
}


KBackgroundSettings::~KBackgroundSettings()
{
}


void KBackgroundSettings::load(int desk, bool reparseConfig)
{
    m_Desk = desk;
    readSettings(reparseConfig);
}


void KBackgroundSettings::setColorA(const QColor& color)
{
    if (m_ColorA == color)
        return;
    dirty = hashdirty = true;
    m_ColorA = color;
}


void KBackgroundSettings::setColorB(const QColor& color)
{
    if (m_ColorB == color)
        return;
    dirty = hashdirty = true;
    m_ColorB = color;
}


void KBackgroundSettings::setPatternName(QString name)
{
    int ohash = KBackgroundPattern::hash();
    KBackgroundPattern::load(name);
    if (ohash == KBackgroundPattern::hash())
	return;

    dirty = hashdirty = true;
    return;
}


void KBackgroundSettings::setProgram(QString name)
{
    int ohash = KBackgroundProgram::hash();
    KBackgroundProgram::load(name);
    if (ohash == KBackgroundProgram::hash())
	return;

    dirty = hashdirty = true;
    return;
}


void KBackgroundSettings::setBackgroundMode(int mode)
{
    if (m_BackgroundMode == mode)
	return;
    dirty = hashdirty = true;
    m_BackgroundMode = mode;
}

void KBackgroundSettings::setBlendMode(int mode)
{
    if (m_BlendMode == mode)
	return;
    dirty = hashdirty = true;
    m_BlendMode = mode;
}

void KBackgroundSettings::setBlendBalance(int value)
{
    if (m_BlendBalance == value)
	return;
    dirty = hashdirty = true;
    m_BlendBalance = value;
}

void KBackgroundSettings::setReverseBlending(bool value)
{
    if (m_ReverseBlending == value)
	return;
    dirty = hashdirty = true;
    m_ReverseBlending = value;
}


void KBackgroundSettings::setWallpaper(QString wallpaper)
{
    if (m_Wallpaper == wallpaper)
        return;
    dirty = hashdirty = true;
    m_Wallpaper = wallpaper;
}


void KBackgroundSettings::setWallpaperMode(int mode)
{
    if (m_WallpaperMode == mode)
        return;
    dirty = hashdirty = true;
    m_WallpaperMode = mode;
}


void KBackgroundSettings::setWallpaperList(QStringList list)
{
    if (m_WallpaperList == list)
	return;
    dirty = hashdirty = true;
    m_WallpaperList = list;
    updateWallpaperFiles();
    changeWallpaper(true);
}


void KBackgroundSettings::setWallpaperChangeInterval(int interval)
{
    if (m_Interval == interval)
	return;
    dirty = hashdirty = true;
    m_Interval = interval;
}


void KBackgroundSettings::setMultiWallpaperMode(int mode)
{
    if (m_MultiMode == mode)
	return;
    dirty = hashdirty = true;
    m_MultiMode = mode;
    changeWallpaper(true);
}


void KBackgroundSettings::readSettings(bool reparse)
{
    if (reparse)
        m_pConfig->reparseConfiguration();

    m_pConfig->setGroup(QString("Desktop%1").arg(m_Desk));

    // Background mode (Flat, div. Gradients, Pattern or Program)
    m_ColorA = m_pConfig->readColorEntry("Color1", &defColorA);
    m_ColorB = m_pConfig->readColorEntry("Color2", &defColorB);

    QString s = m_pConfig->readEntry("Pattern");
    if (!s.isEmpty())
        KBackgroundPattern::load(s);

    s = m_pConfig->readEntry("Program");
    if (!s.isEmpty())
        KBackgroundProgram::load(s);

    m_BackgroundMode = defBackgroundMode;
    s = m_pConfig->readEntry("BackgroundMode", "invalid");
    if (m_BMMap.contains(s)) {
        int mode = m_BMMap[s];
        // consistency check
        if  ( ((mode != Pattern) && (mode != Program)) ||
              ((mode == Pattern) && !pattern().isEmpty()) ||
              ((mode == Program) && !command().isEmpty())
            )
            m_BackgroundMode = mode;
    }

    m_BlendMode = defBlendMode;
    s = m_pConfig->readEntry("BlendMode", "invalid");
    if (m_BlMMap.contains(s)) {
      m_BlendMode = m_BlMMap[s];
    }

    m_BlendBalance = defBlendBalance;
    int value = m_pConfig->readNumEntry( "BlendBalance", defBlendBalance);
    if (value > -201 && value < 201)
      m_BlendBalance = value;

    m_ReverseBlending = m_pConfig->readBoolEntry( "ReverseBlending", defReverseBlending);

    // Multiple wallpaper config
    m_WallpaperList = m_pConfig->readListEntry("WallpaperList");
    updateWallpaperFiles();

    m_Interval = m_pConfig->readNumEntry("ChangeInterval", 60);
    m_LastChange = m_pConfig->readNumEntry("LastChange", 0);
    m_CurrentWallpaper = m_pConfig->readNumEntry("CurrentWallpaper", 0);

    m_MultiMode = defMultiMode;
    s = m_pConfig->readEntry("MultiWallpaperMode");
    if (m_MMMap.contains(s)) {
	int mode = m_MMMap[s];
        // consistency check.
	if ((mode == NoMulti) || m_WallpaperFiles.count())
	    m_MultiMode = mode;
    }

    // Wallpaper mode (NoWallpaper, div. tilings)
    m_WallpaperMode = defWallpaperMode;
    m_Wallpaper = m_pConfig->readPathEntry("Wallpaper");
    s = m_pConfig->readEntry("WallpaperMode", "invalid");
    if (m_WMMap.contains(s)) {
        int mode = m_WMMap[s];
        // consistency check.
        if ((mode == NoWallpaper) || !m_Wallpaper.isEmpty() || (m_MultiMode!=NoMulti))
            m_WallpaperMode = mode;
    }

    dirty = false; hashdirty = true;
}


void KBackgroundSettings::writeSettings()
{
    KBackgroundPattern::writeSettings();
    KBackgroundProgram::writeSettings();

    if (!dirty)
        return;

    m_pConfig->setGroup(QString("Desktop%1").arg(m_Desk));
    m_pConfig->writeEntry("Color1", m_ColorA);
    m_pConfig->writeEntry("Color2", m_ColorB);
    m_pConfig->writeEntry("Pattern", KBackgroundPattern::name());
    m_pConfig->writeEntry("Program", KBackgroundProgram::name());
    m_pConfig->writeEntry("BackgroundMode", m_BMRevMap[m_BackgroundMode]);
    m_pConfig->writeEntry("Wallpaper", m_Wallpaper);
    m_pConfig->writeEntry("WallpaperMode", m_WMRevMap[m_WallpaperMode]);
    m_pConfig->writeEntry("MultiWallpaperMode", m_MMRevMap[m_MultiMode]);
    m_pConfig->writeEntry("BlendMode", m_BlMRevMap[m_BlendMode]);
    m_pConfig->writeEntry("BlendBalance", m_BlendBalance);
    m_pConfig->writeEntry("ReverseBlending", m_ReverseBlending);

    m_pConfig->writeEntry("WallpaperList", m_WallpaperList);
    m_pConfig->writeEntry("ChangeInterval", m_Interval);
    m_pConfig->writeEntry("LastChange", m_LastChange);
    m_pConfig->writeEntry("CurrentWallpaper", m_CurrentWallpaper);

    m_pConfig->sync();

    dirty = false;
}


/*
 * (re)Build m_WallpaperFiles from m_WallpaperList
 */
void KBackgroundSettings::updateWallpaperFiles()
{
    QStringList::Iterator it;
    m_WallpaperFiles.clear();
    for (it=m_WallpaperList.begin(); it!=m_WallpaperList.end(); it++) {
	QFileInfo fi(*it);
	if (!fi.exists())
	    continue;
	if (fi.isFile() && fi.isReadable())
	    m_WallpaperFiles.append(*it);
	if (fi.isDir()) {
	    QDir dir(*it);
	    QStringList lst = dir.entryList(QDir::Files | QDir::Readable);
	    QStringList::Iterator it;
	    for (it=lst.begin(); it!=lst.end(); it++)
		m_WallpaperFiles.append(dir.absFilePath(*it));
	}

    }
}


/*
 * Select a new wallpaper from the list.
 */
void KBackgroundSettings::changeWallpaper(bool init)
{
    switch (m_MultiMode) {
    case InOrder:
	m_CurrentWallpaper++;
	if (init || (m_CurrentWallpaper >= (int) m_WallpaperFiles.count()))
	    m_CurrentWallpaper = 0;
	break;

    case Random:
	m_CurrentWallpaper = (int) (((double) m_WallpaperFiles.count() * rand()) /
				    (1.0 + RAND_MAX));
	break;
    default:
	return;
    }

    m_LastChange = (int) time(0L);

    // sync random config to file without syncing the rest
    int screen_number = 0;
    if (qt_xdisplay())
	screen_number = DefaultScreen(qt_xdisplay());
    QCString configname;
    if (screen_number == 0)
	configname = "kdesktoprc";
    else
	configname.sprintf("kdesktop-screen-%drc", screen_number);

    KConfig cfg(configname);
    cfg.setGroup(QString("Desktop%1").arg(m_Desk));
    cfg.writeEntry("CurrentWallpaper", m_CurrentWallpaper);
    cfg.writeEntry("LastChange", m_LastChange);
    cfg.sync();

    hashdirty = true;
}


QString KBackgroundSettings::currentWallpaper()
{
    if (m_MultiMode == NoMulti)
	return m_Wallpaper;
    if (m_CurrentWallpaper < (int) m_WallpaperFiles.count())
	return m_WallpaperFiles[m_CurrentWallpaper];
    return QString();
}


bool KBackgroundSettings::needWallpaperChange()
{
    if (m_MultiMode == NoMulti)
	return false;

    return ((m_LastChange + 60*m_Interval) <= time(0L));
}


/*
 * Create a fingerprint string for this config. Be somewhat (overly) carefull
 * that only a different final result will give a different fingerprint.
 */

QString KBackgroundSettings::fingerprint()
{
    QString s = QString("bm:%1;").arg(m_BackgroundMode);
    switch (m_BackgroundMode) {
    case Flat:
        s += QString("ca:%1;").arg(m_ColorA.rgb());
        break;
    case Program:
        s += QString("pr:%1;").arg(KBackgroundProgram::hash());
        break;
    case Pattern:
        s += QString("ca:%1;cb:%2;pt:%3;").arg(m_ColorA.rgb())
	     .arg(m_ColorB.rgb()).arg(KBackgroundPattern::hash());
        break;
    default:
        s += QString("ca:%1;cb:%2;").arg(m_ColorA.rgb()).arg(m_ColorB.rgb());
        break;
    }

    s += QString("wm:%1;").arg(m_WallpaperMode);
    if (m_WallpaperMode != NoWallpaper)
        s += QString("wp:%1;").arg(currentWallpaper());
    s += QString("blm:%1;").arg(m_BlendMode);
    if (m_BlendMode != NoBlending) {
      s += QString("blb:%1;").arg(m_BlendBalance);
      s += QString("rbl:%1;").arg(int(m_ReverseBlending));
    }
    return s;
}


int KBackgroundSettings::hash()
{
    if (hashdirty) {
        m_Hash = QHash(fingerprint());
	hashdirty = false;
    }
    return m_Hash;
}


/**** KGlobalBackgroundSettings ****/

KGlobalBackgroundSettings::KGlobalBackgroundSettings()
{
    dirty = false;

    readSettings();
}


QString KGlobalBackgroundSettings::deskName(int desk)
{
    return m_Names[desk];
}


/*
void KGlobalBackgroundSettings::setDeskName(int desk, QString name)
{
    if (name == m_Names[desk])
	return;
    dirty = true;
    m_Names[desk] = name;
}
*/


void KGlobalBackgroundSettings::setCacheSize(int size)
{
    if (size == m_CacheSize)
	return;
    dirty = true;
    m_CacheSize = size;
}


void KGlobalBackgroundSettings::setLimitCache(bool limit)
{
    if (limit == m_bLimitCache)
	return;
    dirty = true;
    m_bLimitCache = limit;
}


void KGlobalBackgroundSettings::setCommonBackground(bool common)
{
    if (common == m_bCommon)
	return;
    dirty = true;
    m_bCommon = common;
}


void KGlobalBackgroundSettings::setDockPanel(bool dock)
{
    if (dock == m_bDock)
	return;
    dirty = true;
    m_bDock = dock;
}


void KGlobalBackgroundSettings::setExportBackground(bool _export)
{
    if (_export == m_bExport)
	return;
    dirty = true;
    m_bExport = _export;
}


void KGlobalBackgroundSettings::readSettings()
{
    int screen_number = 0;
    if (qt_xdisplay())
	screen_number = DefaultScreen(qt_xdisplay());
    QCString configname;
    if (screen_number == 0)
	configname = "kdesktoprc";
    else
	configname.sprintf("kdesktop-screen-%drc", screen_number);

    KConfig cfg(configname);
    cfg.setGroup("Background Common");
    m_bCommon = cfg.readBoolEntry("CommonDesktop", _defCommon);
    m_bDock = cfg.readBoolEntry("Dock", _defDock);
    m_bExport = cfg.readBoolEntry("Export", _defExport);
    m_bLimitCache = cfg.readBoolEntry("LimitCache", _defLimitCache);
    m_CacheSize = cfg.readNumEntry("CacheSize", _defCacheSize);

    m_Names.clear();
    NETRootInfo info( qt_xdisplay(), NET::DesktopNames | NET::NumberOfDesktops );
    for ( int i = 0 ; i < info.numberOfDesktops() ; ++i )
      m_Names.append( QString::fromUtf8(info.desktopName(i+1)) );

    dirty = false;
}


void KGlobalBackgroundSettings::writeSettings()
{
    if (!dirty)
	return;

    int screen_number = 0;
    if (qt_xdisplay())
	screen_number = DefaultScreen(qt_xdisplay());
    QCString configname;
    if (screen_number == 0)
	configname = "kdesktoprc";
    else
	configname.sprintf("kdesktop-screen-%drc", screen_number);

    KConfig cfg(configname);
    cfg.setGroup("Background Common");
    cfg.writeEntry("CommonDesktop", m_bCommon);
    cfg.writeEntry("Dock", m_bDock);
    cfg.writeEntry("Export", m_bExport);
    cfg.writeEntry("LimitCache", m_bLimitCache);
    cfg.writeEntry("CacheSize", m_CacheSize);

    dirty = false;
}

