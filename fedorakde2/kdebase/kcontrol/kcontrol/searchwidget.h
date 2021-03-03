/*
  Copyright (c) 2000 Matthias Elter <elter@kde.org>
 
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
*/                                                                            

#ifndef __searchwidget_h__
#define __searchwidget_h__

#include <qwidget.h>
#include <qlist.h>
#include <qstring.h>
#include <qstringlist.h>

#include "modules.h"

class KListBox;
class KLineEdit;
class QListBoxItem;

class KeywordListEntry
{
 public:
  KeywordListEntry(const QString& name, ConfigModule* module);
  
  void addModule(ConfigModule* module);

  QString name() { return _name; }
  QList<ConfigModule> modules() { return _modules; }
  
 private:
  QString _name;
  QList<ConfigModule> _modules;
  
};

class SearchWidget : public QWidget
{  
  Q_OBJECT    
  
public:   
  SearchWidget(QWidget *parent, const char *name=0);

  void populateKeywordList(ConfigModuleList *list);

signals:
  void moduleSelected(const QString&);

protected:
  void populateKeyListBox(const QString& regexp);
  void populateResultListBox(const QString& keyword);

protected slots:
  void slotSearchTextChanged(const QString &);
  void slotKeywordSelected(const QString &);
  void slotModuleSelected(int idx);
  void slotModuleClicked(QListBoxItem *item);

private:
  KListBox  *_keyList, *_resultList;
  KLineEdit *_input; 
  QList<KeywordListEntry> _keywords;
  QStringList _results;
};

#endif
