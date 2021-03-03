/*
    Naughty applet - Runaway process monitor for the KDE panel

    Copyright 2000 Rik Hemsley (rikkus) <rik@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef NAUGHTY_CONFIG_DIALOG_H
#define NAUGHTY_CONFIG_DIALOG_H

#include <kdialogbase.h>

class KEditListBox;
class KIntNumInput;

class NaughtyConfigDialog : public KDialogBase
{
  Q_OBJECT

  public:

    NaughtyConfigDialog
      (
       const QStringList & items,
       uint interval,
       uint threshold,
       QWidget * parent = 0,
       const char * name = 0
      );

    ~NaughtyConfigDialog();

    QStringList ignoreList() const;
    uint updateInterval() const;
    uint threshold() const;

  private:

    KEditListBox * listBox_;

    KIntNumInput * kini_updateInterval_;
    KIntNumInput * kini_threshold_;
};

#endif
