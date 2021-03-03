/*
    kshorturifilter.h

    This file is part of the KDE project
    Copyright (C) 2000 Dawit Alemayehu <adawit@kde.org>
    Copyright (C) 2000 Malte Starostik <starosti@zedat.fu-berlin.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>

#include <qdir.h>
#include <qlist.h>

#include <kurl.h>
#include <kdebug.h>
#include <kprotocolinfo.h>
#include <klocale.h>
#include <kinstance.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kconfig.h>

//#include "kshorturiopts.h"
#include "kshorturifilter.h"


#define FQDN_PATTERN    "[a-zA-Z][a-zA-Z0-9-]*\\.[a-zA-Z]"
#define IPv4_PATTERN    "[0-9][0-9]?[0-9]?\\.[0-9][0-9]?[0-9]?\\.[0-9][0-9]?[0-9]?\\.[0-9][0-9]?[0-9]?:?[[0-9][0-9]?[0-9]?]?/?"
#define ENV_VAR_PATTERN "$[a-zA-Z_][a-zA-Z0-9_]*"

#define QFL1(x) QString::fromLatin1(x)

typedef QMap<QString,QString> EntryMap;

KInstance *KShortURIFilterFactory::s_instance = 0;

KShortURIFilter::KShortURIFilter( QObject *parent, const char *name )
                :KURIFilterPlugin( parent, name ? name : "kshorturifilter", 1.0),
                 DCOPObject("KShortURIFilterIface")
{
    configure();
    m_strDefaultProtocol = QFL1("http://");
}

bool KShortURIFilter::isValidShortURL( const QString& cmd ) const
{
  // Loose many of the QRegExp matches as they tend
  // to slow things down.  They are also unnecessary!! (DA)
  if ( cmd[cmd.length()-1] == '&' || !cmd.contains('.') ||
       cmd.contains(QFL1("||")) || cmd.contains(QFL1("&&")) ||
       cmd.contains(QRegExp(QFL1("[ ;<>]"))) )
       return false;

  return true;
}

bool KShortURIFilter::expandEnvVar( QString& cmd ) const
{
  // ENVIRONMENT variable expansion
  int env_len = 0, env_loc = 0;
  bool matchFound = false;
  QRegExp r (QFL1(ENV_VAR_PATTERN));
  while( 1 )
  {
    env_loc = r.match( cmd, env_loc, &env_len );
    if( env_loc == -1 ) break;
    const char* exp = getenv( cmd.mid( env_loc + 1, env_len - 1 ).local8Bit().data() );
    if(exp)
    {
      cmd.replace( env_loc, env_len, QString::fromLocal8Bit(exp) );
      matchFound = true;
    }
    else
    {
      env_loc++; // skip past the '$' or end up in endless loop!!
    }
  }
  return matchFound;
}

bool KShortURIFilter::filterURI( KURIFilterData& data ) const
{
 /*
  * Here is a description of how the shortURI deals with the supplied
  * data.  First it expands any environment variable settings and then
  * deals with special shortURI cases. These special cases are the "smb:"
  * URL scheme which is very specific to KDE, "#" and "##" which are
  * shortcuts for man:/ and info:/ protocols respectively. It then handles
  * local files.  Then it checks to see if the URL is valid and one that is
  * supported by KDE's IO system.  If all the above checks fails, it simply
  * lookups the URL in the user-defined list and returns without filtering
  * if it is not found. TODO: the user-defined table is currently only manually
  * hackable and is missing a config dialog.  Simply copying the file
  */
  KURL url = data.uri();
  QString cmd = url.url();

  // Environment variable expansion.
  if ( expandEnvVar( cmd ) )
    url = cmd;

  // TODO: Make this a bit more intelligent for Minicli! There
  // is no need to make comparisons if the supplied data is a local
  // executable and only the argument part, if any, changed!

  int temp_int(0);
  // Handle SMB Protocol shortcuts ...
  //Hmm, don't handle them ! (aleXXX)
  /*int temp_int = cmd.find( QFL1("smb:"), 0, false );
  if ( temp_int == 0 || cmd.find( QFL1("\\\\") ) == 0 )
  {
    if( temp_int == 0 )
      cmd = QDir::cleanDirPath( cmd.mid( 4 ) );
    else
    {
      temp_int = 0;
      while( cmd[temp_int] == '\\' ) temp_int++;
      cmd = cmd.mid( temp_int );
    }

    for (uint i=0; i < cmd.length(); i++)
    {
      if (cmd[i]=='\\')
        cmd[i]='/';
    }
    cmd[0] == '/' ? cmd.prepend( QFL1("smb:") ) : cmd.prepend( QFL1("smb:/") );
    setFilteredURI( data, cmd );
    setURIType( data, KURIFilterData::NET_PROTOCOL );
    return true;
  }*/

  // Handle MAN & INFO pages shortcuts...
  QString man_proto = QFL1("man:");
  QString info_proto = QFL1("info:");
  if( cmd[0] == '#' ||
      cmd.find( man_proto, 0, true ) == 0 ||
      cmd.find( info_proto, 0, true ) == 0 )
  {
    temp_int = cmd.length();
    if( cmd.left(2) == QFL1("##") )
      cmd = QFL1("info:/") + ( temp_int == 2 ? QFL1("dir") : cmd.mid(2));
    else if ( cmd[0] == '#' )
      cmd = QFL1("man:/") + cmd.mid(1);

    else if ( temp_int == (int)man_proto.length() && cmd.contains(QFL1("man:")))
      cmd += '/';
    else if ( temp_int == (int)info_proto.length() && cmd.contains(QFL1("info:")))
      cmd += QFL1( "/dir" );
    setFilteredURI( data, cmd );
    setURIType( data, KURIFilterData::HELP );
    return true;
  }

  // Filter for the about command.
  if ( cmd == ( QFL1("about:") ) )
  {
    cmd = QFL1("about:konqueror");
    setFilteredURI( data, cmd );
    setURIType( data, KURIFilterData::NET_PROTOCOL );
    return true;
  }

  // LOCAL URL TEST CASE BEGINS...
  // Remove any leading "file:" if present
  // from the supplied command.
  if( cmd.startsWith( QFL1( "file:") ) )
    url = cmd.remove(0,5);

  // Expanding shortcut to HOME URL...
  if( cmd[0] == '~' )
  {
    temp_int = cmd.find('/');
    if( temp_int == -1 )
      temp_int = cmd.length();
    if( temp_int == 1 )
    {
      cmd.replace ( 0, 1, QDir::homeDirPath() );
      url = cmd;
    }
    else
    {
      QString user = cmd.mid( 1, temp_int-1 );
      struct passwd *dir = getpwnam(user.local8Bit().data());
      if( dir && strlen(dir->pw_dir) )
      {
        cmd.replace (0, temp_int, QString::fromLocal8Bit(dir->pw_dir));
        url = cmd;          // update the URL...
      }
      else
      {
        QString msg = dir ? i18n("<qt><b>%1</b> doesn't have a home directory!</qt>").arg(user) :
                            i18n("<qt>There is no user called <b>%1</b>.</qt>").arg(user);
        setErrorMsg( data, msg );
        setURIType( data, KURIFilterData::ERROR );
        // Always return true for error conditions so
        // that other filters will not be invoked !!
        return true;
      }
    }
  }

  // Checking for local resource match...
  // Determine if "uri" is an absolute path to a local resource  OR
  // A local resource with a supplied absolute path in KURIFilterData
  QString abs_path = data.absolutePath();
  bool fileNotFound = false;
  bool canBeAbsolute = (url.isMalformed() && !abs_path.isEmpty());
  bool canBeLocalAbsolute = (canBeAbsolute && abs_path[0] =='/');
  if( cmd[0] == '/' || canBeLocalAbsolute )
  {
    struct stat buff;
    cmd = QDir::cleanDirPath( cmd );
    if( cmd[0] == '/' )
    {
      temp_int = stat( cmd.local8Bit().data() , &buff );
    }
    else
    {
      temp_int = cmd.length();
      if( (temp_int==1 && cmd[0]=='.') || (temp_int==2 && cmd[0]=='.' && cmd[1]=='.') )
        cmd += '/';
      abs_path = QDir::cleanDirPath(abs_path + '/' + cmd);
      temp_int = stat( abs_path.local8Bit().data() , &buff );
      if( temp_int == 0 )
        cmd = abs_path;
    }

    if( temp_int == 0 )
    {
      bool isDir = S_ISDIR( buff.st_mode );
      if( !isDir && access (cmd.local8Bit().data(), X_OK) == 0 )
      {
        setFilteredURI( data, cmd );
        setURIType( data, KURIFilterData::EXECUTABLE );
        return true;
      }
      // Open "uri" as file:/xxx if it is a non-executable local resource.
      if( isDir || S_ISREG( buff.st_mode ) )
      {
        setFilteredURI( data, cmd );
        setURIType( data, ( isDir ) ? KURIFilterData::LOCAL_DIR : KURIFilterData::LOCAL_FILE );
        return true;
      }
    }
    else
    {
        fileNotFound = true;
    }
  }

  // Let us deal with possible relative URLs to see
  // if it is executable under the user's $PATH variable.
  // We try hard to avoid parsing any possible command
  // line arguments or options that might have been supplied.
  abs_path = cmd;
  temp_int = abs_path.find( ' ' );
  if( temp_int > 0 )
  {
    QChar ch = abs_path[0];
    if( ch != '\'' && ch != '"' && cmd[temp_int - 1] != '\\' )
        abs_path = abs_path.left( temp_int );
  }

  if( !KStandardDirs::findExe( abs_path ).isNull() )
  {
    setFilteredURI( data, abs_path );
    // check if we have command line arguments
    if( abs_path != cmd )
        setArguments(data, cmd.right(cmd.length() - temp_int));
    setURIType( data, KURIFilterData::EXECUTABLE );
    return true;
  }

  // Process URLs of known and supported protocols so we don't have
  // to resort to the pattern matching scheme below which can possibly
  // be slow things down...
  QStringList protocols = KProtocolInfo::protocols();
  for( QStringList::ConstIterator it = protocols.begin(); it != protocols.end(); it++ )
  {
    if( (cmd.left((*it).length()).lower() == *it) &&
        !url.isMalformed() && !url.isLocalFile() )
    {
      setFilteredURI( data, cmd );
      if ( *it == QFL1("man") || *it == QFL1("help") )
        setURIType( data, KURIFilterData::HELP );
      else
        setURIType( data, KURIFilterData::NET_PROTOCOL );
      return true;
    }
  }

  // Provided as a filter for remote URLs.  Example,
  // if the current path is ftp://ftp.kde.org/pub/
  // and user typed ../ in the location bar they would
  // correctly get ftp://ftp.kde.org/. Is that cool or what ??
  if( canBeAbsolute && !canBeLocalAbsolute )
  {
    KURL u( KURL( data.absolutePath() ), cmd );
    if( !u.isMalformed() )
    {
      setFilteredURI( data, u.url() );
      setURIType( data, KURIFilterData::NET_PROTOCOL );
      return true;
    }
  }

  // Okay this is the code that allows users to supply custom
  // matches for specific URLs using Qt's regexp class.  This
  // is hard-coded for now in the constructor, but will soon be
  // moved to the config dialog so that people can configure this
  // stuff.  This is perhaps one of those unecessary but somewhat
  // useful features that usually makes people go WHOO and WHAAA.
  if ( !cmd.contains( ' ' ) )
  {
    QRegExp match;
    QValueList<URLHint>::ConstIterator it;
    for( it = m_urlHints.begin(); it != m_urlHints.end(); ++it )
    {
        match = (*it).regexp;
        if ( match.match( cmd, 0 ) == 0 )
        {
            cmd.prepend( (*it).prepend );
            setFilteredURI( data, cmd );
            setURIType( data, KURIFilterData::NET_PROTOCOL );
            return true;
        }
    }
    // If cmd is NOT a local resource, check if it
    // is a valid "shortURL" candidate and append
    // the default protocol the user supplied. (DA)
    if ( url.isMalformed() && isValidShortURL(cmd) )
    {
        cmd.insert( 0, m_strDefaultProtocol );
        setFilteredURI( data, cmd );
        setURIType( data, KURIFilterData::NET_PROTOCOL );
        return true;
    }
  }

  // If we previously determined that we want
  if( fileNotFound )
  {
    setErrorMsg( data, QString::null );
    setURIType( data, KURIFilterData::ERROR );
    return true;
  }

  // If we reach this point, we cannot filter
  // this thing so simply return false so that
  // other filters, if present, can take a crack
  // at it.
  return false;
}

KCModule* KShortURIFilter::configModule( QWidget*, const char* ) const
{
        return 0; //new KShortURIOptions( parent, name );
}

QString KShortURIFilter::configName() const
{
    return i18n("&ShortURLs");
}

void KShortURIFilter::configure()
{
    KConfig config( name() + QFL1("rc"), false, false );
    EntryMap map = config.entryMap( QFL1("Pattern Matching") );
    if( !map.isEmpty() )
    {
        EntryMap::Iterator it = map.begin();
        for( ; it != map.end(); ++it )
            m_urlHints.append( URLHint(it.key(), it.data()) );
    }

    // Include some basic defaults.  Note these will always be
    // overridden by a users entries. TODO: Make this configurable
    // from the control panel.
    m_urlHints.append( URLHint(QFL1(IPv4_PATTERN), QFL1("http://")) );
    m_urlHints.append( URLHint(QFL1(FQDN_PATTERN), QFL1("http://")) );
}

/***************************************** KShortURIFilterFactory *******************************************/

KShortURIFilterFactory::KShortURIFilterFactory( QObject *parent, const char *name )
                       :KLibFactory( parent, name )
{
    s_instance = new KInstance( "kshorturifilter" );
}

KShortURIFilterFactory::~KShortURIFilterFactory()
{
    delete s_instance;
}

QObject *KShortURIFilterFactory::create( QObject *parent, const char *name, const char*, const QStringList & )
{
    QObject *obj = new KShortURIFilter( parent, name );
    emit objectCreated( obj );
    return obj;
}

KInstance *KShortURIFilterFactory::instance()
{
    return s_instance;
}

extern "C"
{
    void *init_libkshorturifilter()
    {
        return new KShortURIFilterFactory;
    }
}

#include "kshorturifilter.moc"
