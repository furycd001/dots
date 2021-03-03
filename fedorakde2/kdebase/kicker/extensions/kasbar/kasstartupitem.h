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

#ifndef KASSTARTUPITEM_H
#define KASSTARTUPITEM_H

#include <qpixmap.h>
#include <qstring.h>
#include <qlist.h>
#include "kasitem.h"

class Startup;
class KasPopup;
class QTimer;

/**
 * A KasItem that represents a single Startup.
 */
class KasStartupItem : public KasItem
{
    Q_OBJECT

public:
    KasStartupItem( KasBar *parent, Startup *startup );
    virtual ~KasStartupItem();

    QPixmap icon() const;
    Startup *startup() const { return startup_; }

    /**
     * Reimplemented to paint the item.
     */
    virtual void paint( QPainter *p, int x, int y );

    void paintAnimation( QPainter *p, int x, int y );

protected slots:
    void aniTimerFired();

private:
    Startup *startup_;
    QTimer *aniTimer;
    int frame;
    QList<QPixmap> anim;
    QString label;
    QPixmap pixmap;
};

#endif // KASSTARTUPITEM_H

