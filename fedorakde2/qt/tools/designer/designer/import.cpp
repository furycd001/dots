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

#include <qvariant.h>  // HP-UX compiler needs this here
#include "import.h"
#include "mainwindow.h"

#include <qmap.h>
#include <qprocess.h>
#include <qfileinfo.h>

static QMap<QString, QString> importMap;

QString Import::filters()
{
    QString f = tr( "Qt User-Interface Files (*.ui)" ) + ";;" +
		tr( "TMAKE Projectfile (*.pro)" ) + ";;";
    for ( QMap<QString, QString>::Iterator it = importMap.begin(); it != importMap.end(); ++it )
	f += it.key() + ";;";
    f += tr( "All Files (*)" );

    return f;
}
void Import::addImportEntry( const QString &wilrdcard, const QString &command )
{
    importMap.insert( wilrdcard, command );
}

int Import::numFilters()
{
    return importMap.count();
}

QString Import::wildcard( int i )
{
    int j = 0;
    for ( QMap<QString, QString>::Iterator it = importMap.begin(); it != importMap.end(); ++it ) {
	if ( i == j )
	    return it.key();
	++j;
    }
    return QString::null;
}

QString Import::command( int i )
{
    int j = 0;
    for ( QMap<QString, QString>::Iterator it = importMap.begin(); it != importMap.end(); ++it ) {
	if ( i == j )
	    return *it;
	++j;
    }
    return QString::null;
}




Import::Import( const QString &fn, MainWindow *mw )
    : filename( fn ), mainWindow( mw ), haveLoaded( FALSE )
{
    QString command;
    QString ext = QFileInfo( fn ).extension();
    for ( QMap<QString, QString>::Iterator it = importMap.begin(); it != importMap.end(); ++it ) {
	if ( it.key().find( ext ) != -1 )
	    command = *it;
    }

    if ( !command.isEmpty() ) {
	QStringList lst = QStringList::split( ' ', command );
	process = new QProcess( lst[ 0 ] );
	QStringList::Iterator it2 = lst.begin();
	++it2;
	for ( ; it2 != lst.end(); ++it2 ) {
	    if ( *it2 == "%f" )
		process->addArgument( fn );
	    else
		process->addArgument( *it2 );
	}

	connect( process, SIGNAL( dataStdout( const QString & ) ),
		 this, SLOT( gotData( const QString & ) ) );
	connect( process, SIGNAL( processExited() ),
		 this, SLOT( bye() ) );
	process->start();
    }
}


void Import::gotData( const QString &data )
{
    incomingData += data;
}

void Import::bye()
{
    if ( haveLoaded )
	return;

    haveLoaded = TRUE;
    QStringList lst = QStringList::split( '\n', incomingData );
    for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it )
	mainWindow->openFile( (*it).stripWhiteSpace(), FALSE );
    delete process;
    delete this;
}
