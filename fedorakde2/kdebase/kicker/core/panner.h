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

#ifndef __panner_h__
#define __panner_h__

#include <qscrollview.h>
#include <qtimer.h>

class Panner : public QScrollView
{
    Q_OBJECT

public:
    Panner( Orientation orientation, QWidget* parent, const char* name = 0);
    ~Panner();

    bool eventFilter( QObject *, QEvent * );

    QSize minimumSizeHint() const { return QWidget::minimumSizeHint(); }

    Qt::Orientation orientation() const {return orient; }
    virtual void setOrientation(Orientation orientation);

    void updateArrows();

public slots:
    void scrollRightDown();
    void scrollLeftUp();

signals:
    void needScrollButtons(bool);

protected:
    void resizeEvent(QResizeEvent *ev);
    void contentsWheelEvent(QWheelEvent *){;}
    void viewportWheelEvent(QWheelEvent *){;}

    virtual void layoutChildren() = 0;

private:
    Orientation orient;
};

#endif
