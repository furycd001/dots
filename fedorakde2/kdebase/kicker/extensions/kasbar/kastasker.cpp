#include <qbitmap.h>
#include <qapplication.h>
#include <taskmanager.h>

#include "kastaskitem.h"
#include "kasstartupitem.h"
#include "kasgroupitem.h"
#include "kastasker.h"

//
// Bitmap data used for the window state indicators
//
static unsigned char min_bits[] = {
    0x00, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x18, 0x00};
static unsigned char max_bits[] = {
    0xff, 0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xff};
static unsigned char shade_bits[] = {
    0x06, 0x1e, 0x7e, 0xfe, 0xfe, 0x7e, 0x1e, 0x06};

static const char *micro_max[]={
"6 6 2 1",
". c None",
"# c #000000",
"######",
"######",
"##..##",
"##..##",
"######",
"######",
};

static const char *micro_min[]={
"6 6 2 1",
". c None",
"# c #000000",
"......",
"######",
"######",
".####.",
"..##..",
"......"
};

static const char *micro_shade[]={
"6 6 2 1",
". c None",
"# c #000000",
".##...",
".###..",
".####.",
".####.",
".###..",
".##..."
};

KasTasker::KasTasker( Orientation o, QWidget* parent, const char* name )
  : KasBar( o, parent, name ),
    master_( 0 ),
    manager( new TaskManager( this, "taskmanager" ) ),
    minPix( 0 ), maxPix( 0 ), shadePix( 0 ),
    microShadePix(0), 
    microMaxPix(0), 
    microMinPix(0),    
    enableThumbs_( true ),
    thumbnailSize_( 0.2 ),
    enableNotifier_( true ),
    showModified_( true ),
    showAllWindows_( true ),
    thumbUpdateDelay_( 10 ),
    groupWindows_( true )
{
   setAcceptDrops( true );

   connect( manager, SIGNAL( taskAdded(Task*) ), SLOT( addTask(Task*) ) );
   connect( manager, SIGNAL( taskRemoved(Task*) ), SLOT( removeTask(Task*) ) );
   connect( manager, SIGNAL( startupAdded(Startup*) ), SLOT( addStartup(Startup*) ) );
   connect( manager, SIGNAL( startupRemoved(Startup*) ), SLOT( removeStartup(Startup*) ) );
   connect( this, SIGNAL( itemSizeChanged( int ) ), SLOT( refreshAll() ) );
}

KasTasker::KasTasker( Orientation o, KasTasker *master, QWidget* parent, const char* name )
  : KasBar( o, parent, name ),
    master_( master ),
    manager( master->manager ),
    minPix( 0 ), maxPix( 0 ), shadePix( 0 ),
    microShadePix(0), 
    microMaxPix(0),     
    microMinPix(0),
    enableThumbs_( master->enableThumbs_ ),
    thumbnailSize_( master->thumbnailSize_ ),
    enableNotifier_( master->enableNotifier_ ),
    showModified_( master->showModified_ ),
    showAllWindows_( master->showAllWindows_ ),
    thumbUpdateDelay_( master->thumbUpdateDelay_ ),
    groupWindows_( false )
{
  setAcceptDrops( true );

  setItemSize( master->itemSize() );
  setTint( master->hasTint() );
  setTransparent( master->isTransparent() );
  setTintColor( master->tintColor() );
  setTintAmount( master->tintAmount() );
}

KasTasker::~KasTasker()
{
   delete minPix;
   delete maxPix;
   delete shadePix;
   delete microShadePix;
   delete microMaxPix;
   delete microMinPix;
}

KasTaskItem *KasTasker::findItem( Task *t )
{
   KasTaskItem *result = 0;
   for ( uint i = 0; i < itemCount(); i++ ) {
      if ( itemAt(i)->inherits( "KasTaskItem" ) ) {
	 KasTaskItem *curr = static_cast<KasTaskItem *> (itemAt( i ));
	 if ( curr->task() == t ) {
	    result = curr;
	    break;
	 }
      }
   }
   return result;
}

KasStartupItem *KasTasker::findItem( Startup *s )
{
   KasStartupItem *result = 0;
   for ( uint i = 0; i < itemCount(); i++ ) {
      if ( itemAt(i)->inherits( "KasStartupItem" ) ) {
	 KasStartupItem *curr = static_cast<KasStartupItem *> (itemAt( i ));
	 if ( curr->startup() == s ) {
	    result = curr;
	    break;
	 }
      }
   }
   return result;
}

void KasTasker::addTask( Task *t )
{
   KasItem *item = 0;
   if ( showAllWindows_ || t->isOnCurrentDesktop() ) {
      if ( groupWindows_ ) {
	 item = maybeAddToGroup( t );
      }
      if ( !item ) {
	item = new KasTaskItem( this, t );
	append( item );
      }

      //
      // Ensure the window manager knows where we put the icon.
      //
      QPoint p = mapToGlobal( itemPos( item ) );
      QSize s( itemExtent(), itemExtent() );
      t->publishIconGeometry( QRect( p, s ) );
   }
}

KasItem *KasTasker::maybeAddToGroup( Task *t )
{
   KasItem *item = 0;

   QString taskClass = t->className().lower();
   
   for ( uint i = 0; (!item) && (i < itemCount()); i++ ) {
      KasItem *ei = itemAt( i );
      if ( ei->inherits( "KasTaskItem" ) ) {
	 KasTaskItem *eti = static_cast<KasTaskItem *> (ei);
	 
	 // NB This calls Task::className() not QObject::className()
	 QString currClass = eti->task()->className().lower();
	 
	 if ( Task::idMatch( currClass, taskClass ) ) {
	    KasGroupItem *egi = convertToGroup( eti->task() );
	    egi->addTask( t );
	    item = egi;
	    break;
	 }
      }
      else if ( ei->inherits( "KasGroupItem" ) ) {
	 KasGroupItem *egi = static_cast<KasGroupItem *> (ei);

	 for ( int i = 0; i < egi->taskCount(); i++ ) {
	    // NB This calls Task::className() not QObject::className()
	    QString currClass = egi->task( i )->className().lower();
	 
	    if ( Task::idMatch( currClass, taskClass ) ) {
	       egi->addTask( t );
	       item = egi;
	       break;
	    }
	 }
      }
   }

   return item;
}

void KasTasker::removeTask( Task *t )
{
   KasTaskItem *i = findItem( t );
   if ( !i )
     return;

   remove( i );
   refreshIconGeometry();
}

KasGroupItem *KasTasker::convertToGroup( Task *t )
{
  KasTaskItem *ti = findItem( t );
  int i = indexOf( ti );
  KasGroupItem *gi = new KasGroupItem( this );
  gi->addTask( t );
  removeTask( t );
  insert( i, gi );

  connect( manager, SIGNAL( taskRemoved(Task *) ),
	   gi, SLOT( removeTask(Task *) ) );

  return gi;
}

void KasTasker::moveToMain( KasGroupItem *gi, Task *t )
{
  int i = indexOf( gi );
  if ( i != -1 ) {
    remove( gi );
    insert( i, new KasTaskItem( this, t ) );
  }
  else
    append( new KasTaskItem( this, t ) );

  refreshIconGeometry();
}

void KasTasker::addStartup( Startup *s )
{
   if ( enableNotifier_ )
      append( new KasStartupItem( this, s ) );
}

void KasTasker::removeStartup( Startup *s )
{
   KasStartupItem *i = findItem( s );
   remove( i );
}

void KasTasker::refreshAll()
{
   clear();

   QList<Task> l = manager->tasks();
   for ( Task *t = l.first(); t != 0; t = l.next() ) {
      addTask( t );
   }
}

void KasTasker::refreshIconGeometry()
{
   for ( uint i = 0; i < itemCount(); i++ ) {
      if ( itemAt(i)->inherits( "KasTaskItem" ) ) {
	 KasTaskItem *curr = static_cast<KasTaskItem *> (itemAt( i ));

	 QPoint p = mapToGlobal( itemPos( curr ) );
	 QSize s( itemExtent(), itemExtent() );
	 curr->task()->publishIconGeometry( QRect( p, s ) );
      }
   }
}

QBitmap *KasTasker::minIcon()
{
   if ( !minPix ) {
      minPix = new QBitmap(8, 8, min_bits, true);
      minPix->setMask(*minPix);
   }

   return minPix;
}

QBitmap *KasTasker::maxIcon()
{
   if ( !maxPix ) {
      maxPix = new QBitmap(8, 8, max_bits, true);
      maxPix->setMask(*maxPix);
   }
    
   return maxPix;
}

QBitmap *KasTasker::shadeIcon()
{
   if ( !shadePix ) {
      shadePix = new QBitmap(8, 8, shade_bits, true);
      shadePix->setMask(*shadePix);
   }
    
   return shadePix;
}

QPixmap *KasTasker::microShadeIcon()
{
  if ( !microShadePix )
    microShadePix = new QPixmap( micro_shade );

  return microShadePix;
}

QPixmap *KasTasker::microMaxIcon()
{
  if ( !microMaxPix )
    microMaxPix = new QPixmap( micro_max );

  return microMaxPix;
}

QPixmap *KasTasker::microMinIcon()
{
  if ( !microMinPix )
    microMinPix = new QPixmap( micro_min );

  return microMinPix;
}

void KasTasker::setNotifierEnabled( bool enable )
{
   enableNotifier_ = enable;
}

void KasTasker::setThumbnailSize( double size )
{
  thumbnailSize_ = size;
}

void KasTasker::setThumbnailSize( int percent )
{
   double amt = (double) percent / 100.0;
   setThumbnailSize( amt );
}

void KasTasker::setThumbnailsEnabled( bool enable )
{
   enableThumbs_ = enable;
}

void KasTasker::setShowModified( bool enable )
{
   showModified_ = enable;
   update();
}

void KasTasker::setShowAllWindows( bool enable )
{
   if ( showAllWindows_ != enable ) {
      showAllWindows_ = enable;
      refreshAll();
      if ( !showAllWindows_ )
	connect( manager, SIGNAL( desktopChanged( int ) ), SLOT( refreshAll() ) );
      else
	disconnect( manager, SIGNAL( desktopChanged( int ) ), this, SLOT( refreshAll() ) );
   }
}

void KasTasker::setThumbnailUpdateDelay( int secs )
{
  thumbUpdateDelay_ = secs;
}

void KasTasker::setGroupWindows( bool enable )
{
   if ( groupWindows_ != enable ) {
      groupWindows_ = enable;
      refreshAll();
   }
}

#include "kastasker.moc"

