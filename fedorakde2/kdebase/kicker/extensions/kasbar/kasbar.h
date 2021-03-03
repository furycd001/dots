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

#ifndef __KASBAR_H
#define __KASBAR_H

#include <qwidget.h>
#include <qlist.h>
#include <qpoint.h>

class KasItem;
class KRootPixmap;
class KPixmap;

/**
 * The main view for KasBar.
 *
 * @version $Id: kasbar.h,v 1.13 2001/05/21 20:59:41 rich Exp $
 */
class KasBar : public QWidget
{
   Q_OBJECT

   friend class KasItem;
public:
   KasBar( Orientation o, QWidget* parent = 0, const char* name = 0 );
   virtual ~KasBar();

   //
   // Item management
   //
   void append( KasItem *i );
   void insert( int index, KasItem *i );
   void remove( KasItem *i );
   void clear();
   KasItem *itemAt( uint i ) { return items.at( i ); }
   int indexOf( KasItem *i ) { return items.find( i ); }

   //
   // Layout options.
   //
   enum ItemSize {
      Large,
      Medium,
      Small
    };

   int itemSize() const { return itemSize_; }
   int itemExtent() const { return itemExtent_; }

   /**
    * The number of items in the bar.
    */
   unsigned int itemCount() const { return items.count(); }

   int maxBoxes() const { return maxBoxes_; }

   void setOrientation( Orientation o ) { orient = o; updateLayout(); }
   Orientation orientation() { return orient; }

   QSize sizeHint( Orientation,  QSize max );

   //
   // Look and feel options
   //

   /**
    * Is transparency enabled?
    */
   bool isTransparent() const { return transparent_; }

   /**
    * Is tinting enabled?
    */
   bool hasTint() const { return enableTint_; }

   /**
    * Set the amount and color of the tint.
    */
   void setTint( double amount, QColor color );

   /**
    * Set the amount of tinting.
    */
   void setTintAmount( double amount ) { setTint( amount, tintColour_ ); }

   /**
    * Get the amount of tinting.
    */
   double tintAmount() const { return tintAmount_; }

   /**
    * Get the color of the tint.
    */
   QColor tintColor() const { return tintColour_; }

   QColor inactivePenColor() const { return inactivePenColor_; }
   QColor activePenColor() const { return activePenColor_; }

   //
   // Utilities
   //

   /**
    * Redraws the specified item.
    */
   void repaintItem(KasItem *i, bool erase = true );

   /**
    * Returns the item at p or 0.
    */
   KasItem* itemAt(const QPoint &p);

   /**
    * Get the position of the specified item.
    */
   QPoint itemPos( KasItem *i );

   /**
    * The item under the mouse pointer (or 0).
    */
   KasItem *itemUnderMouse() const { return itemUnderMouse_; }

public slots:
   //
   // Layout slots
   //
   void setMaxBoxes( int count );
   void setItemSize( int size );

   virtual void updateLayout();

   /**
    * Enable or disable tinting.
    */
   void setTint( bool enable );

   /**
    * Enable or disable transparency.
    */
   void setTransparent( bool enable );

   /**
    * Set the color of the tint.
    */
   void setTintColor( const QColor &c );

   /**
    * Set the strength of the tint (as a percentage).
    */
   void setTintAmount( int percent );

signals:
   /**
    * Emitted when kasbar wants to resize. This happens when a new window is added.
    */
   void layoutChanged();

   /**
    * Emitted when the item size is changed.
    */
   void itemSizeChanged( int );

protected:
   /**
    * Displays the popup menus, hides/shows windows.
    */
   void mousePressEvent(QMouseEvent *ev);

   /**
    * Overridden to implement the mouse-over highlight effect.
    */
   void mouseMoveEvent(QMouseEvent *ev);

   /**
    * Overridden to implement the drag-over task switching.
    */
   void dragMoveEvent(QDragMoveEvent *ev);

   /**
    * Calls the paint methods for the items in the rectangle specified by the event.
    */
   void paintEvent(QPaintEvent *ev);

   /**
    * Forces the widget to re-layout it's contents.
    */
   void resizeEvent(QResizeEvent *ev);

private:
   /**
    * Internal factory method used by items to get the active bg fill.
    */
   KPixmap *activeBg();

   /**
    * Internal factory method used by items to get the inactive bg fill.
    */
   KPixmap *inactiveBg();

private:
   // Core data
   QList<KasItem> items;
   Orientation orient;
   KasItem *itemUnderMouse_;
   int maxBoxes_;
   int itemSize_;
   int itemExtent_;

   // Implements pseudo-transparency
   bool transparent_;
   KRootPixmap *rootPix;
   bool enableTint_;
   double tintAmount_;
   QColor tintColour_;

   // Look and feel resources
   KPixmap *actBg;
   KPixmap *inactBg;
   QColor activePenColor_;
   QColor inactivePenColor_;
};



#endif
