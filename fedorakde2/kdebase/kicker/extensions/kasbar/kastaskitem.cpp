#include <qpainter.h>
#include <qbitmap.h>
#include <qtimer.h>

#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <dcopclient.h>
#include <kapp.h>

#include <taskmanager.h>

#include "kastasker.h"
#include "kastaskpopup.h"
#include "kastaskitem.h"

/* XPM */
static const char *tiny_floppy[]={
"10 10 4 1",
". c None",
"# c #000000",
"b c #a0a0a0",
"a c #ffffff",
".########.",
".#aaaaaa#.",
".#aaaaaa#.",
".#aaaaaa#.",
".########.",
".########.",
".##bbbb##.",
".##bbbb##.",
".##bbbb##.",
".........."};

KasTaskItem::KasTaskItem( KasTasker *parent, Task *task )
  : KasItem( parent ),
    task_(0),
    thumbTimer(0)
{
   task_ = task;
   connect( task, SIGNAL( changed() ), this, SLOT( update() ) );
   connect( task, SIGNAL( activated() ), this, SLOT( startAutoThumbnail() ) );
   connect( task, SIGNAL( deactivated() ), this, SLOT( stopAutoThumbnail() ) );
   connect( task, SIGNAL( iconChanged() ), this, SLOT( iconChanged() ) );
   update();
}

KasTaskItem::~KasTaskItem()
{
}

KasTasker *KasTaskItem::kasbar() const
{
   return static_cast<KasTasker *> (KasItem::kasbar());
}

QPixmap KasTaskItem::icon()
{
   usedIconLoader = false;

   switch( kasbar()->itemSize() ) {
   case KasBar::Small:
     return task_->bestIcon( KIcon::SizeSmall, usedIconLoader );
     break;
   case KasBar::Medium:
     return task_->bestIcon( KIcon::SizeMedium, usedIconLoader );
     break;
   case KasBar::Large:
     return task_->bestIcon( KIcon::SizeLarge, usedIconLoader );
     break;
   }

   return KGlobal::iconLoader()->loadIcon( "error",
					   KIcon::NoGroup,
					   KIcon::SizeSmall );
}

void KasTaskItem::iconChanged()
{
  iconHasChanged = true;
  update();
}

void KasTaskItem::paint( QPainter *p, int x, int y )
{
   paintFrame( p, x, y );
   paintLabel( p, x, y, task_->visibleName() );

   //
   // Draw background fill
   //
   if ( task_->isActive() ) {
      paintActiveBg( p, x, y );
   }
   else {
      paintInactiveBg( p, x, y );
   }

   //
   // Draw icon
   //
   p->drawPixmap(x+4, y+16, icon() );

   //
   // Overlay the small icon if the icon has changed, we have space,
   // and we are using a KIconLoader icon rather than one from the NET props.
   // This only exists because we are almost always using the icon loader for
   // large icons.
   //
   if ( usedIconLoader && iconHasChanged && ( kasbar()->itemSize() == KasBar::Large ) ) {
      p->drawPixmap(x+34, y+18, task_->pixmap() );
   }

   //
   // Draw window state.
   //
   QString deskStr;
   if ( task_->isOnAllDesktops() )
      deskStr = i18n( "All" );
   else
      deskStr.setNum( task_->desktop() );

   KasTasker *kas = kasbar();
   p->setPen( task_->isActive() ? kasbar()->activePenColor() : kasbar()->inactivePenColor() );

   if ( kas->itemSize() != KasBar::Small ) {
     // Medium and Large modes
     p->drawText( x+kasbar()->itemExtent()-fontMetrics().width(deskStr)-3,
		  y+15+fontMetrics().ascent(), deskStr );

     if( task_->isIconified() )
       p->drawPixmap(x+kasbar()->itemExtent()-11, y+kasbar()->itemExtent()-11, *(kas->minIcon()) );
     else if ( task_->isShaded() )
       p->drawPixmap(x+kasbar()->itemExtent()-11, y+kasbar()->itemExtent()-11, *(kas->shadeIcon()) );
     else
       p->drawPixmap(x+kasbar()->itemExtent()-11, y+kasbar()->itemExtent()-11, *(kas->maxIcon()) );
   }
   else {
     // Small mode
     p->drawText( x+kasbar()->itemExtent()-fontMetrics().width(deskStr)-2,
		  y+13+fontMetrics().ascent(), deskStr );

     if( task_->isIconified() )
       p->drawPixmap( x+kasbar()->itemExtent()-9, y+kasbar()->itemExtent()-9,
		      *(kas->microMinIcon()) );
     else if ( task_->isShaded() )
       p->drawPixmap( x+kasbar()->itemExtent()-9, y+kasbar()->itemExtent()-9,
		      *(kas->microShadeIcon()) );
     else
       p->drawPixmap(x+kasbar()->itemExtent()-9, y+kasbar()->itemExtent()-9,
		     *(kas->microMaxIcon()) );
   }

   //
   // Draw document state.
   //
   if ( kasbar()->showModified() && (!( kasbar()->itemSize() == KasBar::Small ) ) ) {
      if ( task_->isModified() ) {
	 QPixmap floppy( tiny_floppy );
	 p->drawPixmap(x+kasbar()->itemExtent()-12, y+kasbar()->itemExtent()-22, floppy );
      }
   }
}

void KasTaskItem::mousePressEvent( QMouseEvent *ev )
{
   hidePopup();

   if ( ev->button() == LeftButton ) {
      if ( task_->isActive() && (!task_->isShaded()) ) {
	 task_->iconify();
      }
      else {
	 if ( task_->isShaded() )
	    task_->setShaded( false );
	 if ( task_->isIconified() )
	    task_->restore();
	 if ( !task_->isActive() )
	    task_->activate();
      }
   }
   else if ( ev->button() == RightButton ) {
      showWindowMenuAt( task_->window(), ev->globalX(), ev->globalY() );
   }
   else {
      refreshThumbnail();
   }
}

KasPopup *KasTaskItem::createPopup()
{
   KasPopup *pop = new KasTaskPopup( this );
   return pop;
}

void KasTaskItem::dragOverAction()
{
  if ( !task_->isOnCurrentDesktop() )
    task_->toCurrentDesktop();
  if ( task_->isShaded() )
    task_->setShaded( false );
  if ( task_->isIconified() )
    task_->restore();
  if ( !task_->isActive() )
    task_->activate();
}

void KasTaskItem::startAutoThumbnail()
{
   if ( thumbTimer )
      return;
   if ( !kasbar()->thumbnailsEnabled() )
      return;

   thumbTimer = new QTimer( this, "thumbTimer" );
   connect( thumbTimer, SIGNAL( timeout() ),
	    this, SLOT( refreshThumbnail() ) );

   if ( kasbar()->thumbnailUpdateDelay() > 0 )
     thumbTimer->start( kasbar()->thumbnailUpdateDelay() * 1000 );

   QTimer::singleShot( 200, this, SLOT( refreshThumbnail() ) );
}

void KasTaskItem::stopAutoThumbnail()
{
  if ( !thumbTimer )
    return;

  delete thumbTimer;
  thumbTimer = 0;
}

void KasTaskItem::refreshThumbnail()
{
   if ( !kasbar()->thumbnailsEnabled() )
     return;
   if ( !task_->isActive() )
     return;

   // TODO: Check if the popup obscures the window
   KasItem *i = kasbar()->itemUnderMouse();
   if ( i && i->isShowingPopup() ) {
     QTimer::singleShot( 200, this, SLOT( refreshThumbnail() ) );
     return;
   }

   task_->setThumbnailSize( kasbar()->thumbnailSize() );
   task_->updateThumbnail();
}

void KasTaskItem::showWindowMenuAt( WId id, int x, int y )
{
   QByteArray data;
   QDataStream arg(data, IO_WriteOnly);
   arg << id;
   arg << x;
   arg << y;
   if ( !(kapp->dcopClient()->send( "kwin", "KWinInterface", 
				    "showWindowMenuAt(unsigned long int,int,int)",
				    data)))
      qDebug("kasbar: Unable to display window menu, could not talk to KWin via DCOP.");
}

#include "kastaskitem.moc"
