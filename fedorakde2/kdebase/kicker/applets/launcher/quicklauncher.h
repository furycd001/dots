/*****************************************************************

Copyright (c) 2000 Bill Nagel

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

#ifndef __quicklauncher_h__
#define __quicklauncher_h__

#include <qlist.h>
#include <qstring.h>
#include <qpopupmenu.h>

#include <kpanelapplet.h>

#include "quickbutton.h"

class QuickLauncher : public KPanelApplet
{
   Q_OBJECT
public:
   QuickLauncher(const QString& configFile, Type t = Normal, int actions = 0,
   QWidget *parent = 0, const char *name = 0);
   int widthForHeight(int height) const;
   int heightForWidth(int width) const;
   void addApp(int i, QString url);
public slots:
   void slotAddApp(QString url);
   void removeIcon(QuickButton *);
   void about();
protected:
   void mousePressEvent(QMouseEvent *e);
   void resizeEvent(QResizeEvent*);
   void dragEnterEvent(QDragEnterEvent *e);
   void dropEvent(QDropEvent *e);
   void arrangeIcons();
   void saveConfig();
private:
   QPopupMenu *popup;
   QList<QuickButton> buttonList;
   int iconSize;
};

#endif
