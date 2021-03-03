/****************************************************************
**
** Implementation CannonField class, Qt tutorial 9
**
****************************************************************/

#include "cannon.h"
#include <qpainter.h>


CannonField::CannonField( QWidget *parent, const char *name )
        : QWidget( parent, name )
{
    ang = 45;
    setPalette( QPalette( QColor( 250, 250, 200) ) );
}


void CannonField::setAngle( int degrees )
{
    if ( degrees < 5 )
	degrees = 5;
    if ( degrees > 70 )
	degrees = 70;
    if ( ang == degrees )
	return;
    ang = degrees;
    repaint();
    emit angleChanged( ang );
}


void CannonField::paintEvent( QPaintEvent * )
{
    QPainter p( this );

    p.setBrush( blue );
    p.setPen( NoPen );

    p.translate( 0, rect().bottom() );
    p.drawPie( QRect(-35, -35, 70, 70), 0, 90*16 );
    p.rotate( -ang );
    p.drawRect( QRect(33, -4, 15, 8) );
}


QSizePolicy CannonField::sizePolicy() const
{
    return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}
