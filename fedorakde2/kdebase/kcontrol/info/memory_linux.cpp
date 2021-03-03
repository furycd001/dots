#include <syscall.h>
#include <linux/kernel.h>
#include <sys/user.h>
#include <unistd.h>
#include <stdlib.h>
#include <qfile.h>

/* $Id: memory_linux.cpp,v 1.4 2001/05/27 16:19:53 deller Exp $ */

void KMemoryWidget::update()
{
  struct sysinfo info;
  int	shift_val;
  
  syscall(SYS_sysinfo, &info);	/* Get Information from system... */
  
  /* try to fix the change, introduced with kernel 2.3.25, which
     now counts the memory-information in pages (not bytes anymore) */
  if (info.totalram < (4*1024*1024)) /* smaller than 4MB ? */
      shift_val = PAGE_SHIFT;
  else
      shift_val = 0;

  Memory_Info[TOTAL_MEM]    = MEMORY(info.totalram  << shift_val); // total physical memory (without swaps)
  Memory_Info[FREE_MEM]     = MEMORY(info.freeram   << shift_val); // total free physical memory (without swaps)
  Memory_Info[SHARED_MEM]   = MEMORY(info.sharedram << shift_val); 
  Memory_Info[BUFFER_MEM]   = MEMORY(info.bufferram << shift_val); 
  Memory_Info[SWAP_MEM]     = MEMORY(info.totalswap << shift_val); // total size of all swap-partitions
  Memory_Info[FREESWAP_MEM] = MEMORY(info.freeswap  << shift_val); // free memory in swap-partitions

  
  QFile file("/proc/meminfo");
  if (file.open(IO_ReadOnly)) {
	char buf[512];
	while (file.readLine(buf, sizeof(buf) - 1) > 0) {
		if (strncmp(buf,"Cached:",7)==0) {
			unsigned long v;
			v = strtoul(&buf[7],NULL,10);			
			Memory_Info[CACHED_MEM] = MEMORY(v)*1024; // Cached memory in RAM
		}
	}
	file.close();
  }
}

