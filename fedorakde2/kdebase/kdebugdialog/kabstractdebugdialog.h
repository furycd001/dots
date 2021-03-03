/* This file is part of the KDE libraries
    Copyright (C) 2000 David Faure <faure@kde.org>

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

#ifndef KABSTRACTDEBUGDIALOG__H
#define KABSTRACTDEBUGDIALOG__H

#include <kdialog.h>

class KConfig;
class QVBoxLayout;

class KAbstractDebugDialog : public KDialog
{
  Q_OBJECT
public:
  KAbstractDebugDialog( QWidget *parent=0, const char *name=0, bool modal=true );

  virtual ~KAbstractDebugDialog();

  virtual void buildButtons(QVBoxLayout * topLayout);

  virtual void save() = 0;
  KConfig * config() { return pConfig; }

protected slots:
  void slotShowHelp();
protected:
  KConfig* pConfig;
  QPushButton* pOKButton;
  QPushButton* pCancelButton;
  QPushButton* pHelpButton;
};

#endif
