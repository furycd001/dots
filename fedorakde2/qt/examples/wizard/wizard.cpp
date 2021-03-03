/****************************************************************************
** $Id: qt/examples/wizard/wizard.cpp   2.3.2   edited 2001-08-18 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "wizard.h"

#include <qwidget.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qvalidator.h>
#include <qapplication.h>

Wizard::Wizard( QWidget *parent, const char *name )
    : QWizard( parent, name, TRUE )
{
    setupPage1();
    setupPage2();
    setupPage3();

    key->setFocus();
}

void Wizard::setupPage1()
{
    page1 = new QHBox( this );
    page1->setSpacing(8);

    QLabel *info = new QLabel( page1 );
    info->setPalette( yellow );
    info->setText( "Enter your personal\n"
                   "key here.\n\n"
                   "Your personal key\n"
                   "consists of 4 digits" );
    info->setIndent( 8 );
    info->setMaximumWidth( info->sizeHint().width() );

    QVBox *page = new QVBox( page1 );

    QHBox *row1 = new QHBox( page );

    (void)new QLabel( "Key:", row1 );

    key = new QLineEdit( row1 );
    key->setMaxLength( 4 );
    key->setValidator( new QIntValidator( 9999, 0, key ) );

    connect( key, SIGNAL( textChanged( const QString & ) ), this, SLOT( keyChanged( const QString & ) ) );

    addPage( page1, "Personal Key" );

    setNextEnabled( page1, FALSE );
    setHelpEnabled( page1, FALSE );
}

void Wizard::setupPage2()
{
    page2 = new QHBox( this );
    page2->setSpacing(8);

    QLabel *info = new QLabel( page2 );
    info->setPalette( yellow );
    info->setText( "\n"
                   "  Enter your personal  \n"
                   "  data here.  \n\n"
                   "  The required fields are  \n"
                   "  First Name, Last Name \n"
                   "  and E-Mail.  \n" );
    info->setIndent(8);
    info->setMaximumWidth( info->sizeHint().width() );

    QVBox *page = new QVBox( page2 );

    QHBox *row1 = new QHBox( page );
    QHBox *row2 = new QHBox( page );
    QHBox *row3 = new QHBox( page );
    QHBox *row4 = new QHBox( page );
    QHBox *row5 = new QHBox( page );

    QLabel *label1 = new QLabel( " First Name: ", row1 );
    label1->setAlignment( Qt::AlignVCenter );
    QLabel *label2 = new QLabel( " Last Name: ", row2 );
    label2->setAlignment( Qt::AlignVCenter );
    QLabel *label3 = new QLabel( " Address: ", row3 );
    label3->setAlignment( Qt::AlignVCenter );
    QLabel *label4 = new QLabel( " Phone Number: ", row4 );
    label4->setAlignment( Qt::AlignVCenter );
    QLabel *label5 = new QLabel( " E-Mail: ", row5 );
    label5->setAlignment( Qt::AlignVCenter );

    label1->setMinimumWidth( label4->sizeHint().width() );
    label2->setMinimumWidth( label4->sizeHint().width() );
    label3->setMinimumWidth( label4->sizeHint().width() );
    label4->setMinimumWidth( label4->sizeHint().width() );
    label5->setMinimumWidth( label4->sizeHint().width() );

    firstName = new QLineEdit( row1 );
    lastName = new QLineEdit( row2 );
    address = new QLineEdit( row3 );
    phone = new QLineEdit( row4 );
    email = new QLineEdit( row5 );

    connect( firstName, SIGNAL( textChanged( const QString & ) ), this, SLOT( dataChanged( const QString & ) ) );
    connect( lastName, SIGNAL( textChanged( const QString & ) ), this, SLOT( dataChanged( const QString & ) ) );
    connect( email, SIGNAL( textChanged( const QString & ) ), this, SLOT( dataChanged( const QString & ) ) );

    addPage( page2, "Personal Data" );

    setHelpEnabled( page2, FALSE );
}

void Wizard::setupPage3()
{
    page3 = new QHBox( this );
    page3->setSpacing(8);

    QLabel *info = new QLabel( page3 );
    info->setPalette( yellow );
    info->setText( "\n"
                   "  Look here to see of  \n"
                   "  the data you entered  \n"
                   "  is correct. To confirm,  \n"
                   "  press the [Finish] button  \n"
                   "  else go back to correct  \n"
                   "  mistakes." );
    info->setIndent(8);
    info->setAlignment( AlignTop|AlignLeft );
    info->setMaximumWidth( info->sizeHint().width() );

    QVBox *page = new QVBox( page3 );

    QHBox *row1 = new QHBox( page );
    QHBox *row2 = new QHBox( page );
    QHBox *row3 = new QHBox( page );
    QHBox *row4 = new QHBox( page );
    QHBox *row5 = new QHBox( page );
    QHBox *row6 = new QHBox( page );

    QLabel *label1 = new QLabel( " Personal Key: ", row1 );
    label1->setAlignment( Qt::AlignVCenter );
    QLabel *label2 = new QLabel( " First Name: ", row2 );
    label2->setAlignment( Qt::AlignVCenter );
    QLabel *label3 = new QLabel( " Last Name: ", row3 );
    label3->setAlignment( Qt::AlignVCenter );
    QLabel *label4 = new QLabel( " Address: ", row4 );
    label4->setAlignment( Qt::AlignVCenter );
    QLabel *label5 = new QLabel( " Phone Number: ", row5 );
    label5->setAlignment( Qt::AlignVCenter );
    QLabel *label6 = new QLabel( " E-Mail: ", row6 );
    label6->setAlignment( Qt::AlignVCenter );

    label1->setMinimumWidth( label1->sizeHint().width() );
    label2->setMinimumWidth( label1->sizeHint().width() );
    label3->setMinimumWidth( label1->sizeHint().width() );
    label4->setMinimumWidth( label1->sizeHint().width() );
    label5->setMinimumWidth( label1->sizeHint().width() );
    label6->setMinimumWidth( label1->sizeHint().width() );

    lKey = new QLabel( row1 );
    lFirstName = new QLabel( row2 );
    lLastName = new QLabel( row3 );
    lAddress = new QLabel( row4 );
    lPhone = new QLabel( row5 );
    lEmail = new QLabel( row6 );

    addPage( page3, "Finish" );

    setFinish( page3, TRUE );
    setHelpEnabled( page3, FALSE );
}

void Wizard::showPage( QWidget* page )
{
    if ( page == page1 ) {
    } else if ( page == page2 ) {
    } else if ( page == page3 ) {
        lKey->setText( key->text() );
        lFirstName->setText( firstName->text() );
        lLastName->setText( lastName->text() );
        lAddress->setText( address->text() );
        lPhone->setText( phone->text() );
        lEmail->setText( email->text() );
    }

    QWizard::showPage(page);

    if ( page == page1 ) {
        keyChanged( key->text() );
        key->setFocus();
    } else if ( page == page2 ) {
        dataChanged( firstName->text() );
        firstName->setFocus();
    } else if ( page == page3 ) {
        finishButton()->setEnabled( TRUE );
        finishButton()->setFocus();
    }
}

void Wizard::keyChanged( const QString &text )
{
    if ( text.length() == 4 )
        nextButton()->setEnabled( TRUE );
    else
        nextButton()->setEnabled( FALSE );
}

void Wizard::dataChanged( const QString & )
{
    if ( !firstName->text().isEmpty() &&
         !lastName->text().isEmpty() &&
         !email->text().isEmpty() )
        nextButton()->setEnabled( TRUE );
    else
        nextButton()->setEnabled( FALSE );
}
