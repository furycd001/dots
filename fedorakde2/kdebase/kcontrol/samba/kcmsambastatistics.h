/*
 * kcmsambastatistics.h
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
#ifndef kcmsambastatistics_h_included
#define kcmsambastatistics_h_included
 
#include <qwidget.h>
#include <qlist.h>

class QListView;
class QLabel;
class QComboBox;
class QCheckBox;
class QLineEdit;
class QPushButton;

class KConfig;

class SmallLogItem
{
 public:
  SmallLogItem():name(""),count(0){};
  SmallLogItem(QString n):name(n),count(1){};
  QString name;
  int count;
};

class LogItem
{
 public:
  LogItem():name(""), accessed(),count(0) {};
  LogItem(QString n, QString a):name(n), accessed(), count(1)
	{
	  accessed.setAutoDelete(TRUE);
	  accessed.append(new SmallLogItem(a));
	};
  QString name;
  //QStrList accessedBy;
  QList<SmallLogItem> accessed;
  int count;
  SmallLogItem* itemInList(QString name);
  void addItem (QString host);
};

class SambaLog
{
 public:
  SambaLog()
	{
	  items.setAutoDelete(TRUE);
	};
  QList<LogItem> items;
  void addItem (QString share, QString host);
  void printItems();
 private:
  LogItem* itemInList(QString name);
};

class StatisticsView: public QWidget
{
  Q_OBJECT
public:
  StatisticsView(QWidget *parent=0, KConfig *config=0, const char *name=0);
  virtual ~StatisticsView() {};
  void saveSettings() {};
  void loadSettings() {};
  public slots:
	void setListInfo(QListView *list, int nrOfFiles, int nrOfConnections);
private:
  KConfig *configFile;
  QListView *dataList;
  QListView* viewStatistics;
  QLabel* connectionsL, *filesL;
  QComboBox* eventCb;
  QLabel* eventL;
  QLineEdit* serviceLe;
  QLabel* serviceL;
  QLineEdit* hostLe;
  QLabel* hostL;
  QPushButton* calcButton, *clearButton;
  QCheckBox* expandedInfoCb, *expandedUserCb;
  int connectionsCount, filesCount, calcCount;
private slots:
	void clearStatistics();
  void calculate();
};
#endif // main_included
