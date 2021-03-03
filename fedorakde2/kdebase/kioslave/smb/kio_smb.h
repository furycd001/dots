/////////////////////////////////////////////////////////////////////////////
//                                                                         
// Project:     SMB kioslave for KDE2
//
// File:        kio_smb.h  
//                                                                         
// Abstract:    The main kio slave class declaration.  For convenience, 
//              in concurrent devlopment, the implementation for this class 
//              is separated into several .cpp files -- the file containing
//              the implementation should be noted in the comments for each
//              member function.
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


#ifndef KIO_SMB_H_INCLUDED
#define KIO_SMB_H_INCLUDED

//-------------
// QT includes
//-------------
#include <qstring.h>
#include <qlist.h>
#include <qstringlist.h>
#include <qtextstream.h>
#include <qstrlist.h> 

//--------------
// KDE includes
//--------------
#include <kdebug.h>
#include <kinstance.h>
#include <kio/global.h>
#include <kio/slavebase.h>
#include <kurl.h>
#include <klocale.h>  

//-----------------------------
// Standard C library includes
//-----------------------------
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <stdio.h> 
#include <errno.h>
#include <time.h>


//-------------------------------
// Samba client librarh includes
//-------------------------------
extern "C" 
{
#include <libsmbclient.h>
}
                                 
//---------------------------
// kio_smb internal includes
//---------------------------
#include "kio_smb_internal.h"

#define MAX_XFER_BUF_SIZE           16348
#define TEXT_UNKNOWN_ERROR          i18n("Unknown error condition")
#define TEXT_SMBC_INIT_FAILED       i18n("libsmbclient filed to intialize")
#define TEXT_OUT_OF_MEMORY          i18n("Out of Memory")
#define TEXT_AUTHENTICATION_PROMPT  i18n("Enter workgroup/domain authentication information")
#define TEXT_NOSRV_WG               i18n("Server or Workgroup could not be found")
#define KIO_SMB                     7106


using namespace KIO; 

//===========================================================================
class SMBSlave : public SlaveBase
{
private:
    //---------------------------------------------------------------------
    // please make sure your private data does not duplicate existing data
    //---------------------------------------------------------------------
    bool     m_initialized_smbc;
    QString  m_current_workgroup;
    QString  m_default_workgroup;
    SMBUrl   m_current_url;

    QList<SMBAuthInfo> m_auth_cache;
    QStringList        m_workgroup_cache;


protected:
    //---------------------------------------------
    // Authentication functions (kio_smb_auth.cpp) 
    //---------------------------------------------
    // (please prefix functions with auth)
    int auth_initialize_smbc();

    //---------------------------------------------
    // Cache functions (kio_smb_auth.cpp)
    //---------------------------------------------
    // (please prefix functions with cache)
    void cache_add_workgroup( const QString& workgroup );
    bool cache_check_workgroup( const QString& workgroup );

    //Authentication methods
    bool cache_get_AuthInfo( SMBAuthInfo& auth );
    void cache_clear_AuthInfo( const QString& workgroup );
    void cache_set_AuthInfo( const SMBAuthInfo& auth, bool store_in_kdesu=false );

    //Stat methods
    int cache_stat( const SMBUrl& url, struct stat* st );

    //create a KIO::AuthInfo structure from the SMBAuthInfo struct
    AuthInfo cache_create_AuthInfo( const SMBAuthInfo& auth );

    
    //-----------------------------------------
    // Browsing functions (kio_smb_browse.cpp) 
    //-----------------------------------------
    // (please prefix functions with browse)
    bool browse_stat_path(const SMBUrl& url, UDSEntry& udsentry);
    
    //---------------------------------------------
    // Configuration functions (kio_smb_config.cpp) 
    //---------------------------------------------
    // (please prefix functions with config)
    

    //---------------------------------------
    // Directory functions (kio_smb_dir.cpp) 
    //---------------------------------------
    // (please prefix functions with dir)


    //--------------------------------------
    // File IO functions (kio_smb_file.cpp) 
    //--------------------------------------
    // (please prefix functions with file)

    //----------------------------
    // Misc functions (this file)
    //----------------------------
    

public:
    
    //-----------------------------------------------------------------------
    // smbclient authentication callback (note that this is called by  the
    // global ::auth_smbc_get_data() call.
    void auth_smbc_get_data(const char *server,const char *share,
                            char *workgroup, int wgmaxlen,
                            char *username, int unmaxlen,
                            char *password, int pwmaxlen);


    //-----------------------------------------------------------------------
    // Overwritten functions from the base class that define the operation of
    // this slave. (See the base class headerfile slavebase.h for more 
    // details)
    //-----------------------------------------------------------------------

    // Functions overwritten in kio_smb.cpp
    SMBSlave(const QCString& pool, const QCString& app);
    virtual ~SMBSlave();
    
    // Functions overwritten in kio_smb_browse.cpp
    virtual void listDir( const KURL& url );
    virtual void stat( const KURL& url );

    // Functions overwritten in kio_smb_config.cpp
    virtual void reparseConfiguration();

    // Functions overwritten in kio_smb_dir.cpp
    virtual void chmod( const KURL& kurl, int permissions );
    virtual void copy( const KURL& src, const KURL &dest, int permissions, bool overwrite );
    virtual void del( const KURL& kurl, bool isfile);
    virtual void mkdir( const KURL& kurl, int permissions );
    virtual void rename( const KURL& src, const KURL& dest, bool overwrite );
    virtual void symlink( const QString& target, const KURL& dest, bool overwrite );

    // Functions overwritten in kio_smb_file.cpp
    virtual void get( const KURL& kurl );
    virtual void mimetype( const KURL& url );
    virtual void put( const KURL& kurl, int permissions, bool overwrite, bool resume );
    virtual void setSubURL(const KURL& kurl);

    // Functions not implemented  (yet)
    //virtual void setHost(const QString& host, int port, const QString& user, const QString& pass);
    //virtual void openConnection();
    //virtual void closeConnection();
    //virtual void slave_status();
    //virtual void special( const QByteArray & );
};

//===========================================================================
// pointer to the slave created in kdemain
extern SMBSlave* G_TheSlave;


//==========================================================================
// the global libsmbclient authentication callback function
extern "C"
{

void auth_smbc_get_data(const char *server,const char *share,
                        char *workgroup, int wgmaxlen,
                        char *username, int unmaxlen,
                        char *password, int pwmaxlen);

}


//===========================================================================
// Main slave entrypoint (see kio_smb.cpp)
extern "C" 
{ 

int kdemain( int argc, char **argv ); 

}


#endif  //#endif KIO_SMB_H_INCLUDED
