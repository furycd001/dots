/*
    This file is part of the KDE File Manager

    Copyright (C) 1998-2000 Waldo Bastian (bastian@kde.org)
    Copyright (C) 2000      Dawit Alemayehu (adawit@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

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
// KDE File Manager -- HTTP Cookie Dialogs
// $Id$

#ifndef _KCOOKIEWIN_H_
#define _KCOOKIEWIN_H_

#include <qgroupbox.h>

#include <kdialog.h>
#include "kcookiejar.h"

class QLineEdit;
class QPushButton;
class QVButtonGroup;
class KURLLabel;

class KCookieDetail : public QGroupBox
{
    Q_OBJECT

public :
    KCookieDetail( KHttpCookie* cookie, int cookieCount, QWidget *parent=0,
                   const char *name=0 );
    ~KCookieDetail();

private :
    QLineEdit*   m_name;
    QLineEdit*   m_value;
    QLineEdit*   m_expires;
    QLineEdit*   m_domain;
    QLineEdit*   m_path;
    QLineEdit*   m_secure;

    KHttpCookie* m_cookie;
    KHttpCookie* m_cookie_orig;

private slots:
    void slotNextCookie();
};

class KCookieWin : public KDialog
{
    Q_OBJECT

public :
    KCookieWin( QWidget *parent, KHttpCookie* cookie, int defaultButton=0,
                bool showDetails=false );
    ~KCookieWin();

    KCookieAdvice advice( KCookieJar *cookiejar, KHttpCookie* cookie );

private :
    QPushButton*   m_button;
    QVButtonGroup* m_btnGrp;
    KCookieDetail* m_detailView;
    bool m_showDetails;

private slots:
    void slotCookieDetails();
};
#endif
