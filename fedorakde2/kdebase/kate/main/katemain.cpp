/***************************************************************************
                          katemain.cpp  -  description
                             -------------------
    begin                : Wed Jan 3 2001
    copyright            : (C) 2001 by Christoph "Crossfire" Cullmann
    email                : crossfire@babylon2k.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kstddirs.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kstdaction.h>
#include <kaboutdata.h>
#include <kglobal.h>
#include <kglobalaccel.h>
#include <kglobalsettings.h>
#include <kstartupinfo.h>
#include <dcopclient.h>
#include <kurl.h>

#include "../app/kateapp.h"
#include "../factory/katefactory.h"

#include <stdio.h>

static KCmdLineOptions options[] =
{
    { "n", I18N_NOOP("start a new Kate (off by default)"), 0 },
    { "+file(s)",          I18N_NOOP("Files to load"), 0 },
    { 0,0,0 }
};

int main( int argc, char **argv )
{
  KCmdLineArgs::init (argc, argv, KateFactory::aboutData());
  KCmdLineArgs::addCmdLineOptions (options);
  KCmdLineArgs* args = KCmdLineArgs::parsedArgs();

  bool running = false;

  DCOPClient *client=0L;
  QCString appID = "";

  if (!args->isSet ("n"))
  {
    client  = new DCOPClient ();
    client->attach();

    QCStringList apps = client->registeredApplications();
    for ( QCStringList::Iterator it = apps.begin(); it != apps.end(); ++it )
    {
      if  ((*it).contains ("kate") > 0)
      {
        appID = (*it);

        QByteArray ba,da;
        QCString replyType;
        if (!(client->call(appID,"KateApp","isSingleInstance()",da,replyType,ba,true)))
	  running = false;
        else
          {
            if (replyType!="QString") running=false;
              else
                {
		   QDataStream reply(ba, IO_ReadOnly);
                   QString result;
                   reply>>result;
		   running=(result=="true");
                }
          }
	break;
      }
    }
  }

  if (running)
  {
    for (int z=0; z<args->count(); z++)
    {
      QByteArray data;
      QDataStream arg(data, IO_WriteOnly);

      arg << args->url(z).url();
      client->send (appID, "KateApp", "openURL(QString)", data);
    }
    KStartupInfo::appStarted();
  }
  else
  {
    KateApp::addCmdLineOptions ();
    KateApp app;
    return app.exec();
  }

  return 0;
}
