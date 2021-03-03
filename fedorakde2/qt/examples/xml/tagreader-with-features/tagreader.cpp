/*
$Id: qt/examples/xml/tagreader-with-features/tagreader.cpp   2.3.2   edited 2001-06-12 $
*/

#include "structureparser.h"
#include <qapplication.h>
#include <qfile.h>
#include <qxml.h>
#include <qlistview.h>
#include <qgrid.h>
#include <qmainwindow.h>
#include <qlabel.h>

int main( int argc, char **argv )
{
    QApplication app( argc, argv );      

    QFile xmlFile( argc == 2 ? argv[1] : "fnord.xml" );
    QXmlInputSource source( xmlFile );

    QXmlSimpleReader reader;

    QGrid * container = new QGrid( 3 );

    QListView * nameSpace = new QListView( container, "table_namespace" );    
    StructureParser * handlerNamespace = new StructureParser( nameSpace );
    reader.setContentHandler( handlerNamespace );
    reader.parse( source );

    QListView * namespacePrefix = new QListView( container, "table_namespace_prefix" );    
    StructureParser * handlerNamespacePrefix = new StructureParser( namespacePrefix );
    reader.setContentHandler( handlerNamespacePrefix );
    reader.setFeature( "http://xml.org/sax/features/namespaces", TRUE );
    reader.setFeature( "http://xml.org/sax/features/namespace-prefixes", TRUE );
    reader.parse( source );

    QListView * prefix = new QListView( container, "table_prefix");    
    StructureParser * handlerPrefix = new StructureParser( prefix );
    reader.setContentHandler( handlerPrefix );
    reader.setFeature( "http://xml.org/sax/features/namespaces", FALSE );
    reader.setFeature( "http://xml.org/sax/features/namespace-prefixes", TRUE );
    reader.parse( source );

    QLabel * namespaceLabel = new QLabel( 
                             "Default:\n"
                             "http://xml.org/sax/features/namespaces: TRUE\n"
                             "http://xml.org/sax/features/namespace-prefixes: FALSE\n",
                             container );

    QLabel * namespacePrefixLabel = new QLabel( 
                             "\n"
                             "http://xml.org/sax/features/namespaces: TRUE\n"
                             "http://xml.org/sax/features/namespace-prefixes: TRUE\n",
                             container );

    QLabel * prefixLabel = new QLabel( 
                             "\n"
                             "http://xml.org/sax/features/namespaces: FALSE\n"
                             "http://xml.org/sax/features/namespace-prefixes: TRUE\n",
                             container );


    app.setMainWidget( container );
    container->setCaption("Qt Example - XML Tagreader");
    container->show();
    return app.exec();      
}
