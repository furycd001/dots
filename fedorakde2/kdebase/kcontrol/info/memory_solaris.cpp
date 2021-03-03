/*
 *  memory_solaris.cpp
 *
 *  $Id: memory_solaris.cpp,v 1.2 2001/05/27 16:19:53 deller Exp $
 *
 *  Torsten Kasch <tk@Genetik.Uni-Bielefeld.DE>
 */

#include <unistd.h>
#include <stdlib.h>
#include <kstat.h>
#include <sys/stat.h>
#include <sys/swap.h>

#define PAGETOK(a) (( (t_memsize) sysconf( _SC_PAGESIZE )) *  (t_memsize) a)

void KMemoryWidget::update() {

	kstat_ctl_t	*kctl;
	kstat_t		*ksp;
	kstat_named_t	*kdata;

	/*
	 *  get a kstat handle first and update the user's kstat chain
	 */
	if( (kctl = kstat_open()) == NULL )
		return;
	while( kstat_chain_update( kctl ) != 0 )
		;

	/*
	 *  traverse the kstat chain to find the appropriate kstat
	 */
	if( (ksp = kstat_lookup( kctl, "unix", 0, "system_pages" )) == NULL )
		return;

	if( kstat_read( kctl, ksp, NULL ) == -1 )
		return;

	/*
	 *  lookup the data
	 */
#if 0
	kdata = (kstat_named_t *) kstat_data_lookup( ksp, "physmem" );
	if( kdata != NULL ) {
		Memory_Info[TOTAL_MEM] = PAGETOK(kdata->value.ui32);
	}
#endif
	Memory_Info[TOTAL_MEM] = PAGETOK(sysconf(_SC_PHYS_PAGES));

	kdata = (kstat_named_t *) kstat_data_lookup( ksp, "freemem" );
	if( kdata != NULL )
		Memory_Info[FREE_MEM] = PAGETOK(kdata->value.ui32);

#warning "FIXME: Memory_Info[CACHED_MEM]"
	Memory_Info[CACHED_MEM] = NO_MEMORY_INFO; // cached memory in ram
	  
	kstat_close( kctl );

	/*
	 *  Swap Info
	 */
	struct swaptable	*swt;
	struct swapent		*ste;
	int			i;
	int			ndevs;
	long			swaptotal;
	long			swapfree;
	char			dummy[128];

	/*
	 *  allocate memory to hold info for all swap devices
	 */
	if( (ndevs = swapctl( SC_GETNSWP, NULL )) < 1 )
		return;
	if( (swt = (struct swaptable *) malloc(
			sizeof( int )
			+ ndevs * sizeof( struct swapent ))) == NULL ) {
		return;
	}

	/*
	 *  fill in the required fields and retrieve the info thru swapctl()
	 */
	swt->swt_n = ndevs;
	ste = &(swt->swt_ent[0]);
	for( i = 0; i < ndevs; i++ ) {
		/*
		 *  since we're not interested in the path(s),
		 *  we'll re-use the same buffer
		 */
		ste->ste_path = dummy;
		ste++;
	}
	swapctl( SC_LIST, swt );

	/*
	 *  sum up the total/free pages
	 */
	swaptotal = swapfree = 0L;
	ste = &(swt->swt_ent[0]);
	for( i = 0; i < ndevs; i++ ) {
		if( (! (ste->ste_flags & ST_INDEL))
				&& (! (ste->ste_flags & ST_DOINGDEL)) ) {
			swaptotal += ste->ste_pages;
			swapfree += ste->ste_free;
		}
		ste++;
	}
	free( swt );


	Memory_Info[SWAP_MEM]     = PAGETOK(swaptotal);
	Memory_Info[FREESWAP_MEM] = PAGETOK(swapfree);
}
