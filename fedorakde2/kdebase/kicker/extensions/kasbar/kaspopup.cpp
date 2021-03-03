#include <qapplication.h>

#include "kasitem.h"
#include "kasbar.h"
#include "kaspopup.h"

KasPopup::KasPopup( KasItem *item, const char *name )
   : QWidget( 0, name, WStyle_Customize | WStyle_Tool | WStyle_StaysOnTop ),
     item_( item ),
     kasbar_( item->kasbar() )
{
}

KasPopup::~KasPopup()
{
}

void KasPopup::positionSelf()
{
   move( calcPosition( item_, width(), height() ) );
}

void KasPopup::show()
{
   positionSelf();
   QWidget::show();
}

QPoint KasPopup::calcPosition( KasItem *item, int w, int h )
{
   KasBar *kasbar = item->kasbar();
   QPoint pos = kasbar->itemPos( item );

   if ( ( pos.x() < 0 ) && ( pos.y() < 0 ) )
      return QPoint();

   pos = kasbar->mapToGlobal( pos );
   int x = pos.x();
   int y = pos.y();

   if ( kasbar->orientation() == Horizontal ) {
      if ( y < ( qApp->desktop()->height() / 2 ) )
	 y = y + kasbar->itemExtent();
      else
	 y = y - h;

      if ( (x + w) > qApp->desktop()->width() )
         x = x - w + kasbar->itemExtent();
   }
   else {
      if ( x < ( qApp->desktop()->width() / 2 ) )
	 x = x + kasbar->itemExtent();
      else
	 x = x - w;

      if ( (y + h) > qApp->desktop()->height() )
         y = y - h + kasbar->itemExtent();
   }

   return QPoint( x, y );
}

#include "kaspopup.moc"


