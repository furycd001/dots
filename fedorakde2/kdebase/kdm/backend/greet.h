/* $TOG: greet.h /main/5 1998/02/09 13:55:28 kaleb $ */
/* $Id: greet.h,v 1.19 2001/07/24 19:28:38 ossi Exp $ */
/*

Copyright 1994, 1998  The Open Group

All Rights Reserved.

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from The Open Group.

*/
/* $XFree86: xc/programs/xdm/greet.h,v 1.5 2000/05/31 07:15:11 eich Exp $ */

/*
 * greet.h - interface to xdm's external greeter and config reader
 */

#ifndef GREET_H
#define GREET_H

#define DEBUG_CORE	0x01
#define DEBUG_CONFIG	0x02
#define DEBUG_GREET	0x04
#define DEBUG_HLPCON	0x08
#define DEBUG_WSESS	0x10
#define DEBUG_WCONFIG	0x20
#define DEBUG_WGREET	0x40
#define DEBUG_AUTH	0x100
#define DEBUG_NOFORK	0x200

#ifndef TRUE
# define TRUE	1
# define FALSE	0
#endif

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
# define ATTR_UNUSED __attribute__((unused))
# define ATTR_NORETURN __attribute__((noreturn))
# define ATTR_PRINTFLIKE(fmt,var) __attribute__((format(printf,fmt,var)))
#else
# define ATTR_UNUSED
# define ATTR_NORETURN
# define ATTR_PRINTFLIKE(fmt,var)
#endif

#define as(ar) ((int)(sizeof(ar)/sizeof(ar[0])))

/*
 * Exit codes for fork()ed session process and greeter
 */
#define EX_NORMAL		0	/* do whatever seems appropriate */
#define EX_REMANAGE_DPY		1	/* force remanage */
#define EX_UNMANAGE_DPY		2	/* force deletion */
#define EX_RESERVER_DPY		3	/* force server termination */
#define EX_AL_RESERVER_DPY	4	/* reserver; maybe, auto-(re-)login */
#define EX_OPENFAILED_DPY	5	/* XOpenDisplay failed, retry */
#define EX_TEXTLOGIN		6	/* start console login */
#define EX_REBOOT		7	/* shutdown & reboot */
#define EX_HALT			8	/* shutdown & halt/poweroff */

/*
 * Command codes greeter -> core
 */
#define G_SessionExit	2	/* int code; async */
#define G_Verify	3	/* str name, str pass; int V_ret */
#define G_Restrict	4	/* str name; <variable> */
#define G_Login		5	/* str name, str pass, argv sessargs; async */
#define G_GetCfg	6	/* int what; int sts, <variable>  */
#define G_GetSessArg	7	/* str user; argv sessargs */
#define G_SetupDpy	8	/* ; async */

/*
 * Command codes core -> config reader
 */
#define GC_Files	1	/* get file list */
#define GC_GetConf	2	/* get a config group */
# define GC_gGlobal	1	/* get global config array */
# define GC_gXservers	2	/* get Xservers equivalent */
# define GC_gXaccess	3	/* get Xaccess equivalent */
# define GC_gDisplay	4	/* get per-display config array */

/*
 * Error code core -> greeter
 */
#define GE_Ok		0
#define GE_NoFkt	1	/* no such function (only for extensions!) */
#define GE_NoEnt	2	/* no such config entry */
#define GE_BadType	3	/* unknown config entry type */

/*
 * Log levels.
 * Used independently in core, greeter & config reader.
 */
#define DM_DEBUG	0
#define DM_INFO		1
#define DM_ERR		2
#define DM_PANIC	3

/*
 * Return codes of Verify, Restrict and StartSession
 */
#define V_ERROR		0
#define V_AUTH		1
#define V_NOHOME	2
#define V_NOLOGIN	3
#define V_NOROOT	4
#define V_BADSHELL	5
#define V_BADTIME	6
#define V_AEXPIRED	7
#define V_PEXPIRED	8
#define V_MSGERR	9
#define V_OK		100
#define V_AWEXPIRE	101
#define V_PWEXPIRE	102
#define V_MSGINFO	103

/*
 * Config/Runtime data keys
 */
#define C_WHO_MASK	  0x00ff0000	/* Non-zero for proprietary extensions (see manufacturer table [to be written]) */
#define C_TYPE_MASK	  0x0f000000	/* Type of the value */
# define C_TYPE_INT	  0x00000000	/*  Integer */
# define C_TYPE_STR	  0x01000000	/*  String */
# define C_TYPE_ARGV	  0x02000000	/*  0-terminated Array of Strings */
# define C_TYPE_ARR	  0x03000000	/*  Array (only when XDCMP is enabled) */
#define C_PRIVATE	  0xf0000000	/* Private, don't make it visible to interfaces! */

/* global config */

#define C_daemonMode		(C_TYPE_INT | 0x000)

#define C_autoLogin		(C_TYPE_INT | 0x003)

#define C_autoRescan		(C_TYPE_INT | 0x004)

#define C_randomFile		(C_TYPE_STR | 0x005)

#define C_exportList		(C_TYPE_ARGV | 0x006)

#define C_cmdHalt		(C_TYPE_STR | 0x007)
#define C_cmdReboot		(C_TYPE_STR | 0x008)

#define C_pidFile		(C_TYPE_STR | 0x009)
#define C_lockPidFile		(C_TYPE_INT | 0x00a)

#define C_authDir		(C_TYPE_STR | 0x00b)

#define C_requestPort		(C_TYPE_INT | 0x00c)

#define C_sourceAddress		(C_TYPE_INT | 0x00d)
#define C_removeDomainname	(C_TYPE_INT | 0x00e)

#define C_choiceTimeout		(C_TYPE_INT | 0x00f)

#define C_keyFile		(C_TYPE_STR | 0x010)

#define C_willing		(C_TYPE_STR | 0x011)

#define C_PAMService		(C_TYPE_STR | 0x012)

#define C_servers		(C_TYPE_STR | 0x0fe)	/* XXX kill! */
#define C_accessFile		(C_TYPE_STR | 0x0ff)

/* per-display config */

#define C_serverAttempts	(C_TYPE_INT | 0x100)
#define C_serverTimeout		(C_TYPE_INT | 0x101)
#define C_openDelay		(C_TYPE_INT | 0x102)
#define C_openRepeat		(C_TYPE_INT | 0x103)
#define C_openTimeout		(C_TYPE_INT | 0x104)
#define C_startAttempts		(C_TYPE_INT | 0x105)
#define C_startInterval		(C_TYPE_INT | 0x115)
#define C_pingInterval		(C_TYPE_INT | 0x106)
#define C_pingTimeout		(C_TYPE_INT | 0x107)	
#define C_terminateServer	(C_TYPE_INT | 0x108)
#define C_resetSignal		(C_TYPE_INT | 0x10b)	
#define C_termSignal		(C_TYPE_INT | 0x10c)	
#define C_resetForAuth		(C_TYPE_INT | 0x10d)
#define C_authorize		(C_TYPE_INT | 0x10e)
#define C_authNames		(C_TYPE_ARGV | 0x110)
#define C_clientAuthFile	(C_TYPE_STR | 0x111)
#define C_fifoCreate		(C_TYPE_INT | 0x112)
#define C_fifoOwner		(C_TYPE_INT | 0x113)	
#define C_fifoGroup		(C_TYPE_INT | 0x114)	
#define C_resources		(C_TYPE_STR | 0x116)
#define C_xrdb			(C_TYPE_STR | 0x117)
#define C_setup			(C_TYPE_STR | 0x118)
#define C_startup		(C_TYPE_STR | 0x119)
#define C_reset			(C_TYPE_STR | 0x11a)
#define C_session		(C_TYPE_STR | 0x11b)
#define C_userPath		(C_TYPE_STR | 0x11c)
#define C_systemPath		(C_TYPE_STR | 0x11d)
#define C_systemShell		(C_TYPE_STR | 0x11e)
#define C_failsafeClient	(C_TYPE_STR | 0x11f)
#define C_userAuthDir		(C_TYPE_STR | 0x120)
#define C_chooser		(C_TYPE_STR | 0x122)	/* XXX vaporize */
#define C_noPassUsers		(C_TYPE_ARGV | 0x123)
#define C_autoUser		(C_TYPE_STR | 0x124)
#define C_autoPass		(C_TYPE_STR | 0x125)
#define C_autoString		(C_TYPE_STR | 0x126)
#define C_autoLogin1st		(C_TYPE_INT | 0x127)
#define C_autoReLogin		(C_TYPE_INT | 0x128)
#define C_allowNullPasswd	(C_TYPE_INT | 0x129)
#define C_allowRootLogin	(C_TYPE_INT | 0x12a)
#define C_sessSaveFile		(C_TYPE_STR | 0x12b)

/* display variables */
#define C_name			(C_TYPE_STR | 0x200)
#define C_class			(C_TYPE_STR | 0x201)
#define C_displayType		(C_TYPE_INT | 0x202)
#define C_serverArgv		(C_TYPE_ARGV | 0x203)
#define C_serverPid		(C_TYPE_INT | 0x204)
#define C_sessionID		(C_TYPE_INT | 0x205)
#define C_peer			(C_TYPE_ARR | 0x206)
#define C_from			(C_TYPE_ARR | 0x207)
#define C_displayNumber		(C_TYPE_INT | 0x208)
#define C_useChooser		(C_TYPE_INT | 0x209)
#define C_clientAddr		(C_TYPE_ARR | 0x20a)
#define C_connectionType	(C_TYPE_INT | 0x20b)
#define C_console		(C_TYPE_STR | 0x20c)

/**
 ** Various parts of struct display
 **/

/*
 * local     - server runs on local host
 * foreign   - server runs on remote host
 * permanent - session restarted when it exits
 * transient - session not restarted when it exits
 * fromFile  - started via entry in servers file
 * fromXDMCP - started with XDMCP
 */

#define d_location	1
#define Local		1
#define Foreign		0

#define d_lifetime	2
#define Permanent	2
#define Transient	0

#define d_origin	4
#define FromFile	4
#define FromXDMCP	0


#endif /* GREET_H */
