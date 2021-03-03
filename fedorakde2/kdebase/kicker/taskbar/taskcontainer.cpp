/*****************************************************************

Copyright (c) 2001 Matthias Elter <elter@kde.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.#

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include <qpainter.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qbitmap.h>
#include <qcolor.h>
#include <qtooltip.h>

#include <kdebug.h>
#include <kapp.h>
#include <kstyle.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kiconloader.h>
#include <kimageeffect.h>
#include <kglobalsettings.h>

#include "lmbmenu.h"
#include "rmbmenu.h"
#include "taskcontainer.h"
#include "taskcontainer.moc"

#define ICON_WIDTH 18

QImage TaskContainer::blendGradient = QImage();

TaskContainer::TaskContainer( Task *task, TaskManager* manager, bool show, bool sort, bool icon, QWidget *parent, const char *name )
    : QToolButton( parent, name ), taskManager( manager ), showAll( show ), sortByDesktop( sort ), showIcon( icon )
{
    init();

    tasks.append( task );
    updateFilteredTaskList();
    sid = task->className();
    setAcceptDrops(true);

    connect( task, SIGNAL( changed() ), SLOT( update() ) );
}

TaskContainer::TaskContainer( Startup *startup, TaskManager* manager, bool show, bool sort, bool icon, QWidget *parent, const char *name )
    : QToolButton( parent, name ), taskManager( manager ), showAll( show ), sortByDesktop( sort ), showIcon( icon )
{
    init();

    startups.append( startup );
    sid = startup->bin();

    connect( startup, SIGNAL( changed() ), SLOT( update() ) );
    animationTimer.start( 100 );
}

void TaskContainer::init()
{
    setBackgroundMode( NoBackground );

    tasks.setAutoDelete( FALSE );
    ftasks.setAutoDelete( FALSE );
    startups.setAutoDelete( FALSE );

    lmbMenu = new LMBMenu( &ftasks, taskManager, this );
    rmbMenu = new RMBMenu( &ftasks, taskManager, this );
    connect( this, SIGNAL( clicked() ), SLOT( toggled() ) );

    QToolTip::add( this, name() );
    
    // setup animation frames
    frames.setAutoDelete( FALSE );

    static QPixmap* frame1 = new QPixmap( locate( "data", "kicker/pics/disk1.png" ) );
    static QPixmap* frame2 = new QPixmap( locate( "data", "kicker/pics/disk2.png" ) );
    static QPixmap* frame3 = new QPixmap( locate( "data", "kicker/pics/disk3.png" ) );
    static QPixmap* frame4 = new QPixmap( locate( "data", "kicker/pics/disk4.png" ) );
    static QPixmap* frame5 = new QPixmap( locate( "data", "kicker/pics/disk5.png" ) );
    static QPixmap* frame6 = new QPixmap( locate( "data", "kicker/pics/disk6.png" ) );
    static QPixmap* frame7 = new QPixmap( locate( "data", "kicker/pics/disk7.png" ) );
    static QPixmap* frame8 = new QPixmap( locate( "data", "kicker/pics/disk8.png" ) );

    frames.append( frame1 );
    frames.append( frame2 );
    frames.append( frame3 );
    frames.append( frame4 );
    frames.append( frame5 );
    frames.append( frame6 );
    frames.append( frame7 );
    frames.append( frame8 );

    // timers
    connect( &animationTimer, SIGNAL( timeout() ), SLOT( animationTimerFired() ) );
    connect( &dragSwitchTimer, SIGNAL( timeout() ), SLOT( dragSwitch() ) );
    currentFrame = 0;
}

TaskContainer::~TaskContainer()
{
    animationTimer.stop();
    dragSwitchTimer.stop();
}

void TaskContainer::update()
{
    QToolTip::add( this, name() );
    repaint();
}

void TaskContainer::animationTimerFired()
{
    if (showIcon) {
        QPainter p( this );
        QPixmap *pm = frames.at( currentFrame );

        // draw pixmap
        if ( pm && !pm->isNull() )
	    p.drawPixmap( 4, ( height() - pm->height() ) / 2, *pm );

        // increment frame counter
        if ( currentFrame >= 7)
	    currentFrame = 0;
        else
	    currentFrame++;
    }
}

QSizePolicy TaskContainer::sizePolicy() const
{
    return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

void TaskContainer::add( Task* task )
{
    if ( !task ) return;

    tasks.append( task );
    updateFilteredTaskList();
    connect( task, SIGNAL( changed() ), SLOT( update() ) );
    if ( sid.isEmpty() )
	sid = task->className().lower();

    update();
}

void TaskContainer::add( Startup* startup )
{
    if ( !startup ) return;

    startups.append( startup );
    if ( sid.isEmpty() )
	sid = startup->bin().lower();

    connect( startup, SIGNAL( changed() ), SLOT( update() ) );
    if ( !animationTimer.isActive() )
	animationTimer.start( 100 );

    update();
}

void TaskContainer::remove( Task* task )
{
    if ( !task ) return;

    tasks.removeRef( task );
    updateFilteredTaskList();

    update();
}

void TaskContainer::remove( Startup* startup )
{
    if ( !startup ) return;

    startups.removeRef( startup );
    if ( startups.isEmpty() )
	animationTimer.stop();

    update();
}

bool TaskContainer::contains( Task* task )
{
    if ( !task ) return FALSE;
    return ( tasks.contains( task ) > 0 );
}

bool TaskContainer::contains( Startup* startup )
{
    if ( !startup ) return FALSE;
    return ( startups.contains( startup ) > 0 );
}

bool TaskContainer::contains( WId win )
{
    for ( Task* t = tasks.first(); t ; t = tasks.next() )
	if ( t->window() == win )
	    return TRUE;
    return FALSE;
}

bool TaskContainer::isEmpty()
{
    return ( tasks.isEmpty() && startups.isEmpty() );
}

QString TaskContainer::id()
{
    return sid;
}

void TaskContainer::drawButton( QPainter *painter )
{
    // buffered painting
    QPixmap pm( size() );
    QPainter p ( &pm );

    // draw sunken if we contain th active task
    bool active = FALSE;
    for ( Task* t = ftasks.first(); t ; t = ftasks.next() )
	if ( t->isActive() ) {
	    active = TRUE;
	    break;
	}

    bool sunken = isOn() || active;
    QRect br( style().buttonRect( 0, 0, width(), height() ) );

    // draw button background
    if( kapp->kstyle() )
        kapp->kstyle()->drawKickerTaskButton( &p, 0, 0, width(), height(), colorGroup(), QString::null, sunken, 0, const_cast< QBrush *>( &colorGroup().brush( QColorGroup::Background ) ) );
    else
        style().drawPanel ( &p, 0, 0, width(), height(), colorGroup(), sunken, 1, &colorGroup().brush( QColorGroup::Background ) );

    // move button label by 1,1 on sunken buttons
    if ( sunken )
	p.translate( 1,1 );

    if (showIcon) {
        // find icon
        QPixmap pixmap;

        for ( Task* t = ftasks.first(); t ; t = ftasks.next() )
	    if ( !t->pixmap().isNull() )
	        pixmap = t->pixmap();

        if ( pixmap.isNull() && !startups.isEmpty() )
	    pixmap = SmallIcon( startups.first()->icon() );

	// make sure it is no larger than 16x16
	if ( !pixmap.isNull() ) {
	    if ( pixmap.width() > 16 || pixmap.height() > 16 ) {
	        QImage tmp = pixmap.convertToImage();
		pixmap.convertFromImage( tmp.smoothScale( 16, 16 ) );
	    }
	}

        // draw icon
	if ( !pixmap.isNull() )
	  p.drawPixmap( br.x() , ( height()  - pixmap.height() ) / 2, pixmap );
    }

    // find text
    QString text = name();

    // modified overlay
    static QString modStr = "[" + i18n( "modified" ) + "]";
    int modStrPos = text.find( modStr );

    if ( modStrPos >= 0 ) {

	// +1 because we include a space after the closing brace.
	text.remove( modStrPos, modStr.length() + 1 );

	// draw modified overlay!
	// FIXME
    }

    // draw text
    if ( !text.isEmpty() ) {

	int textPos = showIcon?ICON_WIDTH:ICON_WIDTH-16;
	p.setFont(KGlobalSettings::taskbarFont());
	p.setPen(KGlobalSettings::textColor());
	if ( p.fontMetrics().width( text ) > width() - br.x() * 2 - textPos ) {

	    if ( blendGradient.isNull() || blendGradient.size() != size() ) {

		QPixmap bgpm( size() );
		bgpm.fill( black );
		QImage gradient = KImageEffect::gradient( QSize( 30, height() ), QColor( 0,0,0 ), QColor( 255,255,255 )
							  , KImageEffect::HorizontalGradient );
		QPainter bgp( &bgpm );
		bgp.drawImage( width() - 30, 0, gradient );

		blendGradient = bgpm.convertToImage();
	    }

	    // draw text into overlay pixmap
	    QPixmap tpm( pm );
	    QPainter tp( &tpm );

	    // move button label by 1,1 on sunken buttons (different painter)
	    if ( sunken )
		tp.translate( 1,1 );
	    
	    tp.setFont(KGlobalSettings::taskbarFont());
	    tp.setPen(KGlobalSettings::textColor());
	    tp.drawText( br.x() + textPos, -1, width() - textPos, height(), AlignLeft | AlignVCenter, text );

	    // blend text into background image
	    QImage img = pm.convertToImage();
	    QImage timg = tpm.convertToImage();
	    KImageEffect::blend( img, timg, blendGradient, KImageEffect::Red );

	    pm.convertFromImage( img );
	}
	else {
	    p.drawText( br.x() + textPos, -1, width() - textPos, height(), AlignLeft | AlignVCenter, text );
	}
    }

    // draw popup arrow
    if ( ftasks.count() >= 2 )
	style().drawArrow( &p, arrowType, sunken, br.x() + br.width() - 10, 2, 10, 10, colorGroup(), true );

    // bitBlt from buffer pixmap to the button widget
    painter->drawPixmap( 0, 0, pm );
}

QString TaskContainer::name()
{
    QString text = id();

    for ( Task* t = ftasks.first(); t ; t = ftasks.next() ) {

	if ( ftasks.count() > 1 ) {
	    if ( !t->visibleIconName().isEmpty() ) {
		text = t->visibleIconName();
		break;
	    }
	}
	else {
	    if ( !t->visibleName().isEmpty() ) {
		text = t->visibleName();
		break;
	    }
	}
    }

    if ( text.isEmpty() ) {
	for ( Startup* s = startups.first(); s ; s = startups.next() )
	    if ( !s->text().isEmpty() ) {
		text = s->text();
		break;
	    }
    }

    // put iconified tasks into parens
    bool alliconified = TRUE;
    for ( Task* t = ftasks.first(); t ; t = ftasks.next() )
	if ( !t->isIconified() ) {
	    alliconified = FALSE;
	    break;
	}

    if ( !ftasks.isEmpty() && alliconified )
	text = QString( "(%1)" ).arg( text );

    return text;
}

void TaskContainer::toggled()
{
    if ( ftasks.isEmpty() )
	return;
    // one task -> (de)activate
    else if ( ftasks.count() == 1) {

	Task* task = ftasks.first();
	if ( ! task->isActive() ) {
//	    kdDebug(1210) << "task->activate();" << endl;
	    task->activate();
	} else if ( !taskManager->isOnTop( task ) ) {
//	    kdDebug(1210) << "task->raise();" << endl;
	    task->raise();
	} else {
//	    kdDebug(1210) << "task->iconify();" << endl;
	    task->iconify();
	}
    }
    // multiple tasks -> cycle list
    else {
	for ( Task* t = ftasks.first(); t ; t = ftasks.next() )
	    if ( t->isActive() ) {

		// activate next
		Task *t = ftasks.next();
		if ( !t )
		    t = ftasks.first();
		t->activate();
		return;
	    }
	ftasks.first()->activate();
    }
}

void TaskContainer::mousePressEvent( QMouseEvent* e )
{
    if ( e->button() == LeftButton ) {
	if ( ftasks.count() >= 2 )
	    popupLMB();
	else
	    QToolButton::mousePressEvent( e );
    }
    else if ( e->button() == MidButton ) {
	toggled();
    }
    else if ( e->button() == RightButton ) {
	popupRMB();
    }
}

void TaskContainer::mouseReleaseEvent( QMouseEvent* e )
{
    QToolButton::mouseReleaseEvent( e );
}

void TaskContainer::setArrowType( Qt::ArrowType at )
{
    if( arrowType == at )
	return;

    arrowType = at;
    repaint();
}

void TaskContainer::popupLMB()
{
    lmbMenu->init();

    // calc popup menu position
    QPoint pos( mapToGlobal( QPoint( 0, 0 ) ) );

    switch( arrowType ) {
	case RightArrow:
	    pos.setX( pos.x() + width() );
	    break;
	case LeftArrow:
            pos.setX( pos.x() - lmbMenu->sizeHint().width() );
	    break;
	case DownArrow:
	    pos.setY( pos.y() + height() );
	    break;
	case UpArrow:
	    pos.setY( pos.y() - lmbMenu->sizeHint().height() );
	default:
	    break;
    }
    lmbMenu->exec( pos );
}

void TaskContainer::popupRMB()
{
    if ( ftasks.count() < 1 ) return;

    if ( ftasks.count() == 1 ) {
	OpMenu* opMenu = new OpMenu( ftasks.first(), taskManager, this );

	// calc popup menu position
	QPoint pos( mapToGlobal( QPoint( 0, 0 ) ) );

	switch( arrowType ) {
	    case RightArrow:
		pos.setX( pos.x() + width() );
		break;
	    case LeftArrow:
		pos.setX( pos.x() - opMenu->sizeHint().width() );
		break;
	    case DownArrow:
		pos.setY( pos.y() + height() );
		break;
	    case UpArrow:
		pos.setY( pos.y() - opMenu->sizeHint().height() );
	    default:
		break;
	}
	opMenu->exec( pos );
    }
    else {

	rmbMenu->init();

	// calc popup menu position
	QPoint pos( mapToGlobal( QPoint( 0, 0 ) ) );

	switch( arrowType ) {
	    case RightArrow:
		pos.setX( pos.x() + width() );
		break;
	    case LeftArrow:
		pos.setX( pos.x() - rmbMenu->sizeHint().width() );
		break;
	    case DownArrow:
		pos.setY( pos.y() + height() );
		break;
	    case UpArrow:
		pos.setY( pos.y() - rmbMenu->sizeHint().height() );
	    default:
		break;
	}
	rmbMenu->exec( pos );
    }
}

void TaskContainer::publishIconGeometry( QPoint global )
{
    QPoint p = global + geometry().topLeft();

    for ( Task* t = tasks.first(); t ; t = tasks.next() )
	t->publishIconGeometry( QRect( p.x(), p.y(), width(), height() ) );
}

void TaskContainer::dragEnterEvent( QDragEnterEvent* e )
{
    // if a dragitem is held for over a taskbutton for two seconds,
    // activate corresponding window

    if ( ftasks.count() < 1 ) return;

    if( !ftasks.first()->isActive() )
	dragSwitchTimer.start( 1000, TRUE );

    QToolButton::dragEnterEvent( e );
}

void TaskContainer::dragLeaveEvent( QDragLeaveEvent* e )
{
    dragSwitchTimer.stop();

    QToolButton::dragLeaveEvent( e );
}

void TaskContainer::dragSwitch()
{
    if ( ftasks.count() < 1 ) return;
    // FIXME
    ftasks.first()->activate();
}

int TaskContainer::desktop()
{
    if ( tasks.isEmpty() )
	return taskManager->currentDesktop();

    if ( tasks.count() > 1 )
	return taskManager->numberOfDesktops();

    return tasks.first()->desktop();
}

bool TaskContainer::onCurrentDesktop()
{
    if ( isEmpty() )
	return FALSE;

    if ( tasks.count() < 1
	 && startups.count() > 0 )
	return TRUE;

    for ( Task* t = tasks.first(); t ; t = tasks.next() )
	if ( t->isOnCurrentDesktop() )
	    return TRUE;

    return FALSE;
}

void TaskContainer::setShowAll( bool s )
{
    if ( s == showAll )
	return;

    showAll = s;
    updateFilteredTaskList();
    update();
}

void TaskContainer::setSortByDesktop( bool s )
{
    if ( s == sortByDesktop )
	return;

    sortByDesktop = s;
    updateFilteredTaskList();
    update();
}

void TaskContainer::setShowIcon( bool s )
{
    if ( s == showIcon )
	return;

    showIcon = s;
    updateFilteredTaskList();
    update();
}

void TaskContainer::updateFilteredTaskList()
{
    ftasks.clear();

    if ( showAll ) {
	ftasks = tasks;
    }
    else {
	for ( Task* t = tasks.first(); t ; t = tasks.next() )
	    if ( t->isOnCurrentDesktop() )
		ftasks.append( t );
    }

    // sort container list by desktop
    if ( sortByDesktop && ftasks.count() > 1 ) {

	QList<Task> sorted;
	for ( int desktop = -1; desktop <= taskManager->numberOfDesktops(); desktop++ ) {
	    for ( Task *t = ftasks.first(); t; t = ftasks.next() )
		if ( t->desktop() == desktop )
		    sorted.append( t );
	}
	ftasks = sorted;
    }
}

void TaskContainer::desktopChanged( int )
{
    updateFilteredTaskList();
}

void TaskContainer::windowDesktopChanged( WId )
{
    updateFilteredTaskList();
    update();
}
