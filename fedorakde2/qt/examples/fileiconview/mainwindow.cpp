/****************************************************************************
** $Id: qt/examples/fileiconview/mainwindow.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "mainwindow.h"
#include "qfileiconview.h"
#include "../dirview/dirview.h"

#include <qsplitter.h>
#include <qprogressbar.h>
#include <qlabel.h>
#include <qstatusbar.h>
#include <qtoolbar.h>
#include <qcombobox.h>
#include <qpixmap.h>
#include <qtoolbutton.h>
#include <qdir.h>
#include <qfileinfo.h>

static const char* cdtoparent_xpm[]={
    "15 13 3 1",
    ". c None",
    "* c #000000",
    "a c #ffff99",
    "..*****........",
    ".*aaaaa*.......",
    "***************",
    "*aaaaaaaaaaaaa*",
    "*aaaa*aaaaaaaa*",
    "*aaa***aaaaaaa*",
    "*aa*****aaaaaa*",
    "*aaaa*aaaaaaaa*",
    "*aaaa*aaaaaaaa*",
    "*aaaa******aaa*",
    "*aaaaaaaaaaaaa*",
    "*aaaaaaaaaaaaa*",
    "***************"};

static const char* newfolder_xpm[] = {
    "15 14 4 1",
    " 	c None",
    ".	c #000000",
    "+	c #FFFF00",
    "@	c #FFFFFF",
    "          .    ",
    "               ",
    "          .    ",
    "       .     . ",
    "  ....  . . .  ",
    " .+@+@.  . .   ",
    "..........  . .",
    ".@+@+@+@+@..   ",
    ".+@+@+@+@+. .  ",
    ".@+@+@+@+@.  . ",
    ".+@+@+@+@+.    ",
    ".@+@+@+@+@.    ",
    ".+@+@+@+@+.    ",
    "...........    "};

FileMainWindow::FileMainWindow()
    : QMainWindow()
{
    setup();
}

void FileMainWindow::show()
{
    QMainWindow::show();
}

void FileMainWindow::setup()
{
    QSplitter *splitter = new QSplitter( this );

    dirlist = new DirectoryView( splitter, "dirlist", TRUE );
    dirlist->addColumn( "Name" );
    dirlist->addColumn( "Type" );
    Directory *root = new Directory( dirlist, "/" );
    root->setOpen( TRUE );
    splitter->setResizeMode( dirlist, QSplitter::KeepSize );

    fileview = new QtFileIconView( "/", splitter );
    fileview->setSelectionMode( QIconView::Extended );

    setCentralWidget( splitter );

    QToolBar *toolbar = new QToolBar( this, "toolbar" );
    setRightJustification( TRUE );

    (void)new QLabel( tr( " Path: " ), toolbar );

    pathCombo = new QComboBox( TRUE, toolbar );
    pathCombo->setAutoCompletion( TRUE );
    toolbar->setStretchableWidget( pathCombo );
    connect( pathCombo, SIGNAL( activated( const QString & ) ),
	     this, SLOT ( changePath( const QString & ) ) );

    toolbar->addSeparator();

    QPixmap pix;

    pix = QPixmap( cdtoparent_xpm );
    upButton = new QToolButton( pix, "One directory up", QString::null,
				this, SLOT( cdUp() ), toolbar, "cd up" );

    pix = QPixmap( newfolder_xpm );
    mkdirButton = new QToolButton( pix, "New Folder", QString::null,
				   this, SLOT( newFolder() ), toolbar, "new folder" );

    connect( dirlist, SIGNAL( folderSelected( const QString & ) ),
	     fileview, SLOT ( setDirectory( const QString & ) ) );
    connect( fileview, SIGNAL( directoryChanged( const QString & ) ),
	     this, SLOT( directoryChanged( const QString & ) ) );
    connect( fileview, SIGNAL( startReadDir( int ) ),
	     this, SLOT( slotStartReadDir( int ) ) );
    connect( fileview, SIGNAL( readNextDir() ),
	     this, SLOT( slotReadNextDir() ) );
    connect( fileview, SIGNAL( readDirDone() ),
	     this, SLOT( slotReadDirDone() ) );

    setDockEnabled( Left, FALSE );
    setDockEnabled( Right, FALSE );

    label = new QLabel( statusBar() );
    statusBar()->addWidget( label, 2, TRUE );
    progress = new QProgressBar( statusBar() );
    statusBar()->addWidget( progress, 1, TRUE );

    connect( fileview, SIGNAL( enableUp() ),
	     this, SLOT( enableUp() ) );
    connect( fileview, SIGNAL( disableUp() ),
	     this, SLOT( disableUp() ) );
    connect( fileview, SIGNAL( enableMkdir() ),
	     this, SLOT( enableMkdir() ) );
    connect( fileview, SIGNAL( disableMkdir() ),
	     this, SLOT( disableMkdir() ) );
}

void FileMainWindow::setPathCombo()
{
    QString dir = caption();
    int i = 0;
    bool found = FALSE;
    for ( i = 0; i < pathCombo->count(); ++i ) {
	if ( pathCombo->text( i ) == dir) {
	    found = TRUE;
	    break;
	}
    }

    if ( found )
	pathCombo->setCurrentItem( i );
    else {
	pathCombo->insertItem( dir );
	pathCombo->setCurrentItem( pathCombo->count() - 1 );
    }

}

void FileMainWindow::directoryChanged( const QString &dir )
{
    setCaption( dir );
    setPathCombo();
}

void FileMainWindow::slotStartReadDir( int dirs )
{
    label->setText( tr( " Reading Directory..." ) );
    progress->reset();
    progress->setTotalSteps( dirs );
}

void FileMainWindow::slotReadNextDir()
{
    int p = progress->progress();
    progress->setProgress( ++p );
}

void FileMainWindow::slotReadDirDone()
{
    label->setText( tr( " Reading Directory Done." ) );
    progress->setProgress( progress->totalSteps() );
}

void FileMainWindow::cdUp()
{
    QDir dir = fileview->currentDir();
    dir.cd( ".." );
    fileview->setDirectory( dir );
}

void FileMainWindow::newFolder()
{
    fileview->newDirectory();
}

void FileMainWindow::changePath( const QString &path )
{
    if ( QFileInfo( path ).exists() )
	fileview->setDirectory( path );
    else
	setPathCombo();
}

void FileMainWindow::enableUp()
{
    upButton->setEnabled( TRUE );
}

void FileMainWindow::disableUp()
{
    upButton->setEnabled( FALSE );
}

void FileMainWindow::enableMkdir()
{
    mkdirButton->setEnabled( TRUE );
}

void FileMainWindow::disableMkdir()
{
    mkdirButton->setEnabled( FALSE );
}
