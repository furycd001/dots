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

#ifndef MULTILINEEDITORIMPL_H
#define MULTILINEEDITORIMPL_H

#include "multilineeditor.h"

class FormWindow;
class QMultiLineEdit;

class MultiLineEditor : public MultiLineEditorBase
{
    Q_OBJECT

public:
    MultiLineEditor( QWidget *parent, QWidget *editWidget, FormWindow *fw );

protected slots:
    void okClicked();
    void applyClicked();

private:
    QMultiLineEdit *mlined;
    FormWindow *formwindow;

};

class TextEditor : public MultiLineEditorBase
{
    Q_OBJECT

public:
    TextEditor( QWidget *parent, const QString &text );

    static QString getText( QWidget *parent, const QString &text );
    
protected slots:
    void okClicked();
    void applyClicked();

};

#endif
