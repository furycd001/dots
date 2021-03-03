/*****************************************************************

Copyright (c) 2001 Matthias Elter <elter@kde.org>

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

#include <qpainter.h>

#include "scrollbutton.h"
#include "scrollbutton.moc"

ScrollButton::ScrollButton( QWidget *parent, const char *name )
  : QPushButton( parent, name ), arrowType(UpArrow)
{
}

ScrollButton::~ScrollButton()
{

}

void ScrollButton::setArrowType( Qt::ArrowType at )
{
    arrowType = at;
    repaint();
}

void ScrollButton::drawButton(QPainter *p)
{
    style().drawPanel(p, 0, 0, width(), height(), colorGroup(),
                      isDown(), 2, &colorGroup().brush(QColorGroup::Background));

    if( width() < 10 || height() < 10 )
        return; // don't draw arrows if we are to small

    int d = 0;
    int arrow = 10;
    if (isDown()) d = 1;

    style().drawArrow(p, arrowType, isDown(), (width()-arrow)/2 +d, (height()-arrow)/2 +d,
		      arrow, arrow, colorGroup(), true);
}
