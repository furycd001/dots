/***************************************************************************
                          kateIface.h  -  description
                             -------------------
    begin                : Wed Jan 3 2001
    copyright            : (C) 2001 by Christoph "Crossfire" Cullmann
    email                : crossfire@babylon2k.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef _katemainwindow_Iface_h_
#define _katemainwindow_Iface_h_

#include <dcopobject.h>

class KateMainWindowDCOPIface : virtual public DCOPObject
{
  K_DCOP

  k_dcop:
    virtual void openURL (const QString &name=0)=0;
};
#endif
