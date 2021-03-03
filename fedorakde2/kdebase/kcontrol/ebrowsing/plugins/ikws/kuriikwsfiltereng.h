/*  This file is part of the KDE project
    Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
    Internet Keywords support (C) 1999 Yves Arrouye <yves@realnames.com>
    Current maintainer Yves Arrouye <yves@realnames.com>

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

/* $Id: kuriikwsfiltereng.h,v 1.6 2001/02/07 20:02:08 deller Exp $ */

#ifndef __KURIIKWSFILTERENGINE_H__
#define __KURIIKWSFILTERENGINE_H__

#include <qvaluelist.h>
#include <qstringlist.h>

#include <kservice.h>

class KURL;

class KURISearchFilterEngine
{

public:

    struct IKWSEntry
    {
	  QString m_strName;
	  QString m_strQuery;
	  QString m_strQueryWithSearch;
	  QString m_strCharset;
    };

    KURISearchFilterEngine();
    ~KURISearchFilterEngine() {};

    QCString name() const;
    QValueList<IKWSEntry> ikwsEngines() const { return m_lstInternetKeywordsEngine; }

    QString searchQuery( const KURL& ) const;
    QString ikwsQuery( const KURL& ) const;

    IKWSEntry ikwsEntryByName(const QString & name) const;

    bool verbose() const { return m_bVerbose; }

    void loadConfig();

    static void incRef();
    static void decRef();
    static KURISearchFilterEngine *self();

protected:
    QString formatResult( const QString& query, const QString& cset1, const QString& cset2, const QString& url, bool ) const;

private:

    bool m_bSearchKeywordsEnabled;

    bool m_bInternetKeywordsEnabled;
    QValueList<IKWSEntry> m_lstInternetKeywordsEngine;

    IKWSEntry m_currInternetKeywordsEngine;
    QString m_searchFallback;

    bool m_bVerbose;

    static KURISearchFilterEngine *s_pSelf;
    static unsigned long s_refCnt;
};

#endif
