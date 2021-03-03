// -*- c++ -*-

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

#ifndef KASTASKITEM_H
#define KASTASKITEM_H

#include <qpixmap.h>
#include "kasitem.h"

class Task;
class KasPopup;
class KasTasker;
class KPixmap;

/**
 * A KasItem that represents a single Task.
 */
class KasTaskItem : public KasItem
{
    Q_OBJECT

 public:
    KasTaskItem( KasTasker *parent, Task *task );
    virtual ~KasTaskItem();

    // For thumbnails
  //    bool hasThumbnail() const { return !thumb.isNull(); }
  //    const QPixmap &thumbnail() const { return thumb; }

    QPixmap icon();

    /**
     * Reimplemented to paint the item.
     */
    virtual void paint( QPainter *p, int x, int y );

    /**
     * Called when a mouse press event is received over this item.
     */
    virtual void mousePressEvent( QMouseEvent *e );

    Task *task() const { return task_; }

    KasTasker *kasbar() const;

    void showWindowMenuAt( WId id, int x, int y );

public slots:
    /**
     * Create a thumbnail for this task (does nothing if they're disabled).
     */
    void refreshThumbnail();

    void startAutoThumbnail();
    void stopAutoThumbnail();

    void iconChanged();

protected:
    /**
     * Reimplemented to create a KasTaskPopup.
     */
    virtual KasPopup *createPopup();

    /**
     * Reimplemented to activate the task.
     */
    void dragOverAction();

private:
    Task *task_;
    QTimer *thumbTimer;
    bool usedIconLoader;
    bool iconHasChanged;
};

#endif // KASTASKITEM_H

