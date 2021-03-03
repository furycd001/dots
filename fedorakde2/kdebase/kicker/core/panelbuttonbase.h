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

#ifndef __panelbuttonbase_h__
#define __panelbuttonbase_h__

#include <qbutton.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qcursor.h>
#include <qguardedptr.h>

#include <kurl.h>

#include "global.h"

class PanelButtonBase : public QButton
{
    Q_OBJECT

public:
    PanelButtonBase(QWidget *parent=0, const char *name=0,  WFlags f=0 );

    void setTile(const QString&);
    void setIcon(const QString & name, const QString & fallback);
    void setIcon(const KURL & url);
    void setBackground();
    void setTitle(const QString&);

    void setDrawArrow(bool);

    QString title() const { return _title; }
    QString icon() const { return _iconName; }
    Orientation orientation() { return orient; }
    Direction popupDirection() { return dir; }

public slots:
    void slotSetOrientation(Orientation o) { orient = o; }
    void slotSetPopupDirection(Direction d);

protected slots:
    void slotSettingsChanged(int);

protected:
    virtual void resizeEvent(QResizeEvent*);
    virtual void drawButton(QPainter *);
    virtual void drawButtonLabel(QPainter *, const QPixmap &bg);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);

    void setArrowDirection(Position dir);
    void loadTiles();

    Position _dir;
    bool _drawArrow, _highlight, _changeCursorOverItem;
    QString _tile, _title, _iconName;
    QCursor oldCursor;
    QPixmap _up, _down, _bg;
    QPixmap _icon, _iconh, _iconz;
    Direction   dir;
    Orientation orient;
};

class ZoomButton : public PanelButtonBase
{
 Q_OBJECT
 public:
    ZoomButton();
    ~ZoomButton();
    bool isZoomingEnabled() const { return zooming; }
    bool isWatching( PanelButtonBase* btn );
    void watchMe( PanelButtonBase* btn, const QPoint& center, const QPixmap& pm);

public slots:
    void reconfigure();

 protected:
    void drawButtonLabel(QPainter *p, const QPixmap &bg);
    bool eventFilter( QObject *o, QEvent * e);
    // reimp those to do nothing
    void enterEvent( QEvent*) {}
    void leaveEvent( QEvent*) {}

    QEvent* locked;
    QGuardedPtr<PanelButtonBase> watch;
    QPoint mypos;
    bool zooming;
    int hideTimer;
    int raiseTimer;
};


#endif // __panelbuttonbase_h__

