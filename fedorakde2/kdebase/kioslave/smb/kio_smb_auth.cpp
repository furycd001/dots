/////////////////////////////////////////////////////////////////////////////
//                                                                         
// Project:     SMB kioslave for KDE2
//
// File:        kio_smb_auth.cpp
//                                                                         
// Abstract:    member function implementations for SMBSlave that deal with 
//              SMB directory access
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

#include <qdir.h>
#include <stdlib.h>


//==========================================================================
void auth_smbc_get_data(const char *server,const char *share,
                        char *workgroup, int wgmaxlen,
                        char *username, int unmaxlen,
                        char *password, int pwmaxlen)
//==========================================================================
{
    G_TheSlave->auth_smbc_get_data(server, share,
                                   workgroup,wgmaxlen,
                                   username, unmaxlen,
                                   password, pwmaxlen);
}

//--------------------------------------------------------------------------
void SMBSlave::auth_smbc_get_data(const char *server,const char *share,
                                  char *workgroup, int wgmaxlen,
                                  char *username, int unmaxlen,
                                  char *password, int pwmaxlen)
//--------------------------------------------------------------------------
{
    kdDebug(KIO_SMB) << "auth_smbc_get_data, server = " << server <<
                        ", share = " << share << ", workgroup = " << workgroup << endl;

    //check this to see if we "really" need to authenticate...
    SMBUrlType t = m_current_url.getType();
    if( t == SMBURLTYPE_ENTIRE_NETWORK )
    {
        kdDebug(KIO_SMB) << "we don't really need to authenticate for this top level url, returning" << endl;
        return;
    }

    SMBAuthInfo auth;
    QString  user_prompt;
    QString  passwd_prompt;

    if(m_current_workgroup.length() == 0)
    {
        m_current_workgroup = workgroup;
    }

    auth.m_workgroup = m_current_workgroup.local8Bit();
    auth.m_server    = server;
    auth.m_share     = share;

    bool copyinfo = false;
    if( cache_get_AuthInfo(auth) == false)
    {
        user_prompt = username;

        QString msg( "Please enter Authentication information for:\n" );
        msg.append( "Workgroup = " + auth.m_workgroup + "\n" );
        msg.append( "Server = " + auth.m_server + "\n" );
        msg.append( "Share = " + auth.m_share + "\n" );

        if(openPassDlg(msg, user_prompt, passwd_prompt))
        {
            auth.m_username = user_prompt.local8Bit();
            auth.m_passwd = passwd_prompt.local8Bit();
            cache_set_AuthInfo(auth, true);

            // Hand the data back to libsmbclient if the user didn't cancel
            copyinfo = true;
        }
    }
    else
    {
        copyinfo = true;
    }

    if( copyinfo )
    {
        // Hand the data back to libsmbclient if it's been cached
        memset(workgroup,0,wgmaxlen);
        memset(username,0,unmaxlen);
        memset(password,0,pwmaxlen);
        strncpy(workgroup,auth.m_workgroup,wgmaxlen - 1);
        strncpy(username,auth.m_username,unmaxlen - 1);
        strncpy(password,auth.m_passwd,pwmaxlen - 1);
    }
}

//--------------------------------------------------------------------------
int SMBSlave::auth_initialize_smbc()
// Initalizes the smbclient library
// 
// Returns: 0 on success -1 with errno set on error
//--------------------------------------------------------------------------
{
    if(m_initialized_smbc == false)
    {
        //check for $HOME/.smb/smb.conf, the library dies without it...
        //create it with a sane default if it's not there
        bool mksmbdir = false, mksmbconf = false;
        QDir dir = QDir::home();

        if( dir.cd( ".smb" ) )
        {
            if( !dir.exists( "smb.conf" ) )
            {
                kdDebug(KIO_SMB) << "need to create the smb.conf file" << endl;
                mksmbconf = true;
            }
        }
        else
        {
            kdDebug(KIO_SMB) << "need to create the .smb dir and the smb.conf file" << endl;
            mksmbdir = true;
            mksmbconf = true;
        }

        if( mksmbdir )
        {
            dir.mkdir( ".smb" );
            dir.cd( ".smb" );
        }

        if( mksmbconf )
        {
            //copy our default workgroup to the smb.conf file
            QFile conf( dir.absPath() + "/smb.conf" );
            if( conf.open( IO_WriteOnly ) )
            {
                QTextStream output( &conf );
                output << "[global]" << endl;
                output << "\tworkgroup = " << m_default_workgroup << endl;
                conf.close();
            }
            else
            {
                SlaveBase::error(ERR_INTERNAL,
                    i18n(
"You are missing your $HOME/.smb/smb.conf file, and we could not create it.\n\
Please manually create it to enable the smb ioslave to operate correctly.\n\
The smb.conf file could look like: \
[global]\
    workgroup= <YOUR_DEFAULT_WORKGROUP>"));
                return -1;
            }
        }

        if(smbc_init(::auth_smbc_get_data,0) == -1)
        {
            SlaveBase::error(ERR_INTERNAL, TEXT_SMBC_INIT_FAILED);
            return -1;
        }

        m_initialized_smbc = true;
    }

    return 0;
}


