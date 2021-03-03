#include <kdebug.h>
#include <kconfig.h>

#include "connectionlist.h"

using namespace KDB;

ConnectionList::ConnectionList()
  : QValueList<ConnectionData>()
{
  m_config = new KConfig( "libkdbrc" );
}

ConnectionList::~ConnectionList()
{
  m_config->sync();
  delete m_config;
}

void ConnectionList::load()
{
  kdDebug(20020) << "ConnectionList::load" << endl;
  
  unsigned int count;
  
  m_config->setGroup( "Connections" );
  count = m_config->readUnsignedNumEntry( "count", 0 );
  
  for( unsigned int i = 0; i < count; i++ )
  {
    QString nr = QString( "%1_" ).arg( i );
    QStrList list;
    ConnectionData connection;
    
    if( !( m_config->hasKey( "plugin" ) || m_config->hasKey( "host" ) || m_config->hasKey( "user" ) ) )
      continue;
    
    connection.plugin = m_config->readEntry( nr + "plugin" );
    connection.host = m_config->readEntry( nr + "host" );
    connection.port = m_config->readUnsignedNumEntry( nr + "port", 0 );
    connection.user = m_config->readEntry( nr + "user" );
    connection.password = m_config->readEntry( nr + "password", "" );
    connection.askPassword = m_config->readBoolEntry( nr + "ask", true );
    connection.database = m_config->readEntry( nr + "database" );
    
    append( connection );
  }
}

void ConnectionList::save()
{
  kdDebug(20020) << "ConnectionList::save" << endl;

  unsigned int c = 0;
  
  m_config->setGroup( "Connections" );
  m_config->writeEntry( "count", count() );

  for( ConnectionListIterator it = begin() ; it != end() ; ++it )
  {
    QString nr = QString( "%1_" ).arg( c );
    
    m_config->writeEntry( nr + "plugin", (*it).plugin );
    m_config->writeEntry( nr + "host", (*it).host );
    m_config->writeEntry( nr + "port", (*it).port );
    m_config->writeEntry( nr + "user", (*it).user );
    m_config->writeEntry( nr + "password", (*it).password );
    m_config->writeEntry( nr + "ask", (*it).askPassword );
    m_config->writeEntry( nr + "database", (*it).database );
    
    c++;
  }
}
