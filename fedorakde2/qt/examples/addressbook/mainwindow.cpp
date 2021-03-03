/****************************************************************************
** $Id: qt/examples/addressbook/mainwindow.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright ( C ) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "mainwindow.h"
#include "centralwidget.h"

#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qkeycode.h>
#include <qstatusbar.h>
#include <qapplication.h>
#include <qfiledialog.h>

ABMainWindow::ABMainWindow()
    : QMainWindow( 0, "example addressbook application" ),
      filename( QString::null )
{
    setupMenuBar();
    setupFileTools();
    setupStatusBar();
    setupCentralWidget();
}


ABMainWindow::~ABMainWindow()
{
}

void ABMainWindow::setupMenuBar()
{
    QPopupMenu *file = new QPopupMenu( this );
    menuBar()->insertItem( "&File", file );

    file->insertItem( "New", this, SLOT( fileNew() ), CTRL + Key_N );
    file->insertItem( QPixmap( "fileopen.xpm" ), "Open", this, SLOT( fileOpen() ), CTRL + Key_O );
    file->insertSeparator();
    file->insertItem( QPixmap( "filesave.xpm" ), "Save", this, SLOT( fileSave() ), CTRL + Key_S );
    file->insertItem( "Save As...", this, SLOT( fileSaveAs() ) );
    file->insertSeparator();
    file->insertItem( QPixmap( "fileprint.xpm" ), "Print...", this, SLOT( filePrint() ), CTRL + Key_P );
    file->insertSeparator();
    file->insertItem( "Close", this, SLOT( closeWindow() ), CTRL + Key_W );
    file->insertItem( "Quit", qApp, SLOT( quit() ), CTRL + Key_Q );
}

void ABMainWindow::setupFileTools()
{
    //fileTools = new QToolBar( this, "file operations" );
}

void ABMainWindow::setupStatusBar()
{
    //statusBar()->message( "Ready", 2000 );
}

void ABMainWindow::setupCentralWidget()
{
    view = new ABCentralWidget( this );
    setCentralWidget( view );
}

void ABMainWindow::closeWindow()
{
    close();
}

void ABMainWindow::fileNew()
{
}

void ABMainWindow::fileOpen()
{
    QString fn = QFileDialog::getOpenFileName( QString::null, QString::null, this );
    if ( !fn.isEmpty() ) {
        filename = fn;
        view->load( filename );
    }
}

void ABMainWindow::fileSave()
{
    if ( filename.isEmpty() ) {
        fileSaveAs();
        return;
    }

    view->save( filename );
}

void ABMainWindow::fileSaveAs()
{
    QString fn = QFileDialog::getSaveFileName( QString::null, QString::null, this );
    if ( !fn.isEmpty() ) {
        filename = fn;
        fileSave();
    }
}

void ABMainWindow::filePrint()
{
}
