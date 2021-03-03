/*
 *  khc_navigatoritem.cc - part of the KDE Help Center
 *
 *  Copyright (C) 1999 Matthias Elter (me@kde.org)
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

#include <qlistview.h>
#include <qstring.h>
#include <qfile.h>

#include <kapp.h>
#include <ksimpleconfig.h>

#include "khc_navigatoritem.h"
#include "khc_factory.h"
#include <kstddirs.h>
#include <kglobal.h>
#include <kiconloader.h>

khcNavigatorItem::khcNavigatorItem(QListView* parent, const QString& _text, const QString& _miniicon)
    : QListViewItem(parent)
{
    init(_text, _miniicon);
}

khcNavigatorItem::khcNavigatorItem(QListViewItem* parent, const QString& _text, const QString& _miniicon)
    : QListViewItem(parent)
{
    init(_text, _miniicon);
}

void khcNavigatorItem::init(const QString& _text, const QString& _miniicon)
{
    name = _text;
    miniicon = _miniicon;
    
    setText(0, name);
    setPixmap( 0, SmallIcon(miniicon, 0, 0, KHCFactory::instance()));

    url = QString::null;
}

bool khcNavigatorItem::readKDElnk ( const QString &filename )
{
    QFile file(filename);
    if (!file.open(IO_ReadOnly))
	return false;

    file.close(); 

    KSimpleConfig config( filename, true );
    config.setDesktopGroup();

    // read document url
    QString path = config.readEntry("DocPath");
    if (path.isNull())
	return false;

    url = path;

    // read comment text
    info = config.readEntry("Info");
    if (info.isNull())
	info = config.readEntry("Comment");

    // read icon and miniicon
    //icon = config.readEntry("Icon");
    miniicon = "document2";//config.readEntry("MiniIcon");
    setPixmap(0, SmallIcon(miniicon, 0, 0, KHCFactory::instance()));

    // read name
    name = config.readEntry("Name");
  
    if (name.isNull())
    {
        name = filename.mid(filename.find('/'));
	int pos;
	if ( ( pos = name.findRev( ".desktop" ) ) > 0 )
	{
	    name = name.left( pos );
	}
    }
    setText(0, name);
    return true;
}

void khcNavigatorItem::setName(QString _name)
{
    name = _name;
}

void khcNavigatorItem::setURL(QString _url)
{
    url = _url;
}

void khcNavigatorItem::setInfo(QString _info)
{
    info = _info;
}

void khcNavigatorItem::setIcon(QString _icon)
{
    icon = _icon;
}

void khcNavigatorItem::setMiniIcon(QString _miniicon)
{
    miniicon = _miniicon;
}
