/****************************************************************************
** $Id: qt/src/workspace/qworkspace.cpp   2.3.2   edited 2001-10-17 $
**
** Implementation of the QWorkspace class
**
** Created : 931107
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the workspace module of the Qt GUI Toolkit.
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
** Licensees holding valid Qt Enterprise Edition licenses may use this
** file in accordance with the Qt Commercial License Agreement provided
** with the Software.
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

#include "qworkspace.h"
#ifndef QT_NO_WORKSPACE
#include "qapplication.h"
#include "qobjectlist.h"
#include "qlayout.h"
#include "qtoolbutton.h"
#include "qlabel.h"
#include "qvbox.h"
#include "qaccel.h"
#include "qpopupmenu.h"
#include "qmenubar.h"
#include "qguardedptr.h"
#include "qtooltip.h"
#include "qwmatrix.h"
#include "qpainter.h"
#include "qimage.h"
#include "qdatetime.h"
#include "qguardedptr.h"
#include "qfocusdata.h"

// REVISED: arnt

/*!
  \class QWorkspace qworkspace.h
  \brief The QWorkspace widget provides a workspace window that can
  contain decorated windows, e.g. for MDI.

  \module workspace

  \ingroup application
  \ingroup organizers

  An MDI application has one main window with a menu bar.  The central
  widget of this window is a workspace.  The workspace itself contains
  zero, one or more document windows, each of which displays a
  document.

  The workspace itself is an ordinary Qt widget.  It has a standard
  constructor that takes a parent widget and an object name.  The
  parent window is usually a QMainWindow, but it need not be.

  Document windows (alias MDI windows) are also ordinary Qt widgets,
  that have the workspace as parent widget.  When you call show(),
  hide(), showMaximized(), setCaption(), etc on a document window, it
  is shown, hidden etc. with a frame, caption, icon and icon text,
  just as you'd expect. You can provide widget flags which will be
  used for the layout of the decoration or the behaviour of the widget
  itself.
  To change the geometry of the MDI windows it is necessary to make
  the necessary function calls to the parentWidget() of the widget, as
  this will move or resize the decorated window.

  A document window becomes active when it gets the keyboard focus.
  You can activate it using setFocus(), and the user can activate it
  by moving focus in the normal ways.  The workspace emits a signal
  windowActivated() when it detects the activation change, and the
  function activeWindow() always returns a pointer to the active
  document window.

  The convenience function windowList() returns a list of all document
  windows.  This is useful to create a popup menu "&Windows" on the
  fly, for example.

  QWorkspace provides two built-in layout strategies for child
  windows, cascade() and tile().  Both are slots, so you can easily
  connect menu entries to them.

  In case the toplevel window contains a menu bar and a document
  window is maximized, QWorkspace moves the document window's
  minimize, restore and close buttons from the document window's frame
  to the workspace window's menu bar, and inserts a window operations
  menu on the extreme left of the menu bar.

  \warning User interface research indicates that most users have
  problems with MDI applications.  Use this class cautiously.
*/


#if defined(_WS_WIN_)
#include "qt_windows.h"

extern Qt::WindowsVersion qt_winver;
extern QRgb qt_colorref2qrgb(COLORREF);

const bool win32 = TRUE;
#define TITLEBAR_SEPARATION 1
#define BUTTON_WIDTH 16
#define BUTTON_HEIGHT 14
#define RANGE 16

static const char * const close_xpm[] = {
"12 12 2 1",
"# c #000000",
". c None",
"............",
"............",
"..##....##..",
"...##..##...",
"....####....",
".....##.....",
"....####....",
"...##..##...",
"..##....##..",
"............",
"............",
"............"};

static const char * const maximize_xpm[]={
"12 12 2 1",
"# c #000000",
". c None",
"............",
"............",
".##########.",
".##########.",
".#........#.",
".#........#.",
".#........#.",
".#........#.",
".#........#.",
".##########.",
"............",
"............"};


static const char * const minimize_xpm[] = {
"12 12 2 1",
"# c #000000",
". c None",
"............",
"............",
"............",
"............",
"............",
"............",
"............",
"...######...",
"...######...",
"............",
"............",
"............"};

static const char * const normalize_xpm[] = {
"12 12 2 1",
"# c #000000",
". c None",
"............",
"....######..",
"....######..",
"....#....#..",
"..######.#..",
"..######.#..",
"..#....###..",
"..#....#....",
"..#....#....",
"..######....",
"............",
"............"};

static const char * const normalizeup_xpm[] = {
"12 12 2 1",
"# c #000000",
". c None",
"............",
"....######..",
"....######..",
"....#....#..",
"..######.#..",
"..######.#..",
"..#....###..",
"..#....#....",
"..#....#....",
"..######....",
"............",
"............"};


static const char * const shade_xpm[] = {
"12 12 2 1",
"# c #000000",
". c None",
"............",
"............",
"............",
"............",
"............",
".....#......",
"....###.....",
"...#####....",
"..#######...",
"............",
"............",
"............"};

static const char * const unshade_xpm[] = {
"12 12 2 1",
"# c #000000",
". c None",
"............",
"............",
"............",
"............",
"..#######...",
"...#####....",
"....###.....",
".....#......",
"............",
"............",
"............",
"............"};



#else // !_WS_WIN_

const bool win32 = FALSE;
#define TITLEBAR_SEPARATION 1
#define BUTTON_WIDTH 16
#define BUTTON_HEIGHT 14
#define RANGE 16

static const char * const close_xpm[] = {
"12 12 2 1",
"       s None  c None",
".      c black",
"            ",
"            ",
"   .    .   ",
"  ...  ...  ",
"   ......   ",
"    ....    ",
"    ....    ",
"   ......   ",
"  ...  ...  ",
"   .    .   ",
"            ",
"            "};

static const char * const maximize_xpm[] = {
"12 12 2 1",
"       s None  c None",
".      c black",
"            ",
"            ",
"            ",
"     .      ",
"    ...     ",
"   .....    ",
"  .......   ",
" .........  ",
"            ",
"            ",
"            ",
"            "};

static const char * const minimize_xpm[] = {
"12 12 2 1",
"       s None  c None",
".      c black",
"            ",
"            ",
"            ",
"            ",
" .........  ",
"  .......   ",
"   .....    ",
"    ...     ",
"     .      ",
"            ",
"            ",
"            "};

static const char * const normalize_xpm[] = {
"12 12 2 1",
"       s None  c None",
".      c black",
"            ",
"            ",
"  .         ",
"  ..        ",
"  ...       ",
"  ....      ",
"  .....     ",
"  ......    ",
"  .......   ",
"            ",
"            ",
"            "};

static const char * const normalizeup_xpm[] = {
"12 12 2 1",
"       s None  c None",
".      c black",
"            ",
"            ",
"            ",
"  .......   ",
"   ......   ",
"    .....   ",
"     ....   ",
"      ...   ",
"       ..   ",
"        .   ",
"            ",
"            "};

static const char * const shade_xpm[] = {
"12 12 2 1", "# c #000000",
". c None",
"............",
"............",
".#########..",
".#########..",
"............",
"............",
"............",
"............",
"............",
"............",
"............",
"............"};


static const char * const unshade_xpm[] = {
"12 12 2 1",
"# c #000000",
". c None",
"............",
"............",
".#########..",
".#########..",
".#.......#..",
".#.......#..",
".#.......#..",
".#.......#..",
".#.......#..",
".#########..",
"............",
"............"};


#endif // !_WS_WIN_



static bool resizeHorizontalDirectionFixed = FALSE;
static bool resizeVerticalDirectionFixed = FALSE;
static bool inCaptionChange = FALSE;

class QWorkspaceChildTitleButton : public QLabel
{
    Q_OBJECT
public:
    QWorkspaceChildTitleButton( QWidget* parent );

    QSize sizeHint() const;

public slots:
    void setPixmap( const QPixmap& );
};

class QWorkspaceChildTitleBar;

class QWorkspaceChildTitleLabel : public QFrame
{
    Q_OBJECT
public:
    QWorkspaceChildTitleLabel( QWorkspaceChildTitleBar* parent = 0, const char* name = 0 );

    void setActive( bool act );
    QColor leftColor() const { return leftc; }
    QColor rightColor() const { return rightc; }

    void setLeftMargin( int x );
    void setRightMargin( int x );

public slots:
    void setText( const QString& );

protected:
    void paintEvent( QPaintEvent* );
    void resizeEvent( QResizeEvent* );

    bool event( QEvent* );

    void drawLabel();
    void frameChanged();

private:
    void cutText();
    void getColors();

    QPixmap buffer;
    QColor leftc, aleftc, ileftc;
    QColor rightc, arightc, irightc;
    QColor textc, atextc, itextc;
    int leftm;
    int rightm;
    QString titletext;
    QString cuttext;
};

class QWorkspaceChildTitleBar : public QWidget
{
    Q_OBJECT

friend class QWorkspaceChild;

public:
    QWorkspaceChildTitleBar (QWorkspace* w, QWidget* win, QWidget* parent, const char* name=0, bool iconMode = FALSE );
    ~QWorkspaceChildTitleBar();

    bool isActive() const;

    QSize sizeHint() const;

public slots:
    void setActive( bool );
    void setText( const QString& title );
    void setIcon( const QPixmap& icon );

signals:
    void doActivate();
    void doNormal();
    void doClose();
    void doMaximize();
    void doMinimize();
    void doShade();
    void showOperationMenu();
    void popupOperationMenu( const QPoint& );

protected:
    void resizeEvent( QResizeEvent * );
    void mousePressEvent( QMouseEvent * );
    void mouseReleaseEvent( QMouseEvent * );
    void mouseMoveEvent( QMouseEvent * );

    bool eventFilter( QObject *, QEvent * );

private:
    QToolButton* closeB;
    QToolButton* maxB;
    QToolButton* iconB;
    QToolButton* shadeB;
    QWorkspaceChildTitleLabel* titleL;
    QLabel* iconL;
    int titleHeight;
    bool buttonDown;
    QPoint moveOffset;
    QWorkspace* workspace;
    QWidget* window;
    bool imode		    :1;
    bool act		    :1;

    QString text;
};

class QWorkspaceChild : public QFrame
{
    Q_OBJECT

friend class QWorkspace;

public:
    QWorkspaceChild( QWidget* window,
		     QWorkspace *parent=0, const char *name=0 );
    ~QWorkspaceChild();

    void setActive( bool );
    bool isActive() const;

    void adjustToFullscreen();

    QWidget* windowWidget() const;
    QWidget* iconWidget() const;

    void doResize();
    void doMove();

    QSize minimumSizeHint() const;

    QSize baseSize() const;

signals:
    void showOperationMenu();
    void popupOperationMenu( const QPoint& );

public slots:
    void activate();
    void showMinimized();
    void showMaximized();
    void showNormal();
    void showShaded();
    void setCaption( const QString& );
    void internalRaise();

    void move( int x, int y );

protected:
    void mousePressEvent( QMouseEvent * );
    void mouseReleaseEvent( QMouseEvent * );
    void mouseMoveEvent( QMouseEvent * );
    void enterEvent( QEvent * );
    void leaveEvent( QEvent * );
    void childEvent( QChildEvent* );
    void keyPressEvent( QKeyEvent * );
    void resizeEvent( QResizeEvent * );
    bool eventFilter( QObject *, QEvent * );

    bool focusNextPrevChild( bool next );

private:
    QWidget* childWidget;
    QWidget* lastfocusw;
    bool buttonDown	    :1;
    enum MousePosition {
	Nowhere,
	TopLeft, BottomRight, BottomLeft, TopRight,
	Top, Bottom, Left, Right,
	Center
    };
    MousePosition mode;
    bool moveResizeMode	    :1;
#ifndef QT_NO_CURSOR
    void setMouseCursor( MousePosition m );
#endif
    bool isMove() const {
	return moveResizeMode && mode == Center;
    }
    bool isResize() const {
	return moveResizeMode && !isMove();
    }
    QPoint moveOffset;
    QPoint invertedMoveOffset;
    QWorkspaceChildTitleBar* titlebar;
    QGuardedPtr<QWorkspaceChildTitleBar> iconw;
    QSize windowSize;
    QSize shadeRestore;
    QSize shadeRestoreMin;
    bool act		    :1;
    bool shademode	    :1;
    bool snappedRight	    :1;
    bool snappedDown	    :1;

};


class QWorkspaceData {
public:
    QWorkspaceChild* active;
    QList<QWorkspaceChild> windows;
    QList<QWorkspaceChild> focus;
    QList<QWidget> icons;
    QWorkspaceChild* maxWindow;
    QRect maxRestore;
    QGuardedPtr<QFrame> maxcontrols;
    QGuardedPtr<QMenuBar> maxmenubar;

    int px;
    int py;
    QWidget *becomeActive;
    QGuardedPtr<QWorkspaceChildTitleButton> maxtools;
    QPopupMenu* popup;
    QPopupMenu* toolPopup;
    int menuId;
    int controlId;
    QString topCaption;
    bool autoFocusChange;
};

/*!
  Constructs a workspace with a \a parent and a \a name
 */
QWorkspace::QWorkspace( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    d = new QWorkspaceData;
    d->maxcontrols = 0;
    d->active = 0;
    d->maxWindow = 0;
    d->maxtools = 0;
    d->px = 0;
    d->py = 0;
    d->becomeActive = 0;
    d->popup = new QPopupMenu( this, "qt_internal_mdi_popup" );
    d->toolPopup = new QPopupMenu( this, "qt_internal_mdi_popup" );
    d->autoFocusChange = FALSE;

    d->menuId = -1;
    d->controlId = -1;
    connect( d->popup, SIGNAL( aboutToShow() ), this, SLOT(operationMenuAboutToShow() ));
    connect( d->popup, SIGNAL( activated(int) ), this, SLOT( operationMenuActivated(int) ) );
    d->popup->insertItem(tr("&Restore"), 1);
    d->popup->insertItem(tr("&Move"), 2);
    d->popup->insertItem(tr("&Size"), 3);
    d->popup->insertItem(tr("Mi&nimize"), 4);
    d->popup->insertItem(tr("Ma&ximize"), 5);
    d->popup->insertSeparator();
    d->popup->insertItem(tr("&Close")+"\t"+QAccel::keyToString( CTRL+Key_F4),
		  this, SLOT( closeActiveWindow() ) );

    connect( d->toolPopup, SIGNAL( aboutToShow() ), this, SLOT(toolMenuAboutToShow() ));
    connect( d->toolPopup, SIGNAL( activated(int) ), this, SLOT( operationMenuActivated(int) ) );
    d->toolPopup->insertItem(tr("&Move"), 2);
    d->toolPopup->insertItem(tr("&Size"), 3);
    d->toolPopup->insertItem(tr("&Roll up"), 6);
    d->toolPopup->insertSeparator();
    d->toolPopup->insertItem(tr("&Close")+"\t"+QAccel::keyToString( CTRL+Key_F4),
		  this, SLOT( closeActiveWindow() ) );

    QAccel* a = new QAccel( this );
    a->connectItem( a->insertItem( ALT + Key_Minus),
		    this, SLOT( showOperationMenu() ) );

    a->connectItem( a->insertItem( CTRL + Key_F6),
		    this, SLOT( activateNextWindow() ) );
    a->connectItem( a->insertItem( CTRL + Key_Tab),
		    this, SLOT( activateNextWindow() ) );

    a->connectItem( a->insertItem( CTRL + SHIFT + Key_F6),
		    this, SLOT( activatePreviousWindow() ) );
    a->connectItem( a->insertItem( CTRL + SHIFT + Key_Tab),
		    this, SLOT( activatePreviousWindow() ) );

    a->connectItem( a->insertItem( CTRL + Key_F4 ),
		    this, SLOT( closeActiveWindow() ) );

    setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

    d->topCaption = topLevelWidget()->caption();
    topLevelWidget()->installEventFilter( this );
    installEventFilter( this ); // binary compatibility
}

/*!  Destroys the object and frees any allocated resources. */

QWorkspace::~QWorkspace()
{
    delete d;
    d = 0;
}

/*!\reimp
*/
QSizePolicy QWorkspace::sizePolicy() const
{
    //### removeme 3.0
    return QWidget::sizePolicy();
}

/*!\reimp
 */
QSize QWorkspace::sizeHint() const
{
    QSize s( QApplication::desktop()->size() );
    return QSize( s.width()*2/3, s.height()*2/3);
}


/*! \reimp */

void QWorkspace::childEvent( QChildEvent * e)
{
    if (e->inserted() && e->child()->isWidgetType()) {
	QWidget* w = (QWidget*) e->child();
	if ( !w || !w->testWFlags( WStyle_NormalBorder | WStyle_DialogBorder )
	     || d->icons.contains( w ) )
	    return; 	    // nothing to do

	bool hasBeenHidden = w->isHidden();

	bool hasSize = w->testWState( WState_Resized );
	bool hasPos = w->x() != 0 || w->y() != 0;
	QRect wrect = QRect( w->x(), w->y(), w->width(), w->height() );
	QWorkspaceChild* child = new QWorkspaceChild( w, this );
	child->installEventFilter( this );
	connect( child, SIGNAL( popupOperationMenu( const QPoint& ) ),
		 this, SLOT( popupOperationMenu( const QPoint& ) ) );
	connect( child, SIGNAL( showOperationMenu() ),
		 this, SLOT( showOperationMenu() ) );
	d->windows.append( child );
	if ( child->isVisibleTo( this ) )
	    d->focus.append( child );
	child->internalRaise();

	if ( hasBeenHidden )
	    w->hide();
	else if ( !isVisible() )  // that's a case were we don't receive a showEvent in time. Tricky.
	    child->show();

	place( child );
	if ( hasSize )
	    child->resize( wrect.width(), wrect.height() + child->baseSize().height() );
	if ( hasPos )
	    child->move( wrect.x(), wrect.y() );

	activateWindow( w );
    } else if (e->removed() ) {
	if ( d->windows.contains( (QWorkspaceChild*)e->child() ) ) {
	    d->windows.removeRef( (QWorkspaceChild*)e->child() );
	    d->focus.removeRef( (QWorkspaceChild*)e->child() );
	}
    }
}



void QWorkspace::activateWindow( QWidget* w, bool change_focus )
{
    if ( !w ) {
	d->active = 0;
	emit windowActivated( 0 );
	return;
    }
    if ( !isVisibleTo( 0 ) ) {
	d->becomeActive = w;
	return;
    }

    if ( d->active && d->active->windowWidget() == w && w->hasFocus() )
	return;

    for (QWorkspaceChild* c = d->windows.first(); c; c = d->windows.next() ) {
	c->setActive( c->windowWidget() == w );
	if (c->windowWidget() == w)
	    d->active = c;
    }

    if (!d->active)
	return;

    if ( d->maxWindow && d->maxWindow != d->active && d->active->windowWidget() &&
	d->active->windowWidget()->testWFlags( WStyle_MinMax ) &&
	!d->active->windowWidget()->testWFlags( WStyle_Tool ) ) {
	maximizeWindow( d->active->windowWidget() );
	if ( d->maxtools ) {
	    if ( w->icon() ) {
		d->maxtools->setPixmap( *w->icon() );
	    } else {
		QPixmap pm(14,14);
		pm.fill( white );
		d->maxtools->setPixmap( pm );
	    }
	}
    }

    d->active->internalRaise();
    w->setFocus();

    if ( change_focus ) {
	if ( d->focus.find( d->active ) >=0 ) {
	    d->focus.removeRef( d->active );
	    d->focus.append( d->active );
	}
    }

    emit windowActivated( w );
}


/*!
  Returns the active window, or 0 if no window is active.
 */
QWidget* QWorkspace::activeWindow() const
{
    return d->active?d->active->windowWidget():0;
}


void QWorkspace::place( QWidget* w)
{
    int overlap, minOverlap = 0;
    int possible;

    QRect r1(0, 0, 0, 0);
    QRect r2(0, 0, 0, 0);
    QRect maxRect = rect();
    int x = maxRect.left(), y = maxRect.top();
    QPoint wpos(maxRect.left(), maxRect.top());

    bool firstPass = TRUE;

    do {
	if ( y + w->height() > maxRect.bottom() ) {
	    overlap = -1;
	} else if( x + w->width() > maxRect.right() ) {
	    overlap = -2;
	} else {
	    overlap = 0;

	    r1.setRect(x, y, w->width(), w->height());

	    QWidget *l;
	    for (l = d->windows.first(); l ; l = d->windows.next()) {
		if (! d->icons.contains(l) && ! l->isHidden() && l != w ) {
		    r2.setRect(l->x(), l->y(), l->width(), l->height());

		    if (r2.intersects(r1)) {
			r2.setCoords(QMAX(r1.left(), r2.left()),
				     QMAX(r1.top(), r2.top()),
				     QMIN(r1.right(), r2.right()),
				     QMIN(r1.bottom(), r2.bottom())
				     );

			overlap += (r2.right() - r2.left()) *
				   (r2.bottom() - r2.top());
		    }
		}
	    }
	}

	if (overlap == 0) {
	    wpos = QPoint(x, y);
	    break;
	}

	if (firstPass) {
	    firstPass = FALSE;
	    minOverlap = overlap;
	} else if ( overlap >= 0 && overlap < minOverlap) {
	    minOverlap = overlap;
	    wpos = QPoint(x, y);
	}

	if ( overlap > 0 ) {
	    possible = maxRect.right();
	    if ( possible - w->width() > x) possible -= w->width();

	    QWidget *l;
	    for(l = d->windows.first(); l; l = d->windows.next()) {
		if (! d->icons.contains(l) && ! l->isHidden() && l != w ) {
		    r2.setRect(l->x(), l->y(), l->width(), l->height());

		    if( ( y < r2.bottom() ) && ( r2.top() < w->height() + y ) ) {
			if( r2.right() > x )
			    possible = possible < r2.right() ?
				       possible : r2.right();

			if( r2.left() - w->width() > x )
			    possible = possible < r2.left() - w->width() ?
				       possible : r2.left() - w->width();
		    }
		}
	    }

	    x = possible;
	} else if ( overlap == -2 ) {
	    x = maxRect.left();
	    possible = maxRect.bottom();

	    if ( possible - w->height() > y ) possible -= w->height();

	    QWidget *l;
	    for (l = d->windows.first(); l; l = d->windows.next()) {
		if (l != w && ! d->icons.contains(w)) {
		    r2.setRect(l->x(), l->y(), l->width(), l->height());

		    if( r2.bottom() > y)
			possible = possible < r2.bottom() ?
				   possible : r2.bottom();

		    if( r2.top() - w->height() > y )
			possible = possible < r2.top() - w->height() ?
				   possible : r2.top() - w->height();
		}
	    }

	    y = possible;
	}
    }
    while( overlap != 0 && overlap != -1 );

    w->move(wpos);
}


void QWorkspace::insertIcon( QWidget* w )
{
    if ( !w || d->icons.contains( w ) )
	return;
    d->icons.append( w );
    if (w->parentWidget() != this )
	w->reparent( this, 0, QPoint(0,0), FALSE);

    int x = 0;
    int y = height();
    for (QWidget* i = d->icons.first(); i ; i = d->icons.next() ) {

	if ( x > 0 && x + i->width() > width() ){
	    x = 0;
	    y -= i->height();
	}

	if ( i != w &&
	    i->geometry().intersects( QRect( x, y-w->height(), w->width(), w->height() ) ) )
	    x += i->width();
    }
    w->move( x, y-w->height() );

    if ( isVisibleTo( parentWidget() ) ) {
	w->show();
	w->lower();
    }

}


void QWorkspace::removeIcon( QWidget* w)
{
    if ( !d->icons.contains( w ) )
	return;
    d->icons.remove( w );
    w->hide();
}

/*! \reimp  */
void QWorkspace::resizeEvent( QResizeEvent * )
{
    if ( d->maxWindow )
	d->maxWindow->adjustToFullscreen();

    QListIterator<QWidget> it( d->icons );
    while ( it.current() ) {
	QWorkspaceChild* w = (QWorkspaceChild*)it.current();
	++it;
	int x = w->x();
	int y = w->y();
	bool m = FALSE;
	if ( x+w->width() > width() ) {
	    m = TRUE;
	    x = width() - w->width();
	}
	if ( y+w->height() > height() ) {
	    y = height() - w->height();
	    m = TRUE;
	}
	if ( m )
	    w->move( x, y );
    }

    for ( QWorkspaceChild *c = d->windows.first(); c; c = d->windows.next() ) {
	if ( c->windowWidget() && !c->windowWidget()->testWFlags( WStyle_Tool ) )
	    continue;

	int x = c->x();
	int y = c->y();
	if ( c->snappedDown )
	    y = height() - c->height();
	if ( c->snappedRight )
	    x = width() - c->width();

	if ( x != c->x() || y != c->y() )
	    c->move( x, y );
    }
}

/*! \reimp */
void QWorkspace::showEvent( QShowEvent *e )
{
    QWidget::showEvent( e );
    if ( d->becomeActive ) {
	activateWindow( d->becomeActive );
	d->becomeActive = 0;
    }
    else if ( d->windows.count() > 0 && !d->active )
	activateWindow( d->windows.first()->windowWidget() );
}

void QWorkspace::minimizeWindow( QWidget* w)
{
    QWorkspaceChild* c = findChild( w );
    if ( c ) {
	setUpdatesEnabled( FALSE );
	bool wasMax = FALSE;
	if ( c == d->maxWindow ) {
	    wasMax = TRUE;
	    d->maxWindow = 0;
	    inCaptionChange = TRUE;
	    if ( !!d->topCaption )
		topLevelWidget()->setCaption( d->topCaption );
	    inCaptionChange = FALSE;
	    hideMaximizeControls();
	}
	insertIcon( c->iconWidget() );
	c->hide();
	if ( wasMax )
	    c->setGeometry( d->maxRestore );
	d->focus.append( c );
	setUpdatesEnabled( TRUE );
    }
}

void QWorkspace::normalizeWindow( QWidget* w)
{
    QWorkspaceChild* c = findChild( w );
    if ( c ) {
	if ( d->maxWindow )
	    hideMaximizeControls();
	if ( c == d->maxWindow ) {
	    c->setGeometry( d->maxRestore );
	    d->maxWindow = 0;
	    inCaptionChange = TRUE;
	    if ( !!d->topCaption )
		topLevelWidget()->setCaption( d->topCaption );
	    inCaptionChange = FALSE;
	} else {
	    if ( c->iconw )
		removeIcon( c->iconw->parentWidget() );
	    c->show();
	}
    }
}

void QWorkspace::maximizeWindow( QWidget* w)
{
    QWorkspaceChild* c = findChild( w );

    if ( !w || w && (!w->testWFlags( WStyle_MinMax ) || w->testWFlags( WStyle_Tool) ) )
	return;

    if ( c ) {
	setUpdatesEnabled( FALSE );
	if (c->iconw && d->icons.contains( c->iconw->parentWidget() ) )
	    normalizeWindow( w );
	QRect r( c->geometry() );
	c->adjustToFullscreen();
	c->show();
	c->internalRaise();
	qApp->sendPostedEvents( c, QEvent::ShowWindowRequest );
	if ( d->maxWindow != c ) {
	    if ( d->maxWindow )
		d->maxWindow->setGeometry( d->maxRestore );
	    d->maxWindow = c;
	    d->maxRestore = r;
	}

	activateWindow( w );
	showMaximizeControls();
	inCaptionChange = TRUE;
	if ( !!d->topCaption )
	    topLevelWidget()->setCaption( QString("%1 - [%2]")
		.arg(d->topCaption).arg(c->caption()) );
	inCaptionChange = FALSE;
	setUpdatesEnabled( TRUE );
    }
}

void QWorkspace::showWindow( QWidget* w)
{
    if ( d->maxWindow && w->testWFlags( WStyle_MinMax ) && !w->testWFlags( WStyle_Tool) )
	maximizeWindow( w );
    else if ( !w->testWFlags( WStyle_Tool ) )
	normalizeWindow( w );
    if ( d->maxWindow )
	d->maxWindow->raise();
}


QWorkspaceChild* QWorkspace::findChild( QWidget* w)
{
    for (QWorkspaceChild* c = d->windows.first(); c; c = d->windows.next() ) {
	if (c->windowWidget() == w)
	    return c;
    }
    return 0;
}

/*!
  Returns a list of all windows.
 */
QWidgetList QWorkspace::windowList() const
{
    QWidgetList windows;
    for (QWorkspaceChild* c = d->windows.first(); c; c = d->windows.next() ) {
	if ( c->windowWidget() && c->windowWidget()->isVisibleTo( c ) )
	    windows.append( c->windowWidget() );
    }
    return windows;
}

/*!\reimp*/
bool QWorkspace::eventFilter( QObject *o, QEvent * e)
{
    static QTime* t = 0;
    static QWorkspace* tc = 0;

    if ( o == this ) {
	if ( e->type() == QEvent::Show ) {
	    if ( d->maxWindow )
		showMaximizeControls();
	} else if ( e->type() == QEvent::Hide ) {
	    if ( !isVisibleTo(0) )
		hideMaximizeControls();
	}
	return FALSE;
    }

    if ( o == d->maxtools && d->menuId != -1 ) {
	switch ( e->type() ) {
	case QEvent::MouseButtonPress:
	    {
		QMenuBar* b = (QMenuBar*)o->parent();
		if ( !t )
		    t = new QTime;
		if ( tc != this || t->elapsed() > QApplication::doubleClickInterval() ) {
		    popupOperationMenu( b->mapToGlobal( QPoint( b->x(), b->y() + b->height() ) ) );
		    t->start();
		    tc = this;
		} else {
		    tc = 0;
		    closeActiveWindow();
		}
		return TRUE;
	    }
	default:
	    break;
	}
	return QWidget::eventFilter( o, e );
    }
    switch ( e->type() ) {
    case QEvent::Hide:
    case QEvent::HideToParent:
	if ( !o->isA( "QWorkspaceChild" ) || !isVisible() )
	    break;
	d->focus.removeRef( (QWorkspaceChild*)o );
	if ( d->active != o )
	    break;
	if ( d->focus.isEmpty() ) {
	    activateWindow( 0 );
	} else {
	    d->autoFocusChange = TRUE;
	    activatePreviousWindow();
	    QWorkspaceChild* c = d->active;
	    while ( d->active &&
		    d->active->windowWidget() &&
		    d->active->windowWidget()->testWFlags( WStyle_Tool ) ) {
		activatePreviousWindow();
		if ( d->active == c )
		    break;
	    }
	    d->autoFocusChange = FALSE;
	}
	if ( d->maxWindow == o && d->maxWindow->isHidden() ) {
	    d->maxWindow->setGeometry( d->maxRestore );
	    d->maxWindow = 0;
	    if ( d->active )
		maximizeWindow( d->active );

	    if ( !d->maxWindow ) {
    		hideMaximizeControls();
   		inCaptionChange = TRUE;
		if ( !!d->topCaption )
		    topLevelWidget()->setCaption( d->topCaption );
		inCaptionChange = FALSE;
	    }
	}
	break;
    case QEvent::Show:
	if ( o->isA("QWorkspaceChild") && !d->focus.containsRef( (QWorkspaceChild*)o ) )
	    d->focus.append( (QWorkspaceChild*)o );
	break;
    case QEvent::CaptionChange:
	if ( inCaptionChange )
	    break;

	inCaptionChange = TRUE;
	if ( o == topLevelWidget() )
	    d->topCaption = ((QWidget*)o)->caption();

	if ( d->maxWindow && !!d->topCaption )
	    topLevelWidget()->setCaption( QString("%1 - [%2]")
		.arg(d->topCaption).arg(d->maxWindow->caption()));
	inCaptionChange = FALSE;

	break;
    case QEvent::Close:
	if ( o == topLevelWidget() )
	{
	    for (QWorkspaceChild* c = d->windows.first(); c; c = d->windows.next() ) {
		if ( c->shademode )
		    c->showShaded();
	    }
	} else if ( o->inherits("QWorkspaceChild") ) {
	    d->popup->hide();
	}
	break;
    default:
	break;
    }
    return QWidget::eventFilter( o, e);
}

void QWorkspace::showMaximizeControls()
{
#ifndef QT_NO_MENUBAR
    QMenuBar* b = 0;

    // Do a breadth-first search first, and query recoursively if nothing is found.
    QObjectList * l = topLevelWidget()->queryList( "QMenuBar", 0,
						   FALSE, FALSE );
    if ( !l || !l->count() ) {
	if ( l )
	    delete l;
	l = topLevelWidget()->queryList( "QMenuBar", 0, 0, TRUE );
    }
    if ( l && l->count() )
	b = (QMenuBar *)l->first();
    delete l;

    if ( !b )
	return;

    if ( !d->maxcontrols ) {
	d->maxmenubar = b;
	d->maxcontrols = new QFrame( topLevelWidget() );
	QHBoxLayout* l = new QHBoxLayout( d->maxcontrols,
					  d->maxcontrols->frameWidth(), 0 );
	QToolButton* iconB = new QToolButton( d->maxcontrols, "iconify" );
	QToolTip::add( iconB, tr( "Minimize" ) );
	l->addWidget( iconB );
	iconB->setFocusPolicy( NoFocus );
	iconB->setIconSet( QPixmap( (const char **)minimize_xpm ));
 	iconB->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
	connect( iconB, SIGNAL( clicked() ),
		 this, SLOT( minimizeActiveWindow() ) );
	QToolButton* restoreB = new QToolButton( d->maxcontrols, "restore" );
	QToolTip::add( restoreB, tr( "Restore Down" ) );
	l->addWidget( restoreB );
	restoreB->setFocusPolicy( NoFocus );
	restoreB->setIconSet( QPixmap( (const char **)normalize_xpm ));
 	restoreB->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
	connect( restoreB, SIGNAL( clicked() ),
		 this, SLOT( normalizeActiveWindow() ) );

	l->addSpacing( 2 );
	QToolButton* closeB = new QToolButton( d->maxcontrols, "close" );
	QToolTip::add( closeB, tr( "Close" ) );
	l->addWidget( closeB );
	closeB->setFocusPolicy( NoFocus );
	closeB->setIconSet( QPixmap( (const char **)close_xpm ) );
 	closeB->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
	connect( closeB, SIGNAL( clicked() ),
		 this, SLOT( closeActiveWindow() ) );

	d->maxcontrols->setFixedSize( 3* BUTTON_WIDTH+2+2*d->maxcontrols->frameWidth(),
				      BUTTON_HEIGHT+2*d->maxcontrols->frameWidth());
    }

    if ( d->controlId == -1 || b->indexOf( d->controlId ) == -1 ) {
	QFrame* dmaxcontrols = d->maxcontrols;
	d->controlId = b->insertItem( dmaxcontrols, -1, b->count() );
    }
    if ( d->active && ( d->menuId == -1 || b->indexOf( d->menuId ) == -1 ) ) {
	if ( !d->maxtools ) {
	    d->maxtools = new QWorkspaceChildTitleButton( topLevelWidget() );
	    d->maxtools->installEventFilter( this );
	}
	if ( d->active->windowWidget() && d->active->windowWidget()->icon() ) {
	    d->maxtools->setPixmap( *d->active->windowWidget()->icon() );
	} else {
	    QPixmap pm(14,14);
	    pm.fill( white );
	    d->maxtools->setPixmap( pm );
	}
	QWorkspaceChildTitleButton* maxtools = d->maxtools; // g++ 2.7.2.3 bug
	d->menuId = b->insertItem( maxtools, -1, 0 );
    }
#endif
}


void QWorkspace::hideMaximizeControls()
{
#ifndef QT_NO_MENUBAR
    if ( d->maxmenubar ) {
	int mi = d->menuId;
	if ( mi != -1 ) {
	    if ( d->maxmenubar->indexOf( mi ) != -1 )
		d->maxmenubar->removeItem( mi );
	    d->maxtools = 0;
	}
	int ci = d->controlId;
	if ( ci != -1 && d->maxmenubar->indexOf( ci ) != -1 )
	    d->maxmenubar->removeItem( ci );
    }
    d->maxcontrols = 0;
    d->menuId = -1;
    d->controlId = -1;
#endif
}

void QWorkspace::closeActiveWindow()
{
    setUpdatesEnabled( FALSE );
    if ( d->maxWindow && d->maxWindow->windowWidget() )
    	d->maxWindow->windowWidget()->close();
    else if ( d->active && d->active->windowWidget() )
	d->active->windowWidget()->close();
    setUpdatesEnabled( TRUE );
}

void QWorkspace::closeAllWindows()
{
    QListIterator<QWorkspaceChild> it( d->windows );
    QWorkspaceChild *c = 0;
    while ( ( c = it.current() ) != 0 ) {
	++it;
	if ( c->windowWidget() )
	    c->windowWidget()->close();
    }
}

void QWorkspace::normalizeActiveWindow()
{
    if  ( d->maxWindow )
	d->maxWindow->showNormal();
    else if ( d->active )
	d->active->showNormal();
}

void QWorkspace::minimizeActiveWindow()
{
    if ( d->maxWindow )
	d->maxWindow->showMinimized();
    else if ( d->active )
	d->active->showMinimized();
}

void QWorkspace::showOperationMenu()
{
    if  ( !d->active || !d->active->windowWidget() )
	return;
    QPoint p( d->active->windowWidget()->mapToGlobal( QPoint(0,0) ) );
    if ( !d->active->isVisible() ) {
	p = d->active->iconWidget()->mapToGlobal( QPoint(0,0) );
	p.ry() -= d->popup->sizeHint().height();
    }
    popupOperationMenu( p );
}

void QWorkspace::popupOperationMenu( const QPoint&  p)
{
    if ( !d->active || !d->active->windowWidget() || !d->active->windowWidget()->testWFlags( WStyle_SysMenu ) )
	return;
    if ( d->active->windowWidget()->testWFlags( WStyle_Tool ))
	d->toolPopup->popup( p );
    else
	d->popup->popup( p );
}

void QWorkspace::operationMenuAboutToShow()
{
    for ( int i = 1; i < 6; i++ ) {
	bool enable = d->active != 0;
	d->popup->setItemEnabled( i, enable );
    }

    if ( !d->active )
	return;

    if ( d->active == d->maxWindow ) {
	d->popup->setItemEnabled( 2, FALSE );
	d->popup->setItemEnabled( 3, FALSE );
	d->popup->setItemEnabled( 5, FALSE );
    } else if ( d->active->isVisible() ){
	d->popup->setItemEnabled( 1, FALSE );
    } else {
	d->popup->setItemEnabled( 2, FALSE );
	d->popup->setItemEnabled( 3, FALSE );
	d->popup->setItemEnabled( 4, FALSE );
    }

    if ( !d->active->windowWidget()->testWFlags( WStyle_MinMax ) ||
	  d->active->windowWidget()->testWFlags( WStyle_Tool ) ) {
	d->popup->setItemEnabled( 4, FALSE );
	d->popup->setItemEnabled( 5, FALSE );
    }
}

void QWorkspace::toolMenuAboutToShow()
{
    if ( !d->active )
	return;

    if ( d->active->shademode )
	d->toolPopup->changeItem( 6, "&Roll down" );
    else
	d->toolPopup->changeItem( 6, "&Roll up" );
}

void QWorkspace::operationMenuActivated( int a )
{
    if ( !d->active )
	return;
    switch ( a ) {
    case 1:
	d->active->showNormal();
	break;
    case 2:
	d->active->doMove();
	break;
    case 3:
	d->active->doResize();
	break;
    case 4:
	d->active->showMinimized();
	break;
    case 5:
	d->active->showMaximized();
	break;
    case 6:
	d->active->showShaded();
	break;
    default:
	break;
    }
}

void QWorkspace::activateNextWindow()
{
    if ( d->focus.isEmpty() )
	return;
    if ( !d->active ) {
	if ( d->focus.first() )
	    activateWindow( d->focus.first()->windowWidget(), FALSE );
	return;
    }

    int a = d->focus.find( d->active ) + 1;

    a = a % d->focus.count();

    if ( d->focus.at( a ) )
	activateWindow( d->focus.at( a )->windowWidget(), FALSE );
    else
	d->active = 0;
}

void QWorkspace::activatePreviousWindow()
{
    if ( d->focus.isEmpty() )
	return;
    if ( !d->active ) {
	if ( d->focus.last() )
	    activateWindow( d->focus.first()->windowWidget(), FALSE );
	else
	    activateWindow( 0 );

	return;
    }

    int a = d->focus.find( d->active ) - 1;

    if ( a < 0 )
	a = d->focus.count()-1;

    if ( d->autoFocusChange ) {
	QWidget *widget = 0;
	while ( a >= 0 && d->focus.at( a ) && ( widget = d->focus.at( a )->windowWidget() ) && !widget->isVisible() )
	    a--;
	if ( a < 0 )
	    a = d->focus.count() - 1;
    }

    if ( d->focus.at( a ) )
	activateWindow( d->focus.at( a )->windowWidget(), FALSE );
    else
	activateWindow( 0 );
}


/*!
  \fn void QWorkspace::windowActivated( QWidget* w )

  This signal is emitted when the window widget \a w becomes active. Note that
  \a w can be null, and that more than one signal may be fired for one activation
  event.

  \sa activeWindow(), windowList()
*/



/*!
  Arranges all child windows in a cascade pattern

  \sa tile()
 */
void QWorkspace::cascade()
{
    const int xoffset = 13;
    const int yoffset = 20;

    // make a list of all relevant mdi clients
    QList<QWorkspaceChild> widgets;
    QWorkspaceChild* wc = 0;
    for ( wc = d->windows.first(); wc; wc = d->windows.next() )
	if ( wc->iconw )
	    normalizeWindow( wc->windowWidget() );
    for ( wc = d->focus.first(); wc; wc = d->focus.next() )
	if ( wc->windowWidget()->isVisibleTo( this ) && !wc->windowWidget()->testWFlags( WStyle_Tool ) )
	    widgets.append( wc );

    int x = 0;
    int y = 0;

    setUpdatesEnabled( FALSE );
    QListIterator<QWorkspaceChild> it( widgets );
    int children = d->windows.count() - 1;
    while ( it.current () ) {
	QWorkspaceChild *child = it.current();
	++it;
	child->setUpdatesEnabled( FALSE );
	QSize prefSize = child->windowWidget()->sizeHint() + QSize( 0, child->baseSize().height() );
	if ( !child->windowWidget()->sizeHint().isValid() )
	    prefSize = QSize( width() - children * xoffset, height() - children * yoffset );

	prefSize = prefSize.boundedTo( QSize( size().width() - xoffset * children, size().height() - yoffset * children ) );

	int w = prefSize.width();
	int h = prefSize.height();

	child->showNormal();
	qApp->sendPostedEvents( 0, QEvent::ShowNormal );
	if ( y + h > height() )
	    y = 0;
	if ( x + w > width() )
	    x = 0;
	child->setGeometry( x, y, w, h );
	x += xoffset;
	y += yoffset;
	child->internalRaise();
	child->setUpdatesEnabled( TRUE );
    }
    setUpdatesEnabled( TRUE );
}

/*!
  Arranges all child windows in a tile pattern

  \sa cascade()
 */
void QWorkspace::tile()
{
    int rows = 1;
    int cols = 1;
    int n = 0;
    QWorkspaceChild* c;
    for ( c = d->windows.first(); c; c = d->windows.next() ) {
	if ( !c->windowWidget()->isHidden() &&
	     !c->windowWidget()->testWFlags( WStyle_StaysOnTop ) &&
	     !c->windowWidget()->testWFlags( WStyle_Tool ) )
	    n++;
    }

    while ( rows * cols < n ) {
	if ( cols <= rows )
	    cols++;
	else
	    rows++;
    }
    int add = cols * rows - n;
    bool* used = new bool[ cols*rows ];
    for ( int i = 0; i < rows*cols; i++ )
	used[i] = FALSE;

    int row = 0;
    int col = 0;
    int w = width() / cols;
    int h = height() / rows;
    for ( c = d->windows.first(); c; c = d->windows.next() ) {
	if ( c->windowWidget()->isHidden() || c->windowWidget()->testWFlags( WStyle_Tool ) )
	    continue;
	if ( c->windowWidget()->testWFlags( WStyle_StaysOnTop ) ) {
	    QPoint p = c->pos();
	    if ( p.x()+c->width() < 0 )
		p.setX( 0 );
	    if ( p.x() > width() )
		p.setX( width() - c->width() );
	    if ( p.y() + 10 < 0 )
		p.setY( 0 );
	    if ( p.y() > height() )
		p.setY( height() - c->height() );


	    if ( p != c->pos() )
		c->QFrame::move( p );
	} else {
	    c->showNormal();
	    qApp->sendPostedEvents( 0, QEvent::ShowNormal );
	    used[row*cols+col] = TRUE;
	    if ( add ) {
		c->setGeometry( col*w, row*h, w, 2*h );
		used[(row+1)*cols+col] = TRUE;
		add--;
	    } else {
		c->setGeometry( col*w, row*h, w, h );
	    }
	    while( row < rows && col < cols && used[row*cols+col] ) {
		col++;
		if ( col == cols ) {
		    col = 0;
		    row++;
		}
	    }
	}
    }
    delete [] used;
}


QWorkspaceChildTitleBar::QWorkspaceChildTitleBar (QWorkspace* w, QWidget* win, QWidget* parent,
						  const char* name, bool iconMode )
    : QWidget( parent, name, WStyle_Customize | WStyle_NoBorder )
{
    workspace = w;
    window = win;
    buttonDown = FALSE;
    imode = iconMode;
    act = TRUE;

    titleL = new QWorkspaceChildTitleLabel( this, "__workspace_child_title_bar" );
    closeB = new QToolButton( this, "close" );
    QToolTip::add( closeB, tr( "Close" ) );
    closeB->setFocusPolicy( NoFocus );
    closeB->setIconSet( QPixmap( (const char **)close_xpm ) );
    connect( closeB, SIGNAL( clicked() ),
	     this, SIGNAL( doClose() ) ) ;
    closeB->installEventFilter( this );

    maxB = new QToolButton( this, "maximize" );
    QToolTip::add( maxB, tr( "Maximize" ) );
    maxB->setFocusPolicy( NoFocus );
    maxB->setIconSet( QPixmap( (const char **)maximize_xpm ));
    connect( maxB, SIGNAL( clicked() ),
	     this, SIGNAL( doMaximize() ) );
    maxB->installEventFilter( this );

    iconB = new QToolButton( this, "iconify" );
    iconB->setFocusPolicy( NoFocus );
    iconB->installEventFilter( this );

    shadeB = new QToolButton( this, "shade" );
    QToolTip::add( shadeB, tr( "Roll up" ) );
    shadeB->setIconSet( QPixmap( (const char **)shade_xpm ) );
    shadeB->setFocusPolicy( NoFocus );
    connect( shadeB, SIGNAL( clicked() ),
	     this, SIGNAL( doShade() ) );
    shadeB->installEventFilter( this );

    iconL = new QLabel( this, "icon" );
    iconL->setAlignment( AlignCenter );
    iconL->setAutoMask( TRUE );
    iconL->setFocusPolicy( NoFocus );
    iconL->installEventFilter( this );

    if ( window ) {
	if ( !window->testWFlags( WStyle_Tool ) ) {
#ifdef _WS_WIN_
	    titleHeight = GetSystemMetrics( SM_CYCAPTION );
	    if ( !titleHeight )
		titleHeight = 18;
#else
	    titleHeight = 18;
#endif
	    closeB->resize(titleHeight-4, titleHeight-4);
	    maxB->resize(titleHeight-4, titleHeight-4);
	    iconB->resize(titleHeight-4, titleHeight-4);
	    shadeB->resize(titleHeight-4, titleHeight-4);
	    iconL->resize(titleHeight-4, titleHeight-4);
	    shadeB->hide();
	    if ( !window->testWFlags( WStyle_MinMax ) ) {
		maxB->hide();
		iconB->hide();
	    }
	    if ( !window->testWFlags( WStyle_SysMenu ) ) {
		iconL->hide();
		closeB->hide();
		maxB->hide();
		iconB->hide();
	    }
	} else {
#ifdef _WS_WIN_
	    titleHeight = GetSystemMetrics( SM_CYSMCAPTION );
	    if ( !titleHeight )
		titleHeight = 16;
#else
	    titleHeight = 16;
#endif
   	    closeB->resize( titleHeight-2, titleHeight-2 );
	    maxB->resize( titleHeight-2, titleHeight-2 );
	    iconB->resize( titleHeight-2, titleHeight-2 );
	    shadeB->resize( titleHeight-2, titleHeight-2 );
	    iconL->resize(titleHeight-2, titleHeight-2);
	    maxB->hide();
	    iconB->hide();
	    iconL->hide();
	    if ( !window->testWFlags( WStyle_MinMax ) )
		shadeB->hide();
        }
    } else if ( iconMode ) {
#ifdef _WS_WIN_
	titleHeight = GetSystemMetrics( SM_CYCAPTION );
	if ( !titleHeight )
	    titleHeight = 18;
#else
	titleHeight = 18;
#endif
	closeB->resize(titleHeight-4, titleHeight-4);
	maxB->resize(titleHeight-4, titleHeight-4);
	iconB->resize(titleHeight-4, titleHeight-4);
	shadeB->resize(titleHeight-4, titleHeight-4);
	iconL->resize(titleHeight-4, titleHeight-4);
	shadeB->hide();
    }

    if ( imode ) {
	iconB->setIconSet( QPixmap( (const char **)normalizeup_xpm ) );
	QToolTip::add( iconB, tr( "Restore Up" ) );
	connect( iconB, SIGNAL( clicked() ),
		 this, SIGNAL( doNormal() ) );
    }
    else {
	iconB->setIconSet( QPixmap( (const char **)minimize_xpm ) );
	QToolTip::add( iconB, tr( "Minimize" ) );
	connect( iconB, SIGNAL( clicked() ),
		 this, SIGNAL( doMinimize() ) );
    }

    titleL->installEventFilter( this );
    QFont f = font();
    f.setBold( TRUE );
#ifdef _WS_WIN_ // Don't scale fonts on X
    if ( window && window->testWFlags( WStyle_Tool ) )
	f.setPointSize( f.pointSize() - 1 );
#endif
    titleL->setFont( f );
    setActive( FALSE );
}

QWorkspaceChildTitleBar::~QWorkspaceChildTitleBar()
{
}

void QWorkspaceChildTitleBar::mousePressEvent( QMouseEvent * e)
{
    if ( e->button() == LeftButton ) {
	buttonDown = TRUE;
	moveOffset = mapToParent( e->pos() );
	emit doActivate();
    } else if ( e->button() == RightButton ) {
	emit doActivate();
	emit popupOperationMenu( e->globalPos() );
    }
}

void QWorkspaceChildTitleBar::mouseReleaseEvent( QMouseEvent * e)
{
    if ( e->button() == LeftButton ) {
	buttonDown = FALSE;
	releaseMouse();
    }
}

void QWorkspaceChildTitleBar::mouseMoveEvent( QMouseEvent * e)
{
    if ( !buttonDown )
	return;

    if ( (moveOffset - mapToParent( e->pos() ) ).manhattanLength() < 4 )
	return;

    QPoint p = workspace->mapFromGlobal( e->globalPos() );
    if ( !workspace->rect().contains(p) ) {
	if ( p.x() < 0 )
	    p.rx() = 0;
	if ( p.y() < 0 )
	    p.ry() = 0;
	if ( p.x() > workspace->width() )
	    p.rx() = workspace->width();
	if ( p.y() > workspace->height() )
	    p.ry() = workspace->height();
    }

    QPoint pp = p - moveOffset;

    parentWidget()->move( pp );
}

void QWorkspaceChildTitleBar::setText( const QString& title )
{
    text = title;
    titleL->setText( title );
}

void QWorkspaceChildTitleBar::setIcon( const QPixmap& icon )
{
    int w = iconL->contentsRect().width();
    int h = iconL->contentsRect().height();
    if ( icon.height() > h || icon.width() > w ) {
	QPixmap p;
	p.convertFromImage( icon.convertToImage().smoothScale( w, h ) );
	iconL->setPixmap( p );
    } else {
	iconL->setPixmap( icon );
    }
}

bool QWorkspaceChildTitleBar::eventFilter( QObject * o, QEvent * e)
{
    static QTime* t = 0;
    static QWorkspaceChildTitleBar* tc = 0;
    if ( o == titleL || o == closeB || o == maxB || o == iconB || o == shadeB ) {
	if ( e->type() == QEvent::MouseButtonPress
	     || e->type() == QEvent::MouseButtonRelease
	     || e->type() == QEvent::MouseMove) {
	    QMouseEvent* me = (QMouseEvent*) e;
	    QMouseEvent ne( me->type(), titleL->mapToParent(me->pos()), me->globalPos(),
			    me->button(), me->state() );

	    if (e->type() == QEvent::MouseButtonPress )
		mousePressEvent( &ne );
	    else if ( o == titleL && e->type() == QEvent::MouseButtonRelease )
		mouseReleaseEvent( &ne );
	    else if ( o == titleL )
		mouseMoveEvent( &ne );
	} else if ( (e->type() == QEvent::MouseButtonDblClick) &&
		  ((QMouseEvent*)e)->button() == LeftButton ) {
	    if ( imode )
		emit doNormal();
	    else if ( !maxB->testWState(WState_ForceHide) )
		emit doMaximize();
	    else
	        emit doShade();
	    return TRUE;
	}
    } else if ( o == iconL ) {
	switch ( e->type() ) {
	case QEvent::MouseButtonPress:
	    emit doActivate();
	    if ( !t )
		t = new QTime;
	    if ( tc != this || t->elapsed() > QApplication::doubleClickInterval() ) {
		emit showOperationMenu();
		t->start();
		tc = this;
	    } else {
		tc = 0;
		emit doClose();
	    }
	    return TRUE;
	default:
	    break;
	}
    }
    return QWidget::eventFilter(o, e);
}


void QWorkspaceChildTitleBar::resizeEvent( QResizeEvent * )
{ // NOTE: called with 0 pointer
    int bo = ( height()- closeB->height() ) / 2;
    closeB->move( width() - closeB->width() - bo - 1, bo  );
    maxB->move( closeB->x() - maxB->width() - bo, closeB->y() );
    iconB->move( maxB->x() - iconB->width(), maxB->y() );
    shadeB->move( closeB->x() - shadeB->width(), closeB->y() );

    iconL->move( 2, 1 );
    int right = closeB->isVisibleTo( this ) ? closeB->x() : width();
    if ( iconB->isVisibleTo( this ) )
	right = iconB->x();
    if ( shadeB->isVisibleTo( this ) )
	right = shadeB->x();

    int bottom = rect().bottom() - imode - 1;

    if ( right != width() )
	right-=2;

    titleL->setRightMargin( width() - right );
    titleL->setLeftMargin( iconL->isVisibleTo( this ) ? iconL->width()+4 : 2 );


    titleL->setGeometry( QRect( QPoint( 1, 0 ),
 				QPoint( rect().right()-1, bottom ) ) );
}

void QWorkspaceChildTitleBar::setActive( bool active )
{
    if ( act == active )
	return ;

    act = active;

    titleL->setActive( active );
    if ( active ) {
	if ( imode && !win32 ){
	    iconB->show();
	    maxB->show();
	    closeB->show();
	}
    } else {
	if ( imode && !win32 ){
	    iconB->hide();
	    closeB->hide();
	    maxB->hide();
	}
    }
    if ( imode )
	resizeEvent( 0 );
}

bool QWorkspaceChildTitleBar::isActive() const
{
    return act;
}


QSize QWorkspaceChildTitleBar::sizeHint() const
{
    constPolish();

    return QSize( 128, QMAX( titleHeight, fontMetrics().lineSpacing() ) );
}


QWorkspaceChild::QWorkspaceChild( QWidget* window, QWorkspace *parent,
				  const char *name )
    : QFrame( parent, name,
	      WStyle_Customize | WStyle_NoBorder  | WDestructiveClose )
{
    mode = Nowhere;
    buttonDown = FALSE;
    setMouseTracking( TRUE );
    act = FALSE;
    iconw = 0;
    lastfocusw = 0;
    shademode = FALSE;
    titlebar = 0;
    snappedRight = FALSE;
    snappedDown = FALSE;

    if ( window && window->testWFlags( WStyle_Title ) ) {
	titlebar = new QWorkspaceChildTitleBar( parent, window, this );
	connect( titlebar, SIGNAL( doActivate() ),
		 this, SLOT( activate() ) );
	connect( titlebar, SIGNAL( doClose() ),
		 window, SLOT( close() ) );
	connect( titlebar, SIGNAL( doMinimize() ),
		 this, SLOT( showMinimized() ) );
	connect( titlebar, SIGNAL( doMaximize() ),
		 this, SLOT( showMaximized() ) );
	connect( titlebar, SIGNAL( popupOperationMenu( const QPoint& ) ),
		 this, SIGNAL( popupOperationMenu( const QPoint& ) ) );
	connect( titlebar, SIGNAL( showOperationMenu() ),
		 this, SIGNAL( showOperationMenu() ) );
	connect( titlebar, SIGNAL( doShade() ),
		 this, SLOT( showShaded() ) );
    }


    if ( window && window->testWFlags( WStyle_Tool ) ) {
	setFrameStyle( QFrame::StyledPanel | QFrame::Raised );
	setLineWidth( 2 );
	setMinimumSize( 128, 0 );
    } else {
	setFrameStyle( QFrame::WinPanel | QFrame::Raised );
	setMinimumSize( 128, 0 );
    }

    childWidget = window;
    if (!childWidget)
	return;

    setCaption( childWidget->caption() );

    QPoint p;
    QSize s;
    QSize cs;

    bool hasBeenResized = childWidget->testWState( WState_Resized );

    if ( !hasBeenResized )
	cs = childWidget->sizeHint();
    else
	cs = childWidget->size();

    if ( titlebar ) {
	if( childWidget->icon() )
	    titlebar->setIcon( *childWidget->icon() );
	int th = titlebar->sizeHint().height();
	p = QPoint( contentsRect().x(),
		     th + TITLEBAR_SEPARATION +
		     contentsRect().y() );
	s = QSize( cs.width() + 2*frameWidth(),
		   cs.height() + 2*frameWidth() + th +TITLEBAR_SEPARATION );
    } else {
	p = QPoint( contentsRect().x(), contentsRect().y() );
	s = QSize( cs.width() + 2*frameWidth(),
		    cs.height() + 2*frameWidth() );
    }

    childWidget->reparent( this, p);
    resize( s );

    childWidget->installEventFilter( this );
}

QWorkspaceChild::~QWorkspaceChild()
{
    if ( iconw )
	delete iconw->parentWidget();
}

void QWorkspaceChild::resizeEvent( QResizeEvent * )
{
    QRect r = contentsRect();
    QRect cr;

    if ( titlebar ) {
	int th = titlebar->sizeHint().height();
	titlebar->setGeometry( r.x() , r.y(), r.width(), th );
	cr = QRect( r.x(), r.y() + titlebar->height() + TITLEBAR_SEPARATION + (shademode ? 5 : 0 ),
	    r.width() , r.height() - titlebar->height() - TITLEBAR_SEPARATION );
    } else {
	cr = r;
    }

    if (!childWidget)
	return;

    windowSize = cr.size();
    childWidget->setGeometry( cr );
}

QSize QWorkspaceChild::baseSize() const
{
    int th = titlebar ? titlebar->sizeHint().height() : 0;
    int ts = titlebar ? TITLEBAR_SEPARATION : 0;
    return QSize( 2*frameWidth(), 2*frameWidth() + th + ts);
}

QSize QWorkspaceChild::minimumSizeHint() const
{
    if ( !childWidget )
	return QFrame::minimumSizeHint() + baseSize();
    QSize s = childWidget->minimumSize();
    if ( s.isEmpty() )
	s = childWidget->minimumSizeHint();
    return s + baseSize();
}

void QWorkspaceChild::activate()
{
    ((QWorkspace*)parentWidget())->activateWindow( windowWidget() );
}


bool QWorkspaceChild::eventFilter( QObject * o, QEvent * e)
{
    if ( !isActive() && ( e->type() == QEvent::MouseButtonPress ||
	e->type() == QEvent::FocusIn ) ) {
	if ( iconw ) {
	    ((QWorkspace*)parentWidget())->normalizeWindow( windowWidget() );
	    ((QWorkspace*)parentWidget())->removeIcon( iconw->parentWidget() );
	    delete iconw->parentWidget();
	    iconw = 0;
	}
	activate();
    }

    // for all widgets except the window, we that's the only thing we
    // process, and if we have no childWidget we skip totally
    if ( o != childWidget || childWidget == 0 )
	return FALSE;

    switch ( e->type() ) {
    case QEvent::Show:
	if ( ((QWorkspace*)parentWidget())->d->focus.find( this ) < 0 )
	    ((QWorkspace*)parentWidget())->d->focus.append( this );
	if ( isVisibleTo( parentWidget() ) )
	    break;
	if (( (QShowEvent*)e)->spontaneous() )
	    break;
	// FALL THROUGH
    case QEvent::ShowToParent:
	if ( windowWidget() && windowWidget()->testWFlags( WStyle_StaysOnTop ) ) {
	    internalRaise();
	    show();
	}
	((QWorkspace*)parentWidget())->showWindow( windowWidget() );
	break;
    case QEvent::ShowMaximized:
	((QWorkspace*)parentWidget())->maximizeWindow( windowWidget() );
	break;
    case QEvent::ShowMinimized:
	((QWorkspace*)parentWidget())->minimizeWindow( windowWidget() );
	break;
    case QEvent::ShowNormal:
	((QWorkspace*)parentWidget())->normalizeWindow( windowWidget() );
	if (iconw) {
	    ((QWorkspace*)parentWidget())->removeIcon( iconw->parentWidget() );
	    delete iconw->parentWidget();
	}
	break;
    case QEvent::Hide:
    case QEvent::HideToParent:
	if ( !childWidget->isVisibleTo( this ) ) {
	    QWidget * w = iconw;
	    if ( w && ( w = w->parentWidget() ) ) {
		((QWorkspace*)parentWidget())->removeIcon( w );
		delete w;
	    }
	    hide();
	}
	break;
    case QEvent::CaptionChange:
	setCaption( childWidget->caption() );
	if ( iconw )
 	    iconw->setText( childWidget->caption() );
	break;
    case QEvent::IconChange:
	{
	    QWorkspace* ws = (QWorkspace*)parentWidget();
	    if ( !titlebar )
		break;
	    if ( childWidget->icon() ) {
		titlebar->setIcon( *childWidget->icon() );
	    } else {
		QPixmap pm;
		titlebar->setIcon( pm );
	    }

	    if ( ws->d->maxWindow != this )
		break;

	    if ( ws->d->maxtools ) {
		if ( childWidget->icon() ) {
		    ws->d->maxtools->setPixmap( *childWidget->icon() );
		} else {
		    QPixmap pm(14,14);
		    pm.fill( white );
		    ws->d->maxtools->setPixmap( pm );
		}
	    }
	}
	break;
    case QEvent::Resize:
	{
	    QResizeEvent* re = (QResizeEvent*)e;
	    if ( re->size() != windowSize && !shademode )
		resize( re->size() + baseSize() );
	}
	break;
    default:
	break;
    }

    return QFrame::eventFilter(o, e);
}


void QWorkspaceChild::childEvent( QChildEvent*  e)
{
    if ( e->type() == QEvent::ChildRemoved && e->child() == childWidget ) {
	childWidget = 0;
	if ( iconw ) {
	    ((QWorkspace*)parentWidget())->removeIcon( iconw->parentWidget() );
	    delete iconw->parentWidget();
	}
	close();
    }
}

void QWorkspaceChild::mousePressEvent( QMouseEvent * e)
{
    if ( e->button() == LeftButton ) {
	activate();
	mouseMoveEvent( e );
	buttonDown = TRUE;
	moveOffset = e->pos();
	invertedMoveOffset = rect().bottomRight() - e->pos();
    }
}

void QWorkspaceChild::mouseReleaseEvent( QMouseEvent * e)
{
    if ( e->button() == LeftButton ) {
	buttonDown = FALSE;
	releaseMouse();
	releaseKeyboard();
    }
}

void QWorkspaceChild::mouseMoveEvent( QMouseEvent * e)
{
    if ( shademode )
	return;

    if ( !buttonDown ) {
	if ( e->pos().y() <= RANGE && e->pos().x() <= RANGE)
	    mode = TopLeft;
	else if ( e->pos().y() >= height()-RANGE && e->pos().x() >= width()-RANGE)
	    mode = BottomRight;
	else if ( e->pos().y() >= height()-RANGE && e->pos().x() <= RANGE)
	    mode = BottomLeft;
	else if ( e->pos().y() <= RANGE && e->pos().x() >= width()-RANGE)
	    mode = TopRight;
	else if ( e->pos().y() <= RANGE )
	    mode = Top;
	else if ( e->pos().y() >= height()-RANGE )
	    mode = Bottom;
	else if ( e->pos().x() <= RANGE )
	    mode = Left;
	else if (  e->pos().x() >= width()-RANGE )
	    mode = Right;
	else
	    mode = Center;
#ifndef QT_NO_CURSOR
	setMouseCursor( mode );
#endif
	return;
    }

    if ( testWState(WState_ConfigPending) )
 	return;

    QPoint globalPos = parentWidget()->mapFromGlobal( e->globalPos() );
    if ( !parentWidget()->rect().contains( globalPos ) ) {
	if ( globalPos.x() < 0 )
	    globalPos.rx() = 0;
	if ( globalPos.y() < 0 )
	    globalPos.ry() = 0;
	if ( globalPos.x() > parentWidget()->width() )
	    globalPos.rx() = parentWidget()->width();
	if ( globalPos.y() > parentWidget()->height() )
	    globalPos.ry() = parentWidget()->height();
    }
    QPoint p = globalPos + invertedMoveOffset;
    QPoint pp = globalPos - moveOffset;

    int th = titlebar ? titlebar->sizeHint().height() : 0;
    int ts = titlebar ? TITLEBAR_SEPARATION : 0;

    int mw = QMAX(childWidget->minimumSizeHint().width(),
		  childWidget->minimumWidth()) + 2 * frameWidth();
    int mh = QMAX(childWidget->minimumSizeHint().height(),
		  childWidget->minimumHeight()) +  2 * frameWidth() + ts + th + 1;

    QSize mpsize( geometry().right() - pp.x() + 1,
		  geometry().bottom() - pp.y() + 1 );
    mpsize = mpsize.expandedTo( minimumSize() ).expandedTo( QSize(mw, mh) );
    QPoint mp( geometry().right() - mpsize.width() + 1,
	       geometry().bottom() - mpsize.height() + 1 );

    QRect geom = geometry();

    switch ( mode ) {
    case TopLeft:
	geom =  QRect( mp, geometry().bottomRight() ) ;
	break;
    case BottomRight:
	geom =  QRect( geometry().topLeft(), p ) ;
	break;
    case BottomLeft:
	geom =  QRect( QPoint(mp.x(), geometry().y() ), QPoint( geometry().right(), p.y()) ) ;
	break;
    case TopRight:
	geom =  QRect( QPoint(geometry().x(), mp.y() ), QPoint( p.x(), geometry().bottom()) ) ;
	break;
    case Top:
	geom =  QRect( QPoint( geometry().left(), mp.y() ), geometry().bottomRight() ) ;
	break;
    case Bottom:
	geom =  QRect( geometry().topLeft(), QPoint( geometry().right(), p.y() ) ) ;
	break;
    case Left:
	geom =  QRect( QPoint( mp.x(), geometry().top() ), geometry().bottomRight() ) ;
	break;
    case Right:
	geom =  QRect( geometry().topLeft(), QPoint( p.x(), geometry().bottom() ) ) ;
	break;
    case Center:
	geom.moveTopLeft( pp );
	break;
    default:
	break;
    }

    geom = QRect( geom.topLeft(), geom.size().expandedTo( minimumSize() ).expandedTo( QSize(mw,mh) ).
	boundedTo( childWidget->maximumSize() + QSize( 2*frameWidth(), 2*frameWidth()+ts+th+1 ) ) );

    if ( geom != geometry() && parentWidget()->rect().intersects( geom ) )
	setGeometry( geom );

#ifdef _WS_WIN_
    MSG msg;
    while(PeekMessage( &msg, winId(), WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE ))
	;
#endif
    QApplication::syncX();
}



void QWorkspaceChild::enterEvent( QEvent * )
{
}

void QWorkspaceChild::leaveEvent( QEvent * )
{
#ifndef QT_NO_CURSOR
    if ( !buttonDown )
	setCursor( arrowCursor );
#endif
}


static bool isChildOf( QWidget * child, QWidget * parent )
{
    if ( !parent || !child )
	return FALSE;
    QWidget * w = child;
    while( w && w != parent )
	w = w->parentWidget();
    return w != 0;
}


void QWorkspaceChild::setActive( bool b )
{
    if ( !childWidget )
	return;

    act = b;

    if ( titlebar )
	titlebar->setActive( act );
    if ( iconw )
	iconw->setActive( act );

    QObjectList* ol = childWidget->queryList( "QWidget" );
    if ( act ) {
	QObject *o;
	for ( o = ol->first(); o; o = ol->next() )
	    o->removeEventFilter( this );
	bool hasFocus = isChildOf( focusWidget(), childWidget );
	if ( !hasFocus ) {
	    if ( lastfocusw && ol->contains( lastfocusw ) &&
		 lastfocusw->focusPolicy() != NoFocus ) {
		// this is a bug if lastfocusw has been deleted, a new
		// widget has been created, and the new one is a child
		// of the same window as the old one.  but even though
		// it's a bug the behaviour is reasonable :)
		lastfocusw->setFocus();
	    } else if ( childWidget->focusPolicy() != NoFocus ) {
		childWidget->setFocus();
	    } else {
		// find something, anything, that accepts focus, and use that.
		o = ol->first();
		while( o && ((QWidget*)o)->focusPolicy() == NoFocus )
		    o = ol->next();
		if ( o )
		    ((QWidget*)o)->setFocus();
	    }
	}
    } else {
	lastfocusw = 0;
	if ( isChildOf( focusWidget(), childWidget ) )
	    lastfocusw = focusWidget();
	QObject * o;
	for ( o = ol->first(); o; o = ol->next() ) {
	    o->removeEventFilter( this );
	    o->installEventFilter( this );
	}
    }
    delete ol;
}

bool QWorkspaceChild::isActive() const
{
    return act;
}

QWidget* QWorkspaceChild::windowWidget() const
{
    return childWidget;
}


QWidget* QWorkspaceChild::iconWidget() const
{
    if ( !iconw ) {
	QWorkspaceChild* that = (QWorkspaceChild*) this;
	QVBox* vbox = new QVBox;
	vbox->setFrameStyle( QFrame::WinPanel | QFrame::Raised );
	vbox->resize( 196+2*vbox->frameWidth(), 20 + 2*vbox->frameWidth() );
	that->iconw = new QWorkspaceChildTitleBar( (QWorkspace*)parentWidget(), 0, vbox, "_workspacechild_icon_", TRUE );
	iconw->setActive( isActive() );
	connect( iconw, SIGNAL( doActivate() ),
		 this, SLOT( activate() ) );
	connect( iconw, SIGNAL( doClose() ),
		 windowWidget(), SLOT( close() ) );
	connect( iconw, SIGNAL( doNormal() ),
		 this, SLOT( showNormal() ) );
	connect( iconw, SIGNAL( doMaximize() ),
		 this, SLOT( showMaximized() ) );
	connect( iconw, SIGNAL( popupOperationMenu( const QPoint& ) ),
		 this, SIGNAL( popupOperationMenu( const QPoint& ) ) );
	connect( iconw, SIGNAL( showOperationMenu() ),
		 this, SIGNAL( showOperationMenu() ) );
    }
    if ( windowWidget() ) {
	iconw->setText( windowWidget()->caption() );
	if ( windowWidget()->icon() )
	    iconw->setIcon( *windowWidget()->icon() );
    }
    return iconw->parentWidget();
}

void QWorkspaceChild::showMinimized()
{
    QApplication::postEvent( windowWidget(), new QEvent( QEvent::ShowMinimized ) );
}

void QWorkspaceChild::showMaximized()
{
    QApplication::postEvent( windowWidget(), new QEvent( QEvent::ShowMaximized ) );
}

void QWorkspaceChild::showNormal()
{
    QApplication::postEvent( windowWidget(), new QEvent( QEvent::ShowNormal ) );
}

void QWorkspaceChild::showShaded()
{
    if ( !titlebar)
	return;

    QToolTip::remove( titlebar->shadeB );
    ((QWorkspace*)parentWidget())->activateWindow( windowWidget() );
    if ( shademode ) {
	QToolTip::add( titlebar->shadeB, tr( "Roll up" ) );
	titlebar->shadeB->setIconSet( QPixmap( (const char **)shade_xpm ) );
	shademode = FALSE;
	resize( shadeRestore );
	setMinimumSize( shadeRestoreMin );
    } else {
	QToolTip::add( titlebar->shadeB, tr( "Roll down" ) );
	titlebar->shadeB->setIconSet( QPixmap( (const char **)unshade_xpm ) );
	shadeRestore = size();
	shadeRestoreMin = minimumSize();
	setMinimumHeight(0);
	shademode = TRUE;
	resize( width(), titlebar->height() + TITLEBAR_SEPARATION + 2*lineWidth() );
    }
}

void QWorkspaceChild::adjustToFullscreen()
{
    qApp->sendPostedEvents( childWidget, QEvent::Resize );
    setGeometry( -childWidget->x(), -childWidget->y(),
		 parentWidget()->width() + width() - childWidget->width(),
		 parentWidget()->height() + height() - childWidget->height() );
}


#ifndef QT_NO_CURSOR
/*!
  Sets an appropriate cursor shape for the logical mouse position \a m

  \sa QWidget::setCursor()
 */
void QWorkspaceChild::setMouseCursor( MousePosition m )
{
    switch ( m ) {
    case TopLeft:
    case BottomRight:
	setCursor( sizeFDiagCursor );
	break;
    case BottomLeft:
    case TopRight:
	setCursor( sizeBDiagCursor );
	break;
    case Top:
    case Bottom:
	setCursor( sizeVerCursor );
	break;
    case Left:
    case Right:
	setCursor( sizeHorCursor );
	break;
    default:
	setCursor( arrowCursor );
	break;
    }
}
#endif

void QWorkspaceChild::keyPressEvent( QKeyEvent * e )
{
    if ( !isMove() && !isResize() )
	return;
    bool is_control = e->state() & ControlButton;
    int delta = is_control?1:8;
    QPoint pos = QCursor::pos();
    switch ( e->key() ) {
    case Key_Left:
	pos.rx() -= delta;
	if ( pos.x() <= QApplication::desktop()->geometry().left() ) {
	    if ( mode == TopLeft || mode == BottomLeft ) {
		moveOffset.rx() += delta;
		invertedMoveOffset.rx() += delta;
	    } else {
		moveOffset.rx() -= delta;
		invertedMoveOffset.rx() -= delta;
	    }
	}
	if ( isResize() && !resizeHorizontalDirectionFixed ) {
	    resizeHorizontalDirectionFixed = TRUE;
	    if ( mode == BottomRight )
		mode = BottomLeft;
	    else if ( mode == TopRight )
		mode = TopLeft;
#ifndef QT_NO_CURSOR
	    setMouseCursor( mode );
	    grabMouse( cursor() );
#else
	    grabMouse();
#endif
	}
	break;
    case Key_Right:
	pos.rx() += delta;
	if ( pos.x() >= QApplication::desktop()->geometry().right() ) {
	    if ( mode == TopRight || mode == BottomRight ) {
		moveOffset.rx() += delta;
		invertedMoveOffset.rx() += delta;
	    } else {
		moveOffset.rx() -= delta;
		invertedMoveOffset.rx() -= delta;
	    }
	}
	if ( isResize() && !resizeHorizontalDirectionFixed ) {
	    resizeHorizontalDirectionFixed = TRUE;
	    if ( mode == BottomLeft )
		mode = BottomRight;
	    else if ( mode == TopLeft )
		mode = TopRight;
#ifndef QT_NO_CURSOR
	    setMouseCursor( mode );
	    grabMouse( cursor() );
#else
	    grabMouse();
#endif
	}
	break;
    case Key_Up:
	pos.ry() -= delta;
	if ( pos.y() <= QApplication::desktop()->geometry().top() ) {
	    if ( mode == TopLeft || mode == TopRight ) {
		moveOffset.ry() += delta;
		invertedMoveOffset.ry() += delta;
	    } else {
		moveOffset.ry() -= delta;
		invertedMoveOffset.ry() -= delta;
	    }
	}
	if ( isResize() && !resizeVerticalDirectionFixed ) {
	    resizeVerticalDirectionFixed = TRUE;
	    if ( mode == BottomLeft )
		mode = TopLeft;
	    else if ( mode == BottomRight )
		mode = TopRight;
#ifndef QT_NO_CURSOR
	    setMouseCursor( mode );
	    grabMouse( cursor() );
#else
	    grabMouse();
#endif
	}
	break;
    case Key_Down:
	pos.ry() += delta;
	if ( pos.y() >= QApplication::desktop()->geometry().bottom() ) {
	    if ( mode == BottomLeft || mode == BottomRight ) {
		moveOffset.ry() += delta;
		invertedMoveOffset.ry() += delta;
	    } else {
		moveOffset.ry() -= delta;
		invertedMoveOffset.ry() -= delta;
	    }
	}
	if ( isResize() && !resizeVerticalDirectionFixed ) {
	    resizeVerticalDirectionFixed = TRUE;
	    if ( mode == TopLeft )
		mode = BottomLeft;
	    else if ( mode == TopRight )
		mode = BottomRight;
#ifndef QT_NO_CURSOR
	    setMouseCursor( mode );
	    grabMouse( cursor() );
#else
	    grabMouse();
#endif
	}
	break;
    case Key_Space:
    case Key_Return:
    case Key_Enter:
	moveResizeMode = FALSE;
	releaseMouse();
	releaseKeyboard();
	buttonDown = FALSE;
	break;
    default:
	return;
    }
    QCursor::setPos( pos );
}

bool QWorkspaceChild::focusNextPrevChild( bool next )
{
    QFocusData *f = focusData();

    QWidget *startingPoint = f->home();
    QWidget *candidate = 0;
    QWidget *w = next ? f->next() : f->prev();
    while( !candidate && w != startingPoint ) {
	if ( w != startingPoint &&
	     (w->focusPolicy() & TabFocus) == TabFocus
	     && w->isEnabled() &&!w->focusProxy() && w->isVisible() )
	    candidate = w;
	w = next ? f->next() : f->prev();
    }

    if ( candidate ) {
	QObjectList *ol = queryList();
	bool ischild = ol->findRef( candidate ) != -1;
	delete ol;
	if ( !ischild ) {
	    startingPoint = f->home();
	    QWidget *nw = next ? f->prev() : f->next();
	    QObjectList *ol2 = queryList();
	    QWidget *lastValid = 0;
	    candidate = startingPoint;
	    while ( nw != startingPoint ) {
		if ( ( candidate->focusPolicy() & TabFocus ) == TabFocus
		    && candidate->isEnabled() &&!candidate->focusProxy() && candidate->isVisible() )
		    lastValid = candidate;
		if ( ol2->findRef( nw ) == -1 ) {
		    candidate = lastValid;
		    break;
		}
		candidate = nw;
		nw = next ? f->prev() : f->next();
	    }
	    delete ol2;
	}
    }

    if ( !candidate )
	return FALSE;

    candidate->setFocus();
    return TRUE;
}

void QWorkspaceChild::doResize()
{
    moveResizeMode = TRUE;
    buttonDown = TRUE;
    moveOffset = mapFromGlobal( QCursor::pos() );
    if ( moveOffset.x() < width()/2) {
	if ( moveOffset.y() < height()/2)
	    mode = TopLeft;
	else
	    mode = BottomLeft;
    } else {
	if ( moveOffset.y() < height()/2)
	    mode = TopRight;
	else
	    mode = BottomRight;
    }
    invertedMoveOffset = rect().bottomRight() - moveOffset;
#ifndef QT_NO_CURSOR
    setMouseCursor( mode );
    grabMouse( cursor()  );
#else
    grabMouse();
#endif
    grabKeyboard();
    resizeHorizontalDirectionFixed = FALSE;
    resizeVerticalDirectionFixed = FALSE;
}

void QWorkspaceChild::doMove()
{
    mode = Center;
    moveResizeMode = TRUE;
    buttonDown = TRUE;
    moveOffset = mapFromGlobal( QCursor::pos() );
    invertedMoveOffset = rect().bottomRight() - moveOffset;
#ifndef QT_NO_CURSOR
    grabMouse( SizeAllCursor );
#else
    grabMouse();
#endif
    grabKeyboard();
}

void QWorkspaceChild::setCaption( const QString& cap )
{
    if ( titlebar )
	titlebar->setText( cap );
    QWidget::setCaption( cap );
}

void QWorkspaceChild::internalRaise()
{
    setUpdatesEnabled( FALSE );
    if ( iconw )
	iconw->parentWidget()->raise();
    raise();

    if ( !windowWidget() || windowWidget()->testWFlags( WStyle_StaysOnTop ) ) {
	setUpdatesEnabled( TRUE );
	return;
    }

    QList<QWorkspaceChild> l = ((QWorkspace*)parent())->d->windows;

    for (QWorkspaceChild* c = l.first(); c; c = l.next() ) {
	if ( c->windowWidget() &&
	    !c->windowWidget()->isHidden() &&
	     c->windowWidget()->testWFlags( WStyle_StaysOnTop ) )
	     c->raise();
    }
    setUpdatesEnabled( TRUE );
}

void QWorkspaceChild::move( int x, int y )
{
    int nx = x;
    int ny = y;

    if ( windowWidget() && windowWidget()->testWFlags( WStyle_Tool ) ) {
	int dx = 10;
	int dy = 10;

	if ( QABS( x ) < dx )
	    nx = 0;
	if ( QABS( y ) < dy )
	    ny = 0;
	if ( QABS( x + width() - parentWidget()->width() ) < dx ) {
	    nx = parentWidget()->width() - width();
	    snappedRight = TRUE;
	} else
	    snappedRight = FALSE;

	if ( QABS( y + height() - parentWidget()->height() ) < dy ) {
	    ny = parentWidget()->height() - height();
	    snappedDown = TRUE;
	} else
	    snappedDown = FALSE;
    }
    QFrame::move( nx, ny );
}

QWorkspaceChildTitleLabel::QWorkspaceChildTitleLabel( QWorkspaceChildTitleBar* parent, const char* name )
    : QFrame( parent, name, WRepaintNoErase | WResizeNoErase )
{
    getColors();
}

void QWorkspaceChildTitleLabel::getColors()
{
#ifdef _WS_WIN
    aleftc = arightc = palette().active().highlight();
    ileftc = irightc = palette().inactive().dark();
    atextc = palette().active().highlightedText();
    itextc = palette().inactive().background();
#else
    aleftc = arightc = palette().active().highlight();
    ileftc = irightc = palette().inactive().background();
    atextc = palette().active().highlightedText();
    itextc = palette().inactive().foreground();
#endif

#ifdef _WS_WIN_ // ask system properties on windows
#ifndef SPI_GETGRADIENTCAPTIONS
#define SPI_GETGRADIENTCAPTIONS 0x1008
#endif
#ifndef COLOR_GRADIENTACTIVECAPTION
#define COLOR_GRADIENTACTIVECAPTION 27
#endif
#ifndef COLOR_GRADIENTINACTIVECAPTION
#define COLOR_GRADIENTINACTIVECAPTION 28
#endif
    if ( qt_winver == Qt::WV_98 || qt_winver == WV_2000 ) {
	if ( QApplication::desktopSettingsAware() ) {
	    aleftc = qt_colorref2qrgb(GetSysColor(COLOR_ACTIVECAPTION));
	    ileftc = qt_colorref2qrgb(GetSysColor(COLOR_INACTIVECAPTION));
	    atextc = qt_colorref2qrgb(GetSysColor(COLOR_CAPTIONTEXT));
	    itextc = qt_colorref2qrgb(GetSysColor(COLOR_INACTIVECAPTIONTEXT));

	    arightc = aleftc;
	    irightc = ileftc;

	    BOOL gradient;
	    SystemParametersInfo( SPI_GETGRADIENTCAPTIONS, 0, &gradient, 0 );
	    if ( gradient ) {
		arightc = qt_colorref2qrgb(GetSysColor(COLOR_GRADIENTACTIVECAPTION));
		irightc = qt_colorref2qrgb(GetSysColor(COLOR_GRADIENTINACTIVECAPTION));
	    }
	}
    }
#endif // _WS_WIN_

    setActive( ((QWorkspaceChildTitleBar*)parentWidget())->isActive() );
}

void QWorkspaceChildTitleLabel::setText( const QString& text )
{
    titletext = text;
    cutText();
}

void QWorkspaceChildTitleLabel::cutText()
{
    QFontMetrics fm( font() );

    int maxw = contentsRect().width() - leftm - rightm;

    if ( fm.width( titletext+"m" ) > maxw ) {
	QToolTip::remove( this );
	QToolTip::add( this, titletext );
	int i = titletext.length();
	while ( (fm.width(titletext.left( i ) + "...")  > maxw) && i>0 )
	    i--;
	cuttext = titletext.left( i ) + "...";
	drawLabel();
    } else {
	QToolTip::remove( this );
	cuttext = titletext;
    }
    drawLabel();
}

void QWorkspaceChildTitleLabel::setLeftMargin( int x )
{
    leftm = x;
    cutText();
}

void QWorkspaceChildTitleLabel::setRightMargin( int x )
{
    rightm = x;
    cutText();
}

void QWorkspaceChildTitleLabel::drawLabel()
{
    if ( buffer.isNull() )
	return;

    QPainter p(&buffer);
    p.setFont( font() );

    if ( leftc != rightc ) {
	double rS = leftc.red();
	double gS = leftc.green();
	double bS = leftc.blue();

	double rD = double(rightc.red() - rS) / width();
	double gD = double(rightc.green() - gS) / width();
	double bD = double(rightc.blue() - bS) / width();

	for ( int x = contentsRect().x(); x < contentsRect().width(); x++ ) {
	    rS+=rD;
	    gS+=gD;
	    bS+=bD;
	    p.setPen( QColor( (int)rS, (int)gS, (int)bS ) );
	    p.drawLine( x, contentsRect().y(), x, contentsRect().height() );
	}
    } else {
	p.fillRect( contentsRect(), leftc );
    }
    drawFrame( &p );
    p.setPen( textc );
    QRect cr( 1, 1, width()-2, height()-2);
    p.drawText( cr.x() + leftm, cr.y(),
	cr.width() - rightm, cr.height(),
	AlignLeft | AlignVCenter | SingleLine, cuttext );

    p.end();

    // why does it flicker using update()?
    repaint( FALSE );
}

void QWorkspaceChildTitleLabel::frameChanged()
{
    cutText();
}

void QWorkspaceChildTitleLabel::paintEvent( QPaintEvent* )
{
    bitBlt( this, 0, 0, &buffer, 0, 0, width(), height() );
}

void QWorkspaceChildTitleLabel::resizeEvent( QResizeEvent* e )
{
    QFrame::resizeEvent( e );

    buffer.resize( size() );
    cutText();
}

bool QWorkspaceChildTitleLabel::event( QEvent* e )
{
    if ( e->type() == QEvent::ApplicationPaletteChange ) {
	getColors();
	return TRUE;
    }

    return QFrame::event( e );
}

void QWorkspaceChildTitleLabel::setActive( bool a )
{
    if ( a ) {
	textc = atextc;
	leftc = aleftc;
	rightc = arightc;
    } else {
	textc = itextc;
	leftc = ileftc;
	rightc = irightc;
    }

    drawLabel();
}

QWorkspaceChildTitleButton::QWorkspaceChildTitleButton( QWidget* parent )
    : QLabel( parent )
{
    setAlignment( AlignHCenter | AlignVCenter );
}

void QWorkspaceChildTitleButton::setPixmap( const QPixmap& pm )
{
    if ( pm.height() > 14 || pm.width() > 14 ) {
	QPixmap p;
	p.convertFromImage( pm.convertToImage().smoothScale( 14, 14 ) );
	QLabel::setPixmap( p );
    } else {
	QLabel::setPixmap( pm );
    }
}

QSize QWorkspaceChildTitleButton::sizeHint() const
{
    return QSize( 14,14 );
}

#include "qworkspace.moc"
#endif // QT_NO_WORKSPACE
