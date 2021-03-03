/**************************************************************************

    main.cpp  - The main function for KPager
    Copyright (C) 1998-2000  Antonio Larrosa Jimenez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Send comments and bug fixes to larrosa@kde.org

***************************************************************************/

#include <kuniqueapp.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <qcolor.h>
#include <kdebug.h>
#include <stdlib.h>

#include "kpager.h"

static KCmdLineOptions pagerOpts[] =
{
    { "parent <WId>", I18N_NOOP("Create pager as a child of specified parent window"), "0" }
};

int main(int argc, char **argv)
{
    KAboutData *aboutdata = new KAboutData("kpager", "KPager", "1.5",
					   "Desktop Overview", KAboutData::License_GPL,
					   "(C) 1998,99,2000, Antonio Larrosa Jimenez","",
					   "http://perso.wanadoo.es/antlarr/kpager.html");

    aboutdata->addAuthor("Antonio Larrosa Jimenez",
			 "Original Developer/Mantainer","larrosa@kde.org",
			 "http://perso.wanadoo.es/antlarr/index.html");
    aboutdata->addAuthor("Matthias Elter",
			 "Developer","elter@kde.org", "");
    aboutdata->addAuthor("Matthias Ettrich",
			 "Developer","ettrich@kde.org", "");

    KCmdLineArgs::init(argc, argv, aboutdata);
    KCmdLineArgs::addCmdLineOptions(pagerOpts);

    // parce parent option
    // can't use KCmdLineArgs because need to read this option before kapp initialization
    WId parent = 0;
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            QString strParam(argv[i]);
            if (QString(argv[i]) == "-parent" && i+1 < argc)
            {
                long l = atol(argv[i+1]);
                parent = (WId)l;
                break;
            }
        }
    }

    KApplication *app;
    QWidget *kpager;
    if (!parent)
    {
        if (!KUniqueApplication::start())
        {
            kdError() << "kpager is already running!" << endl;
            return 0;
        }

        app = new KUniqueApplication;

        KMainWindow *pMW = new KPagerMainWindow(0,"KPager");
        pMW->setPlainCaption( i18n("Desktop Pager") );
        kpager = pMW;
    }
    else // we already have the main window
    {
        app = new KApplication;
        app->disableSessionManagement();
        kpager = new KPager(0,"KPager", parent);
    }

    app->setMainWidget(kpager);
    kpager->show();

    int ret = app->exec();

    delete app;
    return ret;
};

