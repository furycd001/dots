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

#ifndef __dockcontainer_h__
#define __dockcontainer_h__

#include <qwidget.h>

class ConfigModule;
class QLabel;

class DockContainer : public QWidget
{
  Q_OBJECT

public:
  DockContainer(QWidget *parent=0);
  virtual ~DockContainer();

  void setBaseWidget(QWidget *widget);
  QWidget *baseWidget() { return _basew; }
  
  void dockModule(ConfigModule *module);

public slots:
  void removeModule();

protected slots:
  void quickHelpChanged();

protected:
  void resizeEvent (QResizeEvent *);
  void deleteModule();

signals:
  void newModule(const QString &name, const QString& docPath, const QString &quickhelp);
  void changedModule(ConfigModule *module);

private:
  QWidget      *_basew;
  QLabel       *_busy;
  ConfigModule *_module;

};

#endif
