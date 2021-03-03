/****************************************************************************
** $Id: qt/examples/layout/layout.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qapplication.h>
#include <qlabel.h>
#include <qcolor.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qmenubar.h>
#include <qpopupmenu.h>

class ExampleWidget : public QWidget
{
public:
    ExampleWidget( QWidget *parent = 0, const char *name = 0 );
    ~ExampleWidget();
private:
};

ExampleWidget::ExampleWidget( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    // Make the top-level layout; a vertical box to contain all widgets
    // and sub-layouts.
    QBoxLayout *topLayout = new QVBoxLayout( this, 5 );

    // Create a menubar...
    QMenuBar *menubar = new QMenuBar( this );
    menubar->setSeparator( QMenuBar::InWindowsStyle );
    QPopupMenu* popup;
    popup = new QPopupMenu( this );
    popup->insertItem( "&Quit", qApp, SLOT(quit()) );
    menubar->insertItem( "&File", popup );

    // ...and tell the layout about it.
    topLayout->setMenuBar( menubar );

    // Make an hbox that will hold a row of buttons.
    QBoxLayout *buttons = new QHBoxLayout( topLayout);
    int i;
    for ( i = 1; i <= 4; i++ ) {
	QPushButton* but = new QPushButton( this );
	QString s;
	s.sprintf( "Button %d", i );
	but->setText( s );

	// Set horizontal stretch factor to 10 to let the buttons
	// stretch horizontally. The buttons will not stretch
	// vertically, since bigWidget below will take up vertical
	// stretch.
	buttons->addWidget( but, 10 );
	// (Actually, the result would have been the same with a
	// stretch factor of 0; if no items in a layout have non-zero
	// stretch, the space is divided equally between members.)
    }

    // Make another hbox that will hold a left-justified row of buttons.
    QBoxLayout *buttons2 = new QHBoxLayout( topLayout );

    QPushButton* but = new QPushButton( "Button five", this );
    buttons2->addWidget( but );

    but = new QPushButton( "Button 6", this );
    buttons2->addWidget( but );

    // Fill up the rest of the hbox with stretchable space, so that
    // the buttons get their minimum width and are pushed to the left.
    buttons2->addStretch( 10 );

    // Make  a big widget that will grab all space in the middle.
    QMultiLineEdit *bigWidget = new QMultiLineEdit( this );
    bigWidget->setText( "This widget will get all the remaining space" );
    bigWidget->setFrameStyle( QFrame::Panel | QFrame::Plain );

    // Set vertical stretch factor to 10 to let the bigWidget stretch
    // vertically. It will stretch horizontally because there are no
    // widgets beside it to take up horizontal stretch.
    //    topLayout->addWidget( bigWidget, 10 );
    topLayout->addWidget( bigWidget); //###

    // Make a grid that will hold a vertical table of QLabel/QLineEdit
    // pairs next to a large QMultiLineEdit.

    // Don't use hard-coded row/column numbers in QGridLayout, you'll
    // regret it when you have to change the layout.
    const int numRows = 3;
    const int labelCol = 0;
    const int linedCol = 1;
    const int multiCol = 2;

    // Let the grid-layout have a spacing of 10 pixels between
    // widgets, overriding the default from topLayout.
    QGridLayout *grid = new QGridLayout( topLayout, 0, 0, 10 );
    int row;

    for ( row = 0; row < numRows; row++ ) {
	QLineEdit *ed = new QLineEdit( this );
	// The line edit goes in the second column
	grid->addWidget( ed, row, linedCol );	

	// Make a label that is a buddy of the line edit
	QString s;
	s.sprintf( "Line &%d", row+1 );
	QLabel *label = new QLabel( ed, s, this );
	// The label goes in the first column.
	grid->addWidget( label, row, labelCol );
    }

    // The multiline edit will cover the entire vertical range of the
    // grid (rows 0 to numRows) and stay in column 2.

    QMultiLineEdit *med = new QMultiLineEdit( this );
    grid->addMultiCellWidget( med, 0, -1, multiCol, multiCol );

    // The labels will take the space they need. Let the remaining
    // horizontal space be shared so that the multiline edit gets
    // twice as much as the line edit.
        grid->setColStretch( linedCol, 10 );
        grid->setColStretch( multiCol, 20 );

    // Add a widget at the bottom.
    QLabel* sb = new QLabel( this );
    sb->setText("Let's pretend this is a status bar");
    sb->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    // This widget will use all horizontal space, and have a fixed height.

    // we should have made a subclass and implemented sizePolicy there...
    sb->setFixedHeight( sb->sizeHint().height() );

    sb->setAlignment( AlignVCenter | AlignLeft );
    topLayout->addWidget( sb );

    topLayout->activate();
}

ExampleWidget::~ExampleWidget()
{
    // All child widgets are deleted by Qt.
    // The top-level layout and all its sub-layouts are deleted by Qt.
}

int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    ExampleWidget f;
    a.setMainWidget(&f);
    f.setCaption("Qt Example - Caption");
    f.show();

    return a.exec();
}
