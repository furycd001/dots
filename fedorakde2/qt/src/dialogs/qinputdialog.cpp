/****************************************************************************
** $Id: qt/src/dialogs/qinputdialog.cpp   2.3.2   edited 2001-03-07 $
**
** Implementation of QInputDialog class
**
** Created : 991212
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

#include "qinputdialog.h"

#ifndef QT_NO_INPUTDIALOG

#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qwidgetstack.h>
#include <qvalidator.h>
#include <qapplication.h>

class QInputDialogPrivate
{
public:
    friend class QInputDialog;
    QLineEdit *lineEdit;
    QSpinBox *spinBox;
    QComboBox *comboBox, *editComboBox;
    QPushButton *ok;
    QWidgetStack *stack;
    QInputDialog::Type type;
};

/*!
  \class QInputDialog qinputdialog.h
  \brief A convenience dialog to get a simple input from the user
  \ingroup dialogs

  The QInputDialog is a simple dialog which can be used if you
  need a simple input from the user. This can be text, a number or
  an item from a list. Also a label has to be set to tell the user
  what he/she should input.

  In this Qt version only the 4 static convenience functions
  getText(), getInteger(), getDouble() and getItem() of QInputDialog
  are available.

  Use it like this:

  \code
  bool ok = FALSE;
  QString text = QInputDialog::getText( tr( "Make an input" ), tr( "Please enter your name" ), QString::null, &ok, this );
  if ( ok && !text.isEmpty() )
      ;// user entered something and pressed ok
  else
      ;// user entered nothing or pressed cancel
  \endcode

  There are more static convenience methods!

  \sa getText(), getInteger(), getDouble(), getItem()
*/

/*!
  \enum QInputDialog::Type

  This enum type specifies the type of the dialog
  (which kind of input can be done):

  <ul>
  <li>\c LineEdit - A QLineEdit is used for taking the input, so a textual or
  (e.g. using a QValidator) a numerical input can be done. Using lineEdit()
  the QLineEdit can be accessed.
  <li>\c SpinBox - A QSpinBox is used for taking the input, so a decimal
  input can be done. Using spinBox() the QSpinBox can be accessed.
  <li>\c ComboBox - A read-only QComboBox is used for taking the input,
  so one item of a list can be chosen. Using comboBox() the QComboBox
  can be accessed.
  <li>\c EditableComboBox - An editable QComboBox is used for taking the input,
  so either one item of a list can be chosen or a text can be entered. Using
  editableComboBox() the QComboBox can be accessed.
  </ul>
*/

/*!
  Constructs the dialog. \a label is the text which is shown to the user (it should mention
  to the user what he/she should input), \a parent the parent widget of the dialog, \a name
  the name of it and if you set \a modal to TRUE, the dialog pops up modally, else it pops
  up modeless. With \a type you specify the type of the dialog.

  \sa getText(), getInteger(), getDouble(), getItem()
*/

QInputDialog::QInputDialog( const QString &label, QWidget* parent, const char* name,
			  bool modal, Type type)
    : QDialog( parent, name, modal )
{
    if ( parent && parent->icon() &&!parent->icon()->isNull() )
	setIcon( *parent->icon() );
    else if ( qApp->mainWidget() && qApp->mainWidget()->icon() && !qApp->mainWidget()->icon()->isNull() )
	QDialog::setIcon( *qApp->mainWidget()->icon() );

    d = new QInputDialogPrivate;
    d->lineEdit = 0;
    d->spinBox = 0;
    d->comboBox = 0;

    QVBoxLayout *vbox = new QVBoxLayout( this, 6, 6 );

    QLabel* l = new QLabel( label, this );
    vbox->addWidget( l );

    d->stack = new QWidgetStack( this );
    vbox->addWidget( d->stack );
    d->lineEdit = new QLineEdit( d->stack );
    d->spinBox = new QSpinBox( d->stack );
    d->comboBox = new QComboBox( FALSE, d->stack );
    d->editComboBox = new QComboBox( TRUE, d->stack );

    QHBoxLayout *hbox = new QHBoxLayout( 6 );
    vbox->addLayout( hbox, AlignRight );

    d->ok = new QPushButton( tr( "&OK" ), this );
    d->ok->setDefault( TRUE );
    QPushButton *cancel = new QPushButton( tr( "&Cancel" ), this );

    QSize bs( d->ok->sizeHint() );
    if ( cancel->sizeHint().width() > bs.width() )
	bs.setWidth( cancel->sizeHint().width() );

    d->ok->setFixedSize( bs );
    cancel->setFixedSize( bs );

    hbox->addWidget( new QWidget( this ) );
    hbox->addWidget( d->ok );
    hbox->addWidget( cancel );

    connect( d->lineEdit, SIGNAL( returnPressed() ),
	     this, SLOT( tryAccept() ) );
    connect( d->lineEdit, SIGNAL( textChanged( const QString & ) ),
	     this, SLOT( textChanged( const QString & ) ) );

    connect( d->ok, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( cancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

    resize( QMAX( sizeHint().width(), 400 ), sizeHint().height() );

    setType( type );
}

/*!
  Returns the line edit, which is used in the LineEdit mode
*/

QLineEdit *QInputDialog::lineEdit() const
{
    return d->lineEdit;
}

/*!
  Returns the spinbox, which is used in the SpinBox mode
*/

QSpinBox *QInputDialog::spinBox() const
{
    return d->spinBox;
}

/*!
  Returns the combobox, which is used in the ComboBox mode
*/

QComboBox *QInputDialog::comboBox() const
{
    return d->comboBox;
}

/*!
  Returns the combobox, which is used in the EditableComboBox mode
*/

QComboBox *QInputDialog::editableComboBox() const
{
    return d->editComboBox;
}

/*!
  Sets the input type of the dialog to \a t.
*/

void QInputDialog::setType( Type t )
{
    switch ( t ) {
    case LineEdit:
	d->stack->raiseWidget( d->lineEdit );
	d->lineEdit->setFocus();
	break;
    case SpinBox:
	d->stack->raiseWidget( d->spinBox );
	d->spinBox->setFocus();
	break;
    case ComboBox:
	d->stack->raiseWidget( d->comboBox );
	d->comboBox->setFocus();
	break;
    case EditableComboBox:
	d->stack->raiseWidget( d->editComboBox );
	d->editComboBox->setFocus();
	break;
    }

    d->type = t;
}

/*!
  Returns the input type of the dialog.

  \sa setType()
*/

QInputDialog::Type QInputDialog::type() const
{
    return d->type;
}

/*!
  Destructor.
*/

QInputDialog::~QInputDialog()
{
    delete d;
}

/*!
  Static convenience function to get a textual input from the user. \a caption is the text
  which is displayed in the title bar of the dialog. \a label is the text which
  is shown to the user (it should mention to the user what he/she should input), \a text
  the default text which will be initially set to the line edit, \a ok a pointer to
  a bool which will be (if not 0!) set to TRUE if the user pressed ok or to FALSE if the
  user pressed cancel, \a parent the parent widget of the dialog and \a name
  the name of it. The dialogs pops up modally!

  This method returns the text which has been entered in the line edit.

  You will use this static method like this:

  \code
  bool ok = FALSE;
  QString text = QInputDialog::getText( tr( "Please enter your name" ), QString::null, &ok, this );
  if ( ok && !text.isEmpty() )
      ;// user entered something and pressed ok
  else
      ;// user entered nothing or pressed cancel
  \endcode
*/

QString QInputDialog::getText( const QString &caption, const QString &label, const QString &text,
			      bool *ok, QWidget *parent, const char *name )
{
    return getText( caption, label, QLineEdit::Normal, text, ok, parent, name );
}

/*!
  Like above, but accepts an a \a mode which the line edit will use to display text.

  \sa getText()
*/

QString QInputDialog::getText( const QString &caption, const QString &label, QLineEdit::EchoMode mode,
			      const QString &text, bool *ok, QWidget *parent, const char *name )
{
    QInputDialog *dlg = new QInputDialog( label, parent, name, TRUE, LineEdit );
    dlg->setCaption( caption );
    dlg->lineEdit()->setText( text );
    dlg->lineEdit()->setEchoMode( mode );
    if ( !text.isEmpty() )
	dlg->lineEdit()->selectAll();

    bool ok_ = FALSE;
    QString result;
    ok_ = dlg->exec() == QDialog::Accepted;
    if ( ok )
	*ok = ok_;
    if ( ok_ )
	result = dlg->lineEdit()->text();

    delete dlg;
    return result;
}

/*!
  Static convenience function to get an integral input from the user. \a caption is the text
  which is displayed in the title bar of the dialog. \a label is the text which
  is shown to the user (it should mention to the user what he/she should input), \a num
  the default number which will be initially set to the spinbox, \a from and \a to the
  range in which the entered number has to be, \a step the step in which the number can
  be increased/decreased by the spinbox, \a ok a pointer to
  a bool which will be (if not 0!) set to TRUE if the user pressed ok or to FALSE if the
  user pressed cancel, \a parent the parent widget of the dialog and \a name
  the name of it. The dialogs pops up modally!

  This method returns the number which has been entered by the user.

  You will use this static method like this:

  \code
  bool ok = FALSE;
  int res = QInputDialog::getInteger( tr( "Please enter a number" ), 22, 0, 1000, 2, &ok, this );
  if ( ok )
      ;// user entered something and pressed ok
  else
      ;// user pressed cancel
  \endcode
*/

int QInputDialog::getInteger( const QString &caption, const QString &label, int num, int from, int to, int step,
			    bool *ok, QWidget *parent, const char *name )
{
    QInputDialog *dlg = new QInputDialog( label, parent, name, TRUE, SpinBox );
    dlg->setCaption( caption );
    dlg->spinBox()->setRange( from, to );
    dlg->spinBox()->setSteps( step, 0 );
    dlg->spinBox()->setValue( num );

    bool ok_ = FALSE;
    int result;
    ok_ = dlg->exec() == QDialog::Accepted;
    if ( ok )
	*ok = ok_;
    result = dlg->spinBox()->value();

    delete dlg;
    return result;
}

/*!
  Static convenience function to get a decimal input from the user. \a caption is the text
  which is displayed in the title bar of the dialog. \a label is the text which
  is shown to the user (it should mention to the user what he/she should input), \a num
  the default decimal number which will be initially set to the line edit, \a from and \a to the
  range in which the entered number has to be, \a decimals the number of decimal which
  the number may have, \a ok a pointer to
  a bool which will be (if not 0!) set to TRUE if the user pressed ok or to FALSE if the
  user pressed cancel, \a parent the parent widget of the dialog and \a name
  the name of it. The dialogs pops up modally!

  This method returns the number which has been entered by the user.

  You will use this static method like this:

  \code
  bool ok = FALSE;
  double res = QInputDialog::getDouble( tr( "Please enter a decimal number" ), 33.7, 0, 1000, 2, &ok, this );
  if ( ok )
      ;// user entered something and pressed ok
  else
      ;// user pressed cancel
  \endcode
*/

double QInputDialog::getDouble( const QString &caption, const QString &label, double num,
				double from, double to, int decimals,
				bool *ok, QWidget *parent, const char *name )
{
    QInputDialog *dlg = new QInputDialog( label, parent, name, TRUE, LineEdit );
    dlg->setCaption( caption );
    dlg->lineEdit()->setValidator( new QDoubleValidator( from, to, decimals, dlg->lineEdit() ) );
    dlg->lineEdit()->setText( QString::number( num ) );
	dlg->lineEdit()->selectAll();

    bool ok_ = FALSE;
    double result;
    ok_ = dlg->exec() == QDialog::Accepted;
    if ( ok )
	*ok = ok_;

    QString editText = dlg->lineEdit()->text();
    int i = dlg->lineEdit()->text().find( '.' );
    if ( i >= 0 ) {
	// has decimal point, now count digits after that
	i++;
	int j = i;
	while( dlg->lineEdit()->text()[j].isDigit() )
	    j++;
        if ( j > decimals )
            editText.truncate( i + decimals );
    }
    result = editText.toDouble();


    delete dlg;
    return result;
}

/*!
  Static convenience function to let the user select an item from a string list. \a caption is the text
  which is displayed in the title bar of the dialog. \a label is the text which
  is shown to the user (it should mention to the user what he/she should input), \a list the
  string list which is inserted into the combobox, \a current the number of the item which should
  be initially the current item, \a editable specifies if the combobox should be editable (if it is TRUE)
  or read-only (if \a editable is FALSE), \a ok a pointer to
  a bool which will be (if not 0!) set to TRUE if the user pressed ok or to FALSE if the
  user pressed cancel, \a parent the parent widget of the dialog and \a name
  the name of it. The dialogs pops up modally!

  This method returns the text of the current item, or if \a editable was TRUE, the current
  text of the combobox.

  You will use this static method like this:

  \code
  QStringList lst;
  lst << "First" << "Second" << "Third" << "Fourth" << "Fifth";
  bool ok = FALSE;
  QString res = QInputDialog::getItem( tr( "Please select an item" ), lst, 1, TRUE, &ok, this );
  if ( ok )
      ;// user selected an item and pressed ok
  else
      ;// user pressed cancel
  \endcode
*/

QString QInputDialog::getItem( const QString &caption, const QString &label, const QStringList &list,
			       int current, bool editable,
			       bool *ok, QWidget *parent, const char *name )
{
    QInputDialog *dlg = new QInputDialog( label, parent, name, TRUE, editable ? EditableComboBox : ComboBox );
    dlg->setCaption( caption );
    if ( editable ) {
	dlg->editableComboBox()->insertStringList( list );
	dlg->editableComboBox()->setCurrentItem( current );
    } else {
	dlg->comboBox()->insertStringList( list );
	dlg->comboBox()->setCurrentItem( current );
    }

    bool ok_ = FALSE;
    QString result;
    ok_ = dlg->exec() == QDialog::Accepted;
    if ( ok )
	*ok = ok_;
    if ( editable )
	result = dlg->editableComboBox()->currentText();
    else
	result = dlg->comboBox()->currentText();

    delete dlg;
    return result;
}

/*!
  \internal
*/

void QInputDialog::textChanged( const QString &s )
{
    d->ok->setEnabled( !s.isEmpty() );
}

/*!
  \internal
*/

void QInputDialog::tryAccept()
{
    if ( !d->lineEdit->text().isEmpty() )
	accept();
}

#endif
