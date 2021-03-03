/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt GUI Designer.
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

#include "kdewidgets.h"

#if defined(HAVE_KDE)
#include <kcharselect.h>
#include <kcolorbutton.h>
#include <kcombobox.h>
#include <kdatepik.h>
#include <kdatetbl.h>
#include <kdualcolorbtn.h>
#include <kiconview.h>
#include <kled.h>
#include <klineedit.h>
#include <klistbox.h>
#include <klistview.h>
#include <knuminput.h>
#include <ktextbrowser.h>
#include <ktoolbarbutton.h>
#include <kurllabel.h>
#endif

#include <widgetdatabase.h>

#include <qdatetime.h>

void qt_init_kde_widget_database()
{
#if defined(HAVE_KDE)
    WidgetDatabase::widgetGroup( "KDE" );

    WidgetDatabaseRecord *r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "KCharSelect";
    r->group = "KDE";
    r->toolTip = "Character Selector";
    r->whatsThis = "Character Selector";
    r->includeFile = "kcharselect.h";
    r->isContainer = FALSE;
    WidgetDatabase::append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "KColorButton";
    r->group = "KDE";
    r->toolTip = "Color Button";
    r->whatsThis = "Color Button";
    r->includeFile = "kcolorbutton.h";
    r->isContainer = FALSE;
    WidgetDatabase::append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "KComboBox";
    r->group = "KDE";
    r->toolTip = "KDE Combo Box";
    r->whatsThis = "KDE ComboBox";
    r->includeFile = "kcombobox.h";
    r->isContainer = FALSE;
    WidgetDatabase::append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "KDatePicker";
    r->group = "KDE";
    r->toolTip = "Date Picker";
    r->whatsThis = "Date Picker";
    r->includeFile = "kdatepik.h";
    r->isContainer = FALSE;
    WidgetDatabase::append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "KDateTable";
    r->group = "KDE";
    r->toolTip = "Date Table";
    r->whatsThis = "Date Table";
    r->includeFile = "kdatetbl.h";
    r->isContainer = FALSE;
    WidgetDatabase::append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "KDualColorButton";
    r->group = "KDE";
    r->toolTip = "Dual Color Button";
    r->whatsThis = "Dual Color Button";
    r->includeFile = "kdualcolorbtn.h";
    r->isContainer = FALSE;
    WidgetDatabase::append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "KIconView";
    r->group = "KDE";
    r->toolTip = "KDE Iconview";
    r->whatsThis = "KDE Iconview";
    r->includeFile = "kiconview.h";
    r->isContainer = FALSE;
    WidgetDatabase::append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "KLed";
    r->group = "KDE";
    r->toolTip = "Led";
    r->whatsThis = "Led";
    r->includeFile = "kled.h";
    r->isContainer = FALSE;
    WidgetDatabase::append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "KLineEdit";
    r->group = "KDE";
    r->toolTip = "KDE Lineedit";
    r->whatsThis = "KDE Lineedit";
    r->includeFile = "klineedit.h";
    r->isContainer = FALSE;
    WidgetDatabase::append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "KListBox";
    r->group = "KDE";
    r->toolTip = "KDE Listbox";
    r->whatsThis = "KDE Listbox";
    r->includeFile = "klistbox.h";
    r->isContainer = FALSE;
    WidgetDatabase::append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "KListView";
    r->group = "KDE";
    r->toolTip = "KDE Listview";
    r->whatsThis = "KDE Listview";
    r->includeFile = "klistview.h";
    r->isContainer = FALSE;
    WidgetDatabase::append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "KIntNumInput";
    r->group = "KDE";
    r->toolTip = "Integer Number Input";
    r->whatsThis = "Integer Number Input";
    r->includeFile = "knuminput.h";
    r->isContainer = FALSE;
    WidgetDatabase::append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "KDoubleNumInput";
    r->group = "KDE";
    r->toolTip = "Double Number Input";
    r->whatsThis = "Double Number Input";
    r->includeFile = "knuminput.h";
    r->isContainer = FALSE;
    WidgetDatabase::append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "KIntSpinBox";
    r->group = "KDE";
    r->toolTip = "KDE Spinbox";
    r->whatsThis = "KDE Spinbox";
    r->includeFile = "knuminput.h";
    r->isContainer = FALSE;
    WidgetDatabase::append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "KTextBrowser";
    r->group = "KDE";
    r->toolTip = "KDE Textbrowser";
    r->whatsThis = "KDE Textbrowser";
    r->includeFile = "ktextbrowser.h";
    r->isContainer = FALSE;
    WidgetDatabase::append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "KToolBarButton";
    r->group = "KDE";
    r->toolTip = "KDE Toolbutton";
    r->whatsThis = "KDE Toolbutton";
    r->includeFile = "ktoolbarbutton.h";
    r->isContainer = FALSE;
    WidgetDatabase::append( r );

    r = new WidgetDatabaseRecord;
    r->iconSet = "";
    r->name = "KURLLabel";
    r->group = "KDE";
    r->toolTip = "URL Label";
    r->whatsThis = "URL Label";
    r->includeFile = "kurllabel.h";
    r->isContainer = FALSE;
    WidgetDatabase::append( r );
#endif
}

QWidget *qt_create_kde_widget( const QString &className, QWidget *parent, const char *name, bool init )
{
#if defined(HAVE_KDE)
    if ( className == "KColorButton" ) {
	return new KColorButton( parent, name );
    } else if ( className == "KCharSelect" ) {
	return new KCharSelect( parent, name );
    } else if ( className == "KComboBox" ) {
	return new KComboBox( parent, name );
    } else if ( className == "KDatePicker" ) {
	return new KDatePicker( parent, QDate::currentDate(), name );
    } else if ( className == "KDateTable" ) {
	return new KDateTable( parent, QDate::currentDate(), name );
    } else if ( className == "KDualColorButton" ) {
	return new KDualColorButton( parent, name );
    } else if ( className == "KIconView" ) {
	KIconView *iv = new KIconView( parent, name );
	if ( init )
	    (void) new QIconViewItem( iv, "New Item" );
	return iv;
    } else if ( className == "KLed" ) {
	return new KLed( parent, name );
    } else if ( className == "KLineEdit" ) {
	return new KLineEdit( parent, name );
    } else if ( className == "KListBox" ) {
	KListBox *lb = new KListBox( parent, name );
	if ( init ) {
	    lb->insertItem( "New Item" );
	    lb->setCurrentItem( 0 );
	}
	return lb;
    } else if ( className == "KListView" ) {
	KListView *lv = new KListView( parent, name );
	lv->setSorting( -1 );
	if ( init ) {
	    lv->addColumn( "Column 1" );
	    lv->setCurrentItem( new QListViewItem( lv, "New Item" ) );
	}
	return lv;
    } else if ( className == "KIntNumInput" ) {
	return new KIntNumInput( parent, name );
    } else if ( className == "KDoubleNumInput" ) {
	return new KDoubleNumInput( parent, name );
    } else if ( className == "KIntSpinBox" ) {
	return new KIntSpinBox( parent, name );
    } else if ( className == "KTextBrowser" ) {
	return new KTextBrowser( parent, name );
    } else if ( className == "KToolBarButton" ) {
	return new KToolBarButton( parent, name );
    } else if ( className == "KURLLabel" ) {
	KURLLabel *l = new KURLLabel( parent, name );
	return l;
    }
#else
    (void)className.latin1(); // avoid warning
    Q_UNUSED( parent );
    Q_UNUSED( name );
    Q_UNUSED( init );
#endif
    return 0;
}

