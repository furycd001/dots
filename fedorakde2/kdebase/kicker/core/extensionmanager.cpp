/*****************************************************************

Copyright (c) 2000 Matthias Elter <elter@kde.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include <kapp.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kdebug.h>
#include <kwinmodule.h>
#include <kwin.h>
#include <kconfig.h>

#include "global.h"
#include "appletinfo.h"
#include "userrectsel.h"
#include "container_extension.h"
#include "extensionmanager.h"
#include "extensionmanager.moc"

ExtensionManager::ExtensionManager(QObject* parent, const char* name)
    : QObject(parent, name), _init (false)
{
    _containers.setAutoDelete(false);

    // restore extension layout or load a default layout
    KConfig* config = KGlobal::config();
    config->setGroup("General");

    if(config->hasKey("Extensions2"))
	loadContainerConfig();
    else
	defaultContainerConfig();
}

ExtensionManager::~ExtensionManager()
{
    removeAllContainers();
}

void ExtensionManager::defaultContainerConfig()
{
//    kdDebug(1210) << "ExtensionManager::defaultContainerConfig()" << endl;
    removeAllContainers();

    /*
    // external taskbar
    QString df = KGlobal::dirs()->findResource("extensions", "ktaskbarextension.desktop");
    InternalExtensionContainer *e1 = new InternalExtensionContainer(AppletInfo(df));
    e1->slotSetPosition(Bottom);
    addContainer(e1);
    */

    saveContainerConfig();
}

void ExtensionManager::saveContainerConfig(bool layoutOnly )
{
//    kdDebug(1210) << "ExtensionManager::saveContainerConfig()" << endl;

    KConfig *config = KGlobal::config();
    config->setGroup("General");

    // build the extension list
    QStringList elist;

    QListIterator<ExtensionContainer> it(_containers);
    for(; it.current() ; ++it)
	elist.append( it.current()->extensionId() );

    // write extension list
    config->writeEntry("Extensions2", elist);

    // write extension config
    it.toFirst();
    for(; it.current() ; ++it)
	{
	    ExtensionContainer* a = it.current();

	    // let the applet container write custom data fields
	    if(!layoutOnly)
		a->writeConfig();
	}
    config->sync();
}

void ExtensionManager::loadContainerConfig()
{
//    kdDebug(1210) << "ExtensionManager::loadContainerConfig()" << endl;

    KConfig *config = KGlobal::config();

    removeAllContainers();

    // read extension list
    config->setGroup("General");
    QStringList elist = config->readListEntry("Extensions2");
    QStringList trusted = config->readListEntry("TrustedExtensions2");
    int securityLevel = config->readNumEntry("SecurityLevel", 1);

    // now restore the extensions
    QStringList::Iterator it = elist.begin();
    while(it != elist.end())
    {
        // extension id
        QString extensionId(*it);
        QString group = extensionId;

        // is there a config group for this extension?
        if(!config->hasGroup(group))
            continue;

        // create a matching applet container
        if (!extensionId.contains("Extension") > 0)
        {
            it++;
            continue;
        }

        // set config group
        config->setGroup(group);

        ExtensionContainer* e = 0;
        QString df = KGlobal::dirs()->findResource("extensions", config->readEntry("DesktopFile"));
        AppletInfo info(df);

        QString configFile = config->readEntry("ConfigFile");
        if (!configFile.isNull())
            info.setConfigFile(configFile);

        config->setGroup("General");

        // child panels can be internal only
        bool ischildpanel = (df.contains("childpanelextension") > 0);

        if(securityLevel == 0 && !ischildpanel)
        {
            QString lib = info.library().mid(3, info.library().length());
            bool trustedextension = false;
            for ( QStringList::Iterator it = trusted.begin(); it != trusted.end(); ++it )
            {
                if ((*it) == lib)
                {
                    trustedextension = true;
                    break;
                }
            }

            if (trustedextension)
                e = new InternalExtensionContainer(info);
            else
                e = new ExternalExtensionContainer(info);
        }
        else
            e = new InternalExtensionContainer(info);

        addContainer(e);
        it++;
    }

}

void ExtensionManager::addExtension(const QString &desktopFile, bool internal)
{
    QString df = KGlobal::dirs()->findResource("extensions", desktopFile);
    AppletInfo info(df);

    ExtensionContainer *e;

    // child panels can be internal only
    if (desktopFile.contains("childpanelextension") > 0) internal = true;

    if (internal) {
	e = new InternalExtensionContainer(info);
	addContainer(e);
	saveContainerConfig();
    }
    else {
	e = new ExternalExtensionContainer(info);
	connect(e, SIGNAL(docked(ExternalExtensionContainer*)),
		    SLOT(slotAddExternal(ExternalExtensionContainer*)));
    }
}

void ExtensionManager::slotAddExternal(ExternalExtensionContainer* e)
{
    addContainer(e);
    saveContainerConfig();
}

void ExtensionManager::addContainer(ExtensionContainer* e)
{
    if (!e) return;

    setUniqueId(e);
    _containers.append(e);

    connect(e, SIGNAL(removeme(ExtensionContainer*) ),
	    SLOT( slotRemoveContainer(ExtensionContainer*)));

    if (e->inherits("ExternalExtensionContainer"))
	connect(e, SIGNAL(embeddedWindowDestroyed() ), this,
		SLOT( embeddedWindowDestroyed()));

    e->show();
}

void ExtensionManager::removeContainer(ExtensionContainer *a)
{
    if (a) {
	a->removeSessionConfigFile();
	delete a;
	_containers.removeRef(a);
    }

    saveContainerConfig(true);
}

void ExtensionManager::removeAllContainers()
{
    while (!_containers.isEmpty()) {
	ExtensionContainer* e = _containers.first();
	_containers.removeRef(e);
	delete e;
    }
}

void ExtensionManager::setUniqueId(ExtensionContainer* e)
{
    QString idBase = "Extension_%1";
    QString newId;
    int i = 0;
    bool unique = false;

    while(!unique)
	{
	    i++;
	    newId = idBase.arg(i);

	    unique = true;
	    QListIterator<ExtensionContainer> it(_containers);
	    for(; it.current() ; ++it)
		{
		    ExtensionContainer* b = static_cast<ExtensionContainer*>(it.current());
		    if (b->extensionId() == newId)
			{
			    unique = false;
			    break;
			}
		}
	}
    e->setExtensionId(newId);
}

void ExtensionManager::embeddedWindowDestroyed()
{
    if (sender() && sender()->inherits("ExternalExtensionContainer"))
	removeContainer((ExternalExtensionContainer*)sender());
}

void ExtensionManager::slotSaveContainerConfig()
{
    saveContainerConfig();
}

void ExtensionManager::slotRemoveContainer(ExtensionContainer* c)
{
    removeContainer(c);
}

QRect ExtensionManager::effectiveWorkArea()
{
    QValueList<WId> list;
    for (ExtensionContainer* e = _containers.first(); e != 0; e = _containers.next()) {
	if (e->isVisible())
	    list.append(e->winId());
    }

    return (PGlobal::kwin_module->workArea(list));
}

bool ExtensionManager::hasInstance(AppletInfo* info) const
{
    bool found = false;

    for (QListIterator<ExtensionContainer> it(_containers); it.current(); ++it ) {
        ExtensionContainer *a = static_cast<ExtensionContainer*>(it.current());
        if (a->info().library() == info->library()) {
            found = true;
            break;
        }
    }
    return found;
}
