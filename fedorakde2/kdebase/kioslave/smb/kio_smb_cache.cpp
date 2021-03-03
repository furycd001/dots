/////////////////////////////////////////////////////////////////////////////
//
// Project:     SMB kioslave for KDE2
//
// File:        kio_smb_cache.cpp
//
// Abstract:    member function implementations for SMBSlave that deal with
//              our internal cache and the password caching daemon
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

//===========================================================================
// SMBSlave authentication and cache function implementations
//===========================================================================
bool SMBSlave::cache_get_AuthInfo(SMBAuthInfo& auth)
{
    kdDebug(KIO_SMB) << "SMBSlave::cache_get_AuthInfo for: " << auth.m_workgroup << "|" << auth.m_server << "|" << auth.m_share << endl;

    //first search our current cache...
    for( SMBAuthInfo* it = m_auth_cache.first();
         it; it = m_auth_cache.next() )
    {
        if( (it->m_server    == auth.m_server) &&
            (it->m_share     == auth.m_share)  &&
            (it->m_workgroup == auth.m_workgroup) )
        {
            kdDebug(KIO_SMB) << "found in top level cache" << endl;
            auth.m_username = it->m_username;
            auth.m_passwd   = it->m_passwd;
            return true;
        }
    }

    //now check the password caching daemon as the last resort
    //if it is there, put it in our cache
    AuthInfo kauth = cache_create_AuthInfo( auth );
    if( checkCachedAuthentication( kauth ) )
    {
        kdDebug(KIO_SMB) << "found in password caching daemon" << endl;
        auth.m_username = kauth.username.local8Bit();
        auth.m_passwd = kauth.password.local8Bit();

        //store the info for later lookups
        cache_set_AuthInfo( auth );
        return true;
    }

    kdDebug(KIO_SMB) << "auth not cached at all..." << endl;
    return false;
}

void SMBSlave::cache_clear_AuthInfo(const QString& workgroup)
{
    SMBAuthInfo* it = m_auth_cache.first();
    while( it )
    {
        if( it->m_workgroup == workgroup.local8Bit() )
        {
            m_auth_cache.remove();
            //now clear it in the password caching daemon as well- but how??
            //the method was deprecated, but Dawit said he would reimplement it
            //delCachedAuthentication( cache_create_AuthInfo(*it) );
            it = m_auth_cache.current();
        }
        else
            it = m_auth_cache.next();
    }
}

void SMBSlave::cache_set_AuthInfo(const SMBAuthInfo& _auth,
                                  bool store_in_kdesu)
{
    SMBAuthInfo* auth = new SMBAuthInfo;
    auth->m_passwd    = _auth.m_passwd;
    auth->m_server    = _auth.m_server;
    auth->m_share     = _auth.m_share;
    auth->m_username  = _auth.m_username;
    auth->m_workgroup = _auth.m_workgroup;

    m_auth_cache.prepend( auth );

    if( store_in_kdesu )
    {
        AuthInfo kauth = cache_create_AuthInfo( *auth );
        cacheAuthentication( kauth );
    }
}

void SMBSlave::cache_add_workgroup( const QString& workgroup)
{
    if( !m_workgroup_cache.contains(workgroup) )
    {
        m_workgroup_cache.prepend(workgroup);
    }
}

bool SMBSlave::cache_check_workgroup(const QString& workgroup)
{

    return m_workgroup_cache.contains(workgroup);

}

int SMBSlave::cache_stat(const SMBUrl& url, struct stat* st)
{
    int result;
    result = smbc_stat(url.toSmbcUrl(), st);
    return result;
}

AuthInfo SMBSlave::cache_create_AuthInfo( const SMBAuthInfo& auth )
{
    AuthInfo rval;
    rval.url.setProtocol( "smb" );

    if( auth.m_server.isEmpty() )
    {
        rval.url.setPath( "/" + auth.m_workgroup );
    }
    else
    {
        rval.url.setPath( "/" + auth.m_server + "/" + auth.m_share );
    }

    rval.username = auth.m_username;
    rval.password = auth.m_passwd;

    kdDebug(KIO_SMB) << "cache_create_AuthInfo, url = " << rval.url.url() << endl;
    return rval;
}


