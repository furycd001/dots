/*****************************************************************

Copyright (c) 1996-2000 the kicker authors. See file AUTHORS.

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

#include <qdir.h>
#include <qdragobject.h>

#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kconfig.h>
#include <kstddirs.h>
#include <kdesktopfile.h>
#include <kglobalsettings.h>
#include <kapp.h>
#include <kurldrag.h>
#include <krecentdocument.h>

#include "recent_mnu.h"
#include "recent_mnu.moc"

PanelRecentMenu::PanelRecentMenu(QWidget *parent, const char *name)
    : PanelMenu(KRecentDocument::recentDocumentDirectory(), parent, name) {}

void PanelRecentMenu::initialize()
{
    if (init) clear();
    init = true;

    insertItem(SmallIconSet("fileclose"), i18n("Clear History"), this, SLOT(slotClearHistory()));
    insertSeparator();

    fileList = KRecentDocument::recentDocuments();

    if (fileList.isEmpty()) {
	insertItem(i18n("No entries"), 0);
	setItemEnabled(0, false);
	return;
    }

    int id = 0;

    for (QStringList::ConstIterator it = fileList.begin(); it != fileList.end(); ++it) {

	KDesktopFile f(*it, true /* read only */);
	insertItem(SmallIconSet(f.readIcon()), f.readName(), id++);
    }
}

void PanelRecentMenu::slotClearHistory()
{
    KRecentDocument::clear();
}

void PanelRecentMenu::slotExec(int id)
{
    if (id >= 0) {
      kapp->propagateSessionManager();
      KApplication::startServiceByDesktopPath(fileList[id]);
    }
}

void PanelRecentMenu::mousePressEvent(QMouseEvent * e)
{
  mouseDown = e->pos();
  QPopupMenu::mousePressEvent(e);
}

void PanelRecentMenu::mouseMoveEvent(QMouseEvent * e)
{
  PanelMenu::mouseMoveEvent(e);

  if (!(e->state() & LeftButton))
    return;

  if (!rect().contains(mouseDown))
    return;

  int dragLength = (e->pos() - mouseDown).manhattanLength();

  if (dragLength > KGlobalSettings::dndEventDelay())
  {
    int id = idAt(mouseDown);

    // Don't drag 'manual' items.
    if (id < 0)
      return;

    KDesktopFile f(fileList[id], true /* read only */);

    KURL url = f.readURL();

    if (url.isEmpty()) // What are we to do ?
      return;
    KURL::List lst;
    lst.append(url);
    QUriDrag * d = KURLDrag::newDrag(lst,this);
    d->setPixmap(SmallIcon(f.readIcon()));
    d->dragCopy();
    close();
  }
}
