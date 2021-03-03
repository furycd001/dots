/****************************************************************************
** $Id: qt/src/widgets/qwidgetstack.cpp   2.3.2   edited 2001-08-03 $
**
** Implementation of QWidgetStack class
**
** Created : 980128
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

#include "qwidgetstack.h"
#ifndef QT_NO_WIDGETSTACK

#include "qobjectlist.h"
#include "qobjectdict.h"
#include "qbutton.h"
#include "qbuttongroup.h"

#include "qapplication.h"

class QWidgetStackPrivate {
public:
    class Invisible: public QWidget
    {
    public:
	Invisible( QWidgetStack * parent ): QWidget( parent )
	{
	    setBackgroundMode( NoBackground );
	}
	const char * className() const
	{
	    return "QWidgetStackPrivate::Invisible";
	}
    };
};

// NOT REVISED
/*! \class QWidgetStack qwidgetstack.h

  \brief The QWidgetStack class provides a stack of widgets, where the
  user can see only the top widget.

  \ingroup organizers

  The application programmer can move any widget to the top of the
  stack at any time using the slot raiseWidget(), and add or remove
  widgets using addWidget() and removeWidget().

  visibleWidget() is the \e get equivalent of raiseWidget(); it
  returns a pointer to the widget that is currently on the top of the
  stack.

  QWidgetStack also provides the ability to manipulate widgets through
  application-specfied integer IDs, and to translate from widget
  pointers to IDs using id() and from IDs to widget pointers using
  widget().  These numeric IDs have and unique (per QWidgetStack, not
  globally) and cannot be -1, but apart from that QWidgetStack does
  not attach any meaning to them.

  The default widget stack is frame-less and propagates its font and
  palette to all its children, but you can use the usual QFrame
  functions (like setFrameStyle()) to add a frame, and use
  setFontPropagation() and setPalettePropagation() to change the
  propagation style.

  Finally, QWidgetStack provides a signal, aboutToShow(), which is
  emitted just before a managed widget is shown.

  \sa QTabDialog QTabBar QFrame
*/


/*!  Constructs an empty widget stack. */

QWidgetStack::QWidgetStack( QWidget * parent, const char *name )
    : QFrame( parent, name )
{
    d = 0;
    dict = new QIntDict<QWidget>;
    focusWidgets = 0;
    topWidget = 0;
    invisible = new QWidgetStackPrivate::Invisible( this );
    setFontPropagation( AllChildren );
    setPalettePropagation( AllChildren );
}


/*! Destructs the object and frees any allocated resources. */

QWidgetStack::~QWidgetStack()
{
    delete focusWidgets;
    focusWidgets = 0;
    delete d;
    d = 0;
    delete dict;
    dict = 0;
}


/*!  Adds \a w to this stack of widgets, with id \a id.

  If \a w is not a child of \c this, QWidgetStack moves it using
  reparent().

  Note that the added children are initially hidden. After you have added the widgets
  you want to the stack, you may want to call raiseWidget() on one of them.
*/

void QWidgetStack::addWidget( QWidget * w, int id )
{
    if ( !w || w == invisible )
	return;

    dict->insert( id+1, w );

    // preserve existing focus
    QWidget * f = w->focusWidget();
    while( f && f != w )
	f = f->parentWidget();
    if ( f ) {
	if ( !focusWidgets )
	    focusWidgets = new QPtrDict<QWidget>( 17 );
	focusWidgets->replace( w, w->focusWidget() );
    }

    w->hide();
    if ( w->parent() != this )
	w->reparent( this, 0, contentsRect().topLeft(), FALSE );
    w->setGeometry( contentsRect() );
}


/*!  Removes \a w from this stack of widgets.  Does not delete \a
  w. If \a w is the currently visible widget, no other widget is
  substituted. \sa visibleWidget() raiseWidget() */

void QWidgetStack::removeWidget( QWidget * w )
{
    if ( !w )
	return;
    int i = id( w );
    if ( i != -1 )
	dict->take( i+1 );
    if ( w == topWidget )
	topWidget = 0;
    if ( dict->isEmpty() )
	invisible->hide(); // let background shine through again
}


/*!  Raises \a id to the top of the widget stack. \sa visibleWidget() */

void QWidgetStack::raiseWidget( int id )
{
    if ( id == -1 )
	return;
    QWidget * w = dict->find( id+1 );
    if ( w )
	raiseWidget( w );
}


/*!  Raises \a w to the top of the widget stack. */

void QWidgetStack::raiseWidget( QWidget * w )
{
    if ( !w || !isMyChild( w ) )
	return;

    topWidget = w;
    if ( !isVisible() )
	return;

    if ( !invisible->isVisible() ) {
	invisible->setGeometry( contentsRect() );
	invisible->lower();
	invisible->show();
	QApplication::sendPostedEvents( invisible, QEvent::ShowWindowRequest );
    }

    // try to move focus onto the incoming widget if focus
    // was somewhere on the outgoing widget.
    QWidget * f = w->focusWidget();
    while ( f && f->parent() != this )
	f = f->parentWidget();
    if ( f && f->parent() == this ) {
	if ( !focusWidgets )
	    focusWidgets = new QPtrDict<QWidget>( 17 );
	focusWidgets->replace( f, f->focusWidget() );
	f->focusWidget()->clearFocus();
	if ( w->focusPolicy() != QWidget::NoFocus ) {
	    f = w;
	} else {
	    // look for the best focus widget we can find
	    // best == what we had (which may be deleted)
	    f = focusWidgets->find( w );
	    if ( f )
		focusWidgets->take( w );
	    // second best == selected button from button group
	    QWidget * fb = 0;
	    // third best == whatever candidate we see first
	    QWidget * fc = 0;
	    bool done = FALSE;
	    const QObjectList * c = w->children();
	    if ( c ) {
		QObjectListIt it( *c );
		QObject * wc;
		while( !done && (wc=it.current()) != 0 ) {
		    ++it;
		    if ( wc->isWidgetType() ) {
			if ( f == wc ) {
			    done = TRUE;
			} else if ( (((QWidget *)wc)->focusPolicy()&QWidget::TabFocus)
				    == QWidget::TabFocus ) {
			    QButton * b = (QButton *)wc;
			    if ( wc->inherits( "QButton" ) &&
				 b->group() && b->isOn() &&
				 b->group()->isExclusive() &&
				 ( fc == 0 ||
				   !fc->inherits( "QButton" ) ||
				   ((QButton*)fc)->group() == b->group() ) )
				fb = b;
			    else if ( !fc )
				fc = (QWidget*)wc;
			}
		    }
		}
		// f exists iff done
		if ( !done ) {
		    if ( fb )
			f = fb;
		    else if ( fc )
			f = fc;
		    else
			f = 0;
		}
	    }
	}
    }

    const QObjectList * c = children();
    QObjectListIt it( *c );
    QObject * o;

    while( (o=it.current()) != 0 ) {
	++it;
	if ( o->isWidgetType() && o != w && o != invisible )
	    ((QWidget *)o)->hide();
    }
    if ( f )
	f->setFocus();

    if ( isVisible() ) {
	emit aboutToShow( w );
	if ( receivers( SIGNAL(aboutToShow(int)) ) ) {
	    int i = id( w );
	    if ( i >= 0 )
		emit aboutToShow( i );
	}
    }

    w->setGeometry( invisible->geometry() );
    w->show();
    QApplication::sendPostedEvents( w, QEvent::ShowWindowRequest );
}


/*!  Returns TRUE if \a w is a child of this widget, else FALSE. */

bool QWidgetStack::isMyChild( QWidget * w )
{
    const QObjectList * c = children();
    if ( !c || !w || w == invisible )
	return FALSE;
    QObjectListIt it( *c );
    QObject * o;

    while( (o=it.current()) != 0 ) {
	++it;
	if ( o->isWidgetType() && o == w )
	    return TRUE;
    }
    return FALSE;
}


/*! \reimp */

void QWidgetStack::frameChanged()
{
    QFrame::frameChanged();
    setChildGeometries();
}


/*! \reimp */

void QWidgetStack::setFrameRect( const QRect & r )
{
    QFrame::setFrameRect( r );
    setChildGeometries();
}


/*!  Fix up the children's geometries. */

void QWidgetStack::setChildGeometries()
{
    invisible->setGeometry( contentsRect() );
    if ( topWidget )
	topWidget->setGeometry( invisible->geometry() );
}


/*! \reimp */
void QWidgetStack::show()
{
    //  Reimplemented in order to set the children's geometries
    //  appropriately.
    if ( !isVisible() && children() ) {
	setChildGeometries();

	const QObjectList * c = children();
	QObjectListIt it( *c );
	QObject * o;

	while( (o=it.current()) != 0 ) {
	    ++it;
	    if ( o->isWidgetType() )
		if ( o == topWidget )
		    ((QWidget *)o)->show();
		else if ( o == invisible && topWidget != 0 )
		    ((QWidget *)o)->show();
		else
		    ((QWidget *)o)->hide();
	}
    }

    QFrame::show();
}


/*!  Returns a pointer to the widget with ID \a id.  If this widget
  stack does not manage a widget with ID \a id, this function returns
  0.

  \sa id() addWidget()
*/

QWidget * QWidgetStack::widget( int id ) const
{
    return id != -1 ? dict->find( id+1 ) : 0;
}


/*!  Returns the ID of the \a widget.  If \a widget is 0 or is not
  being managed by this widget stack, this function returns -1.

  \sa widget() addWidget()
*/

int QWidgetStack::id( QWidget * widget ) const
{
    if ( !widget || !dict )
	return -1;

    QIntDictIterator<QWidget> it( *dict );
    while ( it.current() && it.current() != widget )
	++it;
    return it.current() == widget ? it.currentKey()-1 : -1;
}


/*! Returns a pointer to the currently visible widget (the one on the
  top of the stack), or 0 if nothing is currently being shown.

  \sa aboutToShow() id() raiseWidget()
*/

QWidget * QWidgetStack::visibleWidget() const
{
    return topWidget;
}


/*! \fn void QWidgetStack::aboutToShow( int )

  This signal is emitted just before a managed widget is shown, if
  that managed widget has a non-zero ID.  The argument is the numeric
  ID of the widget.
*/


/*! \fn void QWidgetStack::aboutToShow( QWidget * )

  This signal is emitted just before a managed widget is shown.  The
  argument is a pointer to the widget.
*/


/*! \reimp */

void QWidgetStack::resizeEvent( QResizeEvent * e )
{
    QFrame::resizeEvent( e );
    setChildGeometries();
}


/*! \reimp */

QSize QWidgetStack::sizeHint() const
{
    constPolish();

    QSize size(0,0);
    if ( children() ) {
	const QObjectList * c = children();
	QObjectListIt it( *c );
	QObject * o;

	while( (o=it.current()) != 0 ) {
	    ++it;
	    if ( o->isWidgetType() && o != invisible ) {
		QWidget *w = (QWidget*)o;
		size = size.expandedTo( w->sizeHint() )
		       .expandedTo(w->minimumSize());
	    }
	}
    }
    if ( size.isNull() )
	return QSize(100,50); //### is this a sensible default???
    return QSize( size.width() + 2*frameWidth(), size.height() + 2*frameWidth() );
}


/*! \reimp */
QSize QWidgetStack::minimumSizeHint() const
{
    constPolish();

    QSize size(0,0);
    if ( children() ) {
	const QObjectList * c = children();
	QObjectListIt it( *c );
	QObject * o;

	while( (o=it.current()) != 0 ) {
	    ++it;
	    if ( o->isWidgetType() &&  o != invisible ) {
		    QWidget *w = (QWidget*)o;
		    size = size.expandedTo( w->minimumSizeHint())
					    .expandedTo(w->minimumSize());
	    }
	}
    }
    return QSize( size.width() + 2*frameWidth(), size.height() + 2*frameWidth() );
}

/*! \reimp  */
void QWidgetStack::childEvent( QChildEvent * e)
{
    if ( e->child()->isWidgetType() && e->removed() )
	removeWidget( (QWidget*) e->child() );
}
#endif
