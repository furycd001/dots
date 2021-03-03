/***************************************************************************
                          katecmd.h  -  description
                             -------------------
    begin                : Mon Feb 5 2001
    copyright            : (C) 2001 by Christoph Cullmann
    email                : crossfire@babylon2k.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _KATE_CMD_H
#define _KATE_CMD_H

#include "../main/katemain.h"

#include <qobject.h>
#include <qstring.h>
#include <qlist.h>

class KateCmdParser
{
  public:
    KateCmdParser (KateDocument *doc=0L);
    virtual ~KateCmdParser ();

    virtual bool execCmd (QString cmd=0L, KateView *view=0L)=0;

  private:
    KateDocument *myDoc;
};

class KateCmd : public QObject
{
  Q_OBJECT

  public:
    KateCmd (KateDocument *doc=0L);
    ~KateCmd ();

    void execCmd (QString cmd=0L, KateView *view=0L);

  private:
    KateDocument *myDoc;
    QList<KateCmdParser> myParser;
};

#endif

