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

#ifndef __recentapps_h__
#define __recentapps_h__

#include <qdict.h>

class RecentlyLaunchedApps;

class RecentlyLaunchedAppInfo
{
    friend class RecentlyLaunchedApps;

protected:
    RecentlyLaunchedAppInfo(int nLaunchCount, time_t lastLaunchTime)
    {
        m_nLaunchCount = nLaunchCount;
        m_LastLaunchTime = lastLaunchTime;
    }

    int getLaunchCount() { return m_nLaunchCount; };
    time_t getLastLaunchTime() { return m_LastLaunchTime; };
    void increaseLaunchCount() { m_nLaunchCount++; };
    void setLaunchCount(int nLaunchCount) { m_nLaunchCount = nLaunchCount; };
    void setLastLaunchTime(time_t lastLaunch) { m_LastLaunchTime = lastLaunch; };

private:
    int m_nLaunchCount;
    time_t m_LastLaunchTime;
};

class RecentlyLaunchedApps
{
public:
    RecentlyLaunchedApps();
    void init();
    void save();
    void appLaunched(const QString & strApp);
    void getRecentApps(QStringList & RecentApps);
    void setMaxEntries(int nMax) { m_nMaxEntries = nMax; };
    void removeItem(const QString &strName);

    int m_nNumMenuItems;
    bool m_bNeedToUpdate;

private:
    void checkOverlimit();

    int m_nMaxEntries;
    QDict<RecentlyLaunchedAppInfo> m_Info;
    bool m_bRecentVsOften;
    int m_nNumVisible;
    bool m_bInitialised;
};

#endif
