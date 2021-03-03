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

#ifndef __moduleiconview_h__
#define __moduleiconview_h__

#include <qiconview.h>
#include <kiconview.h>

class ConfigModule;
class ConfigModuleList;

class ModuleIconItem : public QIconViewItem
{

public:
  ModuleIconItem(QIconView *parent, const QString& text, const QPixmap& pm, ConfigModule *m = 0)
	: QIconViewItem(parent, text, pm)
	, _tag(QString::null)
	, _module(m)
	{}

  void setConfigModule(ConfigModule* m) { _module = m; }
  void setTag(const QString& t) { _tag = t; }
  ConfigModule* module() { return _module; }
  QString tag() { return _tag; }


private:
  QString       _tag;
  ConfigModule *_module;
};

class ModuleIconView : public KIconView
{
  Q_OBJECT

public:
  ModuleIconView(ConfigModuleList *list, QWidget * parent = 0, const char * name = 0);
  
  void makeSelected(ConfigModule* module);
  void makeVisible(ConfigModule *module);
  void fill();

signals:
  void moduleSelected(ConfigModule*);

protected slots:
  void slotItemSelected(QIconViewItem*);

protected:
 QDragObject *dragObject(); 
 void keyPressEvent(QKeyEvent *);
  
private:
  QString           _path; 
  ConfigModuleList *_modules;

};



#endif
