/****************************************************************************
** $Id: qt/examples/rangecontrols/rangecontrols.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "rangecontrols.h"

#include <qhbox.h>
#include <qlcdnumber.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qstring.h>
#include <qslider.h>
#include <qcheckbox.h>

#include <limits.h>

RangeControls::RangeControls( QWidget *parent, const char *name )
    : QVBox( parent, name )
{
    QHBox *row1 = new QHBox( this );

    QVBox *cell2 = new QVBox( row1 );
    cell2->setMargin( 10 );
    cell2->setFrameStyle( QFrame::WinPanel | QFrame::Sunken );

    (void)new QWidget( cell2 );

    QLabel *label1 = new QLabel( QString( "Enter a value between\n%1 and %2:" ).arg( -INT_MAX ).arg( INT_MAX ), cell2 );
    label1->setMaximumHeight( label1->sizeHint().height() );
    QSpinBox *sb1 = new QSpinBox( -INT_MAX, INT_MAX, 1, cell2 );
    sb1->setValue( 0 );

    QLabel *label2 = new QLabel( "Enter a zoom value:", cell2 );
    label2->setMaximumHeight( label2->sizeHint().height() );
    QSpinBox *sb2 = new QSpinBox( 0, 1000, 10, cell2 );
    sb2->setSuffix( " %" );
    sb2->setSpecialValueText( "Automatic" );

    QLabel *label3 = new QLabel( "Enter a price:", cell2 );
    label3->setMaximumHeight( label3->sizeHint().height() );
    QSpinBox *sb3 = new QSpinBox( 0, INT_MAX, 1, cell2 );
    sb3->setPrefix( "$" );
    sb3->setValue( 355 );

    (void)new QWidget( cell2 );

    QHBox *row2 = new QHBox( this );

    QVBox *cell3 = new QVBox( row2 );
    cell3->setMargin( 10 );
    cell3->setFrameStyle( QFrame::WinPanel | QFrame::Sunken );
    QSlider *hslider = new QSlider( 0, 64, 1, 33, Qt::Horizontal, cell3 );
    QLCDNumber *lcd2 = new QLCDNumber( 2, cell3 );
    lcd2->display( 33 );
    lcd2->setSegmentStyle( QLCDNumber::Filled );
    connect( hslider, SIGNAL( valueChanged( int ) ), lcd2, SLOT( display( int ) ) );

    QHBox *cell4 = new QHBox( row2 );
    cell4->setFrameStyle( QFrame::WinPanel | QFrame::Sunken );
    cell4->setMargin( 10 );
    QSlider *vslider = new QSlider( 0, 64, 1, 8, Qt::Vertical, cell4 );
    QLCDNumber *lcd3 = new QLCDNumber( 3, cell4 );
    lcd3->display( 8 );
    connect( vslider, SIGNAL( valueChanged( int ) ), lcd3, SLOT( display( int ) ) );
}
