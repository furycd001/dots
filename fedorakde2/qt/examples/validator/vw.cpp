/****************************************************************************
** $Id: qt/examples/validator/vw.cpp   2.3.2   edited 2001-06-13 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "vw.h"

#include <qlineedit.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>

#include "motor.h"

VW::VW( QWidget * parent, const char * name )
    : QWidget( parent, name )
{
    QHBoxLayout * hb;

    hb = new QHBoxLayout( this, 10 );

    QGroupBox * box;
    box = new QGroupBox( this, "input box" );
    hb->addWidget( box, 0, AlignTop );

    QVBoxLayout * b;

    // set up the input box
    b = new QVBoxLayout( box, 12 );

    QLabel * l = new QLabel( "Enter Vehicle Details", box, "header" );
    l->setMinimumSize( l->sizeHint() );
    b->addWidget( l );
    QFrame * f = new QFrame( box, "horizontal divider" );
    f->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    f->setMinimumHeight( 12 );
    b->addWidget( f );

    QGridLayout *grid = new QGridLayout( 3, 2 );
    b->addLayout( grid );

    // here we start on the input grid, with labels and other widget
    // neatly arranged. the variable names are reused all over the
    // place.

    QComboBox * model = new QComboBox( FALSE, box, "model selection" );
    model->insertItem( "Type 1 Beetle" );
    model->insertItem( "Camper" );
    model->insertItem( "Van" );
    model->insertItem( "Fastback" );
    model->insertItem( "Squareback" );
    model->insertItem( "Notchback" );
    model->insertItem( "411" );
    model->setCurrentItem( model->count() - 1 ); // I like the 411
    currentModel = "411";
    model->insertItem( "412" );
    model->insertItem( "Karmann Ghia" );
    model->insertItem( "Thing" );
    model->insertItem( "Safari" );
    model->insertItem( "Kubelwagen" );
    model->insertItem( "Trekker" );
    model->insertItem( "Baja" );
    model->setMinimumSize( model->sizeHint() );
    model->setMaximumHeight( model->minimumSize().height() );
    grid->addWidget( model, 0, 1 );

    l = new QLabel( model, "Model:", box, "model label" );
    l->setMinimumSize( l->sizeHint() );
    grid->addWidget( l, 0, 0 );

    QSpinBox * motor = new QSpinBox( 1000, 1600, 100,
				     box, "motor size selection" );
    motor->setValue( 1000 );
    currentMotorSize = 1000;
    motor->setMinimumSize( motor->sizeHint() );
    motor->setMaximumHeight( motor->minimumSize().height() );
    grid->addWidget( motor, 1, 1 );

    l = new QLabel( motor, "Motor size (cc):", box, "motor size label" );
    l->setMinimumSize( l->sizeHint() );
    grid->addWidget( l, 1, 0 );

    QSpinBox * year = new QSpinBox( box, "model year" );
    year->setRange( 1949, 1981 );
    year->setValue( 1949 );
    currentYear = 1949;
    year->setMinimumSize( year->sizeHint() );
    year->setMaximumHeight( year->minimumSize().height() );
    grid->addWidget( year, 2, 1 );

    l = new QLabel( year, "Year:", box, "model year label" );
    l->setMinimumSize( l->sizeHint() );
    grid->addWidget( l, 2, 0 );

    b->addStretch( 1 );

    b->activate();

    // output box

    box = new QGroupBox( this, "output box" );
    hb->addWidget( box, 0 );

    b = new QVBoxLayout( box, 12 );

    l = new QLabel( "Resulting Limousine:", box, "header" );
    l->setMinimumSize( l->sizeHint() );
    b->addWidget( l );

    f = new QFrame( box, "horizontal divider" );
    f->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    f->setMinimumHeight( 12 );
    b->addWidget( f );

    l = new QLabel( box, "output label" );
    l->setAlignment( AlignTop | AlignLeft | WordBreak );
    l->setText( "No VW selected yet." );
    b->addWidget( l, 1 );

    b->addStretch( 1 );

    b->activate();
    hb->activate();

    // set up connections
    connect( model, SIGNAL(activated(const QString&)),
	     this, SLOT(modelSelected(const QString&)) );
    connect( motor, SIGNAL(valueChanged(int)),
	     this, SLOT(motorSelected(int)) );
    connect( year, SIGNAL(valueChanged(int)),
	     this, SLOT(yearSelected(int)) );

    connect( this, SIGNAL(validSelectionMade(const QString&)),
	     l, SLOT(setText(const QString&)) );
}


VW::~VW()
{
    // nothing needs to be done.
}


void VW::modelSelected( const QString& m )
{
    currentModel = m;
    computeSelection();
}


void VW::motorSelected( int m )
{
    currentMotorSize = m;
    computeSelection();
}


void VW::yearSelected( int y )
{
    currentYear = y;
    computeSelection();
}


void VW::computeSelection()
{
    if ( currentModel.isNull() )
	return; // no model selected yet

    QString s;
    s.sprintf( "You have selected a Volkswagen %s model %d with a "
	       "%d cm³ motor.\n\nGood choice!",
	       (const char *)currentModel,
	       (int)currentYear, currentMotorSize );
    emit validSelectionMade( s );
}
