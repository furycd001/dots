/* 	info_sgi.cpp
	
	!!!!! this file will be included by info.cpp !!!!!
*/

#define INFO_CPU_AVAILABLE
#define INFO_IRQ_AVAILABLE
#define INFO_DMA_AVAILABLE
#define INFO_PCI_AVAILABLE
#define INFO_IOPORTS_AVAILABLE
#define INFO_SOUND_AVAILABLE
#define INFO_DEVICES_AVAILABLE
#define INFO_SCSI_AVAILABLE
#define INFO_PARTITIONS_AVAILABLE
#define INFO_XSERVER_AVAILABLE


/*  all following functions should return TRUE, when the Information 
    was filled into the lBox-Widget.
    returning false indicates, that information was not available.
*/
       

#include <sys/systeminfo.h>

bool GetInfo_CPU( QListView *lBox )
{
      QString str;
      char buf[256];

      sysinfo(SI_ARCHITECTURE, buf, sizeof(buf));
      str = QString::fromLocal8Bit(buf);
      new QListViewItem(lBox, str);
      return true;
}


bool GetInfo_IRQ( QListView * )
{
	return false;
}

bool GetInfo_DMA( QListView * )
{
	return false;
}

bool GetInfo_PCI( QListView * )
{
	return false;
}

bool GetInfo_IO_Ports( QListView * )
{
	return false;
}

bool GetInfo_Sound( QListView * )
{
	return false;
}

bool GetInfo_Devices( QListView * )
{
	return false;
}

bool GetInfo_SCSI( QListView * )
{
	return false;
}

bool GetInfo_Partitions( QListView * )
{
	return false;
}

bool GetInfo_XServer_and_Video( QListView *lBox )
{
	return GetInfo_XServer_Generic( lBox );
}

