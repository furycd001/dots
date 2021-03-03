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

#include <qdrawutil.h>
#include <kwindowlistmenu.h>

#include "windowlistbutton.h"
#include "windowlistbutton.moc"

WindowListButton::WindowListButton( QWidget* parent, const char* name )
    : QToolButton( parent, name ), menu( 0 )
{
    setAutoRaise( true );
    setFocusPolicy( NoFocus );
    connect( this, SIGNAL( pressed() ), SLOT( showMenu() ) );
}

void WindowListButton::drawButtonLabel( QPainter * p )
{
    int nX = 0, nY = 0;
    int arrow = 10;
    if ( arrowType == DownArrow ) {
        nY = height() - arrow;
        nX = (width() - arrow) / 2;
    }
    else if ( arrowType == UpArrow ) {
	nY = 0;
        nX = (width() - arrow) / 2;
    }
    else if ( arrowType == RightArrow ) {
        nX = width() - arrow;
        nY = (height() - arrow) / 2;
    }
    else  {
	nX = 0;
        nY = (height() - arrow) / 2;
    }

    if ( isDown() || isOn() ) {
        nX++;
	nY++;
    }

    style().drawArrow( p, arrowType, isOn(), nX, nY, arrow, arrow, colorGroup(), true );
}

void WindowListButton::showMenu()
{
    if ( !menu ) {
        menu = new KWindowListMenu;
        connect( menu, SIGNAL( aboutToHide() ), SLOT( windowListMenuAboutToHide() ) );
    }

    menu->init();

    // calc popup menu position
    QPoint pos( mapToGlobal( QPoint(0,0) ) );

    switch( arrowType ) {
	case RightArrow:
	    pos.setX( pos.x() + width() );
	    break;
	case LeftArrow:
            pos.setX( pos.x() - menu->sizeHint().width() );
	    break;
	case DownArrow:
	    pos.setY( pos.y() + height() );
	    break;
	case UpArrow:
	    pos.setY( pos.y() - menu->sizeHint().height() );
	default:
	    break;
    }
    menu->exec( pos );

    setDown( false );
}

void WindowListButton::windowListMenuAboutToHide()
{
    setOn( false );
}

void WindowListButton::setArrowType( Qt::ArrowType at )
{
    arrowType = at;
    repaint();
}
