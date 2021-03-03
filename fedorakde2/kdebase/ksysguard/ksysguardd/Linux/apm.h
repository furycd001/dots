/*
    KSysGuard, the KDE System Guard
   
	Copyright (c) 1999 - 2000 Chris Schlaeger <cs@kde.org>
    
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

	$Id: apm.h,v 1.2 2001/04/22 13:41:33 cschlaeg Exp $
*/

#ifndef _apm_h_
#define _apm_h_

void initApm(void);
void exitApm(void);
int updateApm(void);

void printApmBatFill(const char*);
void printApmBatFillInfo(const char*);
void printApmBatTime(const char*);
void printApmBatTimeInfo(const char*);

#endif
