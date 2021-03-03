#include <qpainter.h>
#include <qbitmap.h>
#include <qtimer.h>
#include <qwmatrix.h>

#include <kglobal.h>
#include <kwin.h>
#include <kiconloader.h>
#include <kpixmap.h>
#include <kpixmapeffect.h>
#include <klocale.h>

#include <taskmanager.h>

#include "kastasker.h"
#include "kasgrouppopup.h"
#include "kasgroupitem.h"

/* XPM */
static const char *tiny_arrow[]={
"5 9 2 1",
". c None",
"# c #ffffff",
"....#",
"...##",
"..###",
".####",
"#####",
".####",
"..###",
"...##",
"....#"};

#ifndef KDE_USE_FINAL
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
#endif

KasGroupItem::KasGroupItem( KasTasker *parent )
   : KasItem( parent ),
     title_( "Group" ), items()
     
{
   items.setAutoDelete( false );
   setCustomPopup( true );
   connect( parent, SIGNAL( layoutChanged() ), this, SLOT( hidePopup() ) );
   connect( parent, SIGNAL( layoutChanged() ), this, SLOT( update() ) );
}

KasGroupItem::~KasGroupItem()
{
}

KasTasker *KasGroupItem::kasbar() const
{
   return static_cast<KasTasker *> (KasItem::kasbar());
}

void KasGroupItem::setTitle( const QString &title )
{
  title_ = title;
  update();
}

void KasGroupItem::paintLabel( QPainter *p, int x, int y, const QString &text,
			       int arrowSize, bool arrowOnLeft )
{
   int lx = x+2;
   int ly = y+2;
   int w = kasbar()->itemExtent()-4;
   int h = 13;
   arrowSize+=2; // Add a space

   p->fillRect( lx, ly, w, h, QBrush( Qt::black ) );

   // Adjust for arrow
   if ( arrowOnLeft ) {
      lx += arrowSize;
      w -= arrowSize;
   }
   else {
      w -= arrowSize;
   }

   p->setPen( Qt::white );
   if ( fontMetrics().width( text ) > w )
      p->drawText( lx, ly, w, h-1, AlignLeft | AlignVCenter, text );
   else
      p->drawText( lx, ly, w, h-1, AlignCenter, text );
}

void KasGroupItem::paintArrowLabel( QPainter *p, int x, int y, const QString &text )
{
   QPixmap arrow( tiny_arrow );

   QPoint popupPos = KasPopup::calcPosition( this, 10, 10 );
   QPoint pos = kasbar()->mapToGlobal( QPoint( x, y ) );
   QWMatrix turn;

   if ( popupPos.x() < pos.x() ) {
      paintLabel( p, x, y, text, arrow.width(), true );
      p->drawPixmap( x+3, y+4, arrow );
   }
   else if ( popupPos.x() == pos.x() ) {
      if ( popupPos.y() < pos.y() ) {
	 turn.rotate( 90.0 );
	 arrow = arrow.xForm( turn );
	 paintLabel( p, x, y, text, arrow.width(), true );
	 p->drawPixmap( x+3, y+6, arrow );
      }
      else {
	 turn.rotate( 270.0 );
	 arrow = arrow.xForm( turn );
	 paintLabel( p, x, y, text, arrow.width(), false );
	 p->drawPixmap( x+kasbar()->itemExtent()-12, y+6, arrow );
      }
   }
   else {
      turn.rotate( 180.0 );
      arrow = arrow.xForm( turn );
      paintLabel( p, x, y, text, arrow.width(), false );
      p->drawPixmap( x+kasbar()->itemExtent()-8, y+4, arrow );
   }
}

QPixmap KasGroupItem::icon()
{
   bool usedIconLoader = false;
   Task *t = items.first();
   if (!t)
      return KGlobal::iconLoader()->loadIcon( "kicker",
	                                      KIcon::NoGroup,
					      KIcon::SizeSmall );

   switch( kasbar()->itemSize() ) {
   case KasBar::Small:
     return t->bestIcon( KIcon::SizeSmall, usedIconLoader );
     break;
   case KasBar::Medium:
     return t->bestIcon( KIcon::SizeMedium, usedIconLoader );
     break;
   case KasBar::Large:
     return t->bestIcon( KIcon::SizeLarge, usedIconLoader );
     break;
   }

   return KGlobal::iconLoader()->loadIcon( "error",
					   KIcon::NoGroup,
					   KIcon::SizeSmall );
}

void KasGroupItem::paint( QPainter *p, int x, int y )
{
   paintFrame( p, x, y );
   paintArrowLabel( p, x, y, title_ );

   if ( isShowingPopup() )
     paintActiveBg( p, x, y );
   else
     paintInactiveBg( p, x, y );

   //
   // Paint the icon
   //
   p->drawPixmap(x+10, y+16, icon() );

   //
   // Item summary info
   //
   int modCount = 0;
   for ( Task *t = items.first(); t != 0 ; t = items.next() ) {
     if ( t->isModified() )
       modCount++;
   }

   p->setPen( isShowingPopup() ? kasbar()->activePenColor() : kasbar()->inactivePenColor() );

   if ( modCount ) {
     QString modCountStr;
     modCountStr.setNum( modCount );
     p->drawText( x+kasbar()->itemExtent()-fontMetrics().width( modCountStr )-3,
		  y+15+fontMetrics().ascent(),
		  modCountStr );

     QPixmap floppy( tiny_floppy );
     p->drawPixmap(x+kasbar()->itemExtent()-12, y+29, floppy );
   }

   int microsPerCol;
   switch( kasbar()->itemSize() ) {
   default:
   case KasBar::Small:
     microsPerCol = 2;
     break;
   case KasBar::Medium:
     microsPerCol = 4;
     break;
   case KasBar::Large:
     microsPerCol = 7;
     break;
   }

   int xpos = x+3;
   int ypos = y+16;

   for ( int i = 0; ( i < (int) items.count() ) && ( i < microsPerCol ); i++ ) {
      Task *t = items.at( i );
      if (!t)
	break;

      if( t->isIconified() )
	p->drawPixmap( xpos, ypos,
		      *(kasbar()->microMinIcon()) );
      else if ( t->isShaded() )
	p->drawPixmap( xpos, ypos,
		      *(kasbar()->microShadeIcon()) );
      else
	p->drawPixmap( xpos, ypos,
		      *(kasbar()->microMaxIcon()) );

      ypos += 7;
   }

   if ( ((int) items.count() > microsPerCol) && ( kasbar()->itemSize() != KasBar::Small ) ) {
     QString countStr;
     countStr.setNum( items.count() );
     p->drawText( x+kasbar()->itemExtent()-fontMetrics().width( countStr )-3,
		  y+kasbar()->itemExtent()+fontMetrics().ascent()-16,
		  countStr );
   }
}

void KasGroupItem::mousePressEvent( QMouseEvent * )
{
  togglePopup();
  update();
}

KasPopup *KasGroupItem::createPopup()
{
  if ( items.count() ) {
    KasGroupPopup *pop = new KasGroupPopup( this );
    KasTasker *bar = pop->childBar();

    for ( Task *t = items.first(); t != 0; t = items.next() ) {
      bar->addTask( t );
    }
    pop->resize( bar->size() );

    return pop;
  }
  return 0;

//     // Test code
//     //
//     // This generates cool looking fractal-like patterns if you keep unfolding the
//     // groups!
//     int pos = (int) this; 
//     if ( pos % 2 )
//        bar->append( new KasItem( bar ) );
//     if ( pos % 5 )
//        bar->append( new KasItem( bar ) );
//     bar->append( new KasGroupItem( bar ) );
//     if ( pos % 3 )
//        bar->append( new KasItem( bar ) );
//     if ( pos % 7 )
//        bar->append( new KasItem( bar ) );
//     ////////////
}

void KasGroupItem::addTask( Task *t )
{
  if (!t)
    return;

  items.append( t );
  if ( items.count() == 1 )
    setTitle( t->visibleName() );

  connect( t, SIGNAL( changed() ), this, SLOT( update() ) );
  update();
}

void KasGroupItem::removeTask( Task *t )
{
  if ( !t )
    return;

  hidePopup();
  items.remove( t );

  if ( items.count() == 1 )
    kasbar()->moveToMain( this, items.first() );
}

#include "kasgroupitem.moc"
