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

#include <qpopupmenu.h>

#include "global.h"
#include "panel.h"
#include "appletop_mnu.h"

#include "container_base.h"
#include "container_base.moc"

BaseContainer::BaseContainer(QWidget *parent)
  : QWidget(parent)
  , _dir(dUp)
  , _orient(Horizontal)
  , _fspace(0)
  , _moveOffset(QPoint(0,0))
  , _aid(QString::null)
  , _opMnu(0)
  , _actions(0)
{}

BaseContainer::~BaseContainer()
{
    delete _opMnu;
}

QPoint BaseContainer::getPopupPosition(QPopupMenu *menu, QPoint eventpos)
{
    QPoint gpos = mapToGlobal(eventpos);

    switch (_dir) {
        case dDown: return(QPoint(gpos.x(), topLevelWidget()->height() + topLevelWidget()->pos().y()));
        case dUp: return(QPoint(gpos.x(), topLevelWidget()->pos().y() - menu->height()));
        case dLeft: return(QPoint(topLevelWidget()->pos().x() - menu->width(), gpos.y()));
        case dRight: return(QPoint(topLevelWidget()->width() + topLevelWidget()->pos().x(), gpos.y()));
    }
    return eventpos;
}

void BaseContainer::removeRequest()
{
    emit removeme( this );
}
