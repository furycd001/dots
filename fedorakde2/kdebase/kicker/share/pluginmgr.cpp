/*****************************************************************

Copyright (c) 1996-2000 the kicker authors. See file AUTHORS.

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

#include <qwidget.h>

#include <ksimpleconfig.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kdebug.h>

#include <qfile.h>
#include "pluginmgr.h"

KPanelApplet* KickerPluginManager::loadApplet(const QString &applnk,
					      const QString& configFile, QWidget* parent)
{
    static bool dlregistered = false;
    QString libStr, applnkPathStr;

    applnkPathStr = KGlobal::dirs()->findResource("applets", applnk);

    KSimpleConfig config(applnkPathStr);
    config.setDesktopGroup();
    libStr = config.readEntry("X-KDE-Library", "");
    libStr += ".la";
    libStr = KGlobal::dirs()->findResource("module", libStr);
    if(!libStr){
	kdWarning() << "cannot find " << libStr << " for " << applnk << endl;
	return(NULL);
    }

    if(!dlregistered){
	dlregistered = true;
	lt_dlinit();
    }

    void *handle = lt_dlopen(QFile::encodeName(libStr));
    if(!handle){
	kdWarning() << "cannot open applet: " << libStr << " because of " << lt_dlerror() << endl;
	return(NULL);
    }

    void *init_func = lt_dlsym(handle, "init");
    if(!init_func){
	kdWarning() << libStr << " is not a kicker applet!" << endl;
	lt_dlclose(handle);
	return(NULL);
    }

    KPanelApplet* (*init_ptr)(QWidget *, const QString&);
    init_ptr = (KPanelApplet* (*)(QWidget *, const QString&))init_func;
    KPanelApplet *w = init_ptr(parent, configFile);
    lt_dlhandle *tmp = new lt_dlhandle;
    *tmp = handle;
    handleDict.insert((long)w, tmp);
    return(w);
}

KPanelExtension* KickerPluginManager::loadExtension(const QString &applnk,
						 const QString& configFile, QWidget* parent)
{
    static bool dlregistered = false;
    QString libStr, applnkPathStr;

    applnkPathStr = KGlobal::dirs()->findResource("extensions", applnk);

    KSimpleConfig config(applnkPathStr);
    config.setDesktopGroup();
    libStr = config.readEntry("X-KDE-Library", "");
    libStr += ".la";
    libStr = KGlobal::dirs()->findResource("module", libStr);
    if(!libStr){
	kdWarning() << "cannot find " << libStr << " for " << applnk << endl;
	return(NULL);
    }

    if(!dlregistered){
	dlregistered = true;
	lt_dlinit();
    }

    lt_dlhandle handle = lt_dlopen(QFile::encodeName(libStr));
    if(!handle){
	kdWarning() << "cannot open extension: " << libStr << " because of " << lt_dlerror() << endl;
	return(NULL);
    }

    void *init_func = lt_dlsym(handle, "init");
    if(!init_func){
	kdWarning() << libStr << " is not a kicker extension!" << endl;
	lt_dlclose(handle);
	return(NULL);
    }

    KPanelExtension* (*init_ptr)(QWidget *, const QString&);
    init_ptr = (KPanelExtension* (*)(QWidget *, const QString&))init_func;
    KPanelExtension *w = init_ptr(parent, configFile);
    lt_dlhandle *tmp = new lt_dlhandle;
    *tmp = handle;
    handleDict.insert((long)w, tmp);
    return(w);
}

bool KickerPluginManager::removePlugin(QWidget *w)
{
  lt_dlhandle *handle = handleDict.find((long)w);
  if(!handle){
    kdWarning() << "Cannot find handle to remove plugin!" << endl;
    return(false);
  }
  handleDict.remove((long)w);
  delete w;
  if(!lt_dlclose(*handle)){
    kdWarning() << "Unable to unload plugin!" << endl;
    return(false);
  }
  return(true);
}





