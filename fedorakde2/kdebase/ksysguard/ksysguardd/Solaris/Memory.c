/*
    KTop, the KDE Task Manager
   
	Copyright (c) 1999, 2000 Chris Schlaeger <cs@kde.org>

	Solaris support by Torsten Kasch <tk@Genetik.Uni-Bielefeld.DE>
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General Public
    License as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

	$Id: Memory.c,v 1.1.2.1 2001/11/08 10:49:12 tokoe Exp $
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/swap.h>

#include "config.h"

#ifdef HAVE_KSTAT
#include <kstat.h>
#endif

#include "ksysguardd.h"
#include "Command.h"
#include "Memory.h"

static int Dirty = 1;
static t_memsize totalmem = (t_memsize) 0;
static t_memsize freemem = (t_memsize) 0;
static long totalswap = 0L;
static long freeswap = 0L;

/*
 *  this is borrowed from top's m_sunos5 module
 *  used by permission from William LeFebvre
 */
static int pageshift;
static long (*p_pagetok) ();
#define pagetok(size) ((*p_pagetok)(size))

long pagetok_none( long size ) {
	return( size );
}

long pagetok_left( long size ) {
	return( size << pageshift );
}

long pagetok_right( long size ) {
	return( size >> pageshift );
}

void initMemory( void ) {

	long i = sysconf( _SC_PAGESIZE );

	pageshift = 0;
	while( (i >>= 1) > 0 )
		pageshift++;

	/* calculate an amount to shift to K values */
	/* remember that log base 2 of 1024 is 10 (i.e.: 2^10 = 1024) */
	pageshift -= 10;

	/* now determine which pageshift function is appropriate for the 
	result (have to because x << y is undefined for y < 0) */
	if( pageshift > 0 ) {
		/* this is the most likely */
		p_pagetok = pagetok_left;
	} else if( pageshift == 0 ) {
		p_pagetok = pagetok_none;
	} else {
		p_pagetok = pagetok_right;
		pageshift = -pageshift;
	}

#ifdef HAVE_KSTAT
	registerMonitor( "mem/physical/free", "integer",
					printMemFree, printMemFreeInfo );
	registerMonitor( "mem/physical/used", "integer",
					printMemUsed, printMemUsedInfo );
#endif
	registerMonitor( "mem/swap/free", "integer",
					printSwapFree, printSwapFreeInfo );
	registerMonitor( "mem/swap/used", "integer",
					printSwapUsed, printSwapUsedInfo );
}

void exitMemory( void ) {
}

int updateMemory( void ) {

	struct swaptable	*swt;
	struct swapent		*ste;
	int			i;
	int			ndevs;
	long			swaptotal;
	long			swapfree;
	char			dummy[128];
#ifdef HAVE_KSTAT
	kstat_ctl_t		*kctl;
	kstat_t			*ksp;
	kstat_named_t		*kdata;
#endif /* HAVE_KSTAT */

	if( (ndevs = swapctl( SC_GETNSWP, NULL )) < 1 )
		return( 0 );
	if( (swt = (struct swaptable *) malloc(
			sizeof( int )
			+ ndevs * sizeof( struct swapent ))) == NULL )
		return( 0 );

	/*
	 *  fill in the required fields and retrieve the info thru swapctl()
	 */
	swt->swt_n = ndevs;
	ste = &(swt->swt_ent[0]);
	for( i = 0; i < ndevs; i++ ) {
		/*
		 *  since we'renot interested in the path(s),
		 *  we'll re-use the same buffer
		 */
		ste->ste_path = dummy;
		ste++;
	}
	swapctl( SC_LIST, swt );

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

	totalswap = pagetok( swaptotal );
	freeswap = pagetok( swapfree );

#ifdef HAVE_KSTAT
	/*
	 *  get a kstat handle and update the user's kstat chain
	 */
	if( (kctl = kstat_open()) == NULL )
		return( 0 );
	while( kstat_chain_update( kctl ) != 0 )
		;

	totalmem = pagetok( sysconf( _SC_PHYS_PAGES ));

	/*
	 *  traverse the kstat chain to find the appropriate statistics
	 */
	if( (ksp = kstat_lookup( kctl, "unix", 0, "system_pages" )) == NULL )
		return( 0 );
	if( kstat_read( kctl, ksp, NULL ) == -1 )
		return( 0 );

	/*
	 *  lookup the data
	 */
	 kdata = (kstat_named_t *) kstat_data_lookup( ksp, "freemem" );
	 if( kdata != NULL )
	 	freemem = pagetok( kdata->value.ui32 );

	kstat_close( kctl );
#endif /* ! HAVE_KSTAT */

	Dirty = 0;

	return( 0 );
}

void printMemFreeInfo( const char *cmd ) {
	if( Dirty )
		updateMemory();
	fprintf(CurrentClient, "Free Memory\t0\t%ld\tKB\n", totalmem );
}

void printMemFree( const char *cmd ) {
	if( Dirty )
		updateMemory();
	fprintf(CurrentClient, "%ld\n", freemem );
}

void printMemUsedInfo( const char *cmd ) {
	if( Dirty )
		updateMemory();
	fprintf(CurrentClient, "Used Memory\t0\t%ld\tKB\n", totalmem );
}

void printMemUsed( const char *cmd ) {
	if( Dirty )
		updateMemory();
	fprintf(CurrentClient, "%ld\n", totalmem - freemem );
}

void printSwapFreeInfo( const char *cmd ) {
	if( Dirty )
		updateMemory();
	fprintf(CurrentClient, "Free Swap\t0\t%ld\tKB\n", totalswap );
}

void printSwapFree( const char *cmd ) {
	if( Dirty )
		updateMemory();
	fprintf(CurrentClient, "%ld\n", freeswap );
}

void printSwapUsedInfo( const char *cmd ) {
	if( Dirty )
		updateMemory();
	fprintf(CurrentClient, "Used Swap\t0\t%ld\tKB\n", totalswap );
}

void printSwapUsed( const char *cmd ) {
	if( Dirty )
		updateMemory();
	fprintf(CurrentClient, "%ld\n", totalswap - freeswap );
}
