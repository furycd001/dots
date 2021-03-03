#include <qpainter.h>
#include <krootpixmap.h>
#include <kpixmap.h>
#include <kpixmapeffect.h>
#include "kasbar.h"
#include "kasitem.h"

const int SMALL_EXTENT = 36;
const int MEDIUM_EXTENT = 52;
const int LARGE_EXTENT = 68;

KasBar::KasBar( Orientation o, QWidget* parent, const char* name )
   : QWidget( parent, name ),
     items(),
     orient( o ),
     itemUnderMouse_( 0 ),
     maxBoxes_( 100 ), // Temp value
     itemSize_( Medium ),
     itemExtent_( MEDIUM_EXTENT ),
     transparent_( false ),
     enableTint_( false ),
     tintAmount_( 0.1 ), 
     tintColour_( colorGroup().mid() ),
     actBg( 0 ), inactBg( 0 ),
     activePenColor_( Qt::black ), inactivePenColor_( Qt::black )
{
   items.setAutoDelete( true );
   setMouseTracking( true );
   setMaxBoxes( 0 );
}

KasBar::~KasBar()
{
   delete actBg;
   delete inactBg;
}

void KasBar::setItemSize( int size )
{
   if ( size == itemSize_ )
      return;
   itemSize_ = size;

   switch( size ) {
   case Small:
     itemExtent_ = SMALL_EXTENT;
     break;
   case Medium:
     itemExtent_ = MEDIUM_EXTENT;
     break;
   case Large:
     itemExtent_ = LARGE_EXTENT;
     break;
   default:
     itemExtent_ = MEDIUM_EXTENT;
   }

   delete actBg;
   delete inactBg;
   actBg = 0;
   inactBg = 0;

   emit itemSizeChanged( size );
   updateLayout();
}

void KasBar::setTransparent( bool enable )
{
   if ( transparent_ == enable )
      return;

   transparent_ = enable;

   if ( transparent_ ) {
      rootPix = new KRootPixmap( this );

      if ( enableTint_ )
	 rootPix->setFadeEffect( tintAmount_, tintColour_ );

      rootPix->start();

      if ( rootPix->checkAvailable( false ) )
	rootPix->repaint( true );
   }
   else {
      rootPix->stop();
      delete rootPix;
      rootPix = 0;
      setBackgroundMode( NoBackground );
      repaint();
   }
}

void KasBar::setTint( bool enable )
{
   if ( enableTint_ == enable )
      return;

   enableTint_ = enable;

   if ( transparent_ && rootPix ) {
      if ( enableTint_ ) {
	 rootPix->setFadeEffect( tintAmount_, tintColour_ );
      }
      else {
	 rootPix->setFadeEffect( 0.0, tintColour_ );
      }
      if ( rootPix->checkAvailable( false ) )
	rootPix->repaint( true );
   }
}

void KasBar::setTint( double amount, QColor color )
{
   tintAmount_ = amount;
   tintColour_ = color;

   if ( transparent_ && enableTint_ ) {
      rootPix->setFadeEffect( tintAmount_, tintColour_ );
      if ( rootPix->checkAvailable( false ) )
	rootPix->repaint( true );
   }
}

void KasBar::setTintColor( const QColor &c )
{
   setTint( tintAmount_, c );
}

void KasBar::setTintAmount( int percent )
{
   double amt = (double) percent / 100.0;
   setTint( amt, tintColour_ );
}

void KasBar::setMaxBoxes( int count )
{
  if ( count != maxBoxes_ ) {
    if ( count == 0 )
      count = 15; // XXX Hacked

    maxBoxes_ = count;
    updateLayout();
  }
}

QSize KasBar::sizeHint( Orientation o,  QSize )
{
   QSize s;

   if ( !items.count() ) {
      s.setWidth( itemExtent() );
      s.setHeight( itemExtent() );
      return s;
   }

   unsigned int r=0, c=0;
   if( items.count() > (unsigned int) maxBoxes_ ) {
      r = items.count()/maxBoxes_;
      c = maxBoxes_;
   }
   else {
      r = 1;
      c = items.count();
   }

   if( r*c < items.count() ) // remainders
      ++r;

   if( o == Horizontal ) {
      s.setWidth( c*itemExtent() );
      s.setHeight( r*itemExtent() );
   }
   else {
      s.setWidth( r*itemExtent() );
      s.setHeight( c*itemExtent() );
   }

   return s;
}

void KasBar::updateLayout()
{
   if ( !items.count() ) {
      resize(itemExtent(), itemExtent());

      if ( transparent_ ) {
	 rootPix->repaint();
      }
      else
	 repaint( false );

      return;
   }

   unsigned int r=0, c=0;
   if( items.count() > (unsigned int) maxBoxes_ ) {
      r = items.count()/maxBoxes_;
      c = maxBoxes_;
   }
   else{
      r = 1;
      c = items.count();
   }
   if(r*c < items.count()) // remainders
      ++r;

   static unsigned int oldR = 0, oldC = 0;
   if ( oldR != r || oldC != c ) {
      if ( orientation() == Horizontal )
	 resize( c * itemExtent(), r * itemExtent() );
      else
	 resize( r * itemExtent(), c * itemExtent() );
      oldR = r;
      oldC = c;
   }

   if ( transparent_ ) {
     rootPix->repaint();
   }
   else
     repaint( false );
}

void KasBar::append( KasItem *i )
{
   if ( !i )
      return;

   items.append( i );
   updateLayout();
}

void KasBar::insert( int index, KasItem *i )
{
   if ( (!i) || (index < 0) )
      return;

   items.insert( index, i );
   updateLayout();
}

void KasBar::remove( KasItem *i )
{
   items.remove( i );

   if ( i == itemUnderMouse_ )
      itemUnderMouse_ = 0;
   updateLayout();
}

void KasBar::clear()
{
   items.clear();
   itemUnderMouse_ = 0;
   updateLayout();
}

void KasBar::mousePressEvent(QMouseEvent *ev)
{
   KasItem *i = itemAt( ev->pos() );
   if ( i )
      i->mousePressEvent( ev );
}

void KasBar::mouseMoveEvent(QMouseEvent *ev)
{
   KasItem *i = itemAt( ev->pos() );
   if ( itemUnderMouse_ != i ) {
      if ( itemUnderMouse_ )
	 itemUnderMouse_->mouseLeave();
      if ( i )
	 i->mouseEnter();
      itemUnderMouse_ = i;
   }
}

void KasBar::dragMoveEvent ( QDragMoveEvent *ev )
{
   KasItem *i = itemAt( ev->pos() );
   if ( itemUnderMouse_ != i ) {
      if ( itemUnderMouse_ )
	 itemUnderMouse_->dragLeave();
      if ( i )
	 i->dragEnter();
      itemUnderMouse_ = i;
   }
}

void KasBar::paintEvent(QPaintEvent *ev)
{
   QRect cr;
   QPainter p(this);
   KasItem *i;
   int r=0, c=0;

   int totalcols = width()/itemExtent();
   int totalrows = height()/itemExtent();

   if ( items.count() ) {
      if ( orientation() == Horizontal ) {
	 for ( i = items.first(); i; i = items.next() ) {
	    if ( c >= totalcols ) {
	       c = 0; ++r;
	    }
	    cr.setRect(c*itemExtent(), r*itemExtent(), itemExtent(), itemExtent());
	    if ( ev->rect().intersects(cr) )
	       i->paint( &p, c*itemExtent(), r*itemExtent() );
	    ++c;
	 }
      }
      else {
	 for ( i = items.first(); i; i = items.next() ) {
	    if ( r >= totalrows ) {
	       r = 0; ++c;
	    }
	    cr.setRect(c*itemExtent(), r*itemExtent(), itemExtent(), itemExtent());
	    if ( ev->rect().intersects(cr) )
	       i->paint( &p, c*itemExtent(), r*itemExtent() );
	    ++r;
	 }
      }
   }

   if ( !transparent_ ) {
      if( orientation() == Horizontal && c < totalcols ) {
	 p.fillRect( c*itemExtent(), r*itemExtent(), width()-c*itemExtent(),
		     itemExtent(), colorGroup().brush(QColorGroup::Mid) );
      }
      else if( r < totalrows ) {
	 p.fillRect( c*itemExtent(), r*itemExtent(), itemExtent(),
		     height()-r*itemExtent(), colorGroup().brush(QColorGroup::Mid) );
      }
   }
}

void KasBar::resizeEvent(QResizeEvent *ev)
{
   QWidget::resizeEvent(ev);
   emit layoutChanged();
}

void KasBar::repaintItem(KasItem *i, bool erase )
{
   QPoint p = itemPos( i );
   repaint( QRect( p, QSize( itemExtent(), itemExtent()  ) ), transparent_ || erase );
}

KasItem* KasBar::itemAt(const QPoint &p)
{
   QRect cr;
   KasItem *i;
   int r=0, c=0;
   int totalcols = width()/itemExtent();
   int totalrows = height()/itemExtent();

   if(orient == Horizontal){
      for (i = items.first(); i; i = items.next()){
	 if(c >= totalcols){
	    c = 0;
	    ++r;
	 }
	 cr.setRect(c*itemExtent(), r*itemExtent(), itemExtent(), itemExtent());
	 if(cr.contains(p))
	    return(i);
	 ++c;
      }
   }
   else {
      for (i = items.first(); i; i = items.next()) {
	 if(r >= totalrows){
	    r = 0; ++c;
	 }
	 cr.setRect(c*itemExtent(), r*itemExtent(), itemExtent(), itemExtent());
	 if(cr.contains(p))
	    return(i);
	 ++r;
      }
   }

   return 0;
}

QPoint KasBar::itemPos( KasItem *i )
{
   int x;
   int y;
   int totalcols = width()/itemExtent();
   int totalrows = height()/itemExtent();
   int index = items.find( i );
   if ( index == -1 ) {
      x = y = -1;
      return QPoint( x, y );
   }

   int r = 0;
   int c = 0;

   if ( ( orient == Horizontal ) && totalcols ) {
      r = index / totalcols;
      c = index % totalcols;
   }
   else if ( ( orient == Vertical ) && totalrows ) {
      c = index / totalrows;
      r = index % totalrows;
   }

   x = c * itemExtent();
   y = r * itemExtent();
   return QPoint( x, y );
}

KPixmap *KasBar::activeBg()
{
   if ( !actBg ) {
      actBg = new KPixmap;

      actBg->resize( itemExtent()-4, itemExtent()-13-4 );
      KPixmapEffect::gradient( *actBg,
			       colorGroup().light(), colorGroup().mid(),
			       KPixmapEffect::DiagonalGradient );
   }

   return actBg;
}

KPixmap *KasBar::inactiveBg()
{
   if ( !inactBg ) {
      inactBg = new KPixmap;

      inactBg->resize( itemExtent()-4, itemExtent()-13-4 );
      KPixmapEffect::gradient( *inactBg,
			       colorGroup().mid(), colorGroup().dark(),
			       KPixmapEffect::DiagonalGradient );
   }

   return inactBg;
}
#include "kasbar.moc"
