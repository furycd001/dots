/* $Id: memory_netbsd.cpp,v 1.3.2.1 2001/08/21 13:32:34 mueller Exp $ */

#include <sys/param.h>
#if __NetBSD_Version__ > 103080000
#define UVM
#endif
#if defined(__OpenBSD__)
#define UVM
#endif

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#ifdef UVM
#include <uvm/uvm_extern.h>
#else
#include <vm/vm_swap.h>
#endif

void KMemoryWidget::update()
{
  int mib[2], memory;
  size_t len;
#ifdef UVM
  struct  uvmexp uvmexp;
#else
  struct swapent *swaplist;
  int nswap, rnswap, totalswap, freeswap, usedswap;
#endif
  
  /* memory */
  mib[0] = CTL_HW;
  mib[1] = HW_PHYSMEM;
  len = sizeof(memory);
  if( sysctl(mib,2,&memory,&len,NULL,0)< 0 )
    Memory_Info[TOTAL_MEM]    = NO_MEMORY_INFO;
  else
    Memory_Info[TOTAL_MEM]    = memory;

#warning "FIXME: Memory_Info[CACHED_MEM]"
    Memory_Info[CACHED_MEM] = NO_MEMORY_INFO; // cached memory in ram
    
#ifdef UVM
  mib[0] = CTL_VM;
  mib[1] = VM_UVMEXP;
  len = sizeof(uvmexp);
  if ( sysctl(mib, 2, &uvmexp, &len, NULL, 0) < 0 ) {
    Memory_Info[FREE_MEM]     = NO_MEMORY_INFO;
    Memory_Info[ACTIVE_MEM]   = NO_MEMORY_INFO;
    Memory_Info[INACTIVE_MEM] = NO_MEMORY_INFO;
    Memory_Info[SWAP_MEM]     = NO_MEMORY_INFO;
    Memory_Info[FREESWAP_MEM] = NO_MEMORY_INFO;
  } else {
    Memory_Info[FREE_MEM] = MEMORY(uvmexp.free * uvmexp.pagesize);
    Memory_Info[ACTIVE_MEM] = MEMORY(uvmexp.active * uvmexp.pagesize);
    Memory_Info[INACTIVE_MEM] = MEMORY(uvmexp.inactive * uvmexp.pagesize);
    Memory_Info[SWAP_MEM] = MEMORY(uvmexp.swpages * uvmexp.pagesize);
    Memory_Info[FREESWAP_MEM] = MEMORY((uvmexp.swpages - uvmexp.swpginuse) *
							uvmexp.pagesize);
    }
#else
  Memory_Info[FREE_MEM] = NO_MEMORY_INFO;
  Memory_Info[ACTIVE_MEM] = NO_MEMORY_INFO;
  Memory_Info[INACTIVE_MEM] = NO_MEMORY_INFO;

  /* swap */
  totalswap = freeswap = usedswap = 0;
  nswap = swapctl(SWAP_NSWAP,0,0);
  if ( nswap > 0 ) {
    if ( (swaplist = (struct swapent *)malloc(nswap * sizeof(*swaplist))) ) {
      rnswap = swapctl(SWAP_STATS,swaplist,nswap);
      if ( rnswap < 0 || rnswap > nswap )
	totalswap = freeswap = -1;	/* Error */
      else {
	while ( rnswap-- > 0 ) {
	  totalswap += swaplist[rnswap].se_nblks;
	  usedswap += swaplist[rnswap].se_inuse;
	}
	freeswap = totalswap - usedswap;
      }
    } else
      totalswap = freeswap = -1;	/* Error */

    if ( totalswap == -1 ) {
	Memory_Info[SWAP_MEM]     = NO_MEMORY_INFO;
	Memory_Info[FREESWAP_MEM] = NO_MEMORY_INFO;
    } else {				
	Memory_Info[SWAP_MEM]     = MEMORY(totalswap);
	Memory_Info[FREESWAP_MEM] = MEMORY(freeswap);
    }
  }
#endif
}
