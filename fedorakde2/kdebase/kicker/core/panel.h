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

#ifndef __panel_h__
#define __panel_h__

#include <qbutton.h>
#include <dcopobject.h>

#include "global.h"
#include "container_panel.h"

class ContainerArea;

class Panel : public PanelContainer, virtual public DCOPObject
{
    Q_OBJECT
    K_DCOP

public:
    Panel();
    ~Panel();

k_dcop:
    int panelSize() { return static_cast<int>(size()); }
    int panelOrientation() { return static_cast<int>(orientation()); }
    int panelPosition() { return static_cast<int>(position()); }

    void setPanelSize(int size) { setSize(static_cast<Size>(size)); }
    void setPanelPosition(int position) { setPosition(static_cast<Position>(position)); }

    // It makes sense to have these in Panel and not in ContainerArea, think of child panels.
    void addKMenuButton();
    void addDesktopButton();
    void addWindowListButton();
    void addURLButton(const QString &url);
    void addBrowserButton(const QString &startDir);
    void addServiceMenuButton(const QString &name, const QString& relPath);
    void addExeButton(const QString &filePath, const QString &icon, const QString &cmdLine, bool inTerm);
    void addKonsoleButton();

    void addApplet( const QString &desktopFile  );
    void addExtension( const QString &desktopFile );

    void configure();
    void restart();

public:
    Size size() const { return _size; }
    void setSize(Size s);
    QSize sizeHint( Position p, QSize maxSize );

    ContainerArea *containerArea() { return(_containerArea); }

    void readConfig();
    void writeConfig();

signals:
    void configurationChanged();

protected:
    void autoHide(bool hide);
    void closeEvent( QCloseEvent * );

protected slots:
    void slotSetPosition( Position p );
    void slotRestart();
    void scrollLeftUp();
    void scrollRightDown();

private:
    Size _size;
    int _panelsize;

    ContainerArea    *_containerArea;
};

#endif // __panel_h__
