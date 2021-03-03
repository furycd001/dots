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

#include "metadatabase.h"
#include "widgetfactory.h"
#include "formwindow.h"

#include <qobject.h>
#include <qlayout.h>
#include <qptrdict.h>
#include <qobjectlist.h>
#include <qstrlist.h>
#include <qmetaobject.h>
#include <qwidgetlist.h>

class MetaDataBaseRecord
{
public:
    QObject *object;
    QStringList changedProperties;
    QMap<QString,QVariant> fakeProperties;
    QMap<QString, QString> propertyComments;
    int spacing, margin;
    QValueList<MetaDataBase::Connection> connections;
    QValueList<MetaDataBase::Slot> slotList;
    QValueList<MetaDataBase::Include> includes;
    QStringList forwards;
    QWidgetList tabOrder;
    MetaDataBase::MetaInfo metaInfo;
    QCursor cursor;
    QMap<int, QString> pixmapArguments;
};

static QPtrDict<MetaDataBaseRecord> *db = 0;
static QList<MetaDataBase::CustomWidget> *cWidgets = 0;
static bool doUpdate = TRUE;

/*!
  \class MetaDataBase metadatabase.h
  \brief Database which stores meta data of widgets

  The MetaDataBase stores meta information of widgets, which are not
  stored directly in widgets (properties). This is e.g. the
  information which properties have been modified.
*/

MetaDataBase::MetaDataBase()
{
}

void MetaDataBase::setupDataBase()
{
    if ( db && cWidgets )
	return;

    db = new QPtrDict<MetaDataBaseRecord>;
    db->setAutoDelete( TRUE );
    cWidgets = new QList<MetaDataBase::CustomWidget>;
    cWidgets->setAutoDelete( TRUE );
}

void MetaDataBase::addEntry( QObject *o )
{
    if ( !o )
	return;
    setupDataBase();
    if ( db->find( o ) )
	return;
    MetaDataBaseRecord *r = new MetaDataBaseRecord;
    r->object = o;
    r->spacing = r->margin = -1;
    db->insert( (void*)o, r );

    WidgetFactory::initChangedProperties( o );
}

void MetaDataBase::removeEntry( QObject *o )
{
    setupDataBase();
    db->remove( o );
}

void MetaDataBase::setPropertyChanged( QObject *o, const QString &property, bool changed )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    if ( doUpdate &&
	 ( property == "hAlign" || property == "vAlign" || property == "wordwrap" ) ) {
	doUpdate = FALSE;
	setPropertyChanged( o, "alignment", changed );
	doUpdate = TRUE;
    }

    if ( doUpdate && property == "alignment" ) {
	doUpdate = FALSE;
	setPropertyChanged( o, "hAlign", changed );
	setPropertyChanged( o, "vAlign", changed );
	setPropertyChanged( o, "wordwrap", changed );
	doUpdate = TRUE;
    }

    if ( changed ) {
	if ( r->changedProperties.findIndex( property ) == -1 )
	    r->changedProperties.append( property );
    } else {
	if ( r->changedProperties.findIndex( property ) != -1 )
	    r->changedProperties.remove( property );
    }
}

bool MetaDataBase::isPropertyChanged( QObject *o, const QString &property )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return FALSE;
    }

    return r->changedProperties.findIndex( property ) != -1;
}

QStringList MetaDataBase::changedProperties( QObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return QStringList();
    }

    QStringList lst( r->changedProperties );
    return lst;
}

void MetaDataBase::setPropertyComment( QObject *o, const QString &property, const QString &comment )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->propertyComments.insert( property, comment );
}

QString MetaDataBase::propertyComment( QObject *o, const QString &property )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return QString::null;
    }

    return *r->propertyComments.find( property );
}

void MetaDataBase::setFakeProperty( QObject *o, const QString &property, const QVariant& value )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }
    r->fakeProperties[property] = value;
}

QVariant MetaDataBase::fakeProperty( QObject * o, const QString &property)
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return QVariant();
    }
    return r->fakeProperties[property];

}

QMap<QString,QVariant>* MetaDataBase::fakeProperties( QObject* o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return 0;
    }
    return &r->fakeProperties;
}

void MetaDataBase::setSpacing( QObject *o, int spacing )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r || !o->isWidgetType() ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->spacing = spacing;
    QLayout * layout = 0;
    WidgetFactory::layoutType( (QWidget*)o, layout );
    if ( layout )
	layout->setSpacing( spacing );
}

int MetaDataBase::spacing( QObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r || !o->isWidgetType() ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return -1;
    }

    return r->spacing;
}

void MetaDataBase::setMargin( QObject *o, int margin )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r || !o->isWidgetType() ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->margin = margin;
    QLayout * layout = 0;
    WidgetFactory::layoutType( (QWidget*)o, layout );
    if ( margin < 1 )
	margin = 1;
    if ( layout )
	layout->setMargin( margin );
}

int MetaDataBase::margin( QObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r || !o->isWidgetType() ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return -1;
    }

    return r->margin;
}

void MetaDataBase::addConnection( QObject *o, QObject *sender, const QCString &signal,
				  QObject *receiver, const QCString &slot )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }
    Connection conn;
    conn.sender = sender;
    conn.signal = signal;
    conn.receiver = receiver;
    conn.slot = slot;
    r->connections.append( conn );
}

void MetaDataBase::removeConnection( QObject *o, QObject *sender, const QCString &signal,
				     QObject *receiver, const QCString &slot )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    for ( QValueList<Connection>::Iterator it = r->connections.begin(); it != r->connections.end(); ++it ) {
	Connection conn = *it;
	if ( conn.sender == sender &&
	     conn.signal == signal &&
	     conn.receiver == receiver &&
	     conn.slot == slot ) {
	    r->connections.remove( it );
	    break;
	}
    }
}

QValueList<MetaDataBase::Connection> MetaDataBase::connections( QObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return QValueList<Connection>();
    }
    return r->connections;
}

QValueList<MetaDataBase::Connection> MetaDataBase::connections( QObject *o, QObject *sender,
								QObject *receiver )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return QValueList<Connection>();
    }
    QValueList<Connection>::Iterator it = r->connections.begin();
    QValueList<Connection> ret;
    QValueList<Connection>::Iterator conn;
    while ( ( conn = it ) != r->connections.end() ) {
	++it;
	if ( (*conn).sender == sender &&
	     (*conn).receiver == receiver )
	    ret << *conn;
    }

    return ret;
}

QValueList<MetaDataBase::Connection> MetaDataBase::connections( QObject *o, QObject *object )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return QValueList<Connection>();
    }
    QValueList<Connection>::Iterator it = r->connections.begin();
    QValueList<Connection> ret;
    QValueList<Connection>::Iterator conn;
    while ( ( conn = it ) != r->connections.end() ) {
	++it;
	if ( (*conn).sender == object ||
	     (*conn).receiver == object )
	    ret << *conn;
    }
    return ret;
}

void MetaDataBase::doConnections( QObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    QObject *sender = 0, *receiver = 0;
    QObjectList *l = 0;
    QValueList<Connection>::Iterator it = r->connections.begin();
    for ( ; it != r->connections.end(); ++it ) {
	Connection conn = *it;
	if ( qstrcmp( conn.sender->name(), o->name() ) == 0 ) {
	    sender = o;
	} else {
	    l = o->queryList( 0, conn.sender->name(), FALSE );
	    if ( !l || !l->first() ) {
		delete l;
		continue;
	    }
	    sender = l->first();
	    delete l;
	}
	if ( qstrcmp( conn.receiver->name(), o->name() ) == 0 ) {
	    receiver = o;
	} else {
	    l = o->queryList( 0, conn.receiver->name(), FALSE );
	    if ( !l || !l->first() ) {
		delete l;
		continue;
	    }
	    receiver = l->first();
	    delete l;
	}
	QString s = "2""%1";
	s = s.arg( conn.signal );
	QString s2 = "1""%1";
	s2 = s2.arg( conn.slot );

	QStrList signalList = sender->metaObject()->signalNames( TRUE );
	QStrList slotList = receiver->metaObject()->slotNames( TRUE );
	
	// avoid warnings
	if ( signalList.find( conn.signal ) == -1 ||
	     slotList.find( conn.slot ) == -1 )
	    continue;
	
	QObject::connect( sender, s, receiver, s2 );
    }
}

void MetaDataBase::addSlot( QObject *o, const QCString &slot, const QString &access )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    Slot s;
    s.slot = slot;
    s.access = access;
    r->slotList.append( s );
}

void MetaDataBase::removeSlot( QObject *o, const QCString &slot, const QString &access )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    for ( QValueList<Slot>::Iterator it = r->slotList.begin(); it != r->slotList.end(); ++it ) {
	Slot s = *it;
	if ( s.slot == slot &&
	     s.access == access ) {
	    r->slotList.remove( it );
	    break;
	}
    }
}

QValueList<MetaDataBase::Slot> MetaDataBase::slotList( QObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return QValueList<Slot>();
    }

    return r->slotList;
}

bool MetaDataBase::isSlotUsed( QObject *o, const QCString &slot )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return FALSE;
    }

    QValueList<Connection> conns = connections( o );
    for ( QValueList<Connection>::Iterator it = conns.begin(); it != conns.end(); ++it ) {
	if ( (*it).slot == slot )
	    return TRUE;
    }
    return FALSE;
}

bool MetaDataBase::hasSlot( QObject *o, const QCString &slot )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return FALSE;
    }

    QStrList slotList = o->metaObject()->slotNames( TRUE );
    if ( slotList.find( slot ) != -1 )
	return TRUE;

    if ( o->inherits( "FormWindow" ) ) {
	o = ( (FormWindow*)o )->mainContainer();
	slotList = o->metaObject()->slotNames( TRUE );
	if ( slotList.find( slot ) != -1 )
	    return TRUE;
    }

    if ( o->inherits( "CustomWidget" ) ) {
	MetaDataBase::CustomWidget *w = ( (::CustomWidget*)o )->customWidget();
	for ( QValueList<MetaDataBase::Slot>::Iterator it = w->lstSlots.begin(); it != w->lstSlots.end(); ++it ) {
	    QCString s = (*it).slot;
	    if ( !s.data() )
		continue;
	    if ( s == slot )
		return TRUE;
	}
    }

    for ( QValueList<Slot>::Iterator it = r->slotList.begin(); it != r->slotList.end(); ++it ) {
	Slot s = *it;
	if ( s.slot == slot )
	    return TRUE;
    }

    return FALSE;
}

bool MetaDataBase::addCustomWidget( CustomWidget *wid )
{
    setupDataBase();

    for ( CustomWidget *w = cWidgets->first(); w; w = cWidgets->next() ) {
	if ( *wid == *w ) {
	    for ( QValueList<QCString>::ConstIterator it = wid->lstSignals.begin(); it != wid->lstSignals.end(); ++it ) {
		if ( !w->hasSignal( *it ) )
		    w->lstSignals.append( *it );
	    }
	    for ( QValueList<MetaDataBase::Slot>::ConstIterator it2 = wid->lstSlots.begin(); it2 != wid->lstSlots.end(); ++it2 ) {
		if ( !w->hasSlot( (*it2).slot ) )
		    w->lstSlots.append( *it2 );
	    }
	    for ( QValueList<MetaDataBase::Property>::ConstIterator it3 = wid->lstProperties.begin(); it3 != wid->lstProperties.end(); ++it3 ) {
		if ( !w->hasProperty( (*it3).property ) )
		    w->lstProperties.append( *it3 );
	    }
	    delete wid;
	    return FALSE;
	}
    }


    WidgetDatabaseRecord *r = new WidgetDatabaseRecord;
    r->name = wid->className;
    r->group = WidgetDatabase::widgetGroup( "Custom" );
    r->toolTip = wid->className;
    r->icon = new QIconSet( *wid->pixmap, *wid->pixmap );
    r->isContainer = wid->isContainer;
    wid->id = WidgetDatabase::addCustomWidget( r );
    cWidgets->append( wid );
    return TRUE;
}

void MetaDataBase::removeCustomWidget( CustomWidget *w )
{
    cWidgets->removeRef( w );
}

QList<MetaDataBase::CustomWidget> *MetaDataBase::customWidgets()
{
    setupDataBase();
    return cWidgets;
}

MetaDataBase::CustomWidget *MetaDataBase::customWidget( int id )
{
    for ( CustomWidget *w = cWidgets->first(); w; w = cWidgets->next() ) {
	if ( id == w->id )
	    return w;
    }
    return 0;
}

bool MetaDataBase::isWidgetNameUsed( CustomWidget *wid )
{
    for ( CustomWidget *w = cWidgets->first(); w; w = cWidgets->next() ) {
	if ( w == wid )
	    continue;
	if ( wid->className == w->className )
	    return TRUE;
    }
    return FALSE;
}

bool MetaDataBase::hasCustomWidget( const QString &className )
{
    for ( CustomWidget *w = cWidgets->first(); w; w = cWidgets->next() ) {
	if ( w->className == className )
	    return TRUE;
    }
    return FALSE;
}

void MetaDataBase::setTabOrder( QWidget *w, const QWidgetList &order )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*) w );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  w, w->name(), w->className() );
	return;
    }

    r->tabOrder = order;
}

QWidgetList MetaDataBase::tabOrder( QWidget *w )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*) w );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  w, w->name(), w->className() );
	return QWidgetList();
    }

    return r->tabOrder;
}

void MetaDataBase::setIncludes( QObject *o, const QValueList<Include> &incs )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->includes = incs;
}

QValueList<MetaDataBase::Include> MetaDataBase::includes( QObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return QValueList<Include>();
    }

    return r->includes;
}

void MetaDataBase::setForwards( QObject *o, const QStringList &fwds )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->forwards = fwds;
}

QStringList MetaDataBase::forwards( QObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return QStringList();
    }

    return r->forwards;
}

void MetaDataBase::setMetaInfo( QObject *o, MetaInfo mi )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->metaInfo = mi;
}

MetaDataBase::MetaInfo MetaDataBase::metaInfo( QObject *o )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return MetaInfo();
    }

    return r->metaInfo;
}




MetaDataBase::CustomWidget::CustomWidget()
{	
    className = "MyCustomWidget";
    includeFile = "mywidget.h";
    includePolicy = Local;
    sizeHint = QSize( -1, -1 );
    pixmap = new QPixmap( PixmapChooser::loadPixmap( "customwidget.xpm" ) );
    id = -1;
    sizePolicy = QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
    isContainer = FALSE;
};

MetaDataBase::CustomWidget::CustomWidget( const CustomWidget &w )
{
    className = w.className;
    includeFile = w.includeFile;
    includePolicy = w.includePolicy;
    sizeHint = w.sizeHint;
    if ( w.pixmap )
	pixmap = new QPixmap( *w.pixmap );
    else
	pixmap = 0;
    id = w.id;
    isContainer = w.isContainer;
}

MetaDataBase::CustomWidget::~CustomWidget()
{
    delete pixmap;
}
	
void MetaDataBase::setCursor( QWidget *w, const QCursor &c )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)w );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  w, w->name(), w->className() );
	return;
    }

    r->cursor = c;
}

QCursor MetaDataBase::cursor( QWidget *w )
{
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)w );
    if ( !r ) {
	w->unsetCursor();
	return w->cursor();
    }

    return r->cursor;
}

bool MetaDataBase::CustomWidget::operator==( const CustomWidget &w ) const
{
    return className == w.className;
}
	
MetaDataBase::CustomWidget &MetaDataBase::CustomWidget::operator=( const CustomWidget &w )
{
    delete pixmap;
    className = w.className;
    includeFile = w.includeFile;
    includePolicy = w.includePolicy;
    sizeHint = w.sizeHint;
    if ( w.pixmap )
	pixmap = new QPixmap( *w.pixmap );
    else
	pixmap = 0;
    lstSignals = w.lstSignals;
    lstSlots = w.lstSlots;
    lstProperties = w.lstProperties;
    id = w.id;
    isContainer = w.isContainer;
    return *this;
}

bool MetaDataBase::CustomWidget::hasSignal( const QCString &signal ) const
{
    QStrList sigList = QWidget::staticMetaObject()->signalNames( TRUE );
    if ( sigList.find( signal ) != -1 )
	return TRUE;
    for ( QValueList<QCString>::ConstIterator it = lstSignals.begin(); it != lstSignals.end(); ++it ) {
	if ( *it == signal )
	    return TRUE;
    }
    return FALSE;
}

bool MetaDataBase::CustomWidget::hasSlot( const QCString &slot ) const
{
    QStrList slotList = QWidget::staticMetaObject()->slotNames( TRUE );
    if ( slotList.find( slot ) != -1 )
	return TRUE;

    for ( QValueList<MetaDataBase::Slot>::ConstIterator it = lstSlots.begin(); it != lstSlots.end(); ++it ) {
	if ( (*it).slot == slot )
	    return TRUE;
    }
    return FALSE;
}

bool MetaDataBase::CustomWidget::hasProperty( const QCString &prop ) const
{
    QStrList propList = QWidget::staticMetaObject()->propertyNames( TRUE );
    if ( propList.find( prop ) != -1 )
	return TRUE;

    for ( QValueList<MetaDataBase::Property>::ConstIterator it = lstProperties.begin(); it != lstProperties.end(); ++it ) {
	if ( (*it).property == prop )
	    return TRUE;
    }
    return FALSE;
}

void MetaDataBase::setPixmapArgument( QObject *o, int pixmap, const QString &arg )
{
    if ( !o )
	return;
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->pixmapArguments.remove( pixmap );
    r->pixmapArguments.insert( pixmap, arg );
}

QString MetaDataBase::pixmapArgument( QObject *o, int pixmap )
{
    if ( !o )
	return QString::null;
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return QString::null;
    }

    return *r->pixmapArguments.find( pixmap );
}

void MetaDataBase::clearPixmapArguments( QObject *o )
{
    if ( !o )
	return;
    setupDataBase();
    MetaDataBaseRecord *r = db->find( (void*)o );
    if ( !r ) {
	qWarning( "No entry for %p (%s, %s) found in MetaDataBase",
		  o, o->name(), o->className() );
	return;
    }

    r->pixmapArguments.clear();
}
