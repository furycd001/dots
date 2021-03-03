/*
 * ksmbstatus.cpp
 *
 *
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

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>

#include <qstring.h>
#include <qregexp.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpoint.h>
#include <qfont.h>
#include <qtimer.h>

#include <kprocess.h>
#include <kapp.h>
#include <klocale.h>

#include "ksmbstatus.h"
#include "ksmbstatus.moc"


#define Before(ttf,in) in.left(in.find(ttf))
#define After(ttf,in)  (in.contains(ttf)?QString(in.mid(in.find(ttf)+QString(ttf).length())):QString(""))

NetMon::NetMon( QWidget * parent, KConfig *config, const char * name )
   : QWidget(parent, name)
   ,configFile(config)
   ,showmountProc(0)
   ,strShare("")
   ,strUser("")
   ,strGroup("")
   ,strMachine("")
   ,strSince("")
   ,strPid("")
   ,iUser(0)
   ,iGroup(0)
   ,iMachine(0)
   ,iPid(0)
{
    QBoxLayout *topLayout = new QVBoxLayout(this);
    topLayout->setAutoAdd(true);
    topLayout->setMargin(SCREEN_XY_OFFSET);
    topLayout->setSpacing(10);

    list=new QListView(this,"Hello");
    version=new QLabel(this);

    list->setAllColumnsShowFocus(true);
    list->setMinimumSize(425,200);
    list->setShowSortIndicator(true);

    list->addColumn(i18n("Type"));
    list->addColumn(i18n("Service"));
    list->addColumn(i18n("Accessed from"));
    list->addColumn(i18n("UID"));
    list->addColumn(i18n("GID"));
    list->addColumn(i18n("PID"));
    list->addColumn(i18n("Open Files"));

    timer = new QTimer(this);
    timer->start(15000);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    /*menu = new KPopupMenu();
    QObject::connect(list,SIGNAL(rightButtonPressed(QListViewItem *,const QPoint &,int)),
		    SLOT(Killmenu(QListViewItem *,const QPoint &,int)));*/
    update();
};

void NetMon::processNFSLine(char *bufline, int)
{
   QCString line(bufline);
   if (line.contains(":/"))
      new QListViewItem(list,"NFS",After(":",line),Before(":/",line));
};

void NetMon::processSambaLine(char *bufline, int)
{
   QCString line(bufline);
   rownumber++;
   if (rownumber == 2)
      version->setText(bufline); // second line = samba version
   if ((readingpart==header) && line.contains("Service"))
   {
      iUser=line.find("uid");
      iGroup=line.find("gid");
      iPid=line.find("pid");
      iMachine=line.find("machine");
   }
   else if ((readingpart==header) && (line.contains("---")))
   {
      readingpart=connexions;
   }
   else if ((readingpart==connexions) && (int(line.length())>=iMachine))
   {
      strShare=line.mid(0,iUser);
      strUser=line.mid(iUser,iGroup-iUser);
      strGroup=line.mid(iGroup,iPid-iGroup);
      strPid=line.mid(iPid,iMachine-iPid);

      line=line.mid(iMachine,line.length());
      strMachine=line;
      new QListViewItem(list,"SMB",strShare,strMachine, strUser,strGroup,strPid/*,strSince*/);
   }
   else if (readingpart==connexions)
      readingpart=locked_files;
   else if ((readingpart==locked_files) && (line.find("No ")==0))
      readingpart=finished;
   else if (readingpart==locked_files)
   {
      if ((strncmp(bufline,"Pi", 2) !=0) // "Pid DenyMode ..."
          && (strncmp(bufline,"--", 2) !=0)) // "------------"
      {
         char *tok=strtok(bufline," ");
         int pid=atoi(tok);
         (lo)[pid]++;
      }
   };
}

// called when we get some data from smbstatus
// can be called for any size of buffer (one line, several lines,
// half of one ...)
void NetMon::slotReceivedData(KProcess *, char *buffer, int )
{
   //kdDebug()<<"received stuff"<<endl;
   char s[250],*start,*end;
   size_t len;
   start = buffer;
   while ((end = strchr(start,'\n'))) // look for '\n'
   {
      len = end-start;
      strncpy(s,start,len);
      s[len] = '\0';
      //kdDebug() << "recived: "<<s << endl;
      if (readingpart==nfs)
         processNFSLine(s,len);
      else
         processSambaLine(s,len); // process each line
      start=end+1;
   }
   if (readingpart==nfs)
   {
      list->viewport()->update();
      list->update();
   };
   // here we could save the remaining part of line, if ever buffer
   // doesn't end with a '\n' ... but will this happen ?
}

void NetMon::update()
{
   int pid,n;
   QListViewItem *row;
   KProcess * process = new KProcess();

   for (n=0;n<65536;n++) lo[n]=0;
   list->clear();
   /* Re-read the Contents ... */

   rownumber=0;
   readingpart=header;
   nrpid=0;
   connect(process,
           SIGNAL(receivedStdout(KProcess *, char *, int)),
           SLOT(slotReceivedData(KProcess *, char *, int)));
   *process << "smbstatus"; // the command line
   //kdDebug() << "update" << endl;
   if (!process->start(KProcess::Block,KProcess::Stdout)) // run smbstatus
      version->setText(i18n("Error: Unable to run smbstatus"));
   else if (rownumber==0) // empty result
      version->setText(i18n("Error: Unable to open configuration file \"smb.conf\""));
   else
   {
      // ok -> count the number of locked files for each pid
      for (row=list->firstChild();row!=0;row=row->itemBelow())
      {
//         cerr<<"NetMon::update: this should be the pid: "<<row->text(5)<<endl;
         pid=row->text(5).toInt();
         row->setText(6,QString("%1").arg((lo)[pid]));
      }
   }
   delete process;
   process=0;

   readingpart=nfs;
   if (showmountProc!=0)
      delete showmountProc;
   showmountProc=new KProcess();
   //*showmountProc<<"dn";
   *showmountProc<<"showmount"<<"-a"<<"localhost";
   connect(showmountProc,SIGNAL(receivedStdout(KProcess *, char *, int)),SLOT(slotReceivedData(KProcess *, char *, int)));
   //without this timer showmount hangs up to 5 minutes
   //if the portmapper daemon isn't running
   QTimer::singleShot(5000,this,SLOT(killShowmount()));
   //kdDebug()<<"starting kill timer with 5 seconds"<<endl;
   connect(showmountProc,SIGNAL(processExited(KProcess*)),this,SLOT(killShowmount()));
   if (!showmountProc->start(KProcess::NotifyOnExit,KProcess::Stdout)) // run showmount
      version->setText(version->text()+i18n(" Error: Unable to run showmount"));
   //delete showmountProc;
   //showmountProc=0;

   version->adjustSize();
   list->show();
}

void NetMon::killShowmount()
{
   //kdDebug()<<"killShowmount()"<<endl;
   //if (showmountProc==0) cerr<<"showmountProc==0 !"<<endl;
   if (showmountProc!=0)
   {
      //this one kills showmount
      //cerr<<"killing showmount..."<<endl;
      delete showmountProc;
      showmountProc=0;
      //cerr<<"succeeded"<<endl;
   };

};

/*void NetMon::Kill()
{
   QString a(killrow->text(5));
   kill(a.toUInt(),15);
   update();
}

void NetMon::Killmenu(QListViewItem * row, const QPoint& pos, int )
{
   if (row!=0)
   {
       killrow=row;
       menu->clear();
       menu->insertItem("&Kill",this,SLOT(Kill()));
       menu->setTitle("//"+row->text(2)+"/"+row->text(1));
       menu->popup(pos);
   }
}*/

