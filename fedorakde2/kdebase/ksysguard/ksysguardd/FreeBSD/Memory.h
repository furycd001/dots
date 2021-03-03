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

	$Id: Memory.h,v 1.1.6.1 2001/11/08 10:48:51 tokoe Exp $
*/

#ifndef _memory_h_
#define _memory_h_

void initMemory(void);
void exitMemory(void);

int updateMemory(void);

void printMFree(const char* cmd);
void printMFreeInfo(const char* cmd);
void printUsed(const char* cmd);
void printUsedInfo(const char* cmd);
void printBuffers(const char* cmd);
void printBuffersInfo(const char* cmd);
void printCached(const char* cmd);
void printCachedInfo(const char* cmd);
void printSwapUsed(const char* cmd);
void printSwapUsedInfo(const char* cmd);
void printSwapFree(const char* cmd);
void printSwapFreeInfo(const char* cmd);

#endif
