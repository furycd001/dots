/*  This file is part of the KDE project
    Copyright (C) 2000 Alexander Neundorf <neundorf@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef MY_PROCESS_H
#define MY_PROCESS_H

#include <qcstring.h>
#include <kdesu/process.h>

class ClientProcess:public PtyProcess
{
   public:
      ClientProcess();
      ~ClientProcess();

      bool start(const QCString& binary, QCStringList& args);
      QCString error() { return ""; };
      void kill();
      int select(int secs, int usecs, bool* readEvent=0, bool* writeEvent=0);
      bool startingFinished;
      int exited();
   protected:
      int m_exited;
};

#endif
