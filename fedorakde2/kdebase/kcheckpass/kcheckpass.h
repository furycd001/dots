/*****************************************************************
 *
 *	kcheckpass
 *
 *	Simple password checker. Just invoke and send it
 *	the password on stdin.
 *
 *	If the password was accepted, the program exits with 0;
 *	if it was rejected, it exits with 1. Any other exit
 *	code signals an error.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *	Copyright (C) 1998, Caldera, Inc.
 *	Released under the GNU General Public License
 *
 *	Olaf Kirch <okir@caldera.de>      General Framework and PAM support
 *	Christian Esken <esken@kde.org>   Shadow and /etc/passwd support
 *
 *      Other parts were taken from kscreensaver's passwd.cpp
 *****************************************************************/

#ifndef KCHECKPASS_H_
#define KCHECKPASS_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif

#ifdef HAVE_PATHS_H
#include <paths.h>
#endif

#include <pwd.h>
#include <sys/types.h>

#ifndef _PATH_TMP
#define _PATH_TMP "/tmp/"
#endif


#ifdef ultrix
#include <auth.h>
#endif

#include <unistd.h>

#ifdef OSF1_ENH_SEC
#include <sys/security.h>
#include <prot.h>
#endif

/* Default back to HAVE_ETCPASSWD */
#if !defined(HAVE_PAM) && !defined(HAVE_SHADOW) && !defined(HAVE_OSF_C2_PASSWD)
#define HAVE_ETCPASSWD
#endif

/* Make sure there is only one! */
#if defined(HAVE_PAM)
#undef HAVE_SHADOW
#undef HAVE_OSF_C2_PASSWD
#undef HAVE_ETCPASSWD
#endif

#if defined(HAVE_OSF_C2_PASSWD)
#undef HAVE_SHADOW
#undef HAVE_ETCPASSWD
#endif

#if defined(HAVE_SHADOW)
#undef HAVE_ETCPASSWD
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 * This function authenticates the user, returning
 *   1		if the password was accepted
 *   0		otherwise
 *****************************************************************/
int Authenticate(const char *login, const char *pass);

/*****************************************************************
 * Output a message to syslog (and to stderr as well, if available)
 *****************************************************************/
void message(const char *, ...);

#ifdef __cplusplus
}
#endif
#endif
