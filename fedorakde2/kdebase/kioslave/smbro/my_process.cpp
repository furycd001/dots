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

#include <config.h>
#include "my_process.h"

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>

ClientProcess::ClientProcess()
:startingFinished(false)
,m_exited(-1)
{}

ClientProcess::~ClientProcess()
{
   this->kill();
}

void ClientProcess::kill()
{
   int s(0);
   ::waitpid(pid(),&s,WNOHANG);
   ::kill(pid(), SIGTERM);
   ::waitpid(pid(),&s,0);
};

int ClientProcess::exited()
{
   if (m_exited!=-1)
      return m_exited;
   int s(0);
   if (::waitpid(pid(),&s,WNOHANG)==0)
      return -1;
   if (WIFEXITED(s))
   {
      m_exited=WEXITSTATUS(s);
      return m_exited;
   };
   if (WIFSIGNALED(s))
   {
      m_exited=2;
      return m_exited;
   };
   return -1;
};

int ClientProcess::select(int secs, int usecs, bool* readEvent, bool* writeEvent)
{
   if (readEvent!=0)
      *readEvent=false;
   if (writeEvent!=0)
      *writeEvent=false;

   struct timeval tv;
   tv.tv_sec=secs;
   tv.tv_usec=usecs;

   fd_set readFD;
   FD_ZERO(&readFD);
   if (readEvent!=0)
      FD_SET(fd(),&readFD);

   fd_set writeFD;
   FD_ZERO(&writeFD);
   if (writeEvent!=0)
      FD_SET(fd(),&writeFD);

   int result=::select(fd()+1,&readFD,&writeFD,0,&tv);
   if (result>0)
   {
      if (readEvent!=0)
         *readEvent=FD_ISSET(fd(),&readFD);
      if (writeEvent!=0)
         *writeEvent=FD_ISSET(fd(),&writeFD);
   };
   return result;
};

bool ClientProcess::start(const QCString& binary, QCStringList& args)
{
   setTerminal(true);
   // Try to set the default locale to make the parsing of the output
   // of `smbclient' easier.
//   putenv("LANG=C");
   int ret = PtyProcess::exec(binary, args);
   //kdDebug()<<"ClientProcess::start() exec() returned "<<ret<<endl;
   if (ret != 0)
   {
      //cerr<<"could not execute smbclient"<<endl;
      return false;
   }
   return true;
}
