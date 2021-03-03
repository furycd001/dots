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

#ifndef __PLUGINCONFIG_H__
#define __PLUGINCONFIG_H__

#include <qwidget.h>

class QListBox;
class QListBoxItem;
class QPushButton;

class PluginConfig : public QWidget
{
  Q_OBJECT

public:
  PluginConfig( QWidget* parent = 0L, const char* name = 0L );
  void load();
  void save();
  void defaults();

signals:
  void changed();

private slots:
  void slotInfo();
  void slotConfigure();
  void pluginSelected( QListBoxItem* );
  void slotItemChanged( const QString & );
  
private:
  QPushButton *m_conf;
  QListBox *m_pluginList;
  
};

#endif // __CONNECTIONCONFIG_H__
