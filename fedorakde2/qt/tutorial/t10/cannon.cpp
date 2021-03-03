/****************************************************************
**
** Implementation CannonField class, Qt tutorial 10
**
****************************************************************/

#include "cannon.h"
#include <qpainter.h>
#include <qpixmap.h>


CannonField::CannonField( QWidget *parent, const char *name )
        : QWidget( parent, name )
{
    ang = 45;
    f = 0;
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
    repaint( cannonRect(), FALSE );
    emit angleChanged( ang );
}


void CannonField::setForce( int newton )
{
    if ( newton < 0 )
	newton = 0;
    if ( f == newton )
	return;
    f = newton;
    emit forceChanged( f );
}


void CannonField::paintEvent( QPaintEvent *e )
{
    if ( !e->rect().intersects( cannonRect() ) )
	return;

    QRect cr = cannonRect();
    QPixmap pix( cr.size() );
    pix.fill( this, cr.topLeft() );

    QPainter p( &pix );
    p.setBrush( blue );
    p.setPen( NoPen );
    p.translate( 0, pix.height() - 1 );
    p.drawPie( QRect( -35,-35, 70, 70 ), 0, 90*16 );
    p.rotate( -ang );
    p.drawRect( QRect(33, -4, 15, 8) );
    p.end();

    p.begin( this );
    p.drawPixmap( cr.topLeft(), pix );
}


QRect CannonField::cannonRect() const
{
    QRect r( 0, 0, 50, 50 );
    r.moveBottomLeft( rect().bottomLeft() );
    return r;
}


QSizePolicy CannonField::sizePolicy() const
{
    return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}
