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

#include <kglobal.h>
#include <klocale.h>
#include <kapp.h>
#include <kconfig.h>
#include <kdebug.h>

#include "windowlistbutton.h"
#include "scrollbutton.h"
#include "taskbar.h"

#include "taskbarapplet.h"
#include "taskbarapplet.moc"

#define WINDOWLISTBUTTON_SIZE 12
#define SCROLLBUTTON_SIZE 12

extern "C"
{
    KPanelApplet* init( QWidget *parent, const QString& configFile )
    {
        KGlobal::locale()->insertCatalogue( "ktaskbarapplet" );
        TaskbarApplet *taskbar = new TaskbarApplet( configFile, KPanelApplet::Stretch,
                                                    KPanelApplet::Preferences, parent, "ktaskbarapplet" );
	return taskbar;
    }
}

TaskbarApplet::TaskbarApplet( const QString& configFile, Type type, int actions,
                             QWidget *parent, const char *name )
    : KPanelApplet( configFile, type, actions, parent, name ), showScrollButtons( FALSE )
{
    setFrameStyle( QFrame::NoFrame );

    // window list button
    windowListButton = new WindowListButton( this );

    // scrollable taskbar
    taskBar = new TaskBar( TRUE, this );
    connect( taskBar, SIGNAL( needScrollButtons( bool ) ), SLOT( enableScrollButtons( bool ) ) );

    // scroll buttons
    leftScrollButton = new ScrollButton( this );
    rightScrollButton = new ScrollButton( this );

    leftScrollButton->setAutoRepeat( TRUE );
    rightScrollButton->setAutoRepeat( TRUE );

    connect( leftScrollButton, SIGNAL( pressed() ), taskBar, SLOT( scrollLeft() ) );
    connect( rightScrollButton, SIGNAL( pressed() ), taskBar, SLOT( scrollRight() ) );

    // read settings and setup layout
    configure();
}

TaskbarApplet::~TaskbarApplet()
{
}

void TaskbarApplet::configure()
{
    KConfig c( "ktaskbarrc", false, false );
    c.setGroup( "General" );

    setFont( c.readFontEntry( "taskbarFont" ) );
    showWindowListButton = c.readBoolEntry( "ShowWindowListBtn", true );

    reLayout();
}

int TaskbarApplet::widthForHeight( int ) const
{
    return 200;
}

int TaskbarApplet::heightForWidth( int ) const
{
    return 200;
}

void TaskbarApplet::preferences()
{
    kapp->startServiceByDesktopName( "kcmtaskbar" );
}

void TaskbarApplet::orientationChange( Orientation )
{
    leftScrollButton->setArrowType( orientation() == Horizontal ? LeftArrow : UpArrow );
    rightScrollButton->setArrowType( orientation() == Horizontal ? RightArrow : DownArrow );
    reLayout();
}

void TaskbarApplet::popupDirectionChange( Direction d )
{
    ArrowType at = UpArrow;

    switch(d) {
        case Up:
            at = UpArrow;
            break;
        case Down:
            at = DownArrow;
            break;
        case Left:
            at = LeftArrow;
            break;
        case Right:
            at = RightArrow;
            break;
    }
    taskBar->setArrowType( at );
    windowListButton->setArrowType( at );
}

void TaskbarApplet::reLayout()
{
    int w = width();
    int h = height();

    // window list button
    if ( showWindowListButton) {

	if ( orientation() == Horizontal )
	    windowListButton->resize( WINDOWLISTBUTTON_SIZE, h );
	else
	    windowListButton->resize( w, WINDOWLISTBUTTON_SIZE );
	windowListButton->move( 0, 0 );
	windowListButton->show();
    }
    else
	windowListButton->hide();

    // scroll buttons

    if ( showScrollButtons ) {

	if ( orientation() == Horizontal ) {
	    leftScrollButton->resize( SCROLLBUTTON_SIZE, h );
	    rightScrollButton->resize( SCROLLBUTTON_SIZE, h );
	    leftScrollButton->move( w - SCROLLBUTTON_SIZE * 2, 0 );
	    rightScrollButton->move( w - SCROLLBUTTON_SIZE, 0 );
	}
	else {
	    leftScrollButton->resize( w, SCROLLBUTTON_SIZE );
	    rightScrollButton->resize( w, SCROLLBUTTON_SIZE );
	    leftScrollButton->move( 0, h - SCROLLBUTTON_SIZE * 2 );
	    rightScrollButton->move( 0, h - SCROLLBUTTON_SIZE );
	}
	leftScrollButton->show();
	rightScrollButton->show();
    }
    else {
	leftScrollButton->hide();
	rightScrollButton->hide();
    }

    // taskbar
    taskBar->setOrientation( orientation() );

    if ( orientation() == Horizontal ) {
	taskBar->move( showWindowListButton == TRUE ? WINDOWLISTBUTTON_SIZE : 0, 0 );
	taskBar->resize( w
			 - ( showWindowListButton == TRUE ? WINDOWLISTBUTTON_SIZE : 0 )
			 - ( showScrollButtons == TRUE ? SCROLLBUTTON_SIZE * 2 : 0 ),
			 h);
    }
    else {
	taskBar->move( 0, showWindowListButton == TRUE ? WINDOWLISTBUTTON_SIZE : 0 );
	taskBar->resize( w, h
			 - ( showWindowListButton == TRUE ? WINDOWLISTBUTTON_SIZE : 0 )
			 - ( showScrollButtons == TRUE ? SCROLLBUTTON_SIZE * 2 : 0 ) );
    }
}

void TaskbarApplet::enableScrollButtons( bool enable )
{
    if ( showScrollButtons == enable ) return;

    showScrollButtons = enable;
    reLayout();
}

void TaskbarApplet::resizeEvent( QResizeEvent* )
{
    reLayout();
}
