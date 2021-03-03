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

#ifndef __basictab_h__
#define __basictab_h__

#include <qwidget.h>
#include <qstring.h>

#include <klineedit.h>

class KLineEdit;
class KIconButton;
class QCheckBox;
class QGroupBox;
class KURLRequester;

class BasicTab : public QWidget
{
    Q_OBJECT

public:
    BasicTab( QWidget *parent=0, const char *name=0 );

    void apply( bool desktopFileNeedsSave );
    void reset();

signals:
    void changed();
    void changed( bool desktopFileNeedsSave );

public slots:
    void setDesktopFile(const QString& desktopFile);

protected slots:
    void slotChanged(const QString&);
    void slotChanged();
    void termcb_clicked();
    void uidcb_clicked();
    void keyButtonPressed();

protected:
    KLineEdit    *_nameEdit, *_commentEdit, *_typeEdit, *_keyEdit;
    KURLRequester *_execEdit, *_pathEdit;
    KLineEdit    *_termOptEdit, *_uidEdit;
    QCheckBox    *_terminalCB, *_uidCB;
    KIconButton  *_iconButton;
    QGroupBox    *_path_group, *_term_group, *_uid_group, *general_group_keybind;

    QString       _desktopFile;
    bool _khotkeysNeedsSave;
};

#endif
