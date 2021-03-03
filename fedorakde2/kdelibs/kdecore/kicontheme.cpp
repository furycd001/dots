/* vi: ts=8 sts=4 sw=4
 *
 * $Id$
 *
 * This file is part of the KDE project, module kdecore.
 * Copyright (C) 2000 Geert Jansen <jansen@kde.org>
 *                    Antonio Larrosa <larrosa@kde.org>
 *
 * This is free software; it comes under the GNU Library General
 * Public License, version 2. See the file "COPYING.LIB" for the
 * exact licensing terms.
 *
 * kicontheme.cpp: Lowlevel icon theme handling.
 */

#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#include <qstring.h>
#include <qstringlist.h>
#include <qlist.h>
#include <qvaluelist.h>
#include <qmap.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qimage.h>
#include <qfileinfo.h>
#include <qdir.h>

#include <kdebug.h>
#include <kstddirs.h>
#include <kglobal.h>
#include <kconfig.h>
#include <ksimpleconfig.h>
#include <kinstance.h>

#include "kicontheme.h"

class KIconThemePrivate
{
public:
    QString example, screenshot;
    QString linkOverlay, lockOverlay, zipOverlay;
};

/**
 * A subdirectory in an icon theme.
 */
class KIconThemeDir
{
public:
    KIconThemeDir(const QString& dir, const KConfigBase *config);

    QString iconPath(const QString& name) const;
    QStringList iconList() const;
    QString dir() const { return mDir; }

    int context() const { return mContext; }
    int type() const { return mType; }
    int size() const { return mSize; }
    int minSize() const { return mMinSize; }
    int maxSize() const { return mMaxSize; }
    int threshold() const { return mThreshold; }

private:
    bool mbValid;
    int mType, mSize, mContext;
    int mMinSize, mMaxSize;
    int mThreshold;

    QString mDir;
};


/*** KIconTheme ***/

KIconTheme::KIconTheme(const QString& name, const QString& appName)
{
    d = new KIconThemePrivate;

    QStringList icnlibs;
    QStringList::ConstIterator it, itDir;
    QStringList themeDirs;
    QString cDir;

    // Applications can have local additions to the global "locolor" and
    // "hicolor" icon themes. For these, the _global_ theme description
    // files are used..

    if (!appName.isEmpty() && ( name== "hicolor" || name == "locolor" ) )
    {
	icnlibs = KGlobal::dirs()->resourceDirs("data");
	for (it=icnlibs.begin(); it!=icnlibs.end(); it++)
	{
	    cDir = *it + appName + "/icons/" + name;
	    if (QFile::exists( cDir ))
		themeDirs += cDir + "/";
	}
    }
    // Find the theme description file. These are always global.

    icnlibs = KGlobal::dirs()->resourceDirs("icon");
    for (it=icnlibs.begin(); it!=icnlibs.end(); it++)
    {
        cDir = *it + name + "/";
        if (KStandardDirs::exists(cDir))
        {
            themeDirs += cDir;
	    if (mDir.isEmpty() 
		    && KStandardDirs::exists( cDir + "index.desktop"))
		mDir = cDir;
        }
    }

    if (mDir.isEmpty())
    {
        kdDebug(264) << "Icon theme " << name << " not found.\n";
        return;
    }

    KSimpleConfig cfg(mDir + "index.desktop");
    cfg.setGroup("KDE Icon Theme");
    mName = cfg.readEntry("Name");
    mDesc = cfg.readEntry("Comment");
    mDepth = cfg.readNumEntry("DisplayDepth", 32);
    mInherits = cfg.readListEntry("Inherits");
    d->example = cfg.readEntry("Example");
    d->screenshot = cfg.readEntry("ScreenShot");
    d->linkOverlay = cfg.readEntry("LinkOverlay", "link");
    d->lockOverlay = cfg.readEntry("LockOverlay", "lock");
    d->zipOverlay = cfg.readEntry("ZipOverlay", "zip");

    QStringList dirs = cfg.readListEntry("Directories");
    mDirs.setAutoDelete(true);
    for (it=dirs.begin(); it!=dirs.end(); it++)
    {
	cfg.setGroup(*it);
	for (itDir=themeDirs.begin(); itDir!=themeDirs.end(); itDir++)
	    if (KStandardDirs::exists(*itDir + *it + "/"))
		mDirs.append(new KIconThemeDir(*itDir + *it, &cfg));
    }

    // Expand available sizes for scalable icons to their full range
    int i;
    QMap<int,QValueList<int> > scIcons;
    for (KIconThemeDir *dir=mDirs.first(); dir!=0L; dir=mDirs.next())
    {
        if ((dir->type() == KIcon::Scalable) && !scIcons.contains(dir->size()))
        {
            QValueList<int> lst;
            for (i=dir->minSize(); i<=dir->maxSize(); i++)
                lst += i;
            scIcons[dir->size()] = lst;
        }
    }

    QStringList groups;
    groups += "Desktop";
    groups += "Toolbar";
    groups += "MainToolbar";
    groups += "Small";
    groups += "Panel";
    int defDefSizes[] = { 32, 22, 22, 16, 32 };
    cfg.setGroup("KDE Icon Theme");
    for (it=groups.begin(), i=0; it!=groups.end(); it++, i++)
    {
        mDefSize[i] = cfg.readNumEntry(*it + "Default", defDefSizes[i]);
        QValueList<int> lst = cfg.readIntListEntry(*it + "Sizes"), exp;
        QValueList<int>::ConstIterator it2;
        for (it2=lst.begin(); it2!=lst.end(); it2++)
        {
            if (scIcons.contains(*it2))
                exp += scIcons[*it2];
            else
                exp += *it2;
        }
        mSizes[i] = exp;
    }

}

KIconTheme::~KIconTheme()
{
    delete d;
}

bool KIconTheme::isValid() const
{
    return !mDirs.isEmpty();
}

QString KIconTheme::example() const { return d->example; }
QString KIconTheme::screenshot() const { return d->screenshot; }
QString KIconTheme::linkOverlay() const { return d->linkOverlay; }
QString KIconTheme::lockOverlay() const { return d->lockOverlay; }
QString KIconTheme::zipOverlay() const { return d->zipOverlay; }

int KIconTheme::defaultSize(int group) const
{
    if ((group < 0) || (group >= KIcon::LastGroup))
    {
        kdDebug(264) << "Illegal icon group: " << group << "\n";
        return -1;
    }
    return mDefSize[group];
}

QValueList<int> KIconTheme::querySizes(int group) const
{
    QValueList<int> empty;
    if ((group < 0) || (group >= KIcon::LastGroup))
    {
        kdDebug(264) << "Illegal icon group: " << group << "\n";
        return empty;
    }
    return mSizes[group];
}

QStringList KIconTheme::queryIcons(int size, int context) const
{
    int delta = 1000, dw;

    QListIterator<KIconThemeDir> dirs(mDirs);
    KIconThemeDir *dir;

    // Try to find exact match
    for ( ; dirs.current(); ++dirs)
    {
        dir = dirs.current();
        if ((context != KIcon::Any) && (context != dir->context()))
            continue;
        if ((dir->type() == KIcon::Fixed) && (dir->size() == size))
            return dir->iconList();
        if ((dir->type() == KIcon::Scalable) &&
                (size >= dir->minSize()) && (size <= dir->maxSize()))
            return dir->iconList();
	if (dir->type() == KIcon::Threshold) return dir->iconList();
    }

    dirs.toFirst();

    // Find close match
    KIconThemeDir *best = 0L;
    for ( ; dirs.current(); ++dirs)
    {
        dir = dirs.current();
        if ((context != KIcon::Any) && (context != dir->context()))
            continue;
        dw = dir->size() - size;
        if ((dw > 6) || (abs(dw) >= abs(delta)))
            continue;
        delta = dw;
        best = dir;
    }
    if (best == 0L)
        return QStringList();

    return best->iconList();
}

QStringList KIconTheme::queryIconsByContext(int size, int context) const
{
    QListIterator<KIconThemeDir> dirs(mDirs);
    int dw;
    KIconThemeDir *dir;

    // We want all the icons for a given context, but we prefer icons
    // of size size . Note that this may (will) include duplicate icons
    QStringList iconlist[34]; // 33 == 48-16+1
    // Usually, only the 0, 6 (22-16), 10 (32-22), 16 (48-32 or 32-16),
    // 26 (48-22) and 32 (48-16) will be used, but who knows if someone
    // will make icon themes with different icon sizes.

    for ( ; dirs.current(); ++dirs)
    {
        dir = dirs.current();
        if ((context != KIcon::Any) && (context != dir->context()))
            continue;
        dw = abs(dir->size() - size);
        iconlist[(dw<33)?dw:33]+=dir->iconList();
    }

    QStringList iconlistResult;
    for (int i=0; i<34; i++) iconlistResult+=iconlist[i];

    return iconlistResult;
}

KIcon KIconTheme::iconPath(const QString& name, int size, int match) const
{
    KIcon icon;
    QString path;
    int delta = 1000, dw;
    KIconThemeDir *dir;

    dw = 1000; // shut up, gcc
    QListIterator<KIconThemeDir> dirs(mDirs);
    for ( ; dirs.current(); ++dirs)
    {
        dir = dirs.current();

        if (match == KIcon::MatchExact)
        {
            if ((dir->type() == KIcon::Fixed) && (dir->size() != size))
                continue;
            if ((dir->type() == KIcon::Scalable) &&
                ((size < dir->minSize()) || (size > dir->maxSize())))
              continue;
            if ((dir->type() == KIcon::Threshold) &&
		(abs(dir->size()-size) > dir->threshold()))
                continue;
        } else
        {
            dw = dir->size() - size;
            if (dir->type() != KIcon::Threshold &&
               ((dw > 7) || (abs(dw) >= abs(delta))))
                continue;
        }

        path = dir->iconPath(name);
        if (path.isEmpty())
            continue;
        icon.path = path;
        icon.size = dir->size();
        icon.type = dir->type();
	icon.threshold = dir->threshold();
        icon.context = dir->context();

        // if we got in MatchExact that far, we find no better
        if (match == KIcon::MatchExact)
            return icon;
	else
        {
	    delta = dw;
	    if (delta==0) return icon; // We won't find a better match anyway
        }
    }
    return icon;
}

// static
QString *KIconTheme::_theme = 0L;

// static
QStringList *KIconTheme::_theme_list = 0L;

// static
QString KIconTheme::current()
{
    // Static pointer because of unloading problems wrt DSO's.
    if (_theme != 0L)
        return *_theme;

    _theme = new QString();
    KConfig *config = KGlobal::config();
    KConfigGroupSaver saver(config, "Icons");
    *_theme = config->readEntry("Theme");
    if (_theme->isEmpty())
    {
        if (QPixmap::defaultDepth() > 8)
            *_theme = QString::fromLatin1("hicolor");
        else
            *_theme = QString::fromLatin1("locolor");
    }
    return *_theme;
}

// static
QStringList KIconTheme::list()
{
    // Static pointer because of unloading problems wrt DSO's.
    if (_theme_list != 0L)
        return *_theme_list;

    _theme_list = new QStringList();
    QStringList icnlibs = KGlobal::dirs()->resourceDirs("icon");
    QStringList::ConstIterator it;
    for (it=icnlibs.begin(); it!=icnlibs.end(); it++)
    {
        QDir dir(*it);
        if (!dir.exists())
            continue;
        QStringList lst = dir.entryList(QDir::Dirs);
        QStringList::ConstIterator it2;
        for (it2=lst.begin(); it2!=lst.end(); it2++)
        {
            if ((*it2 == ".") || (*it2 == ".."))
                continue;
            if (!KStandardDirs::exists(*it + *it2 + "/index.desktop"))
                continue;
            if (!_theme_list->contains(*it2))
                _theme_list->append(*it2);
        }
    }
    return *_theme_list;
}

// static
void KIconTheme::reconfigure()
{
    delete _theme;
    _theme=0L;
    delete _theme_list;
    _theme_list=0L;
}

/*** KIconThemeDir ***/

KIconThemeDir::KIconThemeDir(const QString& dir, const KConfigBase *config)
{
    mbValid = false;
    mDir = dir;
    mSize = config->readNumEntry("Size");
    mMinSize = 1;    // just set the variables to something
    mMaxSize = 50;   // meaningful in case someone calls minSize or maxSize
    mType = KIcon::Fixed;

    if (mSize == 0)
        return;

    QString tmp = config->readEntry("Context");
    if (tmp == "Devices")
        mContext = KIcon::Device;
    else if (tmp == "MimeTypes")
        mContext = KIcon::MimeType;
    else if (tmp == "FileSystems")
        mContext = KIcon::FileSystem;
    else if (tmp == "Applications")
        mContext = KIcon::Application;
    else if (tmp == "Actions")
        mContext = KIcon::Action;
    else {
        kdDebug(264) << "Invalid Context= line for icon theme: " << mDir << "\n";
        return;
    }
    tmp = config->readEntry("Type");
    if (tmp == "Fixed")
        mType = KIcon::Fixed;
    else if (tmp == "Scalable")
        mType = KIcon::Scalable;
    else if (tmp == "Threshold")
        mType = KIcon::Threshold;
    else {
        kdDebug(264) << "Invalid Type= line for icon theme: " <<  mDir << "\n";
        return;
    }
    if (mType == KIcon::Scalable)
    {
        mMinSize = config->readNumEntry("MinSize", mSize);
        mMaxSize = config->readNumEntry("MaxSize", mSize);
    } else if (mType == KIcon::Threshold)
	mThreshold = config->readNumEntry("Threshold", 2);
    mbValid = true;
}

QString KIconThemeDir::iconPath(const QString& name) const
{
    if (!mbValid)
        return QString::null;
    QString file = mDir + "/" + name;

    if (access(QFile::encodeName(file), R_OK) == 0)
        return file;

    return QString::null;
}

QStringList KIconThemeDir::iconList() const
{
    QDir dir(mDir);
    QStringList lst = dir.entryList("*.png;*.xpm", QDir::Files);
    QStringList result;
    QStringList::ConstIterator it;
    for (it=lst.begin(); it!=lst.end(); it++)
        result += mDir + "/" + *it;
    return result;
}
