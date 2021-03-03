/* 	$Id: info_generic.cpp,v 1.5 1999/07/27 22:34:51 dmuell Exp $

	1998 by Helge Deller (deller@gmx.de)
	free source under GPL
	
	!!!!! this file will be included by info.cpp !!!!!
*/


// Default for unsupportet systems

// the following defines are not really ok here, but maybe we should show, what
// Information could be displayed here....

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
       

bool GetInfo_CPU( QListView * )
{
	return false;
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
