/****************************************************************************
** $Id: qt/examples/action/application.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "application.h"

#include <qimage.h>
#include <qpixmap.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qkeycode.h>
#include <qmultilineedit.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qprinter.h>
#include <qapplication.h>
#include <qaccel.h>
#include <qtextstream.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qwhatsthis.h>
#include <qaction.h>

#include "filesave.xpm"
#include "fileopen.xpm"
#include "fileprint.xpm"

const char * fileOpenText = "<img source=\"fileopen\"> "
"Click this button to open a <em>new file</em>. <br><br>"
"You can also select the <b>Open command</b> from the File menu.";
const char * fileSaveText = "Click this button to save the file you are "
"editing.  You will be prompted for a file name.\n\n"
"You can also select the Save command from the File menu.\n\n"
"Note that implementing this function is left as an exercise for the reader.";
const char * filePrintText = "Click this button to print the file you "
"are editing.\n\n"
"You can also select the Print command from the File menu.";

ApplicationWindow::ApplicationWindow()
    : QMainWindow( 0, "example application main window", WDestructiveClose )
{
    
    // create a printer
    
    printer = new QPrinter;

    
    // create user interface actions
    
    QAction *fileNewAction, *fileOpenAction, *fileSaveAction,
	* fileSaveAsAction, *filePrintAction, *fileCloseAction,
	*fileQuitAction;
    
    fileNewAction = new QAction( "New", "&New", CTRL+Key_N, this, "new" );
    connect( fileNewAction, SIGNAL( activated() ) , this, SLOT( newDoc() ) );
    
    fileOpenAction = new QAction( "Open File", QPixmap( fileopen ), "&Open", CTRL+Key_O, this, "open" );
    connect( fileOpenAction, SIGNAL( activated() ) , this, SLOT( load() ) );
    QMimeSourceFactory::defaultFactory()->setPixmap( "fileopen", QPixmap( fileopen ) );
    fileOpenAction->setWhatsThis( fileOpenText );
    
    fileSaveAction = new QAction( "Save File", QPixmap( filesave ), "&Save", CTRL+Key_S, this, "save" );
    connect( fileSaveAction, SIGNAL( activated() ) , this, SLOT( save() ) );
    fileSaveAction->setWhatsThis( fileSaveText );

    fileSaveAsAction = new QAction( "Save File As", "Save &as", 0,  this, "save as" );
    connect( fileSaveAsAction, SIGNAL( activated() ) , this, SLOT( saveAs() ) );
    fileSaveAsAction->setWhatsThis( fileSaveText );
    
    filePrintAction = new QAction( "Print File", QPixmap( fileprint ), "&Print", CTRL+Key_P, this, "print" );
    connect( filePrintAction, SIGNAL( activated() ) , this, SLOT( print() ) );
    filePrintAction->setWhatsThis( filePrintText );
    
    fileCloseAction = new QAction( "Close", "&Close", CTRL+Key_W, this, "close" );
    connect( fileCloseAction, SIGNAL( activated() ) , this, SLOT( close() ) );

    fileQuitAction = new QAction( "Quit", "&Quit", CTRL+Key_Q, this, "quit" );
    connect( fileQuitAction, SIGNAL( activated() ) , qApp, SLOT( closeAllWindows() ) );

    
    // populate a tool bar with some actions
    
    QToolBar* fileTools = new QToolBar( this, "file operations" );
    fileTools->setLabel( tr( "File Operations" ) );
    fileOpenAction->addTo( fileTools );
    fileSaveAction->addTo( fileTools );
    filePrintAction->addTo( fileTools );
    (void)QWhatsThis::whatsThisButton( fileTools );

    
    // popuplate a menu with all actions
    
    QPopupMenu * file = new QPopupMenu( this );
    menuBar()->insertItem( "&File", file );
    fileNewAction->addTo( file );
    fileOpenAction->addTo( file );
    fileSaveAction->addTo( file );
    fileSaveAsAction->addTo( file );
    file->insertSeparator();
    filePrintAction->addTo( file );
    file->insertSeparator();
    fileCloseAction->addTo( file );
    fileQuitAction->addTo( file );

    
    // add a help menu
    
    QPopupMenu * help = new QPopupMenu( this );
    menuBar()->insertSeparator();
    menuBar()->insertItem( "&Help", help );
    help->insertItem( "&About", this, SLOT(about()), Key_F1 );
    help->insertItem( "About &Qt", this, SLOT(aboutQt()) );
    help->insertSeparator();
    help->insertItem( "What's &This", this, SLOT(whatsThis()), SHIFT+Key_F1 );

    
    // create and define the central widget
    
    e = new QMultiLineEdit( this, "editor" );
    e->setFocus();
    setCentralWidget( e );
    statusBar()->message( "Ready", 2000 );
    resize( 450, 600 );
}


ApplicationWindow::~ApplicationWindow()
{
    delete printer;
}



void ApplicationWindow::newDoc()
{
    ApplicationWindow *ed = new ApplicationWindow;
    ed->show();
}

void ApplicationWindow::load()
{
    QString fn = QFileDialog::getOpenFileName( QString::null, QString::null,
					       this);
    if ( !fn.isEmpty() )
	load( fn );
    else
	statusBar()->message( "Loading aborted", 2000 );
}


void ApplicationWindow::load( const char *fileName )
{
    QFile f( fileName );
    if ( !f.open( IO_ReadOnly ) )
	return;

    filename = fileName; 
    
    e->setAutoUpdate( FALSE );
    e->clear();

    QTextStream t(&f);
    while ( !t.eof() ) {
	QString s = t.readLine();
	e->append( s );
    }
    f.close();

    e->setAutoUpdate( TRUE );
    e->repaint();
    e->setEdited( FALSE );
    setCaption( fileName );
    QString s;
    s.sprintf( "Loaded document %s", fileName );
    statusBar()->message( s, 2000 );
}


void ApplicationWindow::save()
{
    if ( filename.isEmpty() ) {
	saveAs();
	return;
    }

    QString text = e->text();
    QFile f( filename );
    if ( !f.open( IO_WriteOnly ) ) {
	statusBar()->message( QString("Could not write to %1").arg(filename),
			      2000 );
	return;
    }

    QTextStream t( &f );
    t << text;
    f.close();

    e->setEdited( FALSE );

    setCaption( filename );

    statusBar()->message( QString( "File %1 saved" ).arg( filename ), 2000 );
}


void ApplicationWindow::saveAs()
{
    QString fn = QFileDialog::getSaveFileName( QString::null, QString::null,
					       this );
    if ( !fn.isEmpty() ) {
	filename = fn;
	save();
    } else {
	statusBar()->message( "Saving aborted", 2000 );
    }
}


void ApplicationWindow::print()
{
    const int Margin = 10;
    int pageNo = 1;

    if ( printer->setup(this) ) {		// printer dialog
	statusBar()->message( "Printing..." );
	QPainter p;
	p.begin( printer );			// paint on printer
	p.setFont( e->font() );
	int yPos	= 0;			// y position for each line
	QFontMetrics fm = p.fontMetrics();
	QPaintDeviceMetrics metrics( printer ); // need width/height
						// of printer surface
	for( int i = 0 ; i < e->numLines() ; i++ ) {
	    if ( Margin + yPos > metrics.height() - Margin ) {
		QString msg( "Printing (page " );
		msg += QString::number( ++pageNo );
		msg += ")...";
		statusBar()->message( msg );
		printer->newPage();		// no more room on this page
		yPos = 0;			// back to top of page
	    }
	    p.drawText( Margin, Margin + yPos,
			metrics.width(), fm.lineSpacing(),
			ExpandTabs | DontClip,
			e->textLine( i ) );
	    yPos = yPos + fm.lineSpacing();
	}
	p.end();				// send job to printer
	statusBar()->message( "Printing completed", 2000 );
    } else {
	statusBar()->message( "Printing aborted", 2000 );
    }
}

void ApplicationWindow::closeEvent( QCloseEvent* ce )
{
    if ( !e->edited() ) {
	ce->accept();
	return;
    }

    switch( QMessageBox::information( this, "Qt Application Example",
				      "The document has been changed since "
				      "the last save.",
				      "Save Now", "Cancel", "Leave Anyway",
				      0, 1 ) ) {
    case 0:
	save();
	ce->accept();
	break;
    case 1:
    default: // just for sanity
	ce->ignore();
	break;
    case 2:
	ce->accept();
	break;
    }
}


void ApplicationWindow::about()
{
    QMessageBox::about( this, "Qt Application Example",
			"This example demonstrates simple use of "
			"QMainWindow,\nQMenuBar and QToolBar.");
}


void ApplicationWindow::aboutQt()
{
    QMessageBox::aboutQt( this, "Qt Application Example" );
}
