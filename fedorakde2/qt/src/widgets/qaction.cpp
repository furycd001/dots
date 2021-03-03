/****************************************************************************
** $Id: qt/src/widgets/qaction.cpp   2.3.2   edited 2001-09-15 $
**
** Implementation of QAction class
**
** Created : 000000
**
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
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

#include "qaction.h"

#ifndef QT_NO_ACTION

#include <qtoolbar.h>
#include <qlist.h>
#include <qpopupmenu.h>
#include <qaccel.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qstatusbar.h>
#include <qobjectlist.h>


/*!
  \class QAction qaction.h

  \brief The QAction class abstracts a user interface action that can
  appear both in menus and tool bars.

  There are two basic kind of user interface actions, command actions
  and options. QAction usually models a command action, for example
  "open file". When the actual action shall be performed, it emits the
  activated() signal. Options, for example the drawing tools in a
  paint program, are represented by toggle actions (see
  setToggleAction() ). A toggle action emits a toggled() signal
  whenever it changes state. Several toggle actions can be combined in
  a QActionGroup.

  To provide an action to the user, use addTo() to add it to either a
  menu or a tool bar, for example:
  \code
  QPopupMenu* popup = new QPopupMenu;
  QAction* myAction= new QAction;
  myAction->setText( "MyAction" );
  myAction->addTo( popup );
  \endcode

  You can add an action to an arbitrary number of menus and toolbars
  and remove it again with removeFrom().

  Changing an action's properties, for example using setEnabled(),
  setOn() or setText(), immediately shows up in all
  representations. Other properties that define the way an action is
  presented to the user are iconSet(), menuText(), toolTip(),
  statusTip() and whatsThis().

  An action may also be triggered by an accelerator key declared with
  setAccel(). Since accelerators are window specific, the application
  window has to be an ancestor of the action. Generally, it is
  therefore a good idea to always create actions as direct children of
  the main window.

*/


class QActionPrivate
{
public:
    QActionPrivate();
    ~QActionPrivate();
    QIconSet *iconset;
    QString text;
    QString menutext;
    QString tooltip;
    QString statustip;
    QString whatsthis;
    int key;
#ifndef QT_NO_ACCEL
    QAccel* accel;
    int accelid;
#endif
    uint enabled : 1;
    uint toggleaction :1;
    uint on : 1;
    QToolTipGroup* tipGroup;

    struct MenuItem {
	MenuItem():popup(0),id(0){}
	QPopupMenu* popup;
	int id;
    };
    QList<MenuItem> menuitems;
    QList<QToolButton> toolbuttons;

    enum Update { Everything, Icons, State }; // Everything means everything but icons and state
    void update( Update upd = Everything );

    QString menuText() const;
    QString toolTip() const;
    QString statusTip() const;
};

QActionPrivate::QActionPrivate()
{
    iconset = 0;
#ifndef QT_NO_ACCEL
    accel = 0;
    accelid = 0;
#endif
    key = 0;
    enabled = 1;
    toggleaction  = 0;
    on = 0;
    menuitems.setAutoDelete( TRUE );
    tipGroup = new QToolTipGroup( 0 );
}

QActionPrivate::~QActionPrivate()
{
    QListIterator<QToolButton> ittb( toolbuttons );
    QToolButton *tb;

    while ( ( tb = ittb.current() ) ) {
	++ittb;
	QWidget* parent = tb->parentWidget();
	delete tb;
	if ( parent->inherits( "QToolBar" ) ) {
	    QToolBar* toolbar = (QToolBar*) parent;
        QObjectList* lst = toolbar->queryList( "QToolButton" );
        if ( lst->isEmpty() )
		    delete toolbar;
        delete lst;
	}
    }

    QListIterator<QActionPrivate::MenuItem> itmi( menuitems);
    QActionPrivate::MenuItem* mi;
    while ( ( mi = itmi.current() ) ) {
	++itmi;
	QPopupMenu* menu = mi->popup;
	if ( menu->findItem( mi->id ) )
	    menu->removeItem( mi->id );
	if ( !menu->count() ) {
	    delete menu;
	}
    }

#ifndef QT_NO_ACCEL
    delete accel;
#endif
    delete iconset;
    delete tipGroup;
}

void QActionPrivate::update( Update upd )
{
    for ( QListIterator<MenuItem> it( menuitems); it.current(); ++it ) {
	MenuItem* mi = it.current();
	QString t = menuText();
#ifndef QT_NO_ACCEL
	if ( key )
	    t += '\t' + QAccel::keyToString( key );
#endif
	switch ( upd ) {
	case State:
	    mi->popup->setItemEnabled( mi->id, enabled );
	    if ( toggleaction )
		mi->popup->setItemChecked( mi->id, on );
	    break;
	case Icons:
	    if ( iconset )
		mi->popup->changeItem( mi->id, *iconset, t );
	    break;
	default:
	    mi->popup->changeItem( mi->id, t );
	    if ( !whatsthis.isEmpty() )
		mi->popup->setWhatsThis( mi->id, whatsthis );
	    if ( toggleaction ) {
		mi->popup->setCheckable( TRUE );
		mi->popup->setItemChecked( mi->id, on );
	    }
	}
    }
    for ( QListIterator<QToolButton> it2( toolbuttons); it2.current(); ++it2 ) {
	QToolButton* btn = it2.current();
	switch ( upd ) {
	case State:
	    btn->setEnabled( enabled );
	    if ( toggleaction )
		btn->setOn( on );
	    break;
	case Icons:
	    if ( iconset )
		btn->setIconSet( *iconset );
	    break;
	default:
	    btn->setToggleButton( toggleaction );
	    if ( !text.isEmpty() )
		btn->setTextLabel( text, FALSE );
	    QToolTip::remove( btn );
	    QToolTip::add( btn, toolTip(), tipGroup, statusTip() );
	    QWhatsThis::remove( btn );
	    if ( !whatsthis.isEmpty() )
		QWhatsThis::add( btn, whatsthis );
	}
    }
}

QString QActionPrivate::menuText() const
{
    if ( menutext.isNull() )
	return text;
    return menutext;
}

QString QActionPrivate::toolTip() const
{
    if ( tooltip.isNull() ) {
#ifndef QT_NO_ACCEL
	if ( accel )
	    return text + " (" + QAccel::keyToString( accel->key( accelid )) + ")";
#endif
	return text;
    }
    return tooltip;
}

QString QActionPrivate::statusTip() const
{
    if ( statustip.isNull() )
	return toolTip();
    return statustip;
}



/*!
  Constructs an action with parent \a parent and name \a name.

  If \a toggle is TRUE, the action becomes a toggle action.

  If the parent is a QActionGroup, the action automatically becomes a
  member of it.

  Note: for accelerators to work, the parent or one of its ancestors
  has to be the application window.
 */
QAction::QAction( QObject* parent, const char* name, bool toggle )
    : QObject( parent, name )
{
    init();
    d->toggleaction = toggle;
}


/*!\overload
  Constructs an action with text \a text, icon \a icon, menu text
  \a menuText, a keyboard accelerator \a accel, a \a parent and name
  \a name. \a text will also show up in tooltips, unless you call
  setToolTip() with a different tip later.

  If the parent is a QActionGroup, the action automatically becomes a
  member of it.

  Note: for accelerators to work, the parent or one of its ancestors
  has to be the application window.
 */
QAction::QAction( const QString& text, const QIconSet& icon, const QString& menuText, int accel, QObject* parent, const char* name, bool toggle )
    : QObject( parent, name )
{
    init();
    d->toggleaction = toggle;
    if ( !icon.pixmap().isNull() )
	setIconSet( icon );
    d->text = text;
    d->menutext = menuText;
    setAccel( accel );
}

/*!\overload
  Constructs an action with text \a text, menu text \a
  menuText, a keyboard accelerator \a accel, a \a parent and name \a
  name. \a text will also show up in tooltips, unless you call
  setToolTip() with a different tip later.

  If \a toggle is TRUE, the action becomes a toggle action.

  If the parent is a QActionGroup, the action automatically becomes a
  member of it.

  Note: for accelerators to work, the parent or one of its ancestors
  has to be the application window.
 */
QAction::QAction( const QString& text, const QString& menuText, int accel, QObject* parent, const char* name, bool toggle )
    : QObject( parent, name )
{
    init();
    d->toggleaction = toggle;
    d->text = text;
    d->menutext = menuText;
    setAccel( accel );
}


void QAction::init()
{
    d = new QActionPrivate;
    if ( parent() && parent()->inherits("QActionGroup") ) {
	((QActionGroup*) parent())->insert( this );		// insert into action group
    }
}

/*! Destroys the object and frees any allocated resources. */

QAction::~QAction()
{
    delete d;
}

/*!
  Sets the icon set to \a icon.

  \sa iconSet();
 */
void QAction::setIconSet( const QIconSet& icon )
{
    register QIconSet *i = d->iconset;
    d->iconset = new QIconSet( icon );
    delete i;
    d->update( QActionPrivate::Icons );
}

/*!
  Returns the icon set.

  \sa setIconSet();
 */
QIconSet QAction::iconSet() const
{
    if ( d->iconset )
	return *d->iconset;
    return QIconSet();
}

/*!
  Sets the text to \a text.

  \sa setMenuText(), text()
 */
void QAction::setText( const QString& text )
{
    d->text = text;
    d->update();
}

/*!
  Returns the current text.

  \sa setText(), menuText()
 */
QString QAction::text() const
{
    return d->text;
}


/*!
  Sets a special text \a text for menu items. Use this to specify
  ellipses or keyboard shortcuts that should not show up in tooltips or
  as button text.

  \sa setText(), menuText()
 */
void QAction::setMenuText( const QString& text )
{
    d->menutext = text;
    d->update();
}

/*!
  Returns the text used for menu items.

  If no menu text has been defined yet, this is the same as text().

  \sa setMenuText(),  text()
 */
QString QAction::menuText() const
{
    return d->menuText();
}

/*!
  Sets the tool tip to \a tip.

  \sa toolTip()
 */
void QAction::setToolTip( const QString& tip )
{
    d->tooltip = tip;
    d->update();
}

/*!
  Returns the current tool tip.

  If no tool tip has been defined yet, it returns text
  and a hotkey hint.

  \sa setToolTip(), text()
*/
QString QAction::toolTip() const
{
    return d->toolTip();
}

/*!
  Sets the status tip to \a tip. The tip will be displayed on
  all status bars the topmost parent of the action provides.

  \sa statusTip()
*/
void QAction::setStatusTip( const QString& tip )
{
    d->statustip = tip;
    d->update();
}

/*!
  Returns the current status tip.

  If not status tip has been defined yet, this is the same as toolTip()

  \sa setStatusTip(), toolTip()
*/
QString QAction::statusTip() const
{
    return d->statusTip();
}

/*!
  Sets What's This help to \a whatsThis.

  \sa whatsThis()
 */
void QAction::setWhatsThis( const QString& whatsThis )
{
    d->whatsthis = whatsThis;
#ifndef QT_NO_ACCEL
    if ( !d->whatsthis.isEmpty() && d->accel )
	d->accel->setWhatsThis( d->accelid, d->whatsthis );
#endif
    d->update();
}

/*!
  Returns the What's This help for this action.

  \sa setWhatsThis()
 */
QString QAction::whatsThis() const
{
    return d->whatsthis;
}


/*!
  Sets the action's accelerator to \a key.

  Note: For accelerators to work, the parent or one of its ancestors
  has to be the application window.

  \sa accel()
 */
void QAction::setAccel( int key )
{
    d->key = key;
#ifndef QT_NO_ACCEL
    delete d->accel;
    d->accel = 0;
#endif

    if ( !key ) {
	d->update();
	return;
    }

#ifndef QT_NO_ACCEL
    QObject* p = parent();
    while ( p && !p->isWidgetType() ) {
	p = p->parent();
    }
    if ( p ) {
	d->accel = new QAccel( (QWidget*)p, 0, "qt_action_accel" );
	d->accelid = d->accel->insertItem( d->key );
	d->accel->connectItem( d->accelid, this, SLOT( internalActivation() ) );
	if ( !d->whatsthis.isEmpty() )
	    d->accel->setWhatsThis( d->accelid, d->whatsthis );
    }
#if defined(CHECK_STATE)
    else
	qWarning( "QAction::setAccel()  (%s) requires widget in parent chain.", name( "unnamed" ) );
#endif
#endif
    d->update();
}


/*!
  Returns the acceleration key.

  \sa setAccel()
 */
int QAction::accel() const
{
    return d->key;
}


/*!
  Makes the action a toggle action if \e enable is TRUE, or a
  normal action if \e enable is FALSE.

  You may want to add toggle actions to a QActionGroup for exclusive
  toggling.

  \sa isToggleAction()
 */
void QAction::setToggleAction( bool enable )
{
    if ( enable == (bool)d->toggleaction )
	return;
    d->toggleaction = enable;
    d->update();
}

/*!
  Returns whether the action is a toggle action or not.

  \sa setToggleAction()
 */
bool QAction::isToggleAction() const
{
    return d->toggleaction;
}

/*!
  Switches a toggle action on if \e enable is TRUE or off if \e enable is
  FALSE.

  This function should be called only for toggle actions.

  \sa isOn(), setToggleAction()
 */
void QAction::setOn( bool enable )
{
    if ( !isToggleAction() ) {
#if defined(CHECK_STATE)
	qWarning( "QAction::setOn() (%s) Only toggle actions "
		  "may be switched", name( "unnamed" ) );
#endif
	return;
    }
    if ( enable == (bool)d->on )
	return;
    d->on = enable;
    d->update( QActionPrivate::State );
    emit toggled( enable );
}

/*!
  Returns TRUE if this toggle action is switched on, or FALSE if it is
 switched off.

 \sa setOn(), isToggleAction()
*/
bool QAction::isOn() const
{
    return d->on;
}

/*!
  Enables the action if \a enable is TRUE, otherwise disables it.

  Menu items and/or tool buttons presenting the action to the user are
  updated accordingly.

  \sa isEnabled()
 */
void QAction::setEnabled( bool enable )
{
    d->enabled = enable;
#ifndef QT_NO_ACCEL
    if ( d->accel )
	d->accel->setEnabled( enable );
#endif
    d->update( QActionPrivate::State );
}


/*!
  Returns TRUE if the action is enabled, or FALSE if it is disabled.

  \sa setEnabled()
 */
bool QAction::isEnabled() const
{
    return d->enabled;
}

void QAction::internalActivation()
{
    if ( isToggleAction() )
	setOn( !isOn() );
    emit activated();
}

void QAction::toolButtonToggled( bool on )
{
    if ( !isToggleAction() )
	return;
    setOn( on );
}

/*! Adds this action to widget \a w.

  Currently supported widget types are QToolBar and QPopupMenu.

  Returns TRUE when the action was added successfully, FALSE
  otherwise.

  \sa removeFrom()
 */
bool QAction::addTo( QWidget* w )
{
    if ( w->inherits( "QToolBar" ) ) {
	QCString bname = name() + QCString( "_action_button" );
	QToolButton* btn = new QToolButton( (QToolBar*) w, bname );
	btn->setToggleButton( d->toggleaction );
	d->toolbuttons.append( btn );
	if ( d->iconset )
	    btn->setIconSet( *d->iconset );
	d->update( QActionPrivate::State );
	d->update( QActionPrivate::Everything );
	connect( btn, SIGNAL( clicked() ), this, SIGNAL( activated() ) );
	connect( btn, SIGNAL( toggled(bool) ), this, SLOT( toolButtonToggled(bool) ) );
	connect( btn, SIGNAL( destroyed() ), this, SLOT( objectDestroyed() ) );
	connect( d->tipGroup, SIGNAL(showTip(const QString&)), this, SLOT(showStatusText(const QString&)) );
	connect( d->tipGroup, SIGNAL(removeTip()), this, SLOT(clearStatusText()) );
    } else if ( w->inherits( "QPopupMenu" ) ) {
	QActionPrivate::MenuItem* mi = new QActionPrivate::MenuItem;
	mi->popup = (QPopupMenu*) w;
	QIconSet* diconset = d->iconset; // stupid GCC 2.7.x compiler
	if ( diconset )
	    mi->id = mi->popup->insertItem( *diconset, QString::fromLatin1("") );
	else
	    mi->id = mi->popup->insertItem( QString::fromLatin1("") );
	mi->popup->connectItem( mi->id, this, SLOT(internalActivation()) );
	d->menuitems.append( mi );
	d->update( QActionPrivate::State );
	d->update( QActionPrivate::Everything );
	w->topLevelWidget()->className();
	connect( mi->popup, SIGNAL(highlighted( int )), this, SLOT(menuStatusText( int )) );
	connect( mi->popup, SIGNAL(aboutToHide()), this, SLOT(clearStatusText()) );
	connect( mi->popup, SIGNAL( destroyed() ), this, SLOT( objectDestroyed() ) );
    } else {
	qWarning( "QAction::addTo(), unknown object" );
	return FALSE;
    }
    return TRUE;
}

/*!
  Sets the status message to \a text
 */
void QAction::showStatusText( const QString& text )
{
    QObject* par;
    if ( ( par = parent() ) && par->inherits( "QActionGroup" ) )
	par = par->parent();
    if ( !par || !par->isWidgetType() )
	return;
    QObjectList* l = ( (QWidget*)par )->topLevelWidget()->queryList("QStatusBar");
    for ( QStatusBar* bar = (QStatusBar*) l->first(); bar; bar = (QStatusBar*)l->next() ) {
	if ( text.isEmpty() )
	    bar->clear();
	else
	    bar->message( text );
    }
    delete l;
}

/*!
  Sets the status message to the menuitem's status text, or
  to the tooltip, if there is no status text.
*/
void QAction::menuStatusText( int id )
{
    QString text;
    QListIterator<QActionPrivate::MenuItem> it( d->menuitems);
    QActionPrivate::MenuItem* mi;
    while ( ( mi = it.current() ) ) {
	++it;
	if ( mi->id == id ) {
	    text = statusTip();
	    break;
	}
    }

    if ( !text.isEmpty() )
	showStatusText( text );
}

/*!
  Clears the status text.
*/
void QAction::clearStatusText()
{
    showStatusText( QString::null );
}

/*!
  Removes the action from widget \a w

  Returns TRUE when the action was removed successfully, FALSE
  otherwise.

  \sa addTo()
*/
bool QAction::removeFrom( QWidget* w )
{
    if ( w->inherits( "QToolBar" ) ) {
	QListIterator<QToolButton> it( d->toolbuttons);
	QToolButton* btn;
	while ( ( btn = it.current() ) ) {
	    ++it;
	    if ( btn->parentWidget() == w ) {
		d->toolbuttons.removeRef( btn );
		disconnect( btn, SIGNAL( destroyed() ), this, SLOT( objectDestroyed() ) );
		delete btn;
		// no need to disconnect from statusbar
	    }
	}
    } else if ( w->inherits( "QPopupMenu" ) ) {
	QListIterator<QActionPrivate::MenuItem> it( d->menuitems);
	QActionPrivate::MenuItem* mi;
	while ( ( mi = it.current() ) ) {
	    ++it;
	    if ( mi->popup == w ) {
		disconnect( mi->popup, SIGNAL(highlighted( int )), this, SLOT(menuStatusText(int)) );
		disconnect( mi->popup, SIGNAL(aboutToHide()), this, SLOT(clearStatusText()) );
		disconnect( mi->popup, SIGNAL( destroyed() ), this, SLOT( objectDestroyed() ) );
		mi->popup->removeItem( mi->id );
		d->menuitems.removeRef( mi );
	    }
	}
    } else {
	qWarning( "QAction::removeFrom(), unknown object" );
	return FALSE;
    }
    return TRUE;
}

void QAction::objectDestroyed()
{
    const QObject* obj = sender();
    QListIterator<QActionPrivate::MenuItem> it( d->menuitems);
    QActionPrivate::MenuItem* mi;
    while ( ( mi = it.current() ) ) {
	++it;
	if ( mi->popup == obj )
	    d->menuitems.removeRef( mi );
    }
    d->toolbuttons.removeRef( (QToolButton*) obj );
}



/*!
  \fn void QAction::activated()

  This signal is emitted when the action was activated by the user.

  \sa toggled()
*/

/*!
  \fn void QAction::toggled(bool)

  This signal is emitted when a toggle action changes state.

  \sa activated(), setToggleAction()
*/



class QActionGroupPrivate
{
public:
    uint exclusive: 1;
    QList<QAction> actions;
    QAction* selected;
};


/*!
  \class QActionGroup qaction.h

  \brief The QActionGroup class combines actions to a group.

  An action group makes it easier to deal with groups of actions. It
  allows to add, remove or activate its children with a single call
  and provides radio semantics ("one of many" choice) for toggle
  actions.

  QActionGroup is an action by its own and thus can be treated as
  such. Standard action functions like addTo(), removeFrom() and
  setEnabled() are automatically performed on all members of the
  group, i.e. for example that adding a group to a toolbar creates a
  tool button for each child action.

  Toggle action handling is controlled with the setExclusive() flag,
  with the default being TRUE. An exclusive group switches off all
  toggle actions except the one that was activated. This results in a
  "one of many" choice similar to a group of radio buttons (see
  QRadioButton). An exclusive group emits the signal selected()
  whenever the current child action changes.
*/

/*!
  Constructs an action group with parent \a parent and name \a name.

  If \a exclusive is TRUE, any toggle action that is a member of this
  group is toggled off by another action being toggled on.

 */
QActionGroup::QActionGroup( QWidget* parent, const char* name, bool exclusive )
    : QAction( parent, name )
{
    d = new QActionGroupPrivate;
    d->exclusive = exclusive;
    d->selected = 0;
}

/*! Destroys the object and frees any allocated resources. */

QActionGroup::~QActionGroup()
{
    delete d;
}

/*!
  Sets the action group to be exclusive if \e enable is TRUE,
  or to be non-exclusive if \e enable is FALSE.

  In an exclusive group, any toggle action that is a member of this
  group is toggled off by another action being toggled on.

  \sa isExclusive()
 */
void QActionGroup::setExclusive( bool enable )
{
    d->exclusive = enable;
}

/*!
  Returns TRUE if the action group is exclusive, otherwise FALSE.

  \sa setExclusive()
*/

bool QActionGroup::isExclusive() const
{
    return d->exclusive;
}


/*!
  Inserts action \a action to the group.

  It is not necessary to manually insert actions that have this action
  group as their parent object.

 */
void QActionGroup::insert( QAction* action )
{
    if ( d->actions.containsRef( action ) )
	return;

    action->setEnabled( isEnabled() );

    d->actions.append( action );
    connect( action, SIGNAL( destroyed() ), this, SLOT( childDestroyed() ) );
    connect( action, SIGNAL( activated() ), this, SIGNAL( activated() ) );
    connect( action, SIGNAL( toggled( bool ) ), this, SLOT( childToggled( bool ) ) );
}


/*!\reimp
 */
bool QActionGroup::addTo( QWidget* w )
{
    for ( QListIterator<QAction> it( d->actions); it.current(); ++it ) {
	it.current()->addTo( w );
    }
    return TRUE;
}

/*!\reimp
 */
bool QActionGroup::removeFrom( QWidget* w )
{
    for ( QListIterator<QAction> it( d->actions); it.current(); ++it ) {
	it.current()->removeFrom( w );
    }
    return TRUE;
}


void QActionGroup::childToggled( bool b )
{
    if ( !isExclusive() )
	return;
    QAction* s = (QAction*) sender();
    if ( b ) {
	if ( s != d->selected ) {
	    d->selected = s;
	    for ( QListIterator<QAction> it( d->actions); it.current(); ++it ) {
		if ( it.current()->isToggleAction() && it.current() != s )
		    it.current()->setOn( FALSE );
	    }
	    emit activated();
	    emit selected( s );
	}
    } else {
	if ( s == d->selected ) {
	    // at least one has to be selected
	    s->setOn( TRUE );
	}
    }
}

void QActionGroup::childDestroyed()
{
    d->actions.removeRef( (QAction*) sender() );
    if ( d->selected == sender() )
	d->selected = 0;
}

/*!\reimp
 */
void QActionGroup::setEnabled( bool enable )
{
    for ( QListIterator<QAction> it( d->actions); it.current(); ++it ) {
	it.current()->setEnabled( enable );
    }
    QAction::setEnabled( enable );
}

/*!
  \fn void QActionGroup::selected(QAction*)

  This signal is emitted in exclusive groups when the current toggle
  action changes.

 \sa setExclusive()
*/

#endif
