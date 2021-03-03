#include <qpainter.h>
#include <qdrawutil.h>
#include <qtimer.h>

#include <kglobal.h>
#include <kiconloader.h>

#include "kasitem.h"
#include "kaspopup.h"

KasItem::KasItem( KasBar *parent )
   : QObject( parent ),
     kas( parent ),
     pop( 0 ), popupTimer( 0 ),
     mouseOver( false ), customPopup( false )
{
}

KasItem::~KasItem()
{
   if ( pop )
      delete pop;
}

void KasItem::mouseEnter()
{
   static const int POPUP_DELAY = 300;

   if ( (!customPopup) && (popupTimer == 0) ) {
      popupTimer = new QTimer( this, "popupTimer" );
      connect( popupTimer, SIGNAL( timeout() ), SLOT( showPopup() ) );
      popupTimer->start( POPUP_DELAY, true );
   }

   mouseOver = true;
   update( false );
}

void KasItem::dragEnter()
{
   static const int DRAG_SWITCH_DELAY = 1000;

   if ( dragTimer == 0 ) {
      dragTimer = new QTimer( this, "dragTimer" );
      connect( dragTimer, SIGNAL( timeout() ), SLOT( dragOverAction() ) );
      dragTimer->start( DRAG_SWITCH_DELAY, true );
   }

   mouseOver = true;
   update( false );
}

void KasItem::mouseLeave()
{
   if ( popupTimer ) {
      delete popupTimer;
      popupTimer = 0;
   }
   if ( (!customPopup) && pop )
      hidePopup();

   mouseOver = false;
   update( false );
}

void KasItem::dragLeave()
{
   if ( dragTimer ) {
      delete dragTimer;
      dragTimer = 0;
   }

   mouseOver = false;
   update( false );
}

void KasItem::showPopup()
{
   if ( pop )
      return;

   pop = createPopup();
   if ( pop )
      pop->show();
}

void KasItem::hidePopup()
{
   if ( pop )
      delete pop;
   pop = 0;
}

void KasItem::togglePopup()
{
   if ( pop )
      hidePopup();
   else
      showPopup();
}

void KasItem::paintFrame( QPainter *p, int x, int y )
{
   qDrawShadePanel(p, x, y, kas->itemExtent(), kas->itemExtent(), colorGroup(), false, 2);

   if ( mouseOver )
      p->setPen(Qt::white);
   else
      p->setPen(Qt::black);

   p->drawRect(x, y, kas->itemExtent(), kas->itemExtent());
}

void KasItem::paintLabel( QPainter *p, int x, int y, const QString &text  )
{
   p->fillRect( x+2, y+2, kas->itemExtent()-4, 13, QBrush( Qt::black ) );
   p->setPen( Qt::white );

   if ( fontMetrics().width( text ) > kas->itemExtent()-4 )
      p->drawText( x+2, y+2, kas->itemExtent()-4, 12, AlignLeft | AlignVCenter,
		   text );
   else
      p->drawText( x+2, y+2, kas->itemExtent()-4, 12, AlignCenter, text );
}

void KasItem::paintActiveBg( QPainter *p, int x, int y )
{
   p->drawPixmap( x+2, y+15, *(kas->activeBg()) );
}

void KasItem::paintInactiveBg( QPainter *p, int x, int y )
{
   if ( !kas->isTransparent() )
      p->drawPixmap( x+2, y+15, *(kas->inactiveBg()) );
   else
     kasbar()->erase( x+2, y+15, kas->itemExtent()-3, kas->itemExtent()-17);
}

void KasItem::paint( QPainter *p, int x, int y )
{
   //
   // Basic item.
   //
   paintFrame( p, x, y );
   paintLabel( p, x, y, "Kasbar" );
   paintInactiveBg( p, x, y );

   QPixmap pixmap = KGlobal::iconLoader()->loadIcon( "wizard",
						     KIcon::NoGroup,
						     KIcon::SizeMedium );

   if ( !pixmap.isNull() )
      p->drawPixmap(x+4, y+16, pixmap );
}

void KasItem::update()
{
   kas->repaintItem( this );
}

void KasItem::update( bool erase )
{
   kas->repaintItem( this, erase );
}

#include "kasitem.moc"
