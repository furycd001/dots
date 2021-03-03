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

#include <khelpmenu.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <kfiledialog.h>
#include <kprocess.h>
#include <kstddirs.h>
#include <kmessagebox.h>
#include <kpopupmenu.h>

#include "panelop_mnu.h"
#include "panelop_mnu.moc"

#include "addcontainer_mnu.h"
#include "panel.h"
#include "containerarea.h"
#include "global.h"

PanelOpMenu::PanelOpMenu(bool /*title*/, QWidget *parent, const char *name)
    : QPopupMenu(parent, name)
{
    // setup size menu
    sizeMnu = new QPopupMenu(this);
    sizeMnu->setCheckable(true);
    sizeMnu->insertItem(i18n("Tiny"), Tiny);
    sizeMnu->insertItem(i18n("Small"), Small);
    sizeMnu->insertItem(i18n("Normal"), Normal);
    sizeMnu->insertItem(i18n("Large"), Large);
    connect(sizeMnu, SIGNAL(aboutToShow()), SLOT(slotSetupSizeMnu()));
    connect(sizeMnu, SIGNAL(activated(int)), SLOT(slotSetSize(int)));

    // setup help menu
    help = new KHelpMenu(0, KGlobal::instance()->aboutData(), false);
    KPopupMenu *helpMnu = help->menu();

    // build menu
    insertItem(i18n("&Add"), new AddContainerMenu(PGlobal::panel->containerArea(), this));
    insertSeparator();
    insertItem(i18n("Si&ze"), sizeMnu);

    insertItem(SmallIconSet("configure"), i18n("&Preferences..."), this, SLOT(slotConfigure()));
    insertItem(SmallIconSet("kmenuedit"), i18n("&Menu Editor..."), this, SLOT(slotMenuEditor()));
    insertSeparator();

    insertItem(SmallIconSet("help"), i18n("&Help"), helpMnu);
    adjustSize();
}

PanelOpMenu::~PanelOpMenu()
{
    delete help;
}

void PanelOpMenu::slotSetupSizeMnu()
{
    sizeMnu->setItemChecked(Tiny, false);
    sizeMnu->setItemChecked(Small, false);
    sizeMnu->setItemChecked(Normal, false);
    sizeMnu->setItemChecked(Large, false);
    sizeMnu->setItemChecked(PGlobal::panel->size(), true);
}

void PanelOpMenu::slotSetSize(int size)
{
    setItemChecked(PGlobal::panel->size(), false);
    setItemChecked(size, true);
    PGlobal::panel->setSize(static_cast<Size>(size));
}

void PanelOpMenu::slotConfigure()
{
    PanelContainer::writeContainerConfig();

    KProcess proc;
    proc << locate("exe", "kcmshell");
    proc << "panel";
    proc << "kcmtaskbar";
    proc.start(KProcess::DontCare);
}

void PanelOpMenu::slotMenuEditor()
{
    PanelContainer::writeContainerConfig();

    KProcess proc;
    proc << locate("exe", "kmenuedit");
    proc.start(KProcess::DontCare);
}

