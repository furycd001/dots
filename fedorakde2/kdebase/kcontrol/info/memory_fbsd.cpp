// $Id: memory_fbsd.cpp,v 1.12 2001/05/27 16:19:53 deller Exp $

#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/vmmeter.h>
#include <vm/vm_param.h>
#include <stdlib.h>	// For atoi()
#include <stdio.h>
#include <unistd.h>

void KMemoryWidget::update()
{
    char blah[10], buf[80], *used_str, *total_str;
    /* Stuff for sysctl */
    int mib[2],memory;size_t len;
    /* Stuff for swap display */
    int used, total, _free;
    FILE *pipe;

    mib[0]=CTL_HW;mib[1]=HW_PHYSMEM;
    len=sizeof(memory);
    sysctl(mib, 2, &memory, &len, NULL, 0);
  
    snprintf(blah, 10, "%d", memory);
    // Numerical values
    Memory_Info[TOTAL_MEM]    = MEMORY(memory); // total physical memory (without swaps)
    /*	To: questions@freebsd.org
	Anyone have any ideas on how to calculate this */
    
    // added by Brad Hughes bhughes@trolltech.com
    struct vmtotal vmem;
    mib[0] = CTL_VM;
    mib[1] = VM_METER;
    
#warning "FIXME: Memory_Info[CACHED_MEM]"
    Memory_Info[CACHED_MEM] = NO_MEMORY_INFO;
    
    len = sizeof(vmem);
    
    if (sysctl(mib, 2, &vmem, &len, NULL, 0) == 0) 
	Memory_Info[SHARED_MEM]   = MEMORY((vmem.t_armshr * PAGE_SIZE));
      else 
        Memory_Info[SHARED_MEM]   = NO_MEMORY_INFO;

    int buffers;
    len = sizeof (buffers);
    if ((sysctlbyname("vfs.bufspace", &buffers, &len, NULL, 0) == -1) || !len)
	Memory_Info[BUFFER_MEM]   = NO_MEMORY_INFO;  // Doesn't work under FreeBSD v2.2.x
    else
	Memory_Info[BUFFER_MEM]   = MEMORY(buffers);

    int free;
    len = sizeof (buffers);
    if ((sysctlbyname("vm.stats.vm.v_free_count", &free, &len, NULL, 0) == -1) || !len)
	Memory_Info[FREE_MEM]     = NO_MEMORY_INFO;	// Doesn't work under FreeBSD v2.2.x
    else
	Memory_Info[FREE_MEM]     = MEMORY(free*getpagesize());// total free physical memory (without swaps)

    /* Q&D hack for swap display. Borrowed from xsysinfo-1.4 */
    if ((pipe = popen("/usr/sbin/pstat -ks", "r")) == NULL) {
	used = total = 1;
	return;
    }

    fgets(buf, sizeof(buf), pipe);
    fgets(buf, sizeof(buf), pipe);
    fgets(buf, sizeof(buf), pipe);
    fgets(buf, sizeof(buf), pipe);
    pclose(pipe);

    strtok(buf, " ");
    total_str = strtok(NULL, " ");
    used_str = strtok(NULL, " ");
    used = atoi(used_str);
    total = atoi(total_str);

    _free=total-used;
    Memory_Info[SWAP_MEM]     = MEMORY(1024*total); // total size of all swap-partitions
    Memory_Info[FREESWAP_MEM] = MEMORY(1024*_free); // free memory in swap-partitions
}
