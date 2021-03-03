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

	$Id: ProcessList.c,v 1.37 2001/07/26 16:00:14 cschlaeg Exp $
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/user.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <errno.h>

#include "ccont.h"
#include "Command.h"
#include "ProcessList.h"
#include "PWUIDCache.h"
#include "../../gui/SignalIDs.h"
#include "ksysguardd.h"

#ifndef PAGE_SIZE /* Needed for SPARC */
#include <asm/page.h>
#endif

#define BUFSIZE 1024
#define TAGSIZE 32
#define KDEINITLEN strlen("kdeinit: ")

static CONTAINER ProcessList = 0;
static time_t timeStamp = 0;

typedef struct
{
	/* This flag is set for all found processes at the beginning of the
	 * process list update. Processes that do not have this flag set will
	 * be assumed dead and removed from the list. The flag is cleared after
	 * each list update. */
	int alive;

	/* the process ID */
	pid_t pid;

	/* the parent process ID */
	pid_t ppid;

	/* the real user ID */
	uid_t uid;

	/* the real group ID */
	gid_t gid;

	/* a character description of the process status */
    char status[16];

	/* the number of the tty the process owns */
	int ttyNo;

	/*
	 * The nice level. The range should be -20 to 20. I'm not sure
	 * whether this is true for all platforms.
	 */
	int niceLevel;

	/*
	 * The scheduling priority.
	 */
	int priority;

	/*
	 * The total amount of memory the process uses. This includes shared and
	 * swapped memory.
	 */
	unsigned int vmSize;

	/*
	 * The amount of physical memory the process currently uses.
	 */
	unsigned int vmRss;

	/*
	 * The number of 1/100 of a second the process has spend in user space.
	 * If a machine has an uptime of 1 1/2 years or longer this is not a
	 * good idea. I never thought that the stability of UNIX could get me
	 * into trouble! ;)
	 */
	unsigned int userTime;

	/*
	 * The number of 1/100 of a second the process has spend in system space.
	 * If a machine has an uptime of 1 1/2 years or longer this is not a
	 * good idea. I never thought that the stability of UNIX could get me
	 * into trouble! ;)
	 */
	unsigned int sysTime;

	/* system time as multime of 100ms */
	int centStamp;

	/* the current CPU load (in %) from user space */
	double userLoad;

	/* the current CPU load (in %) from system space */
	double sysLoad;

	/* the name of the process */
	char name[64];

	/* the command used to start the process */
	char cmdline[256];

	/* the login name of the user that owns this process */
	char userName[32];
} ProcessInfo;

static unsigned ProcessCount;

static void
validateStr(char* str)
{
	char* s = str;

	/* All characters that could screw up the communication will be
	 * removed. */
	while (*s)
	{
		if (*s == '\t' || *s == '\n' || *s == '\r')
			*s = ' ';
		++s;
	}
	/* Make sure that string contains at least one character (blank). */
	if (str[0] == '\0')
		strcpy(str, " ");
}

static int 
processCmp(void* p1, void* p2)
{
	return (((ProcessInfo*) p1)->pid - ((ProcessInfo*) p2)->pid);
}

static ProcessInfo*
findProcessInList(int pid)
{
	ProcessInfo key;
	long index;

	key.pid = pid;
	if ((index = search_ctnr(ProcessList, processCmp, &key)) < 0)
		return (0);

	return (get_ctnr(ProcessList, index));
}

static int
updateProcess(int pid)
{
	ProcessInfo* ps;
	FILE* fd;
	char buf[BUFSIZE];
	char tag[TAGSIZE];
	char format[32];
	char tagformat[32];
	int userTime, sysTime;
	const char* uName;
	char status;

	if ((ps = findProcessInList(pid)) == 0)
	{
		struct timeval tv;

		ps = (ProcessInfo*) malloc(sizeof(ProcessInfo));
		ps->pid = pid;
		ps->alive = 0;

		gettimeofday(&tv, 0);
		ps->centStamp = tv.tv_sec * 100 + tv.tv_usec / 10000;

		push_ctnr(ProcessList, ps);
		bsort_ctnr(ProcessList, processCmp, 0);
	}

	snprintf(buf, BUFSIZE - 1, "/proc/%d/status", pid);
	if((fd = fopen(buf, "r")) == 0)
	{
		/* process has terminated in the mean time */
		return (-1);
	}

	sprintf(format, "%%%d[^\n]\n", (int) sizeof(buf) - 1);
	sprintf(tagformat, "%%%ds", (int) sizeof(tag) - 1);
	for (;;)
	{
		if (fscanf(fd, format, buf) != 1)
			break;
		buf[sizeof(buf) - 1] = '\0';
		sscanf(buf, tagformat, tag);
		tag[sizeof(tag) - 1] = '\0';
		if (strcmp(tag, "Name:") == 0)
		{
			sscanf(buf, "%*s %63s", ps->name);
			validateStr(ps->name);
		}
		else if (strcmp(tag, "Uid:") == 0)
		{
			sscanf(buf, "%*s %d %*d %*d %*d", (int*) &ps->uid);
		}
	}
	if (fclose(fd))
		return (-1);

        static const char *fmtString =
                "%*d " // PID
                "%*s " // (Comm)
                "%c " // State
                "%d " // Ppid
                "%d " // Pgrp
                "%*d " // Session
                "%*d "  // ttyno
                "%d " // tpgid
                "%*u " // flags
                "%*u " // minflt
                "%*u " // cminflt
                "%*u " // majflt
                "%*u " // cmajflt
                "%d "  // utime
                "%d "  // stime
                "%*d " // cutime
                "%*d " // cstime
                "%*d " // priority
                "%d "  // nice
                "%*u " // numthreads
                "%*u " // itrealvalue
                "%*llu " // starttime
                "%u"   // vsize
                "%d"   // rss
                //"%*lu"  // rsslim
                //"%*lu"  // startcode
                //"%*lu"  // endcode
                //"%*lu"  // startstack
                // ...
                ;

    snprintf(buf, BUFSIZE - 1, "/proc/%d/stat", pid);
	buf[BUFSIZE - 1] = '\0';
	if ((fd = fopen(buf, "r")) == 0)
		return (-1);

	if (fscanf(fd, fmtString,
			   &status, (int*) &ps->ppid, (int*) &ps->gid, &ps->ttyNo,
			   &userTime, &sysTime, &ps->niceLevel, &ps->vmSize,
			   &ps->vmRss) != 9)
		return (-1);
	if (fclose(fd))
		return (-1);

	/* status decoding as taken from fs/proc/array.c */
	if (status == 'R')
		strcpy(ps->status, "running");
	else if (status == 'S')
		strcpy(ps->status, "sleeping");
	else if (status == 'D')
		strcpy(ps->status, "disk sleep");
	else if (status == 'Z')
		strcpy(ps->status, "zombie");
	else if (status == 'T')
		strcpy(ps->status, "stopped");
	else if (status == 'W')
		strcpy(ps->status, "paging");
	else
		sprintf(ps->status, "Unknown: %c", status);

	ps->vmRss = (ps->vmRss + 3) * PAGE_SIZE;

	{
		int newCentStamp;
		int timeDiff, userDiff, sysDiff;
		struct timeval tv;

		gettimeofday(&tv, 0);
		newCentStamp = tv.tv_sec * 100 + tv.tv_usec / 10000;

		timeDiff = newCentStamp - ps->centStamp;
		userDiff = userTime - ps->userTime;
		sysDiff = sysTime - ps->sysTime;

		if ((timeDiff > 0) && (userDiff >= 0) && (sysDiff >= 0))
		{
			ps->userLoad = ((double) userDiff / timeDiff) * 100.0;
			ps->sysLoad = ((double) sysDiff / timeDiff) * 100.0;
			/* During startup we get bigger loads since the time diff
			 * cannot be correct. So we force it to 0. */
			if (ps->userLoad > 100.0)
				ps->userLoad = 0.0;
			if (ps->sysLoad > 100.0)
				ps->sysLoad = 0.0;
		}
		else
			ps->sysLoad = ps->userLoad = 0.0;

		ps->centStamp = newCentStamp;
		ps->userTime = userTime;
		ps->sysTime = sysTime;
	}

	snprintf(buf, BUFSIZE - 1, "/proc/%d/cmdline", pid);
	if ((fd = fopen(buf, "r")) == 0)
		return (-1);

	ps->cmdline[0] = '\0';
	sprintf(buf, "%%%d[^\n]", (int) sizeof(ps->cmdline) - 1);
	fscanf(fd, buf, ps->cmdline);
	ps->cmdline[sizeof(ps->cmdline) - 1] = '\0';
	validateStr(ps->cmdline);
	if (fclose(fd))
		return (-1);

	/* Ugly hack to "fix" program name for kdeinit launched programs. */
	if (strcmp(ps->name, "kdeinit") == 0 &&
		strncmp(ps->cmdline, "kdeinit: ", KDEINITLEN) == 0 &&
		strcmp(ps->cmdline + KDEINITLEN, "Running...") != 0)
	{
		size_t len;
		char* end = strchr(ps->cmdline + KDEINITLEN, ' ');
		if (end)
			len = (end - ps->cmdline) - KDEINITLEN;
		else
			len = strlen(ps->cmdline + KDEINITLEN);
		if (len > 0)
		{
			if (len > sizeof(ps->name) - 1)
				len = sizeof(ps->name) - 1;
			strncpy(ps->name, ps->cmdline + KDEINITLEN, len);
			ps->name[len] = '\0';
		}
	}

	/* find out user name with the process uid */
	uName = getCachedPWUID(ps->uid);
	strncpy(ps->userName, uName, sizeof(ps->userName) - 1);
	ps->userName[sizeof(ps->userName) - 1] = '\0';
	validateStr(ps->userName);

	ps->alive = 1;

	return (0);
}

static void
cleanupProcessList(void)
{
	int i;

	ProcessCount = 0;
	/* All processes that do not have the active flag set are assumed dead
	 * and will be removed from the list. The alive flag is cleared. */
	for (i = 0; i < level_ctnr(ProcessList); i++)
	{
		ProcessInfo* ps = get_ctnr(ProcessList, i);
		if (ps->alive)
		{
			/* Process is still alive. Just clear flag. */
			ps->alive = 0;
			ProcessCount++;
		}
		else
		{
			/* Process has probably died. We remove it from the list and
			 * destruct the data structure. i needs to be decremented so
			 * that after i++ the next list element will be inspected. */
			free(remove_ctnr(ProcessList, i--));
		}
	}
}

static int
updateProcessList(void)
{
	/* There no way that this function can avoid the use of
	 * non-reentrant glibc functions. We cannot use it from within
	 * signal handlers. So we use the timeStamp variable to find out
	 * if the last update was more than 2 seconds ago and refresh the
	 * list by calling this function. */
	DIR* dir;
	struct dirent* entry;

	/* read in current process list via the /proc filesystem entry */
	if ((dir = opendir("/proc")) == NULL)
	{
		print_error("Cannot open directory \'/proc\'!\n"
			   "The kernel needs to be compiled with support\n"
			   "for /proc filesystem enabled!\n");
		return (-1);
	}
	while ((entry = readdir(dir))) 
	{
		if (isdigit(entry->d_name[0]))
		{
			int pid;

			pid = atoi(entry->d_name);
			updateProcess(pid); 
		}
	}
	closedir(dir);

	cleanupProcessList();

	timeStamp = time(0);

	return (0);
}

/*
================================ public part =================================
*/

void
initProcessList(void)
{
	initPWUIDCache();

	ProcessList = new_ctnr(CT_DLL);

	registerMonitor("pscount", "integer", printProcessCount,
					printProcessCountInfo);
	registerMonitor("ps", "table", printProcessList, printProcessListInfo);

	if (!RunAsDaemon)
	{
		registerCommand("kill", killProcess);
		registerCommand("setpriority", setPriority);
	}
}

void
exitProcessList(void)
{
	if (ProcessList)
		destr_ctnr(ProcessList, free);
	exitPWUIDCache();
}

void
printProcessListInfo(const char* cmd)
{
	fprintf(CurrentClient, "Name\tPID\tPPID\tUID\tGID\tStatus\tUser%%\tSystem%%\tNice\tVmSize"
		   "\tVmRss\tLogin\tCommand\n");
	fprintf(CurrentClient, "s\td\td\td\td\tS\tf\tf\td\td\td\ts\ts\n");
}

void
printProcessList(const char* cmd)
{
	int i;

	if ((time(0) - timeStamp) >= 2)
		updateProcessList();

	for (i = 0; i < level_ctnr(ProcessList); i++)
	{
		ProcessInfo* ps = get_ctnr(ProcessList, i);

		fprintf(CurrentClient, "%s\t%ld\t%ld\t%ld\t%ld\t%s\t%.2f\t%.2f\t%d\t%d\t%d"
			   "\t%s\t%s\n",
			   ps->name, (long) ps->pid, (long) ps->ppid,
			   (long) ps->uid, (long) ps->gid, ps->status,
			   ps->userLoad, ps->sysLoad, ps->niceLevel, 
			   ps->vmSize / 1024, ps->vmRss / 1024,
			   ps->userName, ps->cmdline);
	}

	fprintf(CurrentClient, "\n");
}

void
printProcessCount(const char* cmd)
{
	if ((time(0) - timeStamp) >= 2)
		updateProcessList();

	fprintf(CurrentClient, "%d\n", ProcessCount);
}

void
printProcessCountInfo(const char* cmd)
{
	fprintf(CurrentClient, "Number of Processes\t0\t0\t\n");
}

void
killProcess(const char* cmd)
{
	int sig, pid;

	sscanf(cmd, "%*s %d %d", &pid, &sig);
	switch(sig)
	{
	case MENU_ID_SIGABRT:
		sig = SIGABRT;
		break;
	case MENU_ID_SIGALRM:
		sig = SIGALRM;
		break;
	case MENU_ID_SIGCHLD:
		sig = SIGCHLD;
		break;
	case MENU_ID_SIGCONT:
		sig = SIGCONT;
		break;
	case MENU_ID_SIGFPE:
		sig = SIGFPE;
		break;
	case MENU_ID_SIGHUP:
		sig = SIGHUP;
		break;
	case MENU_ID_SIGILL:
		sig = SIGILL;
		break;
	case MENU_ID_SIGINT:
		sig = SIGINT;
		break;
	case MENU_ID_SIGKILL:
		sig = SIGKILL;
		break;
	case MENU_ID_SIGPIPE:
		sig = SIGPIPE;
		break;
	case MENU_ID_SIGQUIT:
		sig = SIGQUIT;
		break;
	case MENU_ID_SIGSEGV:
		sig = SIGSEGV;
		break;
	case MENU_ID_SIGSTOP:
		sig = SIGSTOP;
		break;
	case MENU_ID_SIGTERM:
		sig = SIGTERM;
		break;
	case MENU_ID_SIGTSTP:
		sig = SIGTSTP;
		break;
	case MENU_ID_SIGTTIN:
		sig = SIGTTIN;
		break;
	case MENU_ID_SIGTTOU:
		sig = SIGTTOU;
		break;
	case MENU_ID_SIGUSR1:
		sig = SIGUSR1;
		break;
	case MENU_ID_SIGUSR2:
		sig = SIGUSR2;
		break;
	}
	if (kill((pid_t) pid, sig))
	{
		switch(errno)
		{
		case EINVAL:
			fprintf(CurrentClient, "4\t%d\n", pid);
			break;
		case ESRCH:
			fprintf(CurrentClient, "3\t%d\n", pid);
			break;
		case EPERM:
			fprintf(CurrentClient, "2\t%d\n", pid);
			break;
		default:
			fprintf(CurrentClient, "1\t%d\n", pid);	/* unkown error */
			break;
		}

	}
	else
		fprintf(CurrentClient, "0\t%d\n", pid);
}

void
setPriority(const char* cmd)
{
	int pid, prio;

	sscanf(cmd, "%*s %d %d", &pid, &prio);
	if (setpriority(PRIO_PROCESS, pid, prio))
	{
		switch(errno)
		{
		case EINVAL:
			fprintf(CurrentClient, "4\n");
			break;
		case ESRCH:
			fprintf(CurrentClient, "3\n");
			break;
		case EPERM:
		case EACCES:
			fprintf(CurrentClient, "2\n");
			break;
		default:
			fprintf(CurrentClient, "1\n");	/* unkown error */
			break;
		}
	}
	else
		fprintf(CurrentClient, "0\n");
}
