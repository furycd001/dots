/*****************************************************************

Copyright (c) 2000 Bill Nagel
   based on paneladdappsmenu.cpp which is
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

#include <kstddirs.h>
#include <kdesktopfile.h>
#include <kglobalsettings.h>
#include <ksycocaentry.h>
#include <kservice.h>
#include <kservicegroup.h>

#include "quickaddappsmenu.h"

QuickAddAppsMenu :: QuickAddAppsMenu(const QString &label, const QString &relPath, QObject *target, QWidget *parent, const char *name)
   : PanelServiceMenu(label, relPath, parent, name)
{
   targetObject = target;
   connect(this, SIGNAL(addApp(QString)), target, SLOT(slotAddApp(QString)));
}

QuickAddAppsMenu :: QuickAddAppsMenu(QObject *target, QWidget *parent, const char *name)
  : PanelServiceMenu(QString::null, QString::null, parent, name)
{
   targetObject = target;
   connect(this, SIGNAL(addApp(QString)), target, SLOT(slotAddApp(QString)));
}

void QuickAddAppsMenu :: slotExec(int id)
{
   if (!entryMap_.contains(id)) return;
   KSycocaEntry * e = entryMap_[id];
   KService::Ptr service = static_cast<KService *>(e);
   emit addApp(locate("apps", service->desktopEntryPath()));
}


PanelServiceMenu *QuickAddAppsMenu :: newSubMenu(const QString &label, const QString &relPath, QWidget *parent, const char *name)
{
   return new QuickAddAppsMenu(label, relPath, targetObject, parent, name);
}
#include "quickaddappsmenu.moc"
