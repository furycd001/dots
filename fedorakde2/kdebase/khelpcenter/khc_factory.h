/*  This file is part of the KDE project
    Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef __khc_factory_h__
#define __khc_factory_h__ $Id: khc_factory.h,v 1.1 1999/11/18 16:33:53 shausman Exp $

#include <klibloader.h>
#include <kinstance.h>

class KHCFactory : public KLibFactory
{
  Q_OBJECT
public:
  KHCFactory();
  virtual ~KHCFactory();

  virtual QObject* create( QObject* parent = 0, const char* name = 0, const char* classname = "QObject", const QStringList &args = QStringList() );

  static KInstance *instance();	

private:
  static KInstance *s_instance;
};

#endif
