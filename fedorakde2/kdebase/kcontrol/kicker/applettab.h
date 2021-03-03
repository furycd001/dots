/*
 * applettab.h
 *
 *  Copyright (c) 2000 Matthias Elter <elter@kde.org>
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
 */


#ifndef __applettab_h__
#define __applettab_h__

#include <qwidget.h>

class QGroupBox;
class QGridLayout;
class QButtonGroup;
class QRadioButton;
class QPushButton;
class KListView;
class QListViewItem;

class AppletTab : public QWidget
{
  Q_OBJECT
  
 public:
  AppletTab( QWidget *parent=0, const char* name=0 );

  void load();
  void save();
  void defaults();

  QString quickHelp() const;

 signals:
  void changed();

 protected slots:
  void level_changed(int level);
  void trusted_selection_changed(QListViewItem *);
  void available_selection_changed(QListViewItem *);
  void add_clicked();
  void remove_clicked();

 protected:
  void updateTrusted();
  void updateAvailable();
  void updateAddRemoveButton();
 private:
  QGridLayout *layout;

  // security level group
  QButtonGroup  *level_group;
  QRadioButton  *trusted_rb, *new_rb, *all_rb;

  // trusted list group
  QGroupBox     *list_group;
  QPushButton   *pb_add, *pb_remove;
  KListView     *lb_trusted, *lb_available;
  QStringList   available, l_available, l_trusted;
};

#endif

