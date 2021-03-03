/****************************************************************************
** $Id: qt/examples/themes/themes.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "themes.h"
#include "wood.h"
#include "metal.h"

#include "../buttongroups/buttongroups.h"
#include "../lineedits/lineedits.h"
#include "../listboxcombo/listboxcombo.h"
#include "../checklists/checklists.h"
#include "../progressbar/progressbar.h"
#include "../rangecontrols/rangecontrols.h"
#include "../richtext/richtext.h"

#include <qtabwidget.h>
#include <qapplication.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qfont.h>

#include <qwindowsstyle.h>
#include <qplatinumstyle.h>
#include <qmotifstyle.h>
#include <qmotifplusstyle.h>
#include <qcdestyle.h>

Themes::Themes( QWidget *parent, const char *name, WFlags f )
    : QMainWindow( parent, name, f )
{
    appFont = QApplication::font();
    tabwidget = new QTabWidget( this );

    tabwidget->addTab( new ButtonsGroups( tabwidget ), "Buttons/Groups" );
    QHBox *hbox = new QHBox( tabwidget );
    hbox->setMargin( 5 );
    (void)new LineEdits( hbox );
    (void)new ProgressBar( hbox );
    tabwidget->addTab( hbox, "Lineedits/Progressbar" );
    tabwidget->addTab( new ListBoxCombo( tabwidget ), "Listboxes/Comboboxes" );
    tabwidget->addTab( new CheckLists( tabwidget ), "Listviews" );
    tabwidget->addTab( new RangeControls( tabwidget ), "Rangecontrols" );
    tabwidget->addTab( new MyRichText( tabwidget ), "Fortune" );

    setCentralWidget( tabwidget );

    QPopupMenu *style = new QPopupMenu( this );
    style->setCheckable( TRUE );
    menuBar()->insertItem( "&Style" , style );

    sMetal = style->insertItem( "&Metal", this, SLOT( styleMetal() ) );
    sWood = style->insertItem( "&Norwegian Wood", this, SLOT( styleWood() ) );
    sPlatinum = style->insertItem( "&Platinum" , this ,SLOT( stylePlatinum() ) );
    sWindows = style->insertItem( "&Windows", this, SLOT( styleWindows() ) );
    sCDE = style->insertItem( "&CDE", this, SLOT( styleCDE() ) );
    sMotif = style->insertItem( "M&otif", this, SLOT( styleMotif() ) );
    sMotifPlus = style->insertItem( "Motif P&lus", this, SLOT( styleMotifPlus() ) );
    style->insertSeparator();
    style->insertItem("&Quit", qApp, SLOT( quit() ), CTRL | Key_Q );

    QPopupMenu * help = new QPopupMenu( this );
    menuBar()->insertSeparator();
    menuBar()->insertItem( "&Help", help );
    help->insertItem( "&About", this, SLOT(about()), Key_F1);
    help->insertItem( "About &Qt", this, SLOT(aboutQt()));

    qApp->setStyle( new NorwegianWoodStyle );
    menuBar()->setItemChecked( sWood, TRUE );
}

void Themes::styleWood()
{
    qApp->setStyle( new NorwegianWoodStyle );
    qApp->setFont( appFont, TRUE );
    selectStyleMenu( sWood );
}

void Themes::styleMetal()
{
    qApp->setStyle( new MetalStyle );
    qApp->setFont( appFont, TRUE );
    selectStyleMenu( sMetal );
}

void Themes::stylePlatinum()
{
    qApp->setStyle( new QPlatinumStyle );
    QPalette p( QColor( 239, 239, 239 ) );
    qApp->setPalette( p, TRUE );
    qApp->setFont( appFont, TRUE );
    selectStyleMenu( sPlatinum );
}

void Themes::styleWindows()
{
    qApp->setStyle( new QWindowsStyle );
    qApp->setFont( appFont, TRUE );
    selectStyleMenu( sWindows );
}

void Themes::styleCDE()
{
    qApp->setStyle( new QCDEStyle( TRUE ) );
    selectStyleMenu( sCDE );

    QPalette p( QColor( 75, 123, 130 ) );
    p.setColor( QPalette::Active, QColorGroup::Base, QColor( 55, 77, 78 ) );
    p.setColor( QPalette::Inactive, QColorGroup::Base, QColor( 55, 77, 78 ) );
    p.setColor( QPalette::Disabled, QColorGroup::Base, QColor( 55, 77, 78 ) );
    p.setColor( QPalette::Active, QColorGroup::Highlight, Qt::white );
    p.setColor( QPalette::Active, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
    p.setColor( QPalette::Inactive, QColorGroup::Highlight, Qt::white );
    p.setColor( QPalette::Inactive, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
    p.setColor( QPalette::Disabled, QColorGroup::Highlight, Qt::white );
    p.setColor( QPalette::Disabled, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
    p.setColor( QPalette::Active, QColorGroup::Foreground, Qt::white );
    p.setColor( QPalette::Active, QColorGroup::Text, Qt::white );
    p.setColor( QPalette::Active, QColorGroup::ButtonText, Qt::white );
    p.setColor( QPalette::Inactive, QColorGroup::Foreground, Qt::white );
    p.setColor( QPalette::Inactive, QColorGroup::Text, Qt::white );
    p.setColor( QPalette::Inactive, QColorGroup::ButtonText, Qt::white );
    p.setColor( QPalette::Disabled, QColorGroup::Foreground, Qt::lightGray );
    p.setColor( QPalette::Disabled, QColorGroup::Text, Qt::lightGray );
    p.setColor( QPalette::Disabled, QColorGroup::ButtonText, Qt::lightGray );
    qApp->setPalette( p, TRUE );
    qApp->setFont( QFont( "times", appFont.pointSize() ), TRUE );
}

void Themes::styleMotif()
{
    qApp->setStyle( new QMotifStyle );
    QPalette p( QColor( 192, 192, 192 ) );
    qApp->setPalette( p, TRUE );
    qApp->setFont( appFont, TRUE );
    selectStyleMenu( sMotif );
}

void Themes::styleMotifPlus()
{
    qApp->setStyle( new QMotifPlusStyle );
    QPalette p( QColor( 192, 192, 192 ) );
    qApp->setPalette( p, TRUE );
    qApp->setFont( appFont, TRUE );
    selectStyleMenu( sMotifPlus );
}


void Themes::about()
{
    QMessageBox::about( this, "Qt Themes Example",
			"<p>This example demonstrates the concept of "
			"<b>generalized GUI styles </b> first introduced "
			" with the 2.0 release of Qt.</p>" );
}


void Themes::aboutQt()
{
    QMessageBox::aboutQt( this, "Qt Themes Example" );
}


void Themes::selectStyleMenu( int s )
{
    menuBar()->setItemChecked( sWood, FALSE );
    menuBar()->setItemChecked( sMetal, FALSE );
    menuBar()->setItemChecked( sPlatinum, FALSE );
    menuBar()->setItemChecked( sWindows, FALSE );
    menuBar()->setItemChecked( sCDE, FALSE );
    menuBar()->setItemChecked( sMotif, FALSE );
    menuBar()->setItemChecked( sMotifPlus, FALSE );
    menuBar()->setItemChecked( s, TRUE );
}
