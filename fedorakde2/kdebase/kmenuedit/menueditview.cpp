/*
 *   Copyright (C) 2000 Matthias Elter <elter@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <qsplitter.h>
#include <qvaluelist.h>

#include <kglobal.h>
#include <kconfig.h>
#include <kaction.h>

#include "treeview.h"
#include "desktopfileeditor.h"
#include "menueditview.h"
#include "menueditview.moc"

MenuEditView::MenuEditView( KActionCollection* ac, QWidget *parent, const char *name )
  : QVBox(parent, name), _ac(ac)
{
    _splitter = new QSplitter(Horizontal, this);
    _tree = new TreeView(_ac, _splitter);
    _editor = new DesktopFileEditor(_splitter);

    connect(_tree, SIGNAL(entrySelected(const QString&)),
	    _editor, SLOT(setDesktopFile(const QString&)));
    connect(_tree, SIGNAL(entrySelected(const QString&)),
	    SIGNAL(pathChanged(const QString&)));
    connect(_editor, SIGNAL(changed()), _tree, SLOT(currentChanged()));

    // restore splitter sizes
    KConfig* config = KGlobal::config();
    QValueList<int> sizes = config->readIntListEntry("SplitterSizes");

    if (sizes.isEmpty())
	sizes << 1 << 3;
    _splitter->setSizes(sizes);
    _tree->setFocus();
}

MenuEditView::~MenuEditView()
{
    // save splitter sizes
    KConfig* config = KGlobal::config();
    config->setGroup("General");
    config->writeEntry("SplitterSizes", _splitter->sizes());
    config->sync();
}
