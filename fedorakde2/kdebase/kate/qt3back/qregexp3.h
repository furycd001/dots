/****************************************************************************
** $Id: qregexp3.h,v 1.3 2001/04/05 22:39:04 sdmanson Exp $
**
** Definition of QRegExp class
**
** Created : 950126
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the tools module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QREGEXP3_H
#define QREGEXP3_H
#ifndef QT_H
#include "qstringlist.h"
#endif // QT_H


#if QT_VERSION >=300
#include <qregexp.h>
#else
class QRegExpEngine;
struct QRegExpPrivate;

class Q_EXPORT QRegExp3
{
public:
    QRegExp3();
    QRegExp3( const QString& pattern, bool caseSensitive = TRUE,
	     bool wildcard = FALSE );
    QRegExp3( const QRegExp3& rx );
    ~QRegExp3();
    QRegExp3& operator=( const QRegExp3& rx );

    bool operator==( const QRegExp3& rx ) const;
    bool operator!=( const QRegExp3& rx ) const { return !operator==( rx ); }

    bool isEmpty() const;
    bool isValid() const;
    QString pattern() const;
    void setPattern( const QString& pattern );
    bool caseSensitive() const;
    void setCaseSensitive( bool sensitive );
#ifndef QT_NO_REGEXP_WILDCARD
    bool wildcard() const;
    void setWildcard( bool wildcard );
#endif
    bool minimal() const;
    void setMinimal( bool minimal );

    bool exactMatch( const QString& str );
    bool exactMatch( const QString& str ) const;
#ifndef QT_NO_COMPAT
    int match( const QString& str, int index, int *len = 0,
	       bool indexIsStart = TRUE );
#endif
    int search( const QString& str, int start = 0 );
    int search( const QString& str, int start = 0 ) const;
// QChar versions
#ifdef QCHAR_SUPPORT
    int search(const QChar *str,int start=0);
    int search(const QChar *str,int start=0) const;
    int searchRev(const QChar *str,int start=-1);
    int searchRev(const QChar *str,int start=-1) const ;
    bool exactMatch(const QChar *str);
    bool exactMatch(const QChar *str) const;
// end QChar versions	
#endif
    int searchRev( const QString& str, int start = -1 );
    int searchRev( const QString& str, int start = -1 ) const;
    int matchedLength();
#ifndef QT_NO_REGEXP_CAPTURE
    QStringList capturedTexts();
    QString cap( int nth = 0 );
    int pos( int nth = 0 );
#endif

private:
    void compile( bool caseSensitive );

    QRegExpEngine *eng;
    QRegExpPrivate *priv;
};
#endif // QT_VERSION >= 300
#endif // QREGEXP_H
