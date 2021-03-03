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

#ifndef __menueditview_h__
#define __menueditview_h__

#include <qvbox.h>

class TreeView;
class DesktopFileEditor;
class QSplitter;
class KActionCollection;

class MenuEditView : public QVBox
{
    Q_OBJECT

public:
    MenuEditView( KActionCollection*, QWidget *parent=0, const char *name=0 );
    ~MenuEditView();

signals:
    void pathChanged(const QString&);

protected:
    KActionCollection  *_ac;
    TreeView           *_tree;
    DesktopFileEditor  *_editor;
    QSplitter          *_splitter;
};

#endif
