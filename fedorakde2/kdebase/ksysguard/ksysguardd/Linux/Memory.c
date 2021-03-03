/*
    KSysGuard, the KDE System Guard

    Copyright (c) 1999 - 2001 Chris Schlaeger <cs@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General Public
    License as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Command.h"
#include "ksysguardd.h"

#include "Memory.h"

#define MEMINFOBUFSIZE (2 * 1024)

static char MemInfoBuf[ MEMINFOBUFSIZE ];
static int Dirty = 1;

static unsigned long long Total = 0;
static unsigned long long MFree = 0;
static unsigned long long Available = 0;
static unsigned long long Appl = 0;
static unsigned long long Used = 0;
static unsigned long long Buffers = 0;
static unsigned long long Cached = 0;
static unsigned long long Allocated = 0;
static unsigned long long STotal = 0;
static unsigned long long SFree = 0;
static unsigned long long SUsed = 0;
static unsigned long long CDirty = 0;
static unsigned long long CWriteback = 0;

static void
scan_one(const char* buff, const char *key, unsigned long long* val)
{
	char *b = strstr(buff, key);
	if (b)
		sscanf(b + strlen(key), ": %llu", val);
}

static void
processMemInfo()
{
	unsigned long long Slab = 0;
	scan_one(MemInfoBuf, "MemTotal", &Total);
	scan_one(MemInfoBuf, "MemFree", &MFree);
	scan_one(MemInfoBuf, "MemAvailable", &Available);
	scan_one(MemInfoBuf, "Buffers", &Buffers);
	scan_one(MemInfoBuf, "Cached", &Cached);
	scan_one(MemInfoBuf, "Slab", &Slab);
	scan_one(MemInfoBuf, "SwapTotal", &STotal);
	scan_one(MemInfoBuf, "SwapFree", &SFree);
	scan_one(MemInfoBuf, "Dirty", &CDirty);
	scan_one(MemInfoBuf, "Writeback", &CWriteback);
	Cached += Slab;
	Used = Total - MFree;
	Appl = (Used - (Buffers + Cached));

	Allocated = Total - Available;

	if (STotal == 0)   /* no swap activated */
		SUsed = 0;
	else
		SUsed = STotal - SFree;

	Dirty = 0;
}

/*
================================ public part =================================
*/

void
initMemory(void)
{
	/**
	  Make sure that /proc/meminfo exists and is readable. If not we do
	  not register any monitors for memory.
	 */
	if (updateMemory() < 0)
		return;

	registerMonitor("mem/physical/total", "integer", printTotal, printTotalInfo);
	registerMonitor("mem/physical/free", "integer", printMFree, printMFreeInfo);
	registerMonitor("mem/physical/available", "integer", printAvailable, printAvailableInfo);
	registerMonitor("mem/physical/used", "integer", printUsed, printUsedInfo);
	registerMonitor("mem/physical/application", "integer", printAppl, printApplInfo);
	registerMonitor("mem/physical/buf", "integer", printBuffers, printBuffersInfo);
	registerMonitor("mem/physical/cached", "integer", printCached, printCachedInfo);
	registerMonitor("mem/physical/allocated", "integer", printAllocated, printAllocatedInfo);
	registerMonitor("mem/swap/used", "integer", printSwapUsed, printSwapUsedInfo);
	registerMonitor("mem/swap/free", "integer", printSwapFree, printSwapFreeInfo);
	registerMonitor("mem/cache/dirty", "integer", printCDirty, printCDirtyInfo);
	registerMonitor("mem/cache/writeback", "integer", printCWriteback, printCWritebackInfo);
}

void
exitMemory(void)
{
}

int
updateMemory(void)
{
	/**
	  The amount of total and used memory is read from the /proc/meminfo.
	  It also contains the information about the swap space.
	  The 'file' looks like this:

	  MemTotal:       516560 kB
	  MemFree:          7812 kB
	  MemShared:           0 kB
	  Buffers:         80312 kB
	  Cached:         236432 kB
	  SwapCached:        468 kB
	  Active:         291992 kB
	  Inactive:       133556 kB
	  HighTotal:           0 kB
	  HighFree:            0 kB
	  LowTotal:       516560 kB
	  LowFree:          7812 kB
	  SwapTotal:      899632 kB
	  SwapFree:       898932 kB
	  Dirty:            2736 kB
	  Writeback:           0 kB
	  Mapped:         155996 kB
	  Slab:            73920 kB
	  Committed_AS:   315588 kB
	  PageTables:       1764 kB
	  ReverseMaps:    103458
	 */

	int fd;
	size_t n;

	if ((fd = open("/proc/meminfo", O_RDONLY)) < 0)
	{
		print_error("Cannot open \'/proc/meminfo\'!\n"
		            "The kernel needs to be compiled with support\n"
		            "for /proc file system enabled!\n");
		return -1;
	}

	n = read(fd, MemInfoBuf, MEMINFOBUFSIZE - 1);
	if (n == MEMINFOBUFSIZE - 1 || n <= 0)
	{
		log_error("Internal buffer too small to read \'/proc/meminfo\'");
		close(fd);
		return -1;
	}

	close(fd);
	MemInfoBuf[ n ] = '\0';
	Dirty = 1;

	return 0;
}

void
printTotal(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "%llu\n", Total);
}

void
printTotalInfo(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "Total Memory\t0\t%llu\tKB\n", Total);
}

void
printMFree(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "%llu\n", MFree);
}

void
printMFreeInfo(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "Free Memory\t0\t%llu\tKB\n", Total);
}

void
printAvailable(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "%llu\n", Available);
}

void
printAvailableInfo(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "Available Memory\t0\t%llu\tKB\n", Total);
}

void
printUsed(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "%llu\n", Used);
}

void
printUsedInfo(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "Used Memory\t0\t%llu\tKB\n", Total);
}

void
printAppl(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "%llu\n", Appl);
}

void
printApplInfo(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "Application Memory\t0\t%llu\tKB\n", Total);
}

void
printBuffers(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "%llu\n", Buffers);
}

void
printBuffersInfo(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "Buffer Memory\t0\t%llu\tKB\n", Total);
}

void
printCached(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "%llu\n", Cached);
}

void
printCachedInfo(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "Cached Memory\t0\t%llu\tKB\n", Total);
}

void
printAllocated(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "%llu\n", Allocated);
}

void
printAllocatedInfo(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "Allocated Memory\t0\t%llu\tKB\n", Total);
}

void
printSwapUsed(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "%llu\n", SUsed);
}

void
printSwapUsedInfo(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "Used Swap Memory\t0\t%llu\tKB\n", STotal);
}

void
printSwapFree(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "%llu\n", SFree);
}

void
printSwapFreeInfo(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "Free Swap Memory\t0\t%llu\tKB\n", STotal);
}

void
printCDirty(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "%llu\n", CDirty);
}

void
printCDirtyInfo(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "Dirty Memory\t0\t%llu\tKB\n", CDirty);
}

void
printCWriteback(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "%llu\n", CWriteback);
}

void
printCWritebackInfo(const char* cmd)
{
	if (Dirty)
		processMemInfo();

	fprintf(CurrentClient, "Writeback Memory\t0\t%llu\tKB\n", CWriteback);
}
