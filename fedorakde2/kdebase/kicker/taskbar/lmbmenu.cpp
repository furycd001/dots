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

#include "lmbmenu.h"
#include "lmbmenu.moc"

LMBMenu::LMBMenu( QList<Task>* list, TaskManager* manager, QWidget *parent, const char *name )
    : QPopupMenu( parent, name ), tasks( list ), taskManager( manager )
{
    setCheckable( TRUE );

    connect( this, SIGNAL( activated( int ) ), SLOT( slotExec( int ) ) );
}

LMBMenu::~LMBMenu()
{

}

void LMBMenu::init()
{
    // clean up
    clear();
    map.clear();

    // sanity check
    if ( tasks->isEmpty() ) return;

    // insert task entries
    for ( Task* t = tasks->first(); t ; t = tasks->next() ) {

	int id = insertItem( QIconSet( t->pixmap() ), t->visibleNameWithState() );
	map.insert( id, t );

	if ( t->isActive() )
	    setItemChecked( id, TRUE );
    }
}

void LMBMenu::slotExec( int id )
{
    Task* task = map[ id ];

    if ( !task ) return;

    if ( ! task->isActive() )
	task->activate();
    else if ( !taskManager->isOnTop( task ) )
	task->raise();
    else
	task->iconify();
}
