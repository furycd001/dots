/*
 * kcmsambastatistics.cpp
 *
 * Copyright (c) 2000 Alexander Neundorf <alexander.neundorf@rz.tu-ilmenau.de>
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
#include "kcmsambastatistics.h"
#include "kcmsambastatistics.moc"

#include <qlayout.h>
#include <qlistview.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qpushbutton.h>

#include <kapp.h>
#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>

#include <iostream>

#define STAT_SCREEN_XY_OFFSET 15

StatisticsView::StatisticsView(QWidget *parent,KConfig *config, const char *name)
   :QWidget (parent, name)
   ,configFile(config)
   ,dataList(0)
   ,connectionsCount(0)
   ,filesCount(0)
   ,calcCount(0)
{
  viewStatistics = new QListView( this );
  connectionsL = new QLabel( i18n( "Connections: 0" ), this );
  filesL = new QLabel( i18n( "File accesses: 0" ), this );
  eventCb = new QComboBox( false, this );
  eventL = new QLabel( eventCb, i18n( "Event: " ), this );
  serviceLe = new QLineEdit( this );
  serviceL = new QLabel( serviceLe, i18n( "Service/File:" ), this );
  hostLe = new QLineEdit( this );
  hostL = new QLabel( hostLe, i18n( "Host/User:" ), this );
  calcButton = new QPushButton( i18n( "Search" ), this );
  clearButton = new QPushButton( i18n( "Clear results" ), this );
  expandedInfoCb = new QCheckBox( i18n( "Show expanded service info" ), this );
  expandedUserCb = new QCheckBox( i18n( "Show expanded host info" ), this );

   viewStatistics->setAllColumnsShowFocus(TRUE);
   viewStatistics->setFocusPolicy(QWidget::ClickFocus);
   viewStatistics->setShowSortIndicator(true);
   
   viewStatistics->addColumn(i18n("Nr"),30);
   viewStatistics->addColumn(i18n("Event"),130);
   viewStatistics->addColumn(i18n("Service/File"),200);
   viewStatistics->addColumn(i18n("Host/User"),90);
   viewStatistics->addColumn(i18n("Hits"),50);
   //viewStatistics->addColumn(i18n("Percentage"),100);

   eventCb->insertItem(i18n("Connection"));
   eventCb->insertItem(i18n("File Access"));
   
   expandedInfoCb->setChecked(FALSE);
   expandedUserCb->setChecked(FALSE);
   clearStatistics();
   serviceLe->setText("*");
   hostLe->setText("*");
   
   viewStatistics->setMinimumSize(375,200);
   connectionsL->setMinimumSize(connectionsL->sizeHint());
   filesL->setMinimumSize(filesL->sizeHint());
   eventL->setMinimumSize(eventL->sizeHint());
   eventCb->setMinimumSize(eventCb->sizeHint());
   hostL->setMinimumSize(hostL->sizeHint());
   hostLe->setMinimumSize(120,hostLe->sizeHint().height());
   serviceL->setMinimumSize(serviceL->sizeHint());
   serviceLe->setMinimumSize(120,serviceLe->sizeHint().height());
   calcButton->setMinimumSize(calcButton->sizeHint());
   clearButton->setMinimumSize(clearButton->sizeHint());
   expandedInfoCb->setMinimumSize(expandedInfoCb->sizeHint());
   expandedUserCb->setMinimumSize(expandedUserCb->sizeHint());
   
   QVBoxLayout *topLayout=new QVBoxLayout(this);
   topLayout->setMargin(8);
   topLayout->setSpacing(7);
   topLayout->addWidget(viewStatistics,1);
   QGridLayout *subLayout=new QGridLayout(topLayout,4,3);
   subLayout->setColStretch(1,1);
   subLayout->setColStretch(2,1);
   
   QHBoxLayout *twoButtonsLayout=new QHBoxLayout;
   twoButtonsLayout->addWidget(calcButton,1);
   twoButtonsLayout->addWidget(clearButton,1);
   
   subLayout->addWidget(connectionsL,0,0);
   subLayout->addWidget(filesL,0,1);
   subLayout->addWidget(eventL,1,0);
   subLayout->addWidget(serviceL,1,1);
   subLayout->addWidget(hostL,1,2);
   subLayout->addWidget(eventCb,2,0);
   subLayout->addWidget(serviceLe,2,1);
   subLayout->addWidget(hostLe,2,2);
   subLayout->addLayout(twoButtonsLayout,3,0);
   subLayout->addWidget(expandedInfoCb,3,1);
   subLayout->addWidget(expandedUserCb,3,2);

   connect(clearButton,SIGNAL(clicked()),this,SLOT(clearStatistics()));
   connect(calcButton,SIGNAL(clicked()),this,SLOT(calculate()));
   setListInfo(0,0,0);
};

void StatisticsView::setListInfo(QListView *list, int nrOfFiles, int nrOfConnections)
{
   dataList=list;
   filesCount=nrOfFiles;
   connectionsCount=nrOfConnections;
   connectionsL->setText(i18n("Connections: %1").arg(KGlobal::locale()->formatNumber(connectionsCount, 0)));
   filesL->setText(i18n("File accesses: %1").arg(KGlobal::locale()->formatNumber(filesCount, 0)));
   clearStatistics();
};

void StatisticsView::calculate()
{
   if (dataList==0) return;
   QApplication::setOverrideCursor(waitCursor);
   int connCount(0);
   if (eventCb->currentText()==i18n("Connection"))
      connCount=1;
   //something has to be counted exactly
   if ((expandedInfoCb->isChecked()) || (expandedUserCb->isChecked()))
   {
      SambaLog sLog;
      QRegExp rService(serviceLe->text(),FALSE,TRUE);
      QRegExp rHost(hostLe->text(),FALSE,TRUE);
      QString item2, item3;
      QListViewItem* item=dataList->firstChild();
      while (item!=0)
      {
         if (connCount)
         {
            if ((QString(item->text(1)).contains(i18n("CONNECTION OPENED")))
                && (QString(item->text(2)).contains(rService)) && (QString(item->text(3)).contains(rHost)))
            {
               if (expandedInfoCb->isChecked()) item2=item->text(2);
               else item2=serviceLe->text();
               if (expandedUserCb->isChecked()) item3=item->text(3);
               else item3=hostLe->text();
               sLog.addItem(item2,item3);
               //count++;
            };
         }
         else
         {
            if ((QString(item->text(1)).contains(i18n("FILE OPENED")))
                && (QString(item->text(2)).contains(rService)) && (QString(item->text(3)).contains(rHost)))
            {
               if (expandedInfoCb->isChecked()) item2=item->text(2);
               else item2=serviceLe->text();
               if (expandedUserCb->isChecked()) item3=item->text(3);
               else item3=hostLe->text();
               sLog.addItem(item2,item3);
               
            };
         };
         item=item->nextSibling();
      };
      for (LogItem* tmpItem=sLog.items.first();tmpItem!=0;tmpItem=sLog.items.next())
      {
         for (SmallLogItem *tmpStr=tmpItem->accessed.first();tmpStr!=0;tmpStr=tmpItem->accessed.next())
         {
            calcCount++;
            QString number("");
            number.sprintf("%6d",calcCount);
            QString hits("");
            hits.sprintf("%7d",tmpStr->count);
            new QListViewItem(viewStatistics,number,eventCb->currentText(),tmpItem->name,tmpStr->name,hits);
         };
      };
   }
   //no expanded info needed
   else
   {
      calcCount++;
      int count(0);
      QRegExp rService(serviceLe->text(),FALSE,TRUE);
      QRegExp rHost(hostLe->text(),FALSE,TRUE);
      QListViewItem* item=dataList->firstChild();
      while (item!=0)
      {
         if (connCount)
         {
            if ((QString(item->text(1)).contains(i18n("CONNECTION OPENED")))
                && (QString(item->text(2)).contains(rService)) && (QString(item->text(3)).contains(rHost)))
               count++;
         }
         else
         {
            if ((QString(item->text(1)).contains(i18n("FILE OPENED")))
                && (QString(item->text(2)).contains(rService)) && (QString(item->text(3)).contains(rHost)))
               count++;
         };
         item=item->nextSibling();
      };
      QString number("");
      number.sprintf("%6d",calcCount);
      QString hits("");
      hits.sprintf("%7d",count);
      new QListViewItem(viewStatistics,number,eventCb->currentText(),serviceLe->text(),hostLe->text(),hits);
   };
   QApplication::restoreOverrideCursor();
};

void StatisticsView::clearStatistics()
{
   viewStatistics->clear();
   calcCount=0;
};

void SambaLog::printItems()
{
   kdDebug() << "****** printing items: ******" << endl;
   for (LogItem* tmpItem=items.first();tmpItem!=0;tmpItem=items.next())
   {
      kdDebug() << "SERVICE: " << tmpItem->name << endl;
      for (SmallLogItem* tmpLog=tmpItem->accessed.first();tmpLog!=0;tmpLog=tmpItem->accessed.next())
         kdDebug() << "      accessed by: " << tmpLog->name << "  " << tmpLog->count << endl;
   };
   kdDebug() << "------ end of printing ------" << endl << endl;
};

LogItem* SambaLog::itemInList(QString name)
{
   LogItem* tmpItem(items.first());
   LogItem* foundItem(0);
   while ((tmpItem!=0) && (foundItem==0))
   {
      if (tmpItem->name==name) foundItem=tmpItem;
      tmpItem=items.next();
   };
   return foundItem;
};

void SambaLog::addItem(QString share, QString user)
{
   //cout<<"            adding connection: -"<<share<<"-   -"<<user<<"-"<<endl;
   LogItem* tmp(itemInList(share));
   if (tmp!=0)
   {
      tmp->count++;
      tmp->addItem(user);
   }
   else
   {
      items.append(new LogItem(share,user));
   };
};

SmallLogItem* LogItem::itemInList(QString name)
{
   SmallLogItem* tmpItem(accessed.first());
   SmallLogItem* foundItem(0);
   while ((tmpItem!=0) && (foundItem==0))
   {
      if (tmpItem->name==name) foundItem=tmpItem;
      tmpItem=accessed.next();
   };
   return foundItem;
};

void LogItem::addItem (QString host)
{
   SmallLogItem* tmp(itemInList(host));
   if (tmp!=0)
      tmp->count++;
   else
      accessed.append(new SmallLogItem(host));
};
