#include "kio_smb.h"

//===========================================================================
int main( int argc, char **argv )
{
    kdDebug(KIO_SMB) << "START: main for kiosmb" << endl;

    KInstance instance( "kio_smb" );
    if( argc != 2 )
    {
        kdDebug(KIO_SMB) << "Usage: kiosmb url" << endl;
        return -1;
    }

    SMBSlave slave( NULL, NULL );

    G_TheSlave = &slave;

    KURL url( argv[1] );
    slave.stat( url );

    return 0;
}
