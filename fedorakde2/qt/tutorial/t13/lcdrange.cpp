/****************************************************************
**
** Implementation of LCDRange class, Qt tutorial 12
**
****************************************************************/

#include "lcdrange.h"

#include <qslider.h>
#include <qlcdnumber.h>
#include <qlabel.h>
#include <qlayout.h>


LCDRange::LCDRange( QWidget *parent, const char *name )
        : QWidget( parent, name )
{
    init();
}


LCDRange::LCDRange( const char *s, QWidget *parent, const char *name )
        : QWidget( parent, name )
{
    init();
    setText( s );
}


void LCDRange::init()
{
    QLCDNumber *lcd  = new QLCDNumber( 2, this, "lcd"  );
    slider = new QSlider( Horizontal, this, "slider" );
    slider->setRange( 0, 99 );
    slider->setValue( 0 );

    label = new QLabel( " ", this, "label"  );
    label->setAlignment( AlignCenter );

    connect( slider, SIGNAL(valueChanged(int)),
	     lcd, SLOT(display(int)) );
    connect( slider, SIGNAL(valueChanged(int)),
	     SIGNAL(valueChanged(int)) );

    setFocusProxy( slider );

    QVBoxLayout * l = new QVBoxLayout( this );
    l->addWidget( lcd, 1 );
    l->addWidget( slider );
    l->addWidget( label );
}


int LCDRange::value() const
{
    return slider->value();
}


const char *LCDRange::text() const
{
    return label->text();
}


void LCDRange::setValue( int value )
{
    slider->setValue( value );
}


void LCDRange::setRange( int minVal, int maxVal )
{
    if ( minVal < 0 || maxVal > 99 || minVal > maxVal ) {
	qWarning( "LCDRange::setRange(%d,%d)\n"
		  "\tRange must be 0..99\n"
		  "\tand minVal must not be greater than maxVal",
		  minVal, maxVal );
	return;
    }
    slider->setRange( minVal, maxVal );
}


void LCDRange::setText( const char *s )
{
    label->setText( s );
}
