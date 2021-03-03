/****************************************************************************
** $Id: qt/examples/tetrix/qdragapp.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "qdragapp.h"
#include "qlist.h"
#include "qintdict.h"
#include "qpopupmenu.h"
#include "qcolor.h"
#include "qwidget.h"
#include "qfontmetrics.h"
#include "qcursor.h"
#include "qobjectlist.h"
#include "qobjectdict.h"

QWidget *cursorWidget( QPoint * = 0 );

class QDragger;


class DropWindow : public QWidget
{
    Q_OBJECT
public:
    void paintEvent( QPaintEvent * );
    void closeEvent( QCloseEvent * );

    QDragger *master;
};


struct DropInfo {
    DropInfo()	{ w=0; }
   ~DropInfo()  { delete w; }
    DropWindow *w;
    bool userOpened;
};

struct DraggedInfo {
    QWidget *w;
    QWidget *mother;
    QPoint   pos;
};


class QDragger : public QObject
{
    Q_OBJECT
public:
    QDragger();
    ~QDragger();
    
    bool notify( QObject *, QEvent * ); // event filter
    void closeDropWindow( DropWindow * );
public slots:
    void openDropWindow();
    void killDropWindow();
    void killAllDropWindows();
    void sendChildHome();
    void sendAllChildrenHome();
private:
    bool isParentToDragged( QWidget * );
    bool noWidgets( QWidget * );
    void killDropWindow( DropInfo * );
    void killAllDropWindows( bool );
    void sendChildHome( DraggedInfo * );
    void sendAllChildrenHome( QWidget * );
    QWidget *openDropWindow( const QRect&, bool );

    bool startGrab();
    void grabFinished();
    bool dragEvent( QWidget *, QMouseEvent * );
    bool killDropEvent( QMouseEvent * );
    bool sendChildEvent( QMouseEvent * );

    bool		   killingDrop;
    bool		   sendingChild;
    QWidget		  *clickedWidget;
    QWidget		  *hostWidget;
    QCursor		   cursor;

    QPopupMenu*		   menu;
    QPoint		   clickOffset;
    QColor		   dragBackground;
    QColor		   dragForeground;
    DraggedInfo		   dragInfo;
    QIntDict<DraggedInfo>  draggedDict;
    QIntDict<DropInfo>	   dropDict;
};


QDragApplication::QDragApplication( int &argc, char **argv )
    : QApplication( argc, argv )
{
    dragger = new QDragger;
}

QDragApplication::~QDragApplication()
{
    delete dragger;
}

bool QDragApplication::notify( QObject *o, QEvent *e )
{
    if ( !dragger->notify( o, e ) )
	return o->event( e );
    else
	return FALSE;
}

void DropWindow::paintEvent( QPaintEvent * )
{
    const char *msg    = "Drag widgets and drop them here or anywhere!";
    int		startX = ( width() - fontMetrics().width( msg ) )/2;
    startX	       = startX < 0 ? 0 : startX;

    drawText( startX, height()/2, msg );
}

void DropWindow::closeEvent( QCloseEvent *e )
{
    master->closeDropWindow( this );
    e->ignore();
}

QDragger::QDragger()
{
    dragInfo.w	 = 0;
    hostWidget	 = 0;
    killingDrop	 = FALSE;
    sendingChild = FALSE;
    draggedDict.setAutoDelete( TRUE );
    dropDict   .setAutoDelete( TRUE );

    menu = new QPopupMenu;
    menu->insertItem( "Open drop window", 1 );
    menu->insertItem( "Kill drop window", 2 );
    menu->insertItem( "Kill all drop windows", 3 );
    menu->insertSeparator();
//    menu->insertItem( "Send child home", 4 );
    menu->insertItem( "Send all children home", 5 );

    menu->connectItem( 1, this, SLOT(openDropWindow()) );
    menu->connectItem( 2, this, SLOT(killDropWindow()) );
    menu->connectItem( 3, this, SLOT(killAllDropWindows()) );
//    menu->connectItem( 4, this, SLOT(sendChildHome()) );
    menu->connectItem( 5, this, SLOT(sendAllChildrenHome()) );
}

QDragger::~QDragger()
{
    delete menu;
}


bool QDragger::notify( QObject *o, QEvent *e )
{
    if ( !o->isWidgetType() || o == menu )
	return FALSE;
    switch( e->type() ) {
	case QEvent::MouseMove:
	     {
		 QMouseEvent *tmp = (QMouseEvent*) e;
		 if ( killingDrop )
		     return killDropEvent( tmp );
		 if ( sendingChild )
		     return sendChildEvent( tmp );
		 if ( tmp->state() & QMouseEvent::RightButton )
		     return dragEvent( (QWidget*) o, tmp );
		 break;
	     }
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
	case QEvent::MouseButtonDblClick:
	     {
		 QMouseEvent *tmp = (QMouseEvent*) e;
		 if ( killingDrop )
		     return killDropEvent( tmp );
		 if ( sendingChild )
		     return sendChildEvent( tmp );
		 if ( tmp->button() == QMouseEvent::RightButton )
		     return dragEvent( (QWidget*) o, tmp );
	     }
	     break;
	default:
	     break;
    }
    return FALSE;
}

bool QDragger::isParentToDragged( QWidget *w )
{
    QIntDictIterator<DraggedInfo> iter( draggedDict );

    DraggedInfo *tmp;
    while( (tmp = iter.current()) ) {
	++iter;
	if ( tmp->mother == w )
	    return TRUE;
    }
    return FALSE;
}

bool QDragger::noWidgets( QWidget *w )
{
    const QObjectList *l = w->children();
    if ( !l )
	return TRUE;
    QObjectListIt iter( *l );
    QObject *tmp;
    while( (tmp = iter.current()) ) {
	++iter;
	if ( tmp->isWidgetType() )
	    return FALSE;
    }
    return TRUE;
}

void QDragger::sendAllChildrenHome( QWidget *w )
{
    const QObjectList *l = w->children();
    if ( !l )
	return;
    QObjectListIt iter( *l );
    QObject *tmp;
    while( (tmp = iter.current()) ) {
	++iter;
	if ( tmp->isWidgetType() ) {
	    sendAllChildrenHome( (QWidget*) tmp );
	    DraggedInfo *di = draggedDict.find( (long) tmp );
	    if ( di )
		sendChildHome( di );
	}
    }
}

bool QDragger::dragEvent( QWidget *w, QMouseEvent *e )
{
    switch( e->type() ) {
	case QEvent::MouseButtonDblClick:
	case QEvent::MouseButtonPress: {
	    if ( !noWidgets( w ) || // has widget children
		 isParentToDragged( w )	|| // has had widget children
		 w->parentWidget() == 0 ) {       // is top level window
		hostWidget = w;
		menu->popup( w->mapToGlobal( e->pos() ) );
		return TRUE;
	    }
	    if ( !draggedDict.find( (long) w ) ) {
		DraggedInfo *tmp = new DraggedInfo;
		tmp->w	      = w;
		tmp->mother      = w->parentWidget();
		tmp->pos	      = w->frameGeometry().topLeft();
		draggedDict.insert( (long) w, tmp );
	    }
	    dragBackground	 = w->backgroundColor();
	    dragForeground	 = w->foregroundColor();
	    dragInfo.w	 = w;
	    dragInfo.mother = w->parentWidget();
	    dragInfo.pos	 = w->frameGeometry().topLeft();
	    clickOffset = e->pos();
	    dragInfo.w  = w;
	    QPoint p    = w->mapToGlobal(QPoint(0,0));
	    w->reparent( 0, WType_Popup, p, TRUE );
		
	    return TRUE;
	}
	case QEvent::MouseButtonRelease:
	case QEvent::MouseMove: {
	    if ( dragInfo.w != 0 ) {
		QPoint p = QCursor::pos() - clickOffset;
		dragInfo.w->move( p );
		if ( e->type() == QEvent::MouseMove )
		    return TRUE;
	    } else {
		return FALSE;
	    }
	    if ( !dragInfo.w )
		return FALSE;
	    if ( w != dragInfo.w )
		w = dragInfo.w;
	    dragInfo.w = 0;
	    w->hide();
	    QPoint pos;
	    QWidget *target = cursorWidget( &pos );
	    pos = pos - clickOffset;
	    QPoint p;
	    if ( !target ) {
		target = openDropWindow( QRect( pos, w->size() ),
					 FALSE);
		p = QPoint( 0, 0 );
	    }
	    else
		p = target->mapFromGlobal( pos );
	    w->reparent( target, 0, p, TRUE );
	    DropInfo *tmp = dropDict.find( (long) dragInfo.mother );
	    if ( tmp ) {
		if ( !tmp->userOpened && noWidgets( tmp->w ) )
		    dropDict.remove( (long) tmp->w );
	    }
	    if ( !target->isVisible() )
		target->show();
	  }
	  return TRUE;
	default:
	  return FALSE;
    }
}

bool QDragger::killDropEvent( QMouseEvent *e )
{
    switch( e->type() ) {
	case QEvent::MouseButtonDblClick:
	case QEvent::MouseButtonPress:
	    clickedWidget = cursorWidget();
	    return TRUE;
	case QEvent::MouseButtonRelease:
	    hostWidget->releaseMouse();
	    if ( clickedWidget ) {
		DropInfo *tmp = dropDict.find( (long) clickedWidget );
		if( tmp ) {
		    killDropWindow( tmp );
		    dropDict.remove( (long) tmp->w );
		}
	    }
	    grabFinished();
	    return TRUE;
	case QEvent::MouseMove:
	    return TRUE;
	default:
	    break;
    }
    return FALSE;
}

bool QDragger::sendChildEvent( QMouseEvent *e )
{
    switch( e->type() ) {
	case QEvent::MouseButtonDblClick:
	case QEvent::MouseButtonPress:
	    clickedWidget = cursorWidget();
	    return TRUE;
	case QEvent::MouseButtonRelease:
	    hostWidget->releaseMouse();
	    if ( clickedWidget ) {
		DraggedInfo *tmp = draggedDict.find((long) clickedWidget);
		if( tmp ) {
		    QWidget *parent = tmp->w->parentWidget();
		    sendChildHome( tmp );
		    DropInfo *dri = dropDict.find( (long) parent );
		    if ( dri && noWidgets(dri->w) && !dri->userOpened ) {
			killDropWindow( dri );
			dropDict.remove( (long) dri );
		    }
		}
		grabFinished();
	    }
	    return TRUE;
	case QEvent::MouseMove:
	    return TRUE;
	default:
	    break;
    }
    return FALSE;
}

bool QDragger::startGrab()
{
    if ( !hostWidget )
	return FALSE;
    clickedWidget = 0;
    cursor	  = hostWidget->cursor();
    hostWidget->grabMouse();
    hostWidget->setCursor( QCursor( CrossCursor ) );
    return TRUE;
}

void QDragger::grabFinished()
{
    killingDrop	 = FALSE;
    sendingChild = FALSE;
    hostWidget->setCursor( cursor );
}

void QDragger::closeDropWindow( DropWindow *w )
{
    DropInfo *tmp = dropDict.find( (long) w);
    if( tmp )
	killDropWindow( tmp );
}

void QDragger::openDropWindow()
{
    QWidget *tmp = openDropWindow( QRect(100, 100, 300, 200), TRUE );
    tmp->show();
}

QWidget *QDragger::openDropWindow( const QRect &r, bool user )
{
    DropInfo *tmp = new DropInfo;
    DropWindow *w = new DropWindow;
    if ( user ) {
	tmp->userOpened = TRUE;
	w->setCaption( "Drop window" );
    } else {
	tmp->userOpened = FALSE;
	w->setCaption( "Auto drop window" );
    }
    tmp->w = w;
    w->master = this;
    w->setGeometry( r );
    dropDict.insert( (long) w, tmp );
    w->show();
    return w;
}

void QDragger::killDropWindow()
{
    if ( startGrab() )
	killingDrop   = TRUE;
}

void QDragger::killDropWindow( DropInfo *di )
{
    const QObjectList *l = di->w->children();
    if ( !l )
	return;
    QObjectListIt iter( *l );
    QObject *tmp;
    while( (tmp = iter.current()) ) {
	++iter;
	if ( tmp->isWidgetType() ) {
	    DraggedInfo *dri = draggedDict.find( (long) tmp );
	    if ( dri ) {
		sendChildHome( dri );
		draggedDict.remove( (long) tmp );
	    }
	}
    }
    di->w->hide();
}

void QDragger::killAllDropWindows()
{
    killAllDropWindows( FALSE );
}

void QDragger::killAllDropWindows( bool autoOnly )
{
    QIntDictIterator<DropInfo> iter( dropDict );

    DropInfo *tmp;
    while( (tmp = iter.current()) ) {
	++iter;
	if( !autoOnly || !tmp->userOpened ) {
	    killDropWindow( tmp );
	    dropDict.remove( (long) tmp->w );
	}
    }
}

void QDragger::sendChildHome( DraggedInfo *i )
{
    i->w->reparent( i->mother, 0, i->pos, TRUE );
}

void QDragger::sendChildHome()
{
    if ( startGrab() )
	sendingChild  = TRUE;
}

void QDragger::sendAllChildrenHome()
{
    QIntDictIterator<DraggedInfo> iter( draggedDict );

    DraggedInfo *tmp;
    while( (tmp = iter.current()) ) {
	++iter;
	sendChildHome( tmp );
	draggedDict.remove( (long) tmp->w );
    }
    killAllDropWindows( TRUE );
    draggedDict.clear();
}


QWidget *cursorWidget( QPoint *p )
{
    QPoint curpos = QCursor::pos();
    if ( p )
	*p = curpos;
    return QApplication::widgetAt( curpos );
}


#include "qdragapp.moc"
