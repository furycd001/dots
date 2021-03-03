// $Id: tar.cc,v 1.26 2001/04/05 09:31:33 faure Exp $

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

#include <qfile.h>
#include <kurl.h>
#include <kdebug.h>
#include <kinstance.h>
#include <ktar.h>
#include <kmimemagic.h>

#include <errno.h> // to be removed

#include "tar.h"

using namespace KIO;

extern "C" { int kdemain(int argc, char **argv); }

int kdemain( int argc, char **argv )
{
  KInstance instance( "kio_tar" );

  kdDebug(7109) << "Starting " << getpid() << endl;

  if (argc != 4)
  {
     fprintf(stderr, "Usage: kio_tar protocol domain-socket1 domain-socket2\n");
     exit(-1);
  }

  TARProtocol slave(argv[2], argv[3]);
  slave.dispatchLoop();

  kdDebug(7109) << "Done" << endl;
  return 0;
}

TARProtocol::TARProtocol( const QCString &pool, const QCString &app ) : SlaveBase( "tar", pool, app )
{
  kdDebug( 7109 ) << "TarProtocol::TarProtocol" << endl;
  m_tarFile = 0L;
}

TARProtocol::~TARProtocol()
{
    delete m_tarFile;
}

bool TARProtocol::checkNewFile( QString fullPath, QString & path )
{
    kdDebug(7109) << "TARProtocol::checkNewFile " << fullPath << endl;


    // Are we already looking at that file ?
    if ( m_tarFile && m_tarFile->fileName() == fullPath.left(m_tarFile->fileName().length()) )
    {
        // Has it changed ?
        struct stat statbuf;
        if ( ::stat( QFile::encodeName( m_tarFile->fileName() ), &statbuf ) == 0 )
        {
            if ( m_mtime == statbuf.st_mtime )
            {
                path = fullPath.mid( m_tarFile->fileName().length() );
                kdDebug(7109) << "TARProtocol::checkNewFile returning " << path << endl;
                return true;
            }
        }
    }
    kdDebug(7109) << "Need to open a new file" << endl;

    // Close previous file
    if ( m_tarFile )
    {
        m_tarFile->close();
        delete m_tarFile;
        m_tarFile = 0L;
    }

    // Find where the tar file is in the full path
    int pos = 0;
    QString tarFile;
    path = QString::null;

    int len = fullPath.length();
    if ( len != 0 && fullPath[ len - 1 ] != '/' )
        fullPath += '/';

    kdDebug(7109) << "the full path is " << fullPath << endl;
    while ( (pos=fullPath.find( '/', pos+1 )) != -1 )
    {
        QString tryPath = fullPath.left( pos );
        kdDebug(7109) << fullPath << "  trying " << tryPath << endl;
        struct stat statbuf;
        if ( ::stat( QFile::encodeName(tryPath), &statbuf ) == 0 && !S_ISDIR(statbuf.st_mode) )
        {
            tarFile = tryPath;
            m_mtime = statbuf.st_mtime;
            path = fullPath.mid( pos + 1 );
            kdDebug(7109) << "fullPath=" << fullPath << " path=" << path << endl;
            len = path.length();
            if ( len > 1 )
            {
                if ( path[ len - 1 ] == '/' )
                    path.truncate( len - 1 );
            }
            else
                path = QString::fromLatin1("/");
            kdDebug(7109) << "Found. tarFile=" << tarFile << " path=" << path << endl;
            break;
        }
    }
    if ( tarFile.isEmpty() )
    {
        kdDebug(7109) << "TARProtocol::checkNewFile: not found" << endl;
        return false;
    }

    // Open new file
    kdDebug(7109) << "Opening KTarGz on " << tarFile << endl;
    m_tarFile = new KTarGz( tarFile );
    if ( !m_tarFile->open( IO_ReadOnly ) )
    {
        kdDebug(7109) << "Opening " << tarFile << "failed." << endl;
        delete m_tarFile;
        m_tarFile = 0L;
        return false;
    }

    return true;
}


void TARProtocol::createUDSEntry( const KTarEntry * tarEntry, UDSEntry & entry )
{
    UDSAtom atom;
    entry.clear();
    atom.m_uds = UDS_NAME;
    atom.m_str = tarEntry->name();
    entry.append(atom);

    atom.m_uds = UDS_FILE_TYPE;
    atom.m_long = tarEntry->permissions() & S_IFMT; // keep file type only
    entry.append( atom );

    atom.m_uds = UDS_SIZE;
    atom.m_long = tarEntry->isFile() ? ((KTarFile *)tarEntry)->size() : 0L ;
    entry.append( atom );

    atom.m_uds = UDS_MODIFICATION_TIME;
    atom.m_long = tarEntry->date();
    entry.append( atom );

    atom.m_uds = UDS_ACCESS;
    atom.m_long = tarEntry->permissions() & 07777; // keep permissions only
    entry.append( atom );

    atom.m_uds = UDS_USER;
    atom.m_str = tarEntry->user();
    entry.append( atom );

    atom.m_uds = UDS_GROUP;
    atom.m_str = tarEntry->group();
    entry.append( atom );

    atom.m_uds = UDS_LINK_DEST;
    atom.m_str = tarEntry->symlink();
    entry.append( atom );
}

void TARProtocol::listDir( const KURL & url )
{
    kdDebug( 7109 ) << "TarProtocol::listDir " << url.url() << endl;

    QString path;
    if ( !checkNewFile( url.path(), path ) )
    {
        QCString _path( QFile::encodeName(url.path()));
        kdDebug( 7109 ) << "Checking (stat) on " << _path << endl;
        struct stat buff;
        if ( ::stat( _path.data(), &buff ) == -1 || !S_ISDIR( buff.st_mode ) ) {
            error( KIO::ERR_DOES_NOT_EXIST, url.path() );
            return;
        }
        // It's a real dir -> redirect
        KURL redir;
        redir.setPath( url.path() );
        kdDebug( 7109 ) << "Ok, redirection to " << redir.url() << endl;
        redirection( redir );
        finished();
        // And let go of the tar file - for people who want to unmount a cdrom after that
        delete m_tarFile;
        m_tarFile = 0L;
        return;
    }

    if ( path.isEmpty() )
    {
        KURL redir( QString::fromLatin1( "tar:/") );
        kdDebug() << "url.path()==" << url.path() << endl;
        redir.setPath( url.path() + QString::fromLatin1("/") );
        kdDebug() << "TARProtocol::listDir: redirection " << redir.url() << endl;
        redirection( redir );
        finished();
        return;
    }

    kdDebug( 7109 ) << "checkNewFile done" << endl;
    const KTarDirectory* root = m_tarFile->directory();
    const KTarDirectory* dir;
    if (!path.isEmpty() && path != "/")
    {
        kdDebug(7109) << QString("Looking for entry %1").arg(path) << endl;
        const KTarEntry* e = root->entry( path );
        if ( !e )
        {
            error( KIO::ERR_DOES_NOT_EXIST, path );
            return;
        }
        if ( ! e->isDirectory() )
        {
            error( KIO::ERR_IS_FILE, path );
            return;
        }
        dir = (KTarDirectory*)e;
    } else {
        dir = root;
    }

    QStringList l = dir->entries();
    totalSize( l.count() );

    UDSEntry entry;
    QStringList::Iterator it = l.begin();
    for( ; it != l.end(); ++it )
    {
        kdDebug(7109) << (*it) << endl;
        const KTarEntry* tarEntry = dir->entry( (*it) );

        createUDSEntry( tarEntry, entry );

        listEntry( entry, false );
    }

    listEntry( entry, true ); // ready

    finished();

    kdDebug( 7109 ) << "TarProtocol::listDir done" << endl;
}

void TARProtocol::stat( const KURL & url )
{
    QString path;
    UDSEntry entry;
    if ( !checkNewFile( url.path(), path ) )
    {
        // We may be looking at a real directory - this happens
        // when pressing up after being in the root of an archive
        QCString _path( QFile::encodeName(url.path()));
        kdDebug( 7109 ) << "TARProtocol::stat (stat) on " << _path << endl;
        struct stat buff;
        if ( ::stat( _path.data(), &buff ) == -1 || !S_ISDIR( buff.st_mode ) ) {
            kdDebug() << "isdir=" << S_ISDIR( buff.st_mode ) << "  errno=" << strerror(errno) << endl;
            error( KIO::ERR_DOES_NOT_EXIST, url.path() );
            return;
        }
        // Real directory. Return just enough information for KRun to work
        UDSAtom atom;
        atom.m_uds = KIO::UDS_NAME;
        atom.m_str = url.fileName();
        entry.append( atom );
        kdDebug( 7109 ) << "TARProtocol::stat returning name=" << url.fileName() << endl;

        atom.m_uds = KIO::UDS_FILE_TYPE;
        atom.m_long = buff.st_mode & S_IFMT;
        entry.append( atom );

        statEntry( entry );

        finished();

        // And let go of the tar file - for people who want to unmount a cdrom after that
        delete m_tarFile;
        m_tarFile = 0L;
        return;
    }

    const KTarDirectory* root = m_tarFile->directory();
    const KTarEntry* tarEntry;
    if ( path.isEmpty() )
    {
        path = QString::fromLatin1( "/" );
        tarEntry = root;
    } else {
        tarEntry = root->entry( path );
    }
    if ( !tarEntry )
    {
        error( KIO::ERR_DOES_NOT_EXIST, path );
        return;
    }

    createUDSEntry( tarEntry, entry );
    statEntry( entry );

    finished();
}

void TARProtocol::get( const KURL & url )
{
    kdDebug( 7109 ) << "TarProtocol::get" << url.url() << endl;

    QString path;
    if ( !checkNewFile( url.path(), path ) )
    {
        error( KIO::ERR_DOES_NOT_EXIST, url.path() );
        return;
    }

    const KTarDirectory* root = m_tarFile->directory();
    const KTarEntry* tarEntry = root->entry( path );

    if ( !tarEntry )
    {
        error( KIO::ERR_DOES_NOT_EXIST, path );
        return;
    }
    if ( tarEntry->isDirectory() )
    {
        error( KIO::ERR_IS_DIRECTORY, path );
        return;
    }
    const KTarFile* tarFileEntry = static_cast<const KTarFile *>(tarEntry);
    if ( !tarEntry->symlink().isEmpty() )
    {
      kdDebug(7102) << "Redirection to " << tarEntry->symlink() << endl;
      KURL realURL( url, tarEntry->symlink() );
      kdDebug(7102) << "realURL= " << realURL.url() << endl;
      redirection( realURL.url() );
      finished();
      return;
    }

    totalSize( tarFileEntry->size() );

    QByteArray completeData = tarFileEntry->data();

    KMimeMagicResult * result = KMimeMagic::self()->findBufferFileType( completeData, path );
    kdDebug(7102) << "Emitting mimetype " << result->mimeType() << endl;
    mimeType( result->mimeType() );

    data( completeData );

    processedSize( tarFileEntry->size() );

    finished();
}

/*
  In case someone wonders how the old filter stuff looked like :    :)
void TARProtocol::slotData(void *_p, int _len)
{
  switch (m_cmd) {
    case CMD_PUT:
      assert(m_pFilter);
      m_pFilter->send(_p, _len);
      break;
    default:
      abort();
      break;
    }
}

void TARProtocol::slotDataEnd()
{
  switch (m_cmd) {
    case CMD_PUT:
      assert(m_pFilter && m_pJob);
      m_pFilter->finish();
      m_pJob->dataEnd();
      m_cmd = CMD_NONE;
      break;
    default:
      abort();
      break;
    }
}

void TARProtocol::jobData(void *_p, int _len)
{
  switch (m_cmd) {
  case CMD_GET:
    assert(m_pFilter);
    m_pFilter->send(_p, _len);
    break;
  case CMD_COPY:
    assert(m_pFilter);
    m_pFilter->send(_p, _len);
    break;
  default:
    abort();
  }
}

void TARProtocol::jobDataEnd()
{
  switch (m_cmd) {
  case CMD_GET:
    assert(m_pFilter);
    m_pFilter->finish();
    dataEnd();
    break;
  case CMD_COPY:
    assert(m_pFilter);
    m_pFilter->finish();
    m_pJob->dataEnd();
    break;
  default:
    abort();
  }
}

void TARProtocol::filterData(void *_p, int _len)
{
debug("void TARProtocol::filterData");
  switch (m_cmd) {
  case CMD_GET:
    data(_p, _len);
    break;
  case CMD_PUT:
    assert (m_pJob);
    m_pJob->data(_p, _len);
    break;
  case CMD_COPY:
    assert(m_pJob);
    m_pJob->data(_p, _len);
    break;
  default:
    abort();
  }
}
*/

