// -*- c++ -*-

#ifndef KASPOPUP_H
#define KASPOPUP_H

#include <qwidget.h>
#include <kpixmap.h>

class KasItem;
class KasBar;

/**
 * Self positioning popup for KasItems.
 *
 * @author Richard Moore, rich@kde.org
 * @version $Id: kaspopup.h,v 1.2 2001/05/21 20:59:41 rich Exp $
 */
class KasPopup : public QWidget
{
   Q_OBJECT

public:
   KasPopup( KasItem *item, const char *name=0 );
   virtual ~KasPopup();
   
   /**
    * Move the popup to the right position. You should not need
    * to call this directly.
    */
   void positionSelf();
  
   KasItem *item() const { return item_; }
   KasBar *kasbar() const { return kasbar_; }
   
   /**
    * Reimplemented for internal reasons.
    */
   void show();

   static QPoint calcPosition( KasItem *item, int w, int h );

private:
   KasItem *item_;
   KasBar *kasbar_;
};

#endif // KASPOPUP_H


