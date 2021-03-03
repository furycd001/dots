/*
$Id: qt/examples/xml/tagreader/structureparser.cpp   2.3.2   edited 2001-01-26 $
*/

#include "structureparser.h"

#include <iostream.h>
#include <qstring.h>
 
bool StructureParser::startDocument()
{
    indent = "";
    return TRUE;
}

bool StructureParser::startElement( const QString&, const QString&, 
                                    const QString& qName, 
                                    const QXmlAttributes& )
{
    cout << indent << qName << endl;
    indent += "    ";
    return TRUE;
}

bool StructureParser::endElement( const QString&, const QString&, const QString& )
{
    indent.remove( 0, 4 );
    return TRUE;
}
