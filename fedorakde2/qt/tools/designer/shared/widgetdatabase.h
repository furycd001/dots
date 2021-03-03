/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef WIDGETDATABASE_H
#define WIDGETDATABASE_H

#include <qiconset.h>
#include <qstring.h>
#include <qstringlist.h>

struct WidgetDatabaseRecord
{
    WidgetDatabaseRecord();
    ~WidgetDatabaseRecord();
    QString iconSet, name, group, toolTip, whatsThis, includeFile;
    uint isContainer : 1;
    QIconSet *icon;
    int nameCounter;
};

class WidgetDatabase : public Qt
{
public:
    WidgetDatabase();
    static void setupDataBase();

    static int count();
    static int startCustom();

    static QIconSet iconSet( int id );
    static QString className( int id );
    static QString group( int id );
    static QString toolTip( int id );
    static QString whatsThis( int id );
    static QString includeFile( int id );
    static bool isContainer( int id );

    static int idFromClassName( const QString &name );
    static QString createWidgetName( int id );

    static WidgetDatabaseRecord *at( int index );
    static void insert( int index, WidgetDatabaseRecord *r );
    static void append( WidgetDatabaseRecord *r );

    static QString widgetGroup( const QString &g );
    static QString widgetGroup( int i );
    static int numWidgetGroups();
    static bool isGroupVisible( const QString &g );

    static int addCustomWidget( WidgetDatabaseRecord *r );
    static bool isCustomWidget( int id );

    static bool isWhatsThisLoaded();
    static void loadWhatsThis( const QString &docPath );
    
};

#endif
