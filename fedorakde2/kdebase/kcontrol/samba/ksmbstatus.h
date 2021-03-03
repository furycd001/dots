/*
 * ksmbstatus.h
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
#ifndef ksmbstatus_h_included
#define ksmbstatus_h_included
 
#include <kpopupmenu.h>
#include <qtimer.h>
#include <qlabel.h>
#include <kprocess.h>
#include <qcstring.h>
#include <qlistview.h>
#include <kconfig.h>

#define SCREEN_XY_OFFSET 20

class QListView;
class QListViewItem;
class KProcess;

class NetMon : public QWidget
{
Q_OBJECT
public:
   NetMon(QWidget *parent, KConfig * config=0, const char * name=0);
   void saveSettings() {};
   void loadSettings() {};
private:
   KConfig *configFile;
   KProcess *showmountProc;
   QListView *list;
   QLabel *version;
   QTimer *timer;
   KPopupMenu *menu;
   QListViewItem *killrow;
   int rownumber;
   enum {header, connexions, locked_files, finished, nfs} readingpart;
   int lo[65536];
   int nrpid;
   void processNFSLine(char *bufline, int linelen);
   void processSambaLine(char *bufline, int linelen);

   QCString strShare, strUser, strGroup, strMachine, strSince, strPid;
   int iUser, iGroup, iMachine, iPid;

private slots:
   void killShowmount();
   void update();
   //this killing stuff is kindof pointless
   //it kills the smbd which serves a client, this smbd will be started
   //again instantly so smbd mounted stuff or users from windows
   //won't notice it at all
   //aleXXX
/*   void Kill();
   void Killmenu(QListViewItem *row, const QPoint &pos, int column);*/
   void slotReceivedData(KProcess *proc, char *buffer, int buflen);
};

#endif // main_included
