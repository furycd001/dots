/***************************************************************************
                          kateconsole.h  -  description
                             -------------------
    begin                : Wed Jan 3 2001
    copyright            : (C) 2001 by Anders Lund, anders@alweb.dk
    email                : anders@alweb.dk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef __kate_console_h__
#define __kate_console_h__

#include "../main/katemain.h"

#include <qwidget.h>
#include <kparts/part.h>

class KateConsole : public QWidget
{
  Q_OBJECT

  public:
    KateConsole (QWidget* parent=0, const char* name=0);
    ~KateConsole ();

    void cd (KURL url=0L);

  private:
    KParts::ReadOnlyPart *part;
    QVBoxLayout* lo;

  // Only needed for Konsole
  private slots:
    void notifySize (int,int) {};
    void changeColumns (int) {};
    void changeTitle(int,const QString&) {};

    void slotDestroyed ();
};

#endif
