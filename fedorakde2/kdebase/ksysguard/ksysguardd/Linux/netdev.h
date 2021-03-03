/*
    KSysGuard, the KDE System Guard
   
	Copyright (c) 1999, 2000 Chris Schlaeger <cs@kde.org>
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General Public
    License as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

	$Id: netdev.h,v 1.3 2001/04/22 13:41:33 cschlaeg Exp $
*/

#ifndef _netdev_h_
#define _netdev_h_

void initNetDev(void);

void exitNetDev(void);

int updateNetDev(void);

void checkNetDev(void);

void printNetDevRecBytes(const char* cmd);
void printNetDevRecBytesInfo(const char* cmd);
void printNetDevSentBytes(const char* cmd);
void printNetDevSentBytesInfo(const char* cmd);

#endif
