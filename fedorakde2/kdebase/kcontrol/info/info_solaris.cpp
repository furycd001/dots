/*
 *  info_solaris.cpp
 *
 *  $Id: info_solaris.cpp,v 1.1 2001/03/07 05:30:20 waba Exp $
 *
 *  Torsten Kasch <tk@Genetik.Uni-Bielefeld.DE>
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mnttab.h>
#include <kstat.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <time.h>

#ifdef HAVE_LIBDEVINFO_H
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/mkdev.h>
#include <sys/stat.h>
#include <devid.h>
#include <libdevinfo.h>
#endif /* HAVE_LIBDEVINFO_H */

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


bool GetInfo_CPU( QListView *lBox ) {

	kstat_ctl_t	*kctl;
	kstat_t		*ksp;
	kstat_named_t	*kdata;
	char		cputype[16],
			fputype[16];
	char		*timetxt;
	char		*ptr;
	uint32_t	i, ncpus;
	unsigned long	state_begin;
	QString		state;
	QString		mhz;
	QString		inst;

	/*
	 *  get a kstat handle first and update the user's kstat chain
	 */
	if( (kctl = kstat_open()) == NULL ) {
		return false;
	}
	while( kstat_chain_update( kctl ) != 0 )
		;

	/*
	 *  get the # of CPUs
	 */
	if( (ksp = kstat_lookup( kctl, "unix", 0, "system_misc" )) == NULL ) {
		return false;
	}
	if( kstat_read( kctl, ksp, NULL ) == -1 ) {
		return false;
	}
	kdata = (kstat_named_t *) kstat_data_lookup( ksp, "ncpus" );
	if( kdata != NULL ) {
		ncpus = kdata->value.ui32;
	} else {
		ncpus = 0;
	}

	lBox->addColumn( i18n( "Instance" ));
	lBox->addColumn( i18n( "CPU Type" ));
	lBox->addColumn( i18n( "FPU Type" ));
	lBox->addColumn( i18n( "MHz" ));
	lBox->addColumn( i18n( "State" ));

	/*
	 *  get the per-processor info
	 */
	for( i = 0; i < ncpus; i++ ) {

		if( (ksp = kstat_lookup( kctl, "cpu_info", i, NULL )) == NULL ){
			return false;
		}

		if( kstat_read( kctl, ksp, NULL ) == -1 ) {
			return false;
		}

		inst.setNum( i );
		kdata = (kstat_named_t *) kstat_data_lookup( ksp, "cpu_type" );
		if( kdata != NULL ) {
			strcpy( cputype, kdata->value.c );
		} else {
			sprintf( cputype, "???" );
		}
		kdata = (kstat_named_t *) kstat_data_lookup( ksp, "fpu_type" );
		if( kdata != NULL ) {
			strcpy( fputype, kdata->value.c );
		} else {
			sprintf( fputype, "???" );
		}
		kdata = (kstat_named_t *) kstat_data_lookup( ksp, "clock_MHz" );
		if( kdata != NULL ) {
			mhz.setNum( kdata->value.ul );
		} else {
			mhz.setNum( 0 );
		}
		kdata = (kstat_named_t *) kstat_data_lookup( ksp, "state" );
		if( kdata != NULL ) {
			state = QString( kdata->value.c );
		} else {
			state = "???";
		}
		kdata = (kstat_named_t *) kstat_data_lookup( ksp, "state_begin" );
		if( kdata != NULL ) {
			state_begin = kdata->value.i32;
			if( (timetxt = ctime( (time_t *) &state_begin )) != NULL ) {
				ptr = strrchr( timetxt, '\n' );
				*ptr = '\0';
				state += " since " + QString( timetxt );
			}
		}

		new QListViewItem( lBox, inst, cputype, fputype, mhz, state );
	}

	// sorting_allowed = true;
	lBox->setSorting( 0 );

	return true;
}

bool GetInfo_IRQ( QListView * ) {
	return false;
}

bool GetInfo_DMA( QListView * ) {
	return false;
}

bool GetInfo_PCI( QListView * ) {
	return false;
}

bool GetInfo_IO_Ports( QListView * ) {
	return false;
}

bool GetInfo_Sound( QListView * ) {
	return false;
}

bool GetInfo_SCSI( QListView * ) {
	return false;
}

bool GetInfo_Partitions( QListView *lBox ) {

	FILE		*mnttab;
	struct mnttab	mnt;
	struct statvfs	statbuf;
	fsblkcnt_t	tmp;
	QString		total;
	QString		avail;
	time_t		mnttime;
	char		*timetxt;
	char		*ptr;

	if( (mnttab = fopen( MNTTAB, "r" )) == NULL ) {
		return false;
	}

	/*
	 *  set up column headers
	 */
	lBox->addColumn( i18n( "Device" ));
	lBox->addColumn( i18n( "Mount Point" ));
	lBox->addColumn( i18n( "FS Type" ));
	lBox->addColumn( i18n( "Total Size" ));
	// XXX: FIXME: how do I set column alignment correctly?
	lBox->setColumnAlignment( 3, 2 );
	lBox->addColumn( i18n( "Free Size" ));
	// XXX: FIXME: how do I set column alignment correctly?
	lBox->setColumnAlignment( 4, 2 );
	lBox->addColumn( i18n( "Mount Time" ));
	lBox->addColumn( i18n( "Mount Options" ));

	/*
	 *  get info about mounted file systems
	 */
	rewind( mnttab );
	while( getmntent( mnttab, &mnt ) == 0 ) {
		/*
		 *  skip fstype "nfs" and "autofs" for two reasons:
		 *	o if the mountpoint is visible, the fs is not
		 *	  necessarily available (autofs option "-nobrowse")
		 *	  and we don't want to mount every remote fs just
		 *	  to get its size, do we?
		 *	o the name "Partitions" for this statistics implies
		 *	  "local file systems only"
		 */
		if( (strcmp( mnt.mnt_fstype, "nfs" ) == 0)
				|| (strcmp( mnt.mnt_fstype, "autofs" ) == 0) )
			continue;
		if( statvfs( mnt.mnt_mountp, &statbuf ) == 0 ) {
			if( statbuf.f_blocks > 0 ) {
				/*
				 *  produce output in KB, MB, or GB for
				 *  readability -- unfortunately, this
				 *  breaks sorting for these columns...
				 */
				tmp = statbuf.f_blocks
					* (statbuf.f_frsize / 1024);
				if( tmp > 9999 ) {
					tmp /= 1024;
					if( tmp > 9999 ) {
						tmp /= 1024;
						total.setNum( tmp );
						total += " G";
					} else {
						total.setNum( tmp );
						total += " M";
					}
				} else {
					total.setNum( tmp );
					total += " K";
				}
//				avail.setNum( statbuf.f_bavail );
//				avail += " K";
				tmp = statbuf.f_bavail
					* (statbuf.f_frsize / 1024);
				if( tmp > 9999 ) {
					tmp /= 1024;
					if( tmp > 9999 ) {
						tmp /= 1024;
						avail.setNum( tmp );
						avail += " G";
					} else {
						avail.setNum( tmp );
						avail += " M";
					}
				} else {
					avail.setNum( tmp );
					avail += " K";
				}
			} else {
				total = "-";
				avail = "-";
			}
		} else {
			total = "???";
			avail = "???";
		}
		/*
		 *  ctime() adds a '\n' which we have to remove
		 *  so that we get a one-line output for the QListViewItem
		 */
		mnttime = (time_t) atol( mnt.mnt_time );
		if( (timetxt = ctime( &mnttime )) != NULL ) {
			ptr = strrchr( timetxt, '\n' );
			*ptr = '\0';
		}
		
		new QListViewItem(
			lBox,
			mnt.mnt_special,
			mnt.mnt_mountp,
			mnt.mnt_fstype,
			total,
			avail,
			QString( timetxt ),
			mnt.mnt_mntopts
		);
	}
	fclose( mnttab );
	
	lBox->setSorting( 0 );
	// sorting_allowed = true;

	return true;
}

bool GetInfo_XServer_and_Video( QListView *lBox ) {
	return GetInfo_XServer_Generic( lBox );
}

#ifdef HAVE_LIBDEVINFO_H
/*
 *  get Solaris' device configuration data through libdevinfo(3)
 *  and display it in a prtconf(1M) style tree
 *
 *  NOTE: though the devinfo library seems to be present on earlier
 *        Solaris releases, this interface is documented to be available
 *        since Solaris 7 (libdevinfo.h is missing on pre-Solaris 7 systems)
 *
 *  documentation for libdevinfo(3) including code samples on which
 *  this implementation is based on is available at
 *	http://soldc.sun.com/developer/support/driver/wps/libdevinfo/
 */

/*
 *  we start with various helper routines for GetInfo_Devices()
 */

/*
 *  mktree() -- break up the device path and place its components
 *		into the tree widget
 */
QListViewItem *mktree( QListViewItem *top, const char *path ) {

	QListViewItem	*parent,
			*previous,
			*result;
	char		*str = strdup( path ),
			*token;

	/*
	 *  start at "/"
	 */
	parent = top;
	result = (*top).firstChild();
	previous = (*top).firstChild();

	token = strtok( str, "/" );
	while( token != NULL ) {
		/*
		 *  find insert pos:
		 *  try to match the node at the current level
		 *
		 *  NOTE: this implementation assumes that there are
		 *        no two nodes with identical names at the
		 *        same level of the device tree
		 */
		while( result != NULL ) {
			if( strcmp( token, (*result).text( 0 ).latin1()) == 0 )
				break;
			previous = result;
			result = (*result).nextSibling();
		}
		if( result == NULL ) {
			/*
			 *  we haven't found the node, create a new one
			 */
			result = new QListViewItem( parent,
					previous,
					token );
		} else {
			/*
			 *  we've found the node
			 */
			parent = result;
			previous = NULL;
			if( (*result).firstChild() == NULL ) {
				/*
				 *  create new node during next iteration
				 */
				result->setExpandable( true );
				result->setOpen( false );
			} else {
				/*
				 *  follow the child path
				 */
				result = (*result).firstChild();
			}
		}
		token = strtok( NULL, "/" );
	}
	free( str );

	return( result );
}

/*
 *  prop_type_str()  -- return the property type as a string
 */
char *prop_type_str( di_prop_t prop ) {

	switch( di_prop_type( prop )) {
		case DI_PROP_TYPE_UNDEF_IT:
			return( "undefined" );
		case DI_PROP_TYPE_BOOLEAN:
			return( "BOOL" );
		case DI_PROP_TYPE_INT:
			return( "INT" );
		case DI_PROP_TYPE_STRING:
			return( "STRING" );
		case DI_PROP_TYPE_BYTE:
			return( "BYTE" );
		default:
			return( "unknown" );
	}
}

/*
 *  prop_type_guess() -- guess the property type
 */
int prop_type_guess( uchar_t *data, int len ) {

	int	slen;
	int	guess;
	int	i, c;

	if( len < 0 )
		return( -1 );
	else if( len == 0 )
		return( DI_PROP_TYPE_BOOLEAN );

	slen = 0;
	guess = DI_PROP_TYPE_STRING;

	for( i = 0; i < len; i++ ) {
		c = (int) data[i];
		switch( c ) {
			case 0:
				if( i == (len - 1 ))
					break;
				if( slen == 0 )
					guess = DI_PROP_TYPE_BYTE;
				else
					guess = slen = 0;
				break;
			default:
				if( ! isprint( c ))
					guess = DI_PROP_TYPE_BYTE;
				else
					slen++;
		}
		if( guess != DI_PROP_TYPE_STRING )
			break;
	}

//	if( (guess == DI_PROP_TYPE_BYTE) && (len % sizeof( int ) == 0 ))
//		guess = DI_PROP_TYPE_INT;

	return( guess );
}

/*
 *  dump_minor_node()  --  examine a device minor node
 *			   this routine gets passed to di_walk_node()
 */
int dump_minor_node( di_node_t node, di_minor_t minor, void *arg ) {

	QListViewItem	*item;
	QString		majmin;
	char		*type;
	dev_t		dev;

	item = new QListViewItem( (QListViewItem *) arg,
			di_minor_name( minor ));
	item->setExpandable( true );
	item->setOpen( false );
	new QListViewItem( item, i18n( "Spectype:" ),
		(di_minor_spectype( minor ) == S_IFCHR)
			? i18n( "character special" )
			: i18n( "block special" ));
	type = di_minor_nodetype( minor );
	new QListViewItem( item, i18n( "Nodetype:" ),
		(type == NULL) ? "NULL" : type );

	if( (dev = di_minor_devt( minor )) != DDI_DEV_T_NONE ) {
		majmin.sprintf( "%ld/%ld", major( dev ), minor( dev ));
		new QListViewItem( item, i18n( "Major/Minor:" ), majmin );
	}

	if( di_minor_next( node, minor ) == DI_MINOR_NIL )
		return( DI_WALK_TERMINATE );
	else
		return( DI_WALK_CONTINUE );
}

/*
 *  propvalue() -- return the property value
 */
QString propvalue( di_prop_t prop ) {

	int	type;
	int	i, n;
	char	*strp;
	int	*intp;
	uchar_t	*bytep;
	QString	result;

	/*
	 *  Since a lot of printable strings seem to be tagged as 'byte',
	 *  we're going to guess, if the property is not STRING or INT
	 *  The actual type is shown in the info tree, though.
	 */
	type = di_prop_type( prop );
	if( (type != DI_PROP_TYPE_STRING) && (type != DI_PROP_TYPE_INT) ) {
		n = di_prop_bytes( prop, &bytep );
		type = prop_type_guess( bytep, n );
	}

	result = "";
	switch( type ) {
		case DI_PROP_TYPE_STRING:
			if( (n = di_prop_strings( prop, &strp )) < 0 ) {
				result = "(error)";
			} else {
				for( i = 0; i < n; i++ ) {
					result += "\"";
					result += strp;
					result += "\" ";
					strp += strlen( strp ) + 1;
				}
			}
			break;
		case DI_PROP_TYPE_INT:
			if( (n = di_prop_ints( prop, &intp )) < 0 ) {
				result = "(error)";
			} else {
				for( i = 0; i < n; i++ ) {
					QString tmp;
					tmp.setNum( intp[i] );
					result += tmp;
					result += " ";
				}
			}
			break;
		case DI_PROP_TYPE_BOOLEAN:
			/*
			 *  hmm, Sun's sample code handles the existance
			 *  of a boolean property as "true", whereas
			 *  prtconf(1M) obviously does not (Sol8, at least)
			 *  -- we're doing the same and handle "bool" as "byte"
			 */
		case DI_PROP_TYPE_BYTE:
			if( (n = di_prop_bytes( prop, &bytep )) < 0 ) {
				result = "(error)";
			} else {
				if( n == 0 ) {
					result = i18n( "(no value)" );
					break;
				}
				result = "0x";
				for( i = 0; i < n; i++ ) {
					QString tmp;
					unsigned byte = (unsigned) bytep[i];
					tmp.sprintf( "%2.2x", byte );
					result += tmp;
				}
			}
			break;
		default:
			result = "???";
	}

	return( result );
}

/*
 *  dump_node() -- examine a device node and its children
 *		   this routine gets passed to di_walk_node()
 */
int dump_node( di_node_t node, void *arg ) {

	QListViewItem	*top = (QListViewItem *) arg,
			*parent,
			*previous;
	char		*path;
	char		*drivername;
	char		*names;
	QString		compatnames;
	int		i, n;
	di_prop_t	prop;

	path = di_devfs_path( node );

	/*
	 *  if this is the root node ("/"), initialize the tree
	 */
	if( strlen( path ) == 1 ) {
		top->setText( 0, QString( di_binding_name( node )));
		top->setPixmap( 0, SmallIcon( "kcmdevices" ));
		top->setOpen( true );
		top->setSelectable( false );
		top->setExpandable( false );
	}

	/*
	 *  place the node in the tree
	 */
	parent = mktree( top, path );

	/*
	 *  we have to handle the root node differently...
	 */
	if( strlen( path ) > 1 ) {
		parent->setExpandable( true );
		parent->setOpen( false );
	} else {
		previous = parent;
		parent = top;
	}

	/*
	 *  node name and physical device path
	 */
	drivername = di_driver_name( node );
	previous = new QListViewItem( parent,
		i18n( "Driver Name:" ),
		(drivername == NULL)
			? i18n( "(driver not attached)" )
			: drivername );
	previous = new QListViewItem( parent, previous,
		i18n( "Binding Name:" ), di_binding_name( node ));

	n = di_compatible_names( node, &names );
	if( n < 1 ) {
		compatnames = i18n( "(none)" );
	} else {
		for( i = 0; i < n; i++ ) {
			compatnames += names;
			compatnames += " ";
			names += strlen( names ) + 1;
		}
	}

	previous = new QListViewItem( parent, previous,
		i18n( "Compatible Names:" ), compatnames );

	previous = new QListViewItem( parent, previous,
		i18n( "Physical Path:" ), QString( path ));

	/*
	 *  dump the node's property list (if any)
	 */
	if( (prop = di_prop_next( node, DI_PROP_NIL )) != DI_PROP_NIL ) {
		previous = new QListViewItem( parent, previous, i18n( "Properties" ));
		previous->setExpandable( true );
		previous->setOpen( false );
		do {
			/*
			 *  property type & value
			 */
			QListViewItem	*tmp,
					*prev;
			tmp = new QListViewItem( previous, di_prop_name( prop ));
			tmp->setExpandable( true );
			tmp->setOpen( false );
			prev = new QListViewItem( tmp, i18n( "Type:" ),
				prop_type_str( prop ));
			new QListViewItem( tmp, prev, i18n( "Value:" ),
				propvalue( prop ));
		} while( (prop = di_prop_next( node, prop )) != DI_PROP_NIL );
	}

	/*
	 *  if there are minor nodes, expand the tree appropriately
	 */
	if( di_minor_next( node, DI_MINOR_NIL ) != DI_MINOR_NIL ) {
		previous = new QListViewItem( parent, previous, i18n( "Minor Nodes" ));
		previous->setExpandable( true );
		previous->setOpen( false );
		di_walk_minor( node, NULL, 0, previous, dump_minor_node );
	}

	return( DI_WALK_CONTINUE );
}

bool GetInfo_Devices( QListView *lBox ) {

	QListViewItem		*top;
	di_node_t		root_node;

	/*
	 *  create a snapshot of the device tree
	 */
	if( (root_node = di_init( "/", DINFOCPYALL )) == DI_NODE_NIL ) {
		return( false );
	}
	// XXX: might try to di_prom_init() here as well (if we're setgid sys)

	/*
	 *  prepare the tree widget
	 */
	lBox->addColumn( i18n( "Device Information" ));
	lBox->addColumn( i18n( "Value" ));

	top = new QListViewItem( lBox );

	/*
	 *  traverse the device tree
	 */
	di_walk_node( root_node, DI_WALK_CLDFIRST, top, dump_node );

	di_fini( root_node );

	sorting_allowed = false;
	return true;
}

#else /* ! HAVE_LIBDEVINFO_H */
bool GetInfo_Devices( QListView * ) {
	return false;
}
#endif /* ! HAVE_LIBDEVINFO_H */
