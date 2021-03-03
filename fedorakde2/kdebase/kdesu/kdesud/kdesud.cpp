/* vi: ts=8 sts=4 sw=4
 *
 * $Id: kdesud.cpp,v 1.21 2001/02/09 18:08:07 waba Exp $
 *
 * This file is part of the KDE project, module kdesu.
 * Copyright (C) 1999,2000 Geert Jansen <jansen@kde.org>
 * 
 *
 * kdesud.cpp: KDE su daemon. Offers "keep password" functionality to kde su.
 *
 * The socket $KDEHOME/socket-$(HOSTNAME)/kdesud_$(display) is used for communication with
 * client programs.
 *
 * The protocol: Client initiates the connection. All commands and responses
 * are terminated by a newline.
 *
 *   Client                     Server     Description
 *   ------                     ------     -----------
 *
 *   PASS <pass> <timeout>      OK         Set password for commands in
 *                                         this session. Password is
 *                                         valid for <timeout> seconds.
 *
 *   USER <user>                OK         Set the target user [required]
 *
 *   EXEC <command>             OK         Execute command <command>. If
 *                              NO         <command> has been executed
 *                                         before (< timeout) no PASS
 *                                         command is needed.
 *                                              
 *   DEL <command>              OK         Delete password for command
 *                              NO         <command>.
 *
 *   PING                       OK         Ping the server (diagnostics).
 */


#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>
#include <pwd.h>
#include <errno.h>

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/resource.h>
#include <sys/wait.h>
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>		// Needed on some systems.
#endif

#include <qvector.h>
#include <qfile.h>

#include <kinstance.h>
#include <kdebug.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kstddirs.h>
#include <kaboutdata.h>
#include <kdesu/client.h>
#include <kdesu/defaults.h>

#include "repo.h"
#include "handler.h"

#include <X11/X.h>
#include <X11/Xlib.h>

#ifndef SUN_LEN
#define SUN_LEN(ptr) ((socklen_t) (((struct sockaddr_un *) 0)->sun_path) \
                     + strlen ((ptr)->sun_path))   
#endif

#define ERR strerror(errno)

// Globals

Repository *repo;
const char *Version = "1.01";
QCString sock;
Display *x11Display;


void kdesud_cleanup()
{
    unlink(sock);
}


// Borrowed from kdebase/kaudio/kaudioserver.cpp

extern "C" int xio_errhandler(Display *);

int xio_errhandler(Display *)
{
    kdError(1205) << "Fatal IO error, exiting...\n";
    kdesud_cleanup();
    exit(1);
    return 1;  //silence compilers
}

int initXconnection()
{
    x11Display = XOpenDisplay(NULL);
    if (x11Display != 0L) 
    {
	XSetIOErrorHandler(xio_errhandler);
	XCreateSimpleWindow(x11Display, DefaultRootWindow(x11Display), 
		0, 0, 1, 1, 0,
		BlackPixelOfScreen(DefaultScreenOfDisplay(x11Display)),
		BlackPixelOfScreen(DefaultScreenOfDisplay(x11Display)));
	return XConnectionNumber(x11Display);
    } else 
    {
	kdWarning(1205) << "Can't connect to the X Server.\n";
	kdWarning(1205) << "Might not terminate at end of session.\n";
	return -1;
    }
}

extern "C" {
  void signal_exit(int);
  void sigchld_handler(int);
}

void signal_exit(int sig)
{
    kdDebug(1205) << "Exiting on signal " << sig << "\n";
    kdesud_cleanup();
    exit(1);
}

void sigchld_handler(int)
{
    int status;
    pid_t pid;

    while (1) 
    {
	pid = waitpid((pid_t) -1, &status, WNOHANG);
	if (pid <= 0)
	    break;
	kdDebug(1205) << "PID " << (int) pid << " exited\n";
    }
}

/**
 * Creates an AF_UNIX socket in socket resource, mode 0600.
 */

int create_socket()
{
    int sockfd;
    socklen_t addrlen;
    struct stat s;

    QCString display(getenv("DISPLAY"));
    if (display.isEmpty())
    {
	kdWarning(1205) << "$DISPLAY is not set\n";
	return -1;
    }

    // strip the screen number from the display
    display.replace(QRegExp("\\.[0-9]+$"), "");

    sock = QFile::encodeName(locateLocal("socket", QString("kdesud_%1").arg(display)));
    int stat_err=lstat(sock, &s);
    if(!stat_err && S_ISLNK(s.st_ino)) {
	kdWarning(1205) << "Someone is running a symlink attack on you\n";
	if(unlink(sock)) {
	    kdWarning(1205) << "Could not delete symlink\n";
	    return -1;
	}
    }

    if (!access(sock, R_OK|W_OK)) 
    {
	KDEsuClient client;
	if (client.ping() == -1) 
	{
	    kdWarning(1205) << "stale socket exists\n";
	    if (unlink(sock)) 
	    {
		kdWarning(1205) << "Could not delete stale socket\n";
		return -1;
	    }
	} else 
	{
	    kdWarning(1205) << "kdesud is already running\n";
	    return -1;
	}

    }

    sockfd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
	kdError(1205) << "socket(): " << ERR << "\n";
	return -1;
    }

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sock, sizeof(addr.sun_path)-1);
    addr.sun_path[sizeof(addr.sun_path)-1] = '\000';
    addrlen = SUN_LEN(&addr);
    if (bind(sockfd, (struct sockaddr *)&addr, addrlen) < 0) 
    {
	kdError(1205) << "bind(): " << ERR << "\n";
	return -1;
    }

    struct linger lin;
    lin.l_onoff = lin.l_linger = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_LINGER, (char *) &lin,
		   sizeof(linger)) < 0) 
    {
	kdError(1205) << "setsockopt(SO_LINGER): " << ERR << "\n";
	return -1;
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *) &opt,
		   sizeof(opt)) < 0) 
    {
	kdError(1205) << "setsockopt(SO_REUSEADDR): " << ERR << "\n";
	return -1;
    }
    opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *) &opt,
		   sizeof(opt)) < 0) 
    {
	kdError(1205) << "setsockopt(SO_KEEPALIVE): " << ERR << "\n";
	return -1;
    }
    chmod(sock, 0600);
    return sockfd;
}


/**
 * Main program
 */

int main(int argc, char *argv[])
{
    KAboutData aboutData("kdesud", I18N_NOOP("KDE su daemon"),
	    Version, I18N_NOOP("Daemon used by kdesu"),
	    KAboutData::License_Artistic,
	    "Copyright (c) 1999,2000 Geert Jansen");
    aboutData.addAuthor("Geert Jansen", I18N_NOOP("Author"),
	    "jansen@kde.org", "http://www.stack.nl/~geertj/");
    KCmdLineArgs::init(argc, argv, &aboutData);
    KInstance instance(&aboutData);

    // Set core dump size to 0
    struct rlimit rlim;
    rlim.rlim_cur = rlim.rlim_max = 0;
    if (setrlimit(RLIMIT_CORE, &rlim) < 0) 
    {
	kdError(1205) << "setrlimit(): " << ERR << "\n";
	exit(1);
    }

    // Create the Unix socket.
    int sockfd = create_socket();
    if (sockfd < 0)
	exit(1);
    if (listen(sockfd, 1) < 0) 
    {
	kdError(1205) << "listen(): " << ERR << "\n";
	kdesud_cleanup();
	exit(1);
    }
    int maxfd = sockfd;

    // Ok, we're accepting connections. Fork to the background.
    pid_t pid = fork();
    if (pid == -1) 
    {
	kdError(1205) << "fork():" << ERR << "\n";
	kdesud_cleanup();
	exit(1);
    }
    if (pid)
	exit(0);

    // Make sure we exit when the display gets closed.
    int x11Fd = initXconnection();
    maxfd = QMAX(maxfd, x11Fd);

    repo = new Repository;
    QVector<ConnectionHandler> handler;
    handler.setAutoDelete(true);

    // Signal handlers 
    struct sigaction sa;
    sa.sa_handler = signal_exit;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGHUP, &sa, 0L);
    sigaction(SIGINT, &sa, 0L);
    sigaction(SIGTERM, &sa, 0L);
    sigaction(SIGQUIT, &sa, 0L);

    sa.sa_handler = sigchld_handler;
    sa.sa_flags = SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, 0L);
    sa.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sa, 0L);

    // Main execution loop 

    socklen_t addrlen;
    struct sockaddr_un clientname;
    struct timeval tv;

    fd_set tmp_fds, active_fds;
    FD_ZERO(&active_fds);
    FD_SET(sockfd, &active_fds);
    if (x11Fd != -1)
	FD_SET(x11Fd, &active_fds);

    while (1) 
    {
	tmp_fds = active_fds;
	tv.tv_sec = 5; tv.tv_usec = 0;
	if (select(maxfd+1, &tmp_fds, 0L, 0L, &tv) < 0) 
	{
	    if (errno == EINTR)
		continue;
	    kdError(1205) << "select(): " << ERR << "\n";
	    exit(1);
	}
	repo->expire();
	for (int i=0; i<=maxfd; i++) 
	{
	    if (!FD_ISSET(i, &tmp_fds)) 
		continue;

	    if (i == x11Fd) 
	    {
		// Discard X events
		XEvent event_return;
		if (x11Display)
		    XNextEvent(x11Display, &event_return);
	    }

	    if (i == sockfd) 
	    {
		// Accept new connection
		int fd;
		addrlen = 64;
		fd = accept(sockfd, (struct sockaddr *) &clientname, &addrlen);
		if (fd < 0) 
		{
		    kdError(1205) << "accept():" << ERR << "\n";
		    continue;
		}
		if (fd+1 > (int) handler.size())
		    handler.resize(fd+1);
		handler.insert(fd, new ConnectionHandler(fd));
		maxfd = QMAX(maxfd, fd);
		FD_SET(fd, &active_fds);
		continue;
	    }

	    // handle alreay established connection
	    if (handler[i] && handler[i]->handle() < 0) 
	    {
		handler.remove(i);
		FD_CLR(i, &active_fds);
	    }
	}
    }
    kdWarning(1205) << "???\n";
}

