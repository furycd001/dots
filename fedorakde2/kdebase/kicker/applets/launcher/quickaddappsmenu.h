/*****************************************************************

Copyright (c) 2000 Bill Nagel
   based on paneladdappsmenu.h which is
   Copyright (c) 1999-2000 the kicker authors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#ifndef __quickaddappsmenu_h__
#define __quickaddappsmenu_h__

#include "service_mnu.h"

class QuickAddAppsMenu : public PanelServiceMenu
{
   Q_OBJECT
public:
   QuickAddAppsMenu(const QString &label, const QString &relPath, QObject *target, QWidget *parent=0, const char *name=0);
   QuickAddAppsMenu(QObject *target, QWidget *parent=0, const char *name=0);
signals:
   void addApp(QString);
protected slots:
   virtual void slotExec(int id);
protected:
   virtual PanelServiceMenu *newSubMenu(
      const QString &label,
      const QString &relPath,
      QWidget *parent,
      const char *name
   );
private:
   QObject *targetObject;
};

#endif
