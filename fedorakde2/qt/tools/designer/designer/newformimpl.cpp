/**********************************************************************
** Copyright (C) 2000-2001 Trolltech AS.  All rights reserved.
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

#include "newformimpl.h"
#include "pixmapchooser.h"
#include "mainwindow.h"
#include "stdlib.h"

#include <qiconview.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qregexp.h>
#include <qpushbutton.h>

static QString templatePath( const QString &t )
{
    QString path = getenv( "QTDIR" );
    if ( !path.isNull() ) {
	path.append("/tools/designer/templates");
	if ( QFileInfo( path ).exists() )
	    return path;
    }
    return t;
}

NewForm::NewForm( QWidget *parent, const QString &tPath )
    : NewFormBase( parent, 0, TRUE ), templPath( tPath )
{
    connect( helpButton, SIGNAL( clicked() ), MainWindow::self, SLOT( showDialogHelp() ) );
    QIconViewItem *i = new QIconViewItem( templateView );
    i->setText( tr( "Dialog" ) );
    i->setPixmap( PixmapChooser::loadPixmap( "newform.xpm" ) );
    i->setDragEnabled( FALSE );
    i = new QIconViewItem( templateView );
    i->setText( tr( "Wizard" ) );
    i->setPixmap( PixmapChooser::loadPixmap( "newform.xpm" ) );
    i->setDragEnabled( FALSE );
    i = new QIconViewItem( templateView );
    i->setText( tr( "Widget" ) );
    i->setPixmap( PixmapChooser::loadPixmap( "newform.xpm" ) );
    i->setDragEnabled( FALSE );

    QString templateDir = templatePath( templPath );
    if ( !templateDir.isNull() ) {
	QDir dir( templateDir );
	const QFileInfoList *filist = dir.entryInfoList( QDir::DefaultFilter, QDir::DirsFirst | QDir::Name );
	if ( filist ) {
	    QFileInfoListIterator it( *filist );
	    QFileInfo *fi;
	    while ( ( fi = it.current() ) != 0 ) {
		++it;
		if ( !fi->isFile() )
		    continue;
		i = new QIconViewItem( templateView );
		i->setDragEnabled( FALSE );
		QString name = fi->baseName();
		name = name.replace( QRegExp( "_" ), " " );
		i->setText( name );
		i->setPixmap( PixmapChooser::loadPixmap( "newform.xpm" ) );
	    }
	}
    }

    templateView->setCurrentItem( templateView->firstItem() );
    templateView->viewport()->setFocus();
}

NewForm::Form NewForm::formType() const
{
    if ( templateView->currentItem()->text() == tr( "Dialog" ) )
	return Dialog;
    if ( templateView->currentItem()->text() == tr( "Wizard" ) )
	return Wizard;
    if ( templateView->currentItem()->text() == tr( "Widget" ) )
	return Widget;
    return Custom;
}

QString NewForm::templateFile() const
{
    QString path = templatePath( templPath );
    if ( !path.isNull() )
	path.append( "/" );
    path.append( templateView->currentItem()->text() );
    path.append( ".ui" );
    path = path.replace( QRegExp( " " ), "_" );
    return path;
}
