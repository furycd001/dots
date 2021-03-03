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

#include "formwindow.h"
#include "defs.h"
#include "mainwindow.h"
#include "widgetfactory.h"
#include "sizehandle.h"
#include "metadatabase.h"
#include "resource.h"
#include "layout.h"
#include "connectioneditorimpl.h"
#include <widgetdatabase.h>
#include "pixmapchooser.h"
#include "orderindicator.h"
#include "hierarchyview.h"

#include <qevent.h>
#include <qpainter.h>
#include <qpen.h>
#include <qlabel.h>
#include <qobjectlist.h>
#include <qtimer.h>
#include <qapplication.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <qstatusbar.h>
#include <qapplication.h>
#include <qpalette.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qsizegrip.h>
#include <qpushbutton.h>
#include <qwhatsthis.h>
#include <qmetaobject.h>
#include <qtooltip.h>

static void setCursorToAll( const QCursor &c, QWidget *start )
{
    start->setCursor( c );
    QObjectList *l = (QObjectList*)start->children();
    if ( l ) {
	for ( QObject *o = l->first(); o; o = l->next() ) {
	    if ( o->isWidgetType() && !o->inherits( "SizeHandle" ) )
		setCursorToAll( c, ( (QWidget*)o ) );
	}
    }
}

static void restoreCursors( QWidget *start, FormWindow *fw )
{
    if ( fw->widgets()->find( start ) )
	start->setCursor( MetaDataBase::cursor( start ) );
    else
	start->setCursor( ArrowCursor );
    QObjectList *l = (QObjectList*)start->children();
    if ( l ) {
	for ( QObject *o = l->first(); o; o = l->next() ) {
	    if ( o->isWidgetType() && !o->inherits( "SizeHandle" ) )
		restoreCursors( ( (QWidget*)o ), fw );
	}
    }
}

#if defined(_WS_WIN32_) // #### needed for the workaround for repaint problem on windows
#include <qt_windows.h>
static void flickerfree_update( QWidget *w )
{
    InvalidateRect( w->winId(), 0, FALSE );
}
#endif

/*!
  \class FormWindow formwindow.h
  \brief Editor window for a form

  The FormWindow is the widget which is used as editor for forms. It
  handles inserting, deleting, moving, resizing, etc. of widgets.

  Normally multiple formwindows are used at the same time in the
  Designer. So each formwindow has its own undo/redo buffer, etc.

  Also the formwindow has some signals to inform e.g. about selection
  changes which is interesting for the PropertyEditor.

  For handling the events of the child widgets (moving, etc.) the
  handleMousePress(), etc. functions are called from the application
  event filter which is implemented in MainWindow::eventFilter().
*/

FormWindow::FormWindow( MainWindow *mw, QWidget *parent, const char *name )
    : QWidget( parent, name, WDestructiveClose ), mainwindow( mw ),
      commands( 100 ), pixInline( TRUE )
{
    init();
}

FormWindow::FormWindow( QWidget *parent, const char *name )
    : QWidget( parent, name, WDestructiveClose ), mainwindow( 0 ),
      commands( 100 ), pixInline( TRUE )
{
    init();
}

void FormWindow::init()
{
    toolFixed = FALSE;
    checkedSelectionsForMove = FALSE;
    mContainer = 0;
    connectSender = connectReceiver = 0;
    currTool = POINTER_TOOL;
    unclippedPainter = 0;
    widgetPressed = FALSE;
    drawRubber = FALSE;
    setFocusPolicy( ClickFocus );
    sizePreviewLabel = 0;
    checkSelectionsTimer = new QTimer( this, "checkSelectionsTimer" );
    connect( checkSelectionsTimer, SIGNAL( timeout() ),
	     this, SLOT( invalidCheckedSelections() ) );
    updatePropertiesTimer = new QTimer( this );
    connect( updatePropertiesTimer, SIGNAL( timeout() ),
	     this, SLOT( updatePropertiesTimerDone() ) );
    showPropertiesTimer = new QTimer( this );
    connect( showPropertiesTimer, SIGNAL( timeout() ),
	     this, SLOT( showPropertiesTimerDone() ) );
    selectionChangedTimer = new QTimer( this );
    connect( selectionChangedTimer, SIGNAL( timeout() ),
	     this, SLOT( selectionChangedTimerDone() ) );
    windowsRepaintWorkaroundTimer = new QTimer( this );
    connect( windowsRepaintWorkaroundTimer, SIGNAL( timeout() ),
	     this, SLOT( windowsRepaintWorkaroundTimerTimeout() ) );
    insertParent = 0;
    connect( &commands, SIGNAL( undoRedoChanged( bool, bool, const QString &, const QString & ) ),
	     this, SIGNAL( undoRedoChanged( bool, bool, const QString &, const QString & ) ) );
    propShowBlocked = FALSE;

    setIcon( PixmapChooser::loadPixmap( "form.xpm", PixmapChooser::Mini ) );

    connect( &commands, SIGNAL( modificationChanged( bool ) ),
	     this, SLOT( modificationChanged( bool ) ) );
    buffer = 0;

    QWidget *w = WidgetFactory::create( WidgetDatabase::idFromClassName( "QFrame" ), this );
    setMainContainer( w );
    propertyWidget = w;
}

void FormWindow::setMainWindow( MainWindow *w )
{
    mainwindow = w;
}


FormWindow::~FormWindow()
{
}

void FormWindow::closeEvent( QCloseEvent *e )
{
    if ( mainwindow->unregisterClient( this ) )
	e->accept();
    else
	e->ignore();
}

void FormWindow::paintGrid( QWidget *w, QPaintEvent *e )
{
    if ( !mainWindow()->showGrid() )
	return;
    int x = 0, y = 0, jmax = 0;
    QPainter p( w );
    p.setClipRegion( e->rect() );
    p.setPen( colorGroup().foreground() );
    jmax = w->height() / mainWindow()->grid().y() + 20;
    int end = w->width() / mainWindow()->grid().x() + 20;
    for ( int i = 0; i < end; ++i ) {
	y = 0;
	for ( int j = 0; j < jmax; ++j ) {
	    p.drawPoint( x, y );
	    y += mainWindow()->grid().y();
	}
	x += mainWindow()->grid().x();
    }
}

/*!  For operations like drawing a rubber band or drawing the rect
  when inserting a new widget, a unclipped painter (which draws also
  on child widgets) is needed. This method does all the initialization.
*/

void FormWindow::beginUnclippedPainter( bool doNot )
{
    endUnclippedPainter();
    bool unclipped = testWFlags( WPaintUnclipped );
    setWFlags( WPaintUnclipped );
    unclippedPainter = new QPainter;
    unclippedPainter->begin( this );
    if ( !unclipped )
	clearWFlags( WPaintUnclipped );
    if ( doNot ) {
	unclippedPainter->setPen( QPen( color0, 2 ) );
	unclippedPainter->setRasterOp( NotROP );
    }
}

/*!
  Gets rid of an open unclipped painter.

  \sa beginUnclippedPainter()
*/

void FormWindow::endUnclippedPainter()
{
    if ( unclippedPainter )
	unclippedPainter->end();
    delete unclippedPainter;
    unclippedPainter = 0;
}

QPoint FormWindow::gridPoint( const QPoint &p )
{
    return QPoint( ( p.x() / grid().x() ) * grid().x(),
		   ( p.y() / grid().y() ) * grid().y() );
}

void FormWindow::drawSizePreview( const QPoint &pos, const QString& text )
{
    unclippedPainter->save();
    unclippedPainter->setPen( QPen( colorGroup().foreground(), 1  ));
    unclippedPainter->setRasterOp( CopyROP );
    if ( !sizePreviewPixmap.isNull() )
	unclippedPainter->drawPixmap( sizePreviewPos, sizePreviewPixmap );
    if ( text.isNull() ) {
	sizePreviewPixmap = QPixmap(); // set null again
	unclippedPainter->restore();
	return;
    }
    QRect r  =  fontMetrics().boundingRect( 0, 0, 0, 0, AlignCenter, text );
    r = QRect( pos + QPoint( 10, 10 ), r.size() + QSize( 5, 5 ) );

    checkPreviewGeometry( r );

    sizePreviewPos = r.topLeft();
    sizePreviewPixmap = QPixmap::grabWindow( winId(), r.x(), r.y(), r.width(), r.height() );
    unclippedPainter->setBrush( QColor( 255, 255, 128 ) );
    unclippedPainter->drawRect( r );
    unclippedPainter->drawText( r, AlignCenter, text );
    unclippedPainter->restore();
}

void FormWindow::insertWidget()
{
    if ( currTool == POINTER_TOOL )
	return;

    bool useSizeHint = !oldRectValid || ( currRect.width() < 2 && currRect.height() < 2 );
    Orientation orient = Horizontal;
    QString n = WidgetDatabase::className( currTool );
    if (  useSizeHint && ( n == "Spacer" || n == "QSlider" || n == "Line" ) ) {
	QPopupMenu m( mainWindow() );
	m.insertItem( tr( "&Horizontal" ) );
	int ver = m.insertItem( tr( "&Vertical" ) );
	int r = m.exec( QCursor::pos() );
	if ( r == ver )
	    orient = Vertical;
    }
	

    QWidget *w = WidgetFactory::create( currTool, insertParent, 0, TRUE, &currRect, orient );
    if ( !w )
	return;
    if ( !savePixmapInline() && currTool == WidgetDatabase::idFromClassName( "PixmapLabel" ) )
	( (QLabel*)w )->setPixmap( PixmapChooser::loadPixmap( "image.xpm" ) );
    int id = WidgetDatabase::idFromClassName( WidgetFactory::classNameOf(w) );
    if ( WidgetDatabase::isCustomWidget( id ) ) {
	QWhatsThis::add( w, tr("<b>A %1 (custom widget)</b> "
			    "<p>Select <b>Edit Custom Widgets...</b> in the <b>Tools->Custom</b> "
			    "menu to add and change the custom widgets. You can add "
			    "properties as well as signals and slots to integrate them into the "
			    "designer, and provide a pixmap which will be used to represent "
			    "the widget on the form.</p>")
			    .arg(WidgetDatabase::toolTip( id )) );
	QToolTip::add( w, tr("A %1 (custom widget)").arg(WidgetDatabase::toolTip( id )) );
    } else {
	QString tt = WidgetDatabase::toolTip( id );
	QString wt = WidgetDatabase::whatsThis( id );
	if ( !wt.isEmpty() && !tt.isEmpty() )
	    QWhatsThis::add( w, QString("<b>A %1</b><p>%2</p>").arg( tt ).arg( wt ) );
    }

    QString s = w->name();
    unify( w, s, TRUE );
    w->setName( s );
    if ( !w )
	return;
    insertWidget( w );
    QRect r( currRect );
    if ( !oldRectValid ||
	 ( currRect.width() < 2 && currRect.height() < 2 ) )
	r = QRect( rectAnchor, QSize( 0, 0 ) );

    QPoint p = r.topLeft();
    p = mapToGlobal( p );
    p = insertParent->mapFromGlobal( p );
    r = QRect( p, r.size() );

    if ( useSizeHint ) {
	r.setWidth( w->sizeHint().width() );
	r.setHeight( w->sizeHint().height() );
    }

    if ( r.width() < 2 * grid().x() )
	r.setWidth( 2 * grid().x() );
    if ( r.height() < 2 * grid().y() )
	r.setHeight( 2 * grid().y() );

    const QObjectList *l = insertParent->children();
    QObjectListIt it( *l );
    QWidgetList lst;
    for ( ; it.current(); ) {
	QObject *o = it.current();
	++it;
	if ( o->isWidgetType() &&
	     ( (QWidget*)o )->isVisibleTo( this ) &&
	     insertedWidgets.find( (QWidget*)o ) && o != w ) {
	    QRect r2( ( (QWidget*)o )->pos(),
		      ( (QWidget*)o )->size() );
	    if ( r.contains( r2 ) )
		lst.append( (QWidget*)o );
	}
    }

    if ( !lst.isEmpty() ) {
	QWidget *pw = WidgetFactory::containerOfWidget( w );
	QValueList<QPoint> op, np;
	for ( QWidget *i = lst.first(); i; i = lst.next() ) {
	    op.append( i->pos() );
	    QPoint pos = pw->mapFromGlobal( i->mapToGlobal( QPoint( 0, 0 ) ) );
	    pos -= r.topLeft();
	    np.append( pos );
	}

	MoveCommand *mv = new MoveCommand( tr( "Reparent Widgets" ), this, lst, op, np, insertParent, pw );

	if ( !toolFixed )
	    mainwindow->resetTool();
	else
	    setCursorToAll( CrossCursor, w );

	InsertCommand *cmd = new InsertCommand( tr( "Insert %1" ).arg( w->name() ), this, w, r );

	QList<Command> commands;
	commands.append( mv );
	commands.append( cmd );
	
	MacroCommand *mc = new MacroCommand( tr( "Insert %1" ).arg( w->name() ), this, commands );
	commandHistory()->addCommand( mc );
	mc->execute();
    } else {
	if ( !toolFixed )
	    mainwindow->resetTool();
	else
	    setCursorToAll( CrossCursor, w );

	InsertCommand *cmd = new InsertCommand( tr( "Insert %1" ).arg( w->name() ), this, w, r );
	commandHistory()->addCommand( cmd );
	cmd->execute();
    }
}

void FormWindow::insertWidget( QWidget *w, bool checkName )
{
    if ( checkName ) {
	QString s = w->name();
	unify( w, s, TRUE );
	w->setName( s );
    }

    MetaDataBase::addEntry( w );
    int id = WidgetDatabase::idFromClassName( WidgetFactory::classNameOf(w) );
    if ( WidgetDatabase::isCustomWidget( id ) ) {
	QWhatsThis::add( w, tr("<b>A %1 (custom widget)</b> "
			    "<p>Select <b>Edit Custom Widgets...</b> in the <b>Tools->Custom</b> "
			    "menu to add and change the custom widgets. You can add "
			    "properties as well as signals and slots to integrate them into the "
			    "designer, and provide a pixmap which will be used to represent "
			    "the widget on the form.</p>")
			    .arg(WidgetDatabase::toolTip( id )) );
	QToolTip::add( w, tr("A %1 (custom widget)").arg(WidgetDatabase::toolTip( id )) );
    } else {
	QString tt = WidgetDatabase::toolTip( id );
	QString wt = WidgetDatabase::whatsThis( id );
	if ( !wt.isEmpty() && !tt.isEmpty() )
	    QWhatsThis::add( w, QString("<b>A %1</b><p>%2</p>").arg( tt ).arg( wt ) );
    }

    restoreCursors( w, this );
    widgets()->insert( w, w );
    w->show();
}

void FormWindow::removeWidget( QWidget *w )
{
    MetaDataBase::removeEntry( w );
    widgets()->take( w );
}

void FormWindow::handleMousePress( QMouseEvent *e, QWidget *w )
{
    checkedSelectionsForMove = FALSE;
    checkSelectionsTimer->stop();
    if ( !sizePreviewLabel ) {
	sizePreviewLabel = new QLabel( this );
	sizePreviewLabel->hide();
	sizePreviewLabel->setBackgroundColor( QColor( 255, 255, 128 ) );
	sizePreviewLabel->setFrameStyle( QFrame::Plain | QFrame::Box );
    }

    switch ( currTool ) {
    case POINTER_TOOL:
	if ( !isMainContainer( w ) ) { // press on a child widget
	    // if the clicked widget is not in a layout, raise it
	    if ( !w->parentWidget() || WidgetFactory::layoutType( w->parentWidget() ) == WidgetFactory::NoLayout )
		w->raise();
	    if ( ( e->state() & ControlButton ) ) { // with control pressed, always start rubber band selection
		drawRubber = TRUE;
		currRect = QRect( 0, 0, -1, -1 );
		startRectDraw( mapFromGlobal( e->globalPos() ), e->globalPos(), this, Rubber );
		break;
	    }
		
	    bool sel = isWidgetSelected( w );
	    if ( !( ( e->state() & ControlButton ) || ( e->state() & ShiftButton ) ) ) { // control not pressed...
		if ( !sel ) { // ...and widget no selectted: unselect all
		    clearSelection( FALSE );
		} else { // ...widget selected
		    // only if widget has a layout (it is a layout meta widget or a laid out container!), unselect its childs
		    if ( WidgetFactory::layoutType( w ) != WidgetFactory::NoLayout ) {
			QObjectList *l = w->queryList( "QWidget" );
			setPropertyShowingBlocked( TRUE );
			for ( QObject *o = l->first(); o; o = l->next() ) {
			    if ( !o->isWidgetType() )
				continue;
			    if ( insertedWidgets.find( (QWidget*)o ) )
				selectWidget( (QWidget*)o, FALSE );
			}
			setPropertyShowingBlocked( FALSE );
			delete l;
		    }
		}
		qApp->processEvents();
	    }
	    if ( ( ( e->state() & ControlButton ) || ( e->state() & ShiftButton ) ) &&
		 sel && e->button() == LeftButton ) { // control pressed and selected, unselect widget
		selectWidget( w, FALSE );
		break;
	    }
		
 	    raiseChildSelections( w ); // raise selections and select widget
	    selectWidget( w );
	
	    // if widget is laid out, find the first non-laid out super-widget
	    QWidget *realWidget = w; // but store the original one
	    while ( w->parentWidget() &&
		    ( WidgetFactory::layoutType( w->parentWidget()) != WidgetFactory::NoLayout || !insertedWidgets.find(w) ) )
		w = w->parentWidget();
	
	    if ( e->button() == LeftButton ) { // left button: store original geometry and more as the widget might start moving
		widgetPressed = TRUE;
		widgetGeom = QRect( w->pos(), w->size() );
		oldPressPos = w->mapFromGlobal( e->globalPos() );
		origPressPos = oldPressPos;
		checkedSelectionsForMove = FALSE;
		moving.clear();
	    } else if ( e->button() == RightButton ) { // RMB menu
		mainwindow->popupWidgetMenu( e->globalPos(), this, realWidget);
	    }
	} else { // press was on the formwindow
	    if ( e->button() == LeftButton ) { // left button: start rubber selection and show formwindow properties
		drawRubber = TRUE;
		if ( !( ( e->state() & ControlButton ) || ( e->state() & ShiftButton ) ) ) {
		    clearSelection( FALSE );
		    QWidget *opw = propertyWidget;
		    propertyWidget = mainContainer();
		    repaintSelection( opw );
		}
		currRect = QRect( 0, 0, -1, -1 );
		startRectDraw( mapFromGlobal( e->globalPos() ), e->globalPos(), this, Rubber );
	    } else if ( e->button() == RightButton ) { // RMB menu
		clearSelection();
		mainwindow->popupFormWindoMenu( e->globalPos(), this );
	    }
	}
	break;
    case CONNECT_TOOL:
	if ( e->button() != LeftButton )
	    break;
	saveBackground();
	mainWindow()->statusBar()->message( tr( "Connect '%1' with..." ).arg( w->name() ) );
	connectStartPos = mapFromGlobal( e->globalPos() );
	currentConnectPos = mapFromGlobal( e->globalPos() );
	connectSender = designerWidget( w );
	connectReceiver = designerWidget( w );
	beginUnclippedPainter( FALSE );
	drawConnectLine();
	break;
    case ORDER_TOOL:
	if ( !isMainContainer( w ) ) { // press on a child widget
	    orderedWidgets.removeRef( w );
	    orderedWidgets.append( w );
	    for ( QWidget *wid = orderedWidgets.last(); wid; wid = orderedWidgets.prev() ) {
		int i = stackedWidgets.findRef( wid );
		if ( i != -1 ) {
		    stackedWidgets.removeRef( wid );
		    stackedWidgets.insert( 0, wid );
		}	
	    }
	    QWidgetList oldl = MetaDataBase::tabOrder( this );
	    TabOrderCommand *cmd = new TabOrderCommand( tr( "Change Tab Order" ), this, oldl, stackedWidgets );
	    cmd->execute();
	    commandHistory()->addCommand( cmd, TRUE );
	    updateOrderIndicators();
	}
	break;
    default: // any insert widget tool
	if ( e->button() == LeftButton ) {
	    insertParent = WidgetFactory::containerOfWidget( mainContainer() ); // default parent for new widget is the formwindow
	    if ( !isMainContainer( w ) ) { // press was not on formwindow, check if we can find another parent
		QWidget *wid = w;
		for (;;) {
		    int id = WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( wid ) );
		    if ( ( WidgetDatabase::isContainer( id ) || wid == mainContainer() ) && !wid->inherits( "QLayoutWidget" ) ) {
			insertParent = WidgetFactory::containerOfWidget( wid ); // found another parent, store it
			break;
		    } else {
			wid = wid->parentWidget();
			if ( !wid )
			    break;
		    }
		}
	    }
	    startRectDraw( w->mapFromGlobal( e->globalPos() ), e->globalPos(), w, Insert );
	}
	break;
    }
}

void FormWindow::handleMouseDblClick( QMouseEvent *, QWidget *w )
{
    switch ( currTool ) {
    case ORDER_TOOL:
	if ( !isMainContainer( w ) ) { // press on a child widget
	    orderedWidgets.clear();
	    orderedWidgets.append( w );
	    for ( QWidget *wid = orderedWidgets.last(); wid; wid = orderedWidgets.prev() ) {
		int i = stackedWidgets.findRef( wid );
		if ( i != -1 ) {
		    stackedWidgets.removeRef( wid );
		    stackedWidgets.insert( 0, wid );
		}	
	    }
	    QWidgetList oldl = MetaDataBase::tabOrder( this );
	    TabOrderCommand *cmd = new TabOrderCommand( tr( "Change Tab Order" ), this, oldl, stackedWidgets );
	    cmd->execute();
	    commandHistory()->addCommand( cmd, TRUE );
	    updateOrderIndicators();
	}
    default:
	break;
    }
}

void FormWindow::handleMouseMove( QMouseEvent *e, QWidget *w )
{
    if ( ( e->state() & LeftButton ) != LeftButton )
	return;

    QWidget *newReceiver = (QWidget*)connectReceiver, *oldReceiver = (QWidget*)connectReceiver, *wid;
    bool drawRecRect;
    switch ( currTool ) {
    case POINTER_TOOL:
	if ( widgetPressed && allowMove( w ) ) { // we are prepated for a move

	    // if widget is laid out, find the first non-laid out super-widget
	    while ( w->parentWidget() &&
		    ( WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout || !insertedWidgets.find(w ) ) )
		w = w->parentWidget();
	
	    // calc correct position
	    QPoint pos = w->mapFromGlobal( e->globalPos() );

#if 0
	    QPoint mpos = w->mapToParent( pos );
	    // check if we are not outside the visible area, else correct coords
	    if ( mpos.x() < 0 )
		pos.setX( w->mapFromParent( QPoint( 0, mpos.y() ) ).x() );
	    if ( mpos.y() < 0 )
		pos.setY( w->mapFromParent( QPoint( mpos.x(), 0 ) ).y() );
	    if ( mpos.x() > w->parentWidget()->width() )
		pos.setX( w->mapFromParent( QPoint( w->parentWidget()->width(), mpos.y() ) ).x() );
	    if ( mpos.y() > w->parentWidget()->height() )
		pos.setY( w->mapFromParent( QPoint( mpos.x(), w->parentWidget()->height() ) ).y() );
#endif
	
	    // calc move distance and store it
	    QPoint d = oldPressPos - pos;
	    if ( QABS( d.x() ) < grid().x() )
		d.setX( 0 );
	    if ( QABS( d.y() ) < grid().y() )
		d.setY( 0 );
	    if ( d.x() == 0 )
		pos.setX( oldPressPos.x() );
	    if ( d.y() == 0 )
		pos.setY( oldPressPos.y() );
	    oldPressPos = pos;
	
	    // snap to grid
	    int x = widgetGeom.x() - d.x();
	    widgetGeom.setX( x );
	    x = ( x / grid().x() ) * grid().x();
	    int y = widgetGeom.y() - d.y();
	    widgetGeom.setY( y );
	    y = ( y / grid().y() ) * grid().y();
	    QPoint p = w->pos();
	
	    if ( x - p.x() != 0 || y - p.y() != 0 ) { // if we actually have to move
		if ( !checkedSelectionsForMove ) { // if not checked yet, check if the correct widget are selected...
		    if ( !isWidgetSelected( w ) ) {	// and unselect others. Only siblings can be moved at the same time
			setPropertyShowingBlocked( TRUE );
			selectWidget( w );
			setPropertyShowingBlocked( FALSE );
		    }
		    checkSelectionsForMove( w );
		}
		
		// finally move the selected widgets and show/update preview lable
		moveSelectedWidgets( x - p.x(), y - p.y() );
		sizePreviewLabel->setText( tr( "%1/%2" ).arg( w->pos().x() ).arg( w->pos().y() ) );
		sizePreviewLabel->adjustSize();
		QRect lg( mapFromGlobal( e->globalPos() ) + QPoint( 16, 16 ), sizePreviewLabel->size() );
		checkPreviewGeometry( lg );
		sizePreviewLabel->setGeometry( lg );
		sizePreviewLabel->raise();
		sizePreviewLabel->show();
#if defined(_WS_WIN32_)
		windowsRepaintWorkaroundTimer->start( 100, TRUE );
#endif
	    } else { // if we don't need to move, do some indication
		QRect lg( mapFromGlobal( e->globalPos() ) + QPoint( 16, 16 ), sizePreviewLabel->size() );
		checkPreviewGeometry( lg );
		sizePreviewLabel->move( lg.x(), lg.y() );
	    }
		
	    oldPressPos += ( p - w->pos() );
	} else if ( drawRubber ) { // draw rubber if we are in rubber-selection mode
	    continueRectDraw( mapFromGlobal( e->globalPos() ), e->globalPos(), this, Rubber );
	}
	break;
    case CONNECT_TOOL:
	restoreConnectionLine();
	wid = qApp->widgetAt( e->globalPos(), TRUE );
	if ( wid )
	    wid = designerWidget( wid );
	if ( wid && ( isMainContainer( wid ) || insertedWidgets.find( wid ) ) && wid->isVisibleTo( this ) )
	    newReceiver = wid;
	if ( newReceiver &&
	     ( newReceiver->inherits( "QLayoutWidget" ) || newReceiver->inherits( "Spacer" ) ) )
	     newReceiver = (QWidget*)connectReceiver;
	drawRecRect = newReceiver != connectReceiver;
	currentConnectPos = mapFromGlobal( e->globalPos() );
	if ( newReceiver &&
	     ( isMainContainer( newReceiver ) || insertedWidgets.find( newReceiver ) ) )
	    connectReceiver = newReceiver;
	mainWindow()->statusBar()->message( tr( "Connect '%1' with '%2'" ).arg( connectSender->name() ).
					    arg( connectReceiver->name() ) );
	qApp->processEvents();
	if ( drawRecRect )
	    restoreRect( QRect( mapToForm(  ( (QWidget*)oldReceiver )->parentWidget(), ( (QWidget*)oldReceiver )->pos() ),
				( (QWidget*)oldReceiver )->size() ) );
	drawConnectLine();
	break;
    case ORDER_TOOL:
	break;
    default: // we are in an insert-widget tool
	if ( insertParent ) // draw insert rect
	    continueRectDraw( w->mapFromGlobal( e->globalPos() ), e->globalPos(), w, Insert );
	break;
    }
}

void FormWindow::handleMouseRelease( QMouseEvent *e, QWidget *w )
{
    if ( e->button() != LeftButton )
	return;

    switch ( currTool ) {
    case POINTER_TOOL:
	if ( widgetPressed && allowMove( w ) ) { // we moved the widget
	    sizePreviewLabel->hide();

	    if ( moving.isEmpty() || w->pos() == *moving.find( (ulong)w ) )
		break;
	    // tell property editor to update
	    if ( propertyWidget && !isMainContainer( propertyWidget ) )
		emitUpdateProperties( propertyWidget );
	
	    QWidget *oldParent = ( (QWidget*)moving.begin().key() )->parentWidget();
	    QWidget *newParent = oldParent;
	    // check whether we have to reparent the selection
	    QWidget* wa = containerAt( e->globalPos(), ( (QWidget*)moving.begin().key() ) );
	    if ( wa ) {
		wa = WidgetFactory::containerOfWidget( wa );
		// ok, looks like we moved onto a container
		
		// check whether we really have different parents.
		if ( wa == ( (QWidget*)moving.begin().key() )->parentWidget() )
		    goto make_move_command;
		
		// break layout if necessary
		if ( WidgetFactory::layoutType( wa ) != WidgetFactory::NoLayout ) {
		    if ( QMessageBox::information( mainWindow(), tr( "Inserting a Widget" ),
						   tr( "You tried to insert a widget into the laid out Container Widget '%1'.\n"
						       "This is not possible. In order to insert the widget, the layout of '%1'\n"
						       "has to be broken. Break the layout or cancel the operation?" ).
						   arg( wa->name() ).
						   arg( wa->name() ), tr( "&Break Layout" ), tr( "&Cancel" ) ) )
			goto make_move_command; // cancel
		    breakLayout( wa );
		}
		
		// doesn't need to be a command, the MoveCommand does reparenting too
		bool emitSelChanged = FALSE;
		for ( QMap<ulong, QPoint>::Iterator it = moving.begin(); it != moving.end(); ++it ) {
		    QWidget *i = (QWidget*)it.key();
		    if ( !emitSelChanged && i->inherits( "QButton" )  ) {
			if ( i->parentWidget() && i->parentWidget()->inherits( "QButtonGroup" ) ||
			     wa->inherits( "QButtonGroup" ) )
			    emitSelChanged = TRUE;
			if ( !wa->inherits( "QButtonGroup" ) ) {
			    MetaDataBase::setPropertyChanged( i, "buttonGroupId", FALSE );
			    if ( i->parentWidget() && i->parentWidget()->inherits( "QButtonGroup" ) )
				( (QButtonGroup*)i->parentWidget() )->remove( (QButton*)i );
			}
		    }
		    QPoint pos = wa->mapFromGlobal( i->mapToGlobal( QPoint(0,0) ) );
		    i->reparent( wa, pos, TRUE );
		    raiseSelection( i );
		    raiseChildSelections( i );
		    widgetChanged( i );
		    mainWindow()->objectHierarchy()->widgetRemoved( i );
		    mainWindow()->objectHierarchy()->widgetInserted( i );
		}
		if ( emitSelChanged ) {
		    emit showProperties( wa );
		    emit showProperties( propertyWidget );
		}
		newParent = wa;
	    }
	
	make_move_command:
	    QWidgetList widgets; // collect the widgets and its old and new positions which have been moved
	    QValueList<QPoint> oldPos, newPos;
	    for ( QMap<ulong, QPoint>::Iterator it = moving.begin(); it != moving.end(); ++it ) {
		widgets.append( (QWidget*)it.key() );
		oldPos.append( *it );
		newPos.append( ( (QWidget*)it.key() )->pos() );
	    }
	    // add move command, don't execute it, this is just a summary of the operations we did during the move-event handling
	    commandHistory()->addCommand( new MoveCommand( tr( "Move" ),
							   this, widgets,
							   oldPos, newPos, oldParent, newParent ) );
	} else if ( drawRubber ) { // we were drawing a rubber selection
	    endRectDraw(); // get rid of the rectangle
	    blockSignals( TRUE );
	    selectWidgets(); // select widgets which intersect the rect
	    blockSignals( FALSE );
	    emitSelectionChanged(); // inform about selection changes
	    if ( propertyWidget )
		emitShowProperties( propertyWidget );
	}
	break;
    case CONNECT_TOOL:
	restoreConnectionLine();
	if ( connectSender )
	    restoreRect( QRect( mapToForm(  ( (QWidget*)connectSender )->parentWidget(),
					    ( (QWidget*)connectSender )->pos() ),
				( (QWidget*)connectSender )->size() ) );
	if ( connectReceiver )
	    restoreRect( QRect( mapToForm( ( (QWidget*)connectReceiver )->parentWidget(),
					   ( (QWidget*)connectReceiver )->pos() ),
				( (QWidget*)connectReceiver )->size() ) );
	endUnclippedPainter();
	qApp->processEvents();
	if ( connectSender && connectReceiver )
	    editConnections();
	break;
    case ORDER_TOOL:
	break;
    default: // any insert widget tool is active
	if ( insertParent ) { // we should insert the new widget now
	    endRectDraw();
	    if ( WidgetFactory::layoutType( insertParent ) != WidgetFactory::NoLayout ) {
		if ( QMessageBox::information( mainWindow(), tr( "Inserting a Widget" ),
					       tr( "You tried to insert a widget into the laid out Container Widget '%1'.\n"
						   "This is not possible. In order to insert the widget, the layout of '%1'\n"
						   "has to be broken. Break the layout or cancel the operation?" ).
					       arg( insertParent->name() ).
					       arg( insertParent->name() ), tr( "&Break Layout" ), tr( "&Cancel" ) ) == 0 ) {
		    breakLayout( insertParent );
		} else {
		    if ( !toolFixed )
			mainWindow()->resetTool();
		    break;
		}
	    }
	    insertWidget(); // so do it
	}
	break;
    }
    widgetPressed = FALSE;
    drawRubber = FALSE;
    insertParent = 0;
    delete buffer;
    buffer = 0;
}

void FormWindow::handleKeyPress( QKeyEvent *e, QWidget *w )
{
    e->ignore();
    checkSelectionsTimer->stop();
    if ( !checkedSelectionsForMove &&
	 ( e->key() == Key_Left ||
	   e->key() == Key_Right ||
	   e->key() == Key_Up ||
	   e->key() == Key_Down ) )
	checkSelectionsForMove( propertyWidget );
    checkSelectionsTimer->start( 1000, TRUE );
    if ( e->key() == Key_Left || e->key() == Key_Right ||
	 e->key() == Key_Up || e->key() == Key_Down ) {
	QWidgetList widgets;
	QValueList<QPoint> oldPos, newPos;
	for ( WidgetSelection *s = selections.first(); s; s = selections.next() ) {
	    if ( s->isUsed() ) {
		int dx = 0, dy = 0;
		bool control = e->state() & ControlButton;
		
		switch ( e->key() ) {
		case Key_Left: {
		    e->accept();
		    if ( control )
			dx = -1;
		    else
			dx = -grid().x();
		} break;
		case Key_Right: {
		    e->accept();
		    if ( control )
			dx = 1;
		    else
			dx = grid().x();
		} break;
		case Key_Up: {
		    e->accept();
		    if ( control )
			dy = -1;
		    else
			dy = -grid().y();
		} break;
		case Key_Down: {
		    e->accept();
		    if ( control )
			dy = 1;
		    else
			dy = grid().y();
		} break;
		default:
		    break;
		}
		
		widgets.append( s->widget() );
		oldPos.append( s->widget()->pos() );
		newPos.append( s->widget()->pos() + QPoint( dx, dy ) );
	    }
	}
	if ( !widgets.isEmpty() ) {
	    MoveCommand *cmd = new MoveCommand( tr( "Move" ), this,
						widgets, oldPos, newPos, 0, 0 );
	    commandHistory()->addCommand( cmd, TRUE );
	    cmd->execute();
	}
    }
    if ( !e->isAccepted() ) {
	QObjectList *l = queryList( "QWidget" );
	if ( !l )
	    return;
	if ( l->find( w ) != -1 )
	    e->accept();
	delete l;
    }

}

void FormWindow::handleKeyRelease( QKeyEvent *e, QWidget * )
{
    e->ignore();
}

void FormWindow::selectWidget( QWidget *w, bool select )
{
    if ( isMainContainer( w ) ) {
	QWidget *opw = propertyWidget;
	propertyWidget = mainContainer();
	repaintSelection( opw );
	emitShowProperties( propertyWidget );
	return;
    }

    if ( select ) {
	QWidget *opw = propertyWidget;
	propertyWidget = w;
	repaintSelection( opw );
	if ( !isPropertyShowingBlocked() )
	    emitShowProperties( propertyWidget );
	WidgetSelection *s = usedSelections.find( w );
	if ( s ) {
	    s->show();
	    return;
	}

	for ( WidgetSelection *s2 = selections.first(); s2; s2 = selections.next() ) {
	    if ( !s2->isUsed() ) {
		s = s2;
	    }
	}

	if ( !s ) {
	    s = new WidgetSelection( this, &usedSelections );
	    selections.append( s );
	}

	s->setWidget( w );
	emitSelectionChanged();
    } else {
	WidgetSelection *s = usedSelections.find( w );
	if ( s )
	    s->setWidget( 0 );
	QWidget *opw = propertyWidget;
	if ( !usedSelections.isEmpty() )
	    propertyWidget = QPtrDictIterator<WidgetSelection>( usedSelections ).current()->widget();
	else
	    propertyWidget = mainContainer();
	repaintSelection( opw );
	if ( !isPropertyShowingBlocked() )
	    emitShowProperties( propertyWidget );
	emitSelectionChanged();
    }
}

QPoint FormWindow::grid() const
{
    if ( !mainWindow()->snapGrid() )
	return QPoint( 1, 1 );
    return mainWindow()->grid();
}

void FormWindow::updateSelection( QWidget *w )
{
    WidgetSelection *s = usedSelections.find( w );
    if ( !w->isVisibleTo( this ) )
	selectWidget( w, FALSE );
    else if ( s )
	s->updateGeometry();
}

void FormWindow::raiseSelection( QWidget *w )
{
    WidgetSelection *s = usedSelections.find( w );
    if ( s )
	s->show();
}

void FormWindow::repaintSelection( QWidget *w )
{
    WidgetSelection *s = usedSelections.find( w );
    if ( s )
	s->update();
}

void FormWindow::clearSelection( bool changePropertyDisplay )
{
    QPtrDictIterator<WidgetSelection> it( usedSelections );
    for ( ; it.current(); ++it )
	it.current()->setWidget( 0, FALSE );

    usedSelections.clear();
    if ( changePropertyDisplay ) {
	QWidget *opw = propertyWidget;
	propertyWidget = mainContainer();
	repaintSelection( opw );
	emitShowProperties( propertyWidget );
    }
    emitSelectionChanged();
}

void FormWindow::startRectDraw( const QPoint &p, const QPoint &global, QWidget *, RectType t )
{
    QPoint pos( p );
    pos = mapFromGlobal( global );
    oldRectValid = FALSE;
    beginUnclippedPainter( TRUE );
    if ( t == Rubber )
	unclippedPainter->setPen( QPen( color0, 1 ) );
    if ( t == Insert )
	rectAnchor = gridPoint( pos );
    else if ( t == Rubber )
	rectAnchor = pos;
    currRect = QRect( rectAnchor, QPoint( 0, 0 ) );
    if ( t == Insert )
	drawSizePreview( pos, tr("Use Size Hint") );
}

void FormWindow::continueRectDraw( const QPoint &p, const QPoint &global, QWidget *, RectType t )
{
    QPoint pos =p;
    pos = mapFromGlobal( global );
    QPoint p2;
    if ( t == Insert )
	p2 = gridPoint( pos );
    else if ( t == Rubber )
	p2 = pos;
    QRect r( rectAnchor, p2 );
    r = r.normalize();

    if ( currRect == r ) {
	QString t = tr( "%1/%2" );
	t = t.arg( r.width() - 1 ).arg( r.height() - 1 );
	drawSizePreview( pos, t );
	return;
    }

    if ( oldRectValid )
	unclippedPainter->drawRect( currRect );
    if ( r.width() > 1 || r.height() > 1 ) {
	oldRectValid = TRUE;
	currRect = r;
	if ( t == Insert ) {
	    QString t = tr( "%1/%2" );
	    t = t.arg( r.width() - 1 ).arg( r.height() - 1 );
	    drawSizePreview( pos, t );
	}
	unclippedPainter->setClipRegion( QRegion( rect() ).subtract( QRect( sizePreviewPos, sizePreviewPixmap.size() ) ) );
	unclippedPainter->drawRect( currRect );
	unclippedPainter->setClipping( FALSE );
    } else {
	oldRectValid = FALSE;
	if ( t == Insert )
	    drawSizePreview( pos, tr("Use Size Hint") );
    }
}

void FormWindow::endRectDraw()
{
    if ( !unclippedPainter )
	return;

    if ( oldRectValid )
	unclippedPainter->drawRect( currRect );
    drawSizePreview( QPoint(-1,-1), QString::null );
    endUnclippedPainter();
}

void FormWindow::selectWidgets()
{
    QObjectList *l = mainContainer()->queryList( "QWidget" );
    if ( l ) {
	for ( QObject *o = l->first(); o; o = l->next() ) {
	    if ( ( (QWidget*)o )->isVisibleTo( this ) &&
		 insertedWidgets[ (void*)o ] ) {
		QPoint p = ( (QWidget*)o )->mapToGlobal( QPoint(0,0) );
		p = mapFromGlobal( p );
		QRect r( p, ( (QWidget*)o )->size() );
		if ( r.intersects( currRect ) && !r.contains( currRect ) )
		    selectWidget( (QWidget*)o );
	    }
	}
    }
    delete l;
    emitSelectionChanged();
}

bool FormWindow::isWidgetSelected( QWidget *w )
{
    return usedSelections.find( w ) != 0;
}

void FormWindow::moveSelectedWidgets( int dx, int dy )
{
    QPtrDictIterator<WidgetSelection> it( usedSelections );
    for ( ; it.current(); ++it ) {
	WidgetSelection *s = it.current();
	QWidget *w = s->widget();
	if ( w->parentWidget() && WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout )
	    continue;
	w->move( w->x() + dx, w->y() + dy );
	s->updateGeometry();
	updateChildSelections( w );
    }
}

CommandHistory *FormWindow::commandHistory()
{
    return &commands;
}

void FormWindow::undo()
{
    commandHistory()->undo();
}

void FormWindow::redo()
{
    commandHistory()->redo();
}

void FormWindow::raiseChildSelections( QWidget *w )
{
    QObjectList *l = w->queryList( "QWidget" );
    if ( !l )
	return;
    if ( !l->first() ) {
	delete l;
	return;
    }

    QPtrDictIterator<WidgetSelection> it( usedSelections );
    for ( ; it.current(); ++it ) {
	if ( l->findRef( it.current()->widget() ) != -1 )
	    it.current()->show();
    }
}

void FormWindow::updateChildSelections( QWidget *w )
{
    QObjectList *l = w->queryList( "QWidget" );
    if ( l ) {
	for ( QObject *o = l->first(); o; o = l->next() ) {
	    if ( o->isWidgetType() &&
		 insertedWidgets.find( (QWidget*)o ) )
		updateSelection( (QWidget*)o );
	}
	delete l;
    }
}

void FormWindow::checkSelectionsForMove( QWidget *w )
{
    checkedSelectionsForMove = TRUE;

    QObjectList *l = w->parentWidget()->queryList( "QWidget", 0, FALSE, FALSE );
    moving.clear();
    if ( l ) {
	QPtrDictIterator<WidgetSelection> it( usedSelections );
	WidgetSelection *sel;
	while ( ( sel = it.current() ) != 0 ) {
	    if ( it.current()->widget() == mainContainer() )
		continue;
	    ++it;
	    if ( l->find( sel->widget() ) == -1 ) {
 		if ( WidgetFactory::layoutType( w ) == WidgetFactory::NoLayout )
		    sel->setWidget( 0 );
	    } else {
		if ( WidgetFactory::layoutType( sel->widget()->parentWidget() ) == WidgetFactory::NoLayout ) {
		    moving.insert( (ulong)sel->widget(), sel->widget()->pos() );
		    sel->widget()->raise();
		    raiseChildSelections( sel->widget() );
		    raiseSelection( sel->widget() );
		}
	    }
	}
	delete l;
    }
}

void FormWindow::deleteWidgets()
{
    QWidgetList widgets;
    QPtrDictIterator<WidgetSelection> it( usedSelections );
    for ( ; it.current(); ++it )
	widgets.append( it.current()->widget() );

    DeleteCommand *cmd = new DeleteCommand( tr( "Delete" ), this, widgets );
    commandHistory()->addCommand( cmd );
    cmd->execute();
}

void FormWindow::editAdjustSize()
{
    QList<Command> commands;
    QWidgetList widgets = selectedWidgets();
    if ( widgets.isEmpty() ) {
	QRect oldr = geometry();
	mainContainer()->adjustSize();
	resize( mainContainer()->size() );
	QRect nr = geometry();
	if ( oldr != nr ) {
	    ResizeCommand *cmd = new ResizeCommand( tr( "Adjust Size" ), this, this, oldr, nr );
	    commandHistory()->addCommand( cmd );
	}
	return;
    }
    for ( QWidget* w = widgets.first(); w; w = widgets.next() ) {
	if ( w->parentWidget() && WidgetFactory::layoutType( w->parentWidget() ) != WidgetFactory::NoLayout )
	    continue;
	QRect oldr = w->geometry();
	w->adjustSize();
	QRect nr = w->geometry();
	if ( oldr != nr )
	    commands.append( new ResizeCommand( tr("Adjust Size"), this, w, oldr, nr ) );
    }

    if ( commands.isEmpty() )
	return;
    for ( WidgetSelection *s = selections.first(); s; s = selections.next() )
	s->updateGeometry();

    MacroCommand *cmd = new MacroCommand( tr( "Adjust Size" ), this, commands );
    commandHistory()->addCommand( cmd );
}


QWidgetList FormWindow::selectedWidgets() const
{
    QWidgetList widgets;
    QPtrDictIterator<WidgetSelection> it( usedSelections );
    for ( ; it.current(); ++it )
	widgets.append( it.current()->widget() );
    return widgets;
}

void FormWindow::widgetChanged( QWidget *w )
{
    updateSelection( w );
}

QLabel *FormWindow::sizePreview() const
{
    if ( !sizePreviewLabel ) {
	( (FormWindow*)this )->sizePreviewLabel = new QLabel( (FormWindow*)this );
	( (FormWindow*)this )->sizePreviewLabel->hide();
	( (FormWindow*)this )->sizePreviewLabel->setBackgroundColor( QColor( 255, 255, 128 ) );
	( (FormWindow*)this )->sizePreviewLabel->setFrameStyle( QFrame::Plain | QFrame::Box );
    }
    return sizePreviewLabel;
}

void FormWindow::invalidCheckedSelections()
{
    checkedSelectionsForMove = FALSE;
}

void FormWindow::checkPreviewGeometry( QRect &r )
{
    if ( !rect().contains( r ) ) {
	if ( r.left() < rect().left() )
	    r.moveTopLeft( QPoint( 0, r.top() ) );
	if ( r.right() > rect().right()  )
	    r.moveBottomRight( QPoint( rect().right(), r.bottom() ) );
	if ( r.top() < rect().top() )
	    r.moveTopLeft( QPoint( r.left(), rect().top() ) );
	if ( r.bottom() > rect().bottom()  )
	    r.moveBottomRight( QPoint( r.right(), rect().bottom() ) );
    }
}

void FormWindow::focusInEvent( QFocusEvent * )
{
}

void FormWindow::focusOutEvent( QFocusEvent * )
{
    if ( propertyWidget && !isMainContainer( propertyWidget ) && !isWidgetSelected( propertyWidget ) ) {
	QWidget *opw = propertyWidget;
	propertyWidget = mainContainer();
	repaintSelection( opw );
    }
}

void FormWindow::resizeEvent( QResizeEvent *e )
{
    QWidget::resizeEvent( e );
    if ( currTool == ORDER_TOOL )
	repositionOrderIndicators();

#if defined(_WS_WIN32_)
    windowsRepaintWorkaroundTimer->start( 100, TRUE );
#endif
}

void FormWindow::windowsRepaintWorkaroundTimerTimeout()
{
#if defined(_WS_WIN32_)
    QObjectList *l = queryList( "QWidget" );
    for ( QObject *o = l->first(); o; o = l->next() ) {
	flickerfree_update( (QWidget*)o );
    }
    flickerfree_update( this );
    delete l;
#endif
}

QPtrDict<QWidget> *FormWindow::widgets()
{
    return &insertedWidgets;
}

QWidget *FormWindow::designerWidget( QObject *o ) const
{
    if ( !o || !o->isWidgetType() )
	return 0;
    QWidget *w = (QWidget*)o;
    while ( w && !isMainContainer( w ) && !insertedWidgets[ (void*)w ] )
	w = (QWidget*)w->parent();
    return w;
}

void FormWindow::emitShowProperties( QWidget *w )
{
    if ( w ) {
	QWidget *opw = propertyWidget;
	propertyWidget = w;
	repaintSelection( opw );
    }
    showPropertiesTimer->stop();
    showPropertiesTimer->start( 0, TRUE );
}

void FormWindow::emitUpdateProperties( QWidget *w )
{
    if ( w == propertyWidget ) {
	updatePropertiesTimer->stop();
	updatePropertiesTimer->start( 0, TRUE );
    }
}

void FormWindow::emitSelectionChanged()
{
    selectionChangedTimer->stop();
    selectionChangedTimer->start( 0, TRUE );
}

void FormWindow::updatePropertiesTimerDone()
{
    if ( propertyWidget )
	emit updateProperties( propertyWidget );
}

void FormWindow::showPropertiesTimerDone()
{
    if ( propertyWidget )
	emit showProperties( propertyWidget );
}

void FormWindow::selectionChangedTimerDone()
{
    emit selectionChanged();
}

void FormWindow::currentToolChanged()
{
    toolFixed = FALSE;
    int t = mainwindow->currentTool();
    if ( currTool == t )
	return;

    // tool cleanup
    switch ( currTool ) {
    case ORDER_TOOL:
	hideOrderIndicators();
	break;
    case CONNECT_TOOL:
	restoreConnectionLine();
	if ( connectSender )
	    restoreRect( QRect( mapToForm(  ( (QWidget*)connectSender )->parentWidget(),
					    ( (QWidget*)connectSender )->pos() ),
				( (QWidget*)connectSender )->size() ) );
	if ( connectReceiver )
	    restoreRect( QRect( mapToForm( ( (QWidget*)connectReceiver )->parentWidget(),
					   ( (QWidget*)connectReceiver )->pos() ),
				( (QWidget*)connectReceiver )->size() ) );
	endUnclippedPainter();
	break;
    case POINTER_TOOL:
	break;
    default:
	if ( insertParent )
	    endRectDraw();
	break;
    }

    connectSender = connectReceiver = 0;
    widgetPressed = FALSE;
    drawRubber = FALSE;
    insertParent = 0;
    delete buffer;
    buffer = 0;

    currTool = t;

    if ( hasFocus() )
	clearSelection( FALSE );

    mainWindow()->statusBar()->clear();

    // tool setup
    switch ( currTool ) {
    case POINTER_TOOL:
	if ( propertyWidget && !isMainContainer( propertyWidget ) && !isWidgetSelected( propertyWidget ) )
	    emitShowProperties( mainContainer() );
	restoreCursors( this, this );
	break;
    case ORDER_TOOL:
	mainWindow()->statusBar()->message( tr( "Click widgets to change tab order...") );
	orderedWidgets.clear();
	showOrderIndicators();
	if ( mainWindow()->formWindow() == this )
	    emitShowProperties( mainContainer() );
	setCursorToAll( ArrowCursor, this );
	break;
    case CONNECT_TOOL:
	mainWindow()->statusBar()->message( tr( "Drag a line to create a connection...") );
	setCursorToAll( CrossCursor, this );
	if ( mainWindow()->formWindow() == this )
	    emitShowProperties( mainContainer() );
	break;
    default:
	mainWindow()->statusBar()->message( tr( "Click on the form to insert a %1..." ).arg( WidgetDatabase::toolTip( currTool ).lower() ) );
	setCursorToAll( CrossCursor, this );
	if ( mainWindow()->formWindow() == this )
	    emitShowProperties( mainContainer() );
	break;
    }
}

void FormWindow::showOrderIndicators()
{
    hideOrderIndicators();
    orderIndicators.setAutoDelete( TRUE );
    QObjectList *l = mainContainer()->queryList( "QWidget" );
    stackedWidgets = MetaDataBase::tabOrder( this );
    if ( l ) {
	int order = 1;
	for ( QObject *o = l->first(); o; o = l->next() ) {
	    QWidget* w = (QWidget*) o;
	    if ( w->isVisibleTo( w->parentWidget() ) &&
		 insertedWidgets[ (void*)w ]  &&
		 w->focusPolicy() != NoFocus ) {
		OrderIndicator* ind = new OrderIndicator( order++, w, this );
		orderIndicators.append( ind );
		if ( stackedWidgets.findRef( w ) == -1 )
		    stackedWidgets.append( w );
	    }
	}
    	delete l;
    }
    updateOrderIndicators();
}

void FormWindow::hideOrderIndicators()
{
    orderIndicators.clear();
}

void FormWindow::updateOrderIndicators()
{
    int order = 1;
    for ( QWidget *w = stackedWidgets.first(); w; w = stackedWidgets.next() ) {
	for ( OrderIndicator* i = orderIndicators.first(); i; i = orderIndicators.next() )
	    i->setOrder( order, w );
	order++;
    }
}

void FormWindow::repositionOrderIndicators()
{
    for ( OrderIndicator* i = orderIndicators.first(); i; i = orderIndicators.next() )
	i->reposition();
}


void FormWindow::updateUndoInfo()
{
    commandHistory()->emitUndoRedo();
}

MainWindow *FormWindow::mainWindow() const
{
    return mainwindow;
}

void FormWindow::save( const QString &filename )
{
    mainWindow()->statusBar()->message( tr( "Saving file %1..." ).arg(filename) );
    QStringList missingCustomWidgets;
    QPtrDictIterator<QWidget> it( insertedWidgets );
    for ( ; it.current(); ++it ) {
	if ( it.current()->isA( "CustomWidget" ) ) {
	    QString className = WidgetFactory::classNameOf( it.current() );
	    if ( !MetaDataBase::hasCustomWidget( className ) )
		missingCustomWidgets << className;
	}
    }

    if ( !missingCustomWidgets.isEmpty() ) {
	QString txt = tr( "Following custom widgets are used in '%1',\n"
			  "but they are not known to the designer:" ).arg( name() );
	for ( QStringList::Iterator sit = missingCustomWidgets.begin(); sit != missingCustomWidgets.end(); ++sit )
	    txt += "   " + *sit + "\n";
	txt += "If you save this form and generate code for it by the UIC, \n"
	       "the generated code will not compile. Do you really want to save\n"
	       "this form now?";
	if ( QMessageBox::information( mainWindow(), tr( "Save Form" ), txt ) == 1 )
	    return;
    }

    fname = filename;
    Resource resource( mainWindow() );
    resource.setWidget( this );
    if ( !resource.save( fname ) ) {
	mainWindow()->statusBar()->message( tr( "Failed to save file %1.").arg( filename ), 5000 );
	QMessageBox::warning( mainWindow(), tr( "Save" ), tr( "Couldn't save file %1" ).arg( fname ) );
    } else {
	mainWindow()->statusBar()->message( tr( "%1 saved.").arg( filename ), 3000 );
	commandHistory()->setModified( FALSE );
    }
}

void FormWindow::setPropertyShowingBlocked( bool b )
{
    propShowBlocked = b;
}

bool FormWindow::isPropertyShowingBlocked() const
{
    return propShowBlocked;
}

int FormWindow::numSelectedWidgets() const
{
    return usedSelections.count();
}

QString FormWindow::copy()
{
    Resource resource( mainWindow() );
    resource.setWidget( this );
    return resource.copy();
}

void FormWindow::lowerWidgets()
{
    QWidgetList widgets;
    QPtrDictIterator<WidgetSelection> it( usedSelections );
    for ( ; it.current(); ++it )
	widgets.append( it.current()->widget() );

    LowerCommand *cmd = new LowerCommand( tr( "Lower" ), this, widgets );
    cmd->execute();
    commandHistory()->addCommand( cmd );					
}

static void find_accel( const QString &txt, QMap<QChar, QWidgetList > &accels, QWidget *w )
{
    int i = txt.find( "&" );
    if ( i == -1 )
	return;
    QChar c = txt[ i + 1 ];
    if ( c.isNull() || c == '&' )
	return;
    c = c.lower();
    QMap<QChar, QWidgetList >::Iterator it = accels.find( c );
    if ( it == accels.end() ) {
	QWidgetList wl;
	wl.append( w );
	accels.insert( c, wl );
    } else {
	QWidgetList *wl = &*it;;
	wl->append( w );
    }
}

void FormWindow::checkAccels()
{
    QMap<QChar, QWidgetList > accels;
    QObjectList *l = mainContainer()->queryList( "QWidget" );
    if ( l ) {
	for ( QObject *o = l->first(); o; o = l->next() ) {
	    if ( ( (QWidget*)o )->isVisibleTo( this ) &&
		 insertedWidgets[ (void*)o ] ) {
		QWidget *w = (QWidget*)o;
		const QMetaProperty* text = w->metaObject()->property( "text", TRUE );
		const QMetaProperty* title = w->metaObject()->property( "title", TRUE );
		const QMetaProperty* pageTitle = w->metaObject()->property( "pageTitle", TRUE );
		if ( text )
		    find_accel( w->property( "text" ).toString(), accels, w );
		if ( title )
		    find_accel( w->property( "title" ).toString(), accels, w );
		if ( pageTitle )
		    find_accel( w->property( "pageTitle" ).toString(), accels, w );
	    }
	}
    }
    delete l;

    bool ok = TRUE;
    QWidget *wid;
    for ( QMap<QChar, QWidgetList >::Iterator it = accels.begin(); it != accels.end(); ++it ) {
	if ( (*it).count() > 1 ) {
	    ok = FALSE;
	    switch ( QMessageBox::information( mainWindow(), tr( "Check Accelerators" ),
					       tr( "The accelerator '%1' is used %2 times. What do you "
						   "want to do?" ).arg( it.key() ).arg( (*it).count() ),
					       tr( "&Select widgets using it" ),
					       tr( "&Ignore" ),
					       tr( "&Cancel" ), 0, 2 ) ) {
	    case 0: // select
		clearSelection( FALSE );
		for ( wid = (*it).first(); wid; wid = (*it).next() )
		    selectWidget( wid, TRUE );
		return;
	    case 1: // ignore
		break;
	    case 2: // cancel
		return;
	    }
	}
    }

    if ( ok )
	QMessageBox::information( mainWindow(), tr( "Check Accelerators" ),
				  tr( "No accelerator is used more that once!" ) );
}

void FormWindow::raiseWidgets()
{
    QWidgetList widgets;
    QPtrDictIterator<WidgetSelection> it( usedSelections );
    for ( ; it.current(); ++it )
	widgets.append( it.current()->widget() );

    RaiseCommand *cmd = new RaiseCommand( tr( "Raise" ), this, widgets );
    cmd->execute();
    commandHistory()->addCommand( cmd );					
}

void FormWindow::paste( const QString &cb, QWidget *parent )
{
    Resource resource( mainWindow() );
    resource.setWidget( this );
    resource.paste( cb, parent );
}

void FormWindow::selectAll()
{
    checkedSelectionsForMove = FALSE;
    blockSignals( TRUE );
    QObjectList *l = mainContainer()->queryList( "QWidget" );
    if ( l ) {
	for ( QObject *o = l->first(); o; o = l->next() ) {
	    if ( ( (QWidget*)o )->isVisibleTo( this ) &&
		 insertedWidgets[ (void*)o ] ) {
		selectWidget( (QWidget*)o );
	    }
	}
    }
    delete l;
    blockSignals( FALSE );
    emitSelectionChanged();
    if ( propertyWidget )
	emitShowProperties( propertyWidget );
    emitSelectionChanged();
}

void FormWindow::layoutHorizontal()
{
    QWidgetList widgets( selectedWidgets() );
    LayoutHorizontalCommand *cmd = new LayoutHorizontalCommand( tr( "Layout horizontally" ),
								this, mainContainer(), 0, widgets );
    clearSelection( FALSE );
    commandHistory()->addCommand( cmd );
    cmd->execute();
}

void FormWindow::layoutVertical()
{
    QWidgetList widgets( selectedWidgets() );
    LayoutVerticalCommand *cmd = new LayoutVerticalCommand( tr( "Layout vertically" ),
							    this, mainContainer(), 0, widgets );
    clearSelection( FALSE );
    commandHistory()->addCommand( cmd );
    cmd->execute();
}

void FormWindow::layoutGrid()
{
    int xres = grid().x();
    int yres = grid().y();

    QWidgetList widgets( selectedWidgets() );
    LayoutGridCommand *cmd = new LayoutGridCommand( tr( "Layout in a grid" ),
						    this, mainContainer(), 0, widgets, xres, yres );
    clearSelection( FALSE );
    commandHistory()->addCommand( cmd );
    cmd->execute();
}

void FormWindow::layoutHorizontalContainer( QWidget *w )
{
    if ( w == this )
	w = mainContainer();
    QObjectList *l = (QObjectList*)WidgetFactory::containerOfWidget(w)->children();
    if ( !l )
	return;
    QWidgetList widgets;
    for ( QObject *o = l->first(); o; o = l->next() ) {
	if ( o->isWidgetType() &&
	     ( (QWidget*)o )->isVisibleTo( this ) &&
	     insertedWidgets.find( (QWidget*)o ) )
	    widgets.append( (QWidget*)o );
    }
    LayoutHorizontalCommand *cmd = new LayoutHorizontalCommand( tr( "Layout children horizontally" ),
								this, mainContainer(), w, widgets );
    clearSelection( FALSE );
    commandHistory()->addCommand( cmd );
    cmd->execute();
}

void FormWindow::layoutVerticalContainer( QWidget *w )
{
    if ( w == this )
	w = mainContainer();
    QObjectList *l = (QObjectList*)WidgetFactory::containerOfWidget(w)->children();
    if ( !l )
	return;
    QWidgetList widgets;
    for ( QObject *o = l->first(); o; o = l->next() ) {
	if ( o->isWidgetType() &&
	     ( (QWidget*)o )->isVisibleTo( this ) &&
	     insertedWidgets.find( (QWidget*)o ) )
	    widgets.append( (QWidget*)o );
    }
    LayoutVerticalCommand *cmd = new LayoutVerticalCommand( tr( "Layout children vertically" ),
							    this, mainContainer(), w, widgets );
    clearSelection( FALSE );
    commandHistory()->addCommand( cmd );
    cmd->execute();
}

void FormWindow::layoutGridContainer( QWidget *w )
{
    if ( w == this )
	w = mainContainer();
    int xres = grid().x();
    int yres = grid().y();

    QObjectList *l = (QObjectList*)WidgetFactory::containerOfWidget(w)->children();
    if ( !l )
	return;
    QWidgetList widgets;
    for ( QObject *o = l->first(); o; o = l->next() ) {
	if ( o->isWidgetType() &&
	     ( (QWidget*)o )->isVisibleTo( this ) &&
	     insertedWidgets.find( (QWidget*)o ) )
	    widgets.append( (QWidget*)o );
    }
    LayoutGridCommand *cmd = new LayoutGridCommand( tr( "Layout children in a grid" ),
						    this, mainContainer(), w, widgets, xres, yres );
    clearSelection( FALSE );
    commandHistory()->addCommand( cmd );
    cmd->execute();
}

void FormWindow::breakLayout( QWidget *w )
{
    if ( w == this )
	w = mainContainer();
    w = WidgetFactory::containerOfWidget( w );
    QList<Command> commands;

    for (;;) {
	if ( !w || w == this )
	    break;
	if ( WidgetFactory::layoutType( w ) != WidgetFactory::NoLayout ) {
	    Command *cmd = breakLayoutCommand( w );
	    if ( cmd )
		commands.insert( 0, cmd );
	    if ( !w->inherits( "QLayoutWidget" ) )
		break;
	}
	w = w->parentWidget();
    }

    if ( commands.isEmpty() )
	return;

    clearSelection( FALSE );
    MacroCommand *cmd = new MacroCommand( tr( "Break Layout" ), this, commands );
    commandHistory()->addCommand( cmd );
    cmd->execute();
}

BreakLayoutCommand *FormWindow::breakLayoutCommand( QWidget *w )
{
    QObjectList *l = (QObjectList*)w->children();
    if ( !l )
	return 0;

    QWidgetList widgets;
    for ( QObject *o = l->first(); o; o = l->next() ) {
	if ( o->isWidgetType() &&
	     ( (QWidget*)o )->isVisibleTo( this ) &&
	     insertedWidgets.find( (QWidget*)o ) )
	    widgets.append( (QWidget*)o );
    }
    return new BreakLayoutCommand( tr( "Break Layout" ), this, WidgetFactory::widgetOfContainer( w ), widgets );
}

int FormWindow::numVisibleWidgets() const
{
    QPtrDictIterator<QWidget> it( insertedWidgets );
    int visible = 0;
    for ( ; it.current(); ++it ) {
	if ( it.current()->isVisibleTo( (FormWindow*)this ) )
	    visible++;
    }
    return visible;
}

bool FormWindow::hasInsertedChildren( QWidget *w ) const
{
    if ( !w )
	return FALSE;
    w = WidgetFactory::containerOfWidget( w );
    if ( !w )
	return FALSE;
    QObjectList *l = w->queryList( "QWidget" );
    if ( !l || !l->first() ) {
	delete l;
	return FALSE;
    }

    for ( QObject *o = l->first(); o; o = l->next() ) {
	if ( o->isWidgetType() &&
	     ( (QWidget*)o )->isVisibleTo( (FormWindow*)this ) &&
	     insertedWidgets.find( (QWidget*)o ) ) {
	    delete l;
	    return TRUE;
	}
    }
    delete l;
    return FALSE;
}

bool FormWindow::allowMove( QWidget *w )
{
    w = w->parentWidget();
    while ( w ) {
	if ( ( isMainContainer( w ) || insertedWidgets.find( w ) ) && WidgetFactory::layoutType( w ) == WidgetFactory::NoLayout )
	    return TRUE;
	w = w->parentWidget();
    }
    return FALSE;
}


void FormWindow::editConnections()
{
    buffer = 0;
    if ( !connectSender || !connectReceiver )
	return;
    mainWindow()->statusBar()->clear();
    QWidgetList  selection = selectedWidgets();
    ConnectionEditor editor( mainwindow, connectSender, connectReceiver, this );
    mainWindow()->statusBar()->message( tr( "Edit connections...") );
    editor.exec();
    mainWindow()->statusBar()->clear();
    if ( !toolFixed )
	mainwindow->resetTool();
    connectSender = connectReceiver = 0;
}

void FormWindow::saveBackground()
{
    if ( buffer )
	delete buffer;
    buffer = new QPixmap( width(), height() );
    *buffer = QPixmap::grabWindow( winId() );
}

void FormWindow::restoreConnectionLine()
{
    if (!unclippedPainter)
	return;

    int a =QABS( connectStartPos.x() - currentConnectPos.x() );
    int b = QABS( connectStartPos.y() - currentConnectPos.y() );
    QRect r( connectStartPos, currentConnectPos );

    if ( a < 32 || b < 32 ) { // special case: vertical or horizontal line
	r = r.normalize();
	unclippedPainter->drawPixmap( r.x() - 2, r.y() - 2, *buffer,
				      r.x() - 2, r.y() - 2, r.width() + 4, r.height() + 4 );
	return;
    }

    if ( a <= 0 )
	a = 1;
    if ( b <= 0 )
	b = 1;
    int w, h;
    if ( b > a ) {
	h = 64;
	w = ( a * h ) / b;
    } else {
	w = 64;
	h = ( b * w ) / a;
    }

    int dx = 2 * w / 3;
    int dy = 2 * h / 3;
    QPoint p( connectStartPos );
	
    if ( r.x() > r.right() ) {
	dx = dx * -1;
	p.setX( p.x() - 64 );
	r.moveBy( -64, 0 );
    }
    if ( r.y() > r.bottom() ) {
	dy = dy * -1;
	p.setY( p.y() - 64 );
	r.moveBy( 0, -64 );
    }
	
    w = h = 64;
    r = r.normalize();
    while ( r.contains( p ) ) {
	unclippedPainter->drawPixmap( p, *buffer, QRect( p, QSize( w, h ) ) );
	unclippedPainter->setPen( red );
	p.setX( p.x() + dx );
	p.setY( p.y() + dy );
    }

    unclippedPainter->drawPixmap( connectStartPos.x() - 10, connectStartPos.y() - 10, *buffer,
				  connectStartPos.x() - 10, connectStartPos.y() - 10, 20, 20 );
}

void FormWindow::restoreRect( const QRect &rect )
{
    if (!unclippedPainter)
	return;

    QRect r( rect );
    r = r.normalize();

    r = QRect( r.x() + 2, r.y() + 2, r.width() - 4, r.height() - 4 );

    unclippedPainter->drawPixmap( r.x() - 2, r.y() - 2, *buffer, r.x() - 2, r.y() - 2, r.width() + 4, 4 );
    unclippedPainter->drawPixmap( r.x() - 2, r.y() - 2, *buffer, r.x() - 2, r.y() - 2, 4, r.height() + 4 );
    unclippedPainter->drawPixmap( r.x() - 2, r.y() + r.height() - 3, *buffer, r.x() - 2, r.y() + r.height() - 3, r.width() + 4, 5 );
    unclippedPainter->drawPixmap( r.x() + r.width() - 2, r.y(), *buffer, r.x() + r.width() - 2, r.y(), 4, r.height() + 4 );
}

void FormWindow::drawConnectLine()
{
    if ( !unclippedPainter )
 	return;

    unclippedPainter->setPen( QPen( white, 2 ) );
    unclippedPainter->drawLine( connectStartPos, currentConnectPos );
    unclippedPainter->setPen( QPen( darkCyan, 1 ) );
    unclippedPainter->drawLine( connectStartPos, currentConnectPos );

    unclippedPainter->setPen( QPen( magenta, 1 ) );
    if ( connectSender ) {
 	QWidget *w = (QWidget*)connectSender;
	QPoint p = mapToForm( w, QPoint(0,0) );
	unclippedPainter->drawRect( QRect( p + QPoint( 2, 2 ), w->size() - QSize( 4, 4 ) ) );
     }
    if ( connectReceiver ) {
 	QWidget *w = (QWidget*)connectReceiver;
	QPoint p = mapToForm( w, QPoint(0,0) );
 	unclippedPainter->drawRect( QRect( p + QPoint( 2, 2 ), w->size() - QSize( 4, 4 ) ) );
     }
}

QString FormWindow::fileName() const
{
    return filename;
}

void FormWindow::setFileName( const QString &fn )
{
    filename = fn;
    emit fileNameChanged( filename, this );
}

void FormWindow::modificationChanged( bool m )
{
    emit modificationChanged( m, this );
}

bool FormWindow::unify( QWidget *w, QString &s, bool changeIt )
{
    bool found = !isMainContainer( w ) && qstrcmp( name(), s.latin1() ) == 0;
    if ( !found ) {
	QString orig = s;
	int num  = 1;
	QPtrDictIterator<QWidget> it( insertedWidgets );
	for ( ; it.current(); ++it ) {
	    if ( it.current() != w &&
		 qstrcmp( it.current()->name(), s.latin1() ) == 0 ) {
		found = TRUE;
		if ( !changeIt )
		    break;
		s = orig + "_" + QString::number( ++num );
		it.toFirst();
	    }
	}
    }

    if ( !found )
	return TRUE;
    return FALSE;
}

bool FormWindow::isCustomWidgetUsed( MetaDataBase::CustomWidget *w )
{
    QPtrDictIterator<QWidget> it( insertedWidgets );
    for ( ; it.current(); ++it ) {
	if ( it.current()->isA( "CustomWidget" ) ) {
	    if ( qstrcmp( WidgetFactory::classNameOf( it.current() ), w->className.utf8() ) == 0 )
		return TRUE;
	}
    }

    return FALSE;
}

void FormWindow::visibilityChanged()
{
    if ( currTool != ORDER_TOOL ) {
	emitUpdateProperties( currentWidget() );
    } else {
	updateOrderIndicators();
	repositionOrderIndicators();
    }
}


/*!
  Maps \a pos in \a w's coordinates to the form's coordinate system.

  This is the equivalent to mapFromGlobal(w->mapToGlobal(pos) ) but
  avoids the two roundtrips to the X-Server on Unix/X11.
 */
QPoint FormWindow::mapToForm( const QWidget* w, const QPoint&  pos ) const
{
    QPoint p = pos;
    const QWidget* i = w;
    while ( i && !i->isTopLevel() && !isMainContainer( (QWidget*)i ) ) {
	p = i->mapToParent( p );
	i = i->parentWidget();
    }
    return mapFromGlobal( w->mapToGlobal( pos ) );
}

static int widgetDepth( QWidget *w )
{
    int d = -1;
    while ( w && !w->isTopLevel() ) {
	d++;
	w = w->parentWidget();
    }

    return d;
}

static bool isChildOf( QWidget *c, QWidget *p )
{
    while ( c && !c->isTopLevel() ) {
	if ( c == p )
	    return TRUE;
	c = c->parentWidget();
    }
    return FALSE;
}

QWidget *FormWindow::containerAt( const QPoint &pos, QWidget *notParentOf )
{
    QPtrDictIterator<QWidget> it( insertedWidgets );
    QWidget *container = 0;
    int depth = -1;
    QWidgetList selected = selectedWidgets();
    if ( rect().contains( mapFromGlobal( pos ) ) ) {
	container = mainContainer();
	depth = widgetDepth( container );
    }

    for ( ; it.current(); ++it ) {
	if ( it.current()->inherits( "QLayoutWidget" ) )
	    continue;
	if ( !it.current()->isVisibleTo( this ) )
	    continue;
	if ( selected.find( it.current() ) != -1 )
	    continue;
	if ( !WidgetDatabase::isContainer( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( it.current() ) ) ) &&
	     it.current() != mainContainer() )
	    continue;
	if ( !it.current()->rect().contains( it.current()->mapFromGlobal( pos ) ) )
	    continue;

	int wd = widgetDepth( it.current() );
	if ( wd == depth && container ) {
	    if ( ( (QObjectList*)it.current()->parentWidget()->children() )->find( it.current() ) >
		 ( (QObjectList*)container->parentWidget()->children() )->find( container ) )
		wd++;
	}
	if ( wd > depth && !isChildOf( it.current(), notParentOf ) ) {
	    depth = wd;
	    container = it.current();
	}
    }

    return container;
}

bool FormWindow::isMainContainer( QWidget *w ) const
{
    return w == (QWidget*)this || w == mainContainer();
}

void FormWindow::setMainContainer( QWidget *w )
{
    bool resetPropertyWidget = isMainContainer( propertyWidget );
    if ( mContainer )
	insertedWidgets.remove( mContainer );
    delete mContainer;
    mContainer = w;
    insertedWidgets.insert( mContainer, mContainer );
    delete layout();
    QHBoxLayout *l = new QHBoxLayout( this );
    l->addWidget( w );
    if ( resetPropertyWidget ) {
	QWidget *opw = propertyWidget;
	propertyWidget = mContainer;
	repaintSelection( opw );
    }
}

bool FormWindow::savePixmapInline() const
{
    return pixInline;
}

QString FormWindow::pixmapLoaderFunction() const
{
    return pixLoader;
}

void FormWindow::setSavePixmapInline( bool b )
{
    pixInline = b;
}

void FormWindow::setPixmapLoaderFunction( const QString &func )
{
    pixLoader = func;
}
