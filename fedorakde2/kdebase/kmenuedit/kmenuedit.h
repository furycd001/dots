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

#ifndef __kmenuedit_h__
#define __kmenuedit_h__

#include <kmainwindow.h>

class MenuEditView;

class KMenuEdit : public KMainWindow
{
    Q_OBJECT

public:
    KMenuEdit( QWidget *parent=0, const char *name=0 );
    ~KMenuEdit();

protected:
    void setupView();
    void setupActions();

protected slots:
    void slotClose();

protected:
    MenuEditView *_view;
};

#endif
