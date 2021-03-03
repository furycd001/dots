/****************************************************************************
** $Id: qt/src/dialogs/qmessagebox.cpp   2.3.2   edited 2001-10-21 $
**
** Implementation of QMessageBox class
**
** Created : 950503
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the dialogs module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qmessagebox.h"

#ifndef QT_NO_MESSAGEBOX

#include "qlabel.h"
#include "qpushbutton.h"
#include "qimage.h"
#include "qapplication.h"
#if defined QT_NON_COMMERCIAL
#include "qnc_win.h"
#endif

// Message box icons, from page 210 of the Windows style guide.

// Hand-drawn to resemble Microsoft's icons, but in the Mac/Netscape
// palette.  The "question mark" icon, which Microsoft recommends not
// using but a lot of people still use, is left out.

/* XPM */
static const char * const information_xpm[]={
"32 32 5 1",
". c None",
"c c #000000",
"* c #999999",
"a c #ffffff",
"b c #0000ff",
"...........********.............",
"........***aaaaaaaa***..........",
"......**aaaaaaaaaaaaaa**........",
".....*aaaaaaaaaaaaaaaaaa*.......",
"....*aaaaaaaabbbbaaaaaaaac......",
"...*aaaaaaaabbbbbbaaaaaaaac.....",
"..*aaaaaaaaabbbbbbaaaaaaaaac....",
".*aaaaaaaaaaabbbbaaaaaaaaaaac...",
".*aaaaaaaaaaaaaaaaaaaaaaaaaac*..",
"*aaaaaaaaaaaaaaaaaaaaaaaaaaaac*.",
"*aaaaaaaaaabbbbbbbaaaaaaaaaaac*.",
"*aaaaaaaaaaaabbbbbaaaaaaaaaaac**",
"*aaaaaaaaaaaabbbbbaaaaaaaaaaac**",
"*aaaaaaaaaaaabbbbbaaaaaaaaaaac**",
"*aaaaaaaaaaaabbbbbaaaaaaaaaaac**",
"*aaaaaaaaaaaabbbbbaaaaaaaaaaac**",
".*aaaaaaaaaaabbbbbaaaaaaaaaac***",
".*aaaaaaaaaaabbbbbaaaaaaaaaac***",
"..*aaaaaaaaaabbbbbaaaaaaaaac***.",
"...caaaaaaabbbbbbbbbaaaaaac****.",
"....caaaaaaaaaaaaaaaaaaaac****..",
".....caaaaaaaaaaaaaaaaaac****...",
"......ccaaaaaaaaaaaaaacc****....",
".......*cccaaaaaaaaccc*****.....",
"........***cccaaaac*******......",
"..........****caaac*****........",
".............*caaac**...........",
"...............caac**...........",
"................cac**...........",
".................cc**...........",
"..................***...........",
"...................**..........."};
/* XPM */
static const char* const warning_xpm[]={
"32 32 4 1",
". c None",
"a c #ffff00",
"* c #000000",
"b c #999999",
".............***................",
"............*aaa*...............",
"...........*aaaaa*b.............",
"...........*aaaaa*bb............",
"..........*aaaaaaa*bb...........",
"..........*aaaaaaa*bb...........",
".........*aaaaaaaaa*bb..........",
".........*aaaaaaaaa*bb..........",
"........*aaaaaaaaaaa*bb.........",
"........*aaaa***aaaa*bb.........",
".......*aaaa*****aaaa*bb........",
".......*aaaa*****aaaa*bb........",
"......*aaaaa*****aaaaa*bb.......",
"......*aaaaa*****aaaaa*bb.......",
".....*aaaaaa*****aaaaaa*bb......",
".....*aaaaaa*****aaaaaa*bb......",
"....*aaaaaaaa***aaaaaaaa*bb.....",
"....*aaaaaaaa***aaaaaaaa*bb.....",
"...*aaaaaaaaa***aaaaaaaaa*bb....",
"...*aaaaaaaaaa*aaaaaaaaaa*bb....",
"..*aaaaaaaaaaa*aaaaaaaaaaa*bb...",
"..*aaaaaaaaaaaaaaaaaaaaaaa*bb...",
".*aaaaaaaaaaaa**aaaaaaaaaaa*bb..",
".*aaaaaaaaaaa****aaaaaaaaaa*bb..",
"*aaaaaaaaaaaa****aaaaaaaaaaa*bb.",
"*aaaaaaaaaaaaa**aaaaaaaaaaaa*bb.",
"*aaaaaaaaaaaaaaaaaaaaaaaaaaa*bbb",
"*aaaaaaaaaaaaaaaaaaaaaaaaaaa*bbb",
".*aaaaaaaaaaaaaaaaaaaaaaaaa*bbbb",
"..*************************bbbbb",
"....bbbbbbbbbbbbbbbbbbbbbbbbbbb.",
".....bbbbbbbbbbbbbbbbbbbbbbbbb.."};
/* XPM */
static const char* const critical_xpm[]={
"32 32 4 1",
". c None",
"a c #999999",
"* c #ff0000",
"b c #ffffff",
"...........********.............",
".........************...........",
".......****************.........",
"......******************........",
".....********************a......",
"....**********************a.....",
"...************************a....",
"..*******b**********b*******a...",
"..******bbb********bbb******a...",
".******bbbbb******bbbbb******a..",
".*******bbbbb****bbbbb*******a..",
"*********bbbbb**bbbbb*********a.",
"**********bbbbbbbbbb**********a.",
"***********bbbbbbbb***********aa",
"************bbbbbb************aa",
"************bbbbbb************aa",
"***********bbbbbbbb***********aa",
"**********bbbbbbbbbb**********aa",
"*********bbbbb**bbbbb*********aa",
".*******bbbbb****bbbbb*******aa.",
".******bbbbb******bbbbb******aa.",
"..******bbb********bbb******aaa.",
"..*******b**********b*******aa..",
"...************************aaa..",
"....**********************aaa...",
"....a********************aaa....",
".....a******************aaa.....",
"......a****************aaa......",
".......aa************aaaa.......",
".........aa********aaaaa........",
"...........aaaaaaaaaaa..........",
".............aaaaaaa............"};


// the Qt logo, for aboutQt
/* XPM */
static const char * const qtlogo_xpm[] = {
/* width height ncolors chars_per_pixel */
"50 50 17 1",
/* colors */
"  c #000000",
". c #495808",
"X c #2A3304",
"o c #242B04",
"O c #030401",
"+ c #9EC011",
"@ c #93B310",
"# c #748E0C",
"$ c #A2C511",
"% c #8BA90E",
"& c #99BA10",
"* c #060701",
"= c #181D02",
"- c #212804",
"; c #61770A",
": c #0B0D01",
"/ c None",
/* pixels */
"/$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$/",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$+++$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$@;.o=::=o.;@$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$+#X*         **X#+$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$#oO*         O  **o#+$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$&.* OO              O*.&$$$$$$$$$$$$$",
"$$$$$$$$$$$$@XOO            * OO    X&$$$$$$$$$$$$",
"$$$$$$$$$$$@XO OO  O  **:::OOO OOO   X@$$$$$$$$$$$",
"$$$$$$$$$$&XO      O-;#@++@%.oOO      X&$$$$$$$$$$",
"$$$$$$$$$$.O  :  *-#+$$$$$$$$+#- : O O*.$$$$$$$$$$",
"$$$$$$$$$#*OO  O*.&$$$$$$$$$$$$+.OOOO **#$$$$$$$$$",
"$$$$$$$$+-OO O *;$$$$$$$$$$$&$$$$;*     o+$$$$$$$$",
"$$$$$$$$#O*  O .+$$$$$$$$$$@X;$$$+.O    *#$$$$$$$$",
"$$$$$$$$X*    -&$$$$$$$$$$@- :;$$$&-    OX$$$$$$$$",
"$$$$$$$@*O  *O#$$$$$$$$$$@oOO**;$$$#    O*%$$$$$$$",
"$$$$$$$;     -+$$$$$$$$$@o O OO ;+$$-O   *;$$$$$$$",
"$$$$$$$.     ;$$$$$$$$$@-OO OO  X&$$;O    .$$$$$$$",
"$$$$$$$o    *#$$$$$$$$@o  O O O-@$$$#O   *o$$$$$$$",
"$$$$$$+=    *@$$$$$$$@o* OO   -@$$$$&:    =$$$$$$$",
"$$$$$$+:    :+$$$$$$@-      *-@$$$$$$:    :+$$$$$$",
"$$$$$$+:    :+$$$$$@o* O    *-@$$$$$$:    :+$$$$$$",
"$$$$$$$=    :@$$$$@o*OOO      -@$$$$@:    =+$$$$$$",
"$$$$$$$-    O%$$$@o* O O    O O-@$$$#*   OX$$$$$$$",
"$$$$$$$. O *O;$$&o O*O* *O      -@$$;    O.$$$$$$$",
"$$$$$$$;*   Oo+$$;O*O:OO--      Oo@+=    *;$$$$$$$",
"$$$$$$$@*  O O#$$$;*OOOo@@-O     Oo;O*  **@$$$$$$$",
"$$$$$$$$X* OOO-+$$$;O o@$$@-    O O     OX$$$$$$$$",
"$$$$$$$$#*  * O.$$$$;X@$$$$@-O O        O#$$$$$$$$",
"$$$$$$$$+oO O OO.+$$+&$$$$$$@-O         o+$$$$$$$$",
"$$$$$$$$$#*    **.&$$$$$$$$$$@o      OO:#$$$$$$$$$",
"$$$$$$$$$+.   O* O-#+$$$$$$$$+;O    OOO:@$$$$$$$$$",
"$$$$$$$$$$&X  *O    -;#@++@#;=O    O    -@$$$$$$$$",
"$$$$$$$$$$$&X O     O*O::::O      OO    Oo@$$$$$$$",
"$$$$$$$$$$$$@XOO                  OO    O*X+$$$$$$",
"$$$$$$$$$$$$$&.*       **  O      ::    *:#$$$$$$$",
"$$$$$$$$$$$$$$$#o*OO       O    Oo#@-OOO=#$$$$$$$$",
"$$$$$$$$$$$$$$$$+#X:* *     O**X#+$$@-*:#$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$%;.o=::=o.#@$$$$$$@X#$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$+++$$$$$$$$$$$+$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"/$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$/",
};




// BEING REVISED: paul
/*!
\class QMessageBox qmessagebox.h
\brief Displays a brief message, an icon, and some buttons.
\ingroup dialogs

A message box is a modal dialog that displays an icon, a text and up to
three push buttons.  It's used for simple messages and questions.

QMessageBox provides a range of different messages, arranged roughly
along two axes: Severity and complexity.

Severity is
<ul>
<li> \c Information - for message boxes that are part of normal operation
<li> \c Warning - for message boxes that tell the user about unusual errors
<li> \c Critical - as Warning, but for critical errors
</ul>

The message box has a different icon for each of the severity levels.


Complexity is one button (Ok) for a simple messages, or two or even
three buttons for questions.

There are static functions that let you do most of the common jobs,
for example:

If a program is unable to find a supporting file, but can do perfectly
well without:

\code
  QMessageBox::information( this, "Application name",
                            "Unable to find the user preferences file.\n"
			    "The factory default will be used instead." );
\endcode

warning() can be used to tell the user about unusual errors, or
errors which can't be easily fixed:

\code
  switch( QMessageBox::warning( this, "Application name",
                                "Could not connect to the <mumble> server.\n"
      			  "This program can't function correctly "
      			  "without the server.\n\n",
      			  "Try again", "Quit", 0,
      			  0, 1 );
  case 0: // Try again or Enter
      // try again
      break;
  case 1: // Quit or Escape
      // exit
      break;
  }
\endcode

    Finally,

  The text part of all message box messages can be either rich text or
  plain text. If you specify a rich text formatted string, it will be
  rendered using the default stylesheet. See
  QStyleSheet::defaultSheet() for details. With certain strings that
  contain XML meta characters, the auto-rich text detection may fail,
  interpreting plain text falsely as rich text. In these rare cases,
  use QStyleSheet::convertFromPlainText() to convert your plain text
  string to a visually equivalent rich text string or set the text
  format explicitly with setTextFormat().

  Here are some examples of how to use the static member functions.
  After these examples you will find an overview of the non-static
  member functions.

  If a program is unable to find a supporting file, it may perhaps do:

  \code
    QMessageBox::information( this, "Application name here",
                              "Unable to find the file \"index.html\".\n"
			      "The factory default will be used instead." );
  \endcode

  The Microsoft Windows User Interface Guidelines strongly recommends
  using the application name as window caption.  The message box has
  just one button, OK, and its text tells the user both what happened
  and what the program will do about it.  Since the application is
  able to make do, the message box is just information, not a warning
  or a critical error.

  Exiting a program is part of its normal operation, and if there are
  unsaved data the user probably should be asked what to do, for
  example like this:

  \code
    switch( QMessageBox::information( this, "Application name here",
                                      "The document contains unsaved work\n"
                                      "Do you want to save it before exiting?",
			              "&Save", "&Don't Save", "&Cancel",
                                      0,      // Enter == button 0
				      2 ) ) { // Escape == button 2
    case 0: // Save clicked, Alt-S or Enter pressed.
        // save
	break;
    case 1: // Don't Save clicked or Alt-D pressed
        // don't save but exit
	break;
    case 2: // Cancel clicked, Alt-C or Escape pressed
        // don't exit
	break;
    }
  \endcode

  Again, the application name is used as window caption, as Microsoft
  recommends.  The Escape button cancels the entire Exit operation,
  and Enter/Return saves the document and exits.

  Disk full errors are unusual (in a perfect world, they are) and they
  certainly can be hard to correct.  This example uses predefined buttons
  instead of hardcoded button texts:

  \code
    switch( QMessageBox::warning( this, "Application name here",
                                  "Could not save the the user preferences,\n"
				  "because the disk is full.  You can delete\n"
				  "some files and press Retry, or you can\n"
				  "abort the Save Preferences operation.",
				  QMessageBox::Retry | QMessageBox::Default,
				  QMessageBox::Abort | QMessageBox::Escape )) {
    case QMessageBox::Retry: // Retry or Enter
        // try again
	break;
    case QMessageBox::Abort: // Abort or Cancel
        // abort
	break;
    }
  \endcode

  The critical() function should be reserved for critical errors.  In
  this example, errorDetails is a QString or const char*, and QString
  is used to concatenate several strings:

  \code
    QMessageBox::critical( 0, "Application name here",
                           QString("An internal error occurred. Please ") +
			   "call technical support at 123456789 and report\n"+
			   "these numbers:\n\n" + errorDetails +
			   "\n\n<Application> will now exit." );
  \endcode

  QMessageBox provides a very simple About box, which displays an
  appropriate icon and the string you give it:

  \code
     QMessageBox::about( this, "About <Application>",
                         "<Application> is a <one-paragraph blurb>\n\n"
			 "Copyright 1951-1997 Such-and-such.  "
			 "<License words here.>\n\n"
			 "For technical support, call 123456789 or see\n"
			 "http://www.such-and-such.com/Application/\n" );
  \endcode

  See about() for more information.

  Finally, you can make a QMessageBox from scratch and set custom
  button texts:

  \code
    QMessageBox mb( "Application name here",
		    "Saving the file will overwrite the old file on disk.\n"
		    "Do you really want to save?",
		    QMessageBox::Information,
		    QMessageBox::Yes | QMessageBox::Default,
		    QMessageBox::No,
		    QMessageBox::Cancel | QMessageBox::Escape );
    mb.setButtonText( QMessageBox::Yes, "Save" );
    mb.setButtonText( QMessageBox::No, "Don't Save" );
    switch( mb.exec() ) {
        case QMessageBox::Yes:
	    // save and exit
	    break;
        case QMessageBox::No:
	    // exit without saving
	    break;
	case QMessageBox::Cancel:
	    // don't save and don't exit
	    break;
    }
  \endcode

  QMessageBox defines two enum types, Icon and an unnamed button type.
  Icon defines the \c Information, \c Warning and \c Critical icons for
  each GUI style.  It is used by the constructor, by the static member
  functions information(), warning() and critical(), and there is a
  function called standardIcon() which gives you access to the various
  icons.

  The button types are:
  <ul>
  <li> \c Ok - the default for single-button message boxes
  <li> \c Cancel - note that this is \e not automatically Escape
  <li> \c Yes
  <li> \c No
  <li> \c Abort
  <li> \c Retry
  <li> \c Ignore
  </ul>

  Button types can be combined with two modifiers by using OR:
  <ul>
  <li> \c Default - makes pressing Enter or Return be equivalent with
  clicking this button.  Normally used with Ok, Yes or similar.
  <li> \c Escape - makes pressing Escape be equivalent with this button.
  Normally used with Abort, Cancel or similar.
  </ul>

  The text(), icon() and iconPixmap() functions provide access to the
  current text and pixmap of a message box, and setText(), setIcon()
  and setIconPixmap() lets you change it.  The difference between
  setIcon() and setIconPixmap() is that the former accepts a
  QMessageBox::Icon and can it be used to set standard icons while the
  latter accepts a QPixmap and can be used to set custom icons.

  setButtonText() and buttonText() provide access to the buttons.

  QMessageBox has no signals or slots.

  <img src=qmsgbox-m.png> <img src=qmsgbox-w.png>

  \sa QDialog, <a href="http://www.iarchitect.com/errormsg.htm">Isys on
  error messages,</a>
  <a href="guibooks.html#fowler">GUI Design Handbook: Message Box.</a>

*/


/*!
  \enum QMessageBox::Icon

  This type includes the following values:
  <ul>
  <li> \c QMessageBox::NoIcon
  <li> \c QMessageBox::Information
  <li> \c QMessageBox::Warning
  <li> \c QMessageBox::Critical
  </ul>
*/


struct QMBData {
    QMBData(QMessageBox* parent) :
	iconLabel( parent, "icon" )
    {
    }

    int			numButtons;		// number of buttons
    QMessageBox::Icon	icon;			// message box icon
    QLabel		iconLabel;		// label holding any icon
    int			button[3];		// button types
    int			defButton;		// default button (index)
    int			escButton;		// escape button (index)
    QSize		buttonSize;		// button size
    QPushButton	       *pb[3];			// buttons
};

static const int LastButton = QMessageBox::Ignore;

/*
  NOTE: The table of button texts correspond to the button enum.
*/

static const char *mb_texts[] = {
    0,
    QT_TRANSLATE_NOOP("QMessageBox","OK"),
    QT_TRANSLATE_NOOP("QMessageBox","Cancel"),
    QT_TRANSLATE_NOOP("QMessageBox","Yes"),
    QT_TRANSLATE_NOOP("QMessageBox","No"),
    QT_TRANSLATE_NOOP("QMessageBox","Abort"),
    QT_TRANSLATE_NOOP("QMessageBox","Retry"),
    QT_TRANSLATE_NOOP("QMessageBox","Ignore"),
    0
};


/*!
  Constructs a message box with no text and a button with the text "OK".

  If \e parent is 0, then the message box becomes an application-global
  modal dialog box.  If \e parent is a widget, the message box becomes
  modal relative to \e parent.

  The \e parent and \e name arguments are passed to the QDialog constructor.
*/

QMessageBox::QMessageBox( QWidget *parent, const char *name )
    : QDialog( parent, name, TRUE, WStyle_Customize | WStyle_DialogBorder | WStyle_Title | WStyle_SysMenu )
{
    if ( parent && parent->icon() && !parent->icon()->isNull() )
	QDialog::setIcon( *parent->icon() );
    else if ( qApp->mainWidget() && qApp->mainWidget()->icon() && !qApp->mainWidget()->icon()->isNull() )
	QDialog::setIcon( *qApp->mainWidget()->icon() );

    init( Ok, 0, 0 );
}


/*!
  Constructs a message box with a \a caption, a \a text, an \a icon and up
  to three buttons.

  The \a icon must be one of:
  <ul>
  <li> \c QMessageBox::NoIcon
  <li> \c QMessageBox::Information
  <li> \c QMessageBox::Warning
  <li> \c QMessageBox::Critical
  </ul>

  Each button can have one of the following values:
  <ul>
  <li>\c QMessageBox::NoButton
  <li>\c QMessageBox::Ok
  <li>\c QMessageBox::Cancel
  <li>\c QMessageBox::Yes
  <li>\c QMessageBox::No
  <li>\c QMessageBox::Abort
  <li>\c QMessageBox::Retry
  <li>\c QMessageBox::Ignore
  </ul>

  Use QMessageBox::NoButton for the later parameters to have less than
  three buttons in your message box.

  One of the buttons can be combined with the \c QMessageBox::Default flag
  to make a default button.

  One of the buttons can be combined with the \c QMessageBox::Escape flag
  to make an escape option.  Hitting the Esc key on the keyboard has
  the same effect as clicking this button with the mouse.

  Example:
  \code
    QMessageBox mb( "Hardware failure",
		    "Disk error detected\nDo you want to stop?",
		    QMessageBox::NoIcon,
		    QMessageBox::Yes | QMessageBox::Default,
		    QMessageBox::No | QMessageBox::Escape );
    if ( mb.exec() == QMessageBox::No )
        // try again
  \endcode

  If \a parent is 0, then the message box becomes an application-global
  modal dialog box.  If \a parent is a widget, the message box becomes
  modal relative to \e parent.

  If \a modal is TRUE the message becomes modal, otherwise it becomes
  modeless.

  The \a parent, \a name, \a modal and \a f arguments are passed to the
  QDialog constructor.

  \sa setCaption(), setText(), setIcon()
*/

QMessageBox::QMessageBox( const QString& caption,
			  const QString &text, Icon icon,
			  int button0, int button1, int button2,
			  QWidget *parent, const char *name,
			  bool modal, WFlags f )
    : QDialog( parent, name, modal, f | WStyle_Customize | WStyle_DialogBorder | WStyle_Title | WStyle_SysMenu )
{
    if ( parent && parent->icon() && !parent->icon()->isNull() )
	QDialog::setIcon( *parent->icon() );
    else if ( qApp->mainWidget() && qApp->mainWidget()->icon() && !qApp->mainWidget()->icon()->isNull() )
	QDialog::setIcon( *qApp->mainWidget()->icon() );

    init( button0, button1, button2 );
    setCaption( caption );
    setText( text );
    setIcon( icon );
}


/*!
  Destructs the message box.
*/

QMessageBox::~QMessageBox()
{
    delete mbd;
}

static QString * translatedTextAboutQt = 0;

void QMessageBox::init( int button0, int button1, int button2 )
{
    if ( !translatedTextAboutQt ) {
	translatedTextAboutQt = new QString;

#if defined(QT_NON_COMMERCIAL)
    QT_NC_MSGBOX
#else
	*translatedTextAboutQt
	    = tr( "<h3>About Qt</h3>"
		  "<p>This program uses Qt version %1, a multiplatform C++ "
		  "GUI toolkit from Trolltech. Qt provides single-source "
		  "portability across Windows 95/98/NT4/Me/2000, Linux, Solaris, "
		  "HP-UX and many other versions of Unix with X11.</p>"
		  "<p>See <tt>http://www.trolltech.com/qt/</tt> for more "
		  "information.</p>" ).arg( QT_VERSION_STR );
#endif

#ifdef QT_NO_RICHTEXT
	QString plain;
	bool on=TRUE;
	for (int i=0; i<translatedTextAboutQt->length(); i++) {
	    char ch = translatedTextAboutQt->at(i).latin1();
	    switch ( ch ) {
	     case '<': on=FALSE; break;
	     case '>': on=TRUE; break;
	     default:
		if ( on )
		    plain += translatedTextAboutQt->at(i);
		else if ( ch == 'p' || ch == '3' )
		    plain += '\n';
	    }
	}
	*translatedTextAboutQt = plain;
#endif
    }
    label = new QLabel( this, "text" );
    CHECK_PTR( label );

#ifdef QT_NO_RICHTEXT
    label->setAlignment( AlignLeft|WordBreak );
#else
    label->setAlignment( AlignLeft );
#endif

    if ( (button2 && !button1) || (button1 && !button0) ) {
#if defined(CHECK_RANGE)
	qWarning( "QMessageBox: Inconsistent button parameters" );
#endif
	button0 = button1 = button2 = 0;
    }
    mbd = new QMBData(this);
    CHECK_PTR( mbd );
    mbd->numButtons = 0;
    mbd->button[0] = button0;
    mbd->button[1] = button1;
    mbd->button[2] = button2;
    mbd->defButton = -1;
    mbd->escButton = -1;
    int i;
    for ( i=0; i<3; i++ ) {
	int b = mbd->button[i];
	if ( (b & Default) ) {
	    if ( mbd->defButton >= 0 ) {
#if defined(CHECK_RANGE)
		qWarning( "QMessageBox: There can be at most one "
			   "default button" );
#endif
	    } else {
		mbd->defButton = i;
	    }
	}
	if ( (b & Escape) ) {
	    if ( mbd->escButton >= 0 ) {
#if defined(CHECK_RANGE)
		qWarning( "QMessageBox: There can be at most one "
			   "escape button" );
#endif
	    } else {
		mbd->escButton = i;
	    }
	}
	b &= ButtonMask;
	if ( b == 0 ) {
	    if ( i == 0 )			// no buttons, add an Ok button
		b = Ok;
	} else if ( b < 0 || b > LastButton ) {
#if defined(CHECK_RANGE)
	    qWarning( "QMessageBox: Invalid button specifier" );
#endif
	    b = Ok;
	} else {
	    if ( i > 0 && mbd->button[i-1] == 0 ) {
#if defined(CHECK_RANGE)
		qWarning( "QMessageBox: Inconsistent button parameters; "
			   "button %d defined but not button %d",
			   i+1, i );
#endif
		b = 0;
	    }
	}
	mbd->button[i] = b;
	if ( b )
	    mbd->numButtons++;
    }
    for ( i=0; i<3; i++ ) {
	if ( i >= mbd->numButtons ) {
	    mbd->pb[i] = 0;
	} else {
	    QCString buttonName;
	    buttonName.sprintf( "button%d", i+1 );
	    mbd->pb[i] = new QPushButton(
		tr(mb_texts[mbd->button[i]]),
	        this, buttonName );
	    if ( mbd->defButton == i ) {
		mbd->pb[i]->setDefault( TRUE );
		mbd->pb[i]->setFocus();
	    }
	    mbd->pb[i]->setAutoDefault( TRUE );
	    mbd->pb[i]->setFocusPolicy( QWidget::StrongFocus );
	    connect( mbd->pb[i], SIGNAL(clicked()), SLOT(buttonClicked()) );
	}
    }
    resizeButtons();
    reserved1 = reserved2 = 0;
    setFontPropagation( SameFont );
    setPalettePropagation( SamePalette );
}


int QMessageBox::indexOf( int button ) const
{
    int index = -1;
    for ( int i=0; i<mbd->numButtons; i++ ) {
	if ( mbd->button[i] == button ) {
	    index = i;
	    break;
	}
    }
    return index;
}


void QMessageBox::resizeButtons()
{
    int i;
    QSize maxSize( style() == MotifStyle ? 0 : 75, 0 );
    for ( i=0; i<mbd->numButtons; i++ ) {
	QSize s = mbd->pb[i]->sizeHint();
	maxSize.setWidth(  QMAX(maxSize.width(), s.width()) );
	maxSize.setHeight( QMAX(maxSize.height(),s.height()) );
    }
    mbd->buttonSize = maxSize;
    for ( i=0; i<mbd->numButtons; i++ )
	mbd->pb[i]->resize( maxSize );
}


/*!
  Returns the message box text currently set, or a
  \link QString::operator!() null string\endlink
  if no text has been set.
  \sa setText(), textFormat()
*/

QString QMessageBox::text() const
{
    return label->text();
}

/*!
  Sets the message box text to be displayed.

  \a text will be interpreted either as a plain text or as a rich
  text, depending on the text format setting; see setTextFormat(). The
  default setting is \c AutoText, i.e. the message box will try to
  auto-detect the format of \a text.

  \sa text(), setTextFormat()
*/

void QMessageBox::setText( const QString &text )
{
    label->setText( text );
}

/*!
  Returns the icon of the message box.

  \sa setIcon(), iconPixmap()
*/

QMessageBox::Icon QMessageBox::icon() const
{
    return mbd->icon;
}


/*!
  Sets the icon of the message box to \a icon, which is a predefined icon:

  <ul>
  <li> \c QMessageBox::NoIcon
  <li> \c QMessageBox::Information
  <li> \c QMessageBox::Warning
  <li> \c QMessageBox::Critical
  </ul>

  The actual pixmap used for displaying the icon depends on the current
  \link style() GUI style\endlink.  You can also set a custom pixmap icon
  using the setIconPixmap() function.

  \sa icon(), setIconPixmap(), iconPixmap()
*/
//#### Bad API (see QWidget::setIcon). Should be setMessageIcon in 3.0 (same for setIconPixmap and friends)
void QMessageBox::setIcon( Icon icon )
{
    setIconPixmap( standardIcon(icon, style()) );
    mbd->icon = icon;
}

/*!
  Returns the pixmap used for a standard icon.  This
  allows the pixmaps to be used in more complex message boxes.
*/

QPixmap QMessageBox::standardIcon( Icon icon, GUIStyle style )
{
    const char * const * xpm_data;
    switch ( icon ) {
    case Information:
	xpm_data = information_xpm;
	break;
    case Warning:
	xpm_data = warning_xpm;
	break;
    case Critical:
	xpm_data = critical_xpm;
	break;
    default:
	xpm_data = 0;
    }
    QPixmap pm;
    if ( xpm_data ) {
	QImage image( (const char **) xpm_data);
	if ( style == MotifStyle ) {
	    // All that color looks ugly in Motif
	    QColorGroup g = QApplication::palette().normal();
	    switch ( icon ) {
	    case Information:
		image.setColor( 2, 0xff000000 | g.dark().rgb() );
		image.setColor( 3, 0xff000000 | g.base().rgb() );
		image.setColor( 4, 0xff000000 | g.text().rgb() );
		break;
	    case Warning:
		image.setColor( 1, 0xff000000 | g.base().rgb() );
		image.setColor( 2, 0xff000000 | g.text().rgb() );
		image.setColor( 3, 0xff000000 | g.dark().rgb() );
		break;
	    case Critical:
		image.setColor( 1, 0xff000000 | g.dark().rgb() );
		image.setColor( 2, 0xff000000 | g.text().rgb() );
		image.setColor( 3, 0xff000000 | g.base().rgb() );
	        break;
	    default:
		break; // Can't happen
	    }
	}
	pm.convertFromImage(image);
    }
    return pm;
}


/*!
  Returns the icon pixmap of the message box.

  Example:
  \code
    QMessageBox mb(...);
    mb.setIcon( QMessageBox::Warning );
    mb.iconPixmap();	// returns the warning icon pixmap
  \endcode

  \sa setIconPixmap(), icon()
*/

const QPixmap *QMessageBox::iconPixmap() const
{
    return mbd->iconLabel.pixmap();
}

/*!
  Sets the icon of the message box to a custom \a pixmap.  Note that
  it's often hard to draw one pixmap which looks appropriate in both
  Motif and Windoes GUI styles.  You may want to draw two.

  \sa iconPixmap(), setIcon()
*/

void QMessageBox::setIconPixmap( const QPixmap &pixmap )
{
    mbd->iconLabel.setPixmap(pixmap);
    mbd->icon = NoIcon;
}


/*!
  Returns the text of the message box button \a button, or null if the
  message box does not contain the button.

  \sa setButtonText()
*/

QString QMessageBox::buttonText( int button ) const
{
    int index = indexOf(button);
    return index >= 0 && mbd->pb[index]
	    ? mbd->pb[index]->text()
	    : QString::null;
}


/*!
  Sets the text of the message box button \a button to \a text.
  Setting the text of a button that is not in the message box is quietly
  ignored.

  \sa buttonText()
*/

void QMessageBox::setButtonText( int button, const QString &text )
{
    int index = indexOf(button);
    if ( index >= 0 && mbd->pb[index] ) {
	mbd->pb[index]->setText( text );
	resizeButtons();
    }
}


/*!
  Internal slot to handle button clicks.
*/

void QMessageBox::buttonClicked()
{
    int reply = 0;
    const QObject *s = sender();
    for ( int i=0; i<mbd->numButtons; i++ ) {
	if ( mbd->pb[i] == s )
	    reply = mbd->button[i];
    }
    done( reply );
}


/*!
  Adjusts the size of the message box to fit the contents just before
  QDialog::exec() or QDialog::show() is called.

  This function will not be called if the message box has been explicitly
  resized before showing it.
*/
void QMessageBox::adjustSize()
{
    if ( !testWState(WState_Polished) )
	polish();
    resizeButtons();
    label->adjustSize();
    QSize labelSize( label->size() );
    int n  = mbd->numButtons;
    int bw = mbd->buttonSize.width();
    int bh = mbd->buttonSize.height();
    int border = bh/2 - style().buttonDefaultIndicatorWidth();
    if ( border <= 0 )
	border = 10;
    int btn_spacing = 7;
    if ( style() == MotifStyle )
	btn_spacing = border;
    int buttons = mbd->numButtons * bw + (n-1) * btn_spacing;
    int h = bh;
    if ( labelSize.height() )
	h += labelSize.height() + 3*border;
    else
	h += 2*border;
    int lmargin = 0;
    if ( mbd->iconLabel.pixmap() && mbd->iconLabel.pixmap()->width() )  {
	mbd->iconLabel.adjustSize();
	lmargin += mbd->iconLabel.width() + border;
	if ( h < mbd->iconLabel.height() + 3*border + bh )
	    h = mbd->iconLabel.height() + 3*border + bh;
    }
    int w = QMAX( buttons, labelSize.width() + lmargin ) + 2*border;

#if defined(_WS_QWS_)
    // who commited QMessagebox? It really should use Layout. This way
    // we do not even have a proper minimum size we could take into
    // acount. ####
    extern QRect qt_maxWindowRect;
    QSize s( w, h );
    s = s.boundedTo( qt_maxWindowRect.size() );
    if ( s.width() < w && label->sizePolicy().hasHeightForWidth() ) {
	resize( s );
	QApplication::sendPostedEvents( this, QEvent::Resize );
	s.rheight() += label->heightForWidth( label->width() ) - label->height();
	s = s.boundedTo( qt_maxWindowRect.size() );
    }
    resize( s );
#else
    resize( w, h );
#endif
    setMinimumSize( size() );
}


/*!\reimp
*/
void QMessageBox::resizeEvent( QResizeEvent * )
{
    int i;
    int n  = mbd->numButtons;
    int bw = mbd->buttonSize.width();
    int bh = mbd->buttonSize.height();
    int border = bh/2 - style().buttonDefaultIndicatorWidth();
    if ( border <= 0 )
	border = 10;
    int btn_spacing = 7;
    if ( style() == MotifStyle )
	btn_spacing = border;
    int lmargin = 0;
    mbd->iconLabel.adjustSize();
    mbd->iconLabel.move( border, border );
    if ( mbd->iconLabel.pixmap() && mbd->iconLabel.pixmap()->width() )
	lmargin += mbd->iconLabel.width() + border;
    label->setGeometry( lmargin+border,
			border,
			width() - lmargin -2*border,
			height() - 3*border - bh );
    int extra_space = (width() - bw*n - 2*border - (n-1)*btn_spacing);
    if ( style() == MotifStyle )
	for ( i=0; i<n; i++ )
	    mbd->pb[i]->move( border + i*bw + i*btn_spacing + extra_space*(i+1)/(n+1),
			      height() - border - bh );
    else
	for ( i=0; i<n; i++ )
	    mbd->pb[i]->move( border + i*bw + extra_space/2 + i*btn_spacing,
			      height() - border - bh );
}


/*!\reimp
*/
void QMessageBox::keyPressEvent( QKeyEvent *e )
{
    if ( e->key() == Key_Escape ) {
	if ( mbd->escButton >= 0 ) {
	    QPushButton *pb = mbd->pb[mbd->escButton];
	    pb->animateClick();
	    e->accept();
	    return;
	}
    }
    QDialog::keyPressEvent( e );
}


/*****************************************************************************
  Static QMessageBox functions
 *****************************************************************************/

/*!
  Opens a modal message box directly using the specified parameters.

  \warning This function is kept for compatibility with old Qt programs
  and will be removed in a future version of Qt.  Please use
  information(), warning() or critical() instead.
*/

int QMessageBox::message( const QString &caption,
			  const QString& text,
			  const QString& buttonText,
			  QWidget    *parent,
			  const char * )
{
    return QMessageBox::information( parent, caption, text,
				     buttonText.isEmpty()
				     ? tr("OK") : buttonText ) == 0;
}


/*!
  Queries the user using a modal message box with two buttons.
  Note that \a caption is not always shown, it depends on the window manager.

  \warning This function is kept for compatibility with old Qt programs
  and will be removed in a future version of Qt.  Please use
  information(), warning() or critical() instead.
*/

bool QMessageBox::query( const QString &caption,
			 const QString& text,
			 const QString& yesButtonText,
			 const QString& noButtonText,
			 QWidget *parent, const char * )
{
    return QMessageBox::information( parent, caption, text,
				     yesButtonText.isEmpty()
				     ? tr("OK") : yesButtonText,
				     noButtonText ) == 0;
}


/*!
  Opens an information message box with a caption, a text and up to three
  buttons.  Returns the identifier of the button that was clicked.

  If \e parent is 0, then the message box becomes an application-global
  modal dialog box.  If \e parent is a widget, the message box becomes
  modal relative to \e parent.

  \sa warning(), critical()
*/

int QMessageBox::information( QWidget *parent,
			      const QString& caption, const QString& text,
			      int button0, int button1, int button2 )
{
    QMessageBox *mb = new QMessageBox( caption, text, Information,
				       button0, button1, button2,
				       parent, "information" );
    CHECK_PTR( mb );
    int reply = mb->exec();
    delete mb;
    return reply;
}


/*!
  Opens a warning message box with a caption, a text and up to three
  buttons.  Returns the identifier of the button that was clicked.

  If \e parent is 0, then the message box becomes an application-global
  modal dialog box.  If \e parent is a widget, the message box becomes
  modal relative to \e parent.

  \sa information(), critical()
*/

int QMessageBox::warning( QWidget *parent,
			  const QString& caption, const QString& text,
			  int button0, int button1, int button2 )
{
    QMessageBox *mb = new QMessageBox( caption, text, Warning,
				       button0, button1, button2,
				       parent, "warning" );
    CHECK_PTR( mb );
    int reply = mb->exec();
    delete mb;
    return reply;
}


/*!
  Opens a critical message box with a caption, a text and up to three
  buttons.  Returns the identifier of the button that was clicked.

  If \e parent is 0, then the message box becomes an application-global
  modal dialog box.  If \e parent is a widget, the message box becomes
  modal relative to \e parent.

  \sa information(), warning()
*/

int QMessageBox::critical( QWidget *parent,
			   const QString& caption, const QString& text,
			   int button0, int button1, int button2 )
{
    QMessageBox *mb = new QMessageBox( caption, text, Critical,
				       button0, button1, button2,
				       parent, "critical" );
    CHECK_PTR( mb );
    int reply = mb->exec();
    delete mb;
    return reply;
}


/*!
  Displays a simple about box with window caption \a caption and
  body text \a text.

  about() looks for a suitable icon for the box in four locations:
  <ol> <li>It prefers \link QWidget::icon() parent->icon() \endlink
  if that exists.  <li>If not, it tries the top level widget
  containing \a parent <li>If that too fails, it tries the \link
  QApplication::mainWidget() main widget. \endlink <li>As a last
  resort it uses the Information icon. </ol>

  The about box has a single button labelled OK.

  \sa QWidget::icon() QApplication::mainWidget()
*/

void QMessageBox::about( QWidget *parent, const QString &caption,
			 const QString& text )
{
    QMessageBox *mb = new QMessageBox( caption, text,
				       Information,
				       Ok + Default, 0, 0,
				       parent, "simple about box" );
    CHECK_PTR( mb );
    QPixmap i;
    if ( parent && parent->icon())
	i = *(parent->icon());
    if ( i.isNull() && parent &&
	 parent->topLevelWidget()->icon() )
	i = *(parent->topLevelWidget()->icon());
    if ( i.isNull() && qApp && qApp->mainWidget() &&
	 qApp->mainWidget()->icon() )
	i = *(qApp->mainWidget()->icon());
    if ( !i.isNull() )
	mb->setIconPixmap( i );
    mb->exec();
    delete mb;
}


/*! \reimp
*/

void QMessageBox::styleChanged( QStyle& )
{
    if ( mbd->icon != NoIcon ) {
	// Reload icon for new style
	setIcon( mbd->icon );
    }
}


static int textBox( QWidget *parent, QMessageBox::Icon severity,
		    const QString& caption, const QString& text,
		    const QString& button0Text,
		    const QString& button1Text,
		    const QString& button2Text,
		    int defaultButtonNumber,
		    int escapeButtonNumber )
{
    int b[3];
    b[0] = 1;
    b[1] = button1Text.isEmpty() ? 0 : 2;
    b[2] = button2Text.isEmpty() ? 0 : 3;

    int i;
    for( i=0; i<3; i++ ) {
	if ( b[i] && defaultButtonNumber == i )
	    b[i] += QMessageBox::Default;
	if ( b[i] && escapeButtonNumber == i )
	    b[i] += QMessageBox::Escape;
    }

    QMessageBox *mb = new QMessageBox( caption, text, severity,
				       b[0], b[1], b[2],
				       parent, "information" );
    CHECK_PTR( mb );
    if ( button0Text.isEmpty() )
	mb->setButtonText( 1, QMessageBox::tr(mb_texts[QMessageBox::Ok]) );
    else
	mb->setButtonText( 1, button0Text );
    if ( b[1] )
	mb->setButtonText( 2, button1Text );
    if ( b[2] )
	mb->setButtonText( 3, button2Text );

#ifndef QT_NO_CURSOR
    mb->setCursor( Qt::arrowCursor );
#endif
    int reply = mb->exec();

    delete mb;
    return reply-1;
}


/*!
  Displays an information message box with a caption, a text and
  1-3 buttons.  Returns the number of the button that was clicked
  (0, 1 or 2).

  \a button0Text is the text of the first button and is optional.  If
  \a button0Text is not supplied, "OK" (translated) will be used.
  \a button1Text is the text of the second button and is optional.
  \a button2Text is the text of the third button and is optional.  \a
  defaultbuttonNumber (0-2) is the index of the default button;
  pressing Return or Enter is the same as clicking the default button.
  It defaults to 0 (the first button).  \a escapeButtonNumber is the
  index of the Escape button; pressing Escape is the same as clicking
  this button.  It defaults to -1 (pressing Escape does nothing);
  supply 0, 1 or 2 to make pressing Escape be equivalent with clicking
  the relevant button.

  If \e parent is 0, then the message box becomes an application-global
  modal dialog box.  If \e parent is a widget, the message box becomes
  modal relative to \e parent.

  \sa warning(), critical()
*/

int QMessageBox::information( QWidget *parent, const QString &caption,
			      const QString& text,
			      const QString& button0Text,
			      const QString& button1Text,
			      const QString& button2Text,
			      int defaultButtonNumber,
			      int escapeButtonNumber )
{
    return textBox( parent, Information, caption, text,
		    button0Text, button1Text, button2Text,
		    defaultButtonNumber, escapeButtonNumber );
}


/*!
  Displays a warning message box with a caption, a text and
  1-3 buttons.  Returns the number of the button that was clicked
  (0, 1 or 2).

  \a button0Text is the text of the first button and is optional.  If
  \a button0Text is not supplied, "OK" (translated) will be used.
  \a button1Text is the text of the second button and is optional, and
  \a button2Text is the text of the third button and is optional.  \a
  defaultbuttonNumber (0-2) is the index of the default button;
  pressing Return or Enter is the same as clicking the default button.
  It defaults to 0 (the first button).  \a escapeButtonNumber is the
  index of the Escape button; pressing Escape is the same as clicking
  this button.  It defaults to -1 (pressing Escape does nothing);
  supply 0, 1 or 2 to make pressing Escape be equivalent with clicking
  the relevant button.

  If \e parent is 0, then the message box becomes an application-global
  modal dialog box.  If \e parent is a widget, the message box becomes
  modal relative to \e parent.

  \sa information(), critical()
*/

int QMessageBox::warning( QWidget *parent, const QString &caption,
				 const QString& text,
				 const QString& button0Text,
				 const QString& button1Text,
				 const QString& button2Text,
				 int defaultButtonNumber,
				 int escapeButtonNumber )
{
    return textBox( parent, Warning, caption, text,
		    button0Text, button1Text, button2Text,
		    defaultButtonNumber, escapeButtonNumber );
}


/*!
  Displays a critical error message box with a caption, a text and
  1-3 buttons.  Returns the number of the button that was clicked
  (0, 1 or 2).

  \a button0Text is the text of the first button and is optional.  If
  \a button0Text is not supplied, "OK" (translated) will be used.
  \a button1Text is the text of the second button and is optional, and
  \a button2Text is the text of the third button and is optional.  \a
  defaultbuttonNumber (0-2) is the index of the default button;
  pressing Return or Enter is the same as clicking the default button.
  It defaults to 0 (the first button).  \a escapeButtonNumber is the
  index of the Escape button; pressing Escape is the same as clicking
  this button.  It defaults to -1 (pressing Escape does nothing);
  supply 0, 1 or 2 to make pressing Escape be equivalent with clicking
  the relevant button.

  If \e parent is 0, then the message box becomes an application-global
  modal dialog box.  If \e parent is a widget, the message box becomes
  modal relative to \e parent.

  \sa information() warning()
*/

int QMessageBox::critical( QWidget *parent, const QString &caption,
				  const QString& text,
				  const QString& button0Text,
				  const QString& button1Text,
				  const QString& button2Text,
				  int defaultButtonNumber,
				  int escapeButtonNumber )
{
    return textBox( parent, Critical, caption, text,
		    button0Text, button1Text, button2Text,
		    defaultButtonNumber, escapeButtonNumber );
}


/*!
  Displays a simple message box about Qt, with window caption \a
  caption and optionally centered over \a parent.  The message includes
  the version number of Qt being used by the application.

  This is neat for inclusion into the Help menu.  See the menu.cpp
  example.
*/

void QMessageBox::aboutQt( QWidget *parent, const QString &caption )
{
    QString c = caption;
    if ( c.isNull() )
	c = "About Qt";
    QMessageBox *mb = new QMessageBox( parent, "about qt" );
    mb->setCaption( caption.isNull()?QString::fromLatin1("About Qt"):caption );
    mb->setText( *translatedTextAboutQt );
    QPixmap pm;
    QImage logo( (const char **)qtlogo_xpm);
    if ( qGray(mb->palette().active().text().rgb()) >
         qGray(mb->palette().active().base().rgb()) )
    {
	// light on dark, adjust some colors
	logo.setColor( 0,0xFFffffff);
	logo.setColor( 1,0xFF666666);
	logo.setColor( 2,0xFFcccc66);
	logo.setColor( 4,0xFFcccccc);
	logo.setColor( 6,0xFFffff66);
	logo.setColor( 7,0xFF999999);
	logo.setColor( 8,0xFF3333FF);
	logo.setColor( 9,0xFFffff33);
	logo.setColor(11,0xFFcccc99);
    }
    if ( pm.convertFromImage( logo ) )
	mb->setIconPixmap( pm );
    mb->setButtonText( 0, tr("OK") );
    if ( mb->mbd && mb->mbd->pb[0] ) {
	mb->mbd->pb[0]->setAutoDefault( TRUE );
	mb->mbd->pb[0]->setFocusPolicy( QWidget::StrongFocus );
	mb->mbd->pb[0]->setDefault( TRUE );
	mb->mbd->pb[0]->setFocus();
    }
    mb->exec();
}

/*!
  \reimp
*/

void QMessageBox::setIcon( const QPixmap &pix )
{
    //reimplemented to avoid compiler warning.
    QDialog::setIcon( pix );
}


/*!
  Returns the current text format.

  \sa setTextFormat()
*/

Qt::TextFormat QMessageBox::textFormat() const
{
    return label->textFormat();
}

/*!
  Sets the text format to \a format. See the Qt::TextFormat enum for
  an explanation of the possible options.

  The default format is \c AutoText.

  \sa textFormat(), setText()
*/

void QMessageBox::setTextFormat( Qt::TextFormat format )
{
    label->setTextFormat( format );
}


// My own personal favorite minimalist error message popped up whilst
// testing Freehand 8 last month.  I took a screen shot.  I believe I
// was trying to convert a file from one format to another.
// Apparently, I...
//
// http://www.people.cornell.edu/pages/mlj8/cant.gif

#endif
