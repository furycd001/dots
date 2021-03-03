/*****************************************************************

Copyright (c) 2000 Matthias Elter

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

#undef Bool // For enable-final
#include <kwinmodule.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kprocess.h>

#include "dockbarextension.h"
#include "dockbarextension.moc"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

extern "C"
{
    KPanelExtension* init(QWidget *parent, const QString& configFile)
    {
	return new DockBarExtension(configFile, KPanelExtension::Normal,
				    0, parent, "dockbarextension");
    }
}

DockBarExtension::DockBarExtension(const QString& configFile, Type type,
				   int actions, QWidget *parent, const char *name)
    : KPanelExtension(configFile, type, actions, parent, name)
{
    containers.setAutoDelete(false);
    kwin_module = new KWinModule(this);
    connect( kwin_module, SIGNAL( windowAdded(WId) ), SLOT( windowAdded(WId) ) );
    
    loadContainerConfig();
}

DockBarExtension::~DockBarExtension()
{
}

QSize DockBarExtension::sizeHint(Position p, QSize) const
{
    if (p == Left || p == Right)
	return QSize(68, 68 * containers.count());
    else
	return QSize(68 * containers.count(), 68);
}

void DockBarExtension::resizeEvent(QResizeEvent*)
{
    layoutContainers();   
}

void DockBarExtension::windowAdded(WId win)
{
    WId resIconwin;
    QString resClass, resName;
     
    // try to read wm hints
    XWMHints *wmhints = XGetWMHints(qt_xdisplay(), win);
    
    if (0 != wmhints) { // we managed to read wm hints
	// read IconWindowHint
	if (wmhints->flags & IconWindowHint) {
	    resIconwin = wmhints->icon_window;
	    kdDebug() << "IconWindowHint: " << resIconwin << endl;
	}
	else
	    return;
    }
    else
	return;
    
    //if (resIconwin <= 0)
    resIconwin = win;
    
    // try to read class hint
    XClassHint hint;
    Status ok = XGetClassHint(qt_xdisplay(), win, &hint);
    
    if (0 != ok) { // we managed to read the class hint
        resName =  hint.res_name;
        resClass = hint.res_class;
    }
    else
	kdDebug() << "Could not read XClassHint." << endl;
    
    // add a container
    addContainer(resIconwin, resClass, resName);
    saveContainerConfig();
    emit updateLayout();
}

void DockBarExtension::layoutContainers()
{
    int i = 0;
    for (DockContainer *c = containers.first(); c != 0; c = containers.next()) {
	
	if (orientation() == Horizontal)
	    c->move(68 * i, 0);
	else
	    c->move(0, 68 * i);
	i++;
    }   
}

void DockBarExtension::addContainer(WId win, QString command, QString)
{
    if(win == 0) return;
    
    DockContainer* c = new DockContainer(command, this);
    containers.append(c);
    connect(c, SIGNAL(embededWindowDestroyed(DockContainer*)), SLOT(embededWindowDestroyed(DockContainer*)));
    
    c->resize(68, 68);
    c->show();
    c->embed(win);
}

void DockBarExtension::removeContainer(DockContainer* c)
{
    containers.remove(c);
    delete c;
}

void DockBarExtension::embededWindowDestroyed(DockContainer* c)
{
    kdDebug() << "embededWindowDestroyed" << endl;
    removeContainer(c);
    saveContainerConfig();
    emit updateLayout();
}

void DockBarExtension::saveContainerConfig()
{
    QStringList commands;
    
    for (DockContainer *c = containers.first(); c != 0; c = containers.next())
	commands.append(c->command());
    
    KConfig *c = config();
    c->setGroup("General");
    c->writeEntry("Commands", commands);
    c->sync();
}

void DockBarExtension::loadContainerConfig()
{
    KConfig *c = config();
    c->setGroup("General");

    QStringList commands = c->readListEntry("Commands");
    
    for (QStringList::Iterator it = commands.begin(); it != commands.end(); ++it) {
	KProcess proc;
	proc << *it;
	proc.start(KProcess::DontCare);
    }	
}
