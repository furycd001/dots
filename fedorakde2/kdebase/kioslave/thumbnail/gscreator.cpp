/*  This file is part of the KDE libraries
    Copyright (C) 2001 Malte Starostik <malte@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

// $Id: gscreator.cpp,v 1.5.2.1 2001/09/05 00:36:58 malte Exp $

#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/wait.h>

#include <qfile.h>
#include <qimage.h>

#include "gscreator.h"

extern "C"
{
    ThumbCreator *new_creator()
    {
        return new GSCreator;
    }
};

// This PS snippet will be prepended to the actual file so that only
// the first page is output.
static const char *prolog =
    "%!PS-Adobe-3.0\n"
    "/.showpage.orig /showpage load def\n"
    "/.showpage.firstonly {\n"
    "    .showpage.orig\n"
    "    quit\n"
    "} def\n"
    "/showpage { .showpage.firstonly } def\n";

static const char *gsargs[] = {
    "gs",
    "-sDEVICE=png16m",
    "-sOutputFile=-",
    "-dNOPAUSE",
    "-dFirstPage=1",
    "-dLastPage=1",
    "-q",
    "-",
    0, // file name
	"-c",
	"showpage",
	"-c",
	"quit",
    0
};

bool GSCreator::create(const QString &path, int, int, QImage &img)
{
    int input[2];
    int output[2];
    QByteArray data(1024);
    bool ok = false;

    if (pipe(input) == -1)
        return false;
    if (pipe(output) == -1)
    {
        close(input[0]);
        close(input[1]);
        return false;
    }

    pid_t pid = fork();
    if (pid == 0)
    {
        // Child process, reopen stdin/stdout on the pipes and exec gs
        close(input[1]);
        close(output[0]);
        dup2(input[0], STDIN_FILENO);
        dup2(output[1], STDOUT_FILENO);
        close(STDERR_FILENO);

        // find first zero entry and put the filename there
        const char **arg = gsargs;
        while (*arg)
            ++arg;
        *arg = path.latin1();

        execvp(gsargs[0], const_cast<char *const *>(gsargs));
        exit(1);
    }
    else if (pid != -1)
    {
        // Parent process, write first-page-only-hack
        // and read the png output
        close(input[0]);
        close(output[1]);

        int count = write(input[1], prolog, strlen(prolog));
        close(input[1]);
        if (count == static_cast<int>(strlen(prolog)))
        {
            int offset = 0;
            while (!ok)
            {
                fd_set fds;
                FD_ZERO(&fds);
                FD_SET(output[0], &fds);
                struct timeval tv;
                tv.tv_sec = 5;
                tv.tv_usec = 0;
                if (select(output[0] + 1, &fds, 0, 0, &tv) <= 0)
                    break; // error or timeout

                if (FD_ISSET(output[0], &fds))
                {
                    count = read(output[0], data.data() + offset, 1024);
                    if (count == -1)
                        break;
                    else if (count) // prepare for next block
                    {
                        offset += count;
                        data.resize(offset + 1024);
                    }
                    else // got all data
                    {
                        data.resize(offset);
                        ok = true;
                    }
                }
            }
        }
        if (!ok) // error or timeout, gs probably didn't exit yet
            kill(pid, SIGTERM);

        int status;
        if (waitpid(pid, &status, 0) != pid || status != 0)
            ok = false;
    }
    else
    {
        // fork() failed, close these
        close(input[0]);
        close(input[1]);
        close(output[0]);
    }
    close(output[1]);

    return ok && img.loadFromData( data );
}

ThumbCreator::Flags GSCreator::flags() const
{
    return static_cast<Flags>(DrawFrame);
}
