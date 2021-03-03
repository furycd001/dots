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

#ifndef __containerarea_h__
#define __containerarea_h__

#include <qlist.h>
#include <qtimer.h>

#include "global.h"
#include "panner.h"
#include "container_base.h"
#include "appletinfo.h"

class ExternalAppletContainer;
class KConfig;
class DragIndicator;

class ContainerArea : public Panner
{
    Q_OBJECT

public:
    ContainerArea(Orientation orientation, bool mainArea, KConfig* config, QWidget* parent, const char* name = 0);
    ~ContainerArea();

public:
    int position() const;
    Direction popupDirection() const;

    void init();

    void addKMenuButton();
    void addDesktopButton();
    void addWindowListButton();
    void addBookmarksButton();
    void addRecentDocumentsButton();
    void addURLButton(const QString &url);
    void addBrowserButton(const QString &startDir, const QString& icon = QString("kdisknav"));
    void addServiceMenuButton(const QString &name, const QString& relPath);
    void addExeButton(const QString &filePath, const QString &icon,
		      const QString &cmdLine, bool inTerm);
    void addKonsoleButton();

    void addApplet( const QString& desktopFile );
    void addApplet(const QString &desktopFile, bool internal);

    void configure();

    bool hasInstance(AppletInfo*) const;

    bool inMoveOperation() const { return _movingAC; }
    int minimumUsedSpace( Orientation, int width, int height ) const;

public slots:
    void startContainerMove(BaseContainer *a);
    void stopContainerMove(BaseContainer *a);
    void setOrientation(Orientation o);
    void setPosition(Position p);
    void slotLayoutChildren();
    void slotSaveContainerConfig();

signals:
    void sizeHintChanged();

protected:
    void layoutChildren();

    BaseContainer* coversContainer(BaseContainer *a, bool strict);

    void updateContainerList();
    int relativeContainerPos(BaseContainer*) const;
    int totalFreeSpace() const;

    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

    void dragEnterEvent(QDragEnterEvent*);
    void dragMoveEvent(QDragMoveEvent*);
    void dragLeaveEvent(QDragLeaveEvent*);
    void dropEvent(QDropEvent*);

    virtual void resizeEvent(QResizeEvent *);

    void addContainer(BaseContainer* a);
    void removeContainer(BaseContainer *a);
    void removeAllContainers();

    void setUniqueId(BaseContainer* a);
    void moveToFirstFreePosition(BaseContainer* a);

    bool isStretch(BaseContainer* a) const;
    void disableStretch();
    void restoreStretch();

    void saveContainerConfig(bool layoutOnly = false);
    void loadContainerConfig();
    void defaultContainerConfig();

    void moveContainerSwitch(BaseContainer* a, int distance);
    int moveContainerPush(BaseContainer* a, int distance);

    int moveContainerPushRecursive(QListIterator<BaseContainer> it, int distanceRequest);

    QRect availableSpaceFollowing(BaseContainer*);
    void moveDragIndicator(int pos);

    void setBackgroundTheme();

    void scrollTo(BaseContainer*);

    KConfig* config() { return _config; }

protected slots:
    void embeddedWindowDestroyed();
    void slotRemoveContainer(BaseContainer*);
    void slotAddExternal(ExternalAppletContainer*);
    void autoScroll();

private:
    QList<BaseContainer> _containers;

    bool	    _block_relayout;
    bool	    _movingAC;
    BaseContainer*  _moveAC;
    QPoint	    _moveOffset;
    Position	    _pos;
    bool	    _mainArea;
    KConfig*	    _config;
    DragIndicator*  _dragIndicator;
    BaseContainer*  _dragMoveAC;
    QPoint	    _dragMoveOffset;
    QTimer          _autoScrollTimer;
};

class DragIndicator : public QWidget
{
    Q_OBJECT

public:
    DragIndicator(QWidget* parent = 0, const char* name = 0)
	: QWidget(parent, name) {;}
    ~DragIndicator() {;}

    QSize preferredSize() const { return _preferredSize; }
    void setPreferredSize(const QSize& size) { _preferredSize = size; }

protected:
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent*);

private:
    QSize _preferredSize;
};

#endif

