/*
 *  lookandfeeltab.h
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


#ifndef __lookandfeeltab_h__
#define __lookandfeeltab_h__

#include "lookandfeeltab.h"

class QLabel;
class QStringList;

class LookAndFeelTab : public LookAndFeelTabBase
{
  Q_OBJECT

 public:
  LookAndFeelTab( QWidget *parent=0, const char* name=0 );

  void load();
  void save();
  void defaults();

  QString quickHelp() const;

 signals:
  void changed();

 protected:
  void setLabel( QLabel *label, const QString &t );
  void fill_tile_input();
  QStringList queryAvailableTiles();

 protected slots:
  void browse_theme();
  void tiles_clicked();
  void kmenu_clicked();
  void kmenu_changed(const QString&);
  void url_clicked();
  void url_changed(const QString&);
  void browser_clicked();
  void browser_changed(const QString&);
  void exe_clicked();
  void exe_changed(const QString&);
  void desktop_clicked();
  void desktop_changed(const QString&);
  void wl_clicked();
  void wl_changed(const QString&);


 private:
  QString theme;
  QPixmap theme_preview;
  QStringList tiles;
};

#endif

