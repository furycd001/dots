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

#ifndef __exe_dlg_h__
#define __exe_dlg_h__

#include <qdialog.h>
#include <qlineedit.h>
#include <qcheckbox.h>

#include <kicondialog.h>

class PanelExeDialog : public QDialog
{
public:
    PanelExeDialog(const QString &path, const QString &pixmap=QString::null,
                   const QString &cmd=QString::null, bool inTerm=false,
                   QWidget *parent=0, const char *name=0);
    bool useTerminal(){return(termBtn->isChecked());}
    const QString commandLine(){return(clEdit->text());}
    const QString icon(){return(iconBtn->icon());}
protected:
    QLineEdit *clEdit;
    KIconButton *iconBtn;
    QCheckBox *termBtn;
};

#endif
