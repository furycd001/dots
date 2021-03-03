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
#include <kiconloader.h>

#include "rmbmenu.h"
#include "rmbmenu.moc"

OpMenu::OpMenu( Task* t, TaskManager* manager, QWidget *parent, const char *name )
    : QPopupMenu( parent, name ), task( t ), taskManager( manager ), deskpopup( 0 )
{
    setCheckable( TRUE );

    connect( this, SIGNAL( aboutToShow() ), SLOT( init() ) );
    connect( this, SIGNAL( activated( int ) ), SLOT( op( int ) ) );

    insertItem( i18n( "Mi&nimize" ), IconifyOp );
    insertItem( i18n( "Ma&ximize" ), MaximizeOp );
    insertItem( i18n( "&Restore" ), RestoreOp );

    insertSeparator();

    insertItem( i18n( "&Shade" ), ShadeOp );
    insertItem( SmallIcon( "attach" ), i18n( "&Always On Top" ), StayOnTopOp );

    insertSeparator();

    insertItem( SmallIcon( "remove" ), i18n( "&Close" ), CloseOp );

    insertSeparator();

    deskpopup = new QPopupMenu( this );
    deskpopup->setCheckable( TRUE );

    connect( deskpopup, SIGNAL( aboutToShow() ), this, SLOT( initDeskPopup() ) );
    connect( deskpopup, SIGNAL( activated( int ) ), this, SLOT( sendToDesktop( int ) ) );


    insertItem( i18n("To &Desktop"), deskpopup );
    insertItem( i18n( "&To Current Desktop" ), ToCurrentOp );
}

OpMenu::~OpMenu()
{

}

void OpMenu::init()
{
    if ( !task ) return;

    setItemEnabled( RestoreOp, task->isIconified() || task->isMaximized() );
    setItemEnabled( IconifyOp, !task->isIconified() );
    setItemEnabled( MaximizeOp, !task->isMaximized() );
    setItemEnabled( ToCurrentOp, !task->isOnCurrentDesktop() );
    setItemChecked( StayOnTopOp, task->isAlwaysOnTop() );
    setItemChecked( ShadeOp, task->isShaded() );
}

void OpMenu::op ( int id )
{
    if ( !task ) return;

    switch (id) {
        case MaximizeOp:
            task->maximize();
            break;
        case IconifyOp:
            task->iconify();
            break;
        case RestoreOp:
            task->restore();
            break;
        case CloseOp:
            task->close();
            break;
        case ToCurrentOp:
            task->toCurrentDesktop();
            break;
        case StayOnTopOp:
            task->setAlwaysOnTop( !task->isAlwaysOnTop() );
            break;
        case ShadeOp:
            task->setShaded( !task->isShaded() );
            break;
        default:
            break;
    }
}

void OpMenu::initDeskPopup()
{
    deskpopup->clear();

    if ( ! taskManager ) return;

    deskpopup->insertItem( i18n("&All Desktops"), 0 );
    deskpopup->insertSeparator();

    if ( task->isOnAllDesktops() )
        deskpopup->setItemChecked( 0, TRUE );

    int id;
    for ( int i = 1; i <= taskManager->numberOfDesktops(); i++ ) {
        id = deskpopup->insertItem( QString( "&%1 %2" ).arg( i ).arg( taskManager->desktopName( i ) ), i );
        if ( !task->isOnAllDesktops() && task->desktop() == i )
            deskpopup->setItemChecked( id, TRUE );
    }
}

void OpMenu::sendToDesktop( int desktop )
{
    if ( !task ) return;
    task->toDesktop( desktop );
}

RMBMenu::RMBMenu( QList<Task>* list, TaskManager* manager, QWidget *parent, const char *name )
    : QPopupMenu( parent, name ), tasks( list ), taskManager( manager )
{
    setCheckable( TRUE );
    connect( this, SIGNAL( activated( int ) ), SLOT( slotExec( int ) ) );
}

RMBMenu::~RMBMenu()
{

}

void RMBMenu::init()
{
    // clean up
    clear();
    map.clear();

    // sanity check
    if ( tasks->isEmpty() ) return;

    // insert task entries
    for ( Task* t = tasks->first(); t ; t = tasks->next() ) {

	int id = insertItem( QIconSet( t->pixmap() ), t->visibleNameWithState(), new OpMenu( t, taskManager, this ) );
	map.insert( id, t );

	if ( t->isActive() )
	    setItemChecked( id, TRUE );
    }
}

void RMBMenu::slotExec( int id )
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
