/*
    KTop, the KDE Task Manager

	Copyright (c) 1999-2000 Hans Petter Bieker<bieker@kde.org>
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

	$Id: ProcessList.c,v 1.5.2.1 2001/11/08 10:48:51 tokoe Exp $
*/

#include <ctype.h>
#include <dirent.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#if __FreeBSD_version > 500015
#include <sys/priority.h>
#endif
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/user.h>
#include <unistd.h>

#include "../../gui/SignalIDs.h"
#include "Command.h"
#include "ProcessList.h"
#include "ccont.h"
#include "ksysguardd.h"

CONTAINER ProcessList = 0;

#define BUFSIZE 1024

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
	 * The amount of memory (shared/swapped/etc) the process shares with
	 * other processes.
	 */
	unsigned int vmLib;

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
	static char *statuses[] = { "idle","run","sleep","stop","zombie" };
	
	ProcessInfo* ps;
	struct passwd* pwent;
	int mib[4];
	struct kinfo_proc p;
	size_t len;

	if ((ps = findProcessInList(pid)) == 0)
	{
		ps = (ProcessInfo*) malloc(sizeof(ProcessInfo));
		ps->pid = pid;
		ps->centStamp = 0;
		push_ctnr(ProcessList, ps);
		bsort_ctnr(ProcessList, processCmp, 0);
	}

	ps->alive = 1;

	mib[0] = CTL_KERN;
	mib[1] = KERN_PROC;
	mib[2] = KERN_PROC_PID;
	mib[3] = pid;

	len = sizeof (p);
	if (sysctl(mib, 4, &p, &len, NULL, 0) == -1 || !len)
		return -1;

#if __FreeBSD_version >= 500015
        ps->pid       = p.ki_pid;
        ps->ppid      = p.ki_ppid;
        ps->uid       = p.ki_uid;    
        ps->gid       = p.ki_pgid;
        ps->priority  = p.ki_pri.pri_user;
        ps->niceLevel = p.ki_nice;
#else
        ps->pid       = p.kp_proc.p_pid;
        ps->ppid      = p.kp_eproc.e_ppid;
        ps->uid       = p.kp_eproc.e_ucred.cr_uid;
        ps->gid       = p.kp_eproc.e_pgid;
        ps->priority  = p.kp_proc.p_priority;
        ps->niceLevel = p.kp_proc.p_nice;
#endif

        /* this isn't usertime -- it's total time (??) */
#if __FreeBSD_version >= 500015
        ps->userTime = p.ki_runtime / 10000;
#elif __FreeBSD_version >= 300000
        ps->userTime = p.kp_proc.p_runtime / 10000;
#else
	ps->userTime = p.kp_proc.p_rtime.tv_sec*100+p.kp_proc.p_rtime.tv_usec/100;
#endif
        ps->sysTime  = 0;
        ps->sysLoad  = 0;

        /* memory, process name, process uid */
	/* find out user name with process uid */
	pwent = getpwuid(ps->uid);
	strncpy(ps->userName,pwent&&pwent->pw_name? pwent->pw_name:"????",sizeof(ps->userName));
	ps->userName[sizeof(ps->userName)-1]='\0';

#if __FreeBSD_version >= 500015
        ps->userLoad = p.ki_pctcpu / 100;
	ps->vmSize   = (p.ki_vmspace->vm_tsize +
			p.ki_vmspace->vm_dsize +
			p.ki_vmspace->vm_ssize) * getpagesize();
	ps->vmRss    = p.ki_vmspace->vm_rssize * getpagesize();
	strncpy(ps->name,p.ki_comm? p.ki_comm:"????",sizeof(ps->name));
	strcpy(ps->status,(p.ki_stat>=1)&&(p.ki_stat<=5)? statuses[p.ki_stat-1]:"????");
#else
        ps->userLoad = p.kp_proc.p_pctcpu / 100;
	ps->vmSize   = (p.kp_eproc.e_vm.vm_tsize +
			p.kp_eproc.e_vm.vm_dsize +
			p.kp_eproc.e_vm.vm_ssize) * getpagesize();
	ps->vmRss    = p.kp_eproc.e_vm.vm_rssize * getpagesize();
	strncpy(ps->name,p.kp_proc.p_comm ? p.kp_proc.p_comm : "????", sizeof(ps->name));
	strcpy(ps->status,(p.kp_proc.p_stat>=1)&&(p.kp_proc.p_stat<=5)? statuses[p.kp_proc.p_stat-1]:"????");
#endif

        /* process command line */
	/* the following line causes segfaults on some FreeBSD systems... why?
		strncpy(ps->cmdline, p.kp_proc.p_args->ar_args, sizeof(ps->cmdline) - 1);
	*/
	strcpy(ps->cmdline, "????");

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

/*
================================ public part ==================================
*/

void
initProcessList(void)
{
	ProcessList = new_ctnr(CT_DLL);

	registerMonitor("ps", "table", printProcessList, printProcessListInfo);
	registerMonitor("pscount", "integer", printProcessCount, printProcessCountInfo);

	if (!RunAsDaemon)
	{
		registerCommand("kill", killProcess);
		registerCommand("setpriority", setPriority);
	}

	updateProcessList();
}

void
exitProcessList(void)
{
	removeMonitor("ps");
	removeMonitor("pscount");

	if (ProcessList)
		free (ProcessList);
}

int
updateProcessList(void)
{
        int mib[3];
        size_t len;
        size_t num;
        struct kinfo_proc *p;


        mib[0] = CTL_KERN;
        mib[1] = KERN_PROC;
        mib[2] = KERN_PROC_ALL;
        sysctl(mib, 3, NULL, &len, NULL, 0);
	p = malloc(len);
        sysctl(mib, 3, p, &len, NULL, 0);

	for (num = 0; num < len / sizeof(struct kinfo_proc); num++)
#if __FreeBSD_version >= 500015
		updateProcess(p[num].ki_pid);
#else
		updateProcess(p[num].kp_proc.p_pid);
#endif
	free(p);
	cleanupProcessList();

	return (0);
}

void
printProcessListInfo(const char* cmd)
{
	fprintf(CurrentClient, "Name\tPID\tPPID\tUID\tGID\tStatus\tUser%%\tSystem%%\tNice\tVmSize\tVmRss\tLogin\tCommand\n");
	fprintf(CurrentClient, "s\td\td\td\td\tS\tf\tf\td\td\td\ts\ts\n");
}

void
printProcessList(const char* cmd)
{
	int i;

	for (i = 1; i < level_ctnr(ProcessList); i++)
	{
		ProcessInfo* ps = get_ctnr(ProcessList, i);

		fprintf(CurrentClient, "%s\t%ld\t%ld\t%ld\t%ld\t%s\t%.2f\t%.2f\t%d\t%d\t%d\t%s\t%s\n",
			   ps->name, (long)ps->pid, (long)ps->ppid,
			   (long)ps->uid, (long)ps->gid, ps->status,
			   ps->userLoad, ps->sysLoad, ps->niceLevel,
			   ps->vmSize / 1024, ps->vmRss / 1024, ps->userName, ps->cmdline);
	}
}

void
printProcessCount(const char* cmd)
{
	fprintf(CurrentClient, "%d\n", ProcessCount);
}

void
printProcessCountInfo(const char* cmd)
{
	fprintf(CurrentClient, "Number of Processes\t1\t65535\t\n");
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
