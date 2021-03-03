/****************************************************************************
** $Id: qt/src/widgets/qtoolbar.cpp   2.3.2   edited 2001-05-27 $
**
** Implementation of QToolBar class
**
** Created : 980315
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the widgets module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qtoolbar.h"
#ifndef QT_NO_TOOLBAR

#include "qmainwindow.h"
#include "qpushbutton.h"
#include "qtooltip.h"
#include "qlayout.h"
#include "qframe.h"
#include "qobjectlist.h"
#include "qobjectdict.h"
#include "qpainter.h"
#include "qdrawutil.h"
#include "qapplication.h"
#include "qtoolbutton.h"
#include "qpopupmenu.h"
#include "qtimer.h"

class QArrowWidget : public QWidget
{
    Q_OBJECT

public:
    QArrowWidget( Qt::Orientation o, QWidget *parent ) : QWidget( parent, "qt_arrow_widget" ), orient( o ) {}

protected:
    void paintEvent( QPaintEvent * ) {
	QPainter p( this );
	QPointArray a;
	if ( orient == Horizontal ) {
	    int h = height();
	    a.setPoints( 5,  0, 0,  3, h / 4, 0, h / 2, 3,3 * h / 4, 0, h );
	} else {
	    int w = width();
	    a.setPoints( 5,  0, 0,  w / 4, 3 , w / 2, 0 , 3 * w / 4, 3 , w, 0 );
	}
	p.setPen( colorGroup().light() );
	p.drawPolyline( a );
	if ( orient == Qt::Horizontal )
	    a.translate( 1, 0 );
	else
	    a.translate( 0, 1 );
	p.setPen( colorGroup().midlight() );
	p.drawPolyline( a );
    }

private:
    Qt::Orientation orient;

};

class QToolBarPrivate
{
public:
    QToolBarPrivate() : moving( FALSE ), arrow( 0 ), menu( 0 ), back( 0 ), button( 0 )
    { stretchable[ 0 ] = FALSE; stretchable[ 1 ] = FALSE; hiddenItems.setAutoDelete( FALSE ); }

    bool moving;
    bool stretchable[ 2 ];
    QToolButton *arrow;
    QPopupMenu *menu;
    QArrowWidget *back;
    QIntDict<QButton> hiddenItems;
    QButton *button;

};


class QToolBarSeparator : public QFrame
{
    Q_OBJECT
public:
    QToolBarSeparator( Orientation, QToolBar *parent, const char* name=0 );

    QSize sizeHint() const;
    QSizePolicy sizePolicy() const;
    Orientation orientation() const { return orient; }
public slots:
   void setOrientation( Orientation );
protected:
    void styleChange( QStyle& );
private:
    Orientation orient;
};



QToolBarSeparator::QToolBarSeparator(Orientation o , QToolBar *parent,
				     const char* name )
    :QFrame( parent, name )
{
    connect( parent, SIGNAL(orientationChanged(Orientation)),
	     this, SLOT(setOrientation(Orientation)) );
    setOrientation( o );
    setBackgroundMode( parent->backgroundMode() );
    setBackgroundOrigin( ParentOrigin );
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) );
}



void QToolBarSeparator::setOrientation( Orientation o )
{
    orient = o;
    if ( style() == WindowsStyle ) {
	if ( orientation() == Vertical )
	    setFrameStyle( HLine + Sunken );
	else
	    setFrameStyle( VLine + Sunken );
    } else {
	    setFrameStyle( NoFrame );
    }
}

void QToolBarSeparator::styleChange( QStyle& )
{
    setOrientation( orient );
}

QSize QToolBarSeparator::sizeHint() const
{
    return orientation() == Vertical ? QSize( 0, 6 ) : QSize( 6, 0 );
}

QSizePolicy QToolBarSeparator::sizePolicy() const
{
    //### removeme 3.0
    return QWidget::sizePolicy();
}

#include "qtoolbar.moc"


// NOT REVISED
/*! \class QToolBar qtoolbar.h

  \brief The QToolBar class provides a tool bar.

  \ingroup application

  A toolbar is a panel that contains a set of controls, usually
  represented by small icons.  Its purpose is to provide quick access
  to frequently used commands or options. Within a main window, the
  user can drag toolbars freely around and hide them with a click on
  the toolbar handle.

  To use QToolBar, you simply create a QToolBar as child of a
  QMainWindow, create a number of QToolButton widgets (or other
  widgets) in left to right (or top to bottom) order, call
  addSeparator() when you want a separator, and that's all.

  The application/application.cpp example does precisely this.

  You may use any kind of widget within a toolbar, with QToolButton
  and QComboBox being the two most common ones.

  Each QToolBar lives in a \link QMainWindow dock \endlink in a
  QMainWindow, and can optionally start a new line in its dock.  Tool
  bars that start a new line are always positioned at the left end or
  top of the tool bar dock; others are placed next to the previous
  tool bar and word-wrapped as necessary. The main window can
  be resized to a smaller size than a toolbar would need to show all
  items. If this happens QToolbar shows a little arrow button at the
  right or bottom end. When clicking on that button, a popup menu is
  opened which shows all items of the toolbar which are
  outside the visible area of the mainwindow.

  Usually, a toolbar gets just the space it needs. However, with
  setHorizontalStretchable()/setVerticalStretchable() or
  setStretchableWidget() you can advise the main window to expand
  the toolbar to fill all available width in the specified orientation.

  The tool bar arranges its buttons either horizontally or vertically
  (see setOrientation() for details). Generally, QMainWindow will set
  the orientation correctly for you. The toolbar emits a signal
  orientationChanged() each time the orientation changes, in case some
  child widgets need adjustments.

  To remove all items from a toolbar, you can use the clear() method.

  \sa QToolButton QMainWindow
  <a href="http://www.iarchitect.com/visual.htm">Parts of Isys on Visual Design</a>
  <a href="guibooks.html#fowler">GUI Design Handbook: Tool Bar.</a>
*/


/*!  Constructs an empty tool bar which is a child of \a parent and
  managed by \a parent, initially in \a dock, labelled \a and starting
  a new line in the dock if \a newLine is TRUE.  \a name is the object
  name, as usual.
*/

QToolBar::QToolBar( const QString &label,
		    QMainWindow * parent, QMainWindow::ToolBarDock dock,
		    bool newLine, const char * name )
    : QWidget( parent, name )
{
    mw = parent;
    o = (dock == QMainWindow::Left || dock == QMainWindow::Right )
	? Vertical : Horizontal;
    init();

    if ( parent )
	parent->addToolBar( this, label, dock, newLine );
}


/*!  Constructs an empty horizontal tool bar with a parent of \a
  parent and managed by \a mainWindow.  The \a label and \a newLine
  are passed straight to QMainWindow::addToolBar().  \a name is the
  object name and \a f is the widget flags.

  This is the constructor to use if you want to create torn-off
  toolbars, or toolbars in the status bar.
*/

QToolBar::QToolBar( const QString &label, QMainWindow * mainWindow,
		    QWidget * parent, bool newLine, const char * name,
		    WFlags f )
    : QWidget( parent, name, f )
{
    mw = mainWindow;
    o = Horizontal;
    init();

    if ( mainWindow )
	mainWindow->addToolBar( this, label, QMainWindow::Unmanaged, newLine );
}


/*!  Constructs an empty tool bar in the top dock of its parent,
  without any label and without requiring a newline.  This is mostly
  useless. */

QToolBar::QToolBar( QMainWindow * parent, const char * name )
    : QWidget( parent, name )
{
    mw = parent;
    o = Horizontal;
    init();

    if ( parent )
	parent->addToolBar( this, QString::null, QMainWindow::Top );
}

/*!
  Common initialization code. Requires that \c mw and \c o are set.
  Does not call QMainWindow::addToolBar
*/
void QToolBar::init()
{
    d = new QToolBarPrivate;
    bl = 0;
    sw = 0;

    bl = new QBoxLayout( this, orientation() == Vertical
			? QBoxLayout::Down : QBoxLayout::LeftToRight,
#ifdef _WS_QWS_
			1,
#else
			style() == WindowsStyle ? 2 : 1,
#endif
			0 );
    boxLayout()->setAutoAdd( TRUE );
    if ( !mw || mw->toolBarsMovable() )
	boxLayout()->addSpacing( 9 );

    if ( mw ) {
	connect( mw, SIGNAL( startMovingToolBar( QToolBar * ) ),
		 this, SLOT( startMoving( QToolBar * ) ) );
	connect( mw, SIGNAL( endMovingToolBar( QToolBar * ) ),
		 this, SLOT( endMoving( QToolBar * ) ) );
    }
#if defined(CHECK_NULL)
    else
	qWarning( "QToolBar::QToolBar main window cannot be 0." );
#endif
    setBackgroundMode( PaletteButton);
    setFocusPolicy( NoFocus );
}

QBoxLayout *QToolBar::boxLayout()
{
    if ( !layout() ) {
	bl = new QBoxLayout( this, orientation() == Vertical
			     ? QBoxLayout::Down : QBoxLayout::LeftToRight,
			     style() == WindowsStyle ? 2 : 1, 0 );
	if ( !mw || mw->toolBarsMovable() )
	    boxLayout()->addSpacing( 9 );
	return bl;
    }

    if ( bl == layout() || layout()->inherits( "QBoxLayout" ) ) {
	bl = (QBoxLayout*)layout();
	return (QBoxLayout*)layout();
    }

    return 0;
}

/*! Destructs the object and frees any allocated resources. */

QToolBar::~QToolBar()
{
    d->menu = 0;
    delete d;
    d = 0;
}


/*!  Adds a separator to the end of the toolbar. */

void QToolBar::addSeparator()
{
    (void) new QToolBarSeparator( orientation(), this, "tool bar separator" );
}

/*!\reimp
*/

void QToolBar::styleChange( QStyle& /*old*/ )
{
    QObjectList *childs = queryList( "QWidget" );
    if ( childs ) {
        QObject *ob = 0;
	for ( ob = childs->first(); ob; ob = childs->next() ) {
            if ( ob->inherits( "QToolButton" ) ) {
                QToolButton* w = (QToolButton*)ob;
                w->setStyle( &style() );
            } else if ( ob->inherits( "QToolBarSeparator" ) ) {
                QToolBarSeparator* w = (QToolBarSeparator*)ob;
                w->setStyle( &style() );
            }
        }
    }
    delete childs;
}
    

/*!  Sets this toolbar to organize its content vertically if \a
  newOrientation is \c Vertical and horizontally if \a newOrientation
  is \c Horizontal.

  Emits the orientationChanged() signal.

  \sa orientation()
*/

void QToolBar::setOrientation( Orientation newOrientation )
{
    if ( o != newOrientation ) {
	o = newOrientation;
	if ( d->arrow ) {
	    delete d->arrow;
	    d->arrow = 0;
	}

	if ( d->back ) {
	    delete d->back;
	    d->back = 0;
	}
	boxLayout()->setDirection( o==Horizontal ? QBoxLayout::LeftToRight :
				   QBoxLayout::TopToBottom );
	emit orientationChanged( newOrientation );
    }
}


/*! \fn Orientation QToolBar::orientation() const

  Returns the current orientation of the toolbar.
*/

/*!  \reimp. */

void QToolBar::show()
{
    QWidget::show();
    if ( mw )
	mw->triggerLayout( FALSE );
}


/*!
  \reimp
*/

void QToolBar::hide()
{
    QWidget::hide();
    if ( mw )
	mw->triggerLayout( FALSE );
}

void QToolBar::setUpGM()
{
    //Does nothing, present for binary compatibility
}


/*!
  Paint the handle.  The Motif style is rather close to Netscape
  and even closer to KDE.
*/

void QToolBar::paintEvent( QPaintEvent * )
{
    paintToolBar();
}



/*!  Returns a pointer to the QMainWindow which controls this tool bar.
*/

QMainWindow * QToolBar::mainWindow()
{
    return mw;
}


/*!
  Sets \a w to be expanded if this toolbar is requested to stretch
  (because QMainWindow right-justifies the dock it's in or isVerticalStretchable()
  or isHorizontalStretchable() of this toolbar is TRUE).

  If you call setStretchableWidget() and the toolbar is not stretchable
  yet, setStretchable(  ) is called.

  \sa QMainWindow::setRightJustification(), setVerticalStretchable(), setHorizontalStretchable()
*/

void QToolBar::setStretchableWidget( QWidget * w )
{
    sw = w;
    boxLayout()->setStretchFactor( w, 1 );

    if ( !isHorizontalStretchable() && !isVerticalStretchable() ) {
	if ( orientation() == Horizontal )
	    setHorizontalStretchable( TRUE );
	else
	    setVerticalStretchable( TRUE );
    }
}


/*! \reimp */

bool QToolBar::event( QEvent * e )
{
    bool r =  QWidget::event( e );
    //after the event filters have dealt with it:
    if ( e->type() == QEvent::ChildInserted ) {
	QObject * child = ((QChildEvent*)e)->child();
	if ( isVisible() && child && child->isWidgetType() &&
	     child->parent() == this && !child->inherits( "QPopupMenu" ) )
	    ( (QWidget*)child )->show();
	if ( child && child->isWidgetType() && ((QWidget*)child) == sw )
	    boxLayout()->setStretchFactor( (QWidget*)child, 1 );
    } else if ( e->type() == QEvent::ChildRemoved ) {
	QObject * child = ((QChildEvent*)e)->child();
	if ( child == d->arrow )
	    d->arrow = 0;
	if ( child == d->menu )
	    d->menu = 0;
	if ( child == d->back )
	    d->back = 0;
    }
    return r;
}


/*! \reimp */

bool QToolBar::eventFilter( QObject * obj, QEvent * e )
{
    //Does nothing, present for binary compatibility
    return QWidget::eventFilter( obj, e );
}


/*!  Sets the label of this tool bar to \a label.  The label is not
currently used; it will be used in a forthcoming tool bar
configuration dialog.

\sa label()
*/

void QToolBar::setLabel( const QString & label )
{
    l = label;
}


/*!  Returns the label of this tool bar.

  \sa setLabel()
*/

QString QToolBar::label() const
{
    return l;
}


/*!
  Clears the toolbar, deleting all childwidgets.
 */
void QToolBar::clear()
{
    if ( !children() )
	return;
    QObjectListIt it( *children() );
    QObject * obj;
    while( (obj=it.current()) != 0 ) {
	++it;
	if ( obj->isWidgetType() )
	    delete obj;
    }
    d->menu = 0;
    d->arrow = 0;
    d->back = 0;
    if ( isVisible() )
	updateArrowStuff();
}

/*!
  \internal
*/

void QToolBar::startMoving( QToolBar *tb )
{
    if ( tb == this ) {
	d->moving = TRUE;
	bool du = !isUpdatesEnabled();
	if ( du )
	    setUpdatesEnabled( TRUE );
	repaint( FALSE );
	if ( du )
	    setUpdatesEnabled( FALSE);
    }
}

/*!
  \internal
*/

void QToolBar::endMoving( QToolBar *tb )
{
    if ( tb == this && d->moving ) {
	bool du = !isUpdatesEnabled();
	if ( du )
	    setUpdatesEnabled( TRUE );
	d->moving = FALSE;
	repaint( TRUE );
	if ( du )
	    setUpdatesEnabled( FALSE);
    }
}

/*!
  Sets the toolbar to be horizontally stretchable if \a b is TRUE, or
  non-stretchable otherwise.

  A stretchable toolbar fills the available width in a toolbar dock. A
  non-stretchable toolbar usually gets just the space it needs.

  The default is FALSE.

  \sa QMainWindow::setRightJustification(), isHorizontalStretchable(),
  setVerticalStretchable(), isVerticalStretchable()
*/

void QToolBar::setHorizontalStretchable( bool b )
{
    if ( d->stretchable[ 0 ] != b ) {
	d->stretchable[ 0 ] = b;
	if ( mw )
	    mw->triggerLayout( FALSE );
    }
}

/*!
  Sets the toolbar to be vertically stretchable if \a b is TRUE, or
  non-stretchable otherwise.

  A stretchable toolbar fills the available height in a toolbar dock. A
  non-stretchable toolbar usually gets just the space it needs.

  The default is FALSE.

  \sa QMainWindow::setRightJustification(), isVerticalStretchable(),
  setHorizontalStretchable(), isHorizontalStretchable()
*/

void QToolBar::setVerticalStretchable( bool b )
{
    if ( d->stretchable[ 1 ] != b ) {
	d->stretchable[ 1 ] = b;
	if ( mw )
	    mw->triggerLayout( FALSE );
    }
}

/*!
  Returns whether the toolbar is stretchable horizontally.

  A stretchable toolbar fills all available width in a toolbar dock. A
  non-stretchable toolbar usually gets just the space it needs.

  \sa setHorizontalStretchable(), setVerticalStretchable(), isVerticalStretchable()
*/

bool QToolBar::isHorizontalStretchable() const
{
    return d->stretchable[ 0 ];
}

/*!
  Returns whether the toolbar is stretchable vertically.

  A stretchable toolbar fills all available height in a toolbar dock. A
  non-stretchable toolbar usually gets just the space it needs.

  \sa setHorizontalStretchable(), setVerticalStretchable(), isHorizontalStretchable()
*/

bool QToolBar::isVerticalStretchable() const
{
    return d->stretchable[ 1 ];
}

/*!
  \fn void QToolBar::orientationChanged( Orientation newOrientation )

  This signal is emitted when the toolbar changed its orientation to
  \a newOrientation.
*/

/*!
  \reimp
*/

QSize QToolBar::minimumSize() const
{
    if ( orientation() == Horizontal )
	return QSize( 0, QWidget::minimumSize().height() );
    return QSize( QWidget::minimumSize().width(), 0 );
}

/*!
  \reimp
*/

QSize QToolBar::minimumSizeHint() const
{
    if ( orientation() == Horizontal )
	return QSize( 0, QWidget::minimumSizeHint().height() );
    return QSize( QWidget::minimumSizeHint().width(), 0 );
}

/*!
  \reimp
*/

void QToolBar::resizeEvent( QResizeEvent *e )
{
    QWidget::resizeEvent( e );
    if ( isVisible() )
	updateArrowStuff();
}

void QToolBar::updateArrowStuff()
{
    if ( !isVisible() )
	return;
    if ( orientation() == Horizontal ) {
	int shw = sizeHint().width();
	if ( d->arrow && d->back && d->arrow->isVisible() && d->back->isVisible() )
	    shw -= QMAX( d->arrow->width(), d->back->width() );
	if ( width() < shw ) {
	    setUpdatesEnabled( TRUE );
	    setupArrowMenu();
	    if ( !d->back ) {
		d->back = new QArrowWidget( orientation(), this );
	    }
	    if ( !d->arrow ) {
		d->arrow = new QToolButton( RightArrow, this );
		d->arrow->setAutoRaise( TRUE );
		d->arrow->setAutoRepeat( FALSE );
		d->arrow->setPopupDelay( 0 );
	    }
	    if ( d->menu && d->menu->count() > 0 ) {
		d->back->show();
		d->back->raise();
		d->arrow->show();
		d->arrow->raise();
		d->arrow->setPopup( d->menu );
	    } else if ( d->menu ) {
		d->back->hide();
		d->arrow->hide();
	    }

	    if ( d->back->geometry() != QRect( width() - 20, 1, 20, height() - 2 ) )
		d->back->setGeometry( width() - 20, 1, 20, height() - 2 );
	    if ( d->arrow->geometry() != QRect( width() - 14, 3, 13, height() - 6 ) )
		d->arrow->setGeometry( width() - 14, 3, 13, height() - 6 );
	    paintToolBar();
	    setUpdatesEnabled( FALSE );
	} else {
	    setUpdatesEnabled( TRUE );
	    if ( d->arrow || d->back ) {
		if ( d->back && d->back->isVisible() ) {
		    d->back->hide();
		    if ( layout() )
			layout()->activate();
		}
		if ( d->arrow && d->arrow->isVisible() ) {
		    d->arrow->hide();
		    if ( layout() )
			layout()->activate();
		}
	    }
	    update();
	}
    } else {
	int shh = sizeHint().height();
	if ( d->arrow && d->back && d->arrow->isVisible() && d->back->isVisible() )
	    shh -= QMAX( d->arrow->height(), d->back->height() );
	if ( height() < shh ) {
	    setUpdatesEnabled( TRUE );
	    setupArrowMenu();
	    if ( !d->back ) {
		d->back = new QArrowWidget( orientation(), this );
	    }
	    if ( !d->arrow ) {
		d->arrow = new QToolButton( DownArrow, this );
		d->arrow->setAutoRepeat( FALSE );
		d->arrow->setAutoRaise( TRUE );
		d->arrow->setPopupDelay( 0 );
	    }
	    if ( d->menu && d->menu->count() > 0 ) {
		d->back->show();
		d->back->raise();
		d->arrow->show();
		d->arrow->raise();
		d->arrow->setPopup( d->menu );
	    } else if ( d->menu ) {
		d->back->hide();
		d->arrow->hide();
	    }

	    if ( d->back->geometry() != QRect( 1, height() - 20, width() - 2, 20 ) )
		d->back->setGeometry( 1, height() - 20, width() - 2, 20 );
	    if ( d->arrow->geometry() != QRect( 3, height() - 14, width() - 6, 13 ) )
		d->arrow->setGeometry( 3, height() - 14, width() - 6, 13 );
	    paintToolBar();
	    setUpdatesEnabled( FALSE );
	} else {
	    setUpdatesEnabled( TRUE );
	    if ( d->arrow || d->back ) {
		if ( d->back && d->back->isVisible() ) {
		    d->back->hide();
		    if ( layout() )
			layout()->activate();
		}
		if ( d->arrow && d->arrow->isVisible() ) {
		    d->arrow->hide();
		    if ( layout() )
			layout()->activate();
		}
	    }
	    update();
	}
    }
}

void QToolBar::setupArrowMenu()
{
    if ( !isVisible() )
	return;
    if ( !d->menu ) {
	d->menu = new QPopupMenu( this );
	connect( d->menu, SIGNAL( activated( int ) ),
		 this, SLOT( popupSelected( int ) ) );
	connect( d->menu, SIGNAL( aboutToShow() ),
		 this, SLOT( setupArrowMenu() ) );
    }

    QObjectList *childs = queryList( "QWidget" );
    if ( childs ) {
	d->menu->clear();
	d->menu->setCheckable( TRUE );
	d->hiddenItems.clear();
	bool justHadSep = TRUE;
	QObject *ob = 0;
	for ( ob = childs->first(); ob; ob = childs->next() ) {
	    if ( ob->isWidgetType() && ( (QWidget*)ob )->isVisible() && ob->parent() == this &&
		 ob != d->arrow && ob != d->menu && ob->inherits( "QButton" ) ) {
		QWidget *w = (QWidget*)ob;
		bool mv = FALSE;
		if ( orientation() == Horizontal )
		    mv = ( w->x() + w->width() > width() - 20 ||
			   w->x() == -w->width() && w->y() == -w->height() );
		else
		    mv = ( w->y() + w->height() > height() - 20 ||
			   w->x() == -w->width() && w->y() == -w->height() );
		if ( mv ) {
		    bool hdb = FALSE;
		    if ( orientation() == Horizontal )
			hdb = w->x() > 2 * style().toolBarHandleExtend();
		    else
			hdb = w->y() > 2 * style().toolBarHandleExtend();
		    if ( hdb )
			w->move( -w->width(), -w->height() );
		    if ( w->inherits( "QToolButton" ) ) {
			QToolButton *b = (QToolButton*)w;
			QString s = b->textLabel();
			if ( s.isEmpty() )
			    s = "";
			int id;
			if ( b->popup() && b->popupDelay() == 0 )
			    id = d->menu->insertItem( b->iconSet(), s, b->popup() );
			else
			    id = d->menu->insertItem( b->iconSet(), s );
			d->hiddenItems.insert( id, b );
			if ( b->isToggleButton() )
			    d->menu->setItemChecked( id, b->isOn() );
			if ( !b->isEnabled() )
			    d->menu->setItemEnabled( id, FALSE );
			justHadSep = FALSE;
		    } else if ( w->inherits( "QButton" ) ) {
			QButton *b = (QButton*)w;
			QString s = b->text();
			if ( s.isEmpty() )
			    s = "";
			int id = -1;
			if ( b->pixmap() )
			    id = d->menu->insertItem( *b->pixmap(), s );
			else
			    id = d->menu->insertItem( s );
			d->hiddenItems.insert( id, b );
			if ( b->isToggleButton() )
			    d->menu->setItemChecked( id, b->isOn() );
			if ( !b->isEnabled() )
			    d->menu->setItemEnabled( id, FALSE );
			justHadSep = FALSE;
		    }
		}
	    } else if ( ob->inherits( "QToolBarSeparator" ) ) {
		if ( !justHadSep )
		    d->menu->insertSeparator();
		justHadSep = TRUE;
	    }
	}
    }
    delete childs;
}

void QToolBar::popupSelected( int id )
{
    QButton *b = d->hiddenItems.find( id );
    d->button = b;
    if ( d->button )
	QTimer::singleShot( 0, this, SLOT( emulateButtonClicked() ) );
}

void QToolBar::emulateButtonClicked()
{
    if ( !d->button )
	return;

    if ( d->button->inherits( "QPushButton" ) &&
	 ( (QPushButton*)d->button )->popup() ) {
	( (QPushButton*)d->button )->popup()->exec( QCursor::pos() );
    } else if ( d->button->inherits( "QToolButton" ) &&
	   ( (QToolButton*)d->button )->popup() ) {
	( (QToolButton*)d->button )->popup()->exec( QCursor::pos() );
    } else if ( d->button->isToggleButton() ) {
	d->button->setOn( !d->button->isOn() );
	emit d->button->pressed();
	emit d->button->released();
	emit d->button->clicked();
	if ( d->button->inherits( "QWhatsThisButton" ) )
	    d->button->setOn( FALSE );
    } else {
	emit d->button->pressed();
	emit d->button->released();
	emit d->button->clicked();
    }
    if ( d ) { // we might got deleted, so check to be sure
	d->button = 0;
	QTimer::singleShot( 0, this, SLOT( updateArrowStuff() ) );
    }
}

void QToolBar::paintToolBar()
{
    if ( mw && !mw->toolBarsMovable() )
	return;

    QPainter p( this );
    int w = width();
    int h = height();
    if ( orientation() == Horizontal && w < sizeHint().width() )
	w++;
    else if ( orientation() == Vertical && h < sizeHint().height() )
	h++;
    style().drawPanel( &p, 0, 0, w, h,
 		       colorGroup(), FALSE, 1, 0 );
    style().drawToolBarHandle( &p, QRect( 0, 0, width(), height() ),
 			       orientation(), d->moving, colorGroup() );
}

/* from chaunsee:

1.  Toolbars should contain only high-frequency functions.  Avoid putting
things like About and Exit on a toolbar unless they are frequent functions.

2.  All toolbar buttons must have some keyboard access method (it can be a
menu or shortcut key or a function in a dialog box that can be accessed
through the keyboard).

3.  Make toolbar functions as efficient as possible (the common example is to
Print in Microsoft applications, it doesn't bring up the Print dialog box, it
prints immediately to the default printer).

4.  Avoid turning toolbars into graphical menu bars.  To me, a toolbar should
be efficient. Once you make almost all the items in a toolbar into graphical
pull-down menus, you start to lose efficiency.

5.  Make sure that adjacent icons are distinctive. There are some toolbars
where you see a group of 4-5 icons that represent related functions, but they
are so similar that you can't differentiate among them.  These toolbars are
often a poor attempt at a "common visual language".

6.  Use any de facto standard icons of your platform (for windows use the
cut, copy, and paste icons provided in dev kits rather than designing your
own).

7.  Avoid putting a highly destructive toolbar button (delete database) by a
safe, high-frequency button (Find) -- this will yield 1-0ff errors).

8.  Tooltips in many Microsoft products simply reiterate the menu text even
when that is not explanatory.  Consider making your tooltips slightly more
verbose and explanatory than the corresponding menu item.

9.  Keep the toolbar as stable as possible when you click on different
objects. Consider disabling toolbar buttons if they are used in most, but not
all contexts.

10.  If you have multiple toolbars (like the Microsoft MMC snap-ins have),
put the most stable toolbar to at the left with less stable ones to the
right. This arrangement (stable to less stable) makes the toolbar somewhat
more predictable.

11.  Keep a single toolbar to fewer than 20 items divided into 4-7 groups of
items.
*/
#endif
