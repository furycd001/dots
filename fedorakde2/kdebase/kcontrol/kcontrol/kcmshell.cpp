/*
  Copyright (c) 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
  Copyright (c) 2000 Matthias Elter <elter@kde.org>

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

*/

#include <stdlib.h>

#include <qfile.h>

#include <dcopclient.h>

#include <kapp.h>
#include <kglobal.h>
#include <kdebug.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kcmdlineargs.h>
#include <kservice.h>
#include <kdesktopfile.h>
#include <qxembed.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <klibloader.h>

#include "kcdialog.h"
#include "kecdialog.h"
#include "moduleinfo.h"
#include "modloader.h"
#include "global.h"
#include "kcmshell.h"
#include "proxywidget.h"

static KCmdLineOptions options[] =
{
    { "list", I18N_NOOP("List all possible modules"), 0},
    { "+module", I18N_NOOP("Configuration module to open."), 0 },
    { "embed <id>", I18N_NOOP("Window ID to embed into."), 0 },
    KCmdLineLastOption
};

static QString locateModule(const QCString module)
{
    // locate the desktop file
    //QStringList files;
    if (module[0] == '/')
    {
        kdDebug() << "Full path given to kcmshell - not supported yet" << endl;
        // (because of KService::findServiceByDesktopPath)
        //files.append(args->arg(0));
    }

    QString path = KCGlobal::baseGroup();
    path += module;
    path += ".desktop";

    if (!KService::serviceByDesktopPath( path ))
    {
        // Path didn't work. Trying as a name
        KService::Ptr serv = KService::serviceByDesktopName( module );
        if ( serv )
            path = serv->entryPath();
        else
        {
            kdError(1208) << i18n("Module %1 not found!").arg(module) << endl;
            return QString::null;
        }
    }
    return path;
}

void 
kcmApplication::setDCOPName(const QCString &dcopName)
{
    m_dcopName = "kcmshell_"+dcopName;
    dcopClient()->registerAs(m_dcopName, false);
}

bool
kcmApplication::isRunning()
{
    if (dcopClient()->appId() == m_dcopName)
       return false; // We are the one and only.
    dcopClient()->attach(); // Reregister as anonymous

    dcopClient()->setNotifications(true);

    QCString replyType;
    QByteArray replyData;
    if (!dcopClient()->call(m_dcopName, "dialog", "activate()", QByteArray(),
		replyType, replyData))
    {
        return false; // Error, we have to do it ourselves.
    }
    return true;
}

void
kcmApplication::waitForExit()
{
    connect(dcopClient(), SIGNAL(applicationRemoved(const QCString&)),
            this, SLOT(slotAppExit(const QCString&)));
    exec();
}

void
kcmApplication::slotAppExit(const QCString &appId)
{
    if (appId == m_dcopName)
        quit();
}


int main(int _argc, char *_argv[])
{
    KAboutData aboutData( "kcmshell", I18N_NOOP("KDE Control Module"),
                          "2.0",
                          I18N_NOOP("A tool to start single KDE control modules"),
                          KAboutData::License_GPL,
                          "(c) 1999-2000, The KDE Developers");
    aboutData.addAuthor("Matthias Hoelzer-Kluepfel",0, "hoelzer@kde.org");
    aboutData.addAuthor("Matthias Elter",0, "elter@kde.org");

    KCmdLineArgs::init(_argc, _argv, &aboutData);
    KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.
    KLocale::setMainCatalogue("kcontrol");
    kcmApplication app;

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    KGlobal::iconLoader()->addAppDir( "kcontrol" );

    if (args->isSet("list")) {
        QStringList files;
        KGlobal::dirs()->findAllResources("apps",
                                          KCGlobal::baseGroup() + "*.desktop",
                                          true, true, files);
        QStringList modules;
        QStringList descriptions;
        uint maxwidth = 0;
        for (QStringList::ConstIterator it = files.begin(); it != files.end(); it++) {

            if (KDesktopFile::isDesktopFile(*it)) {
                KDesktopFile file(*it, true);
                QString module = *it;
                if (module.startsWith(KCGlobal::baseGroup()))
                    module = module.mid(KCGlobal::baseGroup().length());
                if (module.right(8) == ".desktop")
                    module.truncate(module.length() - 8);

                modules.append(module);
                if (module.length() > maxwidth)
                    maxwidth = module.length();
                descriptions.append(QString("%2 (%3)").arg(file.readName()).arg(file.readComment()));
            }
        }

        QByteArray vl;
        vl.fill(' ', 80);
        QString verylong = vl;

        for (uint i = 0; i < modules.count(); i++) {
	    fprintf(stdout, "%s%s - %s\n",
		    (*modules.at(i)).local8Bit().data(),
		    verylong.left(maxwidth - (*modules.at(i)).length()).local8Bit().data(),

		    (*descriptions.at(i)).local8Bit().data());
        }

        return 0;
    }

    if (args->count() < 1) {
        args->usage();
        return -1;
    }

    if (args->count() == 1) {
        app.setDCOPName(args->arg(0));
        if (app.isRunning())
        {
           app.waitForExit();
           return 0;
        }

        QString path = locateModule(args->arg(0));
        if (path.isEmpty())
           return 0;

        // load the module
        ModuleInfo info(path);

        KCModule *module = ModuleLoader::loadModule(info, false);

        if (module) {
            // create the dialog
            QCString embedStr = args->getOption("embed");
            bool embed = false;
            int id;
            if (!embedStr.isEmpty()) 
               id = embedStr.toInt(&embed);
            
            if (!embed)
            {
               KCDialog * dlg = new KCDialog(module, module->buttons(), info.docPath(), 0, 0, true );
               dlg->setCaption(info.name());

               // Needed for modules that use d'n'd (not really the right
               // solution for this though, I guess)
               dlg->setAcceptDrops(true);
	   
               // run the dialog
               dlg->exec();
               delete dlg;
            }
            // if we are going to be embedded, embed
            else
            {
               QWidget *dlg = new ProxyWidget(module, info.name(), "kcmshell", false);
               // Needed for modules that use d'n'd (not really the right
               // solution for this though, I guess)
               dlg->setAcceptDrops(true);

               QXEmbed::embedClientIntoWindow(dlg, id);
               kapp->exec();
               delete dlg;
            }
            ModuleLoader::unloadModule(info);
            return 0;
        }

        KMessageBox::error(0,
                           i18n("There was an error loading the module.\nThe diagnostics is:\n%1")
                           .arg(KLibLoader::self()->lastErrorMessage()));
        return 0;
    }

    // multiple control modules
    QStringList modules;
    for (int i = 0; i < args->count(); i++) {
        QString path = locateModule(args->arg(i));
        if(!path.isEmpty())
            modules.append(path);
    }

    if (modules.count() < 1) return -1;

    // create the dialog
    KExtendedCDialog * dlg = new KExtendedCDialog(0, 0, true);

    // Needed for modules that use d'n'd (not really the right
    // solution for this though, I guess)
    dlg->setAcceptDrops(true);

    // add modules
    for (QStringList::ConstIterator it = modules.begin(); it != modules.end(); ++it)
        dlg->addModule(*it, false);

    // if we are going to be embedded, embed
    QCString embed = args->getOption("embed");
    if (!embed.isEmpty())
    {
        bool ok;
        int id = embed.toInt(&ok);
        if (ok)
	    {
              // NOTE: This has to be changed for QT 3.0. See above!
              QXEmbed::embedClientIntoWindow(dlg, id);
	      delete dlg;
	      return 0;
	    }
    }

    // run the dialog
    dlg->exec();
    delete dlg;
    return 0;
}
#include "kcmshell.moc"
