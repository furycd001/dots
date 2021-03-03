/****************************************************************************
** $Id: qt/src/widgets/qtooltip.cpp   2.3.2   edited 2001-07-11 $
**
** Tool Tips (or Balloon Help) for any widget or rectangle
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

#include "qtooltip.h"
#ifndef QT_NO_TOOLTIP
#include "qlabel.h"
#include "qptrdict.h"
#include "qapplication.h"
#include "qguardedptr.h"
#include "qtimer.h"
#include "qeffects_p.h"

static bool globally_enabled = TRUE;

// Magic value meaning an entire widget - if someone tries to insert a
// tool tip on this part of a widget it will be interpreted as the
// entire widget.

static inline QRect entireWidget()
{
    return QRect( -QWIDGETSIZE_MAX, -QWIDGETSIZE_MAX,
		  2*QWIDGETSIZE_MAX, 2*QWIDGETSIZE_MAX );
}

// Internal class - don't touch

class QTipLabel : public QLabel
{
    Q_OBJECT
public:
    QTipLabel(const QString& text) : QLabel( 0, "toolTipTip",
			  WStyle_StaysOnTop +
			  WStyle_Customize + WStyle_NoBorder + WStyle_Tool )
    {
	setMargin(1);
	setIndent(0);
	setAutoMask( FALSE );
	setFrameStyle( QFrame::Plain | QFrame::Box );
	setLineWidth( 1 );
	setAlignment( AlignLeft | AlignTop );
	polish();
	setText(text);
	adjustSize();
    }
};

// Internal class - don't touch

class QTipManager : public QObject
{
    Q_OBJECT
public:
    QTipManager();
   ~QTipManager();

    struct Tip
    {
	QRect		rect;
	QString		text;
	QString	        groupText;
	QToolTipGroup  *group;
	QToolTip       *tip;
	bool	        autoDelete;
	QRect 		geometry;
	Tip	       *next;
    };

    bool    eventFilter( QObject * o, QEvent * e );
    void    add( const QRect &gm, QWidget *, const QRect &, const QString& ,
		 QToolTipGroup *, const QString& , QToolTip *, bool );
    void    add( QWidget *, const QRect &, const QString& ,
		 QToolTipGroup *, const QString& , QToolTip *, bool );
    void    remove( QWidget *, const QRect & );
    void    remove( QWidget * );

    void    removeFromGroup( QToolTipGroup * );

    void    hideTipAndSleep();

public slots:
    void    hideTip();

private slots:
    void    labelDestroyed();
    void    clientWidgetDestroyed();
    void    showTip();
    void    allowAnimation();

private:
    QTimer  wakeUp;
    QTimer  fallAsleep;

    QPtrDict<Tip> *tips;
    QLabel *label;
    QPoint pos;
    QGuardedPtr<QWidget> widget;
    Tip *currentTip;
    Tip *previousTip;
    bool preventAnimation;
    bool isApplicationFilter;
    QTimer *removeTimer;
};


// We have a global, internal QTipManager object

static QTipManager *tipManager	  = 0;
static bool	    initializedTM = FALSE;

static void cleanupTipManager()
{
    delete tipManager;
    tipManager = 0;
    initializedTM = FALSE;
}

static void initTipManager()
{
    if ( !tipManager ) {
	tipManager = new QTipManager;
	CHECK_PTR( tipManager );
    }
    if ( !initializedTM ) {
	initializedTM = TRUE;
	qAddPostRoutine( cleanupTipManager );
    }
}


QTipManager::QTipManager()
    : QObject( 0, "toolTipManager" )
{
    tips = new QPtrDict<QTipManager::Tip>( 313 );
    currentTip = 0;
    previousTip = 0;
    label = 0;
    preventAnimation = FALSE;
    isApplicationFilter = FALSE;
    connect( &wakeUp, SIGNAL(timeout()), SLOT(showTip()) );
    connect( &fallAsleep, SIGNAL(timeout()), SLOT(hideTip()) );
    removeTimer = new QTimer( this );
}


QTipManager::~QTipManager()
{
    if ( isApplicationFilter && !qApp->closingDown() ) {
	qApp->setGlobalMouseTracking( FALSE );
	qApp->removeEventFilter( tipManager );
    }

    if ( tips ) {
	QPtrDictIterator<QTipManager::Tip> i( *tips );
	QTipManager::Tip *t, *n;
	void *k;
	while( (t = i.current()) != 0 ) {
	    k = i.currentKey();
	    ++i;
	    tips->take( k );
	    while ( t ) {
		n = t->next;
		delete t;
		t = n;
	    }
	}
	delete tips;
    }

    delete label;
}

void QTipManager::add( const QRect &gm, QWidget *w,
		       const QRect &r, const QString &s,
		       QToolTipGroup *g, const QString& gs,
		       QToolTip *tt, bool a )
{
    QTipManager::Tip *h = (*tips)[ w ];
    QTipManager::Tip *t = new QTipManager::Tip;
    t->next = h;
    t->tip = tt;
    t->autoDelete = a;
    t->text = s;
    t->rect = r;
    t->groupText = gs;
    t->group = g;
    t->geometry = gm;

    if ( h )
	tips->take( w );
    else
	connect( w, SIGNAL(destroyed()), this, SLOT(clientWidgetDestroyed()) );

    tips->insert( w, t );

    if ( a && t->rect.contains( pos ) && (!g || g->enabled()) ) {
	removeTimer->stop();
	showTip();
    }

    if ( !isApplicationFilter && qApp ) {
	isApplicationFilter = TRUE;
	qApp->installEventFilter( tipManager );
	qApp->setGlobalMouseTracking( TRUE );
    }

    if ( t->group )
	connect( removeTimer, SIGNAL( timeout() ),
		 t->group, SIGNAL( removeTip() ) );
}

void QTipManager::add( QWidget *w, const QRect &r, const QString &s,
		       QToolTipGroup *g, const QString& gs,
		       QToolTip *tt, bool a )
{
    add( QRect( -1, -1, -1, -1 ), w, r, s, g, gs, tt, a );
}


void QTipManager::remove( QWidget *w, const QRect & r )
{
    QTipManager::Tip *t = (*tips)[ w ];
    if ( t == 0 )
	return;

    if ( t == currentTip )
	hideTip();

    if ( t == previousTip )
	previousTip = 0;

    if ( t->rect == r ) {
	tips->take( w );
	if ( t->next )
	    tips->insert( w, t->next );
	delete t;
    } else {
	while( t->next && t->next->rect != r )
	    t = t->next;
	if ( t->next ) {
	    QTipManager::Tip *d = t->next;
	    t->next = t->next->next;
	    delete d;
	}
    }
#if 0 // not needed, leads sometimes to crashes
    if ( tips->isEmpty() ) {
	// the manager will be recreated if needed
	delete tipManager;
	tipManager = 0;
    }
#endif
}


/*!
  The label was destroyed in the program cleanup phase.
*/

void QTipManager::labelDestroyed()
{
    label = 0;
}


/*!
  Remove sender() from the tool tip data structures.
*/

void QTipManager::clientWidgetDestroyed()
{
    const QObject *s = sender();
    if ( s )
	remove( (QWidget*) s );
}


void QTipManager::remove( QWidget *w )
{
    QTipManager::Tip *t = (*tips)[ w ];
    if ( t == 0 )
	return;

    tips->take( w );
    QTipManager::Tip * d;
    while ( t ) {
	if ( t == currentTip )
	    hideTip();
	d = t->next;
	delete t;
	t = d;
    }

#if 0
    if ( tips->isEmpty() ) {
	delete tipManager;
	tipManager = 0;
    }
#endif
}


void QTipManager::removeFromGroup( QToolTipGroup *g )
{
    QPtrDictIterator<QTipManager::Tip> i( *tips );
    QTipManager::Tip *t;
    while( (t = i.current()) != 0 ) {
	++i;
	while ( t ) {
	    if ( t->group == g ) {
		if ( t->group )
		    disconnect( removeTimer, SIGNAL( timeout() ),
				t->group, SIGNAL( removeTip() ) );
		t->group = 0;
	    }
	    t = t->next;
	}
    }
}



bool QTipManager::eventFilter( QObject *obj, QEvent *e )
{
    // avoid dumping core in case of application madness, and return
    // quickly for some common but irrelevant events
    if ( !qApp || !qApp->activeWindow() ||
	 !obj || !obj->isWidgetType() || // isWidgetType() catches most stuff
	 !e ||
	 e->type() == QEvent::Paint ||
	 e->type() == QEvent::Timer ||
	 e->type() == QEvent::SockAct ||
	 !tips )
	return FALSE;
    QWidget *w = (QWidget *)obj;

    if ( e->type() == QEvent::FocusOut || e->type() == QEvent::FocusIn ) {
	// user moved focus somewhere - hide the tip and sleep
	hideTipAndSleep();
	return FALSE;
    }

    QTipManager::Tip *t = 0;
    while( w && !t ) {
	t = (*tips)[ w ];
	if ( !t )
	    w = w->isTopLevel() ? 0 : w->parentWidget();
    }

    if ( !t ) {
	if ( ( e->type() >= QEvent::MouseButtonPress &&
	       e->type() <= QEvent::FocusOut) || e->type() == QEvent::Leave )
	    hideTip();
	return FALSE;
    }

    // with that out of the way, let's get down to action

    switch( e->type() ) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
	// input - turn off tool tip mode
	hideTipAndSleep();
	break;
    case QEvent::MouseMove:
	{ // a whole scope just for one variable
	    QMouseEvent * m = (QMouseEvent *)e;
	    QPoint mousePos = w->mapFromGlobal( m->globalPos() );

	    if ( currentTip && !currentTip->rect.contains( mousePos ) ) {
		hideTip();
		if ( m->state() == 0 )
		    return FALSE;
	    }

	    wakeUp.stop();
	    if ( m->state() == 0 &&
		    mousePos.x() >= 0 && mousePos.x() < w->width() &&
		    mousePos.y() >= 0 && mousePos.y() < w->height() ) {
		if ( label && label->isVisible() ) {
		    return FALSE;
		} else {
		    if ( fallAsleep.isActive() ) {
			wakeUp.start( 1, TRUE );
		    } else {
			previousTip = 0;
			wakeUp.start( 700, TRUE );
		    }
		    if ( t->group && t->group->ena &&
			 !t->group->del && !t->groupText.isEmpty() ) {
			removeTimer->stop();
			emit t->group->showTip( t->groupText );
		    }
		}
		widget = w;
		pos = mousePos;
		return FALSE;
	    } else {
		hideTip();
	    }
	}
	break;
    case QEvent::Leave:
    case QEvent::Hide:
    case QEvent::Destroy:
	if ( w == widget )
	    hideTip();
	break;
    default:
	break;
    }
    return FALSE;
}



void QTipManager::showTip()
{
    if ( !widget || !globally_enabled )
	return;

    QTipManager::Tip *t = (*tips)[ widget ];
    while ( t && !t->rect.contains( pos ) )
	t = t->next;
    if ( t == 0 )
	return;

    if (  t == currentTip )
	return; // nothing to do

    if ( t->tip ) {
	t->tip->maybeTip( pos );
	return;
    }

    if ( t->group && !t->group->ena )
	return;

    if ( label ) {
	label->setText( t->text );
	label->adjustSize();
	if ( t->geometry != QRect( -1, -1, -1, -1 ) )
	    label->resize( t->geometry.size() );
    } else {
	label = new QTipLabel(t->text);
	if ( t->geometry != QRect( -1, -1, -1, -1 ) )
	    label->resize( t->geometry.size() );
	CHECK_PTR( label );
	connect( label, SIGNAL(destroyed()), SLOT(labelDestroyed()) );
    }
    QPoint p;
    if ( t->geometry == QRect( -1, -1, -1, -1 ) ) {
	p = widget->mapToGlobal( pos ) + QPoint( 2, 16 );
    } else {
	p = widget->mapToGlobal( t->geometry.topLeft() );
	label->setAlignment( WordBreak | AlignCenter );
	int h = label->heightForWidth( t->geometry.width() - 4 );
	label->resize( label->width(), h );
    }
    if ( p.x() + label->width() > QApplication::desktop()->width() )
	p.setX( QApplication::desktop()->width() - label->width() );
    if ( p.y() + label->height() > QApplication::desktop()->height() )
	p.setY( p.y() - 20 - label->height() );
    if ( label->text().length() ) {
	label->move( p );

#ifndef QT_NO_EFFECTS
	if ( QApplication::isEffectEnabled( UI_AnimateTooltip ) == FALSE ||
	     previousTip || preventAnimation )
	    label->show();
	else if ( QApplication::isEffectEnabled( UI_FadeTooltip ) )
	    qFadeEffect( label );
	else
	    qScrollEffect( label );
#else
	label->show();
#endif

	label->raise();
	fallAsleep.start( 10000, TRUE );
    }

    if ( t->group && t->group->del && !t->groupText.isEmpty() ) {
	removeTimer->stop();
	emit t->group->showTip( t->groupText );
    }

    currentTip = t;
    previousTip = 0;
}


void QTipManager::hideTip()
{
    QTimer::singleShot( 250, this, SLOT(allowAnimation()) );
    preventAnimation = TRUE;

    if ( label && label->isVisible() ) {
	label->hide();
	fallAsleep.start( 2000, TRUE );
	wakeUp.stop();
	if ( currentTip && currentTip->group )
	    removeTimer->start( 100, TRUE );
    } else if ( wakeUp.isActive() ) {
	wakeUp.stop();
	if ( currentTip && currentTip->group &&
	     !currentTip->group->del && !currentTip->groupText.isEmpty() )
	    removeTimer->start( 100, TRUE );
    }

    previousTip = currentTip;
    currentTip = 0;
    if ( previousTip && previousTip->autoDelete )
	remove( widget, previousTip->rect );
    if ( !widget.isNull() )	// QGuardedPtr::operator=() is costly in 2.3
	widget = 0;
}

void  QTipManager::hideTipAndSleep()
{
    hideTip();
    fallAsleep.stop();
}


void QTipManager::allowAnimation()
{
    preventAnimation = FALSE;
}


// NOT REVISED
/*!
  \class QToolTip qtooltip.h

  \brief The QToolTip class provides tool tips (sometimes called
  balloon help) for any widget or rectangular part of a widget.

  \ingroup helpsystem

  The tip is a short, one-line text reminding the user of the widget's
  or rectangle's function.  It is drawn immediately below the region,
  in a distinctive black on yellow combination.  In Motif style, Qt's
  tool tips look much like Motif's but feel more like Windows 95 tool
  tips.

  QToolTipGroup provides a way for tool tips to display another text
  elsewhere (most often in a status bar).

  At any point in time, QToolTip is either dormant or active.  In
  dormant mode the tips are not shown, and in active mode they are.
  The mode is global, not particular to any one widget.

  QToolTip switches from dormant to active mode when the user lets the
  mouse rest on a tip-equipped region for a second or so, and remains
  in active mode until the user either clicks a mouse button, presses
  a key, lets the mouse rest for five seconds, or moves the mouse
  outside \e all tip-equpped regions for at least a second.

  The QToolTip class can be used in three different ways: <ol> <li>
  Adding a tip to an entire widget. <li> Adding a tip to a fixed
  rectangle within a widget. <li> Adding a tip to a dynamic rectangle
  within a widget. </ol>

  To add a tip to a widget, call the \e static function QToolTip::add()
  with the widget and tip as arguments:

  \code
    QToolTip::add( quitButton, "Leave the application" );
  \endcode

  This is the simplest and most common use of QToolTip.  The tip will
  be deleted automatically when \e quitButton is deleted, but you can
  remove it yourself, too:

  \code
    QToolTip::remove( quitButton );
  \endcode

  You can also display another text (typically in a \link QStatusBar
  status bar),\endlink courtesy of QToolTipGroup.  This example
  assumes that \e g is a <code>QToolTipGroup *</code> and already
  connected to the appropriate status bar:

  \code
    QToolTip::add( quitButton, "Leave the application", g,
		   "Leave the application, without asking for confirmation" );
    QToolTip::add( closeButton, "Close this window", g,
		   "Close this window, without asking for confirmation" );
  \endcode

  To add a tip to a fixed rectangle within a widget, call the static
  function QToolTip::add() with the widget, rectangle and tip as
  arguments.  (See the tooltip/tooltip.cpp example.)  Again, you can supply a
  QToolTipGroup * and another text if you want.

  Both of the above are one-liners and cover the vast majority of
  cases.  The third and most general way to use QToolTip uses a pure
  virtual function to decide whether to pop up a tool tip.  The
  tooltip/tooltip.cpp example demonstrates this too.  This mode can be
  used to implement e.g. tips for text that can move as the user
  scrolls.

  To use QToolTip like this, you need to subclass QToolTip and
  reimplement maybeTip().  maybeTip() will be called when there's a
  chance that a tip should pop up.  It must decide whether to show a
  tip, and possibly call add() with the rectangle the tip applies to,
  the tip's text and optionally the QToolTipGroup details.  The tip
  will disappear once the mouse moves outside the rectangle you
  supply, and \e not \e reappear - maybeTip() will be called again if
  the user lets the mouse rest within the same rectangle again.  You
  can forcibly remove the tip by calling remove() with no arguments.
  This is handy if the widget scrolls.

  Tooltips can be globally disabled using QToolTip::setEnabled(), or
  disabled in groups with QToolTipGroup::setEnabled().

  \sa QStatusBar QWhatsThis QToolTipGroup
  <a href="guibooks.html#fowler">GUI Design Handbook: Tool Tip</a>
*/


/*
  Global settings for tool tips.
*/

void QToolTip::initialize() // ## remove 3.0
{
}

void QToolTip::cleanup() // ## remove 3.0
{
}


/*!
  Returns the font common to all tool tips.
  \sa setFont()
*/

QFont QToolTip::font()
{
    QTipLabel l("");
    return QApplication::font( &l );
}


/*!
  Sets the font for all tool tips to \a font.
  \sa font()
*/

void QToolTip::setFont( const QFont &font )
{
    QApplication::setFont( font, TRUE, "QTipLabel" );
}


/*!
  Returns the palette common to all tool tips.
  \sa setPalette()
*/

QPalette QToolTip::palette()
{
    QTipLabel l("");
    return QApplication::palette( &l );
}


/*!
  Sets the palette for all tool tips to \a palette.
  \sa palette()
*/

void QToolTip::setPalette( const QPalette &palette )
{
    QApplication::setPalette( palette, TRUE, "QTipLabel" );
}

/*!
  Constructs a tool tip object.  This is necessary only if you need tool
  tips on regions that can move within the widget (most often because
  the widget's contents can scroll).

  \a parent is the widget you want to add dynamic tool tips to and \a
  group (optional) is the tool tip group they should belong to.

  \sa maybeTip().
*/

QToolTip::QToolTip( QWidget * parent, QToolTipGroup * group )
{
    p = parent;
    g = group;
    initTipManager();
    tipManager->add( p, entireWidget(),
		     QString::null, g, QString::null, this, FALSE );
}


/*!
  Adds a tool tip to \e widget.  \e text is the text to be shown in
  the tool tip.  QToolTip makes a deep copy of this string.

  This is the most common entry point to the QToolTip class; it is
  suitable for adding tool tips to buttons, check boxes, combo boxes
  and so on.
*/

void QToolTip::add( QWidget *widget, const QString &text )
{
    initTipManager();
    tipManager->add( widget, entireWidget(),
		     text, 0, QString::null, 0, FALSE );
}


/*!
  Adds a tool tip to \a widget, and to tool tip group \a group.

  \e text is the text shown in the tool tip and \a longText is the
  text emitted from \a group.  QToolTip makes deep copies of both
  strings.

  Normally, \a longText is shown in a status bar or similar.
*/

void QToolTip::add( QWidget *widget, const QString &text,
		    QToolTipGroup *group, const QString& longText )
{
    initTipManager();
    tipManager->add( widget, entireWidget(), text, group, longText, 0, FALSE );
}


/*!
  Remove the tool tip from \e widget.

  If there are more than one tool tip on \a widget, only the one
  covering the entire widget is removed.
*/

void QToolTip::remove( QWidget * widget )
{
    if ( tipManager )
	tipManager->remove( widget, entireWidget() );
}

/*!
  Adds a tool tip to a fixed rectangle within \a widget.  \a text is
  the text shown in the tool tip.  QToolTip makes a deep copy of this
  string.
*/

void QToolTip::add( QWidget * widget, const QRect & rect, const QString &text )
{
    initTipManager();
    tipManager->add( widget, rect, text, 0, QString::null, 0, FALSE );
}


/*!
  Adds a tool tip to an entire \a widget, and to tool tip group \a
  group.

  \e text is the text shown in the tool tip and \a longText is the
  text emitted from \a group.  QToolTip makes deep copies of both
  strings.

  Normally, \a longText is shown in a status bar or similar.
*/

void QToolTip::add( QWidget *widget, const QRect &rect,
		    const QString& text,
		    QToolTipGroup *group, const QString& groupText )
{
    initTipManager();
    tipManager->add( widget, rect, text, group, groupText, 0, FALSE );
}


/*!
  Remove the tool tip for \e rect from \e widget.

  If there are more than one tool tip on \a widget, only the one
  covering rectangle \e rect is removed.
*/

void QToolTip::remove( QWidget * widget, const QRect & rect )
{
    if ( tipManager )
	tipManager->remove( widget, rect );
}


/*!
  Hides any tip that is currently being shown.

  Normally, there is no need to call this function; QToolTip takes
  care of showing and hiding the tips as the user moves the mouse.
*/

void QToolTip::hide()
{
    if ( tipManager )
	tipManager->hideTipAndSleep();
}

/*!
  \fn virtual void QToolTip::maybeTip( const QPoint & p);

  This pure virtual function is half of the most versatile interface
  QToolTip offers.

  It is called when there is a chance that a tool tip should be shown,
  and must decide whether there is a tool tip for the point \a p in
  the widget this QToolTip object relates to.

  \a p is given in that widget's local coordinates.  Most maybeTip()
  implementation will be of the form:

  \code
    if ( <something> ) {
	tip( <something>, <something> );
    }
  \endcode

  The first argument to tip() (a rectangle) should include the \a p,
  or QToolTip, the user or both can be confused.

  \sa tip()
*/


/*!
  Pops up a tip saying \a text right now, and removes that tip once
  the cursor moves out of rectangle \a rect (which is given in the
  coordinate system of the widget this QToolTip relates to).

  The tip will not come back if the cursor moves back; your maybeTip()
  has to reinstate it each time.
*/

void QToolTip::tip( const QRect & rect, const QString &text )
{
    initTipManager();
    tipManager->add( parentWidget(), rect, text, 0, QString::null, 0, TRUE );
}

void QToolTip::tip( const QRect &geometry, const QRect &rect, const QString &text )
{
    initTipManager();
    tipManager->add( geometry, parentWidget(), rect, text, 0, QString::null, 0, TRUE );
}

/*!
  Pops up a tip saying \a text right now, and removes that tip once
  the cursor moves out of rectangle \a rect.

  The tip will not come back if the cursor moves back; your maybeTip()
  has to reinstate it each time.
*/

void QToolTip::tip( const QRect & rect, const QString &text,
		    const QString& groupText )
{
    initTipManager();
    tipManager->add( parentWidget(), rect, text, group(), groupText, 0, TRUE );
}


/*!
  Removes all tool tips for this tooltip's parent widget immediately.
*/

void QToolTip::clear()
{
    if ( tipManager )
	tipManager->remove( parentWidget() );
}


/*!
  \fn QWidget * QToolTip::parentWidget() const

  Returns the widget this QToolTip applies to.

  The tool tip is destroyed automatically when the parent widget is
  destroyed.

  \sa group()
*/


/*!
  \fn QToolTipGroup * QToolTip::group() const

  Returns the tool tip group this QToolTip is a member of, of 0 if it
  isn't a member of any group.

  The tool tip group is the object responsible for relaying contact
  between tool tips and a status bar or something else which can show
  a longer help text.

  \sa parentWidget(), QToolTipGroup
*/


/*!
  \class QToolTipGroup qtooltip.h

  \brief The QToolTipGroup class collects tool tips into natural groups.

  \ingroup helpsystem

  Tool tips can display \e two texts, the one in the tip and
  optionally another one, typically in a status bar.  QToolTipGroup
  provides a way to link tool tips to this status bar.

  QToolTipGroup has practically no API, it is only used as an argument
  to QToolTip's member functions, for example like this:

  \code
    QToolTipGroup * g = new QToolTipGroup( this, "tool tip relay" );
    connect( g, SIGNAL(showTip(const QString&)),
	     myLabel, SLOT(setText(const QString&)) );
    connect( g, SIGNAL(removeTip()),
	     myLabel, SLOT(clear()) );
    QToolTip::add( giraffeButton, "feed giraffe",
		   g, "Give the giraffe a meal" );
    QToolTip::add( gorillaButton, "feed gorilla",
		   g, "Give the gorilla a meal" );
  \endcode

  This example makes the object myLabel (which you have to supply)
  display (one assumes, though you can make myLabel do anything, of
  course) the strings "Give the giraffe a meal" and "Give the gorilla
  a meal" while the relevant tool tips are being displayed.

  Deleting a tool tip group removes the tool tips in it.
*/

/*! \fn void QToolTipGroup::showTip (const QString &longText)

  This signal is emitted when one of the tool tips in the group is
  displayed.  \a longText is the supplementary text for the displayed
  tool tip.

  \sa removeTip()
*/

/*! \fn void QToolTipGroup::removeTip ()

  This signal is emitted when a tool tip in this group is hidden.  See
  the QToolTipGroup documentation for an example of use.

  \sa showTip()
*/


/*!
  Constructs a tool tip group.
*/

QToolTipGroup::QToolTipGroup( QObject *parent, const char *name )
    : QObject( parent, name )
{
    del = TRUE;
    ena = TRUE;
}


/*!
  Destroy this tool tip groups and all tool tips in it.
*/

QToolTipGroup::~QToolTipGroup()
{
    if ( tipManager )
	tipManager->removeFromGroup( this );
}


/*!  Returns TRUE if the group text is shown delayed (at the same time
as the tip) and FALSE if it is shown immediately.

\sa setDelay()
*/

bool QToolTipGroup::delay() const
{
    return del;
}


/*!  Sets the group to show its text immediately if \a enable is
FALSE, and delayed (at the same time as the tip text) if \a enable is
TRUE.  The default is TRUE.

\sa delay()
*/

void QToolTipGroup::setDelay( bool enable )
{
#if 0
    if ( enable && !del ) {
	// maybe we should show the text at once?
    }
#endif
    del = enable;
}

/*!
  Sets the group to be enabled (all tool tips in the group show
  when activated),
  or disabled (tool tips in the group are never shown).

  \sa QToolTip::setEnabled(), enabled()
*/

void QToolTipGroup::setEnabled( bool enable )
{
    ena = enable;
}

/*!
  Returns whether tooltips in the group are enabled.
  \sa setEnabled()
*/
bool QToolTipGroup::enabled() const
{
    return (bool)ena;
}

/*!
  Sets the all tool tips to be enabled (shown when needed)
  or disabled (never shown).

  By default, tool tips are enabled. Note that this function
  effects all tooltips in the entire application.

  \sa QToolTipGroup::setEnabled()
*/

void QToolTip::setEnabled( bool enable )
{
    globally_enabled = enable;
}

/*!
  Returns whether tooltips are enabled globally.
  \sa setEnabled()
*/
bool QToolTip::enabled()
{
    return globally_enabled;
}

#include "qtooltip.moc"
#endif
