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

#include <qpainter.h>
#include <qdragobject.h>

#include <kapp.h>
#include <kaboutapplication.h>
#include <kaboutdata.h>
#include <kglobal.h>
#include <klocale.h>
#include <kconfig.h>
#include <kstddirs.h>

#include "quicklauncher.h"
#include "quickbutton.h"
#include "quickaddappsmenu.h"

extern "C"
{
   KPanelApplet* init(QWidget *parent, const QString& configFile)
   {
      KGlobal::locale()->insertCatalogue("quicklauncher");
      return new QuickLauncher(configFile, KPanelApplet::Normal,
         KPanelApplet::About,
         parent, "quicklauncher");
   }
}

QuickLauncher :: QuickLauncher(const QString& configFile, Type type, int actions,
                               QWidget *parent, const char *name)
   : KPanelApplet(configFile, type, actions, parent, name)
{
   iconSize = 8;
   setAcceptDrops(true);
   buttonList.setAutoDelete(true);
   setBackgroundMode(X11ParentRelative);

   KConfig *c = config();
   c->setGroup("General");
   if (c->hasKey("Buttons")) {
      QStringList urls = c->readListEntry("Buttons");
      QStringList :: Iterator it(urls.begin());
      for (; it != urls.end(); ++it)
         buttonList.append(new QuickButton(*it, this));
   }
   else {
      buttonList.append(new QuickButton(locate("apps", "Home.desktop"), this));
      buttonList.append(new QuickButton(locate("apps", "System/konsole.desktop"), this));
      buttonList.append(new QuickButton(locate("apps", "KControl.desktop"), this));
      buttonList.append(new QuickButton(locate("apps", "Help.desktop"), this));
      buttonList.append(new QuickButton(locate("apps", "Editors/kwrite.desktop"), this));
      saveConfig();
   }

   QuickAddAppsMenu *addAppsMenu = new QuickAddAppsMenu(this, this);
   popup = new QPopupMenu(this);
   popup->insertItem(i18n("Add Application"), addAppsMenu);
   popup->insertSeparator();
   popup->insertItem(i18n("About"), this, SLOT(about()));

   arrangeIcons();
}

int QuickLauncher :: widthForHeight(int h) const
{
   int n = buttonList.count();
   if (!n) n++;
   if (h < 2 * iconSize) return n * iconSize + 4;
   else return (n / 2 + n % 2) * iconSize + 4;
}

int QuickLauncher :: heightForWidth(int w) const
{
   int n = buttonList.count();
   if (!n) n++;
   if (w < 2 * iconSize) return n * iconSize + 4;
   else return (n / 2 + n % 2) * iconSize + 4;
}

void QuickLauncher :: addApp(int i, QString url)
{
   // find and remove the original button and prevent duplicates
   for (QuickButton *button = buttonList.first(); button; button = buttonList.next())
      if (button->getURL() == url)
         buttonList.removeRef(button);

   // make sure index is in valid range
   if (i < 0 || i > (int)buttonList.count()) i = buttonList.count();

   // create the new icon
   QuickButton *button = new QuickButton(url, this);
   button->resize(iconSize, iconSize);
   buttonList.insert(i, button);
   arrangeIcons();
   button->show();
}

void QuickLauncher :: slotAddApp(QString url)
{
   addApp(0, url);
   saveConfig();
}

void QuickLauncher :: removeIcon(QuickButton *button)
{
   if (button) {
      buttonList.removeRef(button);
      arrangeIcons();
      saveConfig();
   }
}

void QuickLauncher :: about()
{
   KAboutData about("quicklauncher", I18N_NOOP("Quick Launcher"), "1.0",
      I18N_NOOP("A simple application launcher"), KAboutData::License_GPL_V2, "(C) 2000 Bill Nagel");

   KAboutApplication a(&about, this);
   a.exec();
}

void QuickLauncher :: mousePressEvent(QMouseEvent *e)
{
   if (e->button() == RightButton)
      popup->popup(e->globalPos());
}

void QuickLauncher::resizeEvent(QResizeEvent*)
{
   arrangeIcons();
}

void QuickLauncher :: dragEnterEvent(QDragEnterEvent *e)
{
   e->accept(QUriDrag::canDecode(e));
}

void QuickLauncher :: dropEvent(QDropEvent *e)
{
   QStringList uriList;
   if (QUriDrag::decodeToUnicodeUris(e, uriList) && uriList.count() > 0) {
      // calculate the position of the new icon
      int i;
      if (orientation() == Vertical) {
         i = (e->pos().y() - 2) / iconSize;
         if (width() >= 2*iconSize) {
            i *= 2;
            if (e->pos().x() > width() / 2) i++;
         }
      } else {
         i = (e->pos().x() - 2) / iconSize;
         if (height() >= 2*iconSize) {
            i *= 2;
            if (e->pos().y() > height() / 2) i++;
         }
      }
      QStringList::ConstIterator it(uriList.begin());
      for (; it != uriList.end(); ++it)
         addApp(i, *it);
      saveConfig();
   }
}

void QuickLauncher :: arrangeIcons()
{
   int space, padding = 2, row = 0, col = 0, i = 0;
   QuickButton *button;

   int tmp = iconSize;
   if (orientation() == Vertical) space = width();
   else space = height();
   if (space >= 54) {
      iconSize = 24; padding = 2;
   } else {
      iconSize = 20; padding = 1;
   }

   if (tmp != iconSize) {
      for (button = buttonList.first(); button; button = buttonList.next())
         button->resize(iconSize, iconSize);
   }

   if (orientation() == Vertical) {
      for (button = buttonList.first(); button; button = buttonList.next()) {
         if ((buttonList.count() == 1) || width() < 2*iconSize)
            button->move(width() / 2 - iconSize/2, 2 + i * iconSize);
         else {
            button->move(((i % 2) == 0) ? padding : width() - iconSize - padding, 2 + row * iconSize);
            if (i % 2) row++;
         }
         i++;
      }
   } else {
      for (button = buttonList.first(); button; button = buttonList.next()) {
         if ((buttonList.count() == 1) || height() < 2*iconSize)
            button->move(2 + i * iconSize, height() / 2 - iconSize/2);
         else {
            button->move(2 + col * iconSize, ((i % 2) == 0) ? padding : height() - iconSize - padding);
            if (i % 2) col++;
         }
         i++;
      }
   }

   updateGeometry();
   emit updateLayout();
}

void QuickLauncher :: saveConfig()
{
   KConfig *c = config();
   c->setGroup("General");
   QStringList urls;
   for (QuickButton *button = buttonList.first(); button; button = buttonList.next())
      urls.append(button->getURL());
   c->writeEntry("Buttons", urls);
   c->sync();
}
#include "quicklauncher.moc"
