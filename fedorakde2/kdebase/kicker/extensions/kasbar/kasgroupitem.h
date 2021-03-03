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

#ifndef KASGROUPITEM_H
#define KASGROUPITEM_H

#include <qpixmap.h>
#include "kasitem.h"

class KasPopup;
class KasTasker;
class KPixmap;
class Task;

/**
 * A KasItem that represents a single Group.
 */
class KasGroupItem : public KasItem
{
   Q_OBJECT

public:
   KasGroupItem( KasTasker *parent/*, Group *group*/ );
   virtual ~KasGroupItem();

   void paintLabel( QPainter *p, int x, int y, const QString &text, int arrowSize, bool arrowOnLeft );
   void paintArrowLabel( QPainter *p, int x, int y, const QString &text );

   /**
    * Reimplemented to paint the item.
    */
   virtual void paint( QPainter *p, int x, int y );

   /**
    * Called when a mouse press event is received over this item.
    */
   virtual void mousePressEvent( QMouseEvent *e );
   
   KasTasker *kasbar() const;

   QString title() const { return title_; }

   Task *task( uint i ) { return items.at( i ); }
   int taskCount() const { return items.count(); }

  QPixmap icon();
  
public slots:
   void addTask( Task *t );
   void removeTask( Task *t );

   void setTitle( const QString & );

protected:
   /**
    * Reimplemented to create a KasGroupPopup.
    */
   virtual KasPopup *createPopup();
   
private:
   QString title_;
   QList <Task> items;
};

#endif // KASGROUPITEM_H

