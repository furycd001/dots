/*
 * main.cpp
 *
 * Copyright (c) 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include <kcmodule.h>
#include <kglobal.h>
#include <klocale.h>

#include "memory.h"

 
/* we have to include the info.cpp-file, to get the DEFINES about possible properties.
   example: we need the "define INFO_CPU_AVAILABLE" */
#include "info.cpp"
#include "info.h" 


extern "C"
{

  KCModule *create_cpu(QWidget *parent, const char * /*name*/)
  { 
#ifdef INFO_CPU_AVAILABLE
    KGlobal::locale()->insertCatalogue("kcminfo");
    return new KInfoListWidget(i18n("Processor(s)"), parent, "Processor(s)", GetInfo_CPU);
#else
    return 0;
#endif
  }

  KCModule *create_irq(QWidget *parent, const char * /*name*/)
  { 
#ifdef INFO_IRQ_AVAILABLE
    KGlobal::locale()->insertCatalogue("kcminfo");
    return new KInfoListWidget(i18n("Interrupt"), parent, "Interrupt", GetInfo_IRQ);
#else
    return 0;
#endif
  }

  KCModule *create_pci(QWidget *parent, const char * /*name*/)
  { 
#ifdef INFO_PCI_AVAILABLE
    KGlobal::locale()->insertCatalogue("kcminfo");
    return new KInfoListWidget(i18n("PCI"), parent, "PCI", GetInfo_PCI);
#else
    return 0;
#endif
  }

  KCModule *create_dma(QWidget *parent, const char * /*name*/)
  { 
#ifdef INFO_DMA_AVAILABLE
    KGlobal::locale()->insertCatalogue("kcminfo");
    return new KInfoListWidget(i18n("DMA-Channel"), parent, "DMA-Channel", GetInfo_DMA);
#else
    return 0;
#endif
  }

  KCModule *create_ioports(QWidget *parent, const char * /*name*/)
  { 
#ifdef INFO_IOPORTS_AVAILABLE
    KGlobal::locale()->insertCatalogue("kcminfo");
    return new KInfoListWidget(i18n("I/O-Port"), parent, "I/O-Port", GetInfo_IO_Ports);
#else
    return 0;
#endif
  }

  KCModule *create_sound(QWidget *parent, const char * /*name*/)
  { 
#ifdef INFO_SOUND_AVAILABLE
    KGlobal::locale()->insertCatalogue("kcminfo");
    return new KInfoListWidget(i18n("Soundcard"), parent, "Soundcard", GetInfo_Sound);
#else
    return 0;
#endif
  }

  KCModule *create_scsi(QWidget *parent, const char * /*name*/)
  { 
#ifdef INFO_SCSI_AVAILABLE
    KGlobal::locale()->insertCatalogue("kcminfo");
    return new KInfoListWidget(i18n("SCSI"), parent, "SCSI", GetInfo_SCSI);
#else
    return 0;
#endif
  }

  KCModule *create_devices(QWidget *parent, const char * /*name*/)
  { 
#ifdef INFO_DEVICES_AVAILABLE
    KGlobal::locale()->insertCatalogue("kcminfo");
    return new KInfoListWidget(i18n("Devices"), parent, "Devices", GetInfo_Devices);
#else
    return 0;
#endif
  }

  KCModule *create_partitions(QWidget *parent, const char * /*name*/)
  { 
#ifdef INFO_PARTITIONS_AVAILABLE
    KGlobal::locale()->insertCatalogue("kcminfo");
    return new KInfoListWidget(i18n("Partitions"), parent, "Partitions", GetInfo_Partitions);
#else
    return 0;
#endif
  }

  KCModule *create_xserver(QWidget *parent, const char * /*name*/)
  { 
#ifdef INFO_XSERVER_AVAILABLE
    KGlobal::locale()->insertCatalogue("kcminfo");
    return new KInfoListWidget(i18n("X-Server"), parent, "X-Server", GetInfo_XServer_and_Video);
#else
    return 0;
#endif
  }

  KCModule *create_memory(QWidget *parent, const char * /*name*/)
  { 
    KGlobal::locale()->insertCatalogue("kcminfo");
    return new KMemoryWidget(parent, "Memory");
  }

}
