/////////////////////////////////////////////////////////////////////////////
//                                                                         
// Project:     SMB kioslave for KDE2
//
// File:        kio_smb_browse.cpp
//                                                                         
// Abstract:    member function implementations for SMBSlave that deal with 
//              SMB browsing
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

using namespace KIO;

//---------------------------------------------------------------------------
bool SMBSlave::browse_stat_path(const SMBUrl& url, UDSEntry& udsentry)
// Returns: true on success, false on failure 
{
    struct stat st;
    UDSAtom     udsatom;

    memset(&st,0,sizeof(st));
    if(cache_stat(url, &st) == 0)
    {
        if(S_ISDIR(st.st_mode))
        {
            // Directory
            udsatom.m_uds  = KIO::UDS_FILE_TYPE;
            udsatom.m_long = S_IFDIR;
            udsentry.append(udsatom);
        }
        else if(S_ISREG(st.st_mode))
        {
            // Regular file
            udsatom.m_uds  = KIO::UDS_FILE_TYPE;
            udsatom.m_long = S_IFREG;
            udsentry.append(udsatom);
        }
        else
        {
            SlaveBase::error(ERR_INTERNAL, TEXT_UNKNOWN_ERROR);
            return false;
        }

        udsatom.m_uds  = KIO::UDS_SIZE;
        udsatom.m_long = st.st_size;
        udsentry.append(udsatom);
    
        udsatom.m_uds  = KIO::UDS_USER;
        udsatom.m_str = st.st_uid;
        udsentry.append(udsatom);
    
        udsatom.m_uds  = KIO::UDS_GROUP;
        udsatom.m_str = st.st_gid;
        udsentry.append(udsatom);
    
        udsatom.m_uds  = KIO::UDS_ACCESS;
        udsatom.m_long = st.st_mode;
        udsentry.append(udsatom); 
    
        udsatom.m_uds  = UDS_MODIFICATION_TIME;
        udsatom.m_long = st.st_mtime;
        udsentry.append(udsatom);
    
        udsatom.m_uds  = UDS_ACCESS_TIME;
        udsatom.m_long = st.st_atime;
        udsentry.append(udsatom);
    
        udsatom.m_uds  = UDS_CREATION_TIME;
        udsatom.m_long = st.st_ctime;
        udsentry.append(udsatom);
    }
    else
    {
        // TODO: Authentication needed
        switch(errno)
        {
        case ENOENT:
        case ENOTDIR:
        case EFAULT:
            SlaveBase::error(ERR_DOES_NOT_EXIST, url.toKioUrl());
            break;
        case EPERM:
        case EACCES:
            SlaveBase::error(ERR_ACCESS_DENIED, url.toKioUrl());
            cache_clear_AuthInfo(m_current_workgroup);
            break;
        case ENOMEM:
            SlaveBase::error(ERR_OUT_OF_MEMORY, TEXT_OUT_OF_MEMORY);
        default:
            SlaveBase::error(ERR_INTERNAL, TEXT_UNKNOWN_ERROR);
        }
        return false;
    }

    return true;
}


//===========================================================================
// TODO: Add stat cache
void SMBSlave::stat( const KURL& kurl )
{
    kdDebug(KIO_SMB) << "SMBSlave::stat on " << kurl.url() << endl;
    m_current_url.fromKioUrl( kurl );

    UDSAtom     udsatom;
    UDSEntry    udsentry; 

    switch(m_current_url.getType())
    {
    case SMBURLTYPE_UNKNOWN:
        SlaveBase::error(ERR_MALFORMED_URL,m_current_url.toKioUrl());
        break;

    case SMBURLTYPE_ENTIRE_NETWORK:
    case SMBURLTYPE_WORKGROUP_OR_SERVER:
        udsatom.m_uds = KIO::UDS_FILE_TYPE;
        udsatom.m_long = S_IFDIR;
        udsentry.append(udsatom);
        break;

    case SMBURLTYPE_SHARE_OR_PATH:
        browse_stat_path(m_current_url, udsentry);
        break;

    default:
        kdDebug(KIO_SMB) << "weird value in stat" << endl;
        break;
    }

    SlaveBase::statEntry(udsentry);
    SlaveBase::finished();

    kdDebug(KIO_SMB) << "SMBSlave::stat on " << kurl.url()
                     << " is returning" << endl;
}

//===========================================================================
// TODO: Add dir cache
void SMBSlave::listDir( const KURL& kurl )
{
    kdDebug(KIO_SMB) << "SMBSlave::listDir on " << kurl.url() << endl;
    m_current_url.fromKioUrl( kurl );
    int                 dirfd;
    struct smbc_dirent  *dirp = NULL;

    UDSEntry    udsentry;
    UDSAtom     atom;

    // Special case if url contains a workgroup
    if(m_current_url.getType() == SMBURLTYPE_SHARE_OR_PATH)
    {
        QString workgroup = m_current_url.getWorkgroup();
        if(cache_check_workgroup(workgroup))
        {
            m_current_workgroup = workgroup;

            KURL redirurl;
            redirurl.setProtocol("smb");
            redirurl.setPath(m_current_url.getServerShareDir());

            //This causes problems in the treeview in konqueror...
            redirection(redirurl);
            SlaveBase::finished();
            return;
        }
    }

    dirfd = smbc_opendir( m_current_url.toSmbcUrl() );
    if(dirfd >= 0)
    {
        while(1)
        {
            dirp = smbc_readdir(dirfd);
            if(dirp == NULL)
            {
                break;
            }

            if(dirp->smbc_type == SMBC_FILE)
            {
                // Set type
                atom.m_uds = KIO::UDS_FILE_TYPE;
                atom.m_long = S_IFREG;
                udsentry.append( atom );

                // Set stat information
                m_current_url.append(dirp->name);
                browse_stat_path(m_current_url, udsentry);
                m_current_url.truncate();
            }
            else if(dirp->smbc_type == SMBC_DIR)
            {
                // Set type
                atom.m_uds = KIO::UDS_FILE_TYPE;
                atom.m_long = S_IFDIR;
                udsentry.append( atom );
                
                // Set stat information
                if(strcmp(dirp->name,".") &&
                   strcmp(dirp->name,".."))
                {
                    m_current_url.append(dirp->name);
                    browse_stat_path(m_current_url, udsentry);
                    m_current_url.truncate();
                }
            }
            else if(dirp->smbc_type == SMBC_SERVER ||
                    dirp->smbc_type == SMBC_FILE_SHARE)
            {
                // Set type
                atom.m_uds = KIO::UDS_FILE_TYPE;
                atom.m_long = S_IFDIR;
                udsentry.append( atom );

                // Set permissions
                atom.m_uds  = KIO::UDS_ACCESS;
                atom.m_long = (S_IRUSR | S_IRGRP | S_IROTH | S_IXUSR | S_IXGRP | S_IXOTH);
                udsentry.append(atom); 
            }
            else if(dirp->smbc_type == SMBC_WORKGROUP)
            {
                // Set type
                atom.m_uds = KIO::UDS_FILE_TYPE;
                atom.m_long = S_IFDIR;
                udsentry.append( atom );

                // Set permissions
                atom.m_uds  = KIO::UDS_ACCESS;
                atom.m_long = (S_IRUSR | S_IRGRP | S_IROTH | S_IXUSR | S_IXGRP | S_IXOTH);
                udsentry.append(atom); 

                // remember the workgroup
                cache_add_workgroup(dirp->name);
            }
            else
            {
                // TODO: we don't handle SMBC_IPC_SHARE, SMBC_PRINTER_SHARE
                //       SMBC_LINK, SMBC_COMMS_SHARE
                //SlaveBase::error(ERR_INTERNAL, TEXT_UNSUPPORTED_FILE_TYPE);
                continue;
            }

            // Set name
            atom.m_uds = KIO::UDS_NAME;
            atom.m_str = dirp->name;
            udsentry.append( atom );

            // Call base class to list entry
            SlaveBase::listEntry(udsentry, false);
            udsentry.clear();
        }

        // clean up
        smbc_closedir(dirfd);
    }
    else
    {
        kdDebug(KIO_SMB) << "there was error, error = " << strerror( errno )
                         << endl;

        switch(errno)
        {
        case ENOENT:
        case ENOTDIR:
        case EFAULT:
            SlaveBase::error(ERR_DOES_NOT_EXIST, m_current_url.toKioUrl());
            break;
        case EPERM:
        case EACCES:
            SlaveBase::error(ERR_ACCESS_DENIED, m_current_url.toKioUrl());
            cache_clear_AuthInfo(m_current_workgroup);
            break;
        case ENOMEM:
            SlaveBase::error(ERR_OUT_OF_MEMORY, TEXT_OUT_OF_MEMORY);
            break;
        case EUCLEAN:
            SlaveBase::error(ERR_INTERNAL, TEXT_SMBC_INIT_FAILED);
            break;
        case ENODEV:
            SlaveBase::error(ERR_INTERNAL, TEXT_NOSRV_WG);
            break;
        default:
            SlaveBase::error(ERR_INTERNAL, TEXT_UNKNOWN_ERROR);
        }
    }

    SlaveBase::listEntry(udsentry, true); 
    SlaveBase::finished();
}
