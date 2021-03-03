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

#include "resource.h"
#include "defs.h"
#include "metadatabase.h"
#include "formwindow.h"
#include "mainwindow.h"
#include "qdom.h"
#include <widgetdatabase.h>
#include "widgetfactory.h"
#include "layout.h"
#include <domtool.h>
#include "command.h"
#include "pixmapchooser.h"

#include <qfile.h>
#include <qtextstream.h>
#include <qobject.h>
#include <qwidget.h>
#include <qobjectlist.h>
#include <qmetaobject.h>
#include <qworkspace.h>
#include <qtabwidget.h>
#include <qapplication.h>
#include <qbuffer.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qtabwidget.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qwidgetstack.h>
#include <qtabbar.h>
#include <qheader.h>
#include <qlistview.h>
#include <qiconview.h>
#include <qlabel.h>
#include <qwizard.h>
#include <qtextcodec.h>
#include <zlib.h>

static QString makeIndent( int indent )
{
    QString s;
    s.fill( ' ', indent * 4 );
    return s;
}

static QString entitize( const QString &s )
{
    QString s2 = s;
#if 0 // don't need that (RS)
    s2 = s2.replace( QRegExp( "\"" ), "&quot;" );
    s2 = s2.replace( QRegExp( "'" ), "&apos;" );
#endif
    s2 = s2.replace( QRegExp( "&" ), "&amp;" );
    s2 = s2.replace( QRegExp( ">" ), "&gt;" );
    s2 = s2.replace( QRegExp( "<" ), "&lt;" );
    return s2;
}

static QString mkBool( bool b )
{
    return b? "true" : "false";
}

/*!
  \class Resource resource.h
  \brief Class for saving/loading, etc. forms

  This class is used for saving and loading forms, code generation,
  transferring data of widgets over the clipboard, etc..

*/


Resource::Resource( QStyle* s, QPalette* p )
    : style( s ), pal( p )
{
    mainwindow = 0;
    previewMode = TRUE;
    formwindow = 0;
    toplevel = 0;
    copying = FALSE;
    pasting = FALSE;
}

Resource::Resource( MainWindow* mw, QStyle* s, QPalette* p )
    : mainwindow( mw ), style( s ), pal( p )
{
    formwindow = 0;
    toplevel = 0;
    previewMode = FALSE;
    copying = FALSE;
    pasting = FALSE;
}

void Resource::setWidget( FormWindow *w )
{
    formwindow = w;
    toplevel = w;
}

QWidget *Resource::widget() const
{
    return toplevel;
}

bool Resource::load( const QString& filename )
{
    if ( filename.isEmpty() )
	return FALSE;
    mainContainerSet = FALSE;

    QFile f( filename );
    f.open( IO_ReadOnly );

    bool b = load( &f );
    f.close();

    if ( formwindow )
	formwindow->setFileName( filename );

    return b;
}

bool Resource::load( QIODevice* dev, QValueList<Image> *imgs )
{
    QDomDocument doc;
    if ( !doc.setContent( dev ) ) {
	return FALSE;
    }

    if ( !previewMode ) {
	toplevel = formwindow = new FormWindow( mainwindow->workSpace(), 0 );
	formwindow->setMainWindow( mainwindow );
	MetaDataBase::addEntry( formwindow );
    }

    QDomElement firstWidget = doc.firstChild().toElement().firstChild().toElement();

    while ( firstWidget.tagName() != "widget" ) {
	if ( firstWidget.tagName() == "include" ) {
	    MetaDataBase::Include inc;
	    inc.location = "global";
	    if ( firstWidget.attribute( "location" ) == "local" )
		inc.location = "local";
	    inc.header = firstWidget.firstChild().toText().data();
	    metaIncludes.append( inc );
	} else if ( firstWidget.tagName() == "comment" ) {
	    metaInfo.comment = firstWidget.firstChild().toText().data();
	} else if ( firstWidget.tagName() == "forward" ) {
	    metaForwards << firstWidget.firstChild().toText().data();
	} else if ( firstWidget.tagName() == "author" ) {
	    metaInfo.author = firstWidget.firstChild().toText().data();
	} else if ( firstWidget.tagName() == "class" ) {
	    metaInfo.className = firstWidget.firstChild().toText().data();
	} else if ( firstWidget.tagName() == "pixmapfunction" ) {
	    if ( formwindow ) {
		formwindow->setSavePixmapInline( FALSE );
		formwindow->setPixmapLoaderFunction( firstWidget.firstChild().toText().data() );
	    }
	}
	
	firstWidget = firstWidget.nextSibling().toElement();
    }
    QDomElement connections = firstWidget;
    while ( connections.tagName() != "connections" && !connections.isNull() )
	connections = connections.nextSibling().toElement();

    QDomElement imageCollection = firstWidget;
    images.clear();
    while ( imageCollection.tagName() != "images" && !imageCollection.isNull() )
	imageCollection = imageCollection.nextSibling().toElement();

    QDomElement customWidgets = firstWidget;
    while ( customWidgets.tagName() != "customwidgets" && !customWidgets.isNull() )
	customWidgets = customWidgets.nextSibling().toElement();

    QDomElement tabOrder = firstWidget;
    while ( tabOrder.tagName() != "tabstops" && !tabOrder.isNull() )
	tabOrder = tabOrder.nextSibling().toElement();

    if ( !imageCollection.isNull() )
	loadImageCollection( imageCollection );
    if ( images.isEmpty() && imgs )
	images = *imgs;
    if ( !customWidgets.isNull() )
	loadCustomWidgets( customWidgets, this );

#if defined (QT_NON_COMMERCIAL)    
    QWidget *w = (QWidget*)createObject( firstWidget, !previewMode ? (QWidget*)formwindow : MainWindow::self, 0);
    if ( !w )
	return FALSE;
    if ( previewMode )
	w->reparent( MainWindow::self, Qt::WType_TopLevel,  w->pos(), TRUE );	    
#else
    if ( !createObject( firstWidget, !previewMode ? formwindow : 0, 0) )
	return FALSE;
#endif

    if ( !connections.isNull() )
	loadConnections( connections );
    if ( !tabOrder.isNull() )
	loadTabOrder( tabOrder );

    if ( formwindow ) {
	MetaDataBase::setIncludes( formwindow, metaIncludes );
	MetaDataBase::setForwards( formwindow, metaForwards );
	metaInfo.classNameChanged = metaInfo.className != QString( formwindow->name() );
	MetaDataBase::setMetaInfo( formwindow, metaInfo );
    }

    if ( previewMode )
	MetaDataBase::doConnections( toplevel );

    if ( mainwindow && formwindow )
	mainwindow->insertFormWindow( formwindow );

    return TRUE;
}

bool Resource::save( const QString& filename )
{
    if ( !formwindow || filename.isEmpty() )
	return FALSE;

    QFile f( filename );
    if ( !f.open( IO_WriteOnly ) )
	return FALSE;
    bool b = save( &f );
    f.close();
    return b;
}

bool Resource::save( QIODevice* dev, bool saveImages, QValueList<Image> *imgs )
{
    if ( !formwindow )
	return FALSE;

    QTextStream ts( dev );
    ts.setCodec( QTextCodec::codecForName( "UTF-8" ) );

    ts << "<!DOCTYPE UI><UI>" << endl;
    saveMetaInfo( ts, 0 );
    saveObject( formwindow->mainContainer(), 0, ts, 0 );
    if ( !MetaDataBase::customWidgets()->isEmpty() && !usedCustomWidgets.isEmpty() )
	saveCustomWidgets( ts, 0 );
    if ( !images.isEmpty() && saveImages )
	saveImageCollection( ts, 0 );
    if ( !MetaDataBase::connections( formwindow ).isEmpty() || !MetaDataBase::slotList( formwindow ).isEmpty() )
	saveConnections( ts, 0 );
    saveTabOrder( ts, 0 );
    ts << "</UI>" << endl;

    if ( imgs )
	*imgs = images;
    images.clear();

    return TRUE;
}

QString Resource::copy()
{
    if ( !formwindow )
	return QString::null;

    copying = TRUE;
    QString s;
    QTextOStream ts( &s );

    ts << "<!DOCTYPE UI-SELECTION><UI-SELECTION>" << endl;
    QWidgetList widgets = formwindow->selectedWidgets();
    QWidgetList tmp( widgets );
    for ( QWidget *w = widgets.first(); w; w = widgets.next() ) {
	QWidget *p = w->parentWidget();
	bool save = TRUE;
	while ( p ) {
	    if ( tmp.findRef( p ) != -1 ) {
		save = FALSE;
		break;
	    }
	    p = p->parentWidget();
	}
	if ( save )
	    saveObject( w, 0, ts, 0 );
    }
    if ( !MetaDataBase::customWidgets()->isEmpty() && !usedCustomWidgets.isEmpty() )
	saveCustomWidgets( ts, 0 );
    if ( !images.isEmpty() )
	saveImageCollection( ts, 0 );
    ts << "</UI-SELECTION>" << endl;

    return s;
}


void Resource::paste( const QString &cb, QWidget *parent )
{
    if ( !formwindow )
	return;
    mainContainerSet = TRUE;

    pasting = TRUE;
    QBuffer buf( QCString( cb.utf8() ) );
    buf.open( IO_ReadOnly );
    QDomDocument doc;
    doc.setContent( &buf );

    QDomElement firstWidget = doc.firstChild().toElement().firstChild().toElement();

    QDomElement imageCollection = firstWidget;
    images.clear();
    while ( imageCollection.tagName() != "images" && !imageCollection.isNull() )
	imageCollection = imageCollection.nextSibling().toElement();

    QDomElement customWidgets = firstWidget;
    while ( customWidgets.tagName() != "customwidgets" && !customWidgets.isNull() )
	customWidgets = customWidgets.nextSibling().toElement();

    if ( !imageCollection.isNull() )
	loadImageCollection( imageCollection );
    if ( !customWidgets.isNull() )
	loadCustomWidgets( customWidgets, this );

    QWidgetList widgets;
    formwindow->clearSelection( FALSE );
    formwindow->setPropertyShowingBlocked( TRUE );
    formwindow->clearSelection( FALSE );
    while ( !firstWidget.isNull() ) {
	if ( firstWidget.tagName() == "widget" ) {
	    QWidget *w = (QWidget*)createObject( firstWidget, parent, 0 );
	    if ( !w )
		continue;
	    widgets.append( w );
	    int x = w->x() + formwindow->grid().x();
	    int y = w->y() + formwindow->grid().y();
	    if ( w->x() + w->width() > parent->width() )
		x = QMAX( 0, parent->width() - w->width() );
	    if ( w->y() + w->height() > parent->height() )
		y = QMAX( 0, parent->height() - w->height() );
	    if ( x != w->x() || y != w->y() )
		w->move( x, y );
	    formwindow->selectWidget( w );
	} else if ( firstWidget.tagName() == "spacer" ) {
	    QWidget *w = createSpacer( firstWidget, parent, 0, firstWidget.tagName() == "vspacer" ? Qt::Vertical : Qt::Horizontal );
	    if ( !w )
		continue;
	    widgets.append( w );
	    int x = w->x() + formwindow->grid().x();
	    int y = w->y() + formwindow->grid().y();
	    if ( w->x() + w->width() > parent->width() )
		x = QMAX( 0, parent->width() - w->width() );
	    if ( w->y() + w->height() > parent->height() )
		y = QMAX( 0, parent->height() - w->height() );
	    if ( x != w->x() || y != w->y() )
		w->move( x, y );
	    formwindow->selectWidget( w );
	}
	firstWidget = firstWidget.nextSibling().toElement();
    }
    formwindow->setPropertyShowingBlocked( FALSE );
    formwindow->emitShowProperties();
    buf.close();

    PasteCommand *cmd = new PasteCommand( FormWindow::tr( "Paste" ), formwindow, widgets );
    formwindow->commandHistory()->addCommand( cmd );
}

void Resource::saveObject( QObject *obj, QDesignerGridLayout* grid, QTextStream &ts, int indent )
{
    if ( obj && obj->isWidgetType() && ( (QWidget*)obj )->isHidden() )
	return;
    QString closeTag;
    if ( obj->isWidgetType() ) {
	const char* className = WidgetFactory::classNameOf( obj );
	if ( obj->isA( "CustomWidget" ) )
	    usedCustomWidgets << QString( className );
	if ( obj != formwindow && !formwindow->widgets()->find( (QWidget*)obj ) )
	    return; // we don't know anything about this thing

	QString attributes;
	if ( grid ) {
	    QDesignerGridLayout::Item item = grid->items[ (QWidget*)obj ];
	    attributes += QString(" row=\"") + QString::number(item.row) + "\" ";
	    attributes += QString(" column=\"") + QString::number(item.column) + "\" ";
	    if ( item.rowspan * item.colspan != 1 ) {
		attributes += QString(" rowspan=\"") + QString::number(item.rowspan) + "\" ";
		attributes += QString(" colspan=\"") + QString::number(item.colspan) + "\" ";
	    }
	}
	
	if ( qstrcmp( className, "Spacer" ) == 0 ) {
	    closeTag = makeIndent( indent ) + "</spacer>\n";
	    ts << makeIndent( indent ) << "<spacer" << attributes << ">" << endl;
	    ++indent;
	} else {
	    closeTag = makeIndent( indent ) + "</widget>\n";
	    ts << makeIndent( indent ) << "<widget" << attributes << ">" << endl;
	    ++indent;
	    ts << makeIndent( indent ) << "<class>" << className << "</class>" << endl;
	}
	if ( WidgetFactory::hasItems( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( obj ) ) ) )
	    saveItems( obj, ts, indent );
	saveObjectProperties( obj, ts, indent );
    } else {
	// test for other objects we created. Nothing so far.
	return;
    }

    if ( obj->inherits( "QTabWidget" ) ) {
	QTabWidget* tw = (QTabWidget*) obj;
	QObjectList* tmpl = tw->queryList( "QWidgetStack" );
	const QObjectList *l = tmpl->first()->children();
	for ( QListIterator<QObject> it ( *l ); it.current(); ++it ) {
	    if ( !it.current()->isWidgetType() ||
		 qstrcmp( it.current()->className(), "QWidgetStackPrivate::Invisible" ) == 0 ||
		 !( ((QDesignerTabWidget*)tw)->tabBar()->tab(  ((QWidgetStack*)tmpl->first())->id( ((QWidget*)it.current()) ) ) ) )
		continue;
	    if ( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( it.current() ) ) == -1 )
		continue; // we don't know this widget
	    ts << makeIndent( indent ) << "<widget>" << endl;
	    ++indent;
	    ts << makeIndent( indent ) << "<class>" << "QWidget" << "</class>" << endl;

	    ts << makeIndent( indent ) << "<property stdset=\"1\">" << endl;
	    indent++;
	    ts << makeIndent( indent ) << "<name>name</name>" << endl;
	    ts << makeIndent( indent ) << "<cstring>" << entitize( it.current()->name() ) << "</cstring>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</property>" << endl;

	    ts << makeIndent( indent ) << "<attribute>" << endl;
	    indent++;
	    ts << makeIndent( indent ) << "<name>" << "title" << "</name>" << endl;
	    ts << makeIndent( indent ) << "<string>" << entitize( tw->tabLabel( (QWidget*)it.current() ) ) << "</string>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</attribute>" << endl;;
	    saveChildrenOf( it.current(), ts, indent );
	    --indent;
	    ts << makeIndent( indent ) << "</widget>" << endl;
	}
	delete tmpl;
    } else if ( obj->inherits( "QWizard" ) ) {
	QWizard* wiz = (QWizard*)obj;
	QObjectList* tmpl = wiz->queryList( "QWidgetStack" );
	const QObjectList *l = tmpl->first()->children();
	for ( QListIterator<QObject> it ( *l ); it.current(); ++it ) {
	    if ( !it.current()->isWidgetType() ||
		 qstrcmp( it.current()->className(), "QWidgetStackPrivate::Invisible" ) == 0 ||
		 ( (QDesignerWizard*)wiz )->isPageRemoved( (QWidget*)it.current() ) )
		continue;
	    if ( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( it.current() ) ) == -1 )
		continue; // we don't know this widget
	    ts << makeIndent( indent ) << "<widget>" << endl;
	    ++indent;
	    ts << makeIndent( indent ) << "<class>" << "QWidget" << "</class>" << endl;
	
	    ts << makeIndent( indent ) << "<property stdset=\"1\">" << endl;
	    indent++;
	    ts << makeIndent( indent ) << "<name>name</name>" << endl;
	    ts << makeIndent( indent ) << "<cstring>" << entitize( it.current()->name() ) << "</cstring>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</property>" << endl;
	
	    ts << makeIndent( indent ) << "<attribute>" << endl;
	    indent++;
	    ts << makeIndent( indent ) << "<name>" << "title" << "</name>" << endl;
	    ts << makeIndent( indent ) << "<string>" << entitize( wiz->title( (QWidget*)it.current() ) ) << "</string>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</attribute>" << endl;;
	    saveChildrenOf( it.current(), ts, indent );
	    --indent;
	    ts << makeIndent( indent ) << "</widget>" << endl;
	}
	delete tmpl;
    } else {
	saveChildrenOf( obj, ts, indent );
    }

    indent--;
    ts << closeTag;
}

void Resource::saveItems( QObject *obj, QTextStream &ts, int indent )
{
    if ( obj->inherits( "QListBox" ) || obj->inherits( "QComboBox" ) ) {
	QListBox *lb = 0;
	if ( obj->inherits( "QListBox" ) )
	    lb = (QListBox*)obj;
	else
	    lb = ( (QComboBox*)obj )->listBox();
	
	QListBoxItem *i = lb->firstItem();
	for ( ; i; i = i->next() ) {
	    ts << makeIndent( indent ) << "<item>" << endl;
	    indent++;
	    QStringList text;
	    text << i->text();
	    QList<QPixmap> pixmaps;
	    if ( i->pixmap() )
		pixmaps.append( i->pixmap() );
	    saveItem( text, pixmaps, ts, indent );
	    indent--;
	    ts << makeIndent( indent ) << "</item>" << endl;
	}
    } else if ( obj->inherits( "QIconView" ) ) {
	QIconView *iv = (QIconView*)obj;
	
	QIconViewItem *i = iv->firstItem();
	for ( ; i; i = i->nextItem() ) {
	    ts << makeIndent( indent ) << "<item>" << endl;
	    indent++;
	    QStringList text;
	    text << i->text();
	    QList<QPixmap> pixmaps;
	    if ( i->pixmap() )
		pixmaps.append( i->pixmap() );
	    saveItem( text, pixmaps, ts, indent );
	    indent--;
	    ts << makeIndent( indent ) << "</item>" << endl;
	}
    } else if ( obj->inherits( "QListView" ) ) {
	QListView *lv = (QListView*)obj;
	int i;
	for ( i = 0; i < lv->header()->count(); ++i ) {
	    ts << makeIndent( indent ) << "<column>" << endl;
	    indent++;
	    QStringList l;
	    l << lv->header()->label( i );
	    QList<QPixmap> pix;
	    pix.setAutoDelete( TRUE );
	    if ( lv->header()->iconSet( i ) )
		pix.append( new QPixmap( lv->header()->iconSet( i )->pixmap() ) );
	    saveItem( l, pix, ts, indent );
	    ts << makeIndent( indent ) << "<property>" << endl;
	    indent++;
	    ts << makeIndent( indent ) << "<name>clickable</name>" << endl;
	    ts << makeIndent( indent ) << "<bool>" << mkBool( lv->header()->isClickEnabled( i ) )<< "</bool>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</property>" << endl;
	    ts << makeIndent( indent ) << "<property>" << endl;
	    indent++;
	    ts << makeIndent( indent ) << "<name>resizeable</name>" << endl;
	    ts << makeIndent( indent ) << "<bool>" << mkBool( lv->header()->isResizeEnabled( i ) ) << "</bool>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</property>" << endl;
	    indent--;
	    ts << makeIndent( indent ) << "</column>" << endl;
	}
	saveItem( lv->firstChild(), ts, indent - 1 );
    }
}

void Resource::saveItem( QListViewItem *i, QTextStream &ts, int indent )
{
    QListView *lv = i->listView();
    while ( i ) {
	ts << makeIndent( indent ) << "<item>" << endl;
	indent++;

	QList<QPixmap> pixmaps;
	QStringList textes;
	for ( int c = 0; c < lv->columns(); ++c ) {
	    pixmaps.append( i->pixmap( c ) );
	    textes << i->text( c );
	}
	saveItem( textes, pixmaps, ts, indent );
	
	if ( i->firstChild() )
	    saveItem( i->firstChild(), ts, indent );
	
	indent--;
	ts << makeIndent( indent ) << "</item>" << endl;
	i = i->nextSibling();
    }
}

void Resource::savePixmap( const QPixmap &p, QTextStream &ts, int indent )
{
    if ( p.isNull() ) {
	ts << makeIndent( indent ) << "<pixmap></pixmap>" << endl;
	return;
    }

    if ( formwindow && !formwindow->savePixmapInline() )
	ts << makeIndent( indent ) << "<pixmap>" << MetaDataBase::pixmapArgument( formwindow, p.serialNumber() ) << "</pixmap>" << endl;
    else
	ts << makeIndent( indent ) << "<pixmap>" << saveInCollection( p ) << "</pixmap>" << endl;
}

QPixmap Resource::loadPixmap( const QDomElement &e )
{
    QString arg = e.firstChild().toText().data();
    if ( formwindow && !formwindow->savePixmapInline() ) {
	QPixmap pix = PixmapChooser::loadPixmap( "image.xpm" );
	MetaDataBase::setPixmapArgument( formwindow, pix.serialNumber(), arg );
	return pix;
    }
	
    QImage img = loadFromCollection( arg );
    QPixmap pix;
    pix.convertFromImage( img );
    MetaDataBase::setPixmapArgument( formwindow, pix.serialNumber(), arg );
    return pix;
}

void Resource::saveItem( const QStringList &text, const QList<QPixmap> &pixmaps, QTextStream &ts, int indent )
{
    QStringList::ConstIterator it = text.begin();
    for ( ; it != text.end(); ++it ) {
	ts << makeIndent( indent ) << "<property>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<name>text</name>" << endl;
	ts << makeIndent( indent ) << "<string>" << entitize( *it ) << "</string>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</property>" << endl;
    }

    for ( int i = 0; i < (int)pixmaps.count(); ++i ) {
	QPixmap *p = ( (QList<QPixmap>)pixmaps ).at( i );
	ts << makeIndent( indent ) << "<property>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<name>pixmap</name>" << endl;
	if ( p )
	    savePixmap( *p, ts, indent );
	else
	    savePixmap( QPixmap(), ts, indent );
	indent--;
	ts << makeIndent( indent ) << "</property>" << endl;
    }
}

void Resource::saveChildrenOf( QObject* obj, QTextStream &ts, int indent )
{
    const QObjectList *l = obj->children();
    if ( !l )
	return; // no children to save

    QString closeTag;
    // if the widget has a layout we pretend that all widget's childs are childs of the layout - makes the structure nicer
    QLayout *layout = 0;
    QDesignerGridLayout* grid = 0;
    if ( obj->isWidgetType() && WidgetFactory::layoutType( (QWidget*)obj, layout ) != WidgetFactory::NoLayout ) {
	WidgetFactory::LayoutType lay = WidgetFactory::layoutType( (QWidget*)obj, layout );
	switch ( lay ) {
	case WidgetFactory::HBox:
	    closeTag = makeIndent( indent ) + "</hbox>";
	    ts << makeIndent( indent ) << "<hbox>" << endl;
	    ++indent;
	    break;
	case WidgetFactory::VBox:
	    closeTag = makeIndent( indent ) + "</vbox>";
	    ts << makeIndent( indent ) << "<vbox>" << endl;
	    ++indent;
	    break;
	case WidgetFactory::Grid:
	    closeTag = makeIndent( indent ) + "</grid>";
	    ts << makeIndent( indent ) << "<grid>" << endl;
	    ++indent;
	    grid = (QDesignerGridLayout*) layout;
	    break;
	default:
	    break;
	}

	// save properties of layout
	if ( lay != WidgetFactory::NoLayout )
	    saveObjectProperties( layout, ts, indent );
	
    }
	
    for ( QListIterator<QObject> it ( *l ); it.current(); ++it )
	saveObject( it.current(), grid, ts, indent );
    if ( !closeTag.isEmpty() ) {
	indent--;
	ts << closeTag << endl;
    }
}

void Resource::saveObjectProperties( QObject *w, QTextStream &ts, int indent )
{
    QStringList changed;
    if ( w->isWidgetType() ) {
	changed = MetaDataBase::changedProperties( w );
	if ( w->inherits( "Spacer" ) ) {
	    if ( changed.find( "sizeHint" ) == changed.end() )
		changed << "sizeHint";
	}
    } else if ( w->inherits( "QLayout" ) ) { // #### should be cleaner (RS)
	changed << "margin" << "spacing";
    }

    if ( w == formwindow->mainContainer() ) {
	if ( changed.findIndex( "geometry" ) == -1 )
	    changed << "geometry";
	if ( changed.findIndex( "caption" ) == -1 )
	    changed << "caption";
    }

    if ( changed.isEmpty() )
	    return;

    bool inLayout = w != formwindow->mainContainer() && !copying && w->isWidgetType() && ( (QWidget*)w )->parentWidget() &&
		    WidgetFactory::layoutType( ( (QWidget*)w )->parentWidget() ) != WidgetFactory::NoLayout;
	

    QStrList lst = w->metaObject()->propertyNames( !w->inherits( "Spacer" ) );
    for ( QListIterator<char> it( lst ); it.current(); ++it ) {
	if ( changed.find( QString::fromLatin1( it.current() ) ) == changed.end() )
	    continue;
	const QMetaProperty* p = w->metaObject()->property( it.current(), TRUE );
	if ( !p || !p->stored( w ) || ( inLayout && qstrcmp( p->name(), "geometry" ) == 0 ) )
	    continue;
	if ( w->inherits( "QLabel" ) && qstrcmp( p->name(), "pixmap" ) == 0 &&
	     ( !( (QLabel*)w )->pixmap() || ( (QLabel*)w )->pixmap()->isNull() ) )
	    continue;
	if ( qstrcmp( p->name(), "name" ) == 0 )
	    knownNames << w->property( "name" ).toString();
	ts << makeIndent( indent ) << "<property";
	if ( p->testFlags( QMetaProperty::StdSet ) )
	    ts << " stdset=\"1\"";
	ts << ">" << endl;
	indent++;
	ts << makeIndent( indent ) << "<name>" << it.current() << "</name>" << endl;
	
	if ( p->isSetType() ) {
	    saveSetProperty( w, it.current(), QVariant::nameToType( p->type() ), ts, indent );
	} else if ( p->isEnumType() ) {
	    saveEnumProperty( w, it.current(), QVariant::nameToType( p->type() ), ts, indent );
	} else {
	    saveProperty( w, it.current(), w->property( p->name() ), QVariant::nameToType( p->type() ), ts, indent );
	}
	indent--;
	ts << makeIndent( indent ) << "</property>" << endl;
    }

    if ( w->isWidgetType() && MetaDataBase::fakeProperties( w ) ) {
	QMap<QString, QVariant>* fakeProperties = MetaDataBase::fakeProperties( w );
	for ( QMap<QString, QVariant>::Iterator fake = fakeProperties->begin();
	      fake != fakeProperties->end(); ++fake ) {
	    if ( MetaDataBase::isPropertyChanged( w, fake.key() ) ) {
		if ( w->inherits( "CustomWidget" ) ) {
		    MetaDataBase::CustomWidget *cw = ( (CustomWidget*)w )->customWidget();
		    if ( cw && !cw->hasProperty( fake.key().latin1() ) && fake.key() != "toolTip" && fake.key() != "whatsThis" )
			continue;
		}

		ts << makeIndent( indent ) << "<property>" << endl;
		indent++;
		ts << makeIndent( indent ) << "<name>" << fake.key() << "</name>" << endl;
		saveProperty( w, fake.key(), *fake, (*fake).type(), ts, indent );
		indent--;
		ts << makeIndent( indent ) << "</property>" << endl;
	    }
	}
    }
}

void Resource::saveSetProperty( QObject *w, const QString &name, QVariant::Type, QTextStream &ts, int indent )
{
    const QMetaProperty *p = w->metaObject()->property( name, TRUE );
    QStrList l( p->valueToKeys( w->property( name ).toInt() ) );
    QString v;
    for ( uint i = 0; i < l.count(); ++i ) {
	v += l.at( i );
	if ( i < l.count() - 1 )
	    v += "|";
    }
    ts << makeIndent( indent ) << "<set>" << v << "</set>" << endl;
}

void Resource::saveEnumProperty( QObject *w, const QString &name, QVariant::Type, QTextStream &ts, int indent )
{
    const QMetaProperty *p = w->metaObject()->property( name, TRUE );
    ts << makeIndent( indent ) << "<enum>" << p->valueToKey( w->property( name ).toInt() ) << "</enum>" << endl;
}

void Resource::saveProperty( QObject *w, const QString &name, const QVariant &value, QVariant::Type t, QTextStream &ts, int indent )
{
    if ( name == "hAlign" || name =="vAlign" || name == "wordwrap" || name == "layoutMargin" || name =="layoutSpacing" )
	return;
    int num, unum;
    QString comment;
    if ( w && formwindow->widgets()->find( (QWidget*)w ) )
	comment = MetaDataBase::propertyComment( w, name );
    switch ( t ) {
    case QVariant::String:
	ts << makeIndent( indent ) << "<string>" << entitize( value.toString() ) << "</string>" << endl;
	if ( !comment.isEmpty() )
	    ts << makeIndent( indent ) << "<comment>" << entitize( comment ) << "</comment>" << endl;
	break;
    case QVariant::CString:
	ts << makeIndent( indent ) << "<cstring>" << entitize( value.toCString() ).latin1() << "</cstring>" << endl;
	break;
    case QVariant::Bool:
	ts << makeIndent( indent ) << "<bool>" << mkBool( value.toBool() ) << "</bool>" << endl;
	break;
    case QVariant::Int:
	num = value.toInt();
	if ( w && w->inherits( "QLayout" ) ) {
	    if ( name == "spacing" )
		num = MetaDataBase::spacing( WidgetFactory::containerOfWidget( WidgetFactory::layoutParent( (QLayout*)w ) ) );
	    else if ( name == "margin" )
		num = MetaDataBase::margin( WidgetFactory::containerOfWidget( WidgetFactory::layoutParent( (QLayout*)w ) ) );
	}
	ts << makeIndent( indent ) << "<number>" << QString::number( num ) << "</number>" << endl;
	break;
    case QVariant::UInt:
	unum = value.toUInt();
	if ( w && w->inherits( "QLayout" ) ) {
	    if ( name == "spacing" )
		num = MetaDataBase::spacing( WidgetFactory::layoutParent( (QLayout*)w ) );
	    else if ( name == "margin" )
		num = MetaDataBase::margin( WidgetFactory::layoutParent( (QLayout*)w ) );
	}
	ts << makeIndent( indent ) << "<number>" << QString::number( unum ) << "</number>" << endl;
	break;
    case QVariant::Rect: {
	QVariant v( value );
	ts << makeIndent( indent ) << "<rect>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<x>" << QString::number( v.toRect().x() ) << "</x>" << endl;
	ts << makeIndent( indent ) << "<y>" << QString::number( v.toRect().y() ) << "</y>" << endl;
	ts << makeIndent( indent ) << "<width>" << QString::number( v.toRect().width() ) << "</width>" << endl;
	ts << makeIndent( indent ) << "<height>" << QString::number( v.toRect().height() ) << "</height>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</rect>" << endl;
    } break;
    case QVariant::Point: {
	QVariant v( value );
	ts << makeIndent( indent ) << "<point>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<x>" << QString::number( v.toPoint().x() ) << "</x>" << endl;
	ts << makeIndent( indent ) << "<y>" << QString::number( v.toPoint().y() ) << "</y>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</point>" << endl;
    } break;
    case QVariant::Size: {
	QVariant v( value );
	ts << makeIndent( indent ) << "<size>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<width>" << QString::number( v.toSize().width() ) << "</width>" << endl;
	ts << makeIndent( indent ) << "<height>" << QString::number( v.toSize().height() ) << "</height>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</size>" << endl;
    } break;
    case QVariant::Color: {
	QVariant v( value );
	ts << makeIndent( indent ) << "<color>" << endl;
	indent++;
	saveColor( ts, indent, v.toColor() );
	indent--;
	ts << makeIndent( indent ) << "</color>" << endl;
    } break;
    case QVariant::Font: {
	QVariant v( value );
	ts << makeIndent( indent ) << "<font>" << endl;
	QFont f( qApp->font() );
	if ( w && w->isWidgetType() && ((QWidget*)w)->parentWidget() )
	    f = ((QWidget*)w)->parentWidget()->font();
	QFont f2( v.toFont() );
	indent++;
	if ( f.family() != f2.family() )
	    ts << makeIndent( indent ) << "<family>" << f2.family() << "</family>" << endl;
	if ( f.pointSize() != f2.pointSize() )
	    ts << makeIndent( indent ) << "<pointsize>" << QString::number( f2.pointSize() ) << "</pointsize>" << endl;
	if ( f.bold() != f2.bold() )
	    ts << makeIndent( indent ) << "<bold>" << QString::number( (int)f2.bold() ) << "</bold>" << endl;
	if ( f.italic() != f2.italic() )
	    ts << makeIndent( indent ) << "<italic>" << QString::number( (int)f2.italic() ) << "</italic>" << endl;
	if ( f.underline() != f2.underline() )
	    ts << makeIndent( indent ) << "<underline>" << QString::number( (int)f2.underline() ) << "</underline>" << endl;
	if ( f.strikeOut() != f2.strikeOut() )
	    ts << makeIndent( indent ) << "<strikeout>" << QString::number( (int)f2.strikeOut() ) << "</strikeout>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</font>" << endl;
    } break;
    case QVariant::SizePolicy: {
	QSizePolicy sp( value.toSizePolicy() );
	ts << makeIndent( indent ) << "<sizepolicy>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<hsizetype>" << (int)sp.horData() << "</hsizetype>" << endl;
	ts << makeIndent( indent ) << "<vsizetype>" << (int)sp.verData() << "</vsizetype>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</sizepolicy>" << endl;
	break;
    }
    case QVariant::Pixmap:
	savePixmap( value.toPixmap(), ts, indent );
	break;
    case QVariant::IconSet:
	ts << makeIndent( indent ) << "<iconset>" << saveInCollection( value.toIconSet().pixmap() ) << "</iconset>" << endl;
	break;
    case QVariant::Image:
	ts << makeIndent( indent ) << "<image>" << saveInCollection( value.toImage() ) << "</image>" << endl;
    break;
    case QVariant::Palette: {
	QPalette p( value.toPalette() );
	ts << makeIndent( indent ) << "<palette>" << endl;
	indent++;

	ts << makeIndent( indent ) << "<active>" << endl;
	indent++;
	saveColorGroup( ts, indent, p.active() );
	indent--;
	ts << makeIndent( indent ) << "</active>" << endl;
	
	ts << makeIndent( indent ) << "<disabled>" << endl;
	indent++;
	saveColorGroup( ts, indent, p.disabled() );
	indent--;
	ts << makeIndent( indent ) << "</disabled>" << endl;
	
	ts << makeIndent( indent ) << "<inactive>" << endl;
	indent++;
	saveColorGroup( ts, indent, p.inactive() );
	indent--;
	ts << makeIndent( indent ) << "</inactive>" << endl;

	indent--;
	ts << makeIndent( indent ) << "</palette>" << endl;
    } break;
    case QVariant::Cursor:
	ts << makeIndent( indent ) << "<cursor>" << value.toCursor().shape() << "</cursor>" << endl;
	break;
    default:
	qWarning( "saving the property %s of type %d not supported yet", name.latin1(), (int)t );
    }
}

void Resource::saveColorGroup( QTextStream &ts, int indent, const QColorGroup &cg )
{
    for( int r = 0 ; r < QColorGroup::NColorRoles ; r++ ) {
	ts << makeIndent( indent ) << "<color>" << endl;
	indent++;
	saveColor( ts, indent, cg.color( (QColorGroup::ColorRole)r ) );
	indent--;
	ts << makeIndent( indent ) << "</color>" << endl;
	QPixmap* pm = cg.brush( (QColorGroup::ColorRole)r ).pixmap();
	if ( pm && !pm->isNull() )
	    savePixmap( *pm, ts, indent );
    }
}

void Resource::saveColor( QTextStream &ts, int indent, const QColor &c )
{
    ts << makeIndent( indent ) << "<red>" << QString::number( c.red() ) << "</red>" << endl;
    ts << makeIndent( indent ) << "<green>" << QString::number( c.green() ) << "</green>" << endl;
    ts << makeIndent( indent ) << "<blue>" << QString::number( c.blue() ) << "</blue>" << endl;
}

QObject *Resource::createObject( const QDomElement &e, QWidget *parent, QLayout* layout )
{
    lastItem = 0;
    QDomElement n = e.firstChild().toElement();
    QWidget *w = 0; // the widget that got created
    QObject *obj = 0; // gets the properties

    int row = e.attribute( "row" ).toInt();
    int col = e.attribute( "column" ).toInt();
    int rowspan = e.attribute( "rowspan" ).toInt();
    int colspan = e.attribute( "colspan" ).toInt();
    if ( rowspan < 1 )
	rowspan = 1;
    if ( colspan < 1 )
	colspan = 1;
    while ( !n.isNull() ) {
	if ( n.tagName() == "class" ) {
	    QString className = n.firstChild().toText().data();
	    if ( previewMode && !layout && className  == "QLayoutWidget" )
		className = "QWidget";
	    if ( previewMode && layout && className == "QLayoutWidget" ) {
		// hide layout widgets in preview mode
		w = parent;
	    } else {
		obj = WidgetFactory::create( WidgetDatabase::idFromClassName( className ), parent, 0, FALSE );
		if ( !obj )
		    return 0;
		if ( !mainContainerSet ) {
		    if ( formwindow && !previewMode )
			formwindow->setMainContainer( (QWidget*)obj );
		    mainContainerSet = TRUE;
		}
		if ( !obj ) {
		    n = n.firstChild().toElement();
		    continue;
		}
		w = (QWidget*)obj;
		if ( layout ) {
		    switch ( WidgetFactory::layoutType( layout ) ) {
		    case WidgetFactory::HBox:
			( (QHBoxLayout*)layout )->addWidget( w );
			break;
		    case WidgetFactory::VBox:
			( (QVBoxLayout*)layout )->addWidget( w );
			break;
		    case WidgetFactory::Grid:
			( (QDesignerGridLayout*)layout )->addMultiCellWidget( w, row, row + rowspan - 1,
									      col, col + colspan - 1 );
			break;
		    default:
			break;
		    }
		}
	
		if ( !toplevel )
		    toplevel = w;
		layout = 0;
		
		if ( w && formwindow ) {
		    if ( !parent || ( !parent->inherits( "QTabWidget" ) && !parent->inherits( "QWizard" ) ) )
			formwindow->insertWidget( w, pasting );
		    else if ( parent && ( parent->inherits( "QTabWidget" ) || parent->inherits( "QWizard" ) ) )
			MetaDataBase::addEntry( w );
		}
		
		if ( style )
		    w->setStyle( style );
		if ( pal )
		    w->setPalette( *pal );
	    }
	} else if ( n.tagName() == "spacer" ) {
	    createSpacer( n, w, layout, Qt::Horizontal );
	} else if ( n.tagName() == "widget" ) {
	    createObject( n, w, layout );
	} else if ( n.tagName() == "hbox" ) {
	    QLayout *parentLayout = layout;
	    if ( previewMode && layout && layout->inherits( "QGridLayout" ) )
		layout = WidgetFactory::createLayout( 0, 0, WidgetFactory::HBox );
	    else
		layout = WidgetFactory::createLayout( w, layout, WidgetFactory::HBox );
	    obj = layout;
	    n = n.firstChild().toElement();
	    if ( previewMode && parentLayout && parentLayout->inherits( "QGridLayout" ) )
		( (QDesignerGridLayout*)parentLayout )->addMultiCellLayout( layout, row, row + rowspan - 1, col, col + colspan - 1 );
	    continue;
	} else if ( n.tagName() == "grid" ) {
	    QLayout *parentLayout = layout;
	    if ( previewMode && layout && layout->inherits( "QGridLayout" ) )
		layout = WidgetFactory::createLayout( 0, 0, WidgetFactory::Grid );
	    else
		layout = WidgetFactory::createLayout( w, layout, WidgetFactory::Grid );
	    obj = layout;
	    n = n.firstChild().toElement();
	    if ( previewMode && parentLayout && parentLayout->inherits( "QGridLayout" ) )
		( (QDesignerGridLayout*)parentLayout )->addMultiCellLayout( layout, row, row + rowspan - 1, col, col + colspan - 1 );
	    continue;
	} else if ( n.tagName() == "vbox" ) {
	    QLayout *parentLayout = layout;
	    if ( previewMode && layout && layout->inherits( "QGridLayout" ) )
		layout = WidgetFactory::createLayout( 0, 0, WidgetFactory::VBox );
	    else
		layout = WidgetFactory::createLayout( w, layout, WidgetFactory::VBox );
	    obj = layout;
	    n = n.firstChild().toElement();
	    if ( previewMode && parentLayout && parentLayout->inherits( "QGridLayout" ) )
		( (QDesignerGridLayout*)parentLayout )->addMultiCellLayout( layout, row, row + rowspan - 1, col, col + colspan - 1 );
	    continue;
 	} else if ( n.tagName() == "property" ) {
	    QDomElement n2 = n.firstChild().toElement();
	    if ( n2.tagName() == "name" && obj ) {
		setObjectProperty( obj, n2.firstChild().toText().data(), n2.nextSibling().toElement() );
	    }
	} else if ( n.tagName() == "attribute" ) {
	    QDomElement n2 = n.firstChild().toElement();
	    if ( n2.tagName() == "name" ) {
		QString attrib = n2.firstChild().toText().data();
		QVariant v = DomTool::elementToVariant( n2.nextSibling().toElement(), QVariant() );

		if ( parent->inherits( "QTabWidget" ) ) {
		    if ( attrib == "title" )
			( (QTabWidget*)parent )->insertTab( (QWidget*)obj, v.toString() );
		} else if ( parent->inherits( "QWizard" ) ) {
		    if ( attrib == "title" )
			( (QWizard*)parent )->addPage( (QWidget*)obj, v.toString() );
		}
	    }
	} else if ( n.tagName() == "item" ) {
	    createItem( n, w );
	} else if ( n.tagName() == "column" ) {
	    createColumn( n, w );
	}
	
	n = n.nextSibling().toElement();
    }

    return w;
}

void Resource::createColumn( const QDomElement &e, QWidget *widget )
{
    if ( !widget )
	return;

    if ( widget->inherits( "QListView" ) ) {
	QListView *lv = (QListView*)widget;
	QDomElement n = e.firstChild().toElement();
	QPixmap pix;
	bool hasPixmap = FALSE;
	QString txt;
	bool clickable = TRUE, resizeable = TRUE;
	while ( !n.isNull() ) {
	    if ( n.tagName() == "property" ) {
		QDomElement n2 = n.firstChild().toElement();
		if ( n2.tagName() == "name" ) {
		    QString attrib = n2.firstChild().toText().data();
		    QVariant v = DomTool::elementToVariant( n2.nextSibling().toElement(), QVariant() );
		    if ( attrib == "text" )
			txt = v.toString();
		    else if ( attrib == "pixmap" ) {
			pix = loadPixmap( n2.nextSibling().toElement() );
			hasPixmap = TRUE;
		    } else if ( attrib == "clickable" )
			clickable = v.toBool();
		    else if ( attrib == "resizeable" )
			resizeable = v.toBool();
		}
	    }
	    n = n.nextSibling().toElement();
	}
	lv->addColumn( txt );
	int i = lv->header()->count() - 1;
	if ( hasPixmap ) {
	    lv->header()->setLabel( i, pix, txt );
	}
	if ( !clickable )
	    lv->header()->setClickEnabled( clickable, i );
	if ( !resizeable )
	    lv->header()->setResizeEnabled( resizeable, i );
    }
}

void Resource::loadItem( const QDomElement &e, QPixmap &pix, QString &txt, bool &hasPixmap )
{
    QDomElement n = e;
    hasPixmap = FALSE;
    while ( !n.isNull() ) {
	if ( n.tagName() == "property" ) {
	    QDomElement n2 = n.firstChild().toElement();
	    if ( n2.tagName() == "name" ) {
		QString attrib = n2.firstChild().toText().data();
		QVariant v = DomTool::elementToVariant( n2.nextSibling().toElement(), QVariant() );
		if ( attrib == "text" )
		    txt = v.toString();
		else if ( attrib == "pixmap" ) {
		    pix = loadPixmap( n2.nextSibling().toElement() );
		    hasPixmap = TRUE;
		}
	    }
	}
	n = n.nextSibling().toElement();
    }
}

void Resource::createItem( const QDomElement &e, QWidget *widget, QListViewItem *i )
{
    if ( !widget || !WidgetFactory::hasItems( WidgetDatabase::idFromClassName( WidgetFactory::classNameOf( widget ) ) ) )
	return;

    if ( widget->inherits( "QListBox" ) || widget->inherits( "QComboBox" ) ) {
	QDomElement n = e.firstChild().toElement();
	QPixmap pix;
	bool hasPixmap = FALSE;
	QString txt;
	loadItem( n, pix, txt, hasPixmap );
	QListBox *lb = 0;
	if ( widget->inherits( "QListBox" ) )
	    lb = (QListBox*)widget;
	else
	    lb = ( (QComboBox*)widget)->listBox();
	if ( hasPixmap ) {
	    new QListBoxPixmap( lb, pix, txt );
	} else {
	    new QListBoxText( lb, txt );
	}
    } else if ( widget->inherits( "QIconView" ) ) {
	QDomElement n = e.firstChild().toElement();
	QPixmap pix;
	bool hasPixmap = FALSE;
	QString txt;
	loadItem( n, pix, txt, hasPixmap );

	QIconView *iv = (QIconView*)widget;
	new QIconViewItem( iv, txt, pix );
    } else if ( widget->inherits( "QListView" ) ) {
	QDomElement n = e.firstChild().toElement();
	QPixmap pix;
	QValueList<QPixmap> pixmaps;
	QStringList textes;
	QListViewItem *item = 0;
	QListView *lv = (QListView*)widget;
	if ( i )
	    item = new QListViewItem( i, lastItem );
	else
	    item = new QListViewItem( lv, lastItem );
	while ( !n.isNull() ) {
	    if ( n.tagName() == "property" ) {
		QDomElement n2 = n.firstChild().toElement();
		if ( n2.tagName() == "name" ) {
		    QString attrib = n2.firstChild().toText().data();
		    QVariant v = DomTool::elementToVariant( n2.nextSibling().toElement(), QVariant() );
		    if ( attrib == "text" )
			textes << v.toString();
		    else if ( attrib == "pixmap" ) {
			QString s = v.toString();
			if ( s.isEmpty() ) {
			    pixmaps << QPixmap();
			} else {
			    pix = loadPixmap( n2.nextSibling().toElement() );
			    pixmaps << pix;
			}
		    }
		}
	    } else if ( n.tagName() == "item" ) {
		item->setOpen( TRUE );
		createItem( n, widget, item );
	    }
		
	    n = n.nextSibling().toElement();
	}
	
	for ( int i = 0; i < lv->columns(); ++i ) {
	    item->setText( i, textes[ i ] );
	    item->setPixmap( i, pixmaps[ i ] );
	}
	lastItem = item;
    }
}

QWidget *Resource::createSpacer( const QDomElement &e, QWidget *parent, QLayout *layout, Qt::Orientation o )
{
    Spacer *spacer = (Spacer*) WidgetFactory::create( WidgetDatabase::idFromClassName("Spacer"),
						      parent, "spacer", FALSE);
    spacer->setOrientation( o );
    QDomElement n = e.firstChild().toElement();
    int row = e.attribute( "row" ).toInt();
    int col = e.attribute( "column" ).toInt();
    int rowspan = e.attribute( "rowspan" ).toInt();
    int colspan = e.attribute( "colspan" ).toInt();
    if ( rowspan < 1 )
	rowspan = 1;
    if ( colspan < 1 )
	colspan = 1;
    spacer->setAutoResize( FALSE );
    while ( !n.isNull() ) {
	if ( n.tagName() == "property" ) {
	    QDomElement n2 = n.firstChild().toElement();
	    if ( n2.tagName() == "name" )
		setObjectProperty( spacer, n2.firstChild().toText().data(), n2.nextSibling().toElement() );
	}
	n = n.nextSibling().toElement();
    }
    spacer->setAutoResize( TRUE );

    if ( !previewMode ) {
	if ( formwindow )
	    formwindow->insertWidget( spacer, pasting );
	if ( layout ) {
	    if ( layout->inherits( "QBoxLayout" ) )
		( (QBoxLayout*)layout )->addWidget( spacer, 0, spacer->alignment() );
	    else
		( (QDesignerGridLayout*)layout )->addMultiCellWidget( spacer, row, row + rowspan - 1, col, col + colspan - 1,
								      spacer->alignment() );
	}
    } else {
	QSpacerItem *item = new QSpacerItem( spacer->width(), spacer->height(),
					     spacer->sizePolicy().horData(),
					     spacer->sizePolicy().verData() );
	if ( layout ) {
	    if ( layout->inherits( "QBoxLayout" ) )
		( (QBoxLayout*)layout )->addItem( item );
	    else
		( (QDesignerGridLayout*)layout )->addMultiCell( item, row, row + rowspan - 1, col, col + colspan - 1,
								spacer->alignment() );
	}
	delete spacer;
	return 0;
    }
    return spacer;
}

/*!
  Attention: this function has to be in sync with Uic::setObjectProperty(). If you change one, change both.
*/
void Resource::setObjectProperty( QObject* obj, const QString &prop, const QDomElement &e )
{
    const QMetaProperty *p = obj->metaObject()->property( prop, TRUE );

    if ( !obj->inherits( "QLayout" ) && !previewMode ) {// no layouts in metadatabase... (RS)
	if ( obj->inherits( "CustomWidget" ) ) {
	    MetaDataBase::CustomWidget *cw = ( (CustomWidget*)obj )->customWidget();
	    if ( cw && !cw->hasProperty( prop.latin1() ) && !p && prop != "toolTip" && prop != "whatsThis" )
		return;
	}
	MetaDataBase::setPropertyChanged( obj, prop, TRUE );
    }

    QVariant defVarient;
    if ( e.tagName() == "font" ) {
	QFont f( qApp->font() );
	if ( obj->isWidgetType() && ( (QWidget*)obj )->parentWidget() )
	    f = ( (QWidget*)obj )->parentWidget()->font();
	defVarient = QVariant( f );
    }

    QString comment;
    QVariant v( DomTool::elementToVariant( e, defVarient, comment ) );

    if ( !comment.isEmpty() ) {
	MetaDataBase::addEntry( obj );
	MetaDataBase::setPropertyComment( obj, prop, comment );
    }

    if ( e.tagName() == "pixmap" ) {
	QPixmap pix = loadPixmap( e );
	v = QVariant( pix );
    } else if ( e.tagName() == "iconset" ) {
	QPixmap pix;
	pix.convertFromImage( loadFromCollection( v.toString() ) );
	v = QVariant( QIconSet( pix ) );
    } else if ( e.tagName() == "image" ) {
	v = QVariant( loadFromCollection( v.toString() ) );
    }

    if ( !p ) {
	if ( !previewMode )
	    MetaDataBase::setFakeProperty( obj, prop, v );
	if ( obj->isWidgetType() ) {
	    if ( prop == "toolTip" ) {
		if ( previewMode && !v.toString().isEmpty() )
		    QToolTip::add( (QWidget*)obj, v.toString() );
	    } else if ( prop == "whatsThis" ) {
		if ( previewMode && !v.toString().isEmpty() )
		    QWhatsThis::add( (QWidget*)obj, v.toString() );
	    }
	    return;
	}
    }


    if ( e.tagName() == "palette" ) {
	QDomElement n = e.firstChild().toElement();
	QPalette p;
	while ( !n.isNull() ) {
	    QColorGroup cg;
	    if ( n.tagName() == "active" ) {
		cg = loadColorGroup( n );
		p.setActive( cg );
	    } else if ( n.tagName() == "inactive" ) {
		cg = loadColorGroup( n );
		p.setInactive( cg );
	    } else if ( n.tagName() == "disabled" ) {
		cg = loadColorGroup( n );
		p.setDisabled( cg );
	    }
	    n = n.nextSibling().toElement();
	}
	v = QPalette( p );
    } else if ( e.tagName() == "enum" && p && p->isEnumType() ) {
	QString key( v.toString() );
	v = QVariant( p->keyToValue( key ) );
    } else if ( e.tagName() == "set" && p && p->isSetType() ) {
	QString keys( v.toString() );
	QStringList lst = QStringList::split( '|', keys );
	QStrList l;
	for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it )
	    l.append( *it );
	v = QVariant( p->keysToValue( l ) );
    }	

    if ( !previewMode && prop == "caption" ) {
	QCString s1 = v.toCString();
	QString s2 = v.toString();
	if ( !s1.isEmpty() )
	    formwindow->setCaption( s1 );
	else if ( !s2.isEmpty() )
	    formwindow->setCaption( s2 );
    }
    if ( !previewMode && prop == "icon" ) {
	formwindow->setIcon( v.toPixmap() );
    }

    if ( prop == "geometry" ) {
	if ( previewMode && obj == toplevel ) {
	    toplevel->resize( v.toRect().size() );
	    return;
	} else if ( !previewMode && obj == formwindow->mainContainer() ) {
	    formwindow->resize( v.toRect().size() );
	    return;
	}
    }

    if ( obj->inherits( "QLayout" ) && !previewMode ) {
	if ( prop == "spacing" ) {
	    MetaDataBase::setSpacing( WidgetFactory::containerOfWidget( WidgetFactory::layoutParent( (QLayout*)obj ) ), v.toInt() );
	    return;
	}
	if ( prop == "margin" ) {
	    MetaDataBase::setMargin( WidgetFactory::containerOfWidget( WidgetFactory::layoutParent( (QLayout*)obj ) ), v.toInt() );
	    return;
	}
    }

    if ( prop == "name" ) {
	if ( pasting ) {
	    QString s = v.toString();
	    formwindow->unify( (QWidget*)obj, s, TRUE );
	    obj->setName( s );
	    return;
	} else if ( !previewMode && formwindow && obj == formwindow->mainContainer() ) {
	    formwindow->setName( v.toCString() );
	}
    }

    if ( prop == "sizePolicy" ) {
	QSizePolicy sp = v.toSizePolicy();
	sp.setHeightForWidth( ( (QWidget*)obj )->sizePolicy().hasHeightForWidth() );
    }

    obj->setProperty( prop, v );
}


QString Resource::saveInCollection( const QImage &img )
{
    QString imgName = "none";
    QValueList<Image>::Iterator it = images.begin();
    for ( ; it != images.end(); ++it ) {
	if ( img == ( *it ).img ) {
	    imgName = ( *it ).name;
	    break;
	}
    }

    if ( imgName == "none" ) {
	Image i;
	imgName = "image" + QString::number( images.count() );
	i.name = imgName;
	i.img = img;
	images.append( i );
    }
    return imgName;
}

void Resource::saveImageData( const QImage &img, QTextStream &ts, int indent )
{
    QByteArray ba;
    QBuffer buf( ba );
    buf.open( IO_WriteOnly );
    QImageIO iio( &buf, "XPM" );
    iio.setImage( img );
    iio.write();
    buf.close();
    ulong len = ba.size() * 2;
    QByteArray bazip( len );
    ::compress(  (uchar*) bazip.data(), &len, (uchar*) ba.data(), ba.size() );
    QString res;
    ts << makeIndent( indent ) << "<data format=\"XPM.GZ\" length=\"" << ba.size() << "\">";
    static const char hexchars[] = "0123456789abcdef";
    for ( int i = 0; i < (int)len; ++i ) {
	uchar s = (uchar) bazip[i];
	ts << hexchars[s >> 4];
	ts << hexchars[s & 0x0f];
    }
    ts << "</data>" << endl;
}

void Resource::saveImageCollection( QTextStream &ts, int indent )
{
    ts << makeIndent( indent ) << "<images>" << endl;
    indent++;

    QValueList<Image>::Iterator it = images.begin();
    for ( ; it != images.end(); ++it ) {
	ts << makeIndent( indent ) << "<image>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<name>" << ( *it ).name << "</name>" << endl;
	saveImageData( (*it).img, ts, indent );
	indent--;
	ts << makeIndent( indent ) << "</image>" << endl;
    }

    indent--;
    ts << makeIndent( indent ) << "</images>" << endl;
}

static QImage loadImageData( QDomElement &n2 )
{
    QImage img;
    QString data = n2.firstChild().toText().data();
    char *ba = new char[ data.length() / 2 ];
    for ( int i = 0; i < (int)data.length() / 2; ++i ) {
	char h = data[ 2 * i ].latin1();
	char l = data[ 2 * i  + 1 ].latin1();
	uchar r = 0;
	if ( h <= '9' )
	    r += h - '0';
	else
	    r += h - 'a' + 10;
	r = r << 4;
	if ( l <= '9' )
	    r += l - '0';
	else
	    r += l - 'a' + 10;
	ba[ i ] = r;
    }
    QString format = n2.attribute( "format", "PNG" );
    if ( format == "XPM.GZ" ) {
	ulong len = n2.attribute( "length" ).toULong();
	if ( len < data.length() * 5 )
	    len = data.length() * 5;
	QByteArray baunzip( len );
	::uncompress( (uchar*) baunzip.data(), &len, (uchar*) ba, data.length()/2 );
	img.loadFromData( (const uchar*)baunzip.data(), len, "XPM" );
    }  else {
	img.loadFromData( (const uchar*)ba, data.length() / 2, format );
    }
    delete [] ba;

    return img;
}

void Resource::loadImageCollection( const QDomElement &e )
{
    QDomElement n = e.firstChild().toElement();
    while ( !n.isNull() ) {
	if ( n.tagName() == "image" ) {
	    QDomElement n2 = n.firstChild().toElement();
	    Image img;
	    while ( !n2.isNull() ) {
		if ( n2.tagName() == "name" )
		    img.name = n2.firstChild().toText().data();
		else if ( n2.tagName() == "data" )
		    img.img = loadImageData( n2 );
		n2 = n2.nextSibling().toElement();
	    }
	    images.append( img );
	    n = n.nextSibling().toElement();
	}
    }
}

QImage Resource::loadFromCollection( const QString &name )
{
    QValueList<Image>::Iterator it = images.begin();
    for ( ; it != images.end(); ++it ) {
	if ( ( *it ).name == name )
	    return ( *it ).img;
    }
    return QImage();
}

void Resource::saveConnections( QTextStream &ts, int indent )
{
    ts << makeIndent( indent ) << "<connections>" << endl;
    indent++;
    QValueList<MetaDataBase::Connection> connections = MetaDataBase::connections( formwindow );
    QValueList<MetaDataBase::Connection>::Iterator it = connections.begin();
    for ( ; it != connections.end(); ++it ) {
	MetaDataBase::Connection conn = *it;
	if ( ( knownNames.findIndex( QString( conn.sender->name() ) ) == -1 && qstrcmp( conn.sender->name(), "this" ) != 0 ) ||
	     ( knownNames.findIndex( QString( conn.receiver->name() ) ) == -1 && qstrcmp( conn.receiver->name(), "this" ) != 0 ) )
	    continue;
	if ( formwindow->isMainContainer( (QWidget*)(*it).receiver ) && !MetaDataBase::hasSlot( formwindow, (*it).slot ) )
	    continue;

	if ( conn.sender->inherits( "CustomWidget" ) ) {
	    MetaDataBase::CustomWidget *cw = ( (CustomWidget*)conn.sender )->customWidget();
	    if ( cw && !cw->hasSignal( conn.signal ) )
		continue;
	}
	
	if ( conn.receiver->inherits( "CustomWidget" ) && !formwindow->isMainContainer( (QWidget*)conn.receiver ) ) {
	    MetaDataBase::CustomWidget *cw = ( (CustomWidget*)conn.receiver )->customWidget();
	    if ( cw && !cw->hasSlot( conn.slot ) )
		continue;
	}

	ts << makeIndent( indent ) << "<connection>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<sender>" << entitize( conn.sender->name() ) << "</sender>" << endl;
	ts << makeIndent( indent ) << "<signal>" << entitize( conn.signal ) << "</signal>" << endl;
	ts << makeIndent( indent ) << "<receiver>" << entitize( conn.receiver->name() ) << "</receiver>" << endl;
	ts << makeIndent( indent ) << "<slot>" << entitize( conn.slot ) << "</slot>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</connection>" << endl;
    }

    QValueList<MetaDataBase::Slot> slotList = MetaDataBase::slotList( formwindow );
    if ( !slotList.isEmpty() ) {
	QValueList<MetaDataBase::Slot>::Iterator it = slotList.begin();
	for ( ; it != slotList.end(); ++it ) {
	    MetaDataBase::Slot slot = *it;
	    ts << makeIndent( indent ) << "<slot access=\"" << slot.access << "\">" << entitize( slot.slot ) << "</slot>" << endl;
	}
    }

    indent--;
    ts << makeIndent( indent ) << "</connections>" << endl;
}

void Resource::loadConnections( const QDomElement &e )
{
    QDomElement n = e.firstChild().toElement();
    while ( !n.isNull() ) {
	if ( n.tagName() == "connection" ) {
	    QDomElement n2 = n.firstChild().toElement();
	    MetaDataBase::Connection conn;
	    while ( !n2.isNull() ) {
		if ( n2.tagName() == "sender" ) {
		    QString name = n2.firstChild().toText().data();
		    if ( name == "this" || qstrcmp( toplevel->name(), name ) == 0 ) {
			conn.sender = toplevel;
		    } else {
			if ( name == "this" )
			    name = toplevel->name();
			QObjectList *l = toplevel->queryList( 0, name, FALSE );
			if ( l ) {
			    if ( l->first() )
				conn.sender = l->first();
			    delete l;
			}
		    }
		} else if ( n2.tagName() == "signal" ) {
		    conn.signal = n2.firstChild().toText().data();
		} else if ( n2.tagName() == "receiver" ) {
		    QString name = n2.firstChild().toText().data();
		    if ( name == "this" || qstrcmp( toplevel->name(), name ) == 0 ) {
			conn.receiver = toplevel;
		    } else {
			QObjectList *l = toplevel->queryList( 0, name, FALSE );
			if ( l ) {
			    if ( l->first() )
				conn.receiver = l->first();
			    delete l;
			}
		    }
		} else if ( n2.tagName() == "slot" ) {
		    conn.slot = n2.firstChild().toText().data();
		}
		n2 = n2.nextSibling().toElement();
	    }
	    if ( formwindow ) {
		if ( conn.sender == formwindow )
		    conn.sender = formwindow->mainContainer();
		if ( conn.receiver == formwindow )
		    conn.receiver = formwindow->mainContainer();
	    }
	    MetaDataBase::addConnection( formwindow ? formwindow : toplevel, conn.sender, conn.signal, conn.receiver, conn.slot );
	} else if ( n.tagName() == "slot" ) {
	    MetaDataBase::Slot slot;
	    slot.access = n.attribute( "access", "public" );
	    slot.slot = n.firstChild().toText().data();
	    MetaDataBase::addSlot( formwindow ? formwindow : toplevel, slot.slot, slot.access );
	}
	n = n.nextSibling().toElement();
    }
}

void Resource::saveCustomWidgets( QTextStream &ts, int indent )
{
    ts << makeIndent( indent ) << "<customwidgets>" << endl;
    indent++;

    QList<MetaDataBase::CustomWidget> *lst = MetaDataBase::customWidgets();
    for ( MetaDataBase::CustomWidget *w = lst->first(); w; w = lst->next() ) {
	if ( usedCustomWidgets.findIndex( w->className ) == -1 )
	    continue;
	ts << makeIndent( indent ) << "<customwidget>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<class>" << w->className << "</class>" << endl;
	ts << makeIndent( indent ) << "<header location=\""
	   << ( w->includePolicy == MetaDataBase::CustomWidget::Local ? "local" : "global" )
	   << "\">" << w->includeFile << "</header>" << endl;
	ts << makeIndent( indent ) << "<sizehint>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<width>" << w->sizeHint.width() << "</width>" << endl;
	ts << makeIndent( indent ) << "<height>" << w->sizeHint.height() << "</height>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</sizehint>" << endl;
	ts << makeIndent( indent ) << "<container>" << (int)w->isContainer << "</container>" << endl;
	ts << makeIndent( indent ) << "<sizepolicy>" << endl;
	indent++;
	ts << makeIndent( indent ) << "<hordata>" << (int)w->sizePolicy.horData() << "</hordata>" << endl;
	ts << makeIndent( indent ) << "<verdata>" << (int)w->sizePolicy.verData() << "</verdata>" << endl;
	indent--;
	ts << makeIndent( indent ) << "</sizepolicy>" << endl;
	ts << makeIndent( indent ) << "<pixmap>" << saveInCollection( w->pixmap->convertToImage() ) << "</pixmap>" << endl;
	if ( !w->lstSignals.isEmpty() ) {
	    for ( QValueList<QCString>::Iterator it = w->lstSignals.begin(); it != w->lstSignals.end(); ++it )
		ts << makeIndent( indent ) << "<signal>" << entitize( *it ) << "</signal>" << endl;
	}
	if ( !w->lstSlots.isEmpty() ) {
	    for ( QValueList<MetaDataBase::Slot>::Iterator it = w->lstSlots.begin(); it != w->lstSlots.end(); ++it )
		ts << makeIndent( indent ) << "<slot access=\"" << (*it).access << "\">" << entitize( (*it).slot ) << "</slot>" << endl;
	}	
	if ( !w->lstProperties.isEmpty() ) {
	    for ( QValueList<MetaDataBase::Property>::Iterator it = w->lstProperties.begin(); it != w->lstProperties.end(); ++it )
		ts << makeIndent( indent ) << "<property type=\"" << (*it).type << "\">" << entitize( (*it).property ) << "</property>" << endl;
	}	
	indent--;
	ts << makeIndent( indent ) << "</customwidget>" << endl;
    }


    indent--;
    ts << makeIndent( indent ) << "</customwidgets>" << endl;
}

void Resource::loadCustomWidgets( const QDomElement &e, Resource *r )
{
    QDomElement n = e.firstChild().toElement();
    while ( !n.isNull() ) {
	if ( n.tagName() == "customwidget" ) {
	    QDomElement n2 = n.firstChild().toElement();
	    MetaDataBase::CustomWidget *w = new MetaDataBase::CustomWidget;
	    while ( !n2.isNull() ) {
		if ( n2.tagName() == "class" ) {
		    w->className = n2.firstChild().toText().data();
		} else if ( n2.tagName() == "header" ) {
		    w->includeFile = n2.firstChild().toText().data();
		    QString s = n2.attribute( "location" );
		    if ( s != "local" )
			w->includePolicy = MetaDataBase::CustomWidget::Global;
		    else
			w->includePolicy = MetaDataBase::CustomWidget::Local;
		} else if ( n2.tagName() == "sizehint" ) {
		    QDomElement n3 = n2.firstChild().toElement();
		    while ( !n3.isNull() ) {
			if ( n3.tagName() == "width" )
			    w->sizeHint.setWidth( n3.firstChild().toText().data().toInt() );
			else if ( n3.tagName() == "height" )
			    w->sizeHint.setHeight( n3.firstChild().toText().data().toInt() );
			n3 = n3.nextSibling().toElement();
		    }
		} else if ( n2.tagName() == "sizepolicy" ) {
		    QDomElement n3 = n2.firstChild().toElement();
		    while ( !n3.isNull() ) {
			if ( n3.tagName() == "hordata" )
			    w->sizePolicy.setHorData( (QSizePolicy::SizeType)n3.firstChild().toText().data().toInt() );
			else if ( n3.tagName() == "verdata" )
			    w->sizePolicy.setVerData( (QSizePolicy::SizeType)n3.firstChild().toText().data().toInt() );
			n3 = n3.nextSibling().toElement();
		    }
		} else if ( n2.tagName() == "pixmap" ) {
		    QPixmap pix;
		    if ( r ) {
			pix = r->loadPixmap( n2 );
		    } else {
			QDomElement n3 = n2.firstChild().toElement();
			QImage img;
			while ( !n3.isNull() ) {
			    if ( n3.tagName() == "data" ) {
				img = loadImageData( n3 );
			    }
			    n3 = n3.nextSibling().toElement();
			}
			pix.convertFromImage( img );
		    }
		    w->pixmap = new QPixmap( pix );
		} else if ( n2.tagName() == "signal" ) {
		    w->lstSignals.append( n2.firstChild().toText().data().latin1() );
		} else if ( n2.tagName() == "container" ) {
		    w->isContainer = (bool)n2.firstChild().toText().data().toInt();
		} else if ( n2.tagName() == "slot" ) {
		    MetaDataBase::Slot slot;
		    slot.slot = n2.firstChild().toText().data().latin1();
		    slot.access = n2.attribute( "access" );
		    w->lstSlots.append( slot );
		} else if ( n2.tagName() == "property" ) {
		    MetaDataBase::Property property;
		    property.property = n2.firstChild().toText().data().latin1();
		    property.type = n2.attribute( "type" );
		    w->lstProperties.append( property );
		}
		n2 = n2.nextSibling().toElement();
	    }
	    MetaDataBase::addCustomWidget( w );
	}
	n = n.nextSibling().toElement();
    }
}

void Resource::saveTabOrder( QTextStream &ts, int indent )
{
    QWidgetList l = MetaDataBase::tabOrder( toplevel );
    if ( l.isEmpty() )
	return;

    ts << makeIndent( indent ) << "<tabstops>" << endl;
    indent++;

    for ( QWidget *w = l.first(); w; w = l.next() ) {
	if ( w->testWState( Qt::WState_ForceHide ) || knownNames.findIndex( w->name() ) == -1 )
	    continue;
	ts << makeIndent( indent ) << "<tabstop>" << w->name() << "</tabstop>" << endl;
    }

    indent--;
    ts << makeIndent( indent ) << "</tabstops>" << endl;
}

void Resource::loadTabOrder( const QDomElement &e )
{
    QWidget *last = 0;
    QDomElement n = e.firstChild().toElement();
    QWidgetList widgets;
    while ( !n.isNull() ) {
	if ( n.tagName() == "tabstop" ) {
	    QString name = n.firstChild().toText().data();
	    QObjectList *l = toplevel->queryList( 0, name, FALSE );
	    if ( l ) {
		if ( l->first() ) {
		    QWidget *w = (QWidget*)l->first();
		    widgets.append( w );
		    if ( last )
			toplevel->setTabOrder( last, w );
		    last = w;
		}
		delete l;
	    }
	}
	n = n.nextSibling().toElement();
    }

    if ( !widgets.isEmpty() )
	MetaDataBase::setTabOrder( toplevel, widgets );
}

void Resource::saveMetaInfo( QTextStream &ts, int indent )
{
    MetaDataBase::MetaInfo info = MetaDataBase::metaInfo( formwindow );
    QString cn;
    if ( info.classNameChanged && !info.className.isEmpty() )
	cn = info.className;
    else
	cn = formwindow->name();
    ts << makeIndent( indent ) << "<class>" << entitize( cn ) << "</class>" << endl;
    if ( !info.comment.isEmpty() )
	ts << makeIndent( indent ) << "<comment>" << entitize( info.comment ) << "</comment>" << endl;
    if ( !info.author.isEmpty() )
	ts << makeIndent( indent ) << "<author>" << entitize( info.author ) << "</author>" << endl;

    QValueList<MetaDataBase::Include> includes = MetaDataBase::includes( formwindow );
    for ( QValueList<MetaDataBase::Include>::Iterator it = includes.begin(); it != includes.end(); ++it )
	ts << makeIndent( indent ) << "<include location=\"" << (*it).location << "\">" << (*it).header << "</include>" << endl;
    QStringList forwards = MetaDataBase::forwards( formwindow );
    for ( QStringList::Iterator it2 = forwards.begin(); it2 != forwards.end(); ++it2 )
	ts << makeIndent( indent ) << "<forward>" << entitize( *it2 ) << "</forward>" << endl;
    if ( formwindow && !formwindow->savePixmapInline() )
	ts << makeIndent( indent ) << "<pixmapfunction>" << formwindow->pixmapLoaderFunction() << "</pixmapfunction>" << endl;
}


QColorGroup Resource::loadColorGroup( const QDomElement &e )
{
    QColorGroup cg;
    int r = -1;
    QDomElement n = e.firstChild().toElement();
    QColor col;
    while ( !n.isNull() ) {
	if ( n.tagName() == "color" ) {
	    r++;
	    cg.setColor( (QColorGroup::ColorRole)r, (col = DomTool::readColor( n ) ) );
	} else if ( n.tagName() == "pixmap" ) {
	    QImage img = loadFromCollection( n.firstChild().toText().data() );
	    QPixmap pix = loadPixmap( n );
	    cg.setBrush( (QColorGroup::ColorRole)r, QBrush( col, pix ) );
	}
	n = n.nextSibling().toElement();
    }
    return cg;
}
