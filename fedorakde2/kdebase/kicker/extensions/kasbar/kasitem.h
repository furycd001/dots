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

#ifndef KASITEM_H
#define KASITEM_H

class QPainter;
class QMouseEvent;
class KasPopup;

#include <qobject.h>
#include "kasbar.h"

/**
 * Abstract base class for items that can be in a KasBar.
 */
class KasItem : public QObject
{
   Q_OBJECT

public:
   KasItem( KasBar *parent );
   virtual ~KasItem();

    /**
     * Reimplemented by subclasses to paint the item.
     */
   virtual void paint( QPainter *p, int x, int y );

   /**
     * Called when a mouse event is received over this item.
     */
   virtual void mousePressEvent( QMouseEvent * ) {}

   virtual void mouseEnter();
   virtual void mouseLeave();

   virtual void dragEnter();
   virtual void dragLeave();

   /**
     * Get the parent KasBar of this item.
     */
   KasBar *kasbar() const { return kas; }

   /**
     * Gets the font metrics from the parent.
     */
   QFontMetrics fontMetrics() const { return kas->fontMetrics(); }

   /**
     * Gets the color group from the parent.
     */
   const QColorGroup &colorGroup() const { return kas->colorGroup(); }

   bool isShowingPopup() const { return (pop ? true : false ); }
   KasPopup *popup() const { return pop; }

   /**
    * If this flag is set, the default popup behaviour is disabled. This
    * means you must call show/hide/toggle yourself if you want the popup
    * to be shown.
    */
   void setCustomPopup( bool enable ) { customPopup = enable; }
   bool hasCustomPopup() const { return customPopup; }

public slots:
   /**
    * Asks the KasBar to update this item. If erase is true then the
    * item will be erased before the paint method is called.
    */
   void update( bool erase );

   /**
    * Update with 
    */
   void update();

   void showPopup();
   void hidePopup();
   void togglePopup();

   virtual void dragOverAction() {}

protected:
   virtual KasPopup *createPopup() { return 0; }

   /**
     * Draw a standard frame for the item.
     */
   void paintFrame( QPainter *p, int x, int y );

   /**
     * Draw a standard label for the item.
     */
   void paintLabel( QPainter *p, int x, int y, const QString &text  );

   /**
     * Paint the background for the active state.
     */
   void paintActiveBg( QPainter *p, int x, int y );

   /**
     * Paint the background for the inactive state.
     */
   void paintInactiveBg( QPainter *p, int x, int y );

private:
   KasBar *kas;
   KasPopup *pop;
   QTimer *popupTimer;
   QTimer *dragTimer;
   bool mouseOver;
   bool customPopup;
};

#endif // KASITEM_H

