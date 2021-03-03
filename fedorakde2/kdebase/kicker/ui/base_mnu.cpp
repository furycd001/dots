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

#include "base_mnu.h"
#include "base_mnu.moc"

PanelMenu::PanelMenu(const QString &startDir, QWidget *parent, const char *name)
  : QPopupMenu(parent, name)
{
    init = false;

    startPath = startDir;

    connect(this, SIGNAL(activated(int)), SLOT(slotExec(int)));
    connect(this, SIGNAL(aboutToShow()), SLOT(slotAboutToShow()));

    // setup cache timer
    KConfig *config = KGlobal::config();
    config->setGroup("menus");
    clearDelay = config->readNumEntry("MenuCacheTime", 60000); // 1 minute
}

void PanelMenu::slotAboutToShow()
{
    // stop the cache timer
    if(clearDelay)
	t.stop();

    // teared off ?
    if ( isTopLevel() )
	clearDelay = 0;

    initialize();
}

void PanelMenu::slotClear()
{
    clear();
    init = false;
}

void PanelMenu::hideEvent(QHideEvent *ev)
{
    // start the cache timer
    if(clearDelay) {
        disconnect(&t, SIGNAL(timeout()), this, SLOT(slotClear()));
        connect(&t, SIGNAL(timeout()), this, SLOT(slotClear()));
        t.start(clearDelay, true);
    }
    QPopupMenu::hideEvent(ev);
}

void PanelMenu::disableAutoClear()
{
    clearDelay = 0;
}
