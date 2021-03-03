/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef LISTBOXEDITOR_H
#define LISTBOXEDITOR_H

class QListBox;
class FormWindow;

#include "listboxeditor.h"

class ListBoxEditor : public ListBoxEditorBase
{
    Q_OBJECT

public:
    ListBoxEditor( QWidget *parent, QWidget *editWidget, FormWindow *fw );

protected slots:
    void insertNewItem();
    void deleteCurrentItem();
    void currentItemChanged( QListBoxItem * );
    void currentTextChanged( const QString & );
    void okClicked();
    void cancelClicked();
    void applyClicked();
    void choosePixmap();
    void moveItemUp();
    void moveItemDown();
    void deletePixmap();

private:
    QListBox *listbox;
    FormWindow *formwindow;
    
};

#endif
