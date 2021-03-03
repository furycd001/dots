/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "hierarchyview.h"
#include "formwindow.h"
#include "globaldefs.h"
#include "mainwindow.h"
#include "command.h"
#include "widgetfactory.h"
#include "widgetdatabase.h"
#include "pixmapchooser.h"

#include <qpalette.h>
#include <qobjectlist.h>
#include <qheader.h>
#include <qpopupmenu.h>
#include <qtabwidget.h>
#include <qwizard.h>
#include <qwidgetstack.h>
#include <qtabbar.h>

HierarchyItem::HierarchyItem( QListViewItem *parent, const QString &txt1, const QString &txt2 )
    : QListViewItem( parent, txt1, txt2 )
{
}

HierarchyItem::HierarchyItem( QListView *parent, const QString &txt1, const QString &txt2 )
    : QListViewItem( parent, txt1, txt2 )
{
}

void HierarchyItem::paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int align )
{
    QColorGroup g( cg );
    g.setColor( QColorGroup::Base, backgroundColor() );
    g.setColor( QColorGroup::Foreground, Qt::black );
    g.setColor( QColorGroup::Text, Qt::black );
    QListViewItem::paintCell( p, g, column, width, align );
    p->save();
    p->setPen( QPen( cg.dark(), 1 ) );
    if ( column == 0 )
	p->drawLine( 0, 0, 0, height() - 1 );
    if ( listView()->firstChild() != this ) {
	if ( nextSibling() != itemBelow() && itemBelow()->depth() < depth() ) {
	    int d = depth() - itemBelow()->depth();
	    p->drawLine( -listView()->treeStepSize() * d, height() - 1, 0, height() - 1 );
	}
    }
    p->drawLine( 0, height() - 1, width, height() - 1 );
    p->drawLine( width - 1, 0, width - 1, height() );
    p->restore();
}

QColor HierarchyItem::backgroundColor()
{
    updateBackColor();
    return backColor;
}

void HierarchyItem::updateBackColor()
{
    if ( listView()->firstChild() == this ) {
 	backColor = backColor1;
	return;
    }

    QListViewItemIterator it( this );
    --it;
    if ( it.current() ) {
	if ( ( ( HierarchyItem*)it.current() )->backColor == backColor1 )
	    backColor = backColor2;
	else
	    backColor = backColor1;
    } else {
	backColor == backColor1;
    }
}

void HierarchyItem::setWidget( QWidget *w )
{
    wid = w;
}

QWidget *HierarchyItem::widget() const
{
    return wid;
}





HierarchyList::HierarchyList( QWidget *parent, HierarchyView *view )
    : QListView( parent ), hierarchyView( view )
{
    header()->setMovingEnabled( FALSE );
    normalMenu = 0;
    tabWidgetMenu = 0;
    addColumn( tr( "Name" ) );
    addColumn( tr( "Class" ) );
    setRootIsDecorated( TRUE );
    QPalette p( palette() );
    p.setColor( QColorGroup::Base, QColor( backColor2 ) );
    setPalette( p );
    disconnect( header(), SIGNAL( sectionClicked( int ) ),
		this, SLOT( changeSortColumn( int ) ) );
    setSorting( -1 );
    setHScrollBarMode( AlwaysOff );
    connect( header(), SIGNAL( sizeChange( int, int, int ) ),
	     this, SLOT( updateHeader() ) );
    connect( this, SIGNAL( clicked( QListViewItem * ) ),
	     this, SLOT( objectClicked( QListViewItem * ) ) );
    connect( this, SIGNAL( returnPressed( QListViewItem * ) ),
	     this, SLOT( objectClicked( QListViewItem * ) ) );
    connect( this, SIGNAL( rightButtonPressed( QListViewItem *, const QPoint&, int ) ),
	     this, SLOT( showRMBMenu( QListViewItem *, const QPoint & ) ) );
    deselect = TRUE;
}

void HierarchyList::keyPressEvent( QKeyEvent *e )
{
    if ( e->key() == Key_Shift || e->key() == Key_Control )
	deselect = FALSE;
    else
	deselect = TRUE;
    QListView::keyPressEvent( e );
}

void HierarchyList::keyReleaseEvent( QKeyEvent *e )
{
    deselect = TRUE;
    QListView::keyReleaseEvent( e );
}

void HierarchyList::viewportMousePressEvent( QMouseEvent *e )
{
    if ( e->state() & ShiftButton || e->state() & ControlButton )
	deselect = FALSE;
    else
	deselect = TRUE;
    QListView::viewportMousePressEvent( e );
}

void HierarchyList::viewportMouseReleaseEvent( QMouseEvent *e )
{
    QListView::viewportMouseReleaseEvent( e );
}

void HierarchyList::objectClicked( QListViewItem *i )
{
    if ( !i )
	return;

    QWidget *w = findWidget( i );
    if ( !w )
	return;
    if ( hierarchyView->formWindow() == w ) {
	if ( deselect )
	    hierarchyView->formWindow()->clearSelection( FALSE );
	hierarchyView->formWindow()->emitShowProperties( hierarchyView->formWindow() );
	return;
    }

    if ( !hierarchyView->formWindow()->widgets()->find( w ) ) {
	if ( w->parent() && w->parent()->inherits( "QWidgetStack" ) &&
	     w->parent()->parent() &&
	     ( w->parent()->parent()->inherits( "QTabWidget" ) ||
	       w->parent()->parent()->inherits( "QWizard" ) ) ) {
	    if ( w->parent()->parent()->inherits( "QTabWidget" ) )
		( (QTabWidget*)w->parent()->parent() )->showPage( w );
	    else
		( (QDesignerWizard*)w->parent()->parent() )->setCurrentPage( ( (QDesignerWizard*)w->parent()->parent() )->pageNum( w ) );
	    w = (QWidget*)w->parent()->parent();
	    hierarchyView->formWindow()->emitUpdateProperties( hierarchyView->formWindow()->currentWidget() );
	} else {
	    return;
	}
    }

    if ( deselect )
	hierarchyView->formWindow()->clearSelection( FALSE );
    if ( w->isVisibleTo( hierarchyView->formWindow() ) )
	hierarchyView->formWindow()->selectWidget( w, TRUE );
}

QWidget *HierarchyList::findWidget( QListViewItem *i )
{	
    return ( (HierarchyItem*)i )->widget();
}

QListViewItem *HierarchyList::findItem( QWidget *w )
{
    QListViewItemIterator it( this );
    while ( it.current() ) {
	if ( ( (HierarchyItem*)it.current() )->widget() == w )
	    return it.current();
	++it;
    }
    return 0;
}

QWidget *HierarchyList::current() const
{
    if ( currentItem() )
	return ( (HierarchyItem*)currentItem() )->widget();
    return 0;
}

void HierarchyList::changeNameOf( QWidget *w, const QString &name )
{
    QListViewItem *item = findItem( w );
    if ( !item )
	return;
    item->setText( 0, name );
}

void HierarchyList::resizeEvent( QResizeEvent *e )
{
    QListView::resizeEvent( e );
    QSize vs = viewportSize( 0, contentsHeight() );

    int os = header()->sectionSize( 1 );
    int ns = vs.width() - header()->sectionSize( 0 );
    if ( ns < 16 )
	ns = 16;
	
    header()->resizeSection( 1, ns );
    header()->repaint( header()->width() - header()->sectionSize( 1 ), 0, header()->sectionSize( 1 ), header()->height() );

    int elipsis = fontMetrics().width("...") + 10;
    viewport()->repaint( header()->sectionPos(1) + os - elipsis, 0, elipsis, viewport()->height(), FALSE );
}

void HierarchyList::setup()
{
    clear();
    QWidget *w = hierarchyView->formWindow()->mainContainer();
    if ( w )
	insertObject( w, 0 );
    updateHeader();
}

void HierarchyList::setOpen( QListViewItem *i, bool b )
{
    QListView::setOpen( i, b );
}

void HierarchyList::updateHeader()
{
    QSize s( header()->sectionPos(1) + header()->sectionSize(1), height() );
    QResizeEvent e( s, size() );
    resizeEvent( &e );
    viewport()->repaint( s.width(), 0, width() - s.width(), height(), FALSE );
}

void HierarchyList::insertObject( QObject *o, QListViewItem *parent )
{
    QListViewItem *item = 0;
    QString className = WidgetFactory::classNameOf( o );
    if ( o->inherits( "QLayoutWidget" ) ) {
	switch ( WidgetFactory::layoutType( (QWidget*)o ) ) {
	case WidgetFactory::HBox:
	    className = "HBox";
	    break;
	case WidgetFactory::VBox:
	    className = "VBox";
	    break;
	case WidgetFactory::Grid:
	    className = "Grid";
	    break;
	default:
	    break;
	}
    }

    QString name = o->name();
    if ( o->parent() && o->parent()->inherits( "QWidgetStack" ) &&
	 o->parent()->parent() ) {
	if ( o->parent()->parent()->inherits( "QTabWidget" ) )
	    name = ( (QTabWidget*)o->parent()->parent() )->tabLabel( (QWidget*)o );
	else if ( o->parent()->parent()->inherits( "QWizard" ) )
	    name = ( (QWizard*)o->parent()->parent() )->title( (QWidget*)o );
    }

    if ( !parent )
	item = new HierarchyItem( this, name, className );
    else
	item = new HierarchyItem( parent, name, className );
    if ( !parent )
 	item->setPixmap( 0, PixmapChooser::loadPixmap( "form.xpm", PixmapChooser::Mini ) );
    else if ( o->inherits( "QLayoutWidget") )
 	item->setPixmap( 0, PixmapChooser::loadPixmap( "layout.xpm", PixmapChooser::Small ) );
    else
	item->setPixmap( 0, WidgetDatabase::iconSet( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( o ) ) ).
			 pixmap( QIconSet::Small, QIconSet::Normal ) );
    ( (HierarchyItem*)item )->setWidget( (QWidget*)o );

    const QObjectList *l = o->children();
    if ( !l )
	return;
    QObjectListIt it( *l );
    it.toLast();
    for ( ; it.current(); --it ) {
	if ( !it.current()->isWidgetType() || ( (QWidget*)it.current() )->isHidden() )
	    continue;
	if (  !hierarchyView->formWindow()->widgets()->find( (QWidget*)it.current() ) ) {
	    if ( it.current()->parent() &&
		 ( it.current()->parent()->inherits( "QTabWidget" ) ||
		   it.current()->parent()->inherits( "QWizard" ) ) &&
		 it.current()->inherits( "QWidgetStack" ) ) {
		QObject *obj = it.current();
		QObjectList *l2 = obj->queryList( "QWidget", 0, TRUE, FALSE );
		QDesignerTabWidget *tw = 0;
		QDesignerWizard *dw = 0;
		if ( it.current()->parent()->inherits( "QTabWidget" ) )
		    tw = (QDesignerTabWidget*)it.current()->parent();
		if ( it.current()->parent()->inherits( "QWizard" ) )
		    dw = (QDesignerWizard*)it.current()->parent();
		QWidgetStack *stack = (QWidgetStack*)obj;
		for ( obj = l2->last(); obj; obj = l2->prev() ) {
		    if ( qstrcmp( obj->className(), "QWidgetStackPrivate::Invisible" ) == 0 ||
			 ( tw && !tw->tabBar()->tab( stack->id( (QWidget*)obj ) ) ) ||
			 ( dw && dw->isPageRemoved( (QWidget*)obj ) ) )
			continue;
		    insertObject( obj, item );
		}
		delete l2;
	    }
	    continue;
	}
	insertObject( it.current(), item );
    }

    if ( item->firstChild() )
	item->setOpen( TRUE );
}

void HierarchyList::setCurrent( QWidget *w )
{
    QListViewItemIterator it( this );
    while ( it.current() ) {
	if ( ( (HierarchyItem*)it.current() )->widget() == w ) {
	    blockSignals( TRUE );
	    setCurrentItem( it.current() );
	    ensureItemVisible( it.current() );
	    blockSignals( FALSE );
	    return;
	}
	++it;
    }
}

void HierarchyList::showRMBMenu( QListViewItem *i, const QPoint & p )
{
    if ( !i )
	return;


    QWidget *w = findWidget( i );
    if ( !w )
	return;

    if ( w != hierarchyView->formWindow() &&
	 !hierarchyView->formWindow()->widgets()->find( w ) )
	return;

    if ( w->isVisibleTo( hierarchyView->formWindow() ) ) {
	if ( !w->inherits( "QTabWidget" ) && !w->inherits( "QWizard" ) ) {
	    if ( !normalMenu )
		normalMenu = hierarchyView->formWindow()->mainWindow()->setupNormalHierarchyMenu( this );
	    normalMenu->popup( p );
	} else {
	    if ( !tabWidgetMenu )
		tabWidgetMenu =
		    hierarchyView->formWindow()->mainWindow()->setupTabWidgetHierarchyMenu( this, SLOT( addTabPage() ),
											  SLOT( removeTabPage() ) );
	    tabWidgetMenu->popup( p );
	}
    }
}

void HierarchyList::addTabPage()
{
    QWidget *w = current();
    if ( !w )
	return;
    if ( w->inherits( "QTabWidget" ) ) {
	QTabWidget *tw = (QTabWidget*)w;
	AddTabPageCommand *cmd = new AddTabPageCommand( tr( "Add Page to %1" ).arg( tw->name() ), hierarchyView->formWindow(),
							tw, "Tab" );
	hierarchyView->formWindow()->commandHistory()->addCommand( cmd );
	cmd->execute();
    } else if ( w->inherits( "QWizard" ) ) {
	QWizard *wiz = (QWizard*)hierarchyView->formWindow()->mainContainer();
	AddWizardPageCommand *cmd = new AddWizardPageCommand( tr( "Add Page to %1" ).arg( wiz->name() ), hierarchyView->formWindow(),
							      wiz, "Page" );
	hierarchyView->formWindow()->commandHistory()->addCommand( cmd );
	cmd->execute();
    }
}

void HierarchyList::removeTabPage()
{
    QWidget *w = current();
    if ( !w )
	return;
    if ( w->inherits( "QTabWidget" ) ) {
	QTabWidget *tw = (QTabWidget*)w;
	if ( tw->currentPage() ) {
	    QDesignerTabWidget *dtw = (QDesignerTabWidget*)tw;
	    DeleteTabPageCommand *cmd = new DeleteTabPageCommand( tr( "Remove Page %1 of %2" ).
								  arg( dtw->pageTitle() ).arg( tw->name() ),
								  hierarchyView->formWindow(), tw, tw->currentPage() );
	    hierarchyView->formWindow()->commandHistory()->addCommand( cmd );
	    cmd->execute();
	}
    } else if ( w->inherits( "QWizard" ) ) {
	QWizard *wiz = (QWizard*)hierarchyView->formWindow()->mainContainer();
	if ( wiz->currentPage() ) {
	    QDesignerWizard *dw = (QDesignerWizard*)wiz;
	    DeleteWizardPageCommand *cmd = new DeleteWizardPageCommand( tr( "Remove Page %1 of %2" ).
									arg( dw->pageTitle() ).arg( wiz->name() ),
									hierarchyView->formWindow(), wiz,
									wiz->currentPage() );
	    hierarchyView->formWindow()->commandHistory()->addCommand( cmd );
	    cmd->execute();
	}
    }
}



HierarchyView::HierarchyView( QWidget *parent )
    : QVBox( parent, 0, WStyle_Customize | WStyle_NormalBorder | WStyle_Title |
	     WStyle_Tool |WStyle_MinMax | WStyle_SysMenu )
{
    setIcon( PixmapChooser::loadPixmap( "logo" ) );
    listview = new HierarchyList( this, this );
    formwindow = 0;
}

void HierarchyView::setFormWindow( FormWindow *fw, QWidget *w )
{
    if ( fw == 0 || w == 0 ) {
	listview->clear();
	formwindow = 0;
    }

    if ( fw == formwindow ) {
	listview->setCurrent( w );
	return;
    }

    formwindow = fw;
    listview->setup();
    listview->setCurrent( w );
}

FormWindow *HierarchyView::formWindow() const
{
    return formwindow;
}

void HierarchyView::closeEvent( QCloseEvent *e )
{
    emit hidden();
    e->accept();
}

void HierarchyView::widgetInserted( QWidget * )
{
    listview->setup();
}

void HierarchyView::widgetRemoved( QWidget * )
{
    listview->setup();
}

void HierarchyView::widgetsInserted( const QWidgetList & )
{
    listview->setup();
}

void HierarchyView::widgetsRemoved( const QWidgetList & )
{
    listview->setup();
}

void HierarchyView::namePropertyChanged( QWidget *w, const QVariant & )
{
    listview->changeNameOf( w, w->name() );
}

void HierarchyView::tabsChanged( QTabWidget * )
{
    listview->setup();
}

void HierarchyView::pagesChanged( QWizard * )
{
    listview->setup();
}

void HierarchyView::rebuild()
{
    listview->setup();
}

void HierarchyView::closed( FormWindow *fw )
{
    if ( fw == formwindow )
	listview->clear();
}
