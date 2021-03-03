/**
 * kcookiesmanagement.h - Cookies manager
 *
 * Copyright 2000-2001 Marco Pinelli <pinmc@orion.it>
 *
 * Contributors:
 * Copyright (c) 2000-2001 Dawit Alemayehu <adawit@kde.org>
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

#ifndef __KCOOKIESMANAGEMENT_H
#define __KCOOKIESMANAGEMENT_H

#include <qdict.h>
#include <qstringlist.h>
#include <qlistview.h>

#include <kcmodule.h>

class QPushButton;
class QLineEdit;
class KListView;
class DCOPClient;
struct CookieProp;

class CookieListViewItem : public QListViewItem
{
public:
    CookieListViewItem(QListView *parent, QString dom);
    CookieListViewItem(QListViewItem *parent, CookieProp *cookie);
    ~CookieListViewItem();

    QString domain() const { return mDomain; }
    CookieProp* cookie() const { return mCookie; }
    CookieProp* leaveCookie();
    void setCookiesLoaded() { mCookiesLoaded = true; }
    bool cookiesLoaded() const { return mCookiesLoaded; }
    virtual QString text(int f) const;

private:
    void init( CookieProp* cookie,
               QString domain = QString::null,
               bool cookieLoaded=false );
    CookieProp *mCookie;
    QString mDomain;
    bool mCookiesLoaded;
};

class KCookiesManagement : public KCModule
{
    Q_OBJECT

public:
    KCookiesManagement(QWidget *parent = 0L, const char *name = 0L);
    ~KCookiesManagement();

    virtual void load();
    virtual void save();
    virtual void defaults();
    virtual QString quickHelp() const;

private slots:
    void changed();
    void deleteCookie();
    void deleteAllCookies();
    void getDomains();
    void getCookies(QListViewItem*);
    void showCookieDetails(QListViewItem*);

private:
    bool getCookieDetails(CookieProp *cookie);
    bool checkCookiejarStatus();
    void clearCookieDetails();

    QLineEdit* le_name;
    QLineEdit* le_value;
    QLineEdit* le_domain;
    QLineEdit* le_path;
    QLineEdit* le_expires;
    QLineEdit* le_isSecure;

    KListView* lv_cookies;
    QGroupBox* grp_details;
    QPushButton* btn_delete;
    QPushButton* btn_reload;
    QPushButton* btn_deleteAll;

    DCOPClient* m_dcopClient;
    bool m_bDeleteAll;

    QStringList deletedDomains;
    typedef QList<CookieProp> CookiePropList;
    QDict<CookiePropList> deletedCookies;
};

#endif // __KCOOKIESMANAGEMENT_H
