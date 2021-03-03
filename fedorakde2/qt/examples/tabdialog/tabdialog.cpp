/****************************************************************************
** $Id: qt/examples/tabdialog/tabdialog.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "tabdialog.h"

#include <qvbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qdatetime.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qlistbox.h>
#include <qapplication.h>

TabDialog::TabDialog( QWidget *parent, const char *name, const QString &_filename )
    : QTabDialog( parent, name ), filename( _filename ), fileinfo( filename )
{
    setupTab1();
    setupTab2();
    setupTab3();

    connect( this, SIGNAL( applyButtonPressed() ), qApp, SLOT( quit() ) );
}

void TabDialog::setupTab1()
{
    QVBox *tab1 = new QVBox( this );
    tab1->setMargin( 5 );

    (void)new QLabel( "Filename:", tab1 );
    QLineEdit *fname = new QLineEdit( filename, tab1 );
    fname->setFocus();

    (void)new QLabel( "Path:", tab1 );
    QLabel *path = new QLabel( fileinfo.dirPath( TRUE ), tab1 );
    path->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    (void)new QLabel( "Size:", tab1 );
    QLabel *size = new QLabel( QString( "%1 KB" ).arg( fileinfo.size() ), tab1 );
    size->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    (void)new QLabel( "Last Read:", tab1 );
    QLabel *lread = new QLabel( fileinfo.lastRead().toString(), tab1 );
    lread->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    (void)new QLabel( "Last Modified:", tab1 );
    QLabel *lmodif = new QLabel( fileinfo.lastModified().toString(), tab1 );
    lmodif->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    addTab( tab1, "General" );
}

void TabDialog::setupTab2()
{
    QVBox *tab2 = new QVBox( this );
    tab2->setMargin( 5 );

    QButtonGroup *bg = new QButtonGroup( 1, QGroupBox::Horizontal, "Permissions", tab2 );

    QCheckBox *readable = new QCheckBox( "Readable", bg );
    if ( fileinfo.isReadable() )
        readable->setChecked( TRUE );

    QCheckBox *writable = new QCheckBox( "Writeable", bg );
    if ( fileinfo.isWritable() )
        writable->setChecked( TRUE );

    QCheckBox *executable = new QCheckBox( "Executable", bg );
    if ( fileinfo.isExecutable() )
        executable->setChecked( TRUE );

    QButtonGroup *bg2 = new QButtonGroup( 2, QGroupBox::Horizontal, "Owner", tab2 );

    (void)new QLabel( "Owner", bg2 );
    QLabel *owner = new QLabel( fileinfo.owner(), bg2 );
    owner->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    (void)new QLabel( "Group", bg2 );
    QLabel *group = new QLabel( fileinfo.group(), bg2 );
    group->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    addTab( tab2, "Permissions" );
}

void TabDialog::setupTab3()
{
    QVBox *tab3 = new QVBox( this );
    tab3->setMargin( 5 );
    tab3->setSpacing( 5 );
    
    (void)new QLabel( QString( "Open %1 with:" ).arg( filename ), tab3 );

    QListBox *prgs = new QListBox( tab3 );
    for ( unsigned int i = 0; i < 30; i++ ) {
        QString prg = QString( "Application %1" ).arg( i );
        prgs->insertItem( prg );
    }
    prgs->setCurrentItem( 3 );

    (void)new QCheckBox( QString( "Open files with the extension '%1' always with this application" ).arg( fileinfo.extension() ), tab3 );

    addTab( tab3, "Applications" );
}
