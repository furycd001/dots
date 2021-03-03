/*****************************************************************

Copyright (c) 1996-2000 the kicker authors. See file AUTHORS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include <qpixmap.h>
#include <dcopclient.h>
#include <kapp.h>

#include "client_mnu.h"
#include "client_mnu.moc"
#include "k_mnu.h"
#include "panelbutton.h"
#include <kdebug.h>

template class QList<KickerClientMenu>;

KickerClientMenu::KickerClientMenu( QWidget * parent, const char *name )
    : QPopupMenu( parent, name), DCOPObject( name )
{
}

KickerClientMenu::~KickerClientMenu()
{
}

void KickerClientMenu::clear()
{
    QPopupMenu::clear();
}
void KickerClientMenu::insertItem( QPixmap icon, QString text, int id )
{
    int globalid = QPopupMenu::insertItem( icon, text, this, SLOT( slotActivated(int) ) );
    setItemParameter( globalid, id );
}

void KickerClientMenu::insertItem( QString text, int id )
{
    int globalid = QPopupMenu::insertItem( text, this, SLOT( slotActivated(int) ) );
    setItemParameter( globalid, id );
}


QCString KickerClientMenu::insertMenu( QPixmap icon, QString text, int id )
{
    QString subname("%1-submenu%2");
    QCString subid = subname.arg(objId()).arg(id).local8Bit();
    KickerClientMenu *sub = new KickerClientMenu(this, subid);
    int globalid = QPopupMenu::insertItem( icon, text, sub, id);
    setItemParameter( globalid, id );

    return subid;
}


void KickerClientMenu::connectDCOPSignal( QCString signal, QCString appId, QCString objId )
{
    // very primitive right now
    if ( signal == "activated(int)" ) {
	app = appId;
	obj = objId;
    } else {
	kdWarning() << "DCOP: no such signal " << className() << "::" << signal.data() << endl;
    }
}

bool KickerClientMenu::process(const QCString &fun, const QByteArray &data,
			       QCString &replyType, QByteArray &replyData)
{
    if ( fun == "clear()" ) {
	clear();
	replyType = "void";
	return TRUE;
    }
    else if ( fun == "insertItem(QPixmap,QString,int)" ) {
	QDataStream dataStream( data, IO_ReadOnly );
	QPixmap icon;
	QString text;
	int id;
	dataStream >> icon >> text >> id;
	insertItem( icon, text, id );
	replyType = "void";
	return TRUE;
    }
    else if ( fun == "insertMenu(QPixmap,QString,int)" ) {
	QDataStream dataStream( data, IO_ReadOnly );
	QPixmap icon;
	QString text;
	int id;
	dataStream >> icon >> text >> id;
	QCString ref = insertMenu( icon, text, id );
	replyType = "QCString";
	QDataStream replyStream( replyData, IO_WriteOnly );
	replyStream << ref;
	return TRUE;
    }
    else if ( fun == "insertItem(QString,int)" ) {
	QDataStream dataStream( data, IO_ReadOnly );
	QString text;
	int id;
	dataStream >> text >> id;
	insertItem( text, id );
	replyType = "void";
	return TRUE;
    }
    else if ( fun == "connectDCOPSignal(QCString,QCString,QCString)" ) {
	QDataStream dataStream( data, IO_ReadOnly );
	QCString signal, appId, objId;
	dataStream >> signal >> appId >> objId;
	connectDCOPSignal( signal, appId, objId );
	replyType = "void";
	return TRUE;
    }
    return FALSE;
}

void KickerClientMenu::slotActivated(int id)
{
    if ( !app.isEmpty()  ) {
	QByteArray data;
	QDataStream dataStream( data, IO_WriteOnly );
	dataStream << id;
	kapp->dcopClient()->send( app, obj, "activated(int)", data );
    }
}

///////////////////////////////////////////////////////////////////////////////


KickerMenuManager::KickerMenuManager( PanelKMenu* menu, QObject *parent, const char *name )
    : QObject( parent, name ), DCOPObject( name )
{
    panelmenu = menu;
    clientmenus.setAutoDelete( TRUE );
    kapp->dcopClient()->setNotifications(true);
    connect( kapp->dcopClient(), SIGNAL( applicationRemoved(const QCString&) ),
	     this, SLOT( applicationRemoved( const QCString& ) ) );
}

KickerMenuManager::~KickerMenuManager()
{
}

static int menucount = 0;
QCString KickerMenuManager::createMenu( QPixmap icon, QString text )
{
    menucount++;
    QCString name;
    name.sprintf("kickerclientmenu-%d", menucount );
    KickerClientMenu* p = new KickerClientMenu( 0, name );
    clientmenus.append(p);
    panelmenu->initialize();
    p->text = text;
    p->icon = icon;
    p->idInParentMenu = panelmenu->insertClientMenu( p );
    p->createdBy = kapp->dcopClient()->senderId();
    panelmenu->adjustSize();
    return name;
}

void KickerMenuManager::removeMenu( QCString menu )
{
    QListIterator<KickerClientMenu> it( clientmenus);
    KickerClientMenu* m;
    while ( ( m = it.current() ) ) {
	++it;
	if ( m->objId() == menu ) {
	    panelmenu->removeClientMenu( m->idInParentMenu );
	    clientmenus.removeRef( m );
	}
    }
    panelmenu->adjustSize();
}


void KickerMenuManager::applicationRemoved( const QCString& appRemoved)
{
    QListIterator<KickerClientMenu> it( clientmenus);
    KickerClientMenu* m;
    while ( ( m = it.current() ) ) {
	++it;
	if ( m->createdBy == appRemoved ) {
	    panelmenu->removeClientMenu( m->idInParentMenu );
	    clientmenus.removeRef( m );
	}
    }
    panelmenu->adjustSize();
}

bool KickerMenuManager::process(const QCString &fun, const QByteArray &data,
				QCString &replyType, QByteArray &replyData)
{
    if ( fun == "createMenu(QPixmap,QString)" ) {
	QDataStream dataStream( data, IO_ReadOnly );
	QPixmap icon;
	QString text;
	dataStream >> icon >> text;
	QDataStream reply( replyData, IO_WriteOnly );
	reply << createMenu( icon, text );
	replyType = "QCString";
	return TRUE;
    } else if ( fun == "removeMenu(QCString)" ) {
	QDataStream dataStream( data, IO_ReadOnly );
	QCString menu;
	dataStream >> menu;
	removeMenu( menu );
	replyType = "void";
	return TRUE;
    }
    else if( fun == "popupKMenu(int,int)" ){
        QDataStream dataStream( data, IO_ReadOnly );
        int x, y;
        dataStream >> x;
        dataStream >> y;
        emit popupKMenu(x, y);
        replyType = "void";
        return(true);
    }
    return FALSE;
}

