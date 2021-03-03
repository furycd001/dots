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

	$Id: stat.h,v 1.5 2001/04/22 13:41:33 cschlaeg Exp $
*/

#ifndef _stat_h_
#define _stat_h_

void initStat(void);

void exitStat(void);

int updateStat(void);

void printCPUUser(const char* cmd);
void printCPUUserInfo(const char* cmd);
void printCPUNice(const char* cmd);
void printCPUNiceInfo(const char* cmd);
void printCPUSys(const char* cmd);
void printCPUSysInfo(const char* cmd);
void printCPUTotalLoad(const char* cmd);
void printCPUTotalLoadInfo(const char* cmd);
void printCPUIdle(const char* cmd);
void printCPUIdleInfo(const char* cmd);
void printCPUWait(const char* cmd);
void printCPUWaitInfo(const char* cmd);
void printCPUxUser(const char* cmd);
void printCPUxUserInfo(const char* cmd);
void printCPUxNice(const char* cmd);
void printCPUxNiceInfo(const char* cmd);
void printCPUxSys(const char* cmd);
void printCPUxSysInfo(const char* cmd);
void printCPUxTotalLoad(const char* cmd);
void printCPUxTotalLoadInfo(const char* cmd);
void printCPUxIdle(const char* cmd);
void printCPUxIdleInfo(const char* cmd);
void printCPUxWait(const char* cmd);
void printCPUxWaitInfo(const char* cmd);
void print24DiskIO(const char* cmd);
void print24DiskIOInfo(const char* cmd);
void print24DiskTotal(const char* cmd);
void print24DiskTotalInfo(const char* cmd);
void print24DiskRIO(const char* cmd);
void print24DiskRIOInfo(const char* cmd);
void print24DiskWIO(const char* cmd);
void print24DiskWIOInfo(const char* cmd);
void print24DiskRBlk(const char* cmd);
void print24DiskRBlkInfo(const char* cmd);
void print24DiskWBlk(const char* cmd);
void print24DiskWBlkInfo(const char* cmd);
void printPageIn(const char* cmd);
void printPageInInfo(const char* cmd);
void printPageOut(const char* cmd);
void printPageOutInfo(const char* cmd);
void printInterruptx(const char* cmd);
void printInterruptxInfo(const char* cmd);
void printCtxt(const char* cmd);
void printCtxtInfo(const char* cmd);
void printUptime(const char* cmd);
void printUptimeInfo(const char* cmd);

#endif
