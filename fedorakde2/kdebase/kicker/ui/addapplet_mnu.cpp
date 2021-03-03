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

#include <kglobal.h>
#include <kconfig.h>
#include <kstddirs.h>

#include "addapplet_mnu.h"
#include "addapplet_mnu.moc"
#include "containerarea.h"

PanelAddAppletMenu::PanelAddAppletMenu(ContainerArea* cArea, QWidget *parent, const char *name)
    : QPopupMenu(parent, name), containerArea(cArea)
{
    setCheckable(true);
    applets.setAutoDelete(true);
    connect(this, SIGNAL(activated(int)), SLOT(slotExec(int)));
    connect(this, SIGNAL(aboutToShow()), SLOT(slotAboutToShow()));
}

void PanelAddAppletMenu::slotAboutToShow()
{
    clear();
    applets.clear();

    QStringList list = KGlobal::dirs()->findAllResources("applets", "*.desktop");

    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
        AppletInfo *ai = new AppletInfo(*it);

        applets.append(ai);
        insertItem(ai->name(), applets.count()-1);
        if (ai->isUniqueApplet() && containerArea->hasInstance(ai)) {
            setItemEnabled(applets.count()-1, false);
            setItemChecked(applets.count()-1, true);
        }
    }
    adjustSize();
}

void PanelAddAppletMenu::slotExec(int id)
{
    if(id >= 0 && !applets.at(id)->desktopFile().isNull())
	containerArea->addApplet(applets.at(id)->desktopFile() );
}
