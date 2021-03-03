#include <config.h>
#include "program.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>	
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h> 
#include <sys/wait.h>
#include <signal.h>

#include <kdebug.h>

Program::Program(const QStringList &args)
:m_pid(0)
,mArgs(args)
,mStarted(false)
{
}

Program::~Program()
{
   if (m_pid!=0)
   {
      ::close(mStdin[0]);
      ::close(mStdout[0]);
      ::close(mStderr[0]);

      ::close(mStdin[1]);
      ::close(mStdout[1]);
      ::close(mStderr[1]);

      int s(0);
      //::wait(&s);
      ::waitpid(m_pid,&s,0);
      this->kill();
      ::waitpid(m_pid,&s,WNOHANG);
   };
}

bool Program::start()
{
   if (mStarted) return false;
   if (pipe(mStdout)==-1) return false;
   if (pipe(mStdin )==-1) return false;
   if (pipe(mStderr )==-1) return false;

   int notificationPipe[2];
   if (pipe(notificationPipe )==-1) return false;

   m_pid=fork();

   if (m_pid>0)
   {
      //parent
      ::close(mStdin[0]);
      ::close(mStdout[1]);
      ::close(mStderr[1]);
      ::close(notificationPipe[1]);
      mStarted=true;
      fd_set notifSet;
      FD_ZERO(&notifSet);
      FD_SET(notificationPipe[0],&notifSet);
      struct timeval tv;
      //wait up to five seconds

      kdDebug(7101)<<"**** waiting for notification"<<endl;
      //0.2 sec
      tv.tv_sec=0;
      tv.tv_usec=1000*200;
      int result=::select(notificationPipe[0]+1,&notifSet,0,0,&tv);
/*      if (result<1)
      {
         kdDebug(7101)<<"**** waiting for notification: failed "<<result<<endl;
         return false;
      }
      else*/
      if(result==1)
      {
         char buf[256];
         result=::read(notificationPipe[0],buf,256);
         //if execvp() failed the child sends us "failed"
         if (result>0)
            return false;
      };
      kdDebug(7101)<<"**** waiting for notification: succeeded"<<result<<endl;
      return true;
   }
   else if (m_pid==-1)
   {
      //failed
      return false;
   }
   else if (m_pid==0)
   {
      ::close(notificationPipe[0]);

      //child
      ::close(0); // close the stdios
      ::close(1);
      ::close(2);

      dup(mStdin[0]);
      dup(mStdout[1]);
      dup(mStderr[1]);

      ::close(mStdin[1]);
      ::close(mStdout[0]);
      ::close(mStderr[0]);

      fcntl(mStdin[0], F_SETFD, FD_CLOEXEC);
      fcntl(mStdout[1], F_SETFD, FD_CLOEXEC);
      fcntl(mStderr[1], F_SETFD, FD_CLOEXEC);

      char **arglist=(char**)malloc((mArgs.count()+1)*sizeof(char*));
      int c=0;

      for (QStringList::Iterator it=mArgs.begin(); it!=mArgs.end(); ++it)
      {
         arglist[c]=(char*)malloc((*it).length()+1);
         strcpy(arglist[c], (*it).latin1());
         c++;
      }
      arglist[mArgs.count()]=0;
      //make parsing easier
      putenv("LANG=C");
      execvp(arglist[0], arglist);
      //we only get here if execvp() failed
      ::write(notificationPipe[1],"failed",strlen("failed"));
      ::close(notificationPipe[1]);
      _exit(-1);
   };
   return false;
}

bool Program::isRunning()
{
	return mStarted;
}

int Program::select(int secs, int usecs, bool& stdoutReceived, bool& stderrReceived/*, bool& stdinWaiting*/)
{
   stdoutReceived=false;
   stderrReceived=false;

   struct timeval tv;
   tv.tv_sec=secs;
   tv.tv_usec=usecs;

   fd_set readFDs;
   FD_ZERO(&readFDs);
   FD_SET(stdoutFD(),&readFDs);
   FD_SET(stderrFD(),&readFDs);

   int maxFD=stdoutFD();
   if (stderrFD()>maxFD) maxFD=stderrFD();

   /*fd_set writeFDs;
   FD_ZERO(&writeFDs);
   FD_SET(stdinFD(),&writeFDs);
   if (stdinFD()>maxFD) maxFD=stdinFD();*/
   maxFD++;

   int result=::select(maxFD,&readFDs,/*&writeFDs*/0,0,&tv);
   if (result>0)
   {
      stdoutReceived=FD_ISSET(stdoutFD(),&readFDs);
      stderrReceived=FD_ISSET(stderrFD(),&readFDs);
      //stdinWaiting=(FD_ISSET(stdinFD(),&writeFDs));
   };
   return result;
};

int Program::kill()
{
   if (m_pid==0)
      return -1;
   return ::kill(m_pid, SIGTERM);
   //::kill(m_pid, SIGKILL);
};

