/*****************************************************************

Copyright (c) 2000 the kicker authors. See file AUTHORS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include <time.h>
#include <qstringlist.h>
#include <qvaluelist.h>

#include <kglobal.h>
#include <kdebug.h>
#include <kconfig.h>

#include "recentapps.h"

RecentlyLaunchedApps::RecentlyLaunchedApps()
{
    // set defaults
    m_bRecentVsOften = false;
    m_nNumVisible = 5; // max entries to display
    m_nMaxEntries = 25; // max entries to store
    m_Info.resize(m_nMaxEntries * 2);
    m_Info.setAutoDelete(TRUE);
    m_nNumMenuItems = 0;
    m_bNeedToUpdate = false;
    m_bInitialised = false;
}

void RecentlyLaunchedApps::init()
{
    if (m_bInitialised) return;

//    kdDebug() << "RecentlyLaunchedApps::init" << endl;
    m_nNumMenuItems = 0;

    KConfig *config = KGlobal::config();
    config->setGroup("menus");

    if (config->hasKey("NumVisibleEntries"))
        m_nNumVisible = config->readNumEntry("NumVisibleEntries");

    if (config->hasKey("MaxRecentAppsEntries")){
        int nMax = config->readNumEntry("MaxRecentAppsEntries");
        setMaxEntries(nMax);
    }

    if (config->hasKey("RecentVsOften"))
        m_bRecentVsOften = config->readBoolEntry("RecentVsOften");

    m_Info.clear();
    if (config->hasKey("RecentAppsStat")){
        QStringList RecentApps = config->readListEntry("RecentAppsStat");
        QValueList<QString>::ConstIterator it;
        for (it = RecentApps.begin(); it != RecentApps.end(); ++it){
            const QString strInfo = (*it).stripWhiteSpace();
            if (!strInfo.isEmpty()) {
                int nCount;
                long lTime;

                int nFind = strInfo.find(" ");
                if (nFind > 0) {
                    QString strCount(strInfo.left(nFind));
                    nCount = strCount.toInt();

                    int nFind2 = strInfo.find(" ", nFind+1);
                    if (nFind2 > 0) {
                        QString strTime(strInfo.mid(nFind+1, nFind2-nFind-1));
                        lTime = strTime.toLong();

                        QString szPath = strInfo.right(strInfo.length() - nFind2 - 1);
                        m_Info.insert(szPath, new RecentlyLaunchedAppInfo(nCount, (time_t)lTime));
                    }
                }
            }
        }
    }
    m_bInitialised = true;
}

void RecentlyLaunchedApps::save()
{
    KConfig *config = KGlobal::config();
    config->setGroup("menus");

    QStringList RecentApps;
    if (!m_Info.isEmpty()) {
        QDictIterator<RecentlyLaunchedAppInfo> it(m_Info);
        for (RecentlyLaunchedAppInfo *pInfo = it.toFirst(); pInfo; ++it, pInfo = it.current()){
            QString strPath(it.currentKey());
            QString strInfo;
            strInfo.sprintf("%d %ld ", pInfo->getLaunchCount(), (long)pInfo->getLastLaunchTime());
            strInfo += strPath;

            RecentApps.append(strInfo);
        }
    }
    config->writeEntry("RecentAppsStat", RecentApps);
    config->sync();
}

void RecentlyLaunchedApps::appLaunched(const QString & strApp)
{
//    kdDebug() << "RecentlyLaunchedApps::appLaunched" << endl;
    RecentlyLaunchedAppInfo *pInfo = m_Info.find(strApp);
    if (pInfo) {
        pInfo->increaseLaunchCount();
        pInfo->setLastLaunchTime(time(0));
    }
    else {
        m_Info.insert(strApp, new RecentlyLaunchedAppInfo( 1, time(0)));
        checkOverlimit();
    }
}

void RecentlyLaunchedApps::checkOverlimit()
{
    if ((int)m_Info.count() < m_nMaxEntries) return;

    QStringList strsToClear;
    QDictIterator<RecentlyLaunchedAppInfo> it(m_Info);

    for( int i = m_Info.count() - m_nMaxEntries; i > 0; i--)
    {
        // not most effective but easiest
        QString strFoundPath;
        RecentlyLaunchedAppInfo *pFoundInfo = 0;
        for (RecentlyLaunchedAppInfo *pInfo = it.toFirst(); pInfo; ++it, pInfo = it.current())
        {
            QString strPath(it.currentKey());
            if (strFoundPath != strPath && strsToClear.find(strPath) == strsToClear.end()) {
                if (!pFoundInfo) {
                    pFoundInfo = pInfo;
                    strFoundPath = strPath;
                }
                else if (m_bRecentVsOften) {
                    if (pFoundInfo->getLastLaunchTime() >= pInfo->getLastLaunchTime()) {
                        pFoundInfo = pInfo;
                        strFoundPath = strPath;
                    }
                }
                else if (pFoundInfo->getLaunchCount() > pInfo->getLaunchCount() ||
                         (pFoundInfo->getLaunchCount() == pInfo->getLaunchCount() &&
                          pFoundInfo->getLastLaunchTime() >= pInfo->getLastLaunchTime()))
                {
                    pFoundInfo = pInfo;
                    strFoundPath = strPath;
                }
            }
        }
        strsToClear.append(strFoundPath);
    }
    if (!strsToClear.isEmpty()) {
        QValueList<QString>::ConstIterator it;
        for (it = strsToClear.begin(); it != strsToClear.end(); ++it)
            m_Info.remove(*it);
    }
}

void RecentlyLaunchedApps::getRecentApps(QStringList & RecentApps)
{
    // not most effective but easiest
    QDictIterator<RecentlyLaunchedAppInfo> it(m_Info);

    RecentApps.clear();

    RecentlyLaunchedAppInfo MinInfo(0, 0);
    QString strFoundPath;
    for (int i = 0; i < m_nNumVisible && i < (int)it.count(); i++)
    {
        RecentlyLaunchedAppInfo *pFoundInfo = &MinInfo;
        for (RecentlyLaunchedAppInfo *pInfo = it.toFirst(); pInfo; ++it, pInfo = it.current())
        {
            QString strPath(it.currentKey());
            if (strFoundPath != strPath && RecentApps.find(strPath) == RecentApps.end()) {
                if (m_bRecentVsOften){
                    if (pFoundInfo->getLastLaunchTime() <= pInfo->getLastLaunchTime()){
                        pFoundInfo = pInfo;
                        strFoundPath = strPath;
                    }
                }
                else if (pFoundInfo->getLaunchCount() < pInfo->getLaunchCount() ||
                         (pFoundInfo->getLaunchCount() == pInfo->getLaunchCount() &&
                          pFoundInfo->getLastLaunchTime() <= pInfo->getLastLaunchTime()))
                {
                    pFoundInfo = pInfo;
                    strFoundPath = strPath;
                }
            }
        }
        if (pFoundInfo != &MinInfo) // should always go through
            RecentApps.append(strFoundPath);
    }
}

void RecentlyLaunchedApps::removeItem(const QString &strName)
{
    if (!strName.isEmpty())
        m_Info.remove(strName);
}
