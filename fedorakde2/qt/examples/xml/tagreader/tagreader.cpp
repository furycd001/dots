/*
$Id: qt/examples/xml/tagreader/tagreader.cpp   2.3.2   edited 2001-01-26 $
*/

#include "structureparser.h"
#include <qfile.h>
#include <qxml.h>

int main( int argc, char **argv )
{
    for ( int i=1; i < argc; i++ ) {
        StructureParser handler;
        QFile xmlFile( argv[i] );
        QXmlInputSource source( xmlFile );
        QXmlSimpleReader reader;
        reader.setContentHandler( &handler );
        reader.parse( source );
    }
    return 0;
}
