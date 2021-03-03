/*

  Copyright (c) 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>

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



#ifndef __PROXYWIDGET_H__
#define __PROXYWIDGET_H__


class QWidget;
class QPushButton;
class QFrame;

class KCModule;
class KAboutData;

#include "dockcontainer.h"
#include <qscrollview.h>

class ProxyView;

class ProxyWidget : public QWidget
{
  Q_OBJECT

public:

  ProxyWidget(KCModule *client, QString title, const char *name=0, bool run_as_root = false);
  ~ProxyWidget();

  QString quickHelp() const;
  const KAboutData *aboutData() const;

public slots:

  void helpClicked();
  void defaultClicked();
  void applyClicked();
  void resetClicked();
  void rootClicked();

  void clientChanged(bool state);


signals:

  void closed();
  void helpRequest();
  void changed(bool state);
  void runAsRoot();


private:

  QPushButton *_help, *_default, *_apply, *_reset, *_root;
  QFrame      *_sep;
  KCModule    *_client;
    ProxyView *view;

};


#endif
