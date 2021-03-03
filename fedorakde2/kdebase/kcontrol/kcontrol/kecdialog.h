/*
   Copyright (c) 2000 Matthias Elter <elter@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

*/

#ifndef __kecdialog_h__
#define __kecdialog_h__

#include <qlist.h>

#include <kdialogbase.h>
#include <kcmodule.h>

class KExtendedCDialog : public KDialogBase
{
    Q_OBJECT

public:
    KExtendedCDialog(QWidget *parent=0, const char *name=0, bool modal=false);
    virtual ~KExtendedCDialog();

    void addModule(const QString& module, bool withfallback=true);

protected slots:
    virtual void slotUser1(); // Reset
    virtual void slotApply();
    virtual void slotOk();
    void clientChanged(bool state);

private:
    QList<KCModule> modules;
};

#endif
