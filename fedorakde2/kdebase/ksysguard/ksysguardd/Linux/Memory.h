/*
    KSysGuard, the KDE System Guard
   
	Copyright (c) 1999 - 2001 Chris Schlaeger <cs@kde.org>
    
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

	$Id: Memory.h,v 1.4 2001/04/22 13:41:33 cschlaeg Exp $
*/

#ifndef _Memory_h_
#define _Memory_h_

void initMemory(void);

void exitMemory(void);

int updateMemory(void);

void printTotal(const char *cmd);
void printTotalInfo(const char *cmd);
void printMFree(const char *cmd);
void printMFreeInfo(const char *cmd);
void printAvailable(const char *cmd);
void printAvailableInfo(const char *cmd);
void printUsed(const char *cmd);
void printUsedInfo(const char *cmd);
void printAppl(const char *cmd);
void printApplInfo(const char *cmd);
void printBuffers(const char *cmd);
void printBuffersInfo(const char *cmd);
void printCached(const char *cmd);
void printCachedInfo(const char *cmd);
void printAllocated(const char *cmd);
void printAllocatedInfo(const char *cmd);
void printSwapUsed(const char *cmd);
void printSwapUsedInfo(const char *cmd);
void printSwapFree(const char *cmd);
void printSwapFreeInfo(const char *cmd);
void printCDirty(const char *cmd);
void printCDirtyInfo(const char *cmd);
void printCWriteback(const char *cmd);
void printCWritebackInfo(const char *cmd);

#endif
