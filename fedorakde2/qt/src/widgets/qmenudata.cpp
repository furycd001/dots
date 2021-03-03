/****************************************************************************
** $Id: qt/src/widgets/qmenudata.cpp   2.3.2   edited 2001-05-02 $
**
** Implementation of QMenuData class
**
** Created : 941128
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

#define	 INCLUDE_MENUITEM_DEF
#include "qmenudata.h"
#ifndef QT_NO_MENUDATA
#include "qpopupmenu.h"
#include "qmenubar.h"
#include "qapplication.h"
#include "qguardedptr.h"

class QMenuItemData {
public:
    QCustomMenuItem    *custom_item;	// custom menu item
};

class QMenuDataData {
    // attention: also defined in qmenubar.cpp and qpopupmenu.cpp
public:
    QMenuDataData();
    QGuardedPtr<QWidget> aWidget;
    int aInt;
};
QMenuDataData::QMenuDataData()
    : aInt(-1)
{}

// NOT REVISED
/*!
  \class QMenuData qmenudata.h
  \brief The QMenuData class is a base class for QMenuBar and QPopupMenu.

  \ingroup misc

  QMenuData has an internal list of menu items.	 A menu item is a text,
  pixmap or a separator, and may also have a popup menu (separators
  have no popup menus).

  The menu item sends out an activated() signal when it is selected, and
  a highlighted() signal when it receives the user input focus.

  Menu items can be accessed through identifiers.

  \sa QAccel
*/


/*****************************************************************************
  QMenuItem member functions
 *****************************************************************************/

QMenuItem::QMenuItem()
{
    ident	 = -1;
    is_separator = FALSE;
    is_checked   = FALSE;
    is_enabled	 = TRUE;
    is_dirty	 = TRUE;
    iconset_data	 = 0;
    pixmap_data	 = 0;
    popup_menu	 = 0;
    widget_item	 = 0;
    accel_key	 = 0;
    signal_data	 = 0;
    d = 0;
}

QMenuItem::~QMenuItem()
{
    delete iconset_data;
    delete pixmap_data;
    delete signal_data;
    delete widget_item;
    delete d;
}


/*****************************************************************************
  QMenuData member functions
 *****************************************************************************/

QMenuItemData* QMenuItem::extra()
{
    if ( !d ) d = new QMenuItemData;
    return d;
}

QCustomMenuItem *QMenuItem::custom() const
{
    if ( !d ) return 0;
    return d->custom_item;
}


static int get_seq_id()
{
    static int seq_no = -2;
    return seq_no--;
}


/*!
  Constructs an empty list.
*/

QMenuData::QMenuData()
{
    actItem = -1;				// no active menu item
    mitems = new QMenuItemList;			// create list of menu items
    CHECK_PTR( mitems );
    mitems->setAutoDelete( TRUE );
    parentMenu = 0;				// assume top level
    isPopupMenu = FALSE;
    isMenuBar = FALSE;
    mouseBtDn = FALSE;
    badSize = TRUE;
    avoid_circularity = 0;
    actItemDown = FALSE;
    d = new QMenuDataData;
}

/*!
  Removes all menu items and disconnects any signals that have been connected.
*/

QMenuData::~QMenuData()
{
    register QMenuItem *mi = mitems->first();
    while ( mi ) {
	if ( mi->popup_menu )			// reset parent pointer for all
	    mi->popup_menu->parentMenu = 0;	//   child menus
	mi = mitems->next();
    }
    delete mitems;				// delete menu item list
    delete d;
}


/*!
  Virtual function; notifies subclasses about an item that has been changed.
*/

void QMenuData::updateItem( int )		// reimplemented in subclass
{
}

/*!
  Virtual function; notifies subclasses that one or more items have been
  inserted or removed.
*/

void QMenuData::menuContentsChanged()		// reimplemented in subclass
{
}

/*!
  Virtual function; notifies subclasses that one or more items have changed
  state (enabled/disabled or checked/unchecked).
*/

void QMenuData::menuStateChanged()		// reimplemented in subclass
{
}

/*!
  Virtual function; notifies subclasses that a popup menu item has been
  inserted.
*/

void QMenuData::menuInsPopup( QPopupMenu * )	// reimplemented in subclass
{
}

/*!
  Virtual function; notifies subclasses that a popup menu item has been
  removed.
*/

void QMenuData::menuDelPopup( QPopupMenu * )	// reimplemented in subclass
{
}


/*!
  Returns the number of items in the menu.
*/

uint QMenuData::count() const
{
    return mitems->count();
}



    /*!
  Internal function that insert a menu item.  Called by all insert()
  functions.
*/

int QMenuData::insertAny( const QString *text, const QPixmap *pixmap,
			  QPopupMenu *popup, const QIconSet* iconset, int id, int index,
			  QWidget* widget, QCustomMenuItem* custom )
{
    if ( index < 0 || index > (int) mitems->count() )	// append
	index = mitems->count();
    if ( id < 0 )				// -2, -3 etc.
	id = get_seq_id();

    register QMenuItem *mi = new QMenuItem;
    CHECK_PTR( mi );
    mi->ident = id;
    if ( widget != 0 ) {
	mi->widget_item = widget;
	mi->is_separator = !widget->isFocusEnabled();
    } else if ( custom != 0 ) {
	mi->extra()->custom_item = custom;
	mi->is_separator = custom->isSeparator();
    } else if ( text == 0 && pixmap == 0 && popup == 0 ) {
	mi->is_separator = TRUE;		// separator
    } else {
	mi->text_data = text?*text:QString::null;
	mi->accel_key = Qt::Key_unknown;
	if ( pixmap )
	    mi->pixmap_data = new QPixmap( *pixmap );
	if ( (mi->popup_menu = popup) ) 
	    menuInsPopup( popup );
	if ( iconset )
	    mi->iconset_data = new QIconSet( *iconset );
    }

    mitems->insert( index, mi );
    menuContentsChanged();			// menu data changed
    return mi->ident;
}

/*!
  Internal function that finds the menu item where \a popup is located,
  storing its index at \a index if \a index is not NULL.
*/
QMenuItem *QMenuData::findPopup( QPopupMenu *popup, int *index )
{
    int i = 0;
    QMenuItem *mi = mitems->first();
    while ( mi ) {
	if ( mi->popup_menu == popup )		// found popup
	    break;
	i++;
	mi = mitems->next();
    }
    if ( index && mi )
	*index = i;
    return mi;
}

void QMenuData::removePopup( QPopupMenu *popup )
{
    int index = 0;
    QMenuItem *mi = findPopup( popup, &index );
    if ( mi )
	removeItemAt( index );
}


/*!
  does nothing, but that virtual ### remove 3.0
*/

void QMenuData::setAllDirty( bool  )
{
}



/*!
  The family of insertItem() functions inserts menu items into a
  popup menu or a menu bar.

  A menu item is usually either a text string or a a pixmap, both with
  an optional icon or keyboard accelerator. As special cases it is
  also possible to insert custom items (see QCustomMenuItem) or even
  widgets into popup menus.

  Some insertItem() members take a popup menu as additional
  argument. Use these to insert submenus to existing menus or pulldown
  menus to a menu bar.

  The amount of insert functions may look confusing, but is actually
  quite handy to use.

  This default version inserts a menu item with a text, an accelerator
  key, an id and an optional index and connects it to an object/slot.

  Example:
  \code
    QMenuBar   *mainMenu = new QMenuBar;
    QPopupMenu *fileMenu = new QPopupMenu;
    fileMenu->insertItem( "New",  myView, SLOT(newFile()), CTRL+Key_N );
    fileMenu->insertItem( "Open", myView, SLOT(open()),    CTRL+Key_O );
    mainMenu->insertItem( "File", fileMenu );
  \endcode

  Not all insert functions take an object/slot parameter or an
  accelerator key. Use connectItem() and setAccel() on these items.

  If you will need to translate accelerators, use QAccel::stringToKey()
  to calculate the accelerator key:
  \code
    fileMenu->insertItem( tr("Open"), myView, SLOT(open()),
			  QAccel::stringToKey( tr("Ctrl+O") ) );
  \endcode

  In the example above, pressing CTRL+N or selecting "open" from the
  menu activates the myView->open() function.

  Some insert functions take a QIconSet parameter to specify the
  little menu item icon. Note that you can always pass a QPixmap
  object instead.

  The menu item is assigned the identifier \a id or an automatically
  generated identifier if \a id is < 0. The generated identifiers
  (negative integers) are guaranteed to be unique within the entire
  application.

  The \a index specifies the position in the menu.  The menu item is
  appended at the end of the list if \a index is negative.

  Note that keyboard accelerators in Qt are not application global, but
  bound to a certain toplevel window. Accelerators in QPopupMenu items
  therefore only work for menus that are associated with a certain
  window. This is true for popup menus that live in a menu bar, for
  instance. In that case, the accelerator will be installed on the
  menu bar itself. It also works for stand-alone popup menus that have
  a toplevel widget in their parentWidget()- chain. The menu will then
  install its accelerator object on that toplevel widget. For all
  other cases, use an independent QAccel object.

  \warning Be careful when passing a literal 0 to insertItem(), as
	some C++ compilers choose the wrong overloaded function.
	Cast the 0 to what you mean, eg. <tt>(QObject*)0</tt>.

  \sa removeItem(), changeItem(), setAccel(), connectItem(), QAccel,
  qnamespace.h
*/

int QMenuData::insertItem( const QString &text,
			   const QObject *receiver, const char* member,
			   int accel, int id, int index )
{
    int actualID = insertAny( &text, 0, 0, 0, id, index );
    connectItem( actualID, receiver, member );
    if ( accel )
	setAccel( accel, actualID );
    return actualID;
}

/*!\overload
  Inserts a menu item with an icon, a text, an accelerator key, an id
  and an optional index and connects it to an object/slot. The icon
  will be displayed to the left of the text in the item.

  \sa removeItem(), changeItem(), setAccel(), connectItem(), QAccel,
  qnamespace.h
*/

int QMenuData::insertItem( const QIconSet& icon,
			   const QString &text,
			   const QObject *receiver, const char* member,
			   int accel, int id, int index )
{
    int actualID = insertAny( &text, 0, 0, &icon, id, index );
    connectItem( actualID, receiver, member );
    if ( accel )
	setAccel( accel, actualID );
    return actualID;
}

/*!\overload
  Inserts a menu item with a pixmap, an accelerator key, an id and an
  optional index and connects it to an object/slot.

  To look best when being highlighted as menu item, the pixmap should
  provide a mask, see QPixmap::mask().

  Returns the menu item identifier.

  \sa removeItem(), changeItem(), setAccel(), connectItem()
*/

int QMenuData::insertItem( const QPixmap &pixmap,
			   const QObject *receiver, const char* member,
			   int accel, int id, int index )
{
    int actualID = insertAny( 0, &pixmap, 0, 0, id, index );
    connectItem( actualID, receiver, member );
    if ( accel )
	setAccel( accel, actualID );
    return actualID;
}


/*!\overload
  Inserts a menu item with an icon, a pixmap, an accelerator key, an id
  and an optional index and connects it to an object/slot. The icon
  will be displayed to the left of the pixmap in the item.

  To look best when being highlighted as menu item, the pixmap should
  provide a mask, see QPixmap::mask().

  Returns the menu item identifier.

  \sa removeItem(), changeItem(), setAccel(), connectItem(), QAccel,
  qnamespace.h
*/

int QMenuData::insertItem( const QIconSet& icon,
			   const QPixmap &pixmap,
			   const QObject *receiver, const char* member,
			   int accel, int id, int index )
{
    int actualID = insertAny( 0, &pixmap, 0, &icon, id, index );
    connectItem( actualID, receiver, member );
    if ( accel )
	setAccel( accel, actualID );
    return actualID;
}


/*!\overload
  Inserts a menu item with a text.  Returns the menu item identifier.

  \sa removeItem(), changeItem(), setAccel(), connectItem()
*/

int QMenuData::insertItem( const QString &text, int id, int index )
{
    return insertAny( &text, 0, 0, 0, id, index );
}

/*!\overload
  Inserts a menu item with an icon and a text.  The icon will be
  displayed to the left of the text in the item. Returns the menu
  item identifier.

  \sa removeItem(), changeItem(), setAccel(), connectItem()
*/

int QMenuData::insertItem( const QIconSet& icon,
			   const QString &text, int id, int index )
{
    return insertAny( &text, 0, 0, &icon, id, index );
}

/*!\overload
  Inserts a menu item with a text and a sub menu.

  The \a popup must be deleted by the programmer or by its parent
  widget.  It is not deleted when this menu item is removed or when
  the menu is deleted.

  Returns the menu item identifier.

  \sa removeItem(), changeItem(), setAccel(), connectItem()
*/

int QMenuData::insertItem( const QString &text, QPopupMenu *popup,
			   int id, int index )
{
    return insertAny( &text, 0, popup, 0, id, index );
}

/*!\overload
  Inserts a menu item with an icon, a text and a sub menu. The icon
  will be displayed to the left of the text in the item.

  The \a popup must be deleted by the programmer or by its parent
  widget.  It is not deleted when this menu item is removed or when
  the menu is deleted.

  Returns the menu item identifier.

  \sa removeItem(), changeItem(), setAccel(), connectItem()
*/

int QMenuData::insertItem( const QIconSet& icon,
			   const QString &text, QPopupMenu *popup,
			   int id, int index )
{
    return insertAny( &text, 0, popup, &icon, id, index );
}

/*!\overload
  Inserts a menu item with a pixmap.  Returns the menu item identifier.

  To look best when being highlighted as menu item, the pixmap should
  provide a mask, see QPixmap::mask().

  \sa removeItem(), changeItem(), setAccel(), connectItem()
*/

int QMenuData::insertItem( const QPixmap &pixmap, int id, int index )
{
    return insertAny( 0, &pixmap, 0, 0, id, index );
}

/*!\overload
  Inserts a menu item with an icon and a pixmap.  The icon will be
  displayed to the left of the pixmap in the item. Returns the menu
  item identifier.

  \sa removeItem(), changeItem(), setAccel(), connectItem()
*/

int QMenuData::insertItem( const QIconSet& icon,
			   const QPixmap &pixmap, int id, int index )
{
    return insertAny( 0, &pixmap, 0, &icon, id, index );
}


/*!\overload
  Inserts a menu item with a pixmap and a sub menu. The icon
  will be displayed to the left of the pixmap in the item.

  The \a popup must be deleted by the programmer or by its parent
  widget.  It is not deleted when this menu item is removed or when
  the menu is deleted.

  Returns the menu item identifier.

  \sa removeItem(), changeItem(), setAccel(), connectItem()
*/

int QMenuData::insertItem( const QPixmap &pixmap, QPopupMenu *popup,
			   int id, int index )
{
    return insertAny( 0, &pixmap, popup, 0, id, index );
}


/*!\overload
  Inserts a menu item with an icon, a pixmap and a sub menu. The icon
  will be displayed to the left of the pixmap in the item.

  The \a popup must be deleted by the programmer or by its parent
  widget.  It is not deleted when this menu item is removed or when
  the menu is deleted.

  Returns the menu item identifier.

  \sa removeItem(), changeItem(), setAccel(), connectItem()
*/

int QMenuData::insertItem( const QIconSet& icon,
			   const QPixmap &pixmap, QPopupMenu *popup,
			   int id, int index )
{
    return insertAny( 0, &pixmap, popup, &icon, id, index );
}



/*!\overload
  Inserts a menu item that consists of the widget \a widget.

  Ownership of \a widget is transferred to the popup menu or the
  menubar.

  Theoretically, any widget can be inserted into a popup menu. In
  practice, this only makes sense with certain widgets.

  If a widget is not focus enabled ( see QWidget::isFocusEnabled() ),
  the menu treats it as a separator. This means, the item is not
  selectable and will never get focus. This way you can for example
  simply insert a QLabel if you need a popup menu with a title.

  If the widget is focus enabled, it will get focus when the user
  traverses the popup menu with the arrow keys. If the widget does not
  accept ArrowUp and ArrowDown in its key event handler, the focus
  will move back to the menu when the the respective arrow key is hit
  one more time. This works for example with a QLineEdit.  If the
  widget accepts the arrow keys itself, it must also provide the
  possibility to put the focus back on the menu again by calling
  QWidget::focusNextPrevChild() respectively. Futhermore should the
  embedded widget close the menu when the user made a selection.  This
  can be done safely by calling \code if ( isVisible() &&
  parentWidget() &&
  parentWidget()->inherits("QPopupMenu") )
	parentWidget()->close();
  \endcode

  \sa removeItem()
*/
int QMenuData::insertItem( QWidget* widget, int id, int index )
{
    return insertAny( 0, 0, 0, 0, id, index, widget );
}


/*!\overload
  Inserts a custom menu item \a custom.

  This only works with popup menus. It is not supported for menu bars.
  Ownership of \a custom is transferred to the popup menu.

  If you want to connect a custom item to a certain slot, use connectItem().

  \sa connectItem(), removeItem(), QCustomMenuItem
*/
int QMenuData::insertItem( QCustomMenuItem* custom, int id, int index )
{
    return insertAny( 0, 0, 0, 0, id, index, 0, custom );
}

/*!\overload
  Inserts a custom menu item \a custom with an \a icon.

  This only works with popup menus. It is not supported for menu bars.
  Ownership of \a custom is transferred to the popup menu.

  If you want to connect a custom item to a certain slot, use connectItem().

  \sa connectItem(), removeItem(), QCustomMenuItem
*/
int QMenuData::insertItem( const QIconSet& icon, QCustomMenuItem* custom, int id, int index )
{
    return insertAny( 0, 0, 0, &icon, id, index, 0, custom );
}


/*!
  Inserts a separator at position \a index.
  The separator becomes the last menu item if \a index is negative.

  In a popup menu, a separator is rendered as a horizontal line.  In a
  Motif menubar, a separator is spacing, so the rest of the items
  (just "Help", normally) are drawn right-justified.  In a Windows
  menubar, separators are ignored (to comply with the Windows style
  guide).
*/
int QMenuData::insertSeparator( int index )
{
    return insertAny( 0, 0, 0, 0, -1, index );
}

/*!
  \fn void QMenuData::removeItem( int id )
  Removes the menu item which has the identifier \a id.
  \sa removeItemAt(), clear()
*/

/*!
  Removes the menu item at position \a index.
  \sa removeItem(), clear()
*/

void QMenuData::removeItemAt( int index )
{
    if ( index < 0 || index >= (int)mitems->count() ) {
#if defined(CHECK_RANGE)
	qWarning( "QMenuData::removeItem: Index %d out of range", index );
#endif
	return;
    }
    QMenuItem *mi = mitems->at( index );
    if ( mi->popup_menu )
	menuDelPopup( mi->popup_menu );
    mitems->remove();
    if ( !QApplication::closingDown() )		// avoid trouble
	menuContentsChanged();
}


/*!
  Removes all menu items.
  \sa removeItem(), removeItemAt()
*/

void QMenuData::clear()
{
    register QMenuItem *mi = mitems->first();
    while ( mi ) {
	if ( mi->popup_menu )
	    menuDelPopup( mi->popup_menu );
	mitems->remove();
	mi = mitems->current();
    }
    if ( !QApplication::closingDown() )		// avoid trouble
	menuContentsChanged();
}


/*!
  Returns the accelerator key that has been defined for the menu item \a id,
  or 0 if it has no accelerator key.
  \sa setAccel(), QAccel, qnamespace.h
*/

int QMenuData::accel( int id ) const
{
    QMenuItem *mi = findItem( id );
    return mi ? mi->key() : 0;
}

/*!
  Defines an accelerator key for the menu item \a id.

  An accelerator key consists of a key code and a combination of the modifiers
  \c SHIFT, \c CTRL, \c ALT, or \c UNICODE_ACCEL (OR'ed or added).
  The header file qnamespace.h contains a list of key codes.

  Defining an accelerator key generates a text which is added to the
  menu item, for instance, \c CTRL + \c Key_O generates "Ctrl+O".  The
  text is formatted differently for different platforms.

  Note that keyboard accelerators in Qt are not application global, but
  bound to a certain toplevel window. Accelerators in QPopupMenu items
  therefore only work for menus that are associated with a certain
  window. This is true for popup menus that live in a menu bar, for
  instance. In that case, the accelerator will be installed on the
  menu bar itself. It also works for stand-alone popup menus that have
  a toplevel widget in their parentWidget()- chain. The menu will then
  install its accelerator object on that toplevel widget. For all
  other cases, use an independent QAccel object.

  Example:
  \code
    QMenuBar   *mainMenu = new QMenuBar;
    QPopupMenu *fileMenu = new QPopupMenu;	// file sub menu
    fileMenu->insertItem( "Open Document", 67 );// add "Open" item
    fileMenu->setAccel( CTRL + Key_O, 67 );     // Control and O to open
    fileMenu->insertItem( "Quit", 69 );		// add "Quit" item
    fileMenu->setAccel( CTRL + ALT + Key_Delete, 69 );
    mainMenu->insertItem( "File", fileMenu );	// add the file menu
  \endcode

  If you will need to translate accelerators, use QAccel::stringToKey():

  \code
    fileMenu->setAccel( QAccel::stringToKey(tr("Ctrl+O")), 67 );
  \endcode

  You can also specify the accelerator in the insertItem() function.

  \sa accel(), insertItem(), QAccel, qnamespace.h
*/

void QMenuData::setAccel( int key, int id )
{
    QMenuData *parent;
    QMenuItem *mi = findItem( id, &parent );
    if ( mi ) {
	mi->accel_key = key;
	parent->menuContentsChanged();
    }
}

/*!
  Returns the icon set that has been set for menu item \a id, or 0 if no icon
  set has been set.
  \sa changeItem(), text(), pixmap()
*/

QIconSet* QMenuData::iconSet( int id ) const
{
    QMenuItem *mi = findItem( id );
    return mi ? mi->iconSet() : 0;
}

/*!
  Returns the text that has been set for menu item \a id, or a
  \link QString::operator!() null string\endlink
  if no text has been set.
  \sa changeItem(), pixmap(), iconSet()
*/

QString QMenuData::text( int id ) const
{
    QMenuItem *mi = findItem( id );
    return mi ? mi->text() : QString::null;
}

/*!
  Returns the pixmap that has been set for menu item \a id, or 0 if no pixmap
  has been set.
  \sa changeItem(), text(), iconSet()
*/

QPixmap *QMenuData::pixmap( int id ) const
{
    QMenuItem *mi = findItem( id );
    return mi ? mi->pixmap() : 0;
}

/*!\obsolete

  Changes the text of the menu item \a id. If the item has an icon,
  the icon remains unchanged.

  \sa text()
*/

void QMenuData::changeItem( const QString &text, int id )
{
    changeItem( id, text);
}

/*!\obsolete

  Changes the pixmap of the menu item \a id. If the item has an icon,
  the icon remains unchanged.

  \sa pixmap()
*/

void QMenuData::changeItem( const QPixmap &pixmap, int id )
{
    changeItem( id, pixmap );
}

/*!\obsolete

  Changes the icon and text of the menu item \a id.

  \sa pixmap()
*/

void QMenuData::changeItem( const QIconSet &icon, const QString &text, int id )
{
    changeItem( id, icon, text );
}


/*!
  Changes the text of the menu item \a id. If the item has an icon,
  the icon remains unchanged.
  \sa text()
*/

void QMenuData::changeItem( int id, const QString &text )
{
    QMenuData *parent;
    QMenuItem *mi = findItem( id, &parent );
    if ( mi ) {					// item found
	if ( mi->text_data == text )		// same string
	    return;
	if ( mi->pixmap_data ) {		// delete pixmap
	    delete mi->pixmap_data;
	    mi->pixmap_data = 0;
	}
	mi->text_data = text;
	if ( !mi->accel_key && text.find( '\t' ) != -1 )
	    mi->accel_key = Qt::Key_unknown;
	parent->menuContentsChanged();
    }
}

/*!
  Changes the pixmap of the menu item \a id. If the item has an icon,
  the icon remains unchanged.
  \sa pixmap()
*/

void QMenuData::changeItem( int id, const QPixmap &pixmap )
{
    QMenuData *parent;
    QMenuItem *mi = findItem( id, &parent );
    if ( mi ) {					// item found
	register QPixmap *i = mi->pixmap_data;
	bool fast_refresh = i != 0 &&
	    i->width() == pixmap.width() &&
	    i->height() == pixmap.height() &&
	    !mi->text();
	if ( !mi->text_data.isNull() )		// delete text
	    mi->text_data = QString::null;
	mi->pixmap_data = new QPixmap( pixmap );
	delete i; // old mi->pixmap_data, could be &pixmap
	if ( fast_refresh )
	    parent->updateItem( id );
	else
	    parent->menuContentsChanged();
    }
}

/*!
  Changes the icon and text of the menu item \a id.
  \sa pixmap()
*/

void QMenuData::changeItem( int id, const QIconSet &icon, const QString &text )
{
    changeItem(id, text);
    changeItemIconSet(id,  icon);
}

/*!
  Changes the icon and pixmap of the menu item \a id.
  \sa pixmap()
*/

void QMenuData::changeItem( int id, const QIconSet &icon, const QPixmap &pixmap )
{
    changeItem(id, pixmap);
    changeItemIconSet(id,  icon);
}



/*!
  Changes the icon of the menu item \a id.
  \sa pixmap()
*/

void QMenuData::changeItemIconSet( int id, const QIconSet &icon )
{
    QMenuData *parent;
    QMenuItem *mi = findItem( id, &parent );
    if ( mi ) {					// item found
	register QIconSet *i = mi->iconset_data;
	bool fast_refresh = i != 0;
	mi->iconset_data = new QIconSet( icon );
	delete i; // old mi->iconset_data, could be &icon
	if ( fast_refresh )
	    parent->updateItem( id );
	else
	    parent->menuContentsChanged();
    }
}


/*!
  Returns TRUE if the item with identifier \a id is enabled or FALSE if
  it is disabled.
  \sa setItemEnabled()
*/

bool QMenuData::isItemEnabled( int id ) const
{
    QMenuItem *mi = findItem( id );
    return mi ? mi->isEnabled() : FALSE;
}

/*!
  Enables the menu item with identifier \a id if \a enable is TRUE, or
  disables the item if \a enable is FALSE.
  \sa isItemEnabled()
*/

void QMenuData::setItemEnabled( int id, bool enable )
{
    QMenuData *parent;
    QMenuItem *mi = findItem( id, &parent );
    if ( mi && (bool)mi->is_enabled != enable ) {
	mi->is_enabled = enable;
#ifndef QT_NO_ACCEL
	if ( mi->popup() )
	    mi->popup()->enableAccel( enable );
#endif
	parent->menuStateChanged();
    }
}


/*!
  Returns TRUE if the menu item has been checked, otherwise FALSE.
  \sa setItemChecked()
*/

bool QMenuData::isItemChecked( int id ) const
{
    QMenuItem *mi = findItem( id );
    return mi ? mi->isChecked() : FALSE;
}

/*!
  Checks the menu item with id \a id if \a check is TRUE, or unchecks
  it if \a check is FALSE, and calls QPopupMenu::setCheckable( TRUE ) if
  necessary.

  \sa isItemChecked()
*/

void QMenuData::setItemChecked( int id, bool check )
{
    QMenuData *parent;
    QMenuItem *mi = findItem( id, &parent );
    if ( mi && (bool)mi->is_checked != check ) {
	mi->is_checked = check;
	if ( parent->isPopupMenu && !((QPopupMenu *)parent)->isCheckable() )
	    ((QPopupMenu *)parent)->setCheckable( TRUE );
	parent->menuStateChanged();
    }
}


/*!
  Returns a pointer to the menu item with identifier \a id, or 0 if
  there is no item with such an identifier.
  \sa indexOf()
*/

QMenuItem *QMenuData::findItem( int id ) const
{
    return findItem( id, 0 );
}


/*!
  Returns a pointer to the menu item with identifier \a id, or 0 if
  there is no item with such an identifier, and changes \a parent to
  point to the parent of the return value.

  \sa indexOf()
*/

QMenuItem * QMenuData::findItem( int id, QMenuData ** parent ) const
{
    if ( parent )
	*parent = (QMenuData *)this;		// ###

    if ( id == -1 )				// bad identifier
	return 0;
    QMenuItemListIt it( *mitems );
    QMenuItem *mi;
    while ( (mi=it.current()) ) {		// search this menu
	++it;
	if ( mi->ident == id )			// found item
	    return mi;
    }
    it.toFirst();
    while ( (mi=it.current()) ) {		// search submenus
	++it;
	if ( mi->popup_menu ) {
	    mi = mi->popup_menu->findItem( id, parent );
	    if ( mi )				// found item
		return mi;
	}
    }
    return 0;					// not found
}

/*!
  Returns the index of the menu item with identifier \a id, or -1 if
  there is no item with such an identifier.
  \sa idAt(), findItem()
*/

int QMenuData::indexOf( int id ) const
{
    if ( id == -1 )				// bad identifier
	return -1;
    QMenuItemListIt it( *mitems );
    QMenuItem *mi;
    int index = 0;
    while ( (mi=it.current()) ) {
	if ( mi->ident == id )			// this one?
	    return index;
	++index;
	++it;
    }
    return -1;					// not found
}

/*!
  Returns the identifier of the menu item at position \a index in the internal
  list, or -1 if \a index is out of range.
  \sa setId(), indexOf()
*/

int QMenuData::idAt( int index ) const
{
    return index < (int)mitems->count() && index >= 0 ?
	   mitems->at(index)->id() : -1;
}

/*!
  Sets the menu identifier of the item at \a index to \a id.

  If index is out of range the operation is ignored.

  \sa idAt()
*/

void QMenuData::setId( int index, int id )
{
    if ( index < (int)mitems->count() )
	mitems->at(index)->ident = id;
}


/*!
  Sets the parameter of the activation signal of item \a id to \a
  param.

  If any receiver takes an integer parameter, this value is passed.

  \sa connectItem(), disconnectItem(), itemParameter()
 */
bool QMenuData::setItemParameter( int id, int param ) {
    QMenuItem *mi = findItem( id );
    if ( !mi )					// no such identifier
	return FALSE;
    if ( !mi->signal_data ) {			// create new signal
	mi->signal_data = new QSignal;
	CHECK_PTR( mi->signal_data );
    }
    mi->signal_data->setParameter( param );
    return TRUE;
}


/*!
  Returns the parameter of the activation signal of item \a id.

  If no parameter has been specified for this item with
  setItemParameter(), the value defaults to \a id.

  \sa connectItem(), disconnectItem(), setItemParameter()
 */
int QMenuData::itemParameter( int id ) const
{
    QMenuItem *mi = findItem( id );
    if ( !mi || !mi->signal_data )
	return id;
    return mi->signal_data->parameter();
}


/*!
  Connects a menu item to a receiver and a slot or signal.

  The receiver's slot/signal is activated when the menu item is activated.

  \sa disconnectItem(), setItemParameter()
*/

bool QMenuData::connectItem( int id, const QObject *receiver,
			     const char* member )
{
    QMenuItem *mi = findItem( id );
    if ( !mi )					// no such identifier
	return FALSE;
    if ( !mi->signal_data ) {			// create new signal
	mi->signal_data = new QSignal;
	CHECK_PTR( mi->signal_data );
	mi->signal_data->setParameter( id );
    }
    return mi->signal_data->connect( receiver, member );
}


/*!
  Disconnects a receiver/member from a menu item.

  All connections are removed when the menu data object is destroyed.

  \sa connectItem(), setItemParameter()
*/

bool QMenuData::disconnectItem( int id, const QObject *receiver,
				const char* member )
{
    QMenuItem *mi = findItem( id );
    if ( !mi || !mi->signal_data )		// no identifier or no signal
	return FALSE;
    return mi->signal_data->disconnect( receiver, member );
}

/*!
  Sets a Whats This help for a certain menu item.

  \arg \e id is the menu item id.
  \arg \e text is the Whats This help text in rich text format ( see QStyleSheet)

  \sa whatsThis()
 */
void QMenuData::setWhatsThis( int id, const QString& text )
{

    QMenuData *parent;
    QMenuItem *mi = findItem( id, &parent );
    if ( mi ) {
	mi->setWhatsThis( text );
	parent->menuContentsChanged();
    }
}

/*!
  Returns the Whats This help text for the specified item \e id or
  QString::null if no text has been defined yet.

  \sa setWhatsThis()
 */
QString QMenuData::whatsThis( int id ) const
{

    QMenuItem *mi = findItem( id );
    return mi? mi->whatsThis() : QString::null;
}



// NOT REVISED
/*!
  \class QCustomMenuItem qmenudata.h
  \brief The QCustomMenuItem class is an abstract base class for custom menu items in
  popup menus.

  A custom menu item is a menu item that is defined by two purely
  virtual functions, paint() and sizeHint(). The size hint tells the
  menu how much space it needs to reserve for this item, whereas paint
  is called whenever the item needs painting.

  This simply mechanism gives applications the possibility to create
  all kinds of application specific menu items. Examples are items
  showing different fonts in a word processor, or menus that allow the
  selection of drawing utilities in a vector drawing program.

  A custom item is inserted into a popup menu with
  QPopupMenu::insertItem().

  Per default, a custom item can also have an icon set and/or an
  accelerator key. You can, however, reimplement fullSpan() to return
  TRUE if you want the item to span the entire popup menu width. This
  is in particular useful for labels.

  If you want the custom item to be treated as a separator only,
  reimplement isSeparator() to return TRUE.

  Note that you can also insert pixmaps or bitmaps as items into a
  popup menu. A custom menu item, however, offers even more
  flexibility and - which is especially important under windows style
  - the possibility to draw the item with a different color when it is
  highlighted.

  menu/menu.cpp shows a simply example how custom menu items can be
used.

  <img src=qpopmenu-fancy.png>


  \sa QMenuData, QPopupMenu
*/



/*!
  Constructs a QCustomMenuItem
 */
QCustomMenuItem::QCustomMenuItem()
{
}

/*!
  Destructs a QCustomMenuItem
 */
QCustomMenuItem::~QCustomMenuItem()
{
}


/*!
  Sets the font of the custom menu item.

  This function is called whenever the font in the popup menu
  changes. For menu items that show their own individual font entry,
  you want to ignore this.
 */
void QCustomMenuItem::setFont( const QFont&  )
{
}



/*!
  Returns whether this item wants to span the entire popup menu width.
  The default is FALSE, meaning that the menu may show an icon and/or
  an accelerator key for this item as well.
 */
bool QCustomMenuItem::fullSpan() const
{
    return FALSE;
}

/*!
  Returns whether this item is just a separator.
 */
bool QCustomMenuItem::isSeparator() const
{
    return FALSE;
}


/*! \fn void QCustomMenuItem::paint( QPainter* p, const QColorGroup& cg, bool act,  bool enabled, int x, int y, int w, int h );

  Paints this item. When this function is invoked, the painter \a p is
  set to the right font and the right foreground color suitable for a
  menu item text. The item is active according to \a act and
  enabled/disabled according to \a enabled. The geometry values \a x,
  \a y, \a w and h specify where to draw the item.

  Do not draw any background, this has already been done by the popup
  menu according to the current gui style.

 */


/*! \fn QSize QCustomMenuItem::sizeHint();

  Returns the size hint of this item.
 */



/*!
  Activates the menu item at index \a index.

  If the index is invalid (for example -1), the object itself is
  deactivated.
 */
void QMenuData::activateItemAt( int index )
{
#ifndef QT_NO_MENUBAR
    if ( isMenuBar )
	( (QMenuBar*)this )->activateItemAt( index );
    else
#endif
    if ( isPopupMenu )
	( (QPopupMenu*)this )->activateItemAt( index );
}

#endif
