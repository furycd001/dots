#include <qpainter.h>
#include <qbitmap.h>
#include <qdrawutil.h>
#include <qtimer.h>

#include <kglobal.h>
#include <kwin.h>
#include <kiconloader.h>
#include <kpixmap.h>
#include <kpixmapeffect.h>
#include <klocale.h>
#include <kstddirs.h>
#include <taskmanager.h>

#include "kaspopup.h"
#include "kasstartupitem.h"

KasStartupItem::KasStartupItem( KasBar *parent, Startup *startup )
    : KasItem( parent )
{
    startup_ = startup;
    frame = 0;
    label = startup_->text();

    //
    // This icon stuff should all be handled by the task manager api, but isn't yet.
    //
    pixmap = icon();

    //
    // Setup animation frames and timer.
    //
    anim.setAutoDelete( true );
    for ( int i = 1; i < 9; i++ )
        anim.append(new QPixmap(locate("data", "kicker/pics/disk" + QString::number(i) + ".png")));

    aniTimer = new QTimer( this );
    connect( aniTimer, SIGNAL( timeout() ),
             this, SLOT( aniTimerFired() ) );
    aniTimer->start( 100 );

    update();
}

KasStartupItem::~KasStartupItem()
{
    delete aniTimer;
}

QPixmap KasStartupItem::icon() const
{
   /**
    * This icon stuff should all be handled by the task manager api, but isn't yet.
    */
   QPixmap pixmap;

   switch( kasbar()->itemSize() ) {
   case KasBar::Small:
     /* ***** NOP ******
	pixmap = KGlobal::iconLoader()->loadIcon( startup_->icon(),
						  KIcon::NoGroup,
						  KIcon::SizeSmall );
     */
      break;
   case KasBar::Medium:
	pixmap = KGlobal::iconLoader()->loadIcon( startup_->icon(),
						  KIcon::NoGroup,
						  KIcon::SizeMedium );
      break;
   case KasBar::Large:
	pixmap = KGlobal::iconLoader()->loadIcon( startup_->icon(),
						  KIcon::NoGroup,
						  KIcon::SizeLarge );
      break;
   default:
	pixmap = KGlobal::iconLoader()->loadIcon( "error",
						  KIcon::NoGroup,
						  KIcon::SizeSmall );
   }

   return pixmap;
}

void KasStartupItem::aniTimerFired()
{
    if ( frame == 7 )
        frame = 0;
    else
        frame++;

    QPainter p( kasbar() );
    QPoint pos = kasbar()->itemPos( this );
    paintAnimation( &p, pos.x(), pos.y() );
}

void KasStartupItem::paintAnimation( QPainter *p, int x, int y )
{
  QPixmap *pix = anim.at(frame);
  if ( pix )
    p->drawPixmap( x+kasbar()->itemExtent()-18, y+16, *pix );
}

void KasStartupItem::paint( QPainter *p, int x, int y )
{
    //
    // Basic frame and label.
    //
    paintFrame( p, x, y );
    paintLabel( p, x, y, label );

    //
    // Draw background fill
    //
    paintInactiveBg( p, x, y );

    if ( kasbar()->itemSize() == KasBar::Small ) {
      //
      // Paint animation only for small mode.
      //
      QPixmap *pix = anim.at(frame);
      if ( pix )
        p->drawPixmap( x+4, y+16, *pix );

    }
    else {
      //
      // Draw icon
      //
      if ( !pixmap.isNull() )
	p->drawPixmap(x+4, y+16, pixmap );

      paintAnimation( p, x, y );
    }
}

#include "kasstartupitem.moc"
