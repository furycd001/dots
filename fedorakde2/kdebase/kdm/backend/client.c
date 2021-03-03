/* $TOG: verify.c /main/37 1998/02/11 10:00:45 kaleb $ */
/* $Id: client.c,v 2.15.2.5 2001/10/25 09:51:15 ossi Exp $ */
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
/* $XFree86: xc/programs/xdm/greeter/verify.c,v 3.9 2000/06/14 00:16:16 dawes Exp $ */

/*
 * xdm - display manager daemon
 * Author:  Keith Packard, MIT X Consortium
 *
 * verify.c
 *
 * user verification and session initiation.
 */

#include "dm.h"
#include "dm_auth.h"
#include "dm_error.h"

#include <errno.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#ifdef SECURE_RPC
# include <rpc/rpc.h>
# include <rpc/key_prot.h>
#endif
#ifdef K5AUTH
# include <krb5/krb5.h>
#endif
#ifdef USE_PAM
# include <security/pam_appl.h>
#elif defined(AIXV3) /* USE_PAM */
# include <login.h>
# include <usersec.h>
extern int loginrestrictions (char *Name, int Mode, char *Tty, char **Msg);
extern int loginfailed (char *User, char *Host, char *Tty);
extern int loginsuccess (char *User, char *Host, char *Tty, char **Msg);
#else /* USE_PAM || AIXV3 */
# ifdef USESHADOW
#  include <shadow.h>
# endif
# ifdef KERBEROS
#  include <sys/param.h>
#  include <krb.h>
#  ifndef NO_AFS
#   include <kafs.h>
#  endif
# endif
# ifdef CSRG_BASED
#  include <sys/param.h>
#  ifdef HAS_SETUSERCONTEXT
#   include <login_cap.h>
#   define USE_LOGIN_CAP 1
#  endif
# endif
/* for nologin */
# include <sys/types.h>
# include <unistd.h>
/* for expiration */
# include <time.h>
#endif	/* USE_PAM || AIXV3 */

#if defined(__osf__) || defined(linux) || defined(MINIX) || defined(__QNXNTO__) || defined(__GNU__)
# define setpgrp setpgid
#endif

#ifdef X_NOT_STDC_ENV
char *getenv();
#endif

#ifdef QNX4
extern char *crypt(char *, char *);
#endif


#ifdef USE_PAM

static char *PAM_password;
static char *infostr, *errstr;

# ifdef sun
typedef struct pam_message pam_message_type;
# else
typedef const struct pam_message pam_message_type;
# endif

static int
PAM_conv (int num_msg,
	  pam_message_type **msg,
	  struct pam_response **resp,
	  void *appdata_ptr ATTR_UNUSED)
{
    int count;
    struct pam_response *reply;

    if (!(reply = calloc(num_msg, sizeof(*reply))))
	return PAM_CONV_ERR;

    for (count = 0; count < num_msg; count++) {
	switch (msg[count]->msg_style) {
	case PAM_TEXT_INFO:
	    if (!StrApp(&infostr, msg[count]->msg, "\n", (char *)0))
		goto conv_err;
	    break;
	case PAM_ERROR_MSG:
	    if (!StrApp(&errstr, msg[count]->msg, "\n", (char *)0))
		goto conv_err;
	    break;
	case PAM_PROMPT_ECHO_OFF:
	    /* wants password */
# ifndef PAM_FAIL_DELAY
	    if (!PAM_password[0])
		goto conv_err;
# endif
	    if (!StrDup (&reply[count].resp, PAM_password))
		goto conv_err;
	    reply[count].resp_retcode = PAM_SUCCESS;
	    break;
	case PAM_PROMPT_ECHO_ON:
	    /* user name given to PAM already */
	    /* fall through */
	default:
	    /* unknown */
	    goto conv_err;
	}
    }
    *resp = reply;
    return PAM_SUCCESS;

  conv_err:
    for (; count >= 0; count--)
	if (reply[count].resp) {
	    switch (msg[count]->msg_style) {
	    case PAM_ERROR_MSG:
	    case PAM_TEXT_INFO:
	    case PAM_PROMPT_ECHO_ON:
		free(reply[count].resp);
		break;
	    case PAM_PROMPT_ECHO_OFF:
		WipeStr(reply[count].resp);
		break;
	    }
	    reply[count].resp = 0;
	}
    /* forget reply too */
    free (reply);
    return PAM_CONV_ERR;
}

static struct pam_conv PAM_conversation = {
	PAM_conv,
	NULL
};

# ifdef PAM_FAIL_DELAY
static void
fail_delay(int retval ATTR_UNUSED, unsigned usec_delay ATTR_UNUSED, 
	   void *appdata_ptr ATTR_UNUSED)
{}
# endif

static pam_handle_t *pamh;

#elif !defined(AIXV3) /* USE_PAM */

# ifdef KERBEROS
static char krbtkfile[MAXPATHLEN];
static int krb4_authed;

static int
krb4_auth(struct passwd *p, char *password)
{
    int ret;
    char realm[REALM_SZ];

    if (krb4_authed)
	return 1;
    krb4_authed = 1;

    if (!p->pw_uid)
	return 0;

    if (krb_get_lrealm(realm, 1)) {
	Debug ("Can't get Kerberos realm.\n");
	return 0;
    }

    sprintf(krbtkfile, "%s.%s", TKT_ROOT, d->name);
    krb_set_tkt_string(krbtkfile);
    unlink(krbtkfile);

    ret = krb_verify_user(p->pw_name, "", realm, password, 1, "rcmd");
    if (ret == KSUCCESS) {
	chown(krbtkfile, p->pw_uid, p->pw_gid);
	Debug("kerberos verify succeeded\n");
	return 1;
    } else if(ret != KDC_PR_UNKNOWN && ret != SKDC_CANT) {
	/* failure */
	Debug("kerberos verify failure %d\n", ret);
	LogError("KERBEROS verification failure '%s' for %s\n",
		 krb_get_err_text(ret), p->pw_name);
	krbtkfile[0] = '\0';
    }
    return 0;
}
# endif /* KERBEROS */

#endif /* USE_PAM */

static int		pwinited;
static struct passwd	*p;
#if !defined(USE_PAM) && !defined(AIXV3) && defined(USESHADOW)
static struct spwd	*sp;
static char		*user_pass;
# define arg_shadow , int shadow
# define NSHADOW , 0
# define YSHADOW , 1
#else
# define arg_shadow
# define NSHADOW
# define YSHADOW
# define user_pass p->pw_passwd
#endif

static int
init_pwd(char *name arg_shadow)
{
#if !defined(USE_PAM) && !defined(AIXV3) && defined(USESHADOW)
    if (pwinited == 2) {
	Debug("p & sp for %s already read\n", name);
	return 1;
    }
#endif

    if (!pwinited) {
	Debug("reading p for %s\n", name);
	p = getpwnam (name);
	endpwent();
	if (!p) {
	    Debug ("getpwnam() failed.\n");
	    return 0;
	}
#if !defined(USE_PAM) && !defined(AIXV3) && defined(USESHADOW)
	user_pass = p->pw_passwd;
#endif
	pwinited = 1;
    }

#if !defined(USE_PAM) && !defined(AIXV3) && defined(USESHADOW)
    if (shadow) {
	Debug("reading sp for %s\n", name);
	errno = 0;
	sp = getspnam(name);
# ifndef QNX4
	endspent();
# endif  /* QNX4 doesn't need endspent() to end shadow passwd ops */
	if (sp) {
	    user_pass = sp->sp_pwdp;
	    pwinited = 2;
	} else
	    Debug ("getspnam() failed, errno=%d.  Are you root?\n", errno);
    }
#endif

    return 1;
}


#if !defined(USE_PAM) && defined(AIXV3)
static char tty[16], hostname[100];
#endif

static int
init_vrf(struct display *d, char *name, char *password)
{
#ifdef USE_PAM
    int		pretc;
#elif defined(AIXV3)
    int		i;
    char	*tmpch;
#endif
    static char	*pname;

    if (!strlen (name)) {
	Debug ("Empty user name provided.\n");
	return V_AUTH;
    }

    if (pname && strcmp(pname, name)) {
	Debug("Re-initializing for new user %s\n", name);
#if !defined(USE_PAM) && !defined(AIXV3) && defined(KERBEROS)
	krb4_authed = 0;
	krbtkfile[0] = '\0';
#endif
	pwinited = 0;
    }
    ReStr (&pname, name);

#ifdef USE_PAM

    PAM_password = (char *)password;

    if (!pamh) {
	Debug("opening new PAM handle\n");
	if (pam_start(PAMService, name, &PAM_conversation, &pamh) != PAM_SUCCESS) {
	    ReInitErrorLog ();
	    return V_ERROR;
	}
	if ((pretc = pam_set_item(pamh, PAM_TTY, d->name)) != PAM_SUCCESS) {
	    pam_end(pamh, pretc);
	    pamh = NULL;
	    ReInitErrorLog ();
	    return V_ERROR;
	}
	if ((pretc = pam_set_item(pamh, PAM_RHOST, "")) != PAM_SUCCESS) {
	    pam_end(pamh, pretc);
	    pamh = NULL;
	    ReInitErrorLog ();
	    return V_ERROR;
	}
# ifdef PAM_FAIL_DELAY
	pam_set_item(pamh, PAM_FAIL_DELAY, fail_delay);
# endif
    } else
	if (pam_set_item(pamh, PAM_USER, name) != PAM_SUCCESS) {
	    ReInitErrorLog ();
	    return V_ERROR;
	}
    ReInitErrorLog ();


    if (infostr) {
	free (infostr);
	infostr = 0;
    }

    if (errstr) {
	free (errstr);
	errstr = 0;
    }

#elif defined(AIXV3)

    if ((d->displayType & d_location) == Foreign) {
	strncpy(hostname, d->name, sizeof(hostname) - 1);
	hostname[sizeof(hostname)-1] = '\0';
	if ((tmpch = strchr(hostname, ':')))
	    *tmpch = '\0';
    } else
	hostname[0] = '\0';

    /* tty names should only be 15 characters long */
# if 1
    for (i = 0; i < 15 && d->name[i]; i++) {
	if (d->name[i] == ':' || d->name[i] == '.')
	    tty[i] = '_';
	else
	    tty[i] = d->name[i];
    }
    tty[i] = '\0';
# else
    memcpy(tty, "/dev/xdm/", 9);
    for (i = 0; i < 6 && d->name[i]; i++) {
	if (d->name[i] == ':' || d->name[i] == '.')
	    tty[9 + i] = '_';
	else
	    tty[9 + i] = d->name[i];
    }
    tty[9 + i] = '\0';
# endif

#endif

	Debug("init_vrf ok\n");
    return V_OK;
}

static int
AccNoPass (struct display *d, char *un)
{
    char **fp;

    if (!strcmp (un, d->autoUser))
	return 1;

    for (fp = d->noPassUsers; *fp; fp++)
	if (!strcmp (un, *fp))
	    return 1;

    return 0;
}

int
Verify (struct display *d, char *name, char *pass)
{
    int		pretc;
#if !defined(USE_PAM) && defined(AIXV3)
    int		reenter;
    char	*msg;
#endif

    Debug ("Verify %s ...\n", name);

    if (!pass[0] && AccNoPass (d, name)) {
	Debug ("accepting despite empty password\n");
	return V_OK;
    }

    if ((pretc = init_vrf(d, name, pass)) != V_OK)
	return pretc;

#ifdef USE_PAM

    if (pam_authenticate(pamh, d->allowNullPasswd ?
				0 : PAM_DISALLOW_NULL_AUTHTOK) != PAM_SUCCESS) {
	ReInitErrorLog ();
	return V_AUTH;
    }
    ReInitErrorLog ();

#elif defined(AIXV3) /* USE_PAM */

    enduserdb();
    msg = NULL;
    if (authenticate(name, pass, &reenter, &msg) || reenter) {
	Debug("authenticate() - %s\n", msg ? msg : "error");
	if (msg)
	    free((void *)msg);
	loginfailed(name, hostname, tty);
	return V_AUTH;
    }
    if (msg)
	free((void *)msg);

#else	/* USE_PAM && AIXV3 */

    if (!init_pwd(name YSHADOW))
	return V_AUTH;

# ifdef KERBEROS
    if (!krb4_auth(p, pass))
# endif  /* KERBEROS */
# ifdef linux	/* only Linux? */
	if (p->pw_passwd[0] == '!' || p->pw_passwd[0] == '*') {
	    Debug ("account is locked\n");
	    return V_AUTH;
	}
# endif
# if defined(ultrix) || defined(__ultrix__)
	if (authenticate_user(p, pass, NULL) < 0)
# else
	if (strcmp (crypt (pass, user_pass), user_pass))
# endif
	{
	    if(!d->allowNullPasswd || user_pass[0]) {
		Debug ("password verify failed\n");
		return V_AUTH;
	    } /* else: null passwd okay */
	}

#endif /* USE_PAM && AIXV3 */

    Debug ("verify succeeded\n");

    return V_OK;
}

void
Restrict (struct display *d)
{
    int			pretc;
    char		*name;
#ifndef USE_PAM
# ifdef AIXV3
    char		*msg;
# else /* AIXV3 */
    struct stat		st;
    char		*nolg;
#  ifdef HAVE_GETUSERSHELL
    char		*s;
#  endif
#  if defined(HAVE_PW_EXPIRE) || defined(USESHADOW)
    int			tim, exp, warntime;
    int			quietlog;
#  endif
#  ifdef USE_LOGIN_CAP
#   ifdef HAVE_LOGIN_GETCLASS
    login_cap_t		*lc;
#   else
    struct login_cap	*lc;
#   endif
#  endif
#  if defined(HAVE_PW_EXPIRE) || defined(USESHADOW)
    int			expire;
    int			retv;
#  endif
# endif /* AIXV3 */
#endif

    name = GRecvStr ();

    Debug("Restrict %s ...\n", name);

    if (!strcmp (name, "root")) {
	if (!d->allowRootLogin)
	    GSendInt (V_NOROOT);
	else
	    GSendInt (V_OK);	/* don't deny root to log in */
	free (name);
	return;
    }

    if ((pretc = init_vrf(d, name, "")) != V_OK) {
	free (name);
	GSendInt (pretc);
	return;
    }

#ifdef USE_PAM

    pretc = pam_acct_mgmt(pamh, 0);
    ReInitErrorLog ();
    if (errstr) {
	GSendInt (V_MSGERR);
	GSendStr (errstr);
    } else if (pretc != PAM_SUCCESS) {
	GSendInt (V_AUTH);
    } else if (infostr) {
	GSendInt (V_MSGINFO);
	GSendStr (infostr);
    } else
	GSendInt (V_OK);
    /* really should do password changing, but it doesn't fit well */

    free (name);

#elif defined(AIXV3)	/* USE_PAM */

    msg = NULL;
    if (loginrestrictions(name,
	((d->displayType & d_location) == Foreign) ? S_RLOGIN : S_LOGIN,
	tty, &msg) == -1)
    {
	Debug("loginrestrictions() - %s\n", msg ? msg : "Error\n");
	loginfailed(name, hostname, tty);
	if (msg) {
	    GSendInt (V_MSGERR);
	    GSendStr (msg);
	} else
	    GSendInt (V_AUTH);
    } else
	    GSendInt (V_OK);
    if (msg)
	free((void *)msg);

    free (name);

#else	/* USE_PAM || AIXV3 */

    if (!init_pwd(name YSHADOW)) {
	free (name);
	GSendInt (V_AUTH);
	return;
    }

    free (name);

# ifdef HAVE_GETUSERSHELL
    for (;;) {
	if (!(s = getusershell())) {
	    Debug("shell not in /etc/shells\n");
	    endusershell();
	    GSendInt (V_BADSHELL);
	    return;
	}
	if (!strcmp(s, p->pw_shell)) {
	    endusershell();
	    break;
	}
    }
# endif

# ifdef USE_LOGIN_CAP
#  ifdef HAVE_LOGIN_GETCLASS
    lc = login_getclass(p->pw_class);
#  else
    lc = login_getpwclass(p);
#  endif
    if (!lc) {
	GSendInt (V_ERROR);
	return;
    }
# endif


/* restrict_nologin */
# ifndef _PATH_NOLOGIN
#  define _PATH_NOLOGIN "/etc/nologin"
# endif

    if ((
# ifdef USE_LOGIN_CAP
    /* Do we ignore a nologin file? */
	!login_getcapbool(lc, "ignorenologin", 0)) &&
	(!stat((nolg = login_getcapstr(lc, "nologin", "", NULL)), &st) ||
# endif
	 !stat((nolg = _PATH_NOLOGIN), &st))) {
	GSendInt (V_NOLOGIN);
	GSendStr (nolg);
# ifdef USE_LOGIN_CAP
	login_close(lc);
# endif
	return;
    }


/* restrict_nohome */
# ifdef USE_LOGIN_CAP
    if (login_getcapbool(lc, "requirehome", 0)) {
	struct stat st;
	if (!*p->pw_dir || stat (p->pw_dir, &st) || st.st_uid != p->pw_uid) {
	    GSendInt (V_NOHOME);
	    login_close(lc);
	    return;
	}
    }
# endif


/* restrict_time */
# ifdef USE_LOGIN_CAP
#  ifdef HAVE_AUTH_TIMEOK
    if (!auth_timeok(lc, time(NULL))) {
	GSendInt (V_BADTIME);
	login_close(lc);
	return;
    }
#  endif
# endif


/* restrict_expired; this MUST be the last one */
# if defined(HAVE_PW_EXPIRE) || defined(USESHADOW)

#  if !defined(HAVE_PW_EXPIRE) || (!defined(USE_LOGIN_CAP) && defined(USESHADOW))
    if (sp)
#  endif
    {

#  define DEFAULT_WARN  (2L * 7L)  /* Two weeks */

	tim = time(NULL) / 86400L;

#  ifdef USE_LOGIN_CAP
	quietlog = login_getcapbool(lc, "hushlogin", 0);
	warntime = login_getcaptime(lc, "warnexpire",
				    DEFAULT_WARN * 86400L, 
				    DEFAULT_WARN * 86400L) / 86400L;
#  else
	quietlog = 0;
#   ifdef USESHADOW
	warntime = sp->sp_warn != -1 ? sp->sp_warn : DEFAULT_WARN;
#   else
	warntime = DEFAULT_WARN;
#   endif
#  endif

	retv = V_OK;

#  ifdef HAVE_PW_EXPIRE
	exp = p->pw_expire / 86400L;
	if (exp) {
#  else
	if (sp->sp_expire != -1) {
	    exp = sp->sp_expire;
#  endif
	    if (tim > exp) {
		GSendInt (V_AEXPIRED);
#  ifdef USE_LOGIN_CAP
		login_close(lc);
#  endif
		return;
	    } else if (tim > (exp - warntime) && !quietlog) {
		expire = exp - tim;
		retv = V_AWEXPIRE;
	    }
	}

#  ifdef HAVE_PW_EXPIRE
	exp = p->pw_change / 86400L;
	if (exp) {
#  else
	if (sp->sp_max != -1) {
	    exp = sp->sp_lstchg + sp->sp_max;
#  endif
	    if (tim > exp) {
		GSendInt (V_PEXPIRED);
#  ifdef USE_LOGIN_CAP
		login_close(lc);
#  endif
		return;
	    } else if (tim > (exp - warntime) && !quietlog) {
		if (retv == V_OK || expire > exp) {
		    expire = exp - tim;
		    retv = V_PWEXPIRE;
		}
	    }
	}

	if (retv != V_OK) {
	    GSendInt (retv);
	    GSendInt (expire);
#  ifdef USE_LOGIN_CAP
	    login_close(lc);
#  endif
	    return;
	}

    }

# endif /* HAVE_PW_EXPIRE || USESHADOW */

    GSendInt (V_OK);
# ifdef USE_LOGIN_CAP
    login_close(lc);
# endif


#endif /* USE_PAM || AIXV3 */
}


static char *envvars[] = {
    "TZ",			/* SYSV and SVR4, but never hurts */
#ifdef AIXV3
    "AUTHSTATE",		/* for kerberos */
#endif
#if defined(sony) && !defined(SYSTYPE_SYSV) && !defined(_SYSTYPE_SYSV)
    "bootdev",
    "boothowto",
    "cputype",
    "ioptype",
    "machine",
    "model",
    "CONSDEVTYPE",
    "SYS_LANGUAGE",
    "SYS_CODE",
#endif
#if (defined(SVR4) || defined(SYSV)) && defined(i386) && !defined(sun)
    "XLOCAL",
#endif
    NULL
};

static char **
userEnv (struct display *d, int useSystemPath, char *user, char *home, char *shell)
{
    char	**env;

    env = defaultEnv (user);
    env = setEnv (env, "XDM_MANAGED", "true");
    env = setEnv (env, "DISPLAY", d->name);
    env = setEnv (env, "HOME", home);
    env = setEnv (env, "PATH", useSystemPath ? d->systemPath : d->userPath);
    env = setEnv (env, "SHELL", shell);
#if !defined(USE_PAM) && !defined(AIXV3) && defined(KERBEROS)
    if (krbtkfile[0] != '\0')
        env = setEnv (env, "KRBTKFILE", krbtkfile);
#endif
    env = inheritEnv (env, envvars);
    return env;
}


static int
SetGid (char *name, int gid)
{
    if (setgid(gid) < 0)
    {
	LogError("setgid %d (user \"%s\") failed, errno=%d\n",
		 gid, name, errno);
	return 0;
    }
#ifndef QNX4
    if (initgroups(name, gid) < 0)
    {
	LogError("initgroups for \"%s\" failed, errno=%d\n", name, errno);
	return 0;
    }
#endif   /* QNX4 doesn't support multi-groups, no initgroups() */
    return 1;
}

static int
SetUid (char *name, int uid)
{
    if (setuid(uid) < 0)
    {
	LogError("setuid %d (user \"%s\") failed, errno=%d\n",
		 uid, name, errno);
	return 0;
    }
    return 1;
}

static int
SetUser (char *name, int uid, int gid)
{
    return SetGid (name, gid) && SetUid (name, uid);
}


static int removeAuth = 0;
static int sourceReset = 0;

int
StartClient(struct display *d, char *name, char *pass, char **sessargs)
{
    char	*shell, *home;
    char	**argv;
#ifdef USE_PAM
    char	**pam_env;
#else
# ifdef AIXV3
    char	*msg;
    char	**theenv;
    extern char	**newenv; /* from libs.a, this is set up by setpenv */
# else
#  ifdef HAS_SETUSERCONTEXT
    extern char	**environ;
#  endif
# endif
#endif
    char	*failsafeArgv[2];
    struct verify_info	*verify;
    int		i, pid;

    if (init_vrf(d, name, pass) != V_OK)
	return 0;

    if (!init_pwd(name NSHADOW))
	return 0;

#ifndef USE_PAM
# ifdef AIXV3
    msg = NULL;
    loginsuccess(name, hostname, tty, &msg);
    if (msg) {
	Debug("loginsuccess() - %s\n", msg);
	free((void *)msg);
    }
# else /* AIXV3 */
#  if defined(KERBEROS) && !defined(NO_AFS)
    if (pass[0] && krb4_auth(p, pass)) {
	if (k_hasafs()) {
	    if (k_setpag() == -1)
		LogError ("setpag() failed for %s\n", name);
	    /*  XXX maybe, use k_afsklog_uid(NULL, NULL, p->pw_uid)? */
	    if ((ret = k_afsklog(NULL, NULL)) != KSUCCESS)
		LogError("Warning %s\n", krb_get_err_text(ret));
	}
    }
#  endif /* KERBEROS && AFS */
# endif /* AIXV3 */
#endif	/* !PAM */

    if (!(verify = malloc (sizeof (*verify)))) {
	LogOutOfMem("StartClient");
	return 0;
    }
    d->verify = verify;
    StrDup (&verify->user, name);
    verify->uid = p->pw_uid;
    verify->gid = p->pw_gid;
    home = p->pw_dir;
    shell = p->pw_shell;
    verify->userEnviron = userEnv (d, p->pw_uid == 0, name, home, shell);
    verify->systemEnviron = systemEnv (d, name, home);
    Debug ("user environment:\n%[|>s"
	   "system environment:\n%[|>s"
	   "end of environments\n", 
	   "", "\n", verify->userEnviron,
	   "", "\n", verify->systemEnviron);

    /*
     * for user-based authorization schemes,
     * add the user to the server's allowed "hosts" list.
     */
    for (i = 0; i < d->authNum; i++)
    {
#ifdef SECURE_RPC
	if (d->authorizations[i]->name_length == 9 &&
	    memcmp(d->authorizations[i]->name, "SUN-DES-1", 9) == 0)
	{
	    XHostAddress	addr;
	    char		netname[MAXNETNAMELEN+1];
	    char		domainname[MAXNETNAMELEN+1];
    
	    getdomainname(domainname, sizeof domainname);
	    user2netname (netname, verify->uid, domainname);
	    addr.family = FamilyNetname;
	    addr.length = strlen (netname);
	    addr.address = netname;
	    XAddHost (d->dpy, &addr);
	}
#endif
#ifdef K5AUTH
	if (d->authorizations[i]->name_length == 14 &&
	    memcmp(d->authorizations[i]->name, "MIT-KERBEROS-5", 14) == 0)
	{
	    /* Update server's auth file with user-specific info.
	     * Don't need to AddHost because X server will do that
	     * automatically when it reads the cache we are about
	     * to point it at.
	     */
	    extern Xauth *Krb5GetAuthFor();

	    XauDisposeAuth (d->authorizations[i]);
	    d->authorizations[i] =
		Krb5GetAuthFor(14, "MIT-KERBEROS-5", d->name);
	    SaveServerAuthorizations (d, d->authorizations, d->authNum);
	}
#endif
    }

    /*
     * Run system-wide initialization file
     */
    sourceReset = 1;
    if (source (verify->systemEnviron, d->startup) != 0) {
	LogError("Cannot execute startup script %s\n", d->startup);
	SessionExit (d, EX_NORMAL);
    }

    Debug ("now starting the session\n");
#ifdef USE_PAM
    pam_open_session(pamh, 0);
    ReInitErrorLog ();
#endif    
    removeAuth = 1;
    switch (pid = Fork ()) {
    case 0:
#ifdef XDMCP
	/* The chooser socket is not closed by CleanUpChild() */
	DestroyWellKnownSockets();
#endif

	/* Do system-dependent login setup here */
#ifdef CSRG_BASED
	setsid();
#else
# if defined(SYSV) || defined(SVR4) || defined(__CYGWIN__)
#  if !(defined(SVR4) && defined(i386)) || defined(SCO325) || defined(__GNU__)
	setpgrp ();
#  endif
# else
	setpgrp (0, getpid ());
# endif
#endif

#ifndef AIXV3

# if !defined(HAS_SETUSERCONTEXT) || defined(USE_PAM)
	if (!SetGid (name, verify->gid))
	    exit (1);
#  ifdef USE_PAM
	pam_setcred(pamh, 0);
	/* pass in environment variables set by libpam and modules it called */
	pam_env = pam_getenvlist(pamh);
	ReInitErrorLog ();
	if (pam_env)
	    for(; *pam_env; pam_env++)
		verify->userEnviron = putEnv(*pam_env, verify->userEnviron);
#  endif
#  if defined(BSD) && (BSD >= 199103)
	if (setlogin(name) < 0)
	{
	    LogError("setlogin for \"%s\" failed, errno=%d\n", name, errno);
	    exit (1);
	}
#  endif
	if (!SetUid (name, verify->uid))
	    exit (1);
# else /* HAS_SETUSERCONTEXT && !USE_PAM */
	/*
	 * Destroy environment unless user has requested its preservation.
	 * We need to do this before setusercontext() because that may
	 * set or reset some environment variables.
	 */
	if (!(environ = initStrArr (0))) {
	    LogOutOfMem("StartSession");
	    exit (1);
	}

	/*
	 * Set the user's credentials: uid, gid, groups,
	 * environment variables, resource limits, and umask.
	 */
	if (setusercontext(NULL, p, p->pw_uid, LOGIN_SETALL) < 0)
	{
	    LogError("setusercontext for \"%s\" failed, errno=%d\n", name,
		     errno);
	    exit (1);
	}

	for (i = 0; environ[i]; i++)
	    verify->userEnviron = putEnv(environ[i], verify->userEnviron);

# endif /* HAS_SETUSERCONTEXT */
#else /* AIXV3 */
	/*
	 * Set the user's credentials: uid, gid, groups,
	 * audit classes, user limits, and umask.
	 */
	if (setpcred(name, NULL) == -1)
	{
	    LogError("setpcred for \"%s\" failed, errno=%d\n", name, errno);
	    exit (1);
	}

	/*
	 * Make a copy of the environment, because setpenv will trash it.
	 */
	if (!(theenv = xCopyStrArr (0, verify->userEnviron)))
	{
	    LogOutOfMem("StartSession");
	    exit (1);
	}

	/*
	 * Set the users process environment. Store protected variables and
	 * obtain updated user environment list. This call will initialize
	 * global 'newenv'. 
	 */
	if (setpenv(name, PENV_INIT | PENV_ARGV | PENV_NOEXEC,
		    theenv, NULL) != 0)
	{
	    LogError("Can't set process environment (user=%s)\n", name);
	    exit (1);
	}

	/*
	 * Free old userEnviron and replace with newenv from setpenv().
	 */
	free(theenv);
	freeStrArr(verify->userEnviron);
	verify->userEnviron = newenv;

#endif /* AIXV3 */

	/*
	 * for user-based authorization schemes,
	 * use the password to get the user's credentials.
	 */
#ifdef SECURE_RPC
	/* do like "keylogin" program */
	if (!pass[0])
	    LogInfo("no password for NIS provided.\n");
	else
	{
	    char    netname[MAXNETNAMELEN+1], secretkey[HEXKEYBYTES+1];
	    int	    nameret, keyret;
	    int	    len;
	    int     key_set_ok = 0;

	    nameret = getnetname (netname);
	    Debug ("User netname: %s\n", netname);
	    len = strlen (pass);
	    if (len > 8)
		bzero (pass + 8, len - 8);
	    keyret = getsecretkey(netname, secretkey, pass);
	    Debug ("getsecretkey returns %d, key length %d\n",
		   keyret, strlen (secretkey));
	    /* is there a key, and do we have the right password? */
	    if (keyret == 1)
	    {
		if (*secretkey)
		{
		    keyret = key_setsecret(secretkey);
		    Debug ("key_setsecret returns %d\n", keyret);
		    if (keyret == -1)
			LogError ("failed to set NIS secret key\n");
		    else
			key_set_ok = 1;
		}
		else
		{
		    /* found a key, but couldn't interpret it */
		    LogError ("password incorrect for NIS principal \"%s\"\n",
			      nameret ? netname : name);
		}
	    }
	    if (!key_set_ok)
	    {
		/* remove SUN-DES-1 from authorizations list */
		int i, j;
		for (i = 0; i < d->authNum; i++)
		{
		    if (d->authorizations[i]->name_length == 9 &&
			memcmp(d->authorizations[i]->name, "SUN-DES-1", 9) == 0)
		    {
			for (j = i+1; j < d->authNum; j++)
			    d->authorizations[j-1] = d->authorizations[j];
			d->authNum--;
			break;
		    }
		}
	    }
	    bzero(secretkey, strlen(secretkey));
	}
#endif
#ifdef K5AUTH
	/* do like "kinit" program */
	if (!pass[0])
	    LogInfo("no password for Kerberos5 provided.\n");
	else
	{
	    int i, j;
	    int result;
	    extern char *Krb5CCacheName();

	    result = Krb5Init(name, pass, d);
	    if (result == 0) {
		/* point session clients at the Kerberos credentials cache */
		verify->userEnviron =
		    setEnv(verify->userEnviron,
			   "KRB5CCNAME", Krb5CCacheName(d->name));
	    } else {
		for (i = 0; i < d->authNum; i++)
		{
		    if (d->authorizations[i]->name_length == 14 &&
			memcmp(d->authorizations[i]->name, "MIT-KERBEROS-5", 14) == 0)
		    {
			/* remove Kerberos from authorizations list */
			for (j = i+1; j < d->authNum; j++)
			    d->authorizations[j-1] = d->authorizations[j];
			d->authNum--;
			break;
		    }
		}
	    }
	}
#endif /* K5AUTH */
	if (pass)
	    bzero(pass, strlen(pass));
	SetUserAuthorization (d, verify);
	home = getEnv (verify->userEnviron, "HOME");
	if (home) {
	    if (chdir (home) < 0) {
		LogError ("user \"%s\": cannot chdir to home \"%s\" (err %d), using \"/\"\n",
			  getEnv (verify->userEnviron, "USER"), home, errno);
		chdir ("/");
		verify->userEnviron = setEnv(verify->userEnviron, "HOME", "/");
	    } else {
		FILE *file;
		for (i = 0; d->sessSaveFile[i]; i++)
		    if (d->sessSaveFile[i] == '/') {
			d->sessSaveFile[i] = 0;
			mkdir (d->sessSaveFile, 0700);
			d->sessSaveFile[i] = '/';
		    }
		if ( (file = fopen(d->sessSaveFile, "w")) != NULL ) {
		    for (i = 0; sessargs[i]; i++)
			fprintf (file, "%s\n", sessargs[i]);
		    /* XXX write here: relogin, dpi, bbp, res */
		    fclose (file);
		}
	    }
	}
	argv = parseArgs ((char **)0, d->session);
Debug ("session args: %'[s\n", sessargs);
	mergeStrArrs (&argv, sessargs);
	if (!argv || !argv[0])
	    argv = addStrArr (argv, "xsession", 8);
	if (argv) {
		Debug ("executing session %s\n", argv[0]);
		execute (argv, verify->userEnviron);
		LogError ("Session \"%s\" execution failed (err %d)\n", argv[0], errno);
	} else {
		LogError ("Session has no command/arguments\n");
	}
	failsafeArgv[0] = d->failsafeClient;
	failsafeArgv[1] = 0;
	execute (failsafeArgv, verify->userEnviron);
	exit (1);
    case -1:
	Debug ("StartSession, fork failed\n");
	LogError ("can't start session on \"%s\", fork failed, errno=%d\n",
		  d->name, errno);
	return 0;
    default:
	Debug ("StartSession, fork succeeded %d\n", pid);
	return pid;
    }
}

void
SessionExit (struct display *d, int status)
{
    /* make sure the server gets reset after the session is over */
    if (d->serverPid >= 2 && d->resetSignal)
	TerminateProcess (d->serverPid, d->resetSignal);
    else
	ResetServer (d);
    if (sourceReset) {
	/*
	 * run system-wide reset file
	 */
	Debug ("Source reset program %s\n", d->reset);
	source (d->verify->systemEnviron, d->reset);
    }
    if (removeAuth)
    {
#ifdef USE_PAM
	if (pamh) {
	    /* shutdown PAM session */
	    pam_setcred(pamh, PAM_DELETE_CRED);
	    pam_close_session(pamh, 0);
	    pam_end(pamh, PAM_SUCCESS);
	    pamh = NULL;
	    ReInitErrorLog ();
	}
#endif
	SetUser (d->verify->user, d->verify->uid, d->verify->gid);
	RemoveUserAuthorization (d, d->verify);
#ifdef K5AUTH
	/* do like "kdestroy" program */
        {
	    krb5_error_code code;
	    krb5_ccache ccache;

	    code = Krb5DisplayCCache(d->name, &ccache);
	    if (code)
		LogError("%s while getting Krb5 ccache to destroy\n",
			 error_message(code));
	    else {
		code = krb5_cc_destroy(ccache);
		if (code) {
		    if (code == KRB5_FCC_NOFILE) {
			Debug ("No Kerberos ccache file found to destroy\n");
		    } else
			LogError("%s while destroying Krb5 credentials cache\n",
				 error_message(code));
		} else
		    Debug ("Kerberos ccache destroyed\n");
		krb5_cc_close(ccache);
	    }
	}
#endif /* K5AUTH */
#if !defined(USE_PAM) && !defined(AIXV3)
# ifdef KERBEROS
	if (krbtkfile[0]) {
	    (void) dest_tkt();
#  ifndef NO_AFS
	    if (k_hasafs())
		(void) k_unlog();
#  endif
	}
# endif
#endif /* !USE_PAM && !AIXV3*/
#ifdef USE_PAM
    } else {
	if (pamh) {
	    pam_end(pamh, PAM_SUCCESS);
	    pamh = NULL;
	    ReInitErrorLog ();
	}
#endif
    }
    Debug ("Display %s exiting with status %d\n", d->name, status);
    exit (status);
}

static void
addData (char *buf, int len, void *ptr)
{
    char ***arr = (char ***)ptr;
    *arr = addStrArr (*arr, buf, len);
}

void
RdUsrData (struct display *d, char *usr, char ***args)
{
    struct passwd *pw;
    int len, pid, fd, pfd[2];
    char buf[256];

    *args = 0;

    if (!usr || !usr[0] || !(pw = getpwnam (usr)))
	return;

    if (pipe (pfd))
	return;
    if ((pid = Fork()) < 0) {
	close (pfd[0]);
	close (pfd[1]);
	return;
    }
    if (!pid) {
	if (!SetUser (pw->pw_name, pw->pw_uid, pw->pw_gid))
	    exit (0);
	if (chdir (pw->pw_dir) < 0)
	    exit (0);
	if ((fd = open (d->sessSaveFile, O_RDONLY)) < 0)
	    exit (0);
	while ((len = read (fd, buf, sizeof(buf))) > 0)
	    write (pfd[1], buf, len);
	exit (1);
    }
    close (pfd[1]);
    *args = initStrArr (0);
    FdGetsCall (pfd[0], addData, args);
    close (pfd[0]);
    (void) Wait4 (pid);
}


#if (defined(Lynx) && !defined(HAS_CRYPT)) || defined(SCO) && !defined(SCO_USA) && !defined(_SCO_DS)
char *crypt(char *s1, char *s2)
{
    return(s2);
}
#endif
