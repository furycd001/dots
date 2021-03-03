/****************************************************************************
** $Id: qt/examples/menu/menu.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "menu.h"
#include <qpopupmenu.h>
#include <qkeycode.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qpainter.h>

/* XPM */
static const char * p1_xpm[] = {
"16 16 3 1",
" 	c None",
".	c #000000000000",
"X	c #FFFFFFFF0000",
"                ",
"                ",
"         ....   ",
"        .XXXX.  ",
" .............. ",
" .XXXXXXXXXXXX. ",
" .XXXXXXXXXXXX. ",
" .XXXXXXXXXXXX. ",
" .XXXXXXXXXXXX. ",
" .XXXXXXXXXXXX. ",
" .XXXXXXXXXXXX. ",
" .XXXXXXXXXXXX. ",
" .XXXXXXXXXXXX. ",
" .XXXXXXXXXXXX. ",
" .............. ",
"                "};

/* XPM */
static const char * p2_xpm[] = {
"16 16 3 1",
" 	c None",
".	c #000000000000",
"X	c #FFFFFFFFFFFF",
"                ",
"   ......       ",
"   .XXX.X.      ",
"   .XXX.XX.     ",
"   .XXX.XXX.    ",
"   .XXX.....    ",
"   .XXXXXXX.    ",
"   .XXXXXXX.    ",
"   .XXXXXXX.    ",
"   .XXXXXXX.    ",
"   .XXXXXXX.    ",
"   .XXXXXXX.    ",
"   .XXXXXXX.    ",
"   .........    ",
"                ",
"                "};

/* XPM */
static const char * p3_xpm[] = {
"16 16 3 1",
" 	c None",
".	c #000000000000",
"X	c #FFFFFFFFFFFF",
"                ",
"                ",
"   .........    ",
"  ...........   ",
"  ........ ..   ",
"  ...........   ",
"  ...........   ",
"  ...........   ",
"  ...........   ",
"  ...XXXXX...   ",
"  ...XXXXX...   ",
"  ...XXXXX...   ",
"  ...XXXXX...   ",
"   .........    ",
"                ",
"                "};


/*
  Auxiliary class to provide fancy menu items with different
  fonts. Used for the "bold" and "underline" menu items in the options
  menu.
 */
class MyMenuItem : public QCustomMenuItem
{
public:
    MyMenuItem( const QString& s, const QFont& f )
	: string( s ), font( f ){};
    ~MyMenuItem(){}

    void paint( QPainter* p, const QColorGroup& /*cg*/, bool /*act*/, bool /*enabled*/, int x, int y, int w, int h )
    {
	p->setFont ( font );
	p->drawText( x, y, w, h, AlignLeft | AlignVCenter | ShowPrefix | DontClip, string );
    }

    QSize sizeHint()
    {
	return QFontMetrics( font ).size( AlignLeft | AlignVCenter | ShowPrefix | DontClip,  string );
    }
private:
    QString string;
    QFont font;
};


MenuExample::MenuExample( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    QPixmap p1( p1_xpm );
    QPixmap p2( p2_xpm );
    QPixmap p3( p3_xpm );

    QPopupMenu *print = new QPopupMenu( this );
    CHECK_PTR( print );
    print->insertTearOffHandle();
    print->insertItem( "&Print to printer", this, SLOT(printer()) );
    print->insertItem( "Print to &file", this, SLOT(file()) );
    print->insertItem( "Print to fa&x", this, SLOT(fax()) );
    print->insertSeparator();
    print->insertItem( "Printer &Setup", this, SLOT(printerSetup()) );

    QPopupMenu *file = new QPopupMenu( this );
    CHECK_PTR( file );
    file->insertItem( p1, "&Open",  this, SLOT(open()), CTRL+Key_O );
    file->insertItem( p2, "&New", this, SLOT(news()), CTRL+Key_N );
    file->insertItem( p3, "&Save", this, SLOT(save()), CTRL+Key_S );
    file->insertItem( "&Close", this, SLOT(closeDoc()), CTRL+Key_W );
    file->insertSeparator();
    file->insertItem( "&Print", print, CTRL+Key_P );
    file->insertSeparator();
    file->insertItem( "E&xit",  qApp, SLOT(quit()), CTRL+Key_Q );

    QPopupMenu *edit = new QPopupMenu( this );
    CHECK_PTR( edit );
    int undoID = edit->insertItem( "&Undo", this, SLOT(undo()) );
    int redoID = edit->insertItem( "&Redo", this, SLOT(redo()) );
    edit->setItemEnabled( undoID, FALSE );
    edit->setItemEnabled( redoID, FALSE );

    QPopupMenu* options = new QPopupMenu( this );
    CHECK_PTR( options );
    options->insertTearOffHandle();
    options->setCaption("Options");
    options->insertItem( "&Normal Font", this, SLOT(normal()) );
    options->insertSeparator();

    options->polish(); // adjust system settings 
    QFont f = options->font();
    f.setBold( TRUE );
    boldID = options->insertItem( new MyMenuItem( "&Bold", f ) );
    options->setAccel( CTRL+Key_B, boldID );
    options->connectItem( boldID, this, SLOT(bold()) );
    f = font();
    f.setUnderline( TRUE );
    underlineID = options->insertItem( new MyMenuItem( "&Underline", f ) );
    options->setAccel( CTRL+Key_U, underlineID );
    options->connectItem( underlineID, this, SLOT(underline()) );

    isBold = FALSE;
    isUnderline = FALSE;
    options->setCheckable( TRUE );


    QPopupMenu *help = new QPopupMenu( this );
    CHECK_PTR( help );
    help->insertItem( "&About", this, SLOT(about()), CTRL+Key_H );
    help->insertItem( "About &Qt", this, SLOT(aboutQt()) );

    menu = new QMenuBar( this );
    CHECK_PTR( menu );
    menu->insertItem( "&File", file );
    menu->insertItem( "&Edit", edit );
    menu->insertItem( "&Options", options );
    menu->insertSeparator();
    menu->insertItem( "&Help", help );
    menu->setSeparator( QMenuBar::InWindowsStyle );

    label = new QLabel( this );
    CHECK_PTR( label );
    label->setGeometry( 20, rect().center().y()-20, width()-40, 40 );
    label->setFrameStyle( QFrame::Box | QFrame::Raised );
    label->setLineWidth( 1 );
    label->setAlignment( AlignCenter );

    connect( this,  SIGNAL(explain(const QString&)),
	     label, SLOT(setText(const QString&)) );

    setMinimumSize( 100, 80 );
}


void MenuExample::open()
{
    emit explain( "File/Open selected" );
}


void MenuExample::news()
{
    emit explain( "File/New selected" );
}

void MenuExample::save()
{
    emit explain( "File/Save selected" );
}


void MenuExample::closeDoc()
{
    emit explain( "File/Close selected" );
}


void MenuExample::undo()
{
    emit explain( "Edit/Undo selected" );
}


void MenuExample::redo()
{
    emit explain( "Edit/Redo selected" );
}


void MenuExample::normal()
{
    isBold = FALSE;
    isUnderline = FALSE;
    menu->setItemChecked( boldID, isBold );
    menu->setItemChecked( underlineID, isUnderline );
    emit explain( "Options/Normal selected" );
}


void MenuExample::bold()
{
    isBold = !isBold;
    menu->setItemChecked( boldID, isBold );
    emit explain( "Options/Bold selected" );
}


void MenuExample::underline()
{
    isUnderline = !isUnderline;
    menu->setItemChecked( underlineID, isUnderline );
    emit explain( "Options/Underline selected" );
}


void MenuExample::about()
{
    QMessageBox::about( this, "Qt Menu Example",
			"This example demonstrates simple use of Qt menus.\n"
			"You can cut and paste lines from it to your own\n"
			"programs." );
}


void MenuExample::aboutQt()
{
    QMessageBox::aboutQt( this, "Qt Menu Example" );
}


void MenuExample::printer()
{
    emit explain( "File/Printer/Print selected" );
}

void MenuExample::file()
{
    emit explain( "File/Printer/Print To File selected" );
}

void MenuExample::fax()
{
    emit explain( "File/Printer/Print To Fax selected" );
}

void MenuExample::printerSetup()
{
    emit explain( "File/Printer/Printer Setup selected" );
}


void MenuExample::resizeEvent( QResizeEvent * )
{
    label->setGeometry( 20, rect().center().y()-20, width()-40, 40 );
}


int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    MenuExample m;

    a.setMainWidget( &m );
    m.setCaption("Qt Examples - Menus");
    m.show();
    return a.exec();
}
