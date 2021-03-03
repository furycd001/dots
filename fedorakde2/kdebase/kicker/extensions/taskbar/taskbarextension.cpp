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

#include "taskbarextension.h"
#include "taskbarextension.moc"

#define WINDOWLISTBUTTON_SIZE 12
#define SCROLLBUTTON_SIZE 12
#define BUTTON_HEIGHT 20

extern "C"
{
    KPanelExtension* init( QWidget *parent, const QString& configFile )
    {
        KGlobal::locale()->insertCatalogue( "taskbarextension" );
   	return new TaskBarExtension( configFile, KPanelExtension::Stretch,
				     KPanelExtension::Preferences, parent, "taskbarextension" );
    }
}

TaskBarExtension::TaskBarExtension(const QString& configFile, Type type,
				   int actions, QWidget *parent, const char *name)
    : KPanelExtension(configFile, type, actions, parent, name), showScrollButtons( FALSE )
{
    setFrameStyle( QFrame::NoFrame );

    // window list button
    windowListButton = new WindowListButton( this );

    // scrollable taskbar
    taskBar = new TaskBar( FALSE, this );
    connect( taskBar, SIGNAL( needScrollButtons( bool ) ), SLOT( enableScrollButtons( bool ) ) );
    connect( taskBar, SIGNAL( containerCountChanged() ), SLOT( containerCountChanged() ) );

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

TaskBarExtension::~TaskBarExtension()
{
}

void TaskBarExtension::configure()
{
    KConfig c( "ktaskbarrc", false, false );
    c.setGroup( "General" );

    setFont( c.readFontEntry( "taskbarFont" ) );
    showWindowListButton = c.readBoolEntry( "ShowWindowListBtn", true );

    reLayout();
}

void TaskBarExtension::positionChange( Position p )
{
    leftScrollButton->setArrowType( LeftArrow );
    rightScrollButton->setArrowType( RightArrow );

    ArrowType at = UpArrow;

    switch( p ) {
        case Bottom:
            at = UpArrow;
            break;
        case Top:
            at = DownArrow;
            break;
        case Right:
            at = LeftArrow;
            break;
        case Left:
            at = RightArrow;
            break;
    }
    taskBar->setArrowType( at );
    windowListButton->setArrowType( at );
}

void TaskBarExtension::enableScrollButtons( bool enable )
{
    showScrollButtons = enable;
}

void TaskBarExtension::containerCountChanged()
{
    emit updateLayout();
}

void TaskBarExtension::resizeEvent( QResizeEvent* )
{
    reLayout();
}

void TaskBarExtension::preferences()
{
    kapp->startServiceByDesktopName("kcmtaskbar");
}

QSize TaskBarExtension::sizeHint(Position p, QSize maxSize) const
{
    int v = taskBar->containerCount();
    int offset = 0;

    if ( showWindowListButton)
	offset += WINDOWLISTBUTTON_SIZE;

    TaskBarExtension *non_const_this = const_cast<TaskBarExtension *>(this);
    if ( (non_const_this->orientation() == Horizontal) && showScrollButtons )
	offset += 2 * SCROLLBUTTON_SIZE;

    if ( p == Left || p == Right )
	return QSize( 130, BUTTON_HEIGHT * v + offset );
    else {
	return QSize( maxSize.width(), BUTTON_HEIGHT );
    }
}

void TaskBarExtension::reLayout()
{
    Qt::Orientation orient = orientation();
    int w = width();
    int h = height();

    // window list button
    if ( showWindowListButton) {

	if ( orient == Horizontal )
	    windowListButton->resize( WINDOWLISTBUTTON_SIZE, h );
	else
	    windowListButton->resize( w, WINDOWLISTBUTTON_SIZE );
	windowListButton->move( 0, 0 );
	windowListButton->show();
    }
    else
	windowListButton->hide();

    // scroll buttons
    if (( orient == Horizontal ) && showScrollButtons) {
	leftScrollButton->resize( SCROLLBUTTON_SIZE, h );
	rightScrollButton->resize( SCROLLBUTTON_SIZE, h );
	leftScrollButton->move( w - SCROLLBUTTON_SIZE * 2, 0 );
	rightScrollButton->move( w - SCROLLBUTTON_SIZE, 0 );
	leftScrollButton->show();
	rightScrollButton->show();
    }
    else {
	leftScrollButton->hide();
	rightScrollButton->hide();
    }

    // taskbar
    taskBar->setOrientation( orient );

    if ( orient == Horizontal ) {
	taskBar->move( showWindowListButton == TRUE ? WINDOWLISTBUTTON_SIZE : 0, 0 );
	taskBar->resize( w
			 - ( showWindowListButton == TRUE ? WINDOWLISTBUTTON_SIZE : 0 )
			 - ( showScrollButtons == TRUE ? SCROLLBUTTON_SIZE * 2 : 0 ),
			 h);
    }
    else {
	taskBar->move( 0, showWindowListButton == TRUE ? WINDOWLISTBUTTON_SIZE : 0 );
	taskBar->resize( w, h
			 - ( showWindowListButton == TRUE ? WINDOWLISTBUTTON_SIZE : 0 ) );
    }
}
