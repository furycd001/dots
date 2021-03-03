/**
 * midi.h
 *
 * Copyright (c) 2000 Antonio Larrosa <larrosa@kde.org>
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

#ifndef __midi_h__
#define __midi_h__

#include <kcmodule.h>
#include <qmap.h>
#include <qlistview.h>

class QPushButton;
class DeviceManager;
class KURLRequester;
class QCheckBox;


class IconThemesConfig : public KCModule
{
  Q_OBJECT

public:
  IconThemesConfig(QWidget *parent = 0L, const char *name = 0L);
  virtual ~IconThemesConfig();
  
  void loadThemes();

  void updateRemoveButton();

  void load();
  void save();
  void defaults();
  
  int buttons();
  
protected slots:
  void configChanged();
  void themeSelected(QListViewItem *item);
  void installNewTheme();
  void removeSelectedTheme();
      
private:
  QListViewItem *iconThemeItem(QString name);

  QListView *m_iconThemes;
  KURLRequester *m_themeRequester;
  QPushButton *m_removeButton;

  QLabel *m_previewExec;
  QLabel *m_previewFolder;
  QLabel *m_previewDocument;
  QListViewItem *m_defaultTheme;
  QMap <QString, QString>m_themeNames;

};

#endif
