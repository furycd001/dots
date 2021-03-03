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

#include <qvariant.h> // HP-UX compiler need this here

#include "widgetfactory.h"
#include <widgetdatabase.h>
#include "metadatabase.h"
#include "mainwindow.h"
#include "formwindow.h"
#include "pixmapchooser.h"
#include "layout.h"
#include "listboxeditorimpl.h"
#include "listvieweditorimpl.h"
#include "iconvieweditorimpl.h"
#include "formwindow.h"
#include "multilineeditorimpl.h"
#include "../integration/kdevelop/kdewidgets.h"

#include <qmodules.h>

#include <qpixmap.h>
#include <qgroupbox.h>
#include <qiconview.h>
#if defined(QT_MODULE_TABLE)
#include <qtable.h>
#endif
#include <qlineedit.h>
#include <qspinbox.h>
#include <qmultilineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qwidgetstack.h>
#include <qcombobox.h>
#include <qtabbar.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qobjectlist.h>
#include <qlcdnumber.h>
#include <qslider.h>
#include <qdial.h>
#include <qprogressbar.h>
#include <qtextview.h>
#include <qtextbrowser.h>
#include <qframe.h>
#include <qmetaobject.h>
#include <qwidgetstack.h>
#include <qwizard.h>
#include <qvaluelist.h>
#include <qtimer.h>

#include <globaldefs.h>


void QLayoutWidget::paintEvent( QPaintEvent* )
{
    QPainter p ( this );
    p.setPen( red );
    p.drawRect( rect() );
}


int QDesignerTabWidget::currentPage() const
{
    return tabBar()->currentTab();
}

void QDesignerTabWidget::setCurrentPage( int i )
{
    tabBar()->setCurrentTab( i );
}

QString QDesignerTabWidget::pageTitle() const
{
    return ((QTabWidget*)this)->tabLabel( QTabWidget::currentPage() );
}

void QDesignerTabWidget::setPageTitle( const QString& title )
{
    changeTab( QTabWidget::currentPage(), title  );
}

void QDesignerTabWidget::setPageName( const QCString& name )
{
    if ( QTabWidget::currentPage() )
	QTabWidget::currentPage()->setName( name );
}

QCString QDesignerTabWidget::pageName() const
{
    if ( !QTabWidget::currentPage() )
	return 0;
    return QTabWidget::currentPage()->name();
}

int QDesignerTabWidget::count() const
{
    return tabBar()->count();
}


int QDesignerWizard::currentPageNum() const
{
    for ( int i = 0; i < pageCount(); ++i ) {
	if ( page( i ) == currentPage() )
	    return i;
    }
    return 0;
}

void QDesignerWizard::setCurrentPage( int i )
{
    if ( i < currentPageNum() ) {
	while ( i < currentPageNum() ) {
	    if ( currentPageNum() == 0 )
		break;
	    back();
	}
	
    } else {
	while ( i > currentPageNum() ) {
	    if ( currentPageNum() == pageCount() - 1 )
		break;
	    next();
	}
    }
}

QString QDesignerWizard::pageTitle() const
{
    return title( currentPage() );
}

void QDesignerWizard::setPageTitle( const QString& title )
{
    setTitle( currentPage(), title );
}

void QDesignerWizard::setPageName( const QCString& name )
{
    if ( QWizard::currentPage() )
	QWizard::currentPage()->setName( name );
}

QCString QDesignerWizard::pageName() const
{
    if ( !QWizard::currentPage() )
	return 0;
    return QWizard::currentPage()->name();
}

int QDesignerWizard::pageNum( QWidget *p )
{
    for ( int i = 0; i < pageCount(); ++i ) {
	if ( page( i ) == p )
	    return i;
    }
    return -1;
}

void QDesignerWizard::addPage( QWidget *p, const QString &t )
{
    QWizard::addPage( p, t );
    if ( removedPages.find( p ) )
	removedPages.remove( p );
}

void QDesignerWizard::removePage( QWidget *p )
{
    QWizard::removePage( p );
    removedPages.insert( p, p );
}

void QDesignerWizard::insertPage( QWidget *p, const QString &t, int index )
{
    if ( removedPages.find( p ) )
	removedPages.remove( p );
    if ( index == pageCount() ) {
	addPage( p, t );
	return;
    }

    QList<Page> pages;
    pages.setAutoDelete( TRUE );

    setCurrentPage( pageCount() - 1 );
    while ( pageCount() ) {
	QWidget *w = currentPage();
	pages.insert( 0, new Page( w, title( w ) ) );
	back();
	removePage( w );
    }
    int i = 0;
    for ( Page *pg = pages.first(); pg; pg = pages.next(), ++i ) {
	if ( i == index ) {
	    addPage( p, t );
	    next();
	}
	addPage( pg->p, pg->t );
	next();
    }
}

QMap< int, QMap< QString, QVariant> > *defaultProperties = 0;
QMap< int, QStringList > *changedProperties = 0;

/*!
  \class WidgetFactory widgetfactory.h
  \brief Set of static functions for creating widgets, layouts and do other stuff

  The widget factory offers functions to create widgets, create and
  delete layouts find out other details - all based on the
  WidgetDatabase's data. So the functions that use ids use the same
  ids as in the WidgetDatabase.
*/


static void saveDefaultProperties( QWidget *w, int id )
{
    QMap< QString, QVariant> propMap;
    QStrList lst = w->metaObject()->propertyNames( TRUE );
    for ( uint i = 0; i < lst.count(); ++i )
	propMap.insert( lst.at( i ), w->property( lst.at( i ) ) );
    defaultProperties->insert( id, propMap );
}

static void saveChangedProperties( QWidget *w, int id )
{
    QStringList l = MetaDataBase::changedProperties( w );
    changedProperties->insert( id, l );
}

/*!  Creates a widget of the type which is registered as \a id as
  child of \a parent. The \a name is optional. If \a init is TRUE, the
  widget is initialized with some defaults, else the plain widget is
  created.
*/

QWidget *WidgetFactory::create( int id, QWidget *parent, const char *name, bool init, const QRect *r, Qt::Orientation orient )
{
    QString n = WidgetDatabase::className( id );
    if ( n.isEmpty() )
	return 0;

    if ( !defaultProperties ) {
	defaultProperties = new QMap< int, QMap< QString, QVariant> >();
	changedProperties = new QMap< int, QStringList >();
    }

    QWidget *w = 0;
    QString str = WidgetDatabase::createWidgetName( id );
    const char *s = str.latin1();
    if ( !WidgetDatabase::isCustomWidget( id ) )
	w = createWidget( n, parent, name ? name : s, init, r, orient );
    else
	w = createCustomWidget( parent, name ? name : s, MetaDataBase::customWidget( id ) );
    if ( !w )
	return 0;
    MetaDataBase::addEntry( w );

    if ( !defaultProperties->contains( id ) )
	saveDefaultProperties( w, id );
    if ( !changedProperties->contains( id ) )
	saveChangedProperties( w, id );

    return w;
}

/*!  Creates a layout on the widget \a widget of the type \a type
  which can be \c HBox, \c VBox or \c Grid.
*/

QLayout *WidgetFactory::createLayout( QWidget *widget, QLayout*  layout, LayoutType type )
{
    int spacing = BOXLAYOUT_DEFAULT_SPACING;
    int margin = 0;

    if ( widget && !widget->inherits( "QLayoutWidget" ) &&
	 ( WidgetDatabase::isContainer( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( widget ) ) ) ||
	   widget && widget->parentWidget() && widget->parentWidget()->inherits( "FormWindow" ) ) )
	margin = BOXLAYOUT_DEFAULT_MARGIN;
	
    if ( !layout && widget && widget->inherits( "QTabWidget" ) )
	widget = ((QTabWidget*)widget)->currentPage();

    if ( !layout && widget && widget->inherits( "QWizard" ) )
	widget = ((QWizard*)widget)->currentPage();

    if ( !layout && widget && widget->inherits( "QWidgetStack" ) )
	widget = ((QWidgetStack*)widget)->visibleWidget();

    MetaDataBase::addEntry( widget );

    if ( !layout && widget && widget->inherits( "QGroupBox" ) ) {
	QGroupBox *gb = (QGroupBox*)widget;
	gb->setColumnLayout( 0, Qt::Vertical );
	gb->layout()->setMargin( 0 );
	gb->layout()->setSpacing( 0 );
	QLayout *l;
	switch ( type ) {
	case HBox:
	    l = new QHBoxLayout( gb->layout() );
	    MetaDataBase::setMargin( gb, margin );
	    MetaDataBase::setSpacing( gb, spacing );
	    l->setAlignment( AlignTop );
	    return l;
	case VBox:
	    l = new QVBoxLayout( gb->layout(), spacing );
	    MetaDataBase::setMargin( gb, margin );
	    MetaDataBase::setSpacing( gb, spacing );
	    l->setAlignment( AlignTop );
	    return l;
	case Grid:
	    l = new QDesignerGridLayout( gb->layout() );
	    MetaDataBase::setMargin( gb, margin );
	    MetaDataBase::setSpacing( gb, spacing );
	    l->setAlignment( AlignTop );
	    return l;
	default:
	    return 0;
	}
    } else {
	if ( layout ) {
	    QLayout *l;
	    switch ( type ) {
	    case HBox:
		l = new QHBoxLayout( layout );
		l->setSpacing( spacing );
		l->setMargin( margin );
		return l;
	    case VBox:
		l = new QVBoxLayout( layout );
		l->setSpacing( spacing );
		l->setMargin( margin );
		return l;
	    case Grid: {
		l = new QDesignerGridLayout( layout );
		l->setSpacing( spacing );
		l->setMargin( margin );
		return l;
	    }
	    default:
		return 0;
	    }
	} else {
	    QLayout *l;
	    switch ( type ) {
	    case HBox:
		l = new QHBoxLayout( widget );
		if ( widget ) {
		    MetaDataBase::setMargin( widget, margin );
		    MetaDataBase::setSpacing( widget, spacing );
		} else {
		    l->setMargin( margin );
		    l->setSpacing( margin );
		}
		return l;
	    case VBox:
		l = new QVBoxLayout( widget );
		if ( widget ) {
		    MetaDataBase::setMargin( widget, margin );
		    MetaDataBase::setSpacing( widget, spacing );
		} else {
		    l->setMargin( margin );
		    l->setSpacing( margin );
		}
		return l;
	    case Grid: {
		l = new QDesignerGridLayout( widget );
		if ( widget ) {
		    MetaDataBase::setMargin( widget, margin );
		    MetaDataBase::setSpacing( widget, spacing );
		} else {
		    l->setMargin( margin );
		    l->setSpacing( margin );
		}
		return l;
	    }
	    default:
		return 0;
	    }
	}
    }
    return 0;
}

void WidgetFactory::deleteLayout( QWidget *widget )
{
    if ( !widget )
	return;

    if ( widget->inherits( "QTabWidget" ) )
	widget = ((QTabWidget*)widget)->currentPage();
    if ( widget->inherits( "QWizard" ) )
	widget = ((QWizard*)widget)->currentPage();
    if ( widget->inherits( "QWidgetStack" ) )
	widget = ((QWidgetStack*)widget)->visibleWidget();
    delete widget->layout();
}

FormWindow *find_formwindow( QWidget *w )
{
    if ( !w )
	return 0;
    while ( w ) {
	if ( w->inherits( "FormWindow" ) )
	    return (FormWindow*)w;
	w = w->parentWidget();
    }
    return 0;
}

/*!  Factory functions for creating a widget of the type \a className
  as child of \a parent with the name \a name.

  If \a init is TRUE, some initial default properties are set. This
  has to be in sync with the initChangedProperties() function!
*/

QWidget *WidgetFactory::createWidget( const QString &className, QWidget *parent, const char *name, bool init,
				      const QRect *r, Qt::Orientation orient )
{
    if ( className == "QPushButton" ) {
	QPushButton *b = 0;
	if ( init ) {
	    b = new QDesignerPushButton( parent, name );
	    b->setText( QString::fromLatin1( name ) );
	} else {
	    b = new QDesignerPushButton( parent, name );
	}
	b->setAutoDefault( TRUE );
	return b;
    } else if ( className == "QToolButton" ) {
	if ( init ) {
	    QDesignerToolButton *tb = new QDesignerToolButton( parent, name );
	    tb->setText( "..." );
	    return tb;
	}
	return new QDesignerToolButton( parent, name );
    } else if ( className == "QCheckBox" ) {
	if ( init ) {
	    QDesignerCheckBox *cb = new QDesignerCheckBox( parent, name );
	    cb->setText( QString::fromLatin1( name ) );
	    return cb;
	}
	return new QDesignerCheckBox( parent, name );
    } else if ( className == "QRadioButton" ) {
	if ( init ) {
	    QDesignerRadioButton *rb = new QDesignerRadioButton( parent, name );
	    rb->setText( QString::fromLatin1( name ) );
	    return rb;
	}
	return new QDesignerRadioButton( parent, name );
    } else if ( className == "QGroupBox" ) {
	if ( init )
	    return new QGroupBox( QString::fromLatin1( name ), parent, name );
	return new QGroupBox( parent, name );
    } else if ( className == "QButtonGroup" ) {
	if ( init )
	    return new QButtonGroup( QString::fromLatin1( name ), parent, name );
	return new QButtonGroup( parent, name );
    } else if ( className == "QIconView" ) {
#if defined(QT_MODULE_ICONVIEW)
	QIconView* iv = new QIconView( parent, name );
	if ( init )
	    (void) new QIconViewItem( iv, MainWindow::tr( "New Item" ) );
	return iv;
#else
	return 0;
#endif
    } else if ( className == "QTable" ) {
#if defined(QT_MODULE_TABLE)
	return new QTable( 3, 3, parent, name );
#else
	return 0;
#endif
    } else if ( className == "QListBox" ) {
	QListBox* lb = new QListBox( parent, name );
	if ( init ) {
	    lb->insertItem( MainWindow::tr( "New Item" ) );
	    lb->setCurrentItem( 0 );
	}
	return lb;
    } else if ( className == "QListView" ) {
	QListView *lv = new QListView( parent, name );
	lv->setSorting( -1 );
	if ( init ) {
	    lv->addColumn( MainWindow::tr( "Column 1" ) );
	    lv->setCurrentItem( new QListViewItem( lv, MainWindow::tr( "New Item" ) ) );
	}
	return lv;
    } else if ( className == "QLineEdit" )
	return new QLineEdit( parent, name );
    else if ( className == "QSpinBox" )
	return new QSpinBox( parent, name );
    else if ( className == "QMultiLineEdit" )
	return new QMultiLineEdit( parent, name );
    else if ( className == "QLabel"  || className == "TextLabel" ) {
	QDesignerLabel *l = new QDesignerLabel( parent, name );
	if ( init ) {
	    l->setText( QString::fromLatin1( name ) );
	    MetaDataBase::addEntry( l );
	    MetaDataBase::setPropertyChanged( l, "text", TRUE );
	}
	return l;
    } else if ( className == "PixmapLabel" ) {
	QDesignerLabel *l = new QDesignerLabel( parent, name );
	if ( init ) {
	    l->setPixmap( PixmapChooser::loadPixmap( "qtlogo.png", PixmapChooser::NoSize ) );
	    l->setScaledContents( TRUE );
	    MetaDataBase::addEntry( l );
	    MetaDataBase::setPropertyChanged( l, "pixmap", TRUE );
	    MetaDataBase::setPropertyChanged( l, "scaledContents", TRUE );
	}
	return l;
    } else if ( className == "QLayoutWidget" )
	return new QLayoutWidget( parent, name );
    else if ( className == "QTabWidget" ) {
	QTabWidget *tw = new QDesignerTabWidget( parent, name );
	if ( init ) {
	    FormWindow *fw = find_formwindow( parent );
	    QWidget *w = fw ? new QDesignerWidget( fw, tw, "tab" ) : new QWidget( tw, "tab" );
	    tw->addTab( w, MainWindow::tr("Tab 1") );
	    w = fw ? new QDesignerWidget( fw, tw, "tab" ) : new QWidget( tw, "tab" );
	    MetaDataBase::addEntry( tw );
	    tw->addTab( w, MainWindow::tr("Tab 2") );
	    MetaDataBase::addEntry( tw );
	    MetaDataBase::addEntry( w );
	}
	return tw;
    } else if ( className == "QComboBox" ) {
	return new QComboBox( FALSE, parent, name );
    } else if ( className == "QWidget" ) {
	if ( parent &&
	     ( parent->inherits( "FormWindow" ) || parent->inherits( "QWizard" ) || parent->inherits( "QTabWidget" ) ) ) {
	    FormWindow *fw = find_formwindow( parent );
	    if ( fw ) {
		QDesignerWidget *dw = new QDesignerWidget( fw, parent, name );
		MetaDataBase::addEntry( dw );
		return dw;
	    }
	}
	return new QWidget( parent, name );
    } else if ( className == "QDialog" ) {
	QDialog *dia = 0;
	if ( parent && parent->inherits( "FormWindow" ) )
	    dia = new QDesignerDialog( (FormWindow*)parent, parent, name );
	else
	    dia = new QDialog( parent, name );
#if defined(QT_NON_COMMERCIAL)
	if ( parent && !parent->inherits("MainWindow") )
#else
	if ( parent )
#endif
	    dia->reparent( parent, QPoint( 0, 0 ), TRUE );
	return dia;
    } else if ( className == "QWizard" ) {
	QWizard *wiz = new QDesignerWizard( parent, name );
#if defined(QT_NON_COMMERCIAL)
	if ( parent && !parent->inherits("MainWindow") )
#else
	if ( parent )
#endif
	    wiz->reparent( parent, QPoint( 0, 0 ), TRUE );
	if ( init && parent && parent->inherits( "FormWindow" ) ) {
	    QDesignerWidget *dw = new QDesignerWidget( (FormWindow*)parent, wiz, "page" );
	    MetaDataBase::addEntry( dw );
	    wiz->addPage( dw, FormWindow::tr( "Page" ) );
	    QTimer::singleShot( 0, wiz, SLOT( next() ) );
	}
	return wiz;
    } else if ( className == "Spacer" ) {
	Spacer *s = new Spacer( parent, name );
	MetaDataBase::addEntry( s );
	MetaDataBase::setPropertyChanged( s, "orientation", TRUE );
	MetaDataBase::setPropertyChanged( s, "sizeType", TRUE );
	if ( !r )
	    return s;
	if ( !r->isValid() || r->width() < 2 && r->height() < 2 )
	    s->setOrientation( orient );
	else if ( r->width() < r->height() )
	    s->setOrientation( Qt::Vertical );
	else
	    s->setOrientation( Qt::Horizontal );
	return s;
    } else if ( className == "QLCDNumber" )
	return new QLCDNumber( parent, name );
    else if ( className == "QProgressBar" )
	return new QProgressBar( parent, name );
    else if ( className == "QTextView" )
	return new QTextView( parent, name );
    else if ( className == "QTextBrowser" )
	return new QTextBrowser( parent, name );
    else if ( className == "QDial" )
	return new QDial( parent, name );
    else if ( className == "QSlider" ) {
	QSlider *s = new QSlider( parent, name );
	if ( !r )
	    return s;
	if ( !r->isValid() || r->width() < 2 && r->height() < 2 )
	    s->setOrientation( orient );
	else if ( r->width() > r->height() )
	    s->setOrientation( Qt::Horizontal );
	MetaDataBase::addEntry( s );
	MetaDataBase::setPropertyChanged( s, "orientation", TRUE );
	return s;
    } else if ( className == "QFrame" ) {
	if ( !init )
	    return new QFrame( parent, name );
	QFrame *f = new QFrame( parent, name );
	f->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );
	return f;
    } else if ( className == "Line" ) {
	Line *l = new Line( parent, name );
	MetaDataBase::addEntry( l );
	MetaDataBase::setPropertyChanged( l, "orientation", TRUE );
	if ( !r )
	    return l;
	if ( !r->isValid() || r->width() < 2 && r->height() < 2 )
	    l->setOrientation( orient );
	else if ( r->width() < r->height() )
	    l->setOrientation( Qt::Vertical );
	return l;
    }

    QWidget *w = qt_create_kde_widget( className, parent, name, init );
    if ( w )
	return w;

    return 0;
}



/*!  Find out which type the layout of the widget is. Returns \c HBox,
  \c VBox, \c Grid or \c NoLayout.  \a layout points to this
  QWidget::layout() of \a w or to 0 after the function call.
*/

WidgetFactory::LayoutType WidgetFactory::layoutType( QWidget *w, QLayout *&layout )
{
    layout = 0;

    if ( w && w->inherits( "QTabWidget" ) )
	w = ((QTabWidget*)w)->currentPage();
    if ( w && w->inherits( "QWizard" ) )
	w = ((QWizard*)w)->currentPage();
    if ( w && w->inherits( "QWidgetStack" ) )
	w = ((QWidgetStack*)w)->visibleWidget();

    if ( !w || !w->layout() )
	return NoLayout;
    QLayout *lay = w->layout();

    if ( w->inherits( "QGroupBox" ) ) {
	QObjectList *l = lay->queryList( "QLayout" );
	if ( l && l->first() )
	    lay = (QLayout*)l->first();
	delete l;
    }
    layout = lay;

    if ( lay->inherits( "QHBoxLayout" ) )
	return HBox;
    else if ( lay->inherits( "QVBoxLayout" ) )
	return VBox;
    else if ( lay->inherits( "QGridLayout" ) )
	return Grid;
    return NoLayout;
}

/*!
  \overload
*/
WidgetFactory::LayoutType WidgetFactory::layoutType( QLayout *layout )
{
    if ( layout->inherits( "QHBoxLayout" ) )
	return HBox;
    else if ( layout->inherits( "QVBoxLayout" ) )
	return VBox;
    else if ( layout->inherits( "QGridLayout" ) )
	return Grid;
    return NoLayout;
}

/*!
  \overload
*/
WidgetFactory::LayoutType WidgetFactory::layoutType( QWidget *w )
{
    QLayout *l = 0;
    return layoutType( w, l );
}


QWidget *WidgetFactory::layoutParent( QLayout *layout )
{
    QObject *o = layout;
    while ( o ) {
	if ( o->isWidgetType() )
	    return (QWidget*)o;
	o = o->parent();
    }
    return 0;
}

/*!  Returns the widget into which children should be inserted when \a
  w is a container known to the designer.

  Usually that is \a w itself, sometimes it is different (e.g. a
  tabwidget is known to the designer as a container but the child
  widgets should be inserted into the current page of the
  tabwidget. So in this case this function returns the current page of
  the tabwidget.)
 */
QWidget* WidgetFactory::containerOfWidget( QWidget *w )
{
    if ( !w )
	return w;
    if ( w->inherits( "QTabWidget" ) )
	return ((QTabWidget*)w)->currentPage();
    if ( w->inherits( "QWizard" ) )
	return ((QWizard*)w)->currentPage();
    if ( w->inherits( "QWidgetStack" ) )
	return ((QWidgetStack*)w)->visibleWidget();
    return w;
}

/*!  Returns the actual designer widget of the container \a w. This is
  normally \a w itself, but might be a parent or grand parent of \a w
  (e.g. when working with a tabwidget and \a w is the container which
  contains and layouts childs, but the actual widget known to the
  designer is the tabwidget which is the parent of \a w. So this
  function returns the tabwidget then.)
*/

QWidget* WidgetFactory::widgetOfContainer( QWidget *w )
{
    if ( w->parentWidget() && w->parentWidget()->inherits( "QWidgetStack" ) )
	w = w->parentWidget();
    while ( w ) {
	if ( WidgetDatabase::isContainer( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ) ) ||
	     w && w->parentWidget() && w->parentWidget()->inherits( "FormWindow" ) )
	    return w;
	w = w->parentWidget();
    }
    return w;
}

/*!
  Returns whether \a o is a passive interactor or not.
 */
bool WidgetFactory::isPassiveInteractor( QObject* o )
{
    if ( o->inherits( "QTabBar" ) )
	return TRUE;
    else if ( o->inherits( "QSizeGrip" ) )
	return TRUE;
    else if ( o->inherits( "QToolButton" ) && o->parent() && o->parent()->inherits( "QTabBar" ) )
	return TRUE;
    else if ( o->parent() && o->parent()->inherits( "QWizard" ) && o->inherits( "QPushButton" ) )
	return TRUE;

    return FALSE;
}


/*!
  Returns the class name of object \a o that should be used for externally (i.e. for saving)
 */
const char* WidgetFactory::classNameOf( QObject* o )
{
    if ( o->inherits( "QDesignerTabWidget" ) )
	return "QTabWidget";
    else if ( o->inherits( "QDesignerDialog" ) )
	return "QDialog";
    else if ( o->inherits( "QDesignerWidget" ) )
	return "QWidget";
    else if ( o->inherits( "CustomWidget" ) )
	return ( (CustomWidget*)o )->realClassName().latin1();
    else if ( o->inherits( "QDesignerLabel" ) )
	return "QLabel";
    else if ( o->inherits( "QDesignerWizard" ) )
	return "QWizard";
    else if ( o->inherits( "QDesignerPushButton" ) )
	return "QPushButton";
    else if ( o->inherits( "QDesignerToolButton" ) )
	return "QToolButton";
    else if ( o->inherits( "QDesignerRadioButton" ) )
	return "QRadioButton";
    else if ( o->inherits( "QDesignerCheckBox" ) )
	return "QCheckBox";
    return o->className();
}

/*!  As some properties are set by default when creating a widget this
  functions markes this properties as changed. Has to be in sync with
  createWidget()!
*/

void WidgetFactory::initChangedProperties( QObject *o )
{
    MetaDataBase::setPropertyChanged( o, "name", TRUE );
    MetaDataBase::setPropertyChanged( o, "geometry", TRUE );

    if ( o->inherits( "QPushButton" ) || o->inherits("QRadioButton") || o->inherits( "QCheckBox" ) || o->inherits( "QToolButton" ) )
	MetaDataBase::setPropertyChanged( o, "text", TRUE );
    else if ( o->inherits( "QGroupBox" ) )
	MetaDataBase::setPropertyChanged( o, "title", TRUE );
    else if ( o->isA( "QFrame" ) ) {
	MetaDataBase::setPropertyChanged( o, "frameShadow", TRUE );
	MetaDataBase::setPropertyChanged( o, "frameShape", TRUE );
    } else if ( o->inherits( "QTabWidget" ) || o->inherits( "QWizard" ) ) {
	MetaDataBase::setPropertyChanged( o, "pageTitle", TRUE );
	MetaDataBase::setPropertyChanged( o, "pageName", TRUE );
#ifndef QT_NO_TABLE
    } else if ( o->inherits( "QTable" ) ) {
	MetaDataBase::setPropertyChanged( o, "numRows", TRUE );
	MetaDataBase::setPropertyChanged( o, "numCols", TRUE );
#endif
    }
}

bool WidgetFactory::hasSpecialEditor( int id )
{
    QString className = WidgetDatabase::className( id );

    if ( className.mid( 1 ) == "ListBox" )
	return TRUE;
    if ( className.mid( 1 ) == "ComboBox" )
	return TRUE;
    if ( className.mid( 1 ) == "ListView" )
	return TRUE;
    if ( className.mid( 1 ) == "IconView" )
	return TRUE;
    if ( className == "QMultiLineEdit" )
	return TRUE;

    return FALSE;
}

bool WidgetFactory::hasItems( int id )
{
    QString className = WidgetDatabase::className( id );

    if ( className.mid( 1 ) == "ListBox" || className.mid( 1 ) == "ListView" ||
	 className.mid( 1 ) == "IconView" || className.mid( 1 ) == "ComboBox" )
	return TRUE;

    return FALSE;
}

void WidgetFactory::editWidget( int id, QWidget *parent, QWidget *editWidget, FormWindow *fw )
{
    QString className = WidgetDatabase::className( id );

    if ( className.mid( 1 ) == "ListBox" ) {
	if ( !editWidget->inherits( "QListBox" ) )
	    return;
	ListBoxEditor *e = new ListBoxEditor( parent, editWidget, fw );
	e->exec();
	delete e;
	return;
    }

    if ( className.mid( 1 ) == "ComboBox" ) {
	if ( !editWidget->inherits( "QComboBox" ) )
	    return;
	QComboBox *cb = (QComboBox*)editWidget;
	ListBoxEditor *e = new ListBoxEditor( parent, cb->listBox(), fw );
	e->exec();
	delete e;
	cb->update();
	return;
    }

    if ( className.mid( 1 ) == "ListView" ) {
	if ( !editWidget->inherits( "QListView" ) )
	    return;
	QListView *lv = (QListView*)editWidget;
	ListViewEditor *e = new ListViewEditor( parent, lv, fw );
	e->exec();
	delete e;
	return;
    }

    if ( className.mid( 1 ) == "IconView" ) {
	if ( !editWidget->inherits( "QIconView" ) )
	    return;
	IconViewEditor *e = new IconViewEditor( parent, editWidget, fw );
	e->exec();
	delete e;
	return;
    }

    if ( className == "QMultiLineEdit" ) {
	MultiLineEditor *e = new MultiLineEditor( parent, editWidget, fw );
	e->exec();
	delete e;
	return;
    }
}

bool WidgetFactory::canResetProperty( QWidget *w, const QString &propName )
{
    if ( propName == "name" || propName == "geometry" )
	return FALSE;
    QStringList l = *changedProperties->find( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( w ) ) );
    return l.findIndex( propName ) == -1;
}

bool WidgetFactory::resetProperty( QWidget *w, const QString &propName )
{
    const QMetaProperty *p = w->metaObject()->property( propName, TRUE );
    if (!p || ( p->reset == 0 ) )
	return FALSE;

    typedef void (QObject::*ProtoVoid)() const;
    ProtoVoid m = (ProtoVoid)p->reset;

    (w->*m)();
    return TRUE;
}

QVariant WidgetFactory::defaultValue( QWidget *w, const QString &propName )
{
    if ( propName == "wordwrap" ) {
	int v = defaultValue( w, "alignment" ).toInt();
	return QVariant( ( v & WordBreak ) == WordBreak, 0 );
    } else if ( propName == "toolTip" || propName == "whatsThis" ) {
	return QVariant( QString::fromLatin1( "" ) );
    } else if ( w->inherits( "CustomWidget" ) ) {
	return QVariant();
    }
    return *( *defaultProperties->find( WidgetDatabase::idFromClassName( classNameOf( w ) ) ) ).find( propName );
}

QString WidgetFactory::defaultCurrentItem( QWidget *w, const QString &propName )
{
    const QMetaProperty *p = w->metaObject()->property( propName, TRUE );
    if ( !p ) {
	int v = defaultValue( w, "alignment" ).toInt();
	if ( propName == "hAlign" ) {
	    if ( ( v & AlignLeft ) == AlignLeft )
		return "AlignLeft";
	    if ( ( v & AlignCenter ) == AlignCenter || ( v & AlignHCenter ) == AlignHCenter )
		return "AlignHCenter";
	    if ( ( v & AlignRight ) == AlignRight )
		return "AlignRight";
	} else if ( propName == "vAlign" ) {
	    if ( ( v & AlignTop ) == AlignTop )
		return "AlignTop";
	    if ( ( v & AlignCenter ) == AlignCenter || ( v & AlignVCenter ) == AlignVCenter )
		return "AlignVCenter";
	    if ( ( v & AlignBottom ) == AlignBottom )
		return "AlignBottom";
	}
	return QString::null;
	
    }
    return p->valueToKey( defaultValue( w, propName ).toInt() );
}

QWidget *WidgetFactory::createCustomWidget( QWidget *parent, const char *name, MetaDataBase::CustomWidget *w )
{
    if ( !w )
	return 0;
    return new CustomWidget( parent, name, w );
}

QVariant WidgetFactory::property( QWidget *w, const char *name )
{
    QVariant v = w->property( name );
    if ( v.isValid() )
	return v;
    return MetaDataBase::fakeProperty( w, name );
}

void QDesignerLabel::updateBuddy()
{

    if ( myBuddy.isEmpty() )
	return;

    QObjectList *l = topLevelWidget()->queryList( "QWidget", myBuddy, FALSE, TRUE );
    if ( !l || !l->first() ) {
	delete l;
	return;
    }

    QLabel::setBuddy( (QWidget*)l->first() );
    delete l;
}

void QDesignerWidget::paintEvent( QPaintEvent *e )
{
    formwindow->paintGrid( this, e );
}

void QDesignerDialog::paintEvent( QPaintEvent *e )
{
    formwindow->paintGrid( this, e );
}

QSizePolicy QLayoutWidget::sizePolicy() const
{
    return sp;
}

bool QLayoutWidget::event( QEvent *e )
{
    if ( e && ( e->type() == QEvent::ChildInserted ||
		e->type() == QEvent::ChildRemoved ||
		e->type() == QEvent::LayoutHint ) )
	updateSizePolicy();
    return QWidget::event( e );
}

void QLayoutWidget::updateSizePolicy()
{
    if ( !children() || children()->count() == 0 ) {
	sp = QWidget::sizePolicy();
	return;
    }

    QObjectListIt it( *children() );
    QObject *o;
    QSizePolicy::SizeType vt = QSizePolicy::Preferred;
    QSizePolicy::SizeType ht = QSizePolicy::Preferred;
    while ( ( o = it.current() ) ) {
	++it;
	if ( !o->inherits( "QWidget" ) || ( (QWidget*)o )->testWState( WState_ForceHide ) )
	    continue;
	QWidget *w = (QWidget*)o;
	if ( w->sizePolicy().horData() == QSizePolicy::Expanding ||
	     w->sizePolicy().horData() == QSizePolicy::MinimumExpanding )
	    ht = QSizePolicy::Expanding;
	else if ( w->sizePolicy().horData() == QSizePolicy::Fixed && ht != QSizePolicy::Expanding )
	    ht = QSizePolicy::Fixed;
	if ( w->sizePolicy().verData() == QSizePolicy::Expanding ||
	     w->sizePolicy().verData() == QSizePolicy::MinimumExpanding )
	    vt = QSizePolicy::Expanding;
	else if ( w->sizePolicy().verData() == QSizePolicy::Fixed && vt != QSizePolicy::Expanding )
	    vt = QSizePolicy::Fixed;
    }

    sp = QSizePolicy( ht, vt );
    if ( layout() )
	layout()->invalidate();
    updateGeometry();
}

void CustomWidget::paintEvent( QPaintEvent *e )
{
    if ( parentWidget() && parentWidget()->inherits( "FormWindow" ) ) {
	( (FormWindow*)parentWidget() )->paintGrid( this, e );
    } else {
	QPainter p( this );
	p.fillRect( rect(), colorGroup().dark() );
	p.drawPixmap( ( width() - cusw->pixmap->width() ) / 2,
		      ( height() - cusw->pixmap->height() ) / 2,
		      *cusw->pixmap );
    }
}

