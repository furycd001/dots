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

#ifndef UIC_H
#define UIC_H
#include <qdom.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qmap.h>

#include <qtextstream.h>

class Uic : public Qt
{
public:
    Uic( QTextStream& out, QDomDocument doc, bool decl, bool subcl, const QString &trm, const QString& subclname );

    void createFormDecl( const QDomElement &e );
    void createFormImpl( const QDomElement &e );

    void createSubDecl( const QDomElement &e, const QString& subclname );
    void createSubImpl( const QDomElement &e, const QString& subclname );

    void createObjectDecl( const QDomElement& e );
    QString createObjectImpl( const QDomElement &e, const QString& parentClass, const QString& parent, const QString& layout = QString::null );
    QString createLayoutImpl( const QDomElement &e, const QString& parentClass, const QString& parent, const QString& layout = QString::null );
    QString createObjectInstance( const QString& objClass, const QString& parent, const QString& objName );
    QString createSpacerImpl( const QDomElement &e, const QString& parentClass, const QString& parent, const QString& layout = QString::null );
    void createExclusiveProperty( const QDomElement & e, const QString& exclusiveProp );
    QString createListBoxItemImpl( const QDomElement &e, const QString &parent );
    QString createIconViewItemImpl( const QDomElement &e, const QString &parent );
    QString createListViewColumnImpl( const QDomElement &e, const QString &parent );
    QString createListViewItemImpl( const QDomElement &e, const QString &parent,
				    const QString &parentItem );
    void createColorGroupImpl( const QString& cg, const QDomElement& e );
    QColorGroup loadColorGroup( const QDomElement &e );

    QString getClassName( const QDomElement& e );
    QString getObjectName( const QDomElement& e );
    QString getLayoutName( const QDomElement& e );
    QString getInclude( const QString& className );

    QString setObjectProperty( const QString& objClass, const QString& obj, const QString &prop, const QDomElement &e, bool stdset );

    QString registerObject( const QString& name );
    QString registeredName( const QString& name );
    bool isObjectRegistered( const QString& name );
    QStringList unique( const QStringList& );


private:
    void registerLayouts ( const QDomElement& e );

    QTextStream& out;
    QStringList objectNames;
    QMap<QString,QString> objectMapper;
    QString indent;
    QStringList tags;
    QStringList layouts;
    QString formName;
    QString lastItem;
    QString trmacro;

    struct Buddy
    {
	Buddy( const QString& k, const QString& b )
	    : key( k ), buddy( b ) {}
	Buddy(){}; // for valuelist
	QString key;
	QString buddy;
	bool operator==( const Buddy& other ) const
	    { return (key == other.key); }
    };
    struct CustomInclude
    {
	QString header;
	QString location;
    };
    QValueList<Buddy> buddies;

    QStringList layoutObjects;
    bool isLayout( const QString& name ) const;

    uint item_used : 1;
    uint cg_used : 1;
    uint pal_used : 1;

    QString nameOfClass;
    QString pixmapLoaderFunction;

};

#endif
