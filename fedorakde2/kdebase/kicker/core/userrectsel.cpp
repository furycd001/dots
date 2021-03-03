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

#include <qapplication.h>
#include <qpainter.h>

#include "userrectsel.h"
#include "userrectsel.moc"

#include <X11/Xlib.h>

UserRectSel::UserRectSel( const QValueList<QRect>& rects, int start  )
  : QWidget( 0, 0, WStyle_Customize | WStyle_NoBorder ),
    rectangles( rects ),
    current ( start )
{
  setGeometry( -10, -10, 2, 2);
}

UserRectSel::~UserRectSel()
{
}

int UserRectSel::select()
{
  show();
  setMouseTracking( TRUE );
  XGrabServer( qt_xdisplay() );
  grabMouse();
  paintCurrent();
  qApp->enter_loop();
  paintCurrent();
  releaseMouse();
  XUngrabServer( qt_xdisplay() );
  qApp->syncX();
  return current;
}

void UserRectSel::mouseReleaseEvent( QMouseEvent * e)
{
  if ( e->button() == LeftButton ) {
	qApp->exit_loop();
  }
}

void UserRectSel::mouseMoveEvent( QMouseEvent * e)
{
  int nearest = current;
  int diff = -1;
  for (int i = 0; i < int(rectangles.count()); i++) {
	QRect r( rectangles[i] );
	int ndiff = (r.center().x() - e->globalPos().x() ) *  (r.center().x() - e->globalPos().x() )
      +  (r.center().y() - e->globalPos().y() ) *  (r.center().y() - e->globalPos().y() );
	if ( r.contains( e->globalPos() ) )
      ndiff = 0;
	if ( diff < 0 || ndiff < diff ) {
      diff = ndiff;
      nearest = i;
	}
  }
  if ( nearest != current ) {
	paintCurrent();
	current = nearest;
	paintCurrent();
  }
}

void UserRectSel::paintCurrent()
{
  QRect r( rectangles[current] );
  QWidget w(0, "", WType_Desktop | WPaintUnclipped );
  QPainter p ( &w );
  p.setPen( QPen( white, 5 ) );
  p.setRasterOp( XorROP );
  p.drawRect( r );
}


int UserRectSel::select( const QValueList<QRect>& rects, int start )
{
  UserRectSel sel( rects, start );
  return sel.select();
}
