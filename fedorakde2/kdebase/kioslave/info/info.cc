#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>

#include <kdebug.h>
#include <kprocess.h>
#include <kstddirs.h>
#include <kinstance.h>

#include "info.h"

using namespace KIO;

InfoProtocol::InfoProtocol( const QCString &pool, const QCString &app )
    : SlaveBase( "info", pool, app )
    , m_page( "" )
    , m_node( "" )
{
    kdDebug( 7108 ) << "InfoProtocol::InfoProtocol" << endl;

    m_infoScript = locate( "data", "kio_info/kde-info2html" );

    m_perl = KGlobal::dirs()->findExe( "perl" );
    
    if( m_infoScript.isEmpty() )
	kDebugFatal( 7108, "Critical error: Cannot locate 'kde-info2html' for HTML-conversion" );
    
    kdDebug( 7108 ) << "InfoProtocol::InfoProtocol - done" << endl;
}

InfoProtocol::~InfoProtocol()
{
    kdDebug( 7108 ) << "InfoProtocol::~InfoProtocol" << endl;

    kdDebug( 7108 ) << "InfoProtocol::~InfoProtocol - done" << endl;
}

void InfoProtocol::get( const KURL& url )
{
    kdDebug( 7108 ) << "InfoProtocol::get" << endl;
    kdDebug( 7108 ) << "URL: " << url.prettyURL() << " , Path :" << url.path() << endl;

    mimeType("text/html");
    // extract the path and node from url
    decodeURL( url );
    /*
    if( m_page.isEmpty() )
    {
	//error( 1, "Syntax error in URL" );
	
	QByteArray array = errorMessage();
	
	data( array );
	finished();

	return;
    }
    */
    if ( m_page.isEmpty() )
      m_page = "dir";

    QString cmds("%1 %2 %3 %4 \"%5\" \"%6\"");
    QCString cmd = cmds.arg(m_perl).arg(m_infoScript).arg(locate("data", "kio_info/kde-info2html.conf")).arg(KGlobal::dirs()->findResourceDir("icon", "hicolor/22x22/actions/up.png")).arg(m_page).arg(m_node).latin1();
    //kdDebug( 7108 ) << "cmd: " << (const char *)cmd << endl;
    
    FILE *fd = popen( cmd.data(), "r" );
    
    char buffer[ 4090 ];
    QByteArray array;
    
    while ( !feof( fd ) )
    {
      int n = fread( buffer, 1, 2048, fd );
      if ( n == -1 )
      {
        // ERROR
        pclose( fd );
	return;
      }
      array.setRawData( buffer, n );
      data( array );
      array.resetRawData( buffer, n );
    }
    
    pclose( fd );

    finished();
    
    kdDebug( 7108 ) << "InfoProtocol::get - done" << endl;
}

void InfoProtocol::mimetype( const KURL& /* url */ )
{
    kdDebug( 7108 ) << "InfoProtocol::mimetype" << endl;

    // to get rid of those "Open with" dialogs...
    mimeType( "text/html" );

    // finish action
    finished();

    kdDebug( 7108 ) << "InfoProtocol::mimetype - done" << endl;
}

void InfoProtocol::decodeURL( const KURL &url )
{
    kdDebug( 7108 ) << "InfoProtocol::decodeURL" << endl;

    /* test for valid directory in url.path().
     * else test for valid dir in url.host()+url.path() because
     *  of "info://usr/local/info/infopage/Top" made from the
     *  "info2html" program.
     * If there is no possibility to get a valid dir, assume the url to 
     *  be a valid page description.
     */


    QString dirstr;
    if (url.hasHost()) {
      dirstr = '/';
      dirstr += url.host();
    }
    dirstr += url.path(); // there HAS to be a url.path() at least
    //kdDebug( 7108 ) << "dirstring: " << dirstr << endl;

    /* now we got a description where a directory is in.
     * Lets test it */

    int slashPos = dirstr.find( "/", 1 );
    int oldPos = 1;
    QDir dir(dirstr.left(slashPos));
    //kdDebug( 7108 ) << "dirpath: " << dir.path() << endl;
    while (dir.exists()) {
      oldPos = slashPos;
      slashPos = dirstr.find( "/", oldPos+1 );
      if (-1 == slashPos) {
	// no more '/' found, 
	// the whole string is a valid path ?
	break;
      }
      dir.setPath(dirstr.left(slashPos));
      //kdDebug( 7108 ) << "dirpath-loop: " << dir.path() << endl;
    }

    // oldPos now has the last dir '/'
    //kdDebug( 7108 ) << "dirstr_ length = " <<dirstr.length() << ", pos = " << oldPos << endl;
    //kdDebug( 7108 ) << "info_ request: " << dirstr.right(dirstr.length() - oldPos) << endl;
    decodePath(dirstr.right(dirstr.length() - oldPos));

    kdDebug( 7108 ) << "InfoProtocol::decodeURL - done" << endl;
}

void InfoProtocol::decodePath( QString path )
{
    kdDebug( 7108 ) << "InfoProtocol::decodePath" << endl;

    m_page = "";
    m_node = "";

    // remove leading slash
    if ('/' == path[0]) {
      path = path.right( path.length() - 1 );
    }
    //kdDebug( 7108 ) << "Path: " << path << endl;

    int slashPos = path.find( "/" );

    if( slashPos < 0 )
    {
	m_page = path;
	m_node = "Top";
	return;
    }

    m_page = path.left( slashPos );

    // remove leading+trailing whitespace
    m_node = path.right( path.length() - slashPos - 1).stripWhiteSpace ();

    kdDebug( 7108 ) << "InfoProtocol::decodePath - done" << endl;
}

QCString InfoProtocol::errorMessage()
{
    kdDebug( 7108 ) << "InfoProtocol::errorMessage" << endl;

    // i18n !!!!!!!!!!!!!!!!!!
    return QCString( "<html><body bgcolor=\"#FFFFFF\">An error occured during converting an info-page to HTML</body></html>" );
}

// A minimalistic stat with only the file type
// This seems to be enough for konqueror
void InfoProtocol::stat( const KURL &url )
{
	UDSEntry uds_entry;
	UDSAtom  uds_atom;

	// Regular file with rwx permission for all
	uds_atom.m_uds = KIO::UDS_FILE_TYPE;
	uds_atom.m_long = S_IFREG | S_IRWXU | S_IRWXG | S_IRWXO;

	uds_entry.append( uds_atom );

	statEntry( uds_entry );

	finished();
}

void InfoProtocol::listDir( const KURL &url )
{
    kdDebug( 7108 ) << "InfoProtocol::listDir" << endl;
 
    if ( !url.directory(true,true).isEmpty()
         && url.directory(true,true) != QString("/") )
    {
        error( KIO::ERR_CANNOT_ENTER_DIRECTORY, url.path() );
        return;
    }
 
    // Match info nodes in the 'dir' file
    // "* infopage:" at the start of a line
    QRegExp regex( "^\\*  *[^: ][^:]*:", false );
 
    QFile f( "/usr/info/dir" );
 
    if ( f.open(IO_ReadOnly) ) {
        QTextStream t( &f );
        QString s;
 
        int start, len;
 
        UDSEntryList uds_entry_list;
        UDSEntry     uds_entry;
        UDSAtom      uds_atom;
 
        uds_atom.m_uds = KIO::UDS_NAME; // we only do names...
        uds_entry.append( uds_atom );
 
        while ( !t.eof() ) {
            s = t.readLine();
 
            start = regex.match( s, 0, &len );
 
            if ( start != -1 ) {
            // Found "* infonode:", add "infonode" to matches
 
                int pos = 1;
                while ( pos < len && s[pos] == ' ')
                    pos++;
 
                QString name = s.mid( pos, (len-pos-1) ).lower();
 
                if ( !name.isEmpty() ) {
                    uds_entry[0].m_str = name;
                    uds_entry_list.append( uds_entry );
                }
            }
        }
        f.close();
 
        listEntries( uds_entry_list );
        finished();
    }
    else {
        kdError(7108) << "cannot open file '/usr/info/dir'" << endl;
    }
    kdDebug( 7108 ) << "InfoProtocol::listDir - done" << endl;
}

extern "C" { int kdemain( int argc, char **argv ); }

int kdemain( int argc, char **argv )
{
  KInstance instance( "kio_info" );

  kdDebug() << "kio_info starting " << getpid() << endl;

  if (argc != 4)
  {
     fprintf(stderr, "Usage: kio_file protocol domain-socket1 domain-socket2\n");
     exit(-1);
  }

  InfoProtocol slave( argv[2], argv[3] );
  slave.dispatchLoop();

  return 0;
}
