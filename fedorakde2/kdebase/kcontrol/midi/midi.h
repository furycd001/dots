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

class QRadioButton;
class QButtonGroup;
class DeviceManager;
class KURLRequester;
class QCheckBox;
class QListBox;
class QLabel;


class KMidConfig : public KCModule
{
  Q_OBJECT

public:
  KMidConfig(QWidget *parent = 0L, const char *name = 0L);
  virtual ~KMidConfig();
  
  void load();
  void save();
  void defaults();
  
  int buttons();
  
protected slots:
  void configChanged();
  void deviceSelected(int idx);
  void useMap(bool i);
      
private:
  KURLRequester *maprequester;
  QCheckBox *usemap;
  QListBox *mididevices;
  QLabel *label;

  DeviceManager *devman;

};

#endif
