/////////////////////////////////////////////////////////////////////////////
//                                                                         
// Project:     SMB kioslave for KDE2
//
// File:        Top level implementation file for kio_smb.cpp
//                                                                         
// Abstract:    member function implementations for SMBSlave
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
SMBSlave::SMBSlave(const QCString& pool, const QCString& app)
: SlaveBase( "smb", pool, app ) 
{
    m_initialized_smbc = false;

    //read in the default workgroup info...
    reparseConfiguration();

    //initialize the library...
    auth_initialize_smbc();
}


//===========================================================================
SMBSlave::~SMBSlave()
{
}


//===========================================================================
// pointer to the slave created in kdemain
SMBSlave* G_TheSlave;

//===========================================================================
int kdemain( int argc, char **argv )
{
    kdDebug(KIO_SMB) << "START: kdemain for kio_smb" << endl;
    
    KInstance instance( "kio_smb" );
    if( argc != 4 )
    {
        kdDebug(KIO_SMB) << "Usage: kio_smb protocol domain-socket1 domain-socket2"
                  << endl;
        return -1;
    }
 
    SMBSlave slave( argv[2], argv[3] );

    G_TheSlave = &slave;
    slave.dispatchLoop();
    
    return 0;
} 




