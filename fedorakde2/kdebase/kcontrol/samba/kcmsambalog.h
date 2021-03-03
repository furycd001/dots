/*
 * kcmsambalog.h
 *
 * Copyright (c) 2000 Alexander Neundorf <neundorf@kde.org>
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
#ifndef kcmsambalog_h_included
#define kcmsambalog_h_included
 
#include <qlabel.h>
#include <qcstring.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <qevent.h>
#include <kconfig.h>

#include <kurlrequester.h>

#define LOGGROUPNAME "SambaLogFileSettings"

class LogView: public QWidget
{
   Q_OBJECT
   public:
      LogView(QWidget *parent=0, KConfig *config=0, const char *name=0);
      virtual ~LogView() {};
      void saveSettings();
      void loadSettings();
   private:
      KConfig *configFile;
      int filesCount, connectionsCount;
      KURLRequester logFileName;
      QLabel label;
      QListView viewHistory;
      QCheckBox showConnOpen, showConnClose, showFileOpen, showFileClose;
      QPushButton updateButton;
   private slots:
      void updateList();
   signals:
      void contentsChanged(QListView* list, int nrOfFiles, int nrOfConnections);
};

class QListViewItemX:public QListViewItem
{
   public:
      //a faster constructor saves a lot time
      QListViewItemX( QListView * parent,
                      const char *c0,     const char *c1 = 0,
                      const char *c2 = 0, const char *c3 = 0,
                      const char *c4 = 0, const char *c5 = 0,
                      const char *c6 = 0, const char *c7 = 0 )
         :QListViewItem(parent)
      {
         setText( 0, c0 );
         setText( 1, c1 );
         setText( 2, c2 );
         setText( 3, c3 );
         if (c4==0) return;
         setText( 4, c4 );
         if (c5==0) return;
         setText( 5, c5 );
         if (c6==0) return;
         setText( 6, c6 );
         if (c7==0) return;
         setText( 7, c7 );
      };
};

#endif // main_included
