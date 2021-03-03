/*
 *  Copyright (c) 1999 Michael Koch <koch@kde.org>
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

#ifndef __CONNECTIONCONFIG_H__
#define __CONNECTIONCONFIG_H__

#include <qwidget.h>

#include <kdb/dblistview.h>

class QListBox;

class ConnectionConfig : public QWidget
{
  Q_OBJECT

public:
  ConnectionConfig( QWidget* parent = 0, const char* name = 0 );
  void load();
  void save();
  void defaults();
  
signals:
  void changed();
  
private slots:
  void slotAdd();
  void slotEdit();
  void slotRemove();
  void slotItemSelected( QListViewItem *item );
 
private:
  KDB::DBListView *m_connectionList;
  QPushButton     *m_btnAdd;
  QPushButton     *m_btnEdit;
  QPushButton     *m_btnRemove;
};

#endif // __CONNECTIONCONFIG_H__
