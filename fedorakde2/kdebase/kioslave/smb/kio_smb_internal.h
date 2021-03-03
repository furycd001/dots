/////////////////////////////////////////////////////////////////////////////
//                                                                         
// Project:     SMB kioslave for KDE2
//
// File:        kio_smb_internal.h  
//                                                                         
// Abstract:    Utility classes used by SMBSlave
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

#ifndef KIO_SMB_INTERNAL_H_INCLUDED
#define KIO_SMB_INTERNAL_H_INCLUDED

#include <kio/authinfo.h>

//===========================================================================
typedef enum _SMBUrlType
//===========================================================================
{
    SMBURLTYPE_UNKNOWN             = 0,
    SMBURLTYPE_ENTIRE_NETWORK      = 1,
    SMBURLTYPE_WORKGROUP_OR_SERVER = 2,
    SMBURLTYPE_SHARE_OR_PATH       = 3
}SMBUrlType;



//===========================================================================
class SMBUrl
//===========================================================================
{
    SMBUrlType m_type;
    QString    m_kio_url;
    QString    m_smbc_url;

    int m_workgroup_index;
    int m_workgroup_len;

public:
    //-----------------------------------------------------------------------
    SMBUrl();
    //-----------------------------------------------------------------------
    
    //-----------------------------------------------------------------------
    SMBUrl(const KURL& kurl);
    //-----------------------------------------------------------------------
    

    //-----------------------------------------------------------------------
    SMBUrl& append(const char* filedir);
    // Appends the specified file and dir to this SMBUrl
    // "smb://server/share" --> "smb://server/share/filedir"
    //-----------------------------------------------------------------------


    //-----------------------------------------------------------------------
    SMBUrlType getType();
    // Returns the type of this SMBUrl:
    //   SMBURLTYPE_UNKNOWN  - Type could not be determined. Bad SMB Url.
    //   SMBURLTYPE_ENTIRE_NETWORK - "smb:/" is entire network
    //   SMBURLTYPE_WORKGROUP_OR_SERVER - "smb:/mygroup" or "smb:/myserver"
    //   URLTYPE_SHARE_OR_PATH - "smb:/mygroupe/mymachine/myshare/mydir"
    //-----------------------------------------------------------------------


    //-----------------------------------------------------------------------
    QString getWorkgroup() const;
    //-----------------------------------------------------------------------


    //-----------------------------------------------------------------------
    QString getServerShareDir() const;
    //-----------------------------------------------------------------------


    //-----------------------------------------------------------------------
    void fromKioUrl(const KURL& kurl);
    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------   
    QCString toSmbcUrl() const;
    // Return a URL that is suitable for libsmbclient
    //----------------------------------------------------------------------- 

    //----------------------------------------------------------------------- 
    const QString& toKioUrl() const;
    // Return a that is suitable for kio framework
    //----------------------------------------------------------------------- 

    //-----------------------------------------------------------------------
    void truncate();
    // Truncates one file/dir level
    // "smb://server/share/filedir" --> "smb://server/share"
    //-----------------------------------------------------------------------

};

//===========================================================================
struct SMBAuthInfo
//===========================================================================
{
    QCString m_workgroup;
    QCString m_server;
    QCString m_share;
    QCString m_username;
    QCString m_passwd;
};


#endif

