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

#include "addextension_mnu.h"
#include "addextension_mnu.moc"
#include "extensionmanager.h"


PanelAddExtensionMenu::PanelAddExtensionMenu(QWidget *parent, const char *name)
    : QPopupMenu(parent, name)
{
    setCheckable(true);
    extensions.setAutoDelete(true);
    connect(this, SIGNAL(activated(int)), SLOT(slotExec(int)));
    connect(this, SIGNAL(aboutToShow()), SLOT(slotAboutToShow()));
}

void PanelAddExtensionMenu::slotAboutToShow()
{
    clear();
    extensions.clear();

    if(!PGlobal::extensionManager)
        return;

    QStringList list = KGlobal::dirs()->findAllResources("extensions", "*.desktop");

    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
        AppletInfo *ai = new AppletInfo(*it);

        extensions.append(ai);
        insertItem(ai->name(), extensions.count()-1);

        if (ai->isUniqueApplet() && PGlobal::extensionManager->hasInstance(ai)) {
            setItemEnabled(extensions.count()-1, false);
            setItemChecked(extensions.count()-1, true);
        }
    }
    adjustSize();
}

void PanelAddExtensionMenu::slotExec(int id)
{
    if(!PGlobal::extensionManager)
        return;

    if(id < 0 || extensions.at(id)->desktopFile().isNull())
        return;

    bool internal = false;

    KConfig *c = KGlobal::config();
    c->setGroup("General");
    if(c->readNumEntry("SecurityLevel") > 1)
        internal = true;

    PGlobal::extensionManager->addExtension(extensions.at(id)->desktopFile(), internal);
}
