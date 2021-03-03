/* This file is part of the KDE libraries
    Copyright (c) 1999 Waldo Bastian <bastian@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include "ktempfile.h"
#include "kapp.h"
#include "kstddirs.h"
#include <qstring.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
   KApplication a(argc, argv, "ktempfiletest");
   printf("Making tempfile after KApplication constructor.\n");
   KTempFile f4;
   printf("Filename = %s\n", f4.name().ascii());

   printf("Making tempfile with \".ps\" extension.\n");
   KTempFile f2(QString::null, ".ps");
   printf("Filename = %s\n", f2.name().ascii());

   printf("Making tempfile in home directory.\n");
   KTempFile f3(QString((const char *)getenv("HOME"))+"/test", ".myext", 0666);
   printf("Filename = %s\n", f3.name().ascii());

   QString name = locateLocal("socket", "test");
   printf("Socket Filename = %s\n", name.ascii());

   printf("Done.\n");
}
