
/***************************************************************************
                          kio_finger.cpp  -  description
                             -------------------
    begin                : Sun Aug 12 2000
    copyright            : (C) 2000 by Andreas Schlapbach
    email                : schlpbch@iam.unibe.ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/* $Id: kio_finger.cpp,v 1.15 2001/05/11 20:59:50 schlpbch Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include <qtextstream.h>
#include <qdict.h>
#include <qcstring.h>
#include <qfile.h>

#include <kdebug.h>
#include <kinstance.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <klocale.h>
#include <kurl.h>
#include <kregexp.h>

#include "kio_finger.h"


using namespace KIO;

static const QString defaultRefreshRate = "60";

extern "C"
{
  int kdemain( int argc, char **argv )
  {
    KInstance instance( "kio_finger" );
    
    //kdDebug() << "*** Starting kio_finger " << getpid() << endl;
    
    if (argc != 4)
      {
	fprintf(stderr, "Usage: kio_finger protocol domain-socket1 domain-socket2\n");
	exit(-1);
      }
    
    FingerProtocol slave(argv[2], argv[3]);
    slave.dispatchLoop();
    
    //kdDebug() << "*** kio_finger Done" << endl;
    return 0;
  }
} 

   
/* ---------------------------------------------------------------------------------- */


FingerProtocol::FingerProtocol(const QCString &pool_socket, const QCString &app_socket)
  : QObject(), SlaveBase("finger", pool_socket, app_socket)
{
  myStdStream = new QString();
  getProgramPath();
}


/* ---------------------------------------------------------------------------------- */


FingerProtocol::~FingerProtocol()
{
  //kdDebug() << "FingerProtocol::~FingerProtocol()" << endl;
  delete myURL;
  delete myPerlPath;
  delete myFingerPath;
  delete myFingerPerlScript;
  delete myFingerCSSFile;
  delete myStdStream;
}


/* ---------------------------------------------------------------------------------- */


void FingerProtocol::get(const KURL& url )
{
  //kdDebug() << "kio_finger::get(const KURL& url)" << endl ;
	      
  this->parseCommandLine(url);

  //kdDebug() << "myURL: " << myURL->prettyURL() << endl;

  // Reset the stream
  *myStdStream="";
  
  QString query = myURL->query();
  QString refreshRate = defaultRefreshRate;

  //kdDebug() << "query: " << query << endl;
  
  // Check the validity of the query 

  QRegExp regExp("?refreshRate=[0-9][0-9]*", true, true);
  if (query.contains(regExp)) {
    //kdDebug() << "looks like a valid query" << endl;
    KRegExp regExp( "([0-9]+)" );
    regExp.match(query.local8Bit());
    refreshRate = regExp.group(0);
  }
  
  //kdDebug() << "Refresh rate: " << refreshRate << endl;
 
  myKProcess = new KShellProcess();  
  *myKProcess << *myPerlPath << *myFingerPerlScript 
	      << *myFingerPath << *myFingerCSSFile 
	      << refreshRate << myURL->host() << myURL->user() ;
  	
  connect(myKProcess, SIGNAL(receivedStdout(KProcess *, char *, int)), 
	  this, SLOT(slotGetStdOutput(KProcess *, char *, int)));
  //connect(myKProcess, SIGNAL(receivedStderr(KProcess *, char *, int)), 
  //	  this, SLOT(slotGetStdOutput(KProcess *, char *, int)));
  
  myKProcess->start(KProcess::Block, KProcess::All);

  data(QCString(myStdStream->local8Bit()));
 
  data(QByteArray());
  finished();  

  //clean up
  
  delete myKProcess;
}


/* ---------------------------------------------------------------------------------- */


void FingerProtocol::slotGetStdOutput(KProcess* /* p */, char *s, int len) 
{
  //kdDebug() <<  "void FingerProtocol::slotGetStdoutOutput()" << endl;		
  *myStdStream += QString::fromLocal8Bit(s, len);
}


/* ---------------------------------------------------------------------------------- */


void FingerProtocol::mimetype(const KURL & /*url*/)
{
  mimeType("text/html");
  finished();
}
     

/* ---------------------------------------------------------------------------------- */


void FingerProtocol::getProgramPath()
{
  //kdDebug() << "kfingerMainWindow::getProgramPath()" << endl;
  // Not to sure wether I'm using the right error number here. - schlpbch -  

  myPerlPath = new QString(KGlobal::dirs()->findExe("perl"));
  if (myPerlPath->isEmpty())
    {
      //kdDebug() << "Perl command not found" << endl; 	
      this->error(ERR_CANNOT_LAUNCH_PROCESS,
		  i18n("Could not find the Perl program on your system, please install.")); 
      exit(-1);
    } 
  else 
    {
      //kdDebug() << "Perl command found:" << *myPerlPath << endl; 
    }
  
  myFingerPath = new QString(KGlobal::dirs()->findExe("finger"));
  if ((myFingerPath->isEmpty()))
    {   
      //kdDebug() << "Finger command not found" << endl;
      this->error(ERR_CANNOT_LAUNCH_PROCESS, 
		  i18n("Could not find the Finger program on your system, please install."));
      exit(-1);
    }
  else
    {
      //kdDebug() << "Finger command found:" << *myFingerPath << endl; 
    }
  
  myFingerPerlScript = new QString(locate("data","kio_finger/kio_finger.pl"));
  if (myFingerPerlScript->isEmpty())
    {
      //kdDebug() << "kio_finger.pl script not found" << endl;     
      this->error(ERR_CANNOT_LAUNCH_PROCESS,
		  i18n("kio_finger Perl script not found."));
      exit(-1);
    }
  else
    {
      //kdDebug() << "kio_finger perl script found: " << *myFingerPerlScript << endl;  
    } 
  
  myFingerCSSFile = new QString(locate("data","kio_finger/kio_finger.css"));
  if (myFingerCSSFile->isEmpty())
    {
      //kdDebug() << "kio_finger.css file not found" << endl;     
      this->warning(i18n("kio_finger CSS script not found. Output will look ugly."));
    }
  else
    {
      //kdDebug() << "kio_finger CSS file found: " << *myFingerCSSFile << endl;  
    }
}


/* --------------------------------------------------------------------------- */


void FingerProtocol::parseCommandLine(const KURL& url) 
{
  myURL = new KURL(url); 
  
  /*
   * Generate a valid finger url
   */

  if(myURL->isEmpty() || myURL->isMalformed() || 
     (myURL->user().isEmpty() && myURL->host().isEmpty())) 
    {
      myURL->setProtocol("finger");
      myURL->setUser("");
      myURL->setHost("localhost");
    }
  
  /*
   * If no specifc port is specified, set it to 79.
   */

  if(myURL->port() == 0) {
    myURL->setPort(79);
  } 

  /*
   * If no refresh rate is given, set it to defaultRefreshRate
   */

  if (myURL->query().isEmpty()) {
    myURL->setQuery("?refreshRate="+defaultRefreshRate);
  }
}

/* ---------------------------------------------------------------------------------- */
#include "kio_finger.moc"
/* ---------------------------------------------------------------------------------- */

