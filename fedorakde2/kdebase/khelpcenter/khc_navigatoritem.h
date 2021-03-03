/*
 *  khc_navigatoritem.h - part of the KDE Help Center
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

#ifndef __khc_navigatoritem_h___
#define __khc_navigatoritem_h___

#include <qlistview.h>

class khcNavigatorItem : public QListViewItem
{
 public:
    khcNavigatorItem (QListView* parent, const QString& text = QString::null, const QString& miniicon = QString::null);
    khcNavigatorItem (QListViewItem* parent, const QString& text = QString::null, const QString& miniicon = QString::null);
    bool readKDElnk (const QString &filename);
    void setName(QString _name);
    void setURL(QString _url);
    void setInfo(QString _info);
    void setIcon(QString _icon);
    void setMiniIcon(QString _miniicon);
    QString getName() {return name;};
    QString getURL() { return url; }
    QString getInfo() {return info;};
    QString getIcon() {return icon;};
    QString getMiniIcon() {return miniicon;};
    
 private:
    void init(const QString& text, const QString& miniicon);
    
 protected:
    QString name;
    QString url;
    QString info;
    QString icon;
    QString miniicon;
};

#endif
