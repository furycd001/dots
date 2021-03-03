/*
    KTop, the KDE Task Manager
   
	Copyright (c) 1999 Chris Schlaeger <cs@kde.org>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef _cpuinfo_h_
#define _cpuinfo_h_

void initCPU(void);

void exitCPU(void);

int updateCPU(void);

void printCPUUser(const char* cmd);
void printCPUUserInfo(const char* cmd);
void printCPUNice(const char* cmd);
void printCPUNiceInfo(const char* cmd);
void printCPUSys(const char* cmd);
void printCPUSysInfo(const char* cmd);
void printCPUIdle(const char* cmd);
void printCPUIdleInfo(const char* cmd);
void printCPUxUser(const char* cmd);
void printCPUxUserInfo(const char* cmd);
void printCPUxNice(const char* cmd);
void printCPUxNiceInfo(const char* cmd);
void printCPUxSys(const char* cmd);
void printCPUxSysInfo(const char* cmd);
void printCPUxIdle(const char* cmd);
void printCPUxIdleInfo(const char* cmd);

#endif
