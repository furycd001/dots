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

#ifndef __dockbarextension_h__
#define __dockbarextension_h__

#include <qlist.h>
#include <kpanelextension.h>
#include "dockcontainer.h"

class KWinModule;

class DockBarExtension : public KPanelExtension
{
    Q_OBJECT

public:
    DockBarExtension(const QString& configFile, Type t = Normal,
		     int actions = 0, QWidget *parent = 0, const char *name = 0);

    virtual ~DockBarExtension();

    QSize sizeHint(Position, QSize maxSize) const;
    Position preferedPosition() const { return Right; }

protected slots:
    void windowAdded(WId);
    void embededWindowDestroyed(DockContainer*);

protected:
    void resizeEvent(QResizeEvent*);
    void addContainer(WId win, QString command, QString name);
    void removeContainer(DockContainer*);
    void saveContainerConfig();
    void loadContainerConfig();
    void layoutContainers();

private:
    KWinModule* kwin_module;
    QList<DockContainer> containers;
};

#endif
