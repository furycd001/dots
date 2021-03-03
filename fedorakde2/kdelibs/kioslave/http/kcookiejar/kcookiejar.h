/*
    This file is part of the KDE File Manager

    Copyright (C) 1998 Waldo Bastian (bastian@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License 
    version 2 as published by the Free Software Foundation.

    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this library; see the file COPYING. If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/
//----------------------------------------------------------------------------
//
// KDE File Manager -- HTTP Cookies
// $Id$

#ifndef KCOOKIEJAR_H
#define KCOOKIEJAR_H

#include <qstring.h>
#include <qstringlist.h>
#include <qdict.h>
#include <qlist.h>
#include <time.h>

class KCookieJar;
class KConfig;

class KCookieList;
class KCookie;
typedef KCookie *KCookiePtr;

enum KCookieAdvice { 
    KCookieDunno=0, 
    KCookieAccept, 
    KCookieReject, 
    KCookieAsk 
};

class KCookie 
{
    friend KCookieJar;
    friend KCookieList;

protected:
    QString mHost;
    QString mDomain;
    QString mPath;
    QString mName;
    QString mValue;
    time_t  mExpireDate;
    int     mProtocolVersion;

    KCookiePtr nextCookie;

    QString cookieStr(void);
public:
    KCookie(const QString &_host=QString::null, 
            const QString &_domain=QString::null, 
            const QString &_path=QString::null,
            const QString &_name=QString::null, 
            const QString &_value=QString::null, 
            time_t _expireDate=0,
            int _protocolVersion=0);
            
    QString domain(void) { return mDomain; } 
    QString host(void) { return mHost; }
    QString path(void) { return mPath; }
    QString name(void) { return mName; }
    QString value(void) { return mValue; }
    time_t  expireDate(void) { return mExpireDate; }
    int     protocolVersion(void) { return mProtocolVersion; }
    
    bool    isExpired(time_t currentDate);
    bool    match(const QString &domain, const QString &fqdn, 
                  const QString &path);

    KCookiePtr next() { return nextCookie; }
};

class KCookieList : public QList<KCookie>
{
public:
    KCookieList() : QList<KCookie>(), advice( KCookieDunno ) 
    { setAutoDelete(true); }
    virtual ~KCookieList() { }

    virtual int compareItems( KCookie * item1, KCookie * item2);

    KCookieAdvice getAdvice(void) { return advice; }
    void setAdvice(KCookieAdvice _advice) { advice = _advice; }

private:
    KCookieAdvice advice;
};

class KCookieJar
{
public:
    /**
     * Constructs a new cookie jar
     *
     * One jar should be enough for all cookies.
     */
    KCookieJar();

    /**
     * Destructs the cookie jar
     *
     * Poor little cookies, they will all be eaten by the cookie monster!
     */
    ~KCookieJar();

    /**
     * Returns whether the cookiejar has been changed
     */
    bool changed() { return cookiesChanged || configChanged; }

    /**
     * Store all the cookies in a safe(?) place
     */
    bool saveCookies(const QString &_filename);

    /**
     * Load all the cookies from file and add them to the cookie jar.
     */
    bool loadCookies(const QString &_filename);

    /**
     * Save the cookie configuration
     */
    void saveConfig(KConfig *_config);

    /**
     * Load the cookie configuration
     */
    void loadConfig(KConfig *_config);

    /**
     * Looks for cookies in the cookie jar which are appropriate for _url.
     * Returned is a string containing all appropriate cookies in a format 
     * which can be added to a HTTP-header without any additional processing.
     */                
    QString findCookies(const QString &_url);

    /**
     * This function parses cookie_headers and returns a linked list of
     * valid KCookie objects for all cookies found in cookie_headers.
     * If no cookies could be found 0 is returned.
     *
     * cookie_headers should be a concatenation of all lines of a HTTP-header
     * which start with "Set-Cookie". The lines should be separated by '\n's.
     */
    KCookiePtr makeCookies(const QString &_url, const QCString &cookie_headers);

    /**
     * This function hands a KCookie object over to the cookie jar.
     * 
     * On return cookiePtr is set to 0.
     */
    void addCookie(KCookiePtr &cookiePtr);

    /**
     * This function advices whether a single KCookie object should 
     * be added to the cookie jar.
     *
     * Possible return values are:
     *     - KCookieAccept, the cookie should be added
     *     - KCookieReject, the cookie should not be added
     *     - KCookieAsk, the user should decide what to do 
     */
    KCookieAdvice cookieAdvice(KCookiePtr cookiePtr);

    /**
     * This function gets the advice for all cookies originating from 
     * _domain.    
     *
     *     - KCookieDunno, no specific advice for _domain
     *     - KCookieAccept, accept all cookies for _domain
     *     - KCookieReject, reject all cookies for _domain 
     *     - KCookieAsk, the user decides what to do with cookies for _domain
     */
    KCookieAdvice getDomainAdvice(const QString &_domain);

    /**
     * This function sets the advice for all cookies originating from 
     * _domain.    
     *
     * _advice can have the following values:
     *     - KCookieDunno, no specific advice for _domain
     *     - KCookieAccept, accept all cookies for _domain
     *     - KCookieReject, reject all cookies for _domain 
     *     - KCookieAsk, the user decides what to do with cookies for _domain
     */
    void setDomainAdvice(const QString &_domain, KCookieAdvice _advice);

    /**
     * This function sets the advice for all cookies originating from 
     * the same domain as _cookie    
     *
     * _advice can have the following values:
     *     - KCookieDunno, no specific advice for _domain
     *     - KCookieAccept, accept all cookies for _domain
     *     - KCookieReject, reject all cookies for _domain 
     *     - KCookieAsk, the user decides what to do with cookies for _domain
     */
    void setDomainAdvice(KCookiePtr _cookie, KCookieAdvice _advice);

    /**
     * This function sets the global advice for cookies 
     *
     * _advice can have the following values:
     *     - KCookieAccept, accept cookies
     *     - KCookieReject, reject cookies 
     *     - KCookieAsk, the user decides what to do with cookies
     *
     * The global advice is used if the domain has no advice set.
     */
    void setGlobalAdvice(KCookieAdvice _advice);

    /**
     * Get a list of all domains known to the cookie jar.
     * A domain is known to the cookie jar if:
     *     - It has a cookie originating from the domain
     *     - It has a specific advice set for the domain
     */
    const QStringList *getDomainList();    
    
    /**
     * Get a list of all cookies in the cookie jar originating from _domain.
     */
    const KCookieList *getCookieList(const QString &_domain);

    /**
     * Remove & delete a cookie from the jar.
     *
     * cookiePtr should be one of the entries in a KCookieList.
     * Update your KCookieList by calling getCookieList after 
     * calling this function.
     */
    void eatCookie(KCookiePtr cookiePtr);

    /**
     * Parses _url and returns the FQDN (_fqdn) 
     * as well as the domain name without the hostname (_domain).
     */
    static bool extractDomain(const QString &_url,
                              QString &_fqdn,
                              QString &_domain,
                              QString &_path);

protected:  
    QDict<KCookieList> cookieDomains;

    QStringList domainList;

    KCookieAdvice globalAdvice;
    bool configChanged;
    bool cookiesChanged;
};

#endif
