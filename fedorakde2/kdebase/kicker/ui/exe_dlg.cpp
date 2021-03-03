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

#include <qlabel.h>
#include <qlayout.h>
#include <qfileinfo.h>

#include <kbuttonbox.h>
#include <kseparator.h>
#include <klocale.h>
#include <kiconloader.h>

#include "exe_dlg.h"

PanelExeDialog::PanelExeDialog(const QString &path, const QString &pixmap,
                               const QString &cmd, bool inTerm,
                               QWidget *parent, const char *name)
    : QDialog(parent, name, true)
{
    setCaption(i18n("Non-KDE application configuration"));
    QFileInfo fi(path);
    QLabel *fileLbl = new QLabel(i18n("Filename: ") + fi.fileName(), this);
    QLabel *clLbl = new QLabel(i18n("Optional command line arguments:"), this);
    clEdit = new QLineEdit(cmd, this);
    termBtn = new QCheckBox(i18n("Run in terminal."), this);
    termBtn->setChecked(inTerm);
    iconBtn = new KIconButton(this);
    iconBtn->setIconType(KIcon::Panel, KIcon::Application);
    if(!pixmap.isEmpty())
        iconBtn->setIcon(pixmap);
    KButtonBox *bbox = new KButtonBox(this);
    bbox->addStretch(1);
    QButton *btn = bbox->addButton(i18n("&OK"));
    connect(btn, SIGNAL(clicked()), this, SLOT(accept()));
    btn = bbox->addButton(i18n("&Cancel"));
    connect(btn, SIGNAL(clicked()), this, SLOT(reject()));
    bbox->layout();

    QGridLayout *layout = new QGridLayout(this, 9, 3, 4);
    layout->addMultiCellWidget(fileLbl, 0, 0, 0, 2);
    layout->addRowSpacing(1, 10);
    layout->addMultiCellWidget(new KSeparator(QFrame::HLine, this), 2, 2, 0, 2);
    layout->addMultiCellWidget(iconBtn, 3, 5, 2, 2);
    layout->addMultiCellWidget(clLbl, 3, 3, 0, 1);
    layout->addMultiCellWidget(clEdit, 4, 4, 0, 1);
    layout->addMultiCellWidget(termBtn, 5, 5, 0, 1);
    layout->addRowSpacing(6, 10);
    layout->addMultiCellWidget(new KSeparator(QFrame::HLine, this), 7, 7, 0, 2);
    layout->addMultiCellWidget(bbox, 8, 8, 0, 2);
    layout->setRowStretch(6, 1);
    layout->setColStretch(0, 1);
    layout->activate();
    // leave decent space for the commandline
    resize(sizeHint().width() > 300 ? sizeHint().width() : 300,
           sizeHint().height());
}



