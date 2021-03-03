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

#include "quickbutton.h"
#include "quickaddappsmenu.h"

#include <qpainter.h>
#include <qdrawutil.h>
#include <qpopupmenu.h>
#include <qdragobject.h>
#include <qtooltip.h>

#include <kmimetype.h>
#include <klocale.h>
#include <kdesktopfile.h>
#include <krun.h>
#include <kiconeffect.h>
#include <kglobalsettings.h>
#include <kcursor.h>
#include <kapp.h>
#include <kipc.h>

QuickButton :: QuickButton(const QString &u, QWidget *parent, const char *name)
   : QButton(parent, name)
{
   setBackgroundMode(X11ParentRelative);
   setMouseTracking(true);
   highlight = false;
   oldCursor = cursor();
   url = u;
   KURL kurl(u);
   local = kurl.isLocalFile();

   if (local && KDesktopFile::isDesktopFile(url)) {
      KDesktopFile df(url);
      QToolTip::add(this, df.readName() + " - " + df.readComment());
   }
   else {
      QToolTip::add(this, url);
   }

   icon = KMimeType::pixmapForURL(KURL(url), 0, KIcon::Panel, KIcon::SizeSmall, KIcon::DefaultState);
   iconh = KMimeType::pixmapForURL(KURL(url), 0, KIcon::Panel, KIcon::SizeSmall, KIcon::ActiveState);
   resize(20, 20);

   QuickAddAppsMenu *addAppsMenu = new QuickAddAppsMenu(parent, this);
   popup = new QPopupMenu(this);
   popup->insertItem(i18n("Add Application"), addAppsMenu);
   popup->insertSeparator();
   popup->insertItem(SmallIcon("exec"), i18n("Open"), this, SLOT(slotExec()));
   popup->insertItem(i18n("Remove"), this, SLOT(slotRemove()));

   slotSettingsChanged(KApplication::SETTINGS_MOUSE);
   connect(kapp, SIGNAL(settingsChanged(int)), SLOT(slotSettingsChanged(int)));
   connect(kapp, SIGNAL(iconChanged(int)), SLOT(slotIconChanged(int)));
   connect(this, SIGNAL(clicked()), SLOT(slotExec()));
   connect(this, SIGNAL(removeIcon(QuickButton *)), parent, SLOT(removeIcon(QuickButton *)));
   kapp->addKipcEventMask(KIPC::SettingsChanged);
   kapp->addKipcEventMask(KIPC::IconChanged);
}

QuickButton :: ~QuickButton()
{
}

QString QuickButton :: getURL()
{
   return url;
}

void QuickButton :: drawButton(QPainter *p)
{
   if (isDown() || isOn()) {
      p->fillRect(rect(), colorGroup().brush(QColorGroup::Mid));
      qDrawWinButton(p, 0, 0, width(), height(), colorGroup(), true);
   }

   drawButtonLabel(p);
}

void QuickButton :: drawButtonLabel(QPainter *p)
{
   QPixmap *pix = &icon;
   if (highlight) pix = &iconh;

   int d = 0;
   if (isDown() || isOn()) d = 1;

   if (width() == 24)
      p->drawPixmap(4 + d, 4 + d, *pix);
   else
      p->drawPixmap(2 + d, 2 + d, *pix);
}

void QuickButton :: enterEvent(QEvent*)
{
   if (changeCursorOverItem)
      setCursor(KCursor().handCursor());

   highlight = true;
   repaint();
}

void QuickButton :: leaveEvent(QEvent*)
{
   if (changeCursorOverItem)
      setCursor(oldCursor);

   highlight = false;
   repaint();
}

void QuickButton :: mousePressEvent(QMouseEvent *e)
{
   QButton::mousePressEvent(e);
   if (e->button() == RightButton)
      popup->popup(e->globalPos());
   else if ( e->button() == LeftButton)
       dragPos = e->pos();
}

void QuickButton :: mouseMoveEvent(QMouseEvent *e)
{
   if ((e->state() & LeftButton) == 0) return;
   QPoint p(e->pos() - dragPos);
   if (p.manhattanLength() <= KGlobalSettings::dndEventDelay())
      return;
   
   setDown(false);
   QStrList uris;
   uris.append(url.latin1());
   QDragObject *dd = new QUriDrag(uris, this);
   dd->setPixmap(icon);

   dd->drag();
}

void QuickButton :: slotSettingsChanged(int category)
{
   if (category != KApplication::SETTINGS_MOUSE) return;

   changeCursorOverItem = KGlobalSettings::changeCursorOverIcon();

   if(!changeCursorOverItem)
      setCursor(oldCursor);
}

void QuickButton :: slotIconChanged(int)
{
   icon = KMimeType::pixmapForURL(KURL(url), 0, KIcon::Panel, KIcon::SizeSmall, KIcon::DefaultState);
   iconh = KMimeType::pixmapForURL(KURL(url), 0, KIcon::Panel, KIcon::SizeSmall, KIcon::ActiveState);
   repaint();
}

void QuickButton :: slotExec()
{
    KIconEffect::visualActivate(this, rect());
    kapp->propagateSessionManager();   // is this needed?
    new KRun(url, 0, local);
}

void QuickButton :: slotRemove()
{
    emit removeIcon(this);
}
#include "quickbutton.moc"
