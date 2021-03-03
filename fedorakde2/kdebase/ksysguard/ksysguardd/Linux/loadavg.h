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

	$Id: loadavg.h,v 1.2 2001/04/22 13:41:33 cschlaeg Exp $
*/

#ifndef _loadavg_h_
#define _loadavg_h_

void initLoadAvg(void);
void exitLoadAvg(void);
int updateLoadAvg(void);

void printLoadAvg1(const char*);
void printLoadAvg1Info(const char*);
void printLoadAvg5(const char*);
void printLoadAvg5Info(const char*);
void printLoadAvg15(const char*);
void printLoadAvg15Info(const char*);

#endif
