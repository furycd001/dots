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

#ifndef __container_base_h__
#define __container_base_h__

#include <qwidget.h>

#include "global.h"

class KConfig;
class	QPopupMenu;

class BaseContainer : public QWidget
{
    Q_OBJECT

public:
    BaseContainer(QWidget *parent=0);
    ~BaseContainer();

    virtual int widthForHeight(int height) = 0;
    virtual int heightForWidth(int width) = 0;

    virtual void about() {}
    virtual void help() {}
    virtual void preferences() {}
    virtual void reportBug() {}

    float freeSpace() const { return _fspace; }
    void setFreeSpace(float f) { _fspace = f; }

    QString appletId() const { return _aid; }
    void setAppletId(const QString& s) { _aid = s; }

    virtual int actions() const { return _actions; }

    Direction popupDirection() const { return _dir; }
    Orientation orientation() const { return _orient; }

    virtual void saveConfiguration(KConfig* config, const QString& group) = 0;
    virtual void configure() {}

    QPoint getPopupPosition(QPopupMenu *menu, QPoint eventpos);
    QPoint moveOffset() const { return _moveOffset; }

    virtual QString appletType() = 0;

public slots:
    virtual void slotSetPopupDirection(Direction d) { _dir = d; }
    virtual void slotSetOrientation(Orientation o) { _orient = o; }

    void removeRequest();

signals:
    void removeme(BaseContainer*);
    void moveme(BaseContainer*);
    void requestSave();

protected:
    Direction          _dir;
    Orientation        _orient;
    float              _fspace;
    QPoint             _moveOffset;
    QString            _aid;
    QPopupMenu 				 *_opMnu;
//    PanelAppletOpMenu *_opMnu;
    int                _actions;
};

#endif

