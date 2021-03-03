/*
  Copyright (c) 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>

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

#ifndef __modloader_h__
#define __modloader_h__

#include <qwidget.h>

#include <kcmodule.h>
#include "moduleinfo.h"

class ModuleLoader
{
public:
  static KCModule *loadModule(const ModuleInfo &mod, bool withfallback=true);
  static void unloadModule(const ModuleInfo &mod);
};

#endif
