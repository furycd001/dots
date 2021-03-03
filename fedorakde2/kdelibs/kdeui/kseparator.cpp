/*
 *   Copyright (C) 1997  Michael Roth <mroth@wirlweb.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */


#include <kdebug.h>
#include <kapp.h>

#include "kseparator.moc"


KSeparator::KSeparator(QWidget* parent, const char* name, WFlags f)
   : QFrame(parent, name, f)
{
   setLineWidth(1);
   setMidLineWidth(0);
   setOrientation( HLine );
}



KSeparator::KSeparator(int orientation, QWidget* parent, const char* name, WFlags f)
   : QFrame(parent, name, f)
{
   setLineWidth(1);
   setMidLineWidth(0);
   setOrientation( orientation );
}



void KSeparator::setOrientation(int orientation)
{
   switch(orientation)
   {
      case Vertical:
      case VLine:
         setFrameStyle( QFrame::VLine | QFrame::Sunken );
         setMinimumSize(2, 0);
         break;
      
      default:
         kdWarning() << "KSeparator::setOrientation(): invalid orientation, using default orientation HLine" << endl;
         
      case Horizontal:
      case HLine:
         setFrameStyle( QFrame::HLine | QFrame::Sunken );
         setMinimumSize(0, 2);
         break;
   }
}



int KSeparator::orientation() const
{
   if ( frameStyle() & VLine )
      return VLine;
   
   if ( frameStyle() & HLine )
      return HLine;
   
   return 0;
}

void KSeparator::drawFrame(QPainter *p)
{
   QPoint	p1, p2;
   QRect	r     = frameRect();
   const QColorGroup & g = colorGroup();

   if ( frameStyle() & HLine ) {
      p1 = QPoint( r.x(), r.height()/2 );
      p2 = QPoint( r.x()+r.width(), p1.y() );
   }
   else {
      p1 = QPoint( r.x()+r.width()/2, 0 );
      p2 = QPoint( p1.x(), r.height() );
   }

   kapp->style().drawSeparator( p, p1.x(), p1.y(), p2.x(), p2.y(), g, true, 1, midLineWidth() );
}


QSize KSeparator::sizeHint() const
{
   if ( frameStyle() & VLine )
      return QSize(2, 0);
   
   if ( frameStyle() & HLine )
      return QSize(0, 2);
   
   return QSize(-1, -1);
}





