/****************************************************************************
** $Id: qt/src/dialogs/qfontdialog.cpp   2.3.2   edited 2001-09-11 $
**
** Implementation of QFontDialog
**
** Created : 970605
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

#include "qwindowdefs.h"

#ifndef QT_NO_FONTDIALOG

#include "qfontdialog.h"

#include "qlineedit.h"
#include "qlistbox.h"
#include "qpushbutton.h"
#include "qcheckbox.h"
#include "qcombobox.h"
#include "qlayout.h"
#include "qvgroupbox.h"
#include "qhgroupbox.h"
#include "qhbox.h"
#include "qlabel.h"
#include "qapplication.h"
#include "qfontdatabase.h"

#include <ctype.h>

// BEING REVISED: hanord
/*!
  \class QFontDialog qfontdialog.h
  \brief The QFontDialog class provides a dialog widget for selecting a text font.
  \ingroup dialogs

  This dialog can be used to let the user choose a font with attributes and so on.
  Normally you may use the static convenience function getFont().

  <img src=qfontdlg-m.png> <img src=qfontdlg-w.png>
*/

class QFontDialogPrivate
{
public:
    QFontDialogPrivate(){};
    QLabel * familyAccel;
    QLineEdit * familyEdit;
    QListBox * familyList;

    QLabel * styleAccel;
    QLineEdit * styleEdit;
    QListBox * styleList;

    QLabel * sizeAccel;
    QLineEdit * sizeEdit;
    QListBox * sizeList;

    QVGroupBox * effects;
    QCheckBox * strikeout;
    QCheckBox * underline;
    QLabel * colorAccel;
    QComboBox * color;

    QHGroupBox * sample;
    QLineEdit * sampleEdit;

    QLabel * scriptAccel;
    QComboBox * scriptCombo;

    QPushButton * ok;
    QPushButton * cancel;

    QBoxLayout * buttonLayout;
    QBoxLayout * effectsLayout;
    QBoxLayout * sampleLayout;
    QBoxLayout * sampleEditLayout;

    QFontDatabase fdb;
    QString       family;
    QString       charSet;
    QString       style;
    QString       size;

    QStringList familyNames;
    QStringList charSetNames;
    QStringList charSetSamples;
    bool usingStandardSizes;
};


/*!
  Constructs a default font dialog. Use setFont() for setting
  the initial values.

  \sa getFont()
*/

QFontDialog::QFontDialog( QWidget *parent, const char *name,
			  bool modal, WFlags f )
    : QDialog( parent, name, modal, f )
{
    setSizeGripEnabled( TRUE );
    d = new QFontDialogPrivate;
    // grid
    d->familyEdit = new QLineEdit( this, "font family I" );
    d->familyEdit->setFocusPolicy( StrongFocus );
    d->familyList = new QListBox( this, "font family II" );
    d->familyList->viewport()->setFocusProxy( d->familyEdit );

    d->familyAccel
	= new QLabel( d->familyEdit, tr("&Font"), this, "family accelerator" );
    d->familyAccel->setIndent( 2 );

    d->styleEdit = new QLineEdit( this, "font style I" );
    d->styleEdit->setFocusPolicy( StrongFocus );
    d->styleList = new QListBox( this, "font style II" );
    d->styleList->viewport()->setFocusProxy( d->styleEdit );
    d->styleAccel
	= new QLabel( d->styleEdit, tr("Font st&yle"), this, "style accelerator" );
    d->styleAccel->setIndent( 2 );

    d->sizeEdit = new QLineEdit( this, "font size I" );
    d->sizeEdit->setFocusPolicy( StrongFocus );
    d->sizeList = new QListBox( this, "font size II" );
    d->sizeList->viewport()->setFocusProxy( d->sizeEdit );
    d->sizeAccel
	= new QLabel ( d->sizeEdit, tr("&Size"), this, "size accelerator" );
    d->sizeAccel->setIndent( 2 );

    // effects box
    d->effects = new QVGroupBox( tr("Effects"), this, "font effects" );
    d->strikeout = new QCheckBox( d->effects, "strikeout on/off" );
    d->strikeout->setText( tr("Stri&keout") );
    d->underline = new QCheckBox( d->effects, "underline on/off" );
    d->underline->setText( tr("&Underline") );

#if 0
    d->color = new QComboBox( TRUE, d->effects, "pen color" );
    d->color->setEnabled( FALSE );
    d->colorAccel
	= new QLabel( d->color, tr("&Color"), d->effects, "color label" );
    d->colorAccel->setIndent( 2 );
#endif

    d->sample = new QHGroupBox( tr("Sample"), this, "sample text" );
    d->sampleEdit = new QLineEdit( d->sample, "r/w sample text" );
    d->sampleEdit->setFixedHeight( 75 );

    // Note that the sample text is *not* translated with tr(), as the
    // characters used depend on the charset encoding.
    d->sampleEdit->setText( "AaBbYyZz" );

    d->sampleEdit->setAlignment( AlignCenter );

    d->scriptCombo = new QComboBox( TRUE, this, "font encoding" );
    d->scriptCombo->setFocusPolicy( StrongFocus );

    d->scriptAccel
	= new QLabel( d->scriptCombo, tr("Scr&ipt"), this,"encoding label");
    d->scriptAccel->setIndent( 2 );

    d->usingStandardSizes = FALSE;

#if 0
    connect( d->familyList, SIGNAL(highlighted(const QString&)),
	     SLOT(familyHighlighted(const QString&)) );
#else
    connect( d->familyList, SIGNAL(highlighted(int)),
	     SLOT(familyHighlighted(int)) );
#endif
    connect( d->scriptCombo, SIGNAL(activated(int)),
	     SLOT(scriptHighlighted(int)) );
    connect( d->styleList, SIGNAL(highlighted(const QString&)),
	     SLOT(styleHighlighted(const QString&)) );
    connect( d->sizeList, SIGNAL(highlighted(const QString&)),
	     SLOT(sizeHighlighted(const QString&)) );

    connect( d->sizeEdit, SIGNAL(textChanged( const QString &)),
	     SLOT(sizeChanged(const QString&)) );

    connect( d->strikeout, SIGNAL(clicked()),
	     SLOT(updateSample()) );
    connect( d->underline, SIGNAL(clicked()),
	     SLOT(updateSample()) );

    updateFamilies();
    if ( d->familyList->count() != 0 )
	d->familyList->setCurrentItem( 0 );

    QSize sz;
    sz = d->familyList->sizeHint();
    sz = d->styleList->sizeHint();
    sz = d->sizeList->sizeHint();

    // grid layout
    QGridLayout * mainGrid = new QGridLayout( this, 9, 6, 12, 0 );

    mainGrid->addWidget( d->familyAccel, 0, 0 );
    mainGrid->addWidget( d->familyEdit, 1, 0 );
    mainGrid->addWidget( d->familyList, 2, 0 );

    mainGrid->addWidget( d->styleAccel, 0, 2 );
    mainGrid->addWidget( d->styleEdit, 1, 2 );
    mainGrid->addWidget( d->styleList, 2, 2 );

    mainGrid->addWidget( d->sizeAccel, 0, 4 );
    mainGrid->addWidget( d->sizeEdit, 1, 4 );
    mainGrid->addWidget( d->sizeList, 2, 4 );

    mainGrid->setColStretch( 0, 38 );
    mainGrid->setColStretch( 2, 24 );
    mainGrid->setColStretch( 4, 10 );

    mainGrid->addColSpacing( 1, 6 );
    mainGrid->addColSpacing( 3, 6 );
    mainGrid->addColSpacing( 5, 6 );

    mainGrid->addRowSpacing( 3, 12 );

    mainGrid->addWidget( d->effects, 4, 0 );

    mainGrid->addMultiCellWidget( d->sample, 4, 7, 2, 4 );

    mainGrid->addWidget( d->scriptAccel, 5, 0 );
    mainGrid->addRowSpacing( 6, 2 );
    mainGrid->addWidget( d->scriptCombo, 7, 0 );

    mainGrid->addRowSpacing( 8, 12 );

    QHBoxLayout *buttonBox = new QHBoxLayout;
    mainGrid->addMultiCell( buttonBox, 9, 9, 0, 4 );

    buttonBox->addStretch( 1 );
    QString okt = modal ? tr("OK") : tr("Apply");
    d->ok = new QPushButton( okt, this, "accept font selection" );
    buttonBox->addWidget( d->ok );
    if ( modal )
	connect( d->ok, SIGNAL(clicked()), SLOT(accept()) );
    connect( d->ok, SIGNAL(clicked()), SLOT(emitSelectedFont()) );
    d->ok->setDefault( TRUE );
    d->ok->setFixedWidth( 80 );

    buttonBox->addSpacing( 12 );

    QString cancelt = modal ? tr("Cancel") : tr("Close");
    d->cancel = new QPushButton( cancelt, this, "cancel/close" );
    buttonBox->addWidget( d->cancel );
    connect( d->cancel, SIGNAL(clicked()), SLOT(reject()) );
    d->cancel->setFixedWidth( 80 );

    resize( 500, 360 );

    d->familyEdit->installEventFilter( this );
    d->styleEdit->installEventFilter( this );
    d->sizeEdit->installEventFilter( this );
    d->familyList->installEventFilter( this );
    d->styleList->installEventFilter( this );
    d->sizeList->installEventFilter( this );

    d->familyEdit->setFocus();
}

/*! Destructs the font dialog and frees up its storage. */

QFontDialog::~QFontDialog()
{
    delete d;
    d = 0;
}

/*!
  Opens a modal font dialog and returns the font selected by the user.

  \a initial is the initial selected font.

  The \a ok parameter is set to TRUE if the user clicked OK, and FALSE if
  the user clicked Cancel.

  If the user clicks Cancel the \a initial font is returned.

  This static function is less capable than the full QFontDialog object,
  but is convenient and easy to use.

  Example:
  \code
    // start at the current working directory and with *.cpp as filter
    bool ok;
    QFont f = QFontDialog::getFont( &ok, QFont( "Times", 12 ), this );
    if ( ok ) {
        // the user selected a valid font
    } else {
        // the user cancelled the dialog
    }
  \endcode

  Another example:
  \code
    mywidget.setFont( QFontDialog::getFont( 0, mywidget.font() ) );
  \endcode
*/
QFont QFontDialog::getFont( bool *ok, const QFont &initial,
			    QWidget *parent, const char* name)
{
    return getFont( ok, &initial, parent, name );
}

/*!
  Opens a modal font dialog and returns the font selected by the user.

  The \a ok parameter is set to TRUE if the user clicked OK, and FALSE if
  the user clicked Cancel.

  If the user clicks Cancel the Qt default font is returned.

  This static function is less capable than the full QFontDialog object,
  but is convenient and easy to use.

  Example:
  \code
    // start at the current working directory and with *.cpp as filter
    bool ok;
    QFont f = QFontDialog::getFont( &ok, this );
    if ( ok ) {
        // the user selected a valid font
    } else {
        // the user cancelled the dialog
    }
  \endcode

*/
QFont QFontDialog::getFont( bool *ok, QWidget *parent,const char* name)
{
    return getFont( ok, 0, parent, name );
}

QFont QFontDialog::getFont( bool *ok, const QFont *def,
			    QWidget *parent, const char* name)
{
    QFont result;
    if ( def )
	result = *def;

    QFontDialog *dlg = new QFontDialog( parent, name, TRUE );
    if ( def )
	dlg->setFont( *def );
    dlg->setCaption( tr("Select Font") );
    if ( dlg->exec() == QDialog::Accepted ) {
	result = dlg->font();
	if ( ok )
	    *ok = TRUE;
    } else {
	if ( ok )
	    *ok = FALSE;
    }
    delete dlg;
    return result;
}

/*! Returns a pointer to the "font family" list box.  This is usable
  mainly if you reimplement updateFontFamilies();
*/

QListBox * QFontDialog::familyListBox() const
{
    return d->familyList;
}

/*! Returns a pointer to the "font style" list box.  This is usable
  mainly if you reimplement updateFontStyles();
*/

QListBox * QFontDialog::styleListBox() const
{
    return d->styleList;
}

/*! Returns a pointer to the "font style" list box.  This is usable
  mainly if you reimplement updateFontStyles();
*/

QComboBox * QFontDialog::scriptCombo() const
{
    return d->scriptCombo;
}

/*! Returns a pointer to the "font size" list box.  This is usable
  mainly if you reimplement updateFontSizes();
*/

QListBox * QFontDialog::sizeListBox() const
{
    return d->sizeList;
}

/*!
  This slot is called if the user changes the font size.
*/

void QFontDialog::sizeChanged( const QString &s )
{
    bool ok = FALSE;
    if ( d->size != s ) {
	(void) s.toInt( &ok );
	if ( ok ) {
	    d->size = s;
	    updateSample();
	}
    }
}


/*!  Event filter to make up, down, pageup and pagedown work correctly
  in the line edits.
*/

bool QFontDialog::eventFilter( QObject * o , QEvent * e )
{
    if ( !o || !e )
	return FALSE;


    if ( e->type() == QEvent::KeyPress ) {
	QListBox * lb = 0;
	QLineEdit * le = 0;

	if ( o == d->familyEdit ) {
	    lb = d->familyList;
	    le = d->familyEdit;
	} else if ( o == d->styleEdit ) {
	    lb = d->styleList;
	    le = d->styleEdit;
	} else if ( o == d->sizeEdit ) {
	    lb = d->sizeList;
	    le = d->sizeEdit;
	} else {
	    return FALSE;
	}

	QKeyEvent * k = (QKeyEvent *)e;
	if ( k->key() == Key_Up ||
	     k->key() == Key_Down ||
	     k->key() == Key_Prior ||
	     k->key() == Key_Next ) {
	    int ci = lb->currentItem();
	    (void)QApplication::sendEvent( lb, k );
	    if ( ci != lb->currentItem() && style() == WindowsStyle )
		le->selectAll();
	    return TRUE;
	}
    } else if ( e->type() == QEvent::FocusIn && style() == WindowsStyle ) {
	if ( o == d->familyEdit )
	    d->familyEdit->selectAll();
	else if ( o == d->styleEdit )
	    d->styleEdit->selectAll();
	else if ( o == d->sizeEdit )
	    d->sizeEdit->selectAll();
    } else if ( e->type() == QEvent::MouseButtonPress ) {
	if ( o == d->familyList )
	    d->familyEdit->setFocus();
	else if ( o == d->styleList )
	    d->styleEdit->setFocus();
	else if ( o == d->sizeList )
	    d->sizeEdit->setFocus();
    }
    return QDialog::eventFilter( o, e );
}



/*!  Update the contents of the "font family" list box.  This
  function can be reimplemented if you have special requirements.
*/

void QFontDialog::updateFamilies()
{
    d->familyNames = d->fdb.families( FALSE );
    QStringList newList;
    QString s;
    QStringList::Iterator it = d->familyNames.begin();
    for( ; it != d->familyNames.end() ; it++ ) {
	s = *it;
	if ( s.contains('-') ) {
	    int i = s.find('-');
	    s = s.right( s.length() - i - 1 ) + " [" + s.left( i ) + "]";
	}
	s[0] = s[0].upper();
#if 0
	if ( d->fdb.isSmoothlyScalable( *it ) )
	    newList.append( s + "(TT)" );
	else if ( d->fdb.isBitmapScalable( *it ) )
	    newList.append( s + "(BT)" );
	else
#endif
	newList.append( s );
    }
    d->familyList->insertStringList( newList );
}

/*!  Update the contents of the "font script" combo box.  This
  function can be reimplemented if you have special requirements.
*/

void QFontDialog::updateScripts()
{
    d->scriptCombo->clear();

    d->charSetNames = d->fdb.charSets( d->family, FALSE );

    if ( d->charSetNames.isEmpty() ) {
	qWarning( "QFontDialog::updateFamilies: Internal error, "
		  "no character sets for family \"%s\"",
		  (const char *) d->family );
	d->charSetNames.append( "Any" );
    }

    QStringList::Iterator it = d->charSetNames.begin();
    for ( ; it != d->charSetNames.end() ; ++it ) 
	d->scriptCombo->insertItem( d->fdb.verboseCharSetName(*it) );
}

/*!  Update the contents of the "font style" list box.  This
  function can be reimplemented if you have special requirements.
*/

void QFontDialog::updateStyles()
{
    d->styleList->clear();

    QStringList styles = d->fdb.styles( d->family, d->charSet );
    if ( styles.isEmpty() ) {
	qWarning( "QFontDialog::updateFamilies: Internal error, "
		  "no styles for family \"%s\" with script \"%s\"",
		  (const char *) d->family, (const char *) d->charSet );
	return;
    }
    d->styleList->insertStringList( styles );
}

/*!  Update the contents of the "font size" list box.  This
  function can be reimplemented if you have special requirements.
*/

void QFontDialog::updateSizes()
{
    //    usingStandardSizes = d->fdb.isScalable( d->family );

    d->sizeList->clear();
    QValueList<int> sizes = d->fdb.pointSizes( d->family,d->style, d->charSet);
    if ( sizes.isEmpty() ) {
	qWarning( "QFontDialog::updateFamilies: Internal error, "
		  "no pointsizes for family \"%s\" with script \"%s\"\n"
		  "and style \"%s\"",
		  (const char *) d->family, (const char *) d->charSet,
		  (const char *) d->style );
	return;
    }
    int i;
    QString tmp;
    for( i = 0 ; (uint)i < sizes.count() ; i++ ) {
	tmp.sprintf( "%i", sizes[i] );
	d->sizeList->insertItem( tmp );
    }
}

/*!

*/

void QFontDialog::familyHighlighted( const QString &s )
{
    d->familyEdit->setText(d->familyList->text(d->familyList->currentItem()));
    if ( style() == WindowsStyle && d->familyEdit->hasFocus() )
	d->familyEdit->selectAll();

    d->family = s;
    updateScripts();
    if ( d->scriptCombo->count() != 0 )
	scriptHighlighted( 0 );
}

void QFontDialog::familyHighlighted( int i )
{
    QString s = d->familyNames[i];
    familyHighlighted( s );
}

/*!

*/

void QFontDialog::scriptHighlighted( int index )
{
    scriptHighlighted( d->charSetNames[index] );
    d->sampleEdit->setText( d->fdb.charSetSample( d->charSetNames[index] ) );
}

void QFontDialog::scriptHighlighted( const QString &s )
{
    d->charSet = s;
    QString currentStyle = d->styleList->currentText();
    updateStyles();
    if ( d->styleList->count() != 0 ) {
	for ( int i = 0 ; i < (int)d->styleList->count() ; i++ ) {
	    if ( currentStyle == d->styleList->text(i) ) {
		d->styleList->setCurrentItem( i ); // Will call styleHighlighted
		break;
	    }
	}
        if ( d->styleList->currentItem() == -1 )
	    d->styleList->setCurrentItem( 0 ); // Will call styleHighlighted
    }
}

/*!

*/

void QFontDialog::styleHighlighted( const QString &s )
{
    d->styleEdit->setText( s );
    if ( style() == WindowsStyle && d->styleEdit->hasFocus() )
	d->styleEdit->selectAll();

    d->style = s;

    if ( d->usingStandardSizes && d->fdb.isScalable( d->family ) ) {
	updateSample();
	return;
    }
    int pSize = d->size.toInt();
    updateSizes();
    QString tmp;
    if ( d->sizeList->count() != 0 ) {
	int i;
	for ( i = 0 ; i < (int)d->sizeList->count() - 1 ; i++ ) {
	    tmp = d->sizeList->text(i);
	    if ( tmp.toInt() >= pSize )
		break;
	}
	d->sizeList->setCurrentItem( i );
    }
}


/*!

*/

void QFontDialog::sizeHighlighted( const QString &s )
{
    d->sizeEdit->setText( s );
    if ( style() == WindowsStyle && d->sizeEdit->hasFocus() )
	d->sizeEdit->selectAll();

    d->size = s;
    updateSample();
}

/*!
  Sets the \a f to the font which values should be set
  in the QFontDialog.

  \sa font()
*/

void QFontDialog::setFont( const QFont &f )
{
#ifdef _WS_WIN32_
    QString famNam = f.family();
#else 
    QString famNam = f.family().lower();
#endif

    QStringList::Iterator it;
    it = d->familyNames.begin();
    int i = 0;
    for( ; it != d->familyNames.end() ; ++it ) {
	QString s = *it;
	if ( famNam == s ) {
	    d->familyList->setCurrentItem( i );
	    i = -1;
	    break;
	}
	if ( s.contains('-') ) {
	    int j = s.find('-');
	    if ( famNam == s.right( s.length() - j - 1 ) ) {
		d->familyList->setCurrentItem( i );
		i = -1;
		break;
	    }
	}
	i++;
    }

    QString styleString = d->fdb.styleString( f );
    if ( !styleString.isEmpty() && d->styleList->count() != 0 ) {
	for ( i = 0 ; i < (int)d->styleList->count() ; i++ ) {
	    if ( styleString == d->styleList->text(i) ) {
		d->styleList->setCurrentItem( i );
		break;
	    }
	}
    }

    if ( d->sizeList->count() != 0 ) {
	int pSize = f.pointSize();
	for ( i = 0 ; i < (int)d->sizeList->count() - 1 ; i++ ) {
	    QString tmp = d->sizeList->text(i);
	    if ( tmp.toInt() >= pSize )
		break;
	}
	d->sizeList->setCurrentItem( i );
    }

    d->strikeout->setChecked( f.strikeOut() );
    d->underline->setChecked( f.underline() );
    
    updateSample();
}

/*!
  Returns the font which the user has chosen.

  \sa setFont()
*/

QFont QFontDialog::font() const
{
    int pSize = d->size.toInt();
    if ( pSize == 0 )
	pSize = 12;
    QFont f = d->fdb.font( d->family, d->style, pSize, d->charSet );
    f.setStrikeOut( d->strikeout->isChecked() );
    f.setUnderline( d->underline->isChecked() );
    return f;
}

void QFontDialog::updateSample()
{
    d->sampleEdit->setFont( font() );
    emit fontHighlighted(font());
}

void QFontDialog::emitSelectedFont()
{
    emit fontSelected(font());
}

/*!
  \fn void QFontDialog::fontSelected( const QFont & )

  This signal is emitted, when the user has chosen a font and clicked ok.
*/

/*!
  \fn void QFontDialog::fontHighlighted( const QFont & )

  This signal is emitted, when the user changed a setting in the dialog.
*/

#endif
