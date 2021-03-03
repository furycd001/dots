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

#include <klocale.h>
#include <kiconloader.h>
#include <kpanelextension.h>

#include "panelop_mnu.h"
#include "extensionop_mnu.h"

PanelExtensionOpMenu::PanelExtensionOpMenu(int actions, QWidget *parent, const char *name)
  : QPopupMenu(parent, name)
{
    insertItem(SmallIcon("panel"), i18n("Panel"), new PanelOpMenu(false, this));
    insertSeparator();

    insertItem(i18n("&Shade"), Shade);
    setAccel(CTRL+Key_S, Shade);
    setItemChecked( Shade, false );

    insertItem(SmallIcon("move"), i18n("&Move"), Move);
    setAccel(CTRL+Key_M, Move);

    insertItem(SmallIcon("remove"), i18n("&Remove"), Remove);
    setAccel(CTRL+Key_R, Remove);

    if (actions & KPanelExtension::ReportBug)
    {
        insertSeparator();
        insertItem(i18n("Report &Bug..."), ReportBug);
        setAccel(CTRL+Key_B, ReportBug);
    }

    if (actions & KPanelExtension::Help
        || actions & KPanelExtension::About)
	insertSeparator();

    if (actions & KPanelExtension::About)
    {
        insertItem(i18n("&About..."), About);
        setAccel(CTRL+Key_A, About);
    }

    if (actions & KPanelExtension::Help)
    {
        insertItem(SmallIcon("help"), i18n("&Help..."), Help);
        setAccel(CTRL+Key_H, Help);
    }

    if (actions & KPanelExtension::Preferences)	{
	insertSeparator();
	insertItem(SmallIcon("configure"), i18n("&Preferences..."), Preferences);
	setAccel(CTRL+Key_P, Preferences);
    }

    adjustSize();
}
