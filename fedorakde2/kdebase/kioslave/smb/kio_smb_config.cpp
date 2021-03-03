/////////////////////////////////////////////////////////////////////////////
//                                                                         
// Project:     SMB kioslave for KDE2
//
// File:        kio_smb_config.cpp
//                                                                         
// Abstract:    member function implementations for SMBSlave that deal with 
//              KDE/SMB slave configuration
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
#include <kconfig.h>
//===========================================================================
void SMBSlave::reparseConfiguration()
{
    KConfig cfg( "kioslaverc" );
    cfg.setGroup( "Browser Settings/SMBro" );
    m_default_workgroup = cfg.readEntry( "Workgroup", "WORKGROUP" ).local8Bit();

    if( m_default_workgroup.isEmpty() )
        m_default_workgroup = "WORKGROUP";

    kdDebug(KIO_SMB) << "reparseConfiguration, m_default_workgroup = " << m_default_workgroup << endl;
}
