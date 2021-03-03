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

#ifndef KLISTDEBUGDIALOG__H
#define KLISTDEBUGDIALOG__H

#include <kabstractdebugdialog.h>
#include <qcheckbox.h>
#include <qlist.h>

/**
 * Control debug output of KDE applications
 * This dialog offers a reduced functionality compared to the full KDebugDialog
 * class, but allows to set debug output on or off to several areas much more easily.
 *
 * @author David Faure <faure@kde.org>
 * @version $Id: klistdebugdialog.h,v 1.2 2000/08/07 10:39:10 faure Exp $
 */
class KListDebugDialog : public KAbstractDebugDialog
{
  Q_OBJECT

public:
  KListDebugDialog( QStringList areaList, QWidget *parent=0, const char *name=0, bool modal=true );
  virtual ~KListDebugDialog() {}

  void activateArea( QCString area, bool activate );

  virtual void save();

private:
  void load();
  QList<QCheckBox> boxes;
};

#endif
