#include <stdio.h>
#include <stdlib.h>

#include "rc2ui.h"


int main( int argc, char** argv )
{
    const char* error	 = 0;
    const char* fileName = 0;

    for ( int n = 1; n < argc && error == 0; n++ ) {
	if ( fileName )		// can handle only one file
	    error	 = "Too many input files specified";
	else
	    fileName = argv[n];
    }

    if ( argc < 2 || error || !fileName ) {
	if ( error )
	    fprintf( stderr, "rc2ui: %s\n", error );
	fprintf( stderr, "usage: %s filename\n", argv[0]);
	exit( 1 );
    }

    QFile file( fileName );
    if ( !file.open( IO_ReadOnly ) )
	qFatal( "uic: Could not open file '%s' ", fileName );
    QTextStream in;
    in.setDevice( &file );

    RC2UI c( &in );
    return !c.parse();
}
