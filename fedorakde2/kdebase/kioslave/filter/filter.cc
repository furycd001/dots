/*
This file is part of KDE

 Copyright (C) 1999-2000 Waldo Bastian (bastian@kde.org)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
// $Id: filter.cc,v 1.5 2001/03/31 13:16:51 matz Exp $

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <kinstance.h>
#include <kdebug.h>
#include <kmimemagic.h>
#include <kfilterbase.h>

#include "filter.h"

extern "C" { int kdemain(int argc, char **argv); }

int kdemain( int argc, char ** argv)
{
  KInstance instance( "kio_filter" );

  kdDebug(7110) << "Starting " << getpid() << endl;

  if (argc != 4)
  {
     fprintf(stderr, "Usage: kio_filter protocol domain-socket1 domain-socket2\n");
     exit(-1);
  }

  FilterProtocol slave(argv[1], argv[2], argv[3]);
  slave.dispatchLoop();

  kdDebug(7110) << "Done" << endl;
  return 0;
}

FilterProtocol::FilterProtocol( const QCString & protocol, const QCString &pool, const QCString &app )
 : KIO::SlaveBase( protocol, pool, app )
{
    QString mimetype = QString::fromLatin1("application/x-") + QString::fromLatin1(protocol);
    filter = KFilterBase::findFilterByMimeType( mimetype );
    ASSERT(filter);
}

void FilterProtocol::get( const KURL & )
{
  if (subURL.isEmpty())
  {
     error( KIO::ERR_NO_SOURCE_PROTOCOL, mProtocol );
     return;
  }
  if (!filter)
  {
      error( KIO::ERR_INTERNAL, mProtocol );
      return;
  }
  needSubURLData();

  filter->init(IO_ReadOnly);

  bool bNeedHeader = true;
  bool bNeedMimetype = true;
  bool bError = true;
  int result;

  QByteArray inputBuffer;
  QByteArray outputBuffer(8*1024); // Start with a modest buffer
  filter->setOutBuffer( outputBuffer.data(), outputBuffer.size() );
  while(true)
  {
     if (filter->inBufferEmpty())
     {
        dataReq(); // Request data
        result = readData( inputBuffer);
  kdDebug(7110) << "requestData: got " << result << endl;
        if (result <= 0)
        {
          bError = true;
          break; // Unexpected EOF.
        }
        filter->setInBuffer( inputBuffer.data(), inputBuffer.size() );
     }
     if (bNeedHeader)
     {
        bError = !filter->readHeader();
        if (bError)
            break;
        bNeedHeader = false;
     }
     result = filter->uncompress();
     if ((filter->outBufferAvailable() == 0) || (result == KFilterBase::END))
     {
         kdDebug(7110) << "avail_out = " << filter->outBufferAvailable() << endl;
        if (filter->outBufferAvailable() != 0)
        {
            // Discard unused space :-)
            outputBuffer.resize(outputBuffer.size() - filter->outBufferAvailable());
        }
        if (bNeedMimetype)
        {
            KMimeMagicResult * result = KMimeMagic::self()->findBufferFileType( outputBuffer, subURL.fileName() );
            kdDebug(7110) << "Emitting mimetype " << result->mimeType() << endl;
            mimeType( result->mimeType() );
            bNeedMimetype = false;
        }
        data( outputBuffer ); // Send data
        filter->setOutBuffer( outputBuffer.data(), outputBuffer.size() );
        if (result == KFilterBase::END)
           break; // Finished.
     }
     if (result != KFilterBase::OK)
     {
        bError = true;
        break; // Error
     }
  }

  if (!bError)
  {
     dataReq(); // Request data
     result = readData( inputBuffer);
  kdDebug(7110) << "requestData: got " << result << "(expecting 0)" << endl;
     data( QByteArray() ); // Send EOF
  }

  filter->terminate();

  if (bError)
  {
     error(KIO::ERR_COULD_NOT_READ, subURL.url());
     subURL = KURL(); // Clear subURL
     return;
  }

  subURL = KURL(); // Clear subURL
  finished();
}

void FilterProtocol::put( const KURL &/*url*/, int, bool /*_overwrite*/, bool /*_resume*/ )
{
  error( KIO::ERR_UNSUPPORTED_ACTION, QString::fromLatin1("put"));
}

void FilterProtocol::setSubURL(const KURL &url)
{
   subURL = url;
}

