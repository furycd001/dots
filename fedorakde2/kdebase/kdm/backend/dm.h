/* $TOG: dm.h /main/67 1998/02/09 13:55:01 kaleb $ */
/* $Id: dm.h,v 1.32.2.3 2001/10/25 09:51:15 ossi Exp $ */
/*

Copyright 1988, 1998  The Open Group

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
/* $XFree86: xc/programs/xdm/dm.h,v 3.19 2000/06/14 00:16:14 dawes Exp $ */

/*
 * xdm - display manager daemon
 * Author:  Keith Packard, MIT X Consortium
 *
 * dm.h
 *
 * global xdm core declarations
 */

#ifndef _DM_H_
#define _DM_H_ 1

#ifdef MINIX
# ifdef MNX_TCPCONN
#  define TCPCONN
# endif
#endif /* MINIX */

#include "greet.h"

#include <X11/Xos.h>
#include <X11/Xfuncs.h>
#include <X11/Xmd.h>
#include <X11/Xauth.h>
#include <X11/Intrinsic.h>

#if defined(X_POSIX_C_SOURCE)
# define _POSIX_C_SOURCE X_POSIX_C_SOURCE
# include <setjmp.h>
# include <limits.h>
# undef _POSIX_C_SOURCE
#else
# if defined(X_NOT_POSIX) || defined(_POSIX_SOURCE)
#  include <setjmp.h>
#  include <limits.h>
# else
#  define _POSIX_SOURCE
#  include <setjmp.h>
#  include <limits.h>
#  undef _POSIX_SOURCE
# endif
#endif

#ifdef X_NOT_STDC_ENV
# define Time_t long
extern Time_t time ();
#else
# include <time.h>
# define Time_t time_t
#endif

#ifdef XDMCP
# include <X11/Xdmcp.h>
#endif

#ifdef pegasus
# undef dirty		/* Some bozo put a macro called dirty in sys/param.h */
#endif

#ifndef X_NOT_POSIX
# ifdef _POSIX_SOURCE
#  include <sys/wait.h>
# else
#  define _POSIX_SOURCE
#  ifdef SCO325
#   include <sys/procset.h>
#   include <sys/siginfo.h>
#  endif
#  include <sys/wait.h>
#  undef _POSIX_SOURCE
# endif
# define waitCode(w)	(WIFEXITED(w) ? WEXITSTATUS(w) : 0)
# define waitSig(w)	(WIFSIGNALED(w) ? WTERMSIG(w) : 0)
#ifdef WCOREDUMP
# define waitCore(w)    (WCOREDUMP(w))
#else
# define waitCore(w)    0	/* not in POSIX.  so what? */
#endif
typedef int		waitType;
#else /* X_NOT_POSIX */
# ifdef SYSV
#  define waitCode(w)	(((w) >> 8) & 0x7f)
#  define waitSig(w)	((w) & 0xff)
#  define waitCore(w)	(((w) >> 15) & 0x01)
typedef int		waitType;
# else /* SYSV */
#  include <sys/wait.h>
#  define waitCode(w)	((w).w_T.w_Retcode)
#  define waitSig(w)	((w).w_T.w_Termsig)
#  define waitCore(w)	((w).w_T.w_Coredump)
typedef union wait	waitType;
# endif
#endif /* X_NOT_POSIX */

#define waitCompose(sig,core,code) ((sig) * 256 + (core) * 128 + (code))
#define waitVal(w) waitCompose(waitSig(w), waitCore(w), waitCode(w))
#define WaitCode(w) ((w) & 0x7f)
#define WaitCore(w) (((w) >> 7) & 1)
#define WaitSig(w) (((w) >> 8) & 0xff)

#ifndef FD_ZERO
typedef	struct	my_fd_set { int fds_bits[1]; } my_fd_set;
# define FD_ZERO(fdp)	bzero ((fdp), sizeof (*(fdp)))
# define FD_SET(f,fdp)	((fdp)->fds_bits[(f) / (sizeof (int) * 8)] |=  (1 << ((f) % (sizeof (int) * 8))))
# define FD_CLR(f,fdp)	((fdp)->fds_bits[(f) / (sizeof (int) * 8)] &= ~(1 << ((f) % (sizeof (int) * 8))))
# define FD_ISSET(f,fdp)	((fdp)->fds_bits[(f) / (sizeof (int) * 8)] & (1 << ((f) % (sizeof (int) * 8))))
# define FD_TYPE	my_fd_set
#else
# define FD_TYPE	fd_set
#endif

#if defined(X_NOT_POSIX) && defined(SIGNALRETURNSINT)
# define SIGVAL int
#else
# define SIGVAL void
#endif

#if defined(X_NOT_POSIX) || defined(__EMX__) || defined(__NetBSD__) && defined(__sparc__)
# if defined(SYSV) || defined(__EMX__)
#  define SIGNALS_RESET_WHEN_CAUGHT
#  define UNRELIABLE_SIGNALS
# endif
# define Setjmp(e)	setjmp(e)
# define Longjmp(e,v)	longjmp(e,v)
# define Jmp_buf	jmp_buf
#else
# define Setjmp(e)	sigsetjmp(e,1)
# define Longjmp(e,v)	siglongjmp(e,v)
# define Jmp_buf	sigjmp_buf
#endif


typedef enum displayStatus { running, notRunning, zombie, phoenix, suspended } DisplayStatus;

typedef struct RcStr {
    struct RcStr	*next;
    char		*str;
    int			cnt;
} RcStr;

typedef struct CfgDep {
    RcStr *name;
    long time;
} CfgDep;

typedef struct CfgArr {
	char		*data;		/* config value array; allocated */
	int		*idx;		/* config index array; alias */
	CfgDep		dep;		/* filestamp */
    	int		numCfgEnt;	/* number of config entries */
} CfgArr;

struct display {
	struct display	*next;
	struct disphist	*hstent;	/* display history entry */

	/* Xservers file / XDMCP information */
	char		*name;		/* DISPLAY name -- also referenced in hstent */
	char		*class2;	/* display class (may be NULL) */
	int		displayType;	/* location/origin/lifetime */
	char		**serverArgv;	/* server program and arguments */
	CfgArr		cfg;		/* config data array */

	/* display state */
	DisplayStatus	status;		/* current status */
	int		pid;		/* process id of child */
	int		serverPid;	/* process id of server (-1 if none) */
	int		stillThere;	/* state during HUP processing */
	Display		*dpy;		/* connection to X server */
	struct verify_info *verify;	/* info about logged in user */
	int		fifofd;		/* fifo for login after logout */
	int		pipefd[2];	/* pipe for re-login after crash */
#ifdef XDMCP
	/* XDMCP state */
	unsigned	sessionID;	/* ID of active session */
	ARRAY8		peer;		/* display peer address */
	ARRAY8		from;		/* XDMCP port of display */
	unsigned	displayNumber;	/* numerical part of name */
	int		useChooser;	/* Run the chooser for this display */
	ARRAY8		clientAddr;	/* for chooser picking */
	unsigned	connectionType;	/* ... */
#endif

	/* server management resources */
	int		serverAttempts;	/* number of attempts at running X */
	int		serverTimeout;	/* how long to wait for X */
	int		startInterval;	/* reset startAttempts after this time */
	int		openDelay;	/* server{Timeout,Delay} fit better */
	int		openRepeat;	/* connection open attempts to make */
	int		openTimeout;	/* abort open attempt timeout */
	int		startAttempts;	/* number of attempts at starting */
	int		pingInterval;	/* interval between XSync */
	int		pingTimeout;	/* timeout for XSync */
	int		terminateServer;/* restart for each session */
	int		resetSignal;	/* signal to reset server */
	int		termSignal;	/* signal to terminate server */
	int		resetForAuth;	/* server reads auth file at reset */

	int		fifoCreate;	/* create a login data fifo */
	int		fifoOwner;	/* owner of fifo */
	int		fifoGroup;	/* group of fifo */
	char		*console;	/* the tty line hidden by the server */

	/* session resources */
	char		*resources;	/* resource file */
	char		*xrdb;		/* xrdb program */
	char		*setup;		/* Xsetup program */
	char		*startup;	/* Xstartup program */
	char		*reset;		/* Xreset program */
	char		*session;	/* Xsession program */
	char		*userPath;	/* path set for session */
	char		*systemPath;	/* path set for startup/reset */
	char		*systemShell;	/* interpreter for startup/reset */
	char		*failsafeClient;/* a client to start when the session fails */
	char		*chooser;	/* chooser program XXX kill! */

	int		autoReLogin;	/* auto-re-login after crash */
	int		autoLogin1st;	/* auto-login at startup? */
	char		*autoUser;	/* user to log in automatically. */
	char		*autoPass;	/* his password. only for krb5 & sec_rpc */
	char		*autoString;	/* xsession arguments. */
	char		**noPassUsers;	/* users allowed in without a password */
	char		*sessSaveFile;	/* rel. file name where previous session is saved */

	int		allowNullPasswd;/* allow null password on login */
	int		allowRootLogin;	/* allow direct root login */

	/* authorization resources */
	int		authorize;	/* enable authorization */
	char		**authNames;	/* authorization protocol names */
	unsigned short	*authNameLens;	/* authorization protocol name lens */
	char		*clientAuthFile;/* client specified auth file */
	char		*userAuthDir;	/* backup directory for tickets */

	/* information potentially derived from resources */
	int		authNameNum;	/* number of protocol names */
	Xauth		**authorizations;/* authorization data */
	int		authNum;	/* number of authorizations */
	char		*authFile;	/* file to store authorization in */

};

struct disphist {
	struct disphist	*next;
	char		*name;
	Time_t		lastStart;	/* time of last display start */
	Time_t		lastExit;	/* time of last display exit */
	int		goodExit;	/* was the last exit "peaceful"? */
	int		startTries;	/* current start try */
	char		*nLogPipe;	/* data read from fifo */
};


struct verify_info {
	int		uid;		/* user id */
	int		gid;		/* group id */
	char		*user;		/* name of the user */
	char		**userEnviron;	/* environment for session */
	char		**systemEnviron;/* environment for startup/reset */
};

#ifdef XDMCP

#define PROTO_TIMEOUT	(30 * 60)   /* 30 minutes should be long enough */

struct protoDisplay {
	struct protoDisplay	*next;
	XdmcpNetaddr		address;   /* UDP address */
	int			addrlen;    /* UDP address length */
	unsigned long		date;	    /* creation date */
	CARD16			displayNumber;
	CARD16			connectionType;
	ARRAY8			connectionAddress;
	CARD32			sessionID;
	Xauth			*fileAuthorization;
	Xauth			*xdmcpAuthorization;
	ARRAY8			authenticationName;
	ARRAY8			authenticationData;
	XdmAuthKeyRec		key;
};
#endif /* XDMCP */

extern char	*config;
extern char	*config2Parse;

extern char	*servers;
extern int	request_port;
extern int	debugLevel;
extern int	daemonMode;
extern char	*pidFile;
extern int	lockPidFile;
extern char	*authDir;
extern int	autoRescan;
extern int	removeDomainname;
extern char	*keyFile;
extern char	*accessFile;
extern char	**exportList;
extern char	*randomFile;
extern char	*willing;
extern int	choiceTimeout;	/* chooser choice timeout */
extern int	autoLogin;
extern char	*cmdHalt;
extern char	*cmdReboot;
extern char	*PAMService;

/* in daemon.c */
extern void BecomeDaemon (void);

/* in dm.c */
extern char *prog, *progpath;
extern void StartDisplay (struct display *d);
extern void SetTitle (char *name, ...);

/* in dpylist.c */
extern int AnyDisplaysLeft (void);
extern void ForEachDisplay (void (*f)(struct display *));
extern void RemoveDisplay (struct display *old);
extern struct display
	*FindDisplayByName (char *name),
#ifdef XDMCP
	*FindDisplayBySessionID (CARD32 sessionID),
	*FindDisplayByAddress (XdmcpNetaddr addr, int addrlen, CARD16 displayNumber),
#endif /* XDMCP */
	*FindDisplayByPid (int pid),
	*FindDisplayByServerPid (int serverPid),
	*NewDisplay (char *name, char *class2);

/* in file.c */
extern void ParseDisplay (char *source);

/* in reset.c */
extern void pseudoReset (Display *dpy);

/* in resource.c */
extern char **FindCfgEnt (struct display *d, int id);
extern int InitResources (char **argv);
extern int LoadDMResources (int force);
extern int LoadDisplayResources (struct display *d);
extern void CloseGetter (void);
extern RcStr *newStr (char *str);
extern void delStr (RcStr *str);

/* in session.c */
extern char **defaultEnv (char *user);
extern char **inheritEnv (char **env, char **what);
extern char **systemEnv (struct display *d, char *user, char *home);
extern int source (char **environ, char *file);
extern void DeleteXloginResources (struct display *d);
extern void LoadXloginResources (struct display *d);
extern void ManageSession (struct display *d);
extern void SetupDisplay (struct display *d);
extern void StopDisplay (struct display *d);
extern void WaitForChild (void);

/* process.c */
extern void RegisterCloseOnFork (int fd);
extern void ClearCloseOnFork (int fd);
extern int Fork (void);
extern int Wait4 (int pid);
extern void execute(char **argv, char **environ);
extern int runAndWait (char **args, char **environ);
extern void TerminateProcess (int pid, int signal);
extern Jmp_buf GErrJmp;
extern char *GOpen (char **argv, char *what, char **env);
extern int GClose (void);
extern void GSendInt (int val);
extern int GRecvInt (void);
extern int GRecvCmd (int *cmd);
extern void GSendArr (int len, char *data);
extern char *GRecvArr (int *len);
extern int GRecvStrBuf (char *buf);
extern int GRecvArrBuf (char *buf);
extern void GSendStr (char *buf);
extern char *GRecvStr (void);
extern void GSendArgv (char **argv);
extern void GSendStrArr (int len, char **data);
extern char **GRecvStrArr (int *len);
extern char **GRecvArgv (void);

/* client.c */
extern int Verify (struct display *d, char *name, char *pass);
extern void Restrict (struct display *d);
extern int StartClient(struct display *d, char *name, char *pass, char **sessargs);
extern void SessionExit (struct display *d, int status);
extern void RdUsrData (struct display *d, char *usr, char ***args);

/* server.c */
extern char *_SysErrorMsg (int n);
extern int StartServer (struct display *d);
extern int WaitForServer (struct display *d);
extern void ResetServer (struct display *d);
extern int PingServer(struct display *d);

/* socket.c */
extern int GetChooserAddr (char *addr, int *lenp);
extern void CreateWellKnownSockets (void);

/* in util.c */
extern int ReStrN (char **dst, char *src, int len);
extern int ReStr (char **dst, char *src);
extern int StrNDup (char **dst, char *src, int len);
extern int StrDup (char **dst, char *src);
extern int StrApp (char **dst, ...);
extern void WipeStr (char *str);
extern char **initStrArr (char **arr);
extern char **extStrArr (char ***arr);
extern char **addStrArr (char **arr, char *str, int len);
extern char **xCopyStrArr (int rn, char **arr);
extern void mergeStrArrs (char ***darr, char **arr);
extern void freeStrArr (char **arr);
extern char **parseArgs (char **argv, char *string);
extern char **setEnv (char **e, char *name, char *value);
extern char **putEnv (char *string, char **env);
extern char *getEnv (char **e, char *name);
extern char *localHostname (void);
extern int Reader (int fd, void *buf, int len);
extern void FdGetsCall (int fd, void (*func)(char *, int, void *), void *ptr);

#ifdef XDMCP

/* in xdmcp.c */
extern char *NetworkAddressToHostname (CARD16 connectionType, ARRAY8Ptr connectionAddress);
extern int AnyWellKnownSockets (void);
extern void DestroyWellKnownSockets (void);
extern void SendFailed (struct display *d, char *reason);
extern void WaitForSomething (void);
extern void init_session_id(void);
extern void registerHostname(char *name, int namelen);

/* in netaddr.c */
extern char *NetaddrAddress(XdmcpNetaddr netaddrp, int *lenp);
extern char *NetaddrPort(XdmcpNetaddr netaddrp, int *lenp);
extern int ConvertAddr (XdmcpNetaddr saddr, int *len, char **addr);
extern int NetaddrFamily (XdmcpNetaddr netaddrp);
extern int addressEqual (XdmcpNetaddr a1, int len1, XdmcpNetaddr a2, int len2);

/* in policy.c */
#if 0
extern ARRAY8Ptr Accept (/* struct sockaddr *from, int fromlen, CARD16 displayNumber */);
#endif
extern ARRAY8Ptr ChooseAuthentication (ARRAYofARRAY8Ptr authenticationNames);
extern int CheckAuthentication (struct protoDisplay *pdpy, ARRAY8Ptr displayID, ARRAY8Ptr name, ARRAY8Ptr data);
extern int SelectAuthorizationTypeIndex (ARRAY8Ptr authenticationName, ARRAYofARRAY8Ptr authorizationNames);
extern int SelectConnectionTypeIndex (ARRAY16Ptr connectionTypes, ARRAYofARRAY8Ptr connectionAddresses);
extern int Willing (ARRAY8Ptr addr, CARD16 connectionType, ARRAY8Ptr authenticationName, ARRAY8Ptr status, xdmOpCode type);

/* in protodpy.c */
extern void DisposeProtoDisplay(struct protoDisplay *pdpy);

extern struct protoDisplay	*FindProtoDisplay (
					XdmcpNetaddr address,
					int          addrlen,
					CARD16       displayNumber);
extern struct protoDisplay	*NewProtoDisplay (
					XdmcpNetaddr address,
					int	     addrlen,
					CARD16	     displayNumber,
					CARD16	     connectionType,
					ARRAY8Ptr    connectionAddress,
					CARD32	     sessionID);

typedef void (*ChooserFunc)(CARD16 connectionType, ARRAY8Ptr addr, char *closure);

/* in access.c */
extern ARRAY8Ptr getLocalAddress (void);
extern int AcceptableDisplayAddress (ARRAY8Ptr clientAddress, CARD16 connectionType, xdmOpCode type);
extern int ForEachMatchingIndirectHost (ARRAY8Ptr clientAddress, CARD16 connectionType, ChooserFunc function, char *closure);
extern int ScanAccessDatabase (void);
extern int UseChooser (ARRAY8Ptr clientAddress, CARD16 connectionType);
extern void ForEachChooserHost (ARRAY8Ptr clientAddress, CARD16 connectionType, ChooserFunc function, char *closure);

/* in choose.c */
extern ARRAY8Ptr IndirectChoice (ARRAY8Ptr clientAddress, CARD16 connectionType);
extern int IsIndirectClient (ARRAY8Ptr clientAddress, CARD16 connectionType);
extern int RememberIndirectClient (ARRAY8Ptr clientAddress, CARD16 connectionType);
extern void ForgetIndirectClient ( ARRAY8Ptr clientAddress, CARD16 connectionType);
extern void ProcessChooserSocket (int fd);
extern void RunChooser (struct display *d);

#endif /* XDMCP */

#ifndef X_NOT_STDC_ENV
# include <stdlib.h>
#else
char *malloc(), *realloc();
#endif

typedef SIGVAL (*SIGFUNC)(int);

SIGVAL (*Signal(int, SIGFUNC Handler))(int);

#ifdef MINIX
# include <sys/nbio.h>
void udp_read_cb(nbio_ref_t ref, int res, int err);
void tcp_listen_cb(nbio_ref_t ref, int res, int err);
#endif

#endif /* _DM_H_ */
