/*****************************************************************
 * drkonqi - The KDE Crash Handler
 *
 * $Id: main.cpp,v 1.23 2001/05/20 01:42:17 mhunter Exp $
 *
 * Copyright (C) 2000 Hans Petter Bieker <bieker@kde.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************/

#include <config.h>

#include <stdlib.h>
#include <unistd.h>

#include <kapp.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

#include "krashconf.h"
#include "toplevel.h"

static const char *version = "1.0";
static const char *description = I18N_NOOP( "KDE Crash Handler gives the user feedback if a program crashed." );

static const KCmdLineOptions options[] =
{
  {"signal <number>", I18N_NOOP("The signal number we caught."), 0},
  {"appname <name>",  I18N_NOOP("Name of the program."), 0},
  {"apppath <path>",  I18N_NOOP("Path to the executable."), 0},
  {"appversion <version>", I18N_NOOP("The version of the program."), 0},
  {"bugaddress <address>", I18N_NOOP("The bug address to use."), 0},
  {"programname <name>", I18N_NOOP("Translated name of the program."), 0},
  {"pid <pid>", I18N_NOOP("The PID of the program"), 0},
  {"kdeinit", I18N_NOOP("The program was started by kdeinit"), 0},
  { 0, 0, 0 }
};

int main( int argc, char* argv[] )
{
  // Drop privs.
  setgid(getgid());
  setuid(getuid());

  // Make sure that DrKonqi doesn't start DrKonqi when it crashes :-]
  setenv("KDE_DEBUG", "true", 1);

  KAboutData aboutData( "drkonqi",
			I18N_NOOP("The KDE Crash Handler"),
			version,
			description,
			KAboutData::License_BSD,
			"(C) 2000, Hans Petter Bieker");
  aboutData.addAuthor("Hans Petter Bieker", 0, "bieker@kde.org");

  KCmdLineArgs::init(argc, argv, &aboutData);
  KCmdLineArgs::addCmdLineOptions( options );

  KApplication a;
  a.disableSessionManagement();

  KrashConfig krashconf;

  Toplevel w(&krashconf);

  return w.exec();
}
