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

#ifndef __childpanelextension_h__
#define __childpanelextension_h__

#include <kpanelextension.h>
#include <dcopobject.h>

class ContainerArea;
class ContainerAreaBox;
class QPopupMenu;

class ChildPanelExtension : public KPanelExtension, virtual public  DCOPObject
{
    Q_OBJECT
    K_DCOP

public:
    ChildPanelExtension(const QString& configFile, Type t = Normal,
			int actions = 0, QWidget *parent = 0, const char *name = 0);

    virtual ~ChildPanelExtension();

    k_dcop:
    int panelSize() { return _size; }
    int panelOrientation() { return static_cast<int>(orientation()); }
    int panelPosition() { return static_cast<int>(position()); }

    void setPanelSize(int size) { slotSetSize(size); }
    void addKMenuButton();
    void addDesktopButton();
    void addWindowListButton();
    void addURLButton(const QString &url);
    void addBrowserButton(const QString &startDir);
    void addServiceMenuButton(const QString &name, const QString& relPath);
    void addExeButton(const QString &filePath, const QString &icon, const QString &cmdLine, bool inTerm);

    void addApplet(const QString &desktopFile, bool internal);
    void addAppletContainer(const QString &desktopFile, bool internal); // bad name, can this go away?####

public:
    QSize sizeHint(Position, QSize maxSize) const;
    Position preferedPosition() const { return Bottom; }
    bool eventFilter( QObject *, QEvent * );

protected:
    void resizeEvent(QResizeEvent*);

protected slots:
    void slotSetSize(int);
    void slotSetupSizeMnu();

private:
    ContainerArea    *_containerArea;
    ContainerAreaBox *_containerAreaBox;
    QPopupMenu       *_opMnu;
    QPopupMenu       *_sizeMnu;
    int               _size;
};

#endif
