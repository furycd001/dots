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

#include <kglobal.h>
#include <klocale.h>

#include "khc_factory.h"
#include "khc_navigator.h"
#include "khc_navigatoritem.h"

KInstance *KHCFactory::s_instance = 0L;

extern "C"
{
  void *init_libkhelpcenterpart()
  {
    return new KHCFactory;
  }
};

KHCFactory::KHCFactory()
{
  s_instance = 0L;
}

KHCFactory::~KHCFactory()
{
  if ( s_instance )
    delete s_instance;

  s_instance = 0L;
}

QObject* KHCFactory::create( QObject* parent, const char* name, const char* /*classname*/, const QStringList & )
{
    KGlobal::locale()->insertCatalogue(QString::fromLatin1("khelpcenter"));
    khcNavigator *nav = new khcNavigator( (QWidget *)parent, parent, name );

    emit objectCreated( nav );

    return nav;
}

KInstance *KHCFactory::instance()
{
  if ( !s_instance )
    s_instance = new KInstance( "khelpcenter" );

  return s_instance;
}

#include "khc_factory.moc"
