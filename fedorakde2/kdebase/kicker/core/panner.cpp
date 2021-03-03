/*****************************************************************

Copyright (c) 1996-2000 the kicker authors. See file AUTHORS.

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

#include <kdebug.h>

#include "panner.h"
#include "panner.moc"

Panner::Panner( Orientation orientation, QWidget* parent, const char* name )
    : QScrollView( parent, name ), orient(orientation)
{
    setResizePolicy(Manual);
    setVScrollBarMode( QScrollView::AlwaysOff );
    setHScrollBarMode( QScrollView::AlwaysOff );

    viewport()->installEventFilter( this );
    viewport()->setBackgroundMode( PaletteBackground );
}

Panner::~Panner() {}

void Panner::setOrientation(Orientation orientation)
{
    orient = orientation;
    setContentsPos(0,0);
    resize(size());
}

void Panner::resizeEvent( QResizeEvent* e )
{
    QScrollView::resizeEvent( e );

    layoutChildren();
    updateArrows();
}

void Panner::scrollRightDown()
{
    if(orientation() == Horizontal) // scroll right
        scrollBy( 40, 0 );
    else // scroll down
        scrollBy( 0, 40 );
}

void Panner::scrollLeftUp()
{
    if(orientation() == Horizontal) // scroll left
        scrollBy( -40, 0 );
    else // scroll up
        scrollBy( 0, -40 );
}

bool Panner::eventFilter( QObject *, QEvent * e)
{
    if ( e->type() == QEvent::LayoutHint ) {
        layoutChildren();
	updateArrows();
    }
    return false;
}

void Panner::updateArrows()
{
    if ((contentsWidth() - 1 > width() && orient == Horizontal)
        || (contentsHeight() -1 > height() && orient == Vertical))
        emit needScrollButtons(true);
    else
        emit needScrollButtons(false);
}
