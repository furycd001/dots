/*  This file is part of the KDE project
    Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
    Copyright (C) 2000 Yves Arrouye <yves@realnames.com>

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

#include <unistd.h>

#include <qtextcodec.h>

#include <kurl.h>
#include <kdebug.h>
#include <kprotocolinfo.h>
#include <kconfig.h>
#include <ksimpleconfig.h>
#include <kapp.h>
#include <kstddirs.h>

#include "kuriikwsfiltereng.h"
#include "searchprovider.h"

unsigned long KURISearchFilterEngine::s_refCnt = 0;
KURISearchFilterEngine *KURISearchFilterEngine::s_pSelf = 0L;

#define IKW_KEY         "Internet Keywords"
#define IKW_SUFFIX      " " IKW_KEY
#define IKW_REALNAMES	"RealNames"

KURISearchFilterEngine::KURISearchFilterEngine()
{
    loadConfig();
}

QString KURISearchFilterEngine::searchQuery( const KURL &url ) const
{
    if ( m_bSearchKeywordsEnabled )
    {
        QString key, search = url.url();
        int pos = search.find(':');
        if ( pos > -1 )
         key = search.left(pos);

        if ( key.isEmpty() || KProtocolInfo::isKnownProtocol( key ) )
            return QString::null;

        SearchProvider *provider = SearchProvider::findByKey(key);

        if ( provider )
            return formatResult( provider->query(), provider->charset(),
                                 QString::null, search.mid(pos+1),
                                 url.isMalformed() );
    }
    return QString::null;
}


QString KURISearchFilterEngine::ikwsQuery( const KURL& url ) const
{
    if (m_bInternetKeywordsEnabled)
	{
	    QString key;
	    QString _url = url.url();
	    if( url.isMalformed() && _url[0] == '/' ) {
		key = QString::fromLatin1( "file" );
	    } else {
		key = url.protocol();
	    }

	    if( KProtocolInfo::isKnownProtocol(key) ) {
		return QString::null;
	    }

            SearchProvider *fallback = SearchProvider::findByDesktopName(m_searchFallback);
            if (fallback)
            {
                QString search = fallback->query();
                /*
                * As a special case, if there is a question mark
                * at the beginning of the query, we'll force the
                * use of the search fallback without going through
                * the Internet Keywords engine.
                *
                */

                QRegExp question("^[ \t]*\\?[ \t]*");
                if (url.isMalformed() && _url.find(question) == 0) {
                        _url = _url.replace(question, "");
                        return formatResult(search, fallback->charset(), QString::null, _url, true);
                } else {
                        int pct = m_currInternetKeywordsEngine.m_strQueryWithSearch.find("\\|");
                        if (pct >= 0)
                        {
                                search = KURL::encode_string( search );
                                QString res = m_currInternetKeywordsEngine.m_strQueryWithSearch;
                                return formatResult( res.replace(pct, 2, search), fallback->charset(), QString::null, _url, url.isMalformed() );
                        }
                }
            }

	    return formatResult( m_currInternetKeywordsEngine.m_strQuery, m_currInternetKeywordsEngine.m_strCharset, QString::null, _url, url.isMalformed() );
	}
    return QString::null;
}

KURISearchFilterEngine::IKWSEntry KURISearchFilterEngine::ikwsEntryByName(const QString &name) const
{
    QValueList<IKWSEntry>::ConstIterator it = m_lstInternetKeywordsEngine.begin();
    QValueList<IKWSEntry>::ConstIterator end = m_lstInternetKeywordsEngine.end();
    for (; it != end; ++it)
	{
	    if ((*it).m_strName == name)
		return *it;
	}
    return IKWSEntry();
}

QCString KURISearchFilterEngine::name() const
{
    return "kuriikwsfilter";
}

void KURISearchFilterEngine::incRef()
{
    s_refCnt++;
}

void KURISearchFilterEngine::decRef()
{
    s_refCnt--;
    if ( s_refCnt == 0 && s_pSelf )
	{
	    delete s_pSelf;
	    s_pSelf = 0;
	}
}

KURISearchFilterEngine* KURISearchFilterEngine::self()
{
    if (!s_pSelf)
	{
	    if ( s_refCnt == 0 )
  	        s_refCnt++; //someone forgot to call incRef
	    s_pSelf = new KURISearchFilterEngine;
	}
    return s_pSelf;
}

QString KURISearchFilterEngine::formatResult( const QString& query,
                                              const QString& cset1,
                                              const QString& cset2,
                                              const QString& url,
                                              bool isMalformed ) const
{
    // Substitute the variable part we find in the query.
    if (!query.isEmpty())
	{
	    QString newurl = query;
	    int pct;

	    // Create a codec for the desired encoding so that we can
	    // transcode the user's "url".
	    QString cseta = cset1;
	    if (cseta.isEmpty()) {
		cseta = "iso-8859-1";
	    }
	    QTextCodec *csetacodec = QTextCodec::codecForName(cseta.latin1());
	    if (!csetacodec) {
		cseta = "iso-8859-1";
		csetacodec = QTextCodec::codecForName(cseta.latin1());
	    }

	    // Substitute the charset indicator for the query.
	    if ((pct = newurl.find("\\2")) >= 0) {
		newurl = newurl.replace(pct, 2, cseta);
	    }

	    // Substitute the charset indicator for the fallback query.
	    if ((pct = newurl.find("\\3")) >= 0) {
		QString csetb = cset2;
		if (csetb.isEmpty()) {
		    csetb = "iso-8859-1";
		}
		newurl = newurl.replace(pct, 2, csetb);
	    }

        QString userquery = csetacodec->fromUnicode(url);
        int space_pos;
        while( (space_pos=userquery.find('+')) != -1 )
        userquery=userquery.replace( space_pos, 1, "%2B" );

        while( (space_pos=userquery.find(' ')) != -1 )
        userquery=userquery.replace( space_pos, 1, "+" );

        if ( isMalformed )
        userquery = KURL::encode_string(userquery);

        if ((pct = newurl.find("\\1")) >= 0)
        newurl = newurl.replace(pct, 2, userquery);

        if ( m_bVerbose )
        kdDebug(7023) << "(" << getpid() << ") filtered " << url << " to " << newurl << "\n";

        return newurl;
    }
    return QString::null;
}

void KURISearchFilterEngine::loadConfig()
{
    // Migrate from the old format,
    // this block should remain until we can assume "every"
    // user has upgraded to a KDE version that contains the
    // sycoca based search provider configuration (malte)
    {
        KSimpleConfig oldConfig(kapp->dirs()->saveLocation("config") + QString(name()) + "rc");
        oldConfig.setGroup("General");
        if (oldConfig.hasKey("SearchEngines"))
        {
            // User has an old config file in his local config dir
            kdDebug(7023) << "Migrating config file to .desktop files..." << endl;
            QString fallback = oldConfig.readEntry("InternetKeywordsSearchFallback");
            QStringList engines = oldConfig.readListEntry("SearchEngines");
            for (QStringList::ConstIterator it = engines.begin(); it != engines.end(); ++it)
            {
                if (!oldConfig.hasGroup(*it + " Search"))
                    continue;
                oldConfig.setGroup(*it + " Search");
                QString query = oldConfig.readEntry("Query");
                QStringList keys = oldConfig.readListEntry("Keys");
                QString charset = oldConfig.readEntry("Charset");
                oldConfig.deleteGroup(*it + " Search");
                QString name;
                for (QStringList::ConstIterator key = keys.begin(); key != keys.end(); ++key)
                {
                    // take the longest key as name for the .desktop file
                    if ((*key).length() > name.length())
                        name = *key;
                }
                if (*it == fallback)
                    fallback = name;
                SearchProvider *provider = SearchProvider::findByKey(name);
                if (provider)
                {
                    // If this entry has a corresponding global entry
                    // that comes with KDE's default configuration,
                    // compare both and if thei're equal, don't
                    // create a local copy
                    if (provider->name() == *it
                        && provider->query() == query
                        && provider->keys() == keys
                        && (provider->charset() == charset || (provider->charset().isEmpty() && charset.isEmpty())))
                    {
                        kdDebug(7023) << *it << " is unchanged, skipping" << endl;
                        continue;
                    }
                    delete provider;
                }
                KSimpleConfig desktop(kapp->dirs()->saveLocation("services", "searchproviders/") + name + ".desktop");
                desktop.setGroup("Desktop Entry");
                desktop.writeEntry("Type", "Service");
                desktop.writeEntry("ServiceTypes", "SearchProvider");
                desktop.writeEntry("Name", *it);
                desktop.writeEntry("Query", query);
                desktop.writeEntry("Keys", keys);
                desktop.writeEntry("Charset", charset);
                kdDebug(7023) << "Created searchproviders/" << name << ".desktop for " << *it << endl;
            }
            oldConfig.deleteEntry("SearchEngines", false);
            oldConfig.setGroup("General");
            oldConfig.writeEntry("InternetKeywordsSearchFallback", fallback);
            kdDebug(7023) << "...completed" << endl;
        }
    }

    kdDebug(7023) << "(" << getpid() << ") Keywords Engine: Loading config..." << endl;
    // First empty any current config we have.
    m_lstInternetKeywordsEngine.clear();

    // Load the config.
    KConfig config( name() + "rc", false, false );
    QStringList engines;
    QString selIKWSEngine;
    config.setGroup( "General" );

    m_bInternetKeywordsEnabled = config.readBoolEntry("InternetKeywordsEnabled", true);
    selIKWSEngine = config.readEntry("InternetKeywordsSelectedEngine", IKW_REALNAMES);
    m_searchFallback = config.readEntry("InternetKeywordsSearchFallback");

    m_bVerbose = config.readBoolEntry("Verbose");
    m_bSearchKeywordsEnabled = config.readBoolEntry("SearchEngineShortcutsEnabled", true);

    kdDebug(7023) << "(" << getpid() << ") Internet Keyword Enabled: " << m_bInternetKeywordsEnabled << endl;
    kdDebug(7023) << "(" << getpid() << ") Selected IKWS Engine(s): " << selIKWSEngine << endl;
    kdDebug(7023) << "(" << getpid() << ") Internet Keywords Fallback Search Engine: " << m_searchFallback << endl;

    engines = config.readListEntry("InternetKeywordsEngines");
    QStringList::ConstIterator gIt = engines.begin();
    QStringList::ConstIterator gEnd = engines.end();
    for (; gIt != gEnd; ++gIt) {
	QString grpName = *gIt + IKW_SUFFIX;
	if (config.hasGroup(grpName)) {
	    config.setGroup( grpName );
	    IKWSEntry e;
	    e.m_strName = *gIt;
	    e.m_strQuery = config.readEntry("Query");
	    e.m_strQueryWithSearch = config.readEntry("QueryWithSearch");
	    e.m_strCharset = config.readEntry("Charset");
	    m_lstInternetKeywordsEngine.append(e);
	    if (selIKWSEngine == (e.m_strName)) {
		m_currInternetKeywordsEngine = e;
	    }
	}
    }

    IKWSEntry rn = ikwsEntryByName(IKW_REALNAMES);
    if (rn.m_strName.isEmpty())	{
	rn.m_strName = IKW_REALNAMES;
	rn.m_strQuery = QString::fromLatin1("https://duckduckgo.com/?q=\\1");
	rn.m_strCharset = "utf-8";
	//rn.m_strQueryWithSearch = QString::fromLatin1("http://navigation.realnames.com/resolver.dll?"
	//					      "action=navigation&realname=\\1&charset=\\2&providerid=180&fallbackuri=\\|");
	if (rn.m_strName == selIKWSEngine)
	    m_currInternetKeywordsEngine = rn;

	m_lstInternetKeywordsEngine.append(rn);
    }
}

