// $Id: kio_ldap.cpp,v 1.15 2001/03/06 15:43:55 matz Exp $

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef TIME_WITH_SYS_TIME
#include <sys/time.h>
#endif

#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include <lber.h>
#include <ldap.h>
#include <kdebug.h>
#include <kldap.h>
#include <kldapurl.h>
#include <kinstance.h>

#include "kio_ldap.h"

using namespace KIO;

extern "C" { int kdemain(int argc, char **argv); }

/**
 * The main program.
 */
int kdemain(int argc, char **argv)
{
    KInstance instance( "kio_ldap" );
    // redirect the signals
    //signal(SIGCHLD, KIOProtocol::sigchld_handler);
    //signal(SIGSEGV, KIOProtocol::sigsegv_handler);

    kdDebug() << "kio_ldap : Starting " << getpid() << endl;

    if (argc != 4) {
	fprintf(stderr, "Usage kio_ldap protocol pool app\n");
	return -1;
    }
    // let the protocol class do its work
    LDAPProtocol slave(argv[2], argv[3]);
    slave.dispatchLoop();

    kdDebug() << "kio_ldap : Done" << endl;
    return 0;
}


/**
 * Initialize the ldap slave
 */
LDAPProtocol::LDAPProtocol(const QCString &pool, const QCString &app)
  : SlaveBase( "ldap", pool, app) 
{
    kdDebug(7110) << "LDAPProtocol::LDAPProtocol" << endl;
}

void LDAPProtocol::setHost( const QString& _host, int _port,
			    const QString& _user, const QString& _pass )
{
  urlPrefix = "ldap://";
  if (!_user.isEmpty()) {
    urlPrefix += _user;
    if (!_pass.isEmpty())
      urlPrefix += ":" + _pass;
    urlPrefix += "@";
  }
  urlPrefix += _host;
  if (_port)
    urlPrefix += QString( ":%1" ).arg( _port );
  kdDebug() << "urlPrefix " << urlPrefix << endl;
}

/**
 * Get the information contained in the URL.
 */
void LDAPProtocol::get(const KURL &url)
{
  /*QString _url = urlPrefix + path;
  if (!query.isEmpty()) { _url += "?" + query; }*/
  QString _url = url.url();
  kdDebug(7110) << "kio_ldap::get(" << _url << ")" << endl;
  KLDAP::Url usrc(_url);

  // check if the URL is a valid LDAP URL
  if (usrc.isMalformed()) {
    error(ERR_MALFORMED_URL, _url);
    return;
  }

  // take the time
  time_t t_start = time( 0L );

  // initiate the search
  KLDAP::Connection c;
  /*if (0 && !c.authenticate()) {    //FIX:user...
      error(ERR_COULD_NOT_AUTHENTICATE, "bla");
      return;
      }*/
  KLDAP::SearchRequest search(c, _url.latin1(), KLDAP::Request::Synchronous);

  // wait for the request
  search.execute();
  search.finish();

  // collect the result
  QString result = search.asLDIF();

  // tell the mimetype
  mimeType("text/plain");

  // tell the length
  int processed_size = result.length();
  totalSize(processed_size);  

  // tell the contents of the URL
  QByteArray array;
  int cnt=0;
  while (cnt < processed_size)
    {
      if (result.length()-cnt > 1024)
	{
	  
	  array.setRawData(result.mid(cnt,1024).latin1(), 1024);
	  data(array);
	  array.resetRawData(result.mid(cnt,1024).latin1(), 1024);
	  cnt += 1024;
	}
      else
	{
	  array.setRawData(result.latin1(), result.length()-cnt);
	  data(array);
	  array.resetRawData(result.latin1(), result.length()-cnt);
	  cnt = processed_size;
	}
      // tell how much we got
      processedSize(cnt);
    }

  // tell we are finished
  data(QByteArray());
  
  // tell how long it took
  time_t t = time( 0L );
  if ( t - t_start >= 1 )
    speed( processed_size / ( t - t_start ) );  

  // tell we are finished
  finished();
}


/**
 * Test if the url contains a directory or a file.
 */
void LDAPProtocol::stat( const KURL &a_url )
{
  /*QString _url = urlPrefix + path;
  if (!query.isEmpty()) { _url += "?" + query; }*/
  QString _url = a_url.url();
  kdDebug(7110) << "kio_ldap: stat(" << _url << ")" << endl;
  KLDAP::Url usrc(_url);

  // check if the URL is a valid LDAP URL
  if (usrc.isMalformed()) {
    error(ERR_MALFORMED_URL, _url);
    return;
  }

  // look how many entries match
  KLDAP::Connection c;
/*  if (0 && !c.authenticate()) {    //FIX:user...
      error(ERR_COULD_NOT_AUTHENTICATE, "bla");
      return;
      }*/
  KLDAP::SearchRequest search(c, _url.local8Bit(), KLDAP::Request::Synchronous);
  QStrList att;
  att.append("dn");
  search.setAttributes(att);
  if (a_url.query().isEmpty()) search.setScope(LDAP_SCOPE_ONELEVEL);
  search.execute();
  search.finish();
  int cnt=0;
  for (KLDAP::Entry e=search.first(); !search.end(); e=search.next())
    cnt++;
  int isDir = 1;
  bool isQuery = 0;
  if (a_url.query().isEmpty()) {
    /* we searched for a subdir */
    if (cnt == 0) isDir=0;
  } else {
    /* we searched for what the user specified */
    if (usrc.scope() == LDAP_SCOPE_BASE) isDir = 0;    /* he only wanted base */
    else {
      /* he wanted more */
      if (cnt <= 1) isDir = 0;   /* but there isn't */
      else isQuery = 1;          /* e.g. /cn=bla??sub?(uid=23) */
    }
  }
  UDSEntry entry;
  UDSAtom atom;

  int pos;
  atom.m_uds = UDS_NAME;
  atom.m_long = 0;
  QString name = usrc.dn();
  if ((pos = name.find(",")) > 0)
    name = name.left(pos);
  if ((pos = name.find("=")) > 0)
    name.remove(0,pos+1);
  atom.m_str = name;
  entry.append(atom);

  atom.m_uds = UDS_FILE_TYPE;
  atom.m_str = "";
  if (isQuery) {
    if (cnt > 1)
      atom.m_long = S_IFDIR;
    else 
      atom.m_long = S_IFREG;
  } else if (isDir)
    atom.m_long = S_IFDIR;
  else 
    atom.m_long = S_IFREG;
  entry.append(atom);
  
  atom.m_uds = KIO::UDS_ACCESS;
  atom.m_long = isDir ? 0500 : 0400;
  entry.append(atom);

  atom.m_uds = UDS_URL;
  atom.m_long = 0;
  KLDAP::Url url(urlPrefix);
  //url.setProtocol("ldap");
  url.setHost(usrc.host());
  url.setPort(usrc.port());
  url.setPath("/"+usrc.dn());
  if (isQuery)
    url.setScope(usrc.scope());
  else if (isDir)
    url.setScope(LDAP_SCOPE_ONELEVEL);
  else
    url.setScope(LDAP_SCOPE_BASE);
  atom.m_str = url.url();
  kdDebug(7110) << "kio_ldap:stat put url:" << atom.m_str << endl;
  entry.append(atom);

  if (!isDir || (isQuery && cnt==1)) {
    atom.m_uds = UDS_MIME_TYPE;
    atom.m_long = 0;
    atom.m_str = "text/plain";
    entry.append(atom);
  }

  statEntry(entry);
  // we are done
  finished();
}

#if 0
/**
 * Get the mimetype. For now its text/plain for each non-subentry
 */
void LDAPProtocol::mimetype(const KURL &url)
{
  /*QString _url = urlPrefix + path;
  if (!query.isEmpty()) { _url += "?" + query; }*/
  QString _url = url.url();
  kdDebug(7110) << "kio_ldap: mimetype(" << _url << ")" << endl;
  KLDAP::Url usrc(_url);
  if (usrc.isMalformed()) {
    error(ERR_MALFORMED_URL, _url);
    return;
  }
  kdDebug(7110) << "kio_ldap: query()==" << url.query() << endl;
  if (!url.query().isEmpty()) {
    if (usrc.scope() == LDAP_SCOPE_BASE)
      mimeType("text/plain");
    else if (usrc.scope() == LDAP_SCOPE_SUBTREE)
      mimeType("text/plain");
  } else {
    /* empty scope, or ONELEVEL/SUB */
    mimeType("inode/directory");
  }
  finished();
}
#endif

/**
 * List the contents of a directory.
 */
void LDAPProtocol::listDir(const KURL &url)
{
  unsigned long total=0, actual=0, dirs=0;
  /*QString _url = urlPrefix + path;
  if (!query.isEmpty()) { _url += "?" + query; }*/
  QString _url = url.url();
  kdDebug(7110) << "kio_ldap: listDir(" << _url << ")" << endl;
  KLDAP::Url usrc(_url);

  // check if the URL is a valid LDAP URL
  if (usrc.isMalformed()) {
    error(ERR_MALFORMED_URL, _url);
    return;
  }

  // look up the entries
  KLDAP::Connection c;
  /*if (0 && !c.authenticate()) {    //FIX:user...
      error(ERR_COULD_NOT_AUTHENTICATE, "bla");
      return;
      }*/
  KLDAP::SearchRequest search(c, _url, KLDAP::Request::Synchronous);
  QStrList att;
  att.append("dn");
  search.setAttributes(att);
  if (url.query().isEmpty() || usrc.scope() == LDAP_SCOPE_BASE)
    search.setScope(LDAP_SCOPE_ONELEVEL);
  search.execute();
  search.finish();

  // publish the results
  UDSEntry entry;
  UDSAtom atom;

  // publish the sub-directories
  for (KLDAP::Entry e=search.first(); !search.end(); e=search.next())
    {
      total++;
      totalSize(total+dirs);
      entry.clear();

      // test if it is really a directory (NOTE: This is expensive!)
      KLDAP::SearchRequest search2(c, usrc.url().local8Bit(), KLDAP::Request::Synchronous);
      search2.setBase(e.dn());
      search2.setScope(LDAP_SCOPE_ONELEVEL);
      search2.setAttributes(att);
      search2.execute();
      search2.finish();
      int cnt=0;
      for (KLDAP::Entry e2=search2.first(); !search2.end(); e2=search2.next())
	cnt++;

      if (cnt > 0)
	{
	  dirs++;
	  totalSize(total+dirs);
	  // the name
	  int pos;
	  atom.m_uds = UDS_NAME;
	  atom.m_long = 0;
	  QString name = e.dn();
	  if ((pos = name.find(",")) > 0)
	    name = name.left(pos);
	  if ((pos = name.find("=")) > 0)
	    name.remove(0,pos+1);
	  atom.m_str = name;
	  entry.append(atom);
	  
	  // the file type
	  atom.m_uds = UDS_FILE_TYPE;
	  atom.m_str = "";
	  atom.m_long = S_IFDIR;
	  entry.append(atom);
	  
          atom.m_uds = KIO::UDS_ACCESS;
          atom.m_long = 0500;
          entry.append(atom);

	  // the url
	  atom.m_uds = UDS_URL;
	  atom.m_long = 0;
	  KLDAP::Url url(urlPrefix);
	  //kdDebug(7110) << "kio_ldap:listDir(dir) put url1:" << endl;
	  //url.setProtocol("ldap");
	  //kdDebug(7110) << "kio_ldap:listDir(dir) put url2:" << endl;
	  url.setHost(usrc.host());
	  //kdDebug(7110) << "kio_ldap:listDir(dir) put url3:" << endl;
	  url.setPort(usrc.port());
	  //kdDebug(7110) << "kio_ldap:listDir(dir) put url4:" << endl;
	  url.setPath("/"+e.dn());
	  //kdDebug(7110) << "kio_ldap:listDir(dir) put url5:" << endl;
	  url.setScope(LDAP_SCOPE_ONELEVEL);
	  atom.m_str = url.url();
	  kdDebug(7110) << "kio_ldap:listDir(dir) put url:" << atom.m_str << endl;
	  entry.append(atom);

	  listEntry(entry, false);
	  actual++;
	  processedSize(actual);
	}
    }

  totalSize(total+dirs);
  actual = dirs;
  processedSize(actual);
  // publish the nodes
  for (KLDAP::Entry e=search.first(); !search.end(); e=search.next())
    {
      entry.clear();
      
      // the name
      int pos;
      atom.m_uds = UDS_NAME;
      atom.m_long = 0;
      QString name = e.dn();
      if ((pos = name.find(",")) > 0)
	name = name.left(pos);
      if ((pos = name.find("=")) > 0)
	name.remove(0,pos+1);
      atom.m_str = name;
      entry.append(atom);

      // the file type
      atom.m_uds = UDS_FILE_TYPE;
      atom.m_str = "";
      atom.m_long = S_IFREG;
      entry.append(atom);

      atom.m_uds = KIO::UDS_ACCESS;
      atom.m_long = 0400;
      entry.append(atom);

      // the mimetype
      atom.m_uds = UDS_MIME_TYPE;
      atom.m_long = 0;
      atom.m_str = "text/plain";
      entry.append(atom);

      // the url
      atom.m_uds = UDS_URL;
      atom.m_long = 0;
      KLDAP::Url url(urlPrefix);
      //url.setProtocol("ldap");
      url.setHost(usrc.host());
      url.setPort(usrc.port());
      url.setPath("/"+e.dn());
      url.setScope(LDAP_SCOPE_BASE);
      atom.m_str = url.url();
      QString dbgurl = url.url();
      kdDebug(7110) << "kio_ldap:listDir(file) put url:" << url.url() << endl;
      entry.append(atom);

      listEntry(entry, false);
      actual++;
      processedSize(actual);
    }
  entry.clear();
  listEntry(entry, true);
  processedSize(total+dirs);
  // we are done
  finished();  
}
