/*
 * main.cpp
 *
 * Copyright (c) 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
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


#include <kapp.h>
#include <kglobal.h>
#include <klocale.h>
#include <kcmodule.h>
#include <ksimpleconfig.h>


#include <X11/Xlib.h>


#include "syssound.h"


extern "C"
{

  KCModule *create_syssound(QWidget *parent, const char *name)
  {
    KGlobal::locale()->insertCatalogue("kcmsyssound");
    return new KSoundWidget(parent, name);
  }

  void init_sound()
  {
    XKeyboardState kbd;
    XKeyboardControl kbdc;

    XGetKeyboardControl(kapp->getDisplay(), &kbd);

    KSimpleConfig config("kwmsoundrc", true, false);
    config.setGroup("Bell");

    kbdc.bell_percent = config.readNumEntry("Volume", kbd.bell_percent);
    kbdc.bell_pitch = config.readNumEntry("Pitch", kbd.bell_pitch);
    kbdc.bell_duration = config.readNumEntry("Duration", kbd.bell_duration);
    if (kbdc.bell_percent == 0)
      kbdc.bell_duration = 0;
    XChangeKeyboardControl(kapp->getDisplay(),
                           KBBellPercent | KBBellPitch | KBBellDuration,
                           &kbdc);
  }

}
