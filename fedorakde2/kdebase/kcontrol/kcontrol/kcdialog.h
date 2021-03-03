/*
   Copyright (c) 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
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

#ifndef __kcdialog_h__
#define __kcdialog_h__

#include <dcopobject.h>
#include <kdialogbase.h>

class KCModule;

class KCDialog : public KDialogBase, public DCOPObject
{
  Q_OBJECT
  K_DCOP
public:
  KCDialog(KCModule *client, int b, const QString &docpath=QString::null, QWidget *parent=0, const char *name=0, bool modal=false);

k_dcop:
  virtual void activate();

protected slots:
    //virtual void slotDefault();
  virtual void slotUser1(); // Reset
  virtual void slotApply();
  virtual void slotOk();
  void clientChanged(bool state);


private:
  KCModule    *_client;
};

#endif
