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

	$Id: stat.c,v 1.18 2001/06/04 14:45:05 tokoe Exp $
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "Command.h"
#include "ksysguardd.h"

#include "stat.h"

typedef struct
{
	/* A CPU can be loaded with user processes, reniced processes and
	* system processes. Unused processing time is called idle load.
	* These variable store the percentage of each load type. */
	float userLoad;
	float niceLoad;
	float sysLoad;
	float idleLoad;
	float waitLoad;

	/* To calculate the loads we need to remember the tick values for each
	* load type. */
	unsigned long userTicks;
	unsigned long niceTicks;
	unsigned long sysTicks;
	unsigned long idleTicks;
	unsigned long waitTicks;
} CPULoadInfo;

typedef struct
{
	unsigned long delta;
	unsigned long old;
} DiskLoadSample;

typedef struct
{
	/* 5 types of samples are taken:
	total, rio, wio, rBlk, wBlk */
	DiskLoadSample s[ 5 ];
} DiskLoadInfo;

typedef struct DiskIOInfo
{
	int major;
	int minor;
	char* devname;

	int alive;
	DiskLoadSample total;
	DiskLoadSample rio;
	DiskLoadSample wio;
	DiskLoadSample rblk;
	DiskLoadSample wblk;
	struct DiskIOInfo* next;
} DiskIOInfo;

#define DISKDEVNAMELEN 16

static int Dirty = 0;

/* We have observed deviations of up to 5% in the accuracy of the timer
* interrupts. So we try to measure the interrupt interval and use this
* value to calculate timing dependent values. */
static float timeInterval = 0;
static struct timeval lastSampling;
static struct timeval currSampling;

static CPULoadInfo CPULoad;
static CPULoadInfo* SMPLoad = 0;
static unsigned CPUCount = 0;
static DiskLoadInfo* DiskLoad = 0;
static unsigned DiskCount = 0;
static DiskIOInfo* DiskIO = 0;
static unsigned long PageIn = 0;
static unsigned long OldPageIn = 0;
static unsigned long PageOut = 0;
static unsigned long OldPageOut = 0;
static unsigned long Ctxt = 0;
static unsigned long OldCtxt = 0;
static unsigned int NumOfInts = 0;
static unsigned long* OldIntr = 0;
static unsigned long* Intr = 0;

static int initStatDisk(char* tag, char* buf, char* label, char* shortLabel,
						int index, cmdExecutor ex, cmdExecutor iq);
static void updateCPULoad(const char* line, CPULoadInfo* load);
static int process24Disk(char* tag, char* buf, const char* label, int idx);
static void processStat(void);
static int process24DiskIO(const char* buf);
static void cleanup24DiskList(void);

static int
initStatDisk(char* tag, char* buf, char* label, char* shortLabel, int index,
			cmdExecutor ex, cmdExecutor iq)
{
	char sensorName[128];

	gettimeofday(&lastSampling, 0);

	if (strcmp(label, tag) == 0)
	{
		unsigned int i;
		buf = buf + strlen(label) + 1;

		for (i = 0; i < DiskCount; ++i)
		{
			sscanf(buf, "%lu", &DiskLoad[i].s[index].old);
			while (*buf && isblank(*buf++));
			while (*buf && isdigit(*buf++));
			sprintf(sensorName, "disk/disk%d/%s", i, shortLabel);
			registerMonitor(sensorName, "float", ex, iq);
		}

		return (1);
	}

	return (0);
}

/**
 * updateCPULoad
 *
 * Parses the total cpu status line from /proc/stat
 */
static void
updateCPULoad(const char* line, CPULoadInfo* load)
{
	unsigned long currUserTicks, currSysTicks, currNiceTicks;
	unsigned long currIdleTicks, currWaitTicks, totalTicks;

	if (sscanf(line, "%*s %lu %lu %lu %lu %lu", &currUserTicks, &currNiceTicks,
	        &currSysTicks, &currIdleTicks, &currWaitTicks) != 5)
	{
		return;
	}

	totalTicks = (currUserTicks - load->userTicks) +
	    (currSysTicks - load->sysTicks) +
	    (currNiceTicks - load->niceTicks) +
	    (currIdleTicks - load->idleTicks) +
	    (currWaitTicks - load->waitTicks);

	if (totalTicks > 10)
	{
		load->userLoad = (100 * (currUserTicks - load->userTicks))
			/ totalTicks;
		load->sysLoad = (100 * (currSysTicks - load->sysTicks))
			/ totalTicks;
		load->niceLoad = (100 * (currNiceTicks - load->niceTicks))
			/ totalTicks;
		load->idleLoad = (100 * (currIdleTicks - load->idleTicks))
			/ totalTicks;
		load->waitLoad = (100 * (currWaitTicks - load->waitTicks))
			/ totalTicks;
	}
	else
		load->userLoad = load->sysLoad = load->niceLoad = load->idleLoad = load->waitLoad = 0.0;

	load->userTicks = currUserTicks;
	load->sysTicks = currSysTicks;
	load->niceTicks = currNiceTicks;
	load->idleTicks = currIdleTicks;
	load->waitTicks = currWaitTicks;
}

static int
process24Disk(char* tag, char* buf, const char* label, int idx)
{
	if (strcmp(label, tag) == 0)
	{
		unsigned long val;
		unsigned int i;
		buf = buf + strlen(label) + 1;

		for (i = 0; i < DiskCount; ++i)
		{
			sscanf(buf, "%lu", &val);
			while (*buf && isblank(*buf++));
			while (*buf && isdigit(*buf++));
			DiskLoad[i].s[idx].delta = val - DiskLoad[i].s[idx].old;
			DiskLoad[i].s[idx].old = val;
		}

		return 1;
	}

	return 0;
}

static int
process24DiskIO(const char* buf)
{
	/* Process disk_io lines as provided by 2.4.x kernels.
	* disk_io: (2,0):(3,3,6,0,0) (3,0):(1413012,511622,12155382,901390,26486215) */
	int major, minor;
	unsigned long total, rblk, rio, wblk, wio;
	DiskIOInfo* ptr = DiskIO;
	DiskIOInfo* last = 0;
	char sensorName[128];
	const char* p;

	p = buf + strlen("disk_io: ");
	while (p && *p)
	{
		if (sscanf(p, "(%d,%d):(%lu,%lu,%lu,%lu,%lu)", &major, &minor,
		        &total, &rio, &rblk, &wio, &wblk) != 7)
			return -1;

		last = 0;
		ptr = DiskIO;
		while (ptr)
		{
			if (ptr->major == major && ptr->minor == minor)
			{
				/* The IO device has already been registered. */
				ptr->total.delta = total - ptr->total.old;
				ptr->total.old = total;
				ptr->rio.delta = rio - ptr->rio.old;
				ptr->rio.old = rio;
				ptr->wio.delta = wio - ptr->wio.old;
				ptr->wio.old = wio;
				ptr->rblk.delta = rblk - ptr->rblk.old;
				ptr->rblk.old = rblk;
				ptr->wblk.delta = wblk - ptr->wblk.old;
				ptr->wblk.old = wblk;
				ptr->alive = 1;
				break;
			}

			last = ptr;
			ptr = ptr->next;
		}

		if (!ptr)
		{
			/* The IO device has not been registered yet. We need to add it. */
			ptr = (DiskIOInfo*)malloc(sizeof(DiskIOInfo));
			ptr->major = major;
			ptr->minor = minor;

			/* 2.6 gives us a nice device name. On 2.4 we get nothing */
			ptr->devname = (char *)malloc(DISKDEVNAMELEN);
			memset(ptr->devname, 0, DISKDEVNAMELEN);

			ptr->total.delta = 0;
			ptr->total.old = total;
			ptr->rio.delta = 0;
			ptr->rio.old = rio;
			ptr->wio.delta = 0;
			ptr->wio.old = wio;
			ptr->rblk.delta = 0;
			ptr->rblk.old = rblk;
			ptr->wblk.delta = 0;
			ptr->wblk.old = wblk;
			ptr->alive = 1;
			ptr->next = 0;
			if (last)
			{
				/* Append new entry at end of list. */
				last->next = ptr;
			}
			else
			{
				/* List is empty, so we insert the fist element into the list. */
				DiskIO = ptr;
			}

			sprintf(sensorName, "disk/%s_(%d:%d)24/total", ptr->devname, major, minor);
			registerMonitor(sensorName, "float", print24DiskIO, print24DiskIOInfo);
			sprintf(sensorName, "disk/%s_(%d:%d)24/rio", ptr->devname, major, minor);
			registerMonitor(sensorName, "float", print24DiskIO, print24DiskIOInfo);
			sprintf(sensorName, "disk/%s_(%d:%d)24/wio", ptr->devname, major, minor);
			registerMonitor(sensorName, "float", print24DiskIO, print24DiskIOInfo);
			sprintf(sensorName, "disk/%s_(%d:%d)24/rblk", ptr->devname, major, minor);
			registerMonitor(sensorName, "float", print24DiskIO, print24DiskIOInfo);
			sprintf(sensorName, "disk/%s_(%d:%d)24/wblk", ptr->devname, major, minor);
			registerMonitor(sensorName, "float", print24DiskIO, print24DiskIOInfo);
		}

		/* Move p after the second ')'. We can safely assume that
		* those two ')' exist. */
		p = strchr(p, ')') + 1;
		p = strchr(p, ')') + 1;
		if (p && *p)
			p = strchr(p, '(');
	}

	return 0;
}

static void
cleanup24DiskList(void)
{
	DiskIOInfo* ptr = DiskIO;
	DiskIOInfo* last = 0;

	while (ptr)
	{
		if (ptr->alive == 0)
		{
			DiskIOInfo* newPtr;
			char sensorName[ 128 ];

			/* Disk device has disappeared. We have to remove it from
			* the list and unregister the monitors. */
			sprintf(sensorName, "disk/%s_(%d:%d)24/total", ptr->devname, ptr->major, ptr->minor);
			removeMonitor(sensorName);
			sprintf(sensorName, "disk/%s_(%d:%d)24/rio", ptr->devname, ptr->major, ptr->minor);
			removeMonitor(sensorName);
			sprintf(sensorName, "disk/%s_(%d:%d)24/wio", ptr->devname, ptr->major, ptr->minor);
			removeMonitor(sensorName);
			sprintf(sensorName, "disk/%s_(%d:%d)24/rblk", ptr->devname, ptr->major, ptr->minor);
			removeMonitor(sensorName);
			sprintf(sensorName, "disk/%s_(%d:%d)24/wblk", ptr->devname, ptr->major, ptr->minor);
			removeMonitor(sensorName);
			if (last)
			{
				last->next = ptr->next;
				newPtr = ptr->next;
			}
			else
			{
				DiskIO = ptr->next;
				newPtr = DiskIO;
				last = 0;
			}

			free(ptr);
			ptr = newPtr;
		}
		else
		{
			ptr->alive = 0;
			last = ptr;
			ptr = ptr->next;
		}
	}
}

static void
processStat(void)
{
	char format[ 32 ];
	char tagFormat[ 16 ];
	char buf[ 1024 ];
	char tag[ 32 ];

	sprintf(format, "%%%d[^\n]\n", (int)sizeof(buf) - 1);
	sprintf(tagFormat, "%%%ds", (int)sizeof(tag) - 1);

	gettimeofday(&currSampling, 0);
	Dirty = 0;

	FILE *stat = fopen("/proc/stat", "r");
	if (!stat)
	{
		print_error("Cannot open file \'/proc/stat\'!\n"
		    "The kernel needs to be compiled with support\n"
		    "for /proc file system enabled!\n");
		return;
	}

	while (fscanf(stat, format, buf) == 1)
	{
		buf[ sizeof(buf) - 1 ] = '\0';
		sscanf(buf, tagFormat, tag);

		if (strcmp("cpu", tag) == 0)
		{
			/* Total CPU load */
			updateCPULoad(buf, &CPULoad);
		}
		else if (strncmp("cpu", tag, 3) == 0)
		{
			/* Load for each SMP CPU */
			int id;
			sscanf(tag + 3, "%d", &id);
			updateCPULoad(buf, &SMPLoad[ id ]);
		}
		else if (process24Disk(tag, buf, "disk", 0))
		{
		}
		else if (process24Disk(tag, buf, "disk_rio", 1))
		{
		}
		else if (process24Disk(tag, buf, "disk_wio", 2))
		{
		}
		else if (process24Disk(tag, buf, "disk_rblk", 3))
		{
		}
		else if (process24Disk(tag, buf, "disk_wblk", 4))
		{
		}
		else if (strcmp("disk_io:", tag) == 0)
		{
			process24DiskIO(buf);
		}
		else if (strcmp("page", tag) == 0)
		{
			unsigned long v1, v2;
			sscanf(buf + 5, "%lu %lu", &v1, &v2);
			PageIn = v1 - OldPageIn;
			OldPageIn = v1;
			PageOut = v2 - OldPageOut;
			OldPageOut = v2;
		}
		else if (strcmp("intr", tag) == 0)
		{
			unsigned int i = 0;
			char* p = buf + 5;

			for (i = 0; i < NumOfInts; i++)
			{
				unsigned long val;

				sscanf(p, "%lu", &val);
				Intr[i] = val - OldIntr[i];
				OldIntr[i] = val;
				while (*p && *p != ' ')
					p++;
				while (*p && *p == ' ')
					p++;
			}
		} else if (strcmp("ctxt", tag) == 0)
		{
			unsigned long val;

			sscanf(buf + 5, "%lu", &val);
			Ctxt = val - OldCtxt;
			OldCtxt = val;
		}
	}
	fclose(stat);

	/* Read Linux 2.5.x /proc/vmstat */
	stat = fopen("/proc/vmstat", "r");
	if (stat)
	{
		while (fscanf(stat, format, buf) == 1)
		{
			buf[ sizeof(buf) - 1 ] = '\0';
			sscanf(buf, tagFormat, tag);

			if (strcmp("pgpgin", tag) == 0)
			{
				unsigned long v1;
				sscanf(buf + 7, "%lu", &v1);
				PageIn = v1 - OldPageIn;
				OldPageIn = v1;
			}
			else if (strcmp("pgpgout", tag) == 0)
			{
				unsigned long v1;
				sscanf(buf + 7, "%lu", &v1);
				PageOut = v1 - OldPageOut;
				OldPageOut = v1;
			}
		}
		fclose(stat);
	}

	/* save exact time interval between this and the last read of /proc/stat */
	timeInterval = currSampling.tv_sec - lastSampling.tv_sec +
	    (currSampling.tv_usec - lastSampling.tv_usec) / 1000000.0;
	lastSampling = currSampling;

	cleanup24DiskList();

}

/*
================================ public part =================================
*/

void
initStat(void)
{
	/* The CPU load is calculated from the values in /proc/stat. The cpu
	* entry contains 7 counters. These counters count the number of ticks
	* the system has spend on user processes, system processes, nice
	* processes, idle and IO-wait time, hard and soft interrupts.
	*
	* SMP systems will have cpu1 to cpuN lines right after the cpu info. The
	* format is identical to cpu and reports the information for each cpu.
	* Linux kernels <= 2.0 do not provide this information!
	*
	* The /proc/stat file looks like this:
	*
	* cpu  <user> <nice> <system> <idling> <waiting> <hardinterrupt> <softinterrupt>
	* disk 7797 0 0 0
	* disk_rio 6889 0 0 0
	* disk_wio 908 0 0 0
	* disk_rblk 13775 0 0 0
	* disk_wblk 1816 0 0 0
	* page 27575 1330
	* swap 1 0
	* intr 50444 38672 2557 0 0 0 0 2 0 2 0 0 3 1429 1 7778 0
	* ctxt 54155
	* btime 917379184
	* processes 347
	*
	* Linux kernel >= 2.4.0 have one or more disk_io: lines instead of
	* the disk_* lines.
	*
	* Linux kernel >= 2.6.x(?) have disk I/O stats in /proc/diskstats
	* and no disk relevant lines are found in /proc/stat
	*/

	char format[ 32 ];
	char tagFormat[ 16 ];
	char buf[ 1024 ];
	char tag[ 32 ];

	sprintf(format, "%%%d[^\n]\n", (int)sizeof(buf) - 1);
	sprintf(tagFormat, "%%%ds", (int)sizeof(tag) - 1);

	FILE *stat = fopen("/proc/stat", "r");
	if (!stat)
	{
		print_error("Cannot open file \'/proc/stat\'!\n"
		    "The kernel needs to be compiled with support\n"
		    "for /proc file system enabled!\n");
		return;
	}
	while (fscanf(stat, format, buf) == 1)
	{
		buf[ sizeof(buf) - 1 ] = '\0';
		sscanf(buf, tagFormat, tag);

		if (strcmp("cpu", tag) == 0)
		{
			/* Total CPU load */
			registerMonitor("cpu/user", "float", printCPUUser,
							printCPUUserInfo);
			registerMonitor("cpu/nice", "float", printCPUNice,
							printCPUNiceInfo);
			registerMonitor("cpu/sys", "float", printCPUSys,
							printCPUSysInfo);
			registerMonitor("cpu/TotalLoad", "float", printCPUTotalLoad,
							printCPUTotalLoadInfo);
			registerMonitor("cpu/idle", "float", printCPUIdle,
							printCPUIdleInfo);
			registerMonitor("cpu/wait", "float", printCPUWait,
							printCPUWaitInfo);
		}
		else if (strncmp("cpu", tag, 3) == 0)
		{
			char cmdName[ 24 ];
			/* Load for each SMP CPU */
			unsigned id;

			sscanf(tag + 3, "%d", &id);
			if (CPUCount < id + 1)
				CPUCount = id + 1;
			sprintf(cmdName, "cpu/cpu%d/user", id);
			registerMonitor(cmdName, "float", printCPUxUser, printCPUxUserInfo);
			sprintf(cmdName, "cpu/cpu%d/nice", id);
			registerMonitor(cmdName, "float", printCPUxNice, printCPUxNiceInfo);
			sprintf(cmdName, "cpu/cpu%d/sys", id);
			registerMonitor(cmdName, "float", printCPUxSys, printCPUxSysInfo);
			sprintf(cmdName, "cpu/cpu%d/TotalLoad", id);
			registerMonitor(cmdName, "float", printCPUxTotalLoad, printCPUxTotalLoadInfo);
			sprintf(cmdName, "cpu/cpu%d/idle", id);
			registerMonitor(cmdName, "float", printCPUxIdle, printCPUxIdleInfo);
			sprintf(cmdName, "cpu/cpu%d/wait", id);
			registerMonitor(cmdName, "float", printCPUxWait, printCPUxWaitInfo);
		}
		else if (strcmp("disk", tag) == 0)
		{
			unsigned long val;
			char* b = buf + 5;

			/* Count the number of registered disks */
			for (DiskCount = 0; *b && sscanf(b, "%lu", &val) == 1; DiskCount++)
			{
				while (*b && isblank(*b++));
				while (*b && isdigit(*b++));
			}

			if (DiskCount > 0)
				DiskLoad = (DiskLoadInfo*)malloc(sizeof(DiskLoadInfo) * DiskCount);

			initStatDisk(tag, buf, "disk", "disk", 0, print24DiskTotal, print24DiskTotalInfo);
		}
		else if (initStatDisk(tag, buf, "disk_rio", "rio", 1, print24DiskRIO, print24DiskRIOInfo));
		else if (initStatDisk(tag, buf, "disk_wio", "wio", 2, print24DiskWIO, print24DiskWIOInfo));
		else if (initStatDisk(tag, buf, "disk_rblk", "rblk", 3, print24DiskRBlk, print24DiskRBlkInfo));
		else if (initStatDisk(tag, buf, "disk_wblk", "wblk", 4, print24DiskWBlk, print24DiskWBlkInfo));
		else if (strcmp("disk_io:", tag) == 0)
			process24DiskIO(buf);
		else if (strcmp("page", tag) == 0)
		{
			sscanf(buf + 5, "%lu %lu", &OldPageIn, &OldPageOut);
			registerMonitor("cpu/pageIn", "float", printPageIn, printPageInInfo);
			registerMonitor("cpu/pageOut", "float", printPageOut, printPageOutInfo);
		}
		else if (strcmp("intr", tag) == 0)
		{
			unsigned int i;
			char cmdName[ 32 ];
			char* p = buf + 5;

			/* Count the number of listed values in the intr line. */
			NumOfInts = 0;
			while (*p)
				if (*p++ == ' ')
					NumOfInts++;

			/* It looks like anything above 24 is always 0. So let's just
			* ignore this for the time being. */
			if (NumOfInts > 25)
				NumOfInts = 25;
			OldIntr = (unsigned long*)malloc(NumOfInts * sizeof(unsigned long));
			Intr = (unsigned long*)malloc(NumOfInts * sizeof(unsigned long));
			i = 0;
			p = buf + 5;
			for (i = 0; p && i < NumOfInts; i++)
			{
				sscanf(p, "%lu", &OldIntr[i]);
				while (*p && *p != ' ')
					p++;
				while (*p && *p == ' ')
					p++;
				sprintf(cmdName, "cpu/interrupts/int%02d", i);
				registerMonitor(cmdName, "float", printInterruptx,
                                        printInterruptxInfo);
			}
		}
		else if (strcmp("ctxt", tag) == 0)
		{
			sscanf(buf + 5, "%lu", &OldCtxt);
			registerMonitor("cpu/context", "float", printCtxt, printCtxtInfo);
		}
	}
	fclose(stat);

	stat = fopen("/proc/vmstat", "r");
	if (!stat)
	{
		print_error("Cannot open file \'/proc/vmstat\'\n");
	} else
	{
		while (fscanf(stat, format, buf) == 1)
		{
			buf[ sizeof(buf) - 1 ] = '\0';
			sscanf(buf, tagFormat, tag);

			if (strcmp("pgpgin", tag) == 0)
			{
				sscanf(buf + 7, "%lu", &OldPageIn);
				registerMonitor("cpu/pageIn", "float", printPageIn, printPageInInfo);
			}
			else if (strcmp("pgpgout", tag) == 0)
			{
				sscanf(buf + 7, "%lu", &OldPageOut);
				registerMonitor("cpu/pageOut", "float", printPageOut, printPageOutInfo);
			}
		}
		fclose(stat);
	}
	if (CPUCount > 0)
		SMPLoad = (CPULoadInfo*)calloc(CPUCount, sizeof(CPULoadInfo));

	/* Call processStat to eliminate initial peek values. */
	processStat();
}

void
exitStat(void)
{
	free(DiskLoad);
	DiskLoad = 0;

	free(SMPLoad);
	SMPLoad = 0;

	free(OldIntr);
	OldIntr = 0;

	free(Intr);
	Intr = 0;

	removeMonitor("cpu/system/user");
	removeMonitor("cpu/system/nice");
	removeMonitor("cpu/system/sys");
	removeMonitor("cpu/system/idle");

	/* Todo: Dynamically registered monitors (per cpu, per disk) are not removed yet) */

	/* These were registered as legacy monitors */
	removeMonitor("cpu/user");
	removeMonitor("cpu/nice");
	removeMonitor("cpu/sys");
	removeMonitor("cpu/idle");
}

int
updateStat(void)
{
	Dirty = 1;
	return 0;
}

void
printCPUUser(const char* cmd)
{
	(void)cmd;

	if (Dirty)
		processStat();

	fprintf(CurrentClient, "%f\n", CPULoad.userLoad);
}

void
printCPUUserInfo(const char* cmd)
{
	(void)cmd;

	fprintf(CurrentClient, "CPU User Load\t0\t100\t%%\n");
}

void
printCPUNice(const char* cmd)
{
	(void)cmd;

	if (Dirty)
		processStat();

	fprintf(CurrentClient, "%f\n", CPULoad.niceLoad);
}

void
printCPUNiceInfo(const char* cmd)
{
	(void)cmd;

	fprintf(CurrentClient, "CPU Nice Load\t0\t100\t%%\n");
}

void
printCPUSys(const char* cmd)
{
	(void)cmd;

	if (Dirty)
		processStat();

	fprintf(CurrentClient, "%f\n", CPULoad.sysLoad);
}

void
printCPUSysInfo(const char* cmd)
{
	(void)cmd;

	fprintf(CurrentClient, "CPU System Load\t0\t100\t%%\n");
}

void
printCPUTotalLoad(const char* cmd)
{
	(void)cmd;

	if (Dirty)
		processStat();

	fprintf(CurrentClient, "%f\n", CPULoad.userLoad + CPULoad.sysLoad + CPULoad.niceLoad + CPULoad.waitLoad);
}

void
printCPUTotalLoadInfo(const char* cmd)
{
	(void)cmd;

	fprintf(CurrentClient, "CPU Total Load\t0\t100\t%%\n");
}

void
printCPUIdle(const char* cmd)
{
	if (Dirty)
		processStat();

	fprintf(CurrentClient, "%f\n", CPULoad.idleLoad);
}

void
printCPUIdleInfo(const char* cmd)
{
	fprintf(CurrentClient, "CPU Idle Load\t0\t100\t%%\n");
}

void
printCPUWait(const char* cmd)
{
	(void)cmd;

	if (Dirty)
		processStat();

	fprintf(CurrentClient, "%f\n", CPULoad.waitLoad);
}

void
printCPUWaitInfo(const char* cmd)
{
	(void)cmd;
	fprintf(CurrentClient, "CPU Wait Load\t0\t100\t%%\n");
}

void
printCPUxUser(const char* cmd)
{
	int id;

	if (Dirty)
		processStat();

	sscanf(cmd + 7, "%d", &id);
	fprintf(CurrentClient, "%f\n", SMPLoad[ id ].userLoad);
}

void
printCPUxUserInfo(const char* cmd)
{
	int id;

	sscanf(cmd + 7, "%d", &id);
	fprintf(CurrentClient, "CPU %d User Load\t0\t100\t%%\n", id+1);
}

void
printCPUxNice(const char* cmd)
{
	int id;

	if (Dirty)
		processStat();

	sscanf(cmd + 7, "%d", &id);
	fprintf(CurrentClient, "%f\n", SMPLoad[ id ].niceLoad);
}

void
printCPUxNiceInfo(const char* cmd)
{
	int id;

	sscanf(cmd + 7, "%d", &id);
	fprintf(CurrentClient, "CPU %d Nice Load\t0\t100\t%%\n", id+1);
}

void
printCPUxSys(const char* cmd)
{
	int id;

	if (Dirty)
		processStat();

	sscanf(cmd + 7, "%d", &id);
	fprintf(CurrentClient, "%f\n", SMPLoad[ id ].sysLoad);
}

void
printCPUxSysInfo(const char* cmd)
{
	int id;

	sscanf(cmd + 7, "%d", &id);
	fprintf(CurrentClient, "CPU %d System Load\t0\t100\t%%\n", id+1);
}

void
printCPUxTotalLoad(const char* cmd)
{
	int id;

	if (Dirty)
		processStat();

	sscanf(cmd + 7, "%d", &id);
	fprintf(CurrentClient, "%f\n", SMPLoad[ id ].userLoad + SMPLoad[ id ].sysLoad + SMPLoad[ id ].niceLoad + SMPLoad[ id ].waitLoad);
}

void
printCPUxTotalLoadInfo(const char* cmd)
{
	int id;

	sscanf(cmd + 7, "%d", &id);
	fprintf(CurrentClient, "CPU %d\t0\t100\t%%\n", id+1);
}

void
printCPUxIdle(const char* cmd)
{
	int id;

	if (Dirty)
		processStat();

	sscanf(cmd + 7, "%d", &id);
	fprintf(CurrentClient, "%f\n", SMPLoad[ id ].idleLoad);
}

void
printCPUxIdleInfo(const char* cmd)
{
	int id;

	sscanf(cmd + 7, "%d", &id);
	fprintf(CurrentClient, "CPU %d Idle Load\t0\t100\t%%\n", id+1);
}

void
printCPUxWait(const char* cmd)
{
	int id;

	if (Dirty)
		processStat();

	sscanf(cmd + 7, "%d", &id);
	fprintf(CurrentClient, "%f\n", SMPLoad[ id ].waitLoad);
}

void
printCPUxWaitInfo(const char* cmd)
{
	int id;

	sscanf(cmd + 7, "%d", &id);
	fprintf(CurrentClient, "CPU %d Wait Load\t0\t100\t%%\n", id+1);
}

void
print24DiskTotal(const char* cmd)
{
	int id;

	if (Dirty)
		processStat();

	sscanf(cmd + 9, "%d", &id);
	fprintf(CurrentClient, "%f\n", (float)(DiskLoad[ id ].s[ 0 ].delta
	        / timeInterval));
}

void
print24DiskTotalInfo(const char* cmd)
{
	int id;

	sscanf(cmd + 9, "%d", &id);
	fprintf(CurrentClient, "Disk %d Total Load\t0\t0\tKB/s\n", id);
}

void
print24DiskRIO(const char* cmd)
{
	int id;

	if (Dirty)
		processStat();

	sscanf(cmd + 9, "%d", &id);
	fprintf(CurrentClient, "%f\n", (float)(DiskLoad[ id ].s[ 1 ].delta
	        / timeInterval));
}

void
print24DiskRIOInfo(const char* cmd)
{
	int id;

	sscanf(cmd + 9, "%d", &id);
	fprintf(CurrentClient, "Disk %d Read\t0\t0\tKB/s\n", id);
}

void
print24DiskWIO(const char* cmd)
{
	int id;

	if (Dirty)
		processStat();

	sscanf(cmd + 9, "%d", &id);
	fprintf(CurrentClient, "%f\n", (float)(DiskLoad[ id ].s[ 2 ].delta
	        / timeInterval));
}

void
print24DiskWIOInfo(const char* cmd)
{
	int id;

	sscanf(cmd + 9, "%d", &id);
	fprintf(CurrentClient, "Disk %d Write\t0\t0\tKB/s\n", id);
}

void
print24DiskRBlk(const char* cmd)
{
	int id;

	if (Dirty)
		processStat();

	sscanf(cmd + 9, "%d", &id);
	/* a block is 512 bytes or 1/2 kBytes */
	fprintf(CurrentClient, "%f\n", (float)(DiskLoad[ id ].s[ 3 ].delta / timeInterval * 2));
}

void
print24DiskRBlkInfo(const char* cmd)
{
	int id;

	sscanf(cmd + 9, "%d", &id);
	fprintf(CurrentClient, "Disk %d Read Data\t0\t0\tKB/s\n", id);
}

void
print24DiskWBlk(const char* cmd)
{
	int id;

	if (Dirty)
		processStat();

	sscanf(cmd + 9, "%d", &id);
	/* a block is 512 bytes or 1/2 kBytes */
	fprintf(CurrentClient, "%f\n", (float)(DiskLoad[ id ].s[ 4 ].delta / timeInterval * 2));
}

void
print24DiskWBlkInfo(const char* cmd)
{
	int id;

	sscanf(cmd + 9, "%d", &id);
	fprintf(CurrentClient, "Disk %d Write Data\t0\t0\tKB/s\n", id);
}

void
printPageIn(const char* cmd)
{
	(void)cmd;

	if (Dirty)
		processStat();

	fprintf(CurrentClient, "%f\n", (float)(PageIn / timeInterval));
}

void
printPageInInfo(const char* cmd)
{
	(void)cmd;

	fprintf(CurrentClient, "Paged in Pages\t0\t0\t1/s\n");
}

void
printPageOut(const char* cmd)
{
	(void)cmd;

	if (Dirty)
		processStat();

	fprintf(CurrentClient, "%f\n", (float)(PageOut / timeInterval));
}

void
printPageOutInfo(const char* cmd)
{
	(void)cmd;

	fprintf(CurrentClient, "Paged out Pages\t0\t0\t1/s\n");
}

void
printInterruptx(const char* cmd)
{
	int id;

	if (Dirty)
		processStat();

	sscanf(cmd + strlen("cpu/interrupts/int"), "%d", &id);
	fprintf(CurrentClient, "%f\n", (float)(Intr[ id ] / timeInterval));
}

void
printInterruptxInfo(const char* cmd)
{
	int id;

	sscanf(cmd + strlen("cpu/interrupt/int"), "%d", &id);
	fprintf(CurrentClient, "Interrupt %d\t0\t0\t1/s\n", id);
}

void
printCtxt(const char* cmd)
{
	(void)cmd;

	if (Dirty)
		processStat();

	fprintf(CurrentClient, "%f\n", (float)(Ctxt / timeInterval));
}

void
printCtxtInfo(const char* cmd)
{
	(void)cmd;

	fprintf(CurrentClient, "Context switches\t0\t0\t1/s\n");
}

void
print24DiskIO(const char* cmd)
{
	int major, minor;
	char devname[DISKDEVNAMELEN];
	char name[ 17 ];
	DiskIOInfo* ptr;

	sscanf(cmd, "disk/%[^_]_(%d:%d)/%16s", devname, &major, &minor, name);

	if (Dirty)
		processStat();

	ptr = DiskIO;
	while (ptr && (ptr->major != major || ptr->minor != minor))
		ptr = ptr->next;

	if (!ptr)
	{
		print_error("RECONFIGURE");
		fprintf(CurrentClient, "0\n");

		log_error("Disk device disappeared");
		return;
	}

	if (strcmp(name, "total") == 0)
		fprintf(CurrentClient, "%f\n", (float)(ptr->total.delta / timeInterval));
	else if (strcmp(name, "rio") == 0)
		fprintf(CurrentClient, "%f\n", (float)(ptr->rio.delta / timeInterval));
	else if (strcmp(name, "wio") == 0)
		fprintf(CurrentClient, "%f\n", (float)(ptr->wio.delta / timeInterval));
	else if (strcmp(name, "rblk") == 0)
		fprintf(CurrentClient, "%f\n", (float)(ptr->rblk.delta / (timeInterval * 2)));
	else if (strcmp(name, "wblk") == 0)
		fprintf(CurrentClient, "%f\n", (float)(ptr->wblk.delta / (timeInterval * 2)));
	else
	{
		fprintf(CurrentClient, "0\n");
		log_error("Unknown disk device property \'%s\'", name);
	}
}

void
print24DiskIOInfo(const char* cmd)
{
	int major, minor;
	char devname[DISKDEVNAMELEN];
	char name[ 17 ];
	DiskIOInfo* ptr = DiskIO;

	sscanf(cmd, "disk/%[^_]_(%d:%d)/%16s", devname, &major, &minor, name);

	while (ptr && (ptr->major != major || ptr->minor != minor))
		ptr = ptr->next;

	if (!ptr)
	{
		/* Disk device has disappeared. Print a dummy answer. */
		fprintf(CurrentClient, "Dummy\t0\t0\t\n");
		return;
	}

	/* remove trailing '?' */
	name[ strlen(name) - 1 ] = '\0';

	if (strcmp(name, "total") == 0)
		fprintf(CurrentClient, "Total accesses device %s (%d:%d)\t0\t0\t1/s\n",
		    devname, major, minor);
	else if (strcmp(name, "rio") == 0)
		fprintf(CurrentClient, "Read data device %s (%d:%d)\t0\t0\t1/s\n",
		    devname, major, minor);
	else if (strcmp(name, "wio") == 0)
		fprintf(CurrentClient, "Write data device %s (%d:%d)\t0\t0\t1/s\n",
		    devname, major, minor);
	else if (strcmp(name, "rblk") == 0)
		fprintf(CurrentClient, "Read accesses device %s (%d:%d)\t0\t0\tKB/s\n",
		    devname, major, minor);
	else if (strcmp(name, "wblk") == 0)
		fprintf(CurrentClient, "Write accesses device %s (%d:%d)\t0\t0\tKB/s\n",
		    devname, major, minor);
	else
	{
		fprintf(CurrentClient, "Dummy\t0\t0\t\n");
		log_error("Request for unknown device property \'%s\'",	name);
	}
}
