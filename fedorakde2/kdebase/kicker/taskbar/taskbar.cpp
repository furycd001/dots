/*****************************************************************

Copyright (c) 2001 Matthias Elter <elter@kde.org>

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

#include <math.h>

#include <qlayout.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kconfig.h>

#include "taskbar.h"
#include "taskbar.moc"
#include "taskcontainer.h"

TaskManager *TaskBar::manager = 0;

#define BUTTON_MIN_HEIGHT 18
#define BUTTON_HEIGHT 20
#define BUTTON_MAX_WIDTH 200
#define BUTTON_MIN_WIDTH 20
#define SCROLLSTEPPING 10

TaskBar::TaskBar( bool enableFrame, QWidget *parent, const char *name )
    : FittsLawScrollView( parent, name )
{
    orient = Horizontal;
    arrowType = LeftArrow;
    blocklayout = TRUE;

    // init
    containers.setAutoDelete( FALSE );
    viewport()->setBackgroundMode( PaletteBackground );
    setVScrollBarMode( AlwaysOff );
    setHScrollBarMode( AlwaysOff );

    // configure
    configure();

    if ( enableFrame )
	setFrameStyle( Sunken | StyledPanel );
    else
	setFrameStyle( NoFrame );

    // connect manager
    connect( taskManager(), SIGNAL( taskAdded( Task* ) ), SLOT( add( Task* ) ) );
    connect( taskManager(), SIGNAL( taskRemoved( Task* ) ), SLOT( remove( Task* ) ) );
    connect( taskManager(), SIGNAL( startupAdded( Startup* ) ), SLOT( add( Startup* ) ) );
    connect( taskManager(), SIGNAL( startupRemoved( Startup* ) ), SLOT( remove( Startup* ) ) );
    connect( taskManager(), SIGNAL( desktopChanged( int ) ), SLOT( desktopChanged( int ) ) );
    connect( taskManager(), SIGNAL( windowDesktopChanged( WId ) ), SLOT( windowDesktopChanged( WId ) ) );

    // register existant tasks
    QList<Task> tasks = taskManager()->tasks();
    for (Task* t = tasks.first(); t!=0; t = tasks.next())
	add( t );

    // register existant startups
    QList<Startup> startups = taskManager()->startups();
    for (Startup* s = startups.first(); s!=0; s = startups.next())
	add( s );

    blocklayout = FALSE;
}

TaskBar::~TaskBar()
{

}

void TaskBar::configure()
{
   // read settings
    KConfig c( "ktaskbarrc", false, false );
    c.setGroup( "General" );

    showAllWindows = c.readBoolEntry( "ShowAllWindows", true );
    groupTasks = c.readBoolEntry( "GroupTasks", true );
    sortByDesktop = c.readBoolEntry( "SortByDesktop", true );
    showIcon = c.readBoolEntry( "ShowIcon", true );

    for ( TaskContainer *c = containers.first(); c; c = containers.next() ) {
	c->setShowAll( showAllWindows );
	c->setSortByDesktop( sortByDesktop );
    }
    
    if ( !blocklayout )
	reLayout();
}

TaskManager* TaskBar::taskManager()
{
    if ( !manager )
	manager = new TaskManager( this );
    return manager;
}

void TaskBar::setOrientation( Orientation o )
{
    if (orient == o) 
       return;
    orient = o;
    reLayout();
}

void TaskBar::resizeEvent( QResizeEvent* )
{
    reLayout();
}

void TaskBar::add( Task* task )
{
    if ( !task ) return;

    // try to group
    if ( groupTasks ) {
	for ( TaskContainer *c = containers.first(); c; c = containers.next() )
	    if ( idMatch( task->className().lower(), c->id().lower() ) ) {
		c->add( task );

		if ( !blocklayout )
		    reLayout();
		return;
	    }
    }

    // create new container
    TaskContainer *c = new TaskContainer( task, taskManager(), showAllWindows, sortByDesktop, showIcon, viewport() );
    addChild( c );
    containers.append( c );
    emit containerCountChanged();

    if ( !blocklayout )
	reLayout();
}

void TaskBar::add( Startup* startup )
{
    if ( !startup ) return;

    // try to group
    if ( groupTasks ) {
	for ( TaskContainer *c = containers.first(); c; c = containers.next() )
	    if ( idMatch( startup->bin().lower(), c->id().lower() ) ) {
		c->add( startup );

		if ( !blocklayout )
		    reLayout();
		return;
	    }
    }

    // create new container
    TaskContainer *c = new TaskContainer( startup, taskManager(), showAllWindows, sortByDesktop, showIcon, viewport() );
    addChild( c );
    containers.append( c );
    emit containerCountChanged();

    if ( !blocklayout )
	reLayout();
}

void TaskBar::remove( Task *task )
{
    QList<TaskContainer> killList;
    for ( TaskContainer *c = containers.first(); c; c = containers.next() ) {
	if ( c->contains( task ) ) {
	    c->remove( task );

	    if ( c->isEmpty() )
		killList.append( c );
	    break;
	}
    }

    for ( TaskContainer *c = killList.first(); c; c = killList.next() ) {
	containers.removeRef( c );
	delete c;
	emit containerCountChanged();
    }
    reLayout();
}

void TaskBar::remove( Startup *startup )
{
    bool found = false;
    QList<TaskContainer> killList;
    for ( TaskContainer *c = containers.first(); c; c = containers.next() ) {
	if ( c->contains( startup ) ) {
	    c->remove( startup );
        found = true;

	    if ( c->isEmpty() )
		killList.append( c );
	    break;
	}
    }

    for ( TaskContainer *c = killList.first(); c; c = killList.next() ) {
	containers.removeRef( c );
	delete c;
	emit containerCountChanged();
    }

    if (found)
      reLayout();
}

void TaskBar::desktopChanged( int desktop )
{
    if ( !showAllWindows ) {
	for ( TaskContainer *c = containers.first(); c; c = containers.next() )
	    c->desktopChanged( desktop );
        emit containerCountChanged();
	reLayout();
    }
}

void TaskBar::windowDesktopChanged( WId win )
{
    for ( TaskContainer *c = containers.first(); c; c = containers.next() )
	if ( c->contains( win ) )
	    c->windowDesktopChanged( win );

    if ( !showAllWindows )
        emit containerCountChanged();
    reLayout();
}

void TaskBar::reLayout()
{
    // filter task container list
    QList<TaskContainer> list;
    for ( TaskContainer *c = containers.first(); c; c = containers.next() ) {
	if ( showAllWindows ) {
	    list.append( c );
	    c->show();
	}
	else if ( c->onCurrentDesktop() ) {
	    list.append( c );
	    c->show();
	}
	else
	    c->hide();
    }
    if ( list.count() < 1 ) return;

    // sort container list by desktop
    if ( sortByDesktop ) {
	QList<TaskContainer> sorted;
	for ( int desktop = -1; desktop <= taskManager()->numberOfDesktops(); desktop++ ) {
	    for ( TaskContainer *c = list.first(); c; c = list.next() )
		if ( c->desktop() == desktop )
		    sorted.append( c );
	}
	list = sorted;
    }

    bool scrollable = FALSE;

    // init content size
    resizeContents( contentsRect().width(), contentsRect().height() );

    // horizontal layout
    if ( orient == Horizontal ) {

	int bwidth = BUTTON_MIN_WIDTH;

	// number of rows simply depends on our height
	int rows = contentsRect().height() / BUTTON_MIN_HEIGHT;
	if ( rows < 1 ) rows = 1;

	// actual button height
	int bheight = contentsRect().height() / rows;

	// buttons per row
	int bpr = (int)ceil( (double)list.count() / rows);

	// adjust content size
	if ( contentsRect().width() < bpr * BUTTON_MIN_WIDTH ) {
	    resizeContents( bpr * BUTTON_MIN_WIDTH, contentsRect().height() );
	    scrollable = TRUE;
	}

	// maximum number of buttons per row
	int mbpr = contentsRect().width() / BUTTON_MIN_WIDTH;

	// expand button width if space permits
	if ( mbpr > bpr ) {
	    bwidth = contentsRect().width() / bpr;
	    if ( bwidth > BUTTON_MAX_WIDTH )
		bwidth = BUTTON_MAX_WIDTH;
	}

	// layout containers
	int i = 0;
	for ( TaskContainer *c = list.first(); c; c = list.next() ) {

	    int row = i % rows;
	    c->setArrowType( arrowType );
	    c->resize( bwidth, bheight );
	    c->show();

	    moveChild( c, ( i / rows ) * bwidth, row * bheight );
	    i++;
	}

    }
    // vertical layout
    else {

	// adjust content size
	if ( contentsRect().height() < (int)list.count() * BUTTON_HEIGHT ) {
	    resizeContents( contentsRect().width(), list.count() * BUTTON_HEIGHT );
	    scrollable = TRUE;
	}

	// layout containers
	int i = 0;
	for ( TaskContainer *c = list.first(); c; c = list.next() ) {

	    c->setArrowType( arrowType );
	    c->resize( contentsRect().width(), BUTTON_HEIGHT );
	    c->show();

	    moveChild( c, 0, i * BUTTON_HEIGHT );
	    i++;
	}
    }
    emit needScrollButtons( scrollable );
    QTimer::singleShot( 100, this, SLOT( publishIconGeometry() ) );
}

void TaskBar::scrollLeft()
{
    if ( orient == Horizontal )
	scrollBy( -SCROLLSTEPPING, 0 );
    else
	scrollBy( 0, -SCROLLSTEPPING );
}

void TaskBar::scrollRight()
{
    if ( orient == Horizontal )
	scrollBy( SCROLLSTEPPING, 0 );
    else
	scrollBy( 0, SCROLLSTEPPING );

}

void TaskBar::setArrowType( Qt::ArrowType at )
{
    arrowType = at;
    for ( TaskContainer *c = containers.first(); c; c = containers.next() )
	c->setArrowType( arrowType );
}

void TaskBar::publishIconGeometry()
{
    QPoint p = mapToGlobal( QPoint( 0,0 ) ); // roundtrip, don't do that too often

    for ( TaskContainer *c = containers.first(); c; c = containers.next() )
        c->publishIconGeometry( p );
}

void TaskBar::viewportMousePressEvent( QMouseEvent* e )
{
    propagateMouseEvent( e );
}

void TaskBar::viewportMouseReleaseEvent( QMouseEvent* e )
{
    propagateMouseEvent( e );
}

void TaskBar::viewportMouseDoubleClickEvent( QMouseEvent* e )
{
    propagateMouseEvent( e );
}

void TaskBar::viewportMouseMoveEvent( QMouseEvent* e )
{
    propagateMouseEvent( e );
}

void TaskBar::propagateMouseEvent( QMouseEvent* e )
{
    if ( !isTopLevel()  ) {
	QMouseEvent me( e->type(), mapTo( topLevelWidget(), e->pos() ),
			e->globalPos(), e->button(), e->state() );
	QApplication::sendEvent( topLevelWidget(), &me );
    }
}

bool TaskBar::idMatch( const QString& id1, const QString& id2 )
{
    if ( id1.isEmpty() || id2.isEmpty() )
        return FALSE;

    if ( id1.contains( id2 ) > 0 )
	return TRUE;

    if ( id2.contains( id1 ) > 0 )
	return TRUE;

    // add hacks here ;-)
    if ( ( id1 == "navigator" && id2 == "netscape")
	 || ( id1 == "netscape" && id2 == "navigator")
	 || ( id1 == "kfmclient" && id2 == "konqueror")
	 || ( id1 == "konqueror" && id2 == "kfmclient")
	 || ( id1 == "command_shell" && id2 == "ddd" )
	 || ( id1 == "ddd" && id2 == "command_shell" )
	 || ( id1 == "gimp_startup" && id2 == "toolbox" )
	 || ( id1 == "toolbox" && id2 == "gimp_startup" )
	 || ( id1 == "gimp" && id2 == "toolbox" )
	 || ( id1 == "toolbox" && id2 == "gimp" )
	 || ( id1 == "xmms" && id2 == "xmms_player" )
	 || ( id1 == "xmms_player" && id2 == "xmms" )
	)
	return TRUE;

    return FALSE;
}

int TaskBar::containerCount()
{
    int i = 0;
    for ( TaskContainer *c = containers.first(); c; c = containers.next() ) {
	if ( showAllWindows ) {
	    i++;
	}
	else if ( c->onCurrentDesktop() ) {
	    i++;
	}
    }
    return i;
}
