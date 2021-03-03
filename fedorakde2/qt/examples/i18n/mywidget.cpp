/****************************************************************************
** $Id: qt/examples/i18n/mywidget.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qaccel.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qstatusbar.h>
#include <qapplication.h>

#include "mywidget.h"

MyWidget::MyWidget( QWidget* parent, const char* name )
	: QMainWindow( parent, name )
{
    QVBox* central = new QVBox(this);
    central->setMargin( 5 ); 
    central->setSpacing( 5 ); 
    setCentralWidget(central);

    QPopupMenu* file = new QPopupMenu(this);
    file->insertItem( tr("E&xit"), qApp, SLOT(quit()),
            QAccel::stringToKey(tr("Ctrl+Q")) );
    menuBar()->insertItem( tr("&File"), file );

    setCaption( tr( "Internationalization Example" ) ); 

    QString l;
    statusBar()->message( tr("Language: English") );

    ( void )new QLabel( tr( "The Main Window" ), central ); 

    QButtonGroup* gbox = new QButtonGroup( 1, QGroupBox::Horizontal, 
				      tr( "View" ), central ); 
    (void)new QRadioButton( tr( "Perspective" ), gbox ); 
    (void)new QRadioButton( tr( "Isometric" ), gbox ); 
    (void)new QRadioButton( tr( "Oblique" ), gbox ); 

    initChoices(central); 
}

static const char* choices[] = {
    QT_TRANSLATE_NOOP( "MyWidget", "First" ), 
    QT_TRANSLATE_NOOP( "MyWidget", "Second" ), 
    QT_TRANSLATE_NOOP( "MyWidget", "Third" ), 
    0
}; 

void MyWidget::initChoices(QWidget* parent)
{
    QListBox* lb = new QListBox( parent ); 
    for ( int i = 0; choices[i]; i++ )
	lb->insertItem( tr( choices[i] ) ); 
}

void MyWidget::closeEvent(QCloseEvent* e)
{
    QWidget::closeEvent(e);
    emit closed();
}
