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
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

	$Id: apm.c,v 1.7 2001/05/28 16:38:45 cschlaeg Exp $
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include "ksysguardd.h"
#include "Command.h"
#include "apm.h"

static int ApmOK = 0;
static int BatFill, BatTime;

#define APMBUFSIZE 128
static char ApmBuf[APMBUFSIZE];
static int Dirty = 0;

static void
processApm(void)
{
	sscanf(ApmBuf, "%*f %*f %*x %*x %*x %*x %d%% %d min",
		   &BatFill, &BatTime);
	Dirty = 0;
}

/*
================================ public part =================================
*/

void
initApm(void)
{
	if (updateApm() < 0)
	{
		ApmOK = -1;
		return;
	}
	else
		ApmOK = 1;

	registerMonitor("apm/batterycharge", "integer", printApmBatFill,
					printApmBatFillInfo);
	registerMonitor("apm/remainingtime", "integer", printApmBatTime,
					printApmBatTimeInfo);
}

void
exitApm(void)
{
	ApmOK = -1;
}

int
updateApm(void)
{
	/* ATTENTION: This function is called from a signal handler! Rules for
	 * signal handlers must be obeyed! */
	size_t n;
	int fd;

	if (ApmOK < 0)
		return (-1);

	if ((fd = open("/proc/apm", O_RDONLY)) < 0)
	{
		if (ApmOK != 0)
			print_error("Cannot open file \'/proc/apm\'!\n"
			   "The kernel needs to be compiled with support\n"
			   "for /proc filesystem enabled!\n");
		return (-1);
	}
	if ((n = read(fd, ApmBuf, APMBUFSIZE - 1)) == APMBUFSIZE - 1)
	{
		log_error("Internal buffer too small to read \'/proc/apm\'");
		return (-1);
	}
	close(fd);
	ApmBuf[n] = '\0';
	Dirty = 1;

	return (0);
}

void
printApmBatFill(const char* c)
{
	if (Dirty)
		processApm();

	fprintf(CurrentClient, "%d\n", BatFill);
}

void
printApmBatFillInfo(const char* c)
{
	fprintf(CurrentClient, "Battery charge\t0\t100\t%%\n");
}

void
printApmBatTime(const char* c)
{
	if (Dirty)
		processApm();

	fprintf(CurrentClient, "%d\n", BatTime);
}

void
printApmBatTimeInfo(const char* c)
{
	fprintf(CurrentClient, "Remaining battery time\t0\t0\tmin\n");
}
