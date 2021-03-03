/* This file is part of the KDE libraries
    Copyright (C) 1998 J�rg Habenicht (j.habenicht@europemail.com)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

/*************************************************************************
 * $Id$
 *
 * $Log$
 * Revision 1.17  2000/09/12 19:15:53  pfeiffer
 * Stefan Hellwig in cooperation with Joerg Habenicht:
 * Draw nicer LEDs, especially sunken ones.
 *
 * Revision 1.16  2000/08/24 12:44:48  porten
 * "friend class" patches from Thomas Kunert <kunert@physik.tu-dresden.de>
 *
 * Revision 1.15  2000/08/17 16:44:44  reggie
 * Don't crash
 *
 * Revision 1.14  2000/06/03 01:04:42  gehrmab
 * * Made drawing routines available for overriding
 * * Added a parent/name constructor
 * * Propertyfication
 *
 * Revision 1.13  2000/05/08 19:38:49  sschiman
 * Calling setColor before setting up the private data is a bad idea ;-)
 *
 * Revision 1.12  2000/05/07 09:49:57  habenich
 * provided method to set the factor to dark the LED
 * precalculated the dark color, is just retrieved at paint events
 *
 * Revision 1.11  2000/04/09 16:08:33  habenich
 * fixed nasty bug #70 disappearing led
 * reenabled flat and raised led painting
 *
 * Revision 1.10  1999/12/25 17:12:18  mirko
 * Modified Look "round" to "raised", as the others are flat and
 * sunken. All enums start with uppercase letters now to fit the overall
 * KDE style.
 * Implemented raised rectangluar look.
 * --Mirko.
 *
 * Revision 1.9  1999/11/12 21:17:09  antlarr
 * Fixed some bugs.
 * Added the possibility to draw a sunk rectangle as the "old" KLedLamp did.
 *
 * Revision 1.9  1999/11/11 16:08:15  antlarr
 * Fixed some bugs.
 * Added the possibility to draw a sunk rectangle as the "old" KLedLamp did.
 *
 * Revision 1.8  1999/11/01 22:03:15  dmuell
 * fixing all kinds of compile warnings
 * (unused var, unused argument etc)
 *
 * Revision 1.7  1999/10/10 13:34:14  mirko
 * First merge with KLedLamp that shows a rectangular LED.
 * It does not yet work reliably.
 *
 * Revision 1.6  1999/03/01 23:34:49  kulow
 * CVS_SILENT ported to Qt 2.0
 *
 * Revision 1.5  1999/02/19 08:52:42  habenich
 * ID und LOG tags included
 *
 *
 *************************************************************************/

#define PAINT_BENCH
#undef PAINT_BENCH

#ifdef PAINT_BENCH
#include <qdatetime.h>
#include <stdio.h>
#endif


#include <qpainter.h>
#include <qcolor.h>
#include <kapp.h>
#include <kpixmapeffect.h>
#include "kled.h"



class KLed::KLedPrivate
{
  friend class KLed;

  int dark_factor;
  QColor offcolor;
};



KLed::KLed(QWidget *parent, const char *name)
  : QWidget( parent, name),
    led_state(On),
    led_look(Raised),
    led_shape(Circular)
{
  QColor col(green);
  d = new KLed::KLedPrivate;
  d->dark_factor = 300;
  d->offcolor = col.dark(300);

  setColor(col);
}


KLed::KLed(const QColor& col, QWidget *parent, const char *name)
  : QWidget( parent, name),
    led_state(On),
    led_look(Raised),
    led_shape(Circular)
{
  d = new KLed::KLedPrivate;
  d->dark_factor = 300;
  d->offcolor = col.dark(300);

  setColor(col);
  //setShape(Circular);
}

KLed::KLed(const QColor& col, KLed::State state,
	   KLed::Look look, KLed::Shape shape, QWidget *parent, const char *name )
  : QWidget(parent, name),
    led_state(state),
    led_look(look),
    led_shape(shape)
{
  d = new KLed::KLedPrivate;
  d->dark_factor = 300;
  d->offcolor = col.dark(300);

  //setShape(shape);
  setColor(col);
}


KLed::~KLed()
{
  delete d;
}

void
KLed::paintEvent(QPaintEvent *)
{
#ifdef PAINT_BENCH
  const int rounds = 1000;
  QTime t;
  t.start();
  for (int i=0; i<rounds; i++) {
#endif
  switch(led_shape)
    {
    case Rectangular:
      switch (led_look)
	{
	case Sunken :
	  paintRectFrame(false);
	  break;
	case Raised :
	  paintRectFrame(true);
	  break;
	case Flat   :
	  paintRect();
	  break;
	default  :
	  qWarning("%s: in class KLed: no KLed::Look set",kapp->argv()[0]);
	}
      break;
    case Circular:
      switch (led_look)
	{
	case Flat   :
	  paintFlat();
	  break;
	case Raised :
	  paintRound();
	  break;
	case Sunken :
	  paintSunken();
	  break;
	default:
	  qWarning("%s: in class KLed: no KLed::Look set",kapp->argv()[0]);
	}
      break;
    default:
      qWarning("%s: in class KLed: no KLed::Shape set",kapp->argv()[0]);
      break;
    }
#ifdef PAINT_BENCH
  }
  int ready = t.elapsed();
  qWarning("elapsed: %d msec. for %d rounds", ready, rounds);
#endif
}

void
KLed::paintFlat() // paint a ROUND FLAT led lamp
{
	QPainter paint;
	QColor color;
	QBrush brush;
	QPen pen;

	// Initialize coordinates, width, and height of the LED
	//
	int width = this->width();
	// Make sure the LED is round!
	if (width > this->height())
		width = this->height();
	width -= 2; // leave one pixel border
	if (width < 0)
	  width = 0;


	// start painting widget
	//
	paint.begin( this );

	// Set the color of the LED according to given parameters
   	color = ( led_state ) ? led_color : d->offcolor;

	// Set the brush to SolidPattern, this fills the entire area
	// of the ellipse which is drawn with a thin grey "border" (pen)
	brush.setStyle( QBrush::SolidPattern );
	brush.setColor( color );

	pen.setWidth( 1 );
	color.setRgb( 170, 170, 170 );		// This is a grey color value
	pen.setColor( color );			// Set the pen accordingly

	paint.setPen( pen );			// Select pen for drawing
	paint.setBrush( brush );		// Assign the brush to the painter

	// Draws a "flat" LED with the given color:
	paint.drawEllipse( 1, 1, width, width );

	paint.end();
	//
	// painting done
}

void
KLed::paintRound() // paint a ROUND RAISED led lamp
{
    QPainter paint;
    QColor color;
    QBrush brush;
    QPen pen;

    // Initialize coordinates, width, and height of the LED
    int width = this->width();

    // Make sure the LED is round!
    if (width > this->height())
      width = this->height();
    width -= 2; // leave one pixel border
    if (width < 0) 
      width = 0;

    // start painting widget
    //
    paint.begin( this );

    // Set the color of the LED according to given parameters
    color = ( led_state ) ? led_color : d->offcolor;

    // Set the brush to SolidPattern, this fills the entire area
    // of the ellipse which is drawn first
    brush.setStyle( QBrush::SolidPattern );
    brush.setColor( color );
    paint.setBrush( brush );		// Assign the brush to the painter

    // Draws a "flat" LED with the given color:
    paint.drawEllipse( 1, 1, width, width );

    // Draw the bright light spot of the LED now, using modified "old"
    // painter routine taken from KDEUI�s KLed widget:

    // Setting the new width of the pen is essential to avoid "pixelized"
    // shadow like it can be observed with the old LED code
    pen.setWidth( 2 );

    // shrink the light on the LED to a size about 2/3 of the complete LED
    int pos = width/5 + 1;
    int light_width = width;
    light_width *= 2;
    light_width /= 3;

    // Calculate the LED�s "light factor":
    int light_quote = (130*2/(light_width?light_width:1))+100;

    // Now draw the bright spot on the LED:
    while (light_width) {
    	color = color.light( light_quote );			// make color lighter
	pen.setColor( color );				// set color as pen color
	paint.setPen( pen );				// select the pen for drawing
	paint.drawEllipse( pos, pos, light_width, light_width );	// draw the ellipse (circle)
	light_width--;
   	if (!light_width)
     		 break;
	paint.drawEllipse( pos, pos, light_width, light_width );
	light_width--;
	if (!light_width)
  		break;
	paint.drawEllipse( pos, pos, light_width, light_width );
	pos++; light_width--;
    }

    // Drawing of bright spot finished, now draw a thin grey border
    // around the LED; it looks nicer that way. We do this here to
    // avoid that the border can be erased by the bright spot of the LED

    pen.setWidth( 1 );
    color.setRgb( 170, 170, 170 );		// This is a grey color value
    pen.setColor( color );			// Set the pen accordingly
    paint.setPen( pen );			// Select pen for drawing
    brush.setStyle( QBrush::NoBrush );		// Switch off the brush
    paint.setBrush( brush );			// This avoids filling of the ellipse

    paint.drawEllipse( 1, 1, width, width );

    paint.end();
    //
    // painting done
}

void
KLed::paintSunken() // paint a ROUND SUNKEN led lamp
{
    QPainter paint;
    QColor color;
    QBrush brush;
    QPen pen;
				
    // First of all we want to know what area should be updated
    // Initialize coordinates, width, and height of the LED
    int	width = this->width();

    // Make sure the LED is round!
    if (width > this->height())
      width = this->height();
    width -= 2; // leave one pixel border
    if (width < 0) 
      width = 0;

    // maybe we could stop HERE, if width <=0 ?

    // start painting widget
    //
    paint.begin( this );

    // Set the color of the LED according to given parameters
    color = ( led_state ) ? led_color : d->offcolor;

    // Set the brush to SolidPattern, this fills the entire area
    // of the ellipse which is drawn first
    brush.setStyle( QBrush::SolidPattern );
    brush.setColor( color );
    paint.setBrush( brush );                // Assign the brush to the painter

    // Draws a "flat" LED with the given color:
    paint.drawEllipse( 1, 1, width, width );

    // Draw the bright light spot of the LED now, using modified "old"
    // painter routine taken from KDEUI�s KLed widget:

    // Setting the new width of the pen is essential to avoid "pixelized"
    // shadow like it can be observed with the old LED code
    pen.setWidth( 2 );

    // shrink the light on the LED to a size about 2/3 of the complete LED
    int pos = width/5 + 1;
    int light_width = width;
    light_width *= 2;
    light_width /= 3;
	
    // Calculate the LED�s "light factor":
    int light_quote = (130*2/(light_width?light_width:1))+100;

    // Now draw the bright spot on the LED:
    while (light_width) {
	color = color.light( light_quote );                      // make color lighter
	pen.setColor( color );                                   // set color as pen color
	paint.setPen( pen );                                     // select the pen for drawing
	paint.drawEllipse( pos, pos, light_width, light_width ); // draw the ellipse (circle)
	light_width--;
	if (!light_width)
		break;
	paint.drawEllipse( pos, pos, light_width, light_width );
	light_width--;
	if (!light_width)
		break;
	paint.drawEllipse( pos, pos, light_width, light_width );
	pos++; light_width--;
    }

    // Drawing of bright spot finished, now draw a thin border
    // around the LED which resembles a shadow with light coming
    // from the upper left.

    pen.setWidth( 3 );
    brush.setStyle( QBrush::NoBrush );              // Switch off the brush
    paint.setBrush( brush );                        // This avoids filling of the ellipse

    // Set the initial color value to 200 (bright) and start
    // drawing the shadow border at 45� (45*16 = 720).

    int shadow_color = 200, angle;
    
    for ( angle = 720; angle < 6480; angle += 240 ) {
      color.setRgb( shadow_color, shadow_color, shadow_color );
      pen.setColor( color );
      paint.setPen( pen );
      paint.drawArc( 1, 1, width, width, angle, 240 );
      if ( angle < 2320 ) {
	shadow_color -= 25;			// set color to a darker value
	if ( shadow_color < 100 ) shadow_color = 100;
      }
      else if ( ( angle > 2320 ) && ( angle < 5760 ) ) {
	shadow_color += 25;			// set color to a brighter value
	if ( shadow_color > 255 ) shadow_color = 255;
      }
      else {
	shadow_color -= 25;			// set color to a darker value again
	if ( shadow_color < 100 ) shadow_color = 100;
      }	// end if ( angle < 2320 )
    }	// end for ( angle = 720; angle < 6480; angle += 160 )

    paint.end();
    //
    // painting done
}

void
KLed::paintRect()
{
  QPainter painter(this);
  QBrush lightBrush(led_color);
  QBrush darkBrush(d->offcolor);
  QPen pen(led_color.dark(300));
  int w=width();
  int h=height();
  // -----
  switch(led_state)
  {
  case On:
    painter.setBrush(lightBrush);
    painter.drawRect(0, 0, w, h);
    break;
  case Off:
    painter.setBrush(darkBrush);
    painter.drawRect(0, 0, w, h);
    painter.setPen(pen);
    painter.drawLine(0, 0, w, 0);
    painter.drawLine(0, h-1, w, h-1);
    // Draw verticals
    int i;
    for(i=0; i < w; i+= 4 /* dx */)
      painter.drawLine(i, 1, i, h-1);
    break;
  default: break;
  }
}

void
KLed::paintRectFrame(bool raised)
{
  QPainter painter(this);
  QBrush lightBrush(led_color);
  QBrush darkBrush(d->offcolor);
  int w=width();
  int h=height();
  QColor black=Qt::black;
  QColor white=Qt::white;
  // -----
  if(raised)
    {
      painter.setPen(white);
      painter.drawLine(0, 0, 0, h-1);
      painter.drawLine(1, 0, w-1, 0);
      painter.setPen(black);
      painter.drawLine(1, h-1, w-1, h-1);
      painter.drawLine(w-1, 1, w-1, h-1);
      painter.fillRect(1, 1, w-2, h-2,
       		       (led_state==On)? lightBrush : darkBrush);
    } else {
      painter.setPen(black);
      painter.drawRect(0,0,w,h);
      painter.drawRect(0,0,w-1,h-1);
      painter.setPen(white);
      painter.drawRect(1,1,w-1,h-1);
      painter.fillRect(2, 2, w-4, h-4,
		       (led_state==On)? lightBrush : darkBrush);
    }
}

KLed::State
KLed::state() const
{
  return led_state;
}

KLed::Shape
KLed::shape() const
{
  return led_shape;
}

const QColor
KLed::color() const
{
  return led_color;
}

KLed::Look
KLed::look() const
{
  return led_look;
}

void
KLed::setState( State state )
{
  if (led_state != state)
    {
      led_state = state;
      update();
    }
}

void
KLed::toggleState()
{
  led_state = (led_state == On) ? Off : On;
  // setColor(led_color);
  update();
}

void
KLed::setShape(KLed::Shape s)
{
  if(led_shape!=s)
    {
      led_shape = s;
      update();
    }
}

void
KLed::setColor(const QColor& col)
{
  if(led_color!=col) {
    led_color = col;
    d->offcolor = col.dark(d->dark_factor);
    update();
  }
}

void
KLed::setDarkFactor(int darkfactor)
{
  if (d->dark_factor != darkfactor) {
    d->dark_factor = darkfactor;
    d->offcolor = led_color.dark(darkfactor);
    update();
  }
}

int
KLed::darkFactor() const
{
  return d->dark_factor;
}

void
KLed::setLook( Look look )
{
  if(led_look!=look)
    {
      led_look = look;
      update();
    }
}

void
KLed::toggle()
{
  toggleState();
}

void
KLed::on()
{
  setState(On);
}

void
KLed::off()
{
  setState(Off);
}

#include "kled.moc"
