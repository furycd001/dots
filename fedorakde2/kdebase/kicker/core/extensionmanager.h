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

#ifndef __extensionmanager_h__
#define __extensionmanager_h__

#include <qlist.h>
#include <qobject.h>

#include <netwm_def.h>

#include "container_extension.h"

class ExtensionManager : public QObject
{
    Q_OBJECT

public:
    ExtensionManager(QObject* parent = 0, const char* name = 0);
    virtual ~ExtensionManager();

    void addExtension(const QString &desktopFile, bool internal = true);

    QRect effectiveWorkArea();

    bool hasInstance(AppletInfo*) const;

protected:
    void addContainer(ExtensionContainer*);
    void removeContainer(ExtensionContainer *);
    void removeAllContainers();

    void setUniqueId(ExtensionContainer*);

    void saveContainerConfig(bool layoutOnly = false);
    void loadContainerConfig();
    void defaultContainerConfig();


protected slots:
    void slotAddExternal(ExternalExtensionContainer*);
    void embeddedWindowDestroyed();
    void slotSaveContainerConfig();
    void slotRemoveContainer(ExtensionContainer*);

private:
    QList<ExtensionContainer> _containers;
    bool                      _init;
};

#endif

