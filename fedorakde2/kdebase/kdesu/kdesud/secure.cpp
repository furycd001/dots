/* vi: ts=8 sts=4 sw=4
 *
 * $Id: secure.cpp,v 1.6 2001/03/29 08:46:32 leitner Exp $
 *
 * This file is part of the KDE project, module kdesu.
 * Copyright (C) 1999,2000 Geert Jansen <g.t.jansen@stud.tue.nl>
 *
 * secure.cpp: Peer credentials for a UNIX socket.
 */

#include <config.h>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include <kdebug.h>
#include "secure.h"


/**
 * Under Linux, Socket_security is supported.
 */

#if defined(SO_PEERCRED)

SocketSecurity::SocketSecurity(int sockfd)
{
    socklen_t len = sizeof(struct ucred);
    if (getsockopt(sockfd, SOL_SOCKET, SO_PEERCRED, &cred, &len) < 0) {
	kdError() << "getsockopt(SO_PEERCRED) " << perror << endl;
	return;
    }

    ok = true;
}

#else


/**
 * The default version does nothing.
 */

SocketSecurity::SocketSecurity(int sockfd)
{
    static bool warned_him = FALSE;

    if (!warned_him) {
        kdWarning() << "Using void socket security. Please add support for your" << endl;
        kdWarning() << "platform to kdesu/kdesud/secure.cpp" << endl;
        warned_him = TRUE;
    }

    // This passes the test made in handler.cpp
    cred.uid = getuid();
    ok = true;
}

#endif
