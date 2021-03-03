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
#include <qdrawutil.h>

#include <kapp.h>
#include <kstyle.h>

#include "extensionhandle.h"

void ExtensionHandle::setFadeOutHandle(bool v)
{
  _fadeout_handle = v;
  repaint();
}

void ExtensionHandle::enterEvent(QEvent *e)
{
  _drawIt = true;
  repaint();
  QWidget::enterEvent( e );
}

void ExtensionHandle::leaveEvent(QEvent *e)
{
  _drawIt = false;
  repaint();
  QWidget::leaveEvent( e );
}

void ExtensionHandle::paintEvent(QPaintEvent *)
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
      qDrawShadeLine( &p, 0, 2, 0, height() - 2, g, false, 2, 0 );
      qDrawShadeLine( &p, 2, 2, 2, height() - 2, g, false, 2, 0 );
    }
  else
    {
      qDrawShadeLine( &p, 2, 0, width() - 2, 0, g, false, 2, 0 );
      qDrawShadeLine( &p, 2, 2, width() - 2, 2, g, false, 2, 0 );
    }
  p.end();

}
