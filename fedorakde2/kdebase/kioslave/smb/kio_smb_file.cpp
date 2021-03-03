/////////////////////////////////////////////////////////////////////////////
//                                                                         
// Project:     SMB kioslave for KDE2
//
// File:        kio_smb_file.cpp
//                                                                         
// Abstract:    member function implementations for SMBSlave that deal with 
//              SMB file access
//
// Author(s):   Matthew Peterson <mpeterson@caldera.com>
//
//---------------------------------------------------------------------------
//                                                                  
// Copyright (c) 2000  Caldera Systems, Inc.                        
//                                                                         
// This program is free software; you can redistribute it and/or modify it 
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2.1 of the License, or  
// (at your option) any later version.                                     
//                                                                         
//     This program is distributed in the hope that it will be useful,     
//     but WITHOUT ANY WARRANTY; without even the implied warranty of      
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       
//     GNU Lesser General Public License for more details.                 
//                                                                         
//     You should have received a copy of the GNU General Public License 
//     along with this program; see the file COPYING.  If not, please obtain
//     a copy from http://www.gnu.org/copyleft/gpl.html   
//                                                                         
/////////////////////////////////////////////////////////////////////////////


#include "kio_smb.h"
#include "kio_smb_internal.h"

//===========================================================================
void SMBSlave::get( const KURL& kurl )
{
    kdDebug(KIO_SMB) << "SMBSlave::get on " << kurl.url() << endl;

    int         filefd          = 0;
    ssize_t     bytesread       = 0;
    char*       buf             = 0;
    time_t      curtime         = 0;
    time_t      lasttime        = 0;
    time_t      starttime       = 0;
    ssize_t     totalbytesread  = 0;
    struct stat st;
    QByteArray  filedata;
    SMBUrl      url;
    
    
    if(auth_initialize_smbc() == -1)
    {
        return;
    }

    // Stat
    url.fromKioUrl(kurl);
    if(cache_stat(url,&st) == -1 )
    {
        if ( errno == EACCES )
           error( KIO::ERR_ACCESS_DENIED, url.toKioUrl());
        else
           error( KIO::ERR_DOES_NOT_EXIST, url.toKioUrl());
        return;
    }
    if ( S_ISDIR( st.st_mode ) ) {
        error( KIO::ERR_IS_DIRECTORY, url.toKioUrl());
        return;
    }

    // Set the total size
    totalSize( st.st_size ); 

    // Open and read the file
    filefd = smbc_open(url.toSmbcUrl(),O_RDONLY,0);
    if(filefd >= 0)
    {
        buf = (char*)malloc(MAX_XFER_BUF_SIZE);
        if(buf)
        {
            lasttime = starttime = time(NULL);
            while(1)
            {
                bytesread = smbc_read(filefd, buf, MAX_XFER_BUF_SIZE);
                if(bytesread == 0)
                {
                    // All done reading
                    break; 
                }
                else if(bytesread < 0)
                {
                    error( KIO::ERR_COULD_NOT_READ, url.toKioUrl());
                    break;
                }
    
                filedata.setRawData(buf,bytesread);
                data( filedata );
                filedata.resetRawData(buf,bytesread);
     
                // increment total bytes read
                totalbytesread += bytesread; 
    
                // display progress at least every second
                curtime = time(NULL);
                if ( curtime - lasttime > 0 )
                {
                    processedSize(totalbytesread);
                    speed(totalbytesread/(curtime-starttime));
                    lasttime = curtime;
                }
            }
    
            free(buf);
        }

        smbc_close(filefd);
    }
    else
    {    
          error( KIO::ERR_CANNOT_OPEN_FOR_READING, url.toKioUrl());
    }

    finished();
}


//===========================================================================
void SMBSlave::mimetype( const KURL& /*url*/ )
{
    error(KIO::ERR_UNSUPPORTED_ACTION, "SMB filesystem does not yet support mimetype()");
    finished();
}



//===========================================================================
void SMBSlave::put( const KURL& kurl,
                    int permissions, 
                    bool overwrite, 
                    bool resume )
{
    kdDebug(KIO_SMB) << "SMBSlave::put on " << kurl.url() << endl;
    m_current_url.fromKioUrl( kurl );

    int         filefd;
    bool        exists;
    mode_t      mode;
    struct stat st;
    QByteArray  filedata;

    exists = (cache_stat(m_current_url, &st) != -1 );
    if ( exists &&  !overwrite && !resume)
    {
        if (S_ISDIR(st.st_mode))
        {
            error( KIO::ERR_DIR_ALREADY_EXIST,  m_current_url.toKioUrl());
        }   
        else
        {
            error( KIO::ERR_FILE_ALREADY_EXIST, m_current_url.toKioUrl());
        }
        return;
    }
 
    if (exists && !resume)
    {
         remove(m_current_url.toKioUrl().local8Bit());
    }

    
    if (resume) 
    {
        // append if resuming
        filefd = smbc_open(m_current_url.toSmbcUrl(), O_RDWR, 0 );
        smbc_lseek(filefd, 0, SEEK_END);
    } 
    else 
    {
        if (permissions != -1)
        {
            mode = permissions | S_IWUSR | S_IRUSR;
        }
        else
        {
            mode = 600;//0666;
        }
        filefd = smbc_open(m_current_url.toSmbcUrl(), O_CREAT | O_TRUNC | O_WRONLY, mode);
    }
 
    if ( filefd < 0 ) 
    {
        if ( errno == EACCES ) 
        {
            error( KIO::ERR_WRITE_ACCESS_DENIED, m_current_url.toKioUrl());
        }
        else 
        {
            error( KIO::ERR_CANNOT_OPEN_FOR_WRITING, m_current_url.toKioUrl());
        }
        return;
    }
 
    // Loop until we got 0 (end of data)
    while(1)
    {
        dataReq(); // Request for data
        if (readData(filedata) <= 0)
        {
            break;
        }
        if(smbc_write(filefd, filedata.data(), filedata.size()) < 0)
        {
            error( KIO::ERR_COULD_NOT_WRITE, m_current_url.toKioUrl());
            break;
        }
    }

    if(smbc_close(filefd))
    {
        error( KIO::ERR_COULD_NOT_WRITE, m_current_url.toKioUrl());
        return;
    }
 
    // set final permissions, if the file was just created
    if ( permissions != -1 && !exists )
    {
        // TODO: did the smbc_chmod fail?
        // TODO: put in call to chmod when it is working!
        // smbc_chmod(url.toSmbcUrl(),permissions);
    }
 
    // We have done our job => finish
    finished();
}               


//===========================================================================
// TODO: do we need this?
void SMBSlave::setSubURL(const KURL&/*url*/)
{
    error(KIO::ERR_UNSUPPORTED_ACTION, "SMB filesystem does not yet support setSubURL()");
    finished();
}


