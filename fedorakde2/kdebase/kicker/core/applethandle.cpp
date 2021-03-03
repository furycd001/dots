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

#include <qpainter.h>

#include <kapp.h>
#include <kstyle.h>

#include "applethandle.h"

void AppletHandle::setFadeOutHandle(bool v)
{
  _fadeout_handle = v;
  repaint();
}

void AppletHandle::enterEvent(QEvent *)
{
  _drawIt = true;
  repaint();
}

void AppletHandle::leaveEvent(QEvent *)
{
  _drawIt = false;
  repaint();
}

void AppletHandle::paintEvent(QPaintEvent *)
{
  if (!_drawIt && _fadeout_handle)
    return;

  QPainter p;
  p.begin(this);

  if(kapp->kstyle()){
    kapp->kstyle()->drawKickerAppletHandle(&p, 0, 0, width(), height(),
                                           colorGroup(), NULL);
    p.end();
    return;
  }

  QColorGroup g = colorGroup();

  if (_orient == Horizontal)
    {
      for(int y = 2; y < height() - 2; y++)
        {
          p.setPen(g.light());
          p.drawPoint(0, y++);
          p.setPen(g.dark());
          p.drawPoint(1, y++);
          y++;
          p.setPen(g.light());
          p.drawPoint(3, y++);
          p.setPen(g.dark());
          p.drawPoint(4, y);
        }
    }
  else
    {
      for(int x = 2; x < width() - 2; x++)
        {
          p.setPen(g.light());
          p.drawPoint(x++, 0);
          p.setPen(g.dark());
          p.drawPoint(x++, 1);
          x++;
          p.setPen(g.light());
          p.drawPoint(x++, 3);
          p.setPen(g.dark());
          p.drawPoint(x, 4);
        }
    }
  p.end();

}

#include "applethandle.moc"


