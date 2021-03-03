/****************************************************************************
** $Id: qt/examples/desktop/desktop.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qimage.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <qapplication.h>
#include <qdropsite.h>
#include <qdragobject.h>
#include <stdio.h>


static double seed = 0.353535353535;
static const int KINDA_RAND_MAX = 32767;

static int kindaRand()
{
    seed = seed*147;
    seed = seed - (double) ((int) seed);
    return (int) ( seed*(KINDA_RAND_MAX + 1) );
}

static int velocity( int i )			// change velocity
{
    const int velmax = 15;
    const int velmin = 4;
    if ( i == 1 || i == 2 )
	i = (kindaRand()&0x7fff % velmax)/3 + velmin;
    else
	i = (kindaRand()&0x7fff % velmax) + velmin;
    return i;
}

//
// Draw polygon on desktop.
//

void poly()
{
    QWidget *d = QApplication::desktop();
    d->setBackgroundColor( Qt::white );		// white desktop

    const int maxpoints = 5;
    const int maxcurves = 8;
    static int xvel[maxpoints];
    static int yvel[maxpoints];
    int head = 0;
    int tail = -maxcurves + 2;
    QPointArray *a = new QPointArray[ maxcurves ];
    register QPointArray *p;
    QRect r = d->rect();			// desktop rectangle

    int i;
    for ( i=0; i<maxcurves; i++ )
	a[i].resize( maxpoints );
    p = &a[0];
    for ( i=0; i<maxpoints; i++ ) {		// setup first polygon points
	p->setPoint( i, (kindaRand()&0x7fff) % r.width(),
			(kindaRand()&0x7fff) % r.height() );
	xvel[i] = velocity(i);
	yvel[i] = velocity(i);
    }

    QPainter paint;
    paint.begin( d );				// start painting desktop

    for ( int ntimes=0; ntimes<2000; ntimes++ ) {
	paint.setBrush( QColor(kindaRand()%360, 180, 255, QColor::Hsv) );
	paint.drawPolygon( a[head] );
	if ( ++tail >= maxcurves )
	    tail = 0;

	int minx=r.left(), maxx=r.right();
	int miny=r.top(),  maxy=r.bottom();
	int x, y;
	p = &a[head];
	if ( ++head >= maxcurves )
	    head = 0;
	for ( i=0; i<maxpoints; i++ ) {		// calc new curve
	    p->point( i, &x, &y );
	    x += xvel[i];
	    y += yvel[i];
	    if ( x >= maxx ) {
		x = maxx - (x - maxx + 1);
		xvel[i] = -velocity(i);
	    }
	    if ( x <= minx ) {
		x = minx + (minx - x + 1);
		xvel[i] = velocity(i);
	    }
	    if ( y >= maxy ) {
		y = maxy - (y - maxy + 1);
		yvel[i] = -velocity(i);
	    }
	    if ( y <= miny ) {
		y = miny + (miny - y + 1);
		yvel[i] = velocity(i);
	    }
	    a[head].setPoint( i, x, y );
	}
    }
    paint.end();				// painting done
    delete[] a;
}


//
// Rotate pattern on desktop.
//

void rotate()
{
    int i;
    const int w = 64;
    const int h = 64;
    QImage image( w, h, 8, 128 );		// create image
    for ( i=0; i<128; i++ )			// build color table
	image.setColor( i, qRgb(i,0,0) );
    for ( int y=0; y<h; y++ ) {			// set image pixels
	uchar *p = image.scanLine(y);
	for ( int x=0; x<w; x++ )
	    *p++ = (x+y)%128;
    }

    QPixmap pm;
    pm = image;					// convert image to pixmap
    pm.setOptimization( QPixmap::BestOptim );	// rotation will be faster

    QWidget *d = QApplication::desktop();	// w = desktop widget

    for ( i=0; i<=360; i += 2 ) {
	QWMatrix m;
	m.rotate( i );				// rotate coordinate system
	QPixmap rpm = pm.xForm( m );		// rpm = rotated pixmap
	d->setBackgroundPixmap( rpm );		// set desktop pixmap
	d->update();				// repaint desktop
    }
}

//
// Generates a marble-like pattern in pm.
//

void generateStone( QPixmap *pm,
		    const QColor &c1, const QColor &c2, const QColor &c3 )
{
    QPainter p;
    QPen p1 ( c1, 0 );
    QPen p2 ( c2, 0 );
    QPen p3 ( c3, 0 );

    p.begin( pm );
    for( int i = 0 ; i < pm->width() ; i++ )
	for( int j = 0 ; j < pm->height() ; j++ ) {
	    int r = kindaRand();
	    if ( r < KINDA_RAND_MAX / 3 )
		p.setPen( p1 );
	    else if ( r < KINDA_RAND_MAX / 3 * 2 )
		p.setPen( p2 );
	    else
		p.setPen( p3 );
	    p.drawPoint( i,j );
	}
    p.end();
}

void drawShadeText( QPainter *p, int x, int y, const char *text,
		    const QColor &topColor, const QColor &bottomColor,
		    int sw = 2 )
{
    if ( !p->isActive() )
	return;

    p->setPen( bottomColor );
    p->drawText( x+sw, y+sw, text );
    p->setPen( topColor );
    p->drawText( x, y, text );
}

// NOTE: desktop drag/drop is experimental

class DesktopWidget : public QWidget, private QDropSite
{
public:
    DesktopWidget( const char *s, QWidget *parent=0, const char *name=0 );
   ~DesktopWidget();
    void paintEvent( QPaintEvent * );

    void dragEnterEvent( QDragEnterEvent *e )
    {
	if ( QImageDrag::canDecode(e) )
	    e->accept();
    }

    void dragLeaveEvent( QDragLeaveEvent * )
    {
    }

    void dragMoveEvent( QDragMoveEvent *e )
    {
	e->accept();
    }

    void dropEvent( QDropEvent * e )
    {
	QPixmap pmp;
	if ( QImageDrag::decode( e, pmp ) ) {
	    setBackgroundPixmap( pmp );
	    update();
	}
    }

private:
    QPixmap *pm;
    QString text;
};

DesktopWidget::DesktopWidget( const char *s, QWidget *parent, const char *name )
    : QWidget( parent, name, WType_Desktop | WPaintDesktop),
	QDropSite(this)
{
    text = s;
    pm	 = 0;
}

DesktopWidget::~DesktopWidget()
{
    delete pm;
}

void DesktopWidget::paintEvent( QPaintEvent * )
{
    QColor c1 = backgroundColor();
    QColor c2 = c1.light(104);
    QColor c3 = c1.dark(106);
    if ( !pm ) {
	pm = new QPixmap( 64, 64 );
	generateStone( pm, c1, c2, c3 );
	setBackgroundPixmap( *pm );
	update();
    }
    QRect br = fontMetrics().boundingRect( text );
    QPixmap offscreen( br.width(), br.height() );
    int x = width()/2  - br.width()/2;
    int y = height()/2 - br.height()/2;
    offscreen.fill( this, x, y );
    QPainter p;
    p.begin( &offscreen );
    drawShadeText( &p, -br.x(), -br.y(), text, c2, c3, 3 );
    p.end();
    bitBlt( this, x, y, &offscreen );
}

void desktopWidget( const char *s = "Trolltech" )
{
    DesktopWidget *t = new DesktopWidget(s);
    t->update();
    qApp->exec();
    delete t;
}

void desktopText( const char *s = "Trolltech" )
{
    const int border = 20;

    QColor c1 =	 qApp->palette().normal().background();
    QColor c2 = c1.light(104);
    QColor c3 = c1.dark(106);

    QPixmap pm(10,10);

    QPainter p;
    p.begin( &pm );
    QRect r = p.fontMetrics().boundingRect( s );
    p.end();

    int appWidth  =  qApp->desktop()->width();
    int appHeight =  qApp->desktop()->height();
    if ( r.width() > appWidth - border*2 )
	r.setWidth( appWidth - border*2 );
    if ( r.height() > appHeight - border*2 )
	r.setHeight( appHeight - border*2 );

    pm.resize( r.size() + QSize( border*2, border*2 ) );
    generateStone( &pm, c1, c2, c3 );
    p.begin( &pm );
    drawShadeText( &p, -r.x() + border, -r.y() + border, s, c2, c3 );
    p.end();

    qApp->desktop()->setBackgroundPixmap( pm );
}

//
// The program starts here.
//

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

    if ( argc > 1 ) {
	QFont f( "charter", 96, QFont::Black );
	f.setStyleHint( QFont::Times );
	app.setFont( f );
    }

    bool validOptions = FALSE;

    if ( argc == 2 ) {
	validOptions = TRUE;
	if ( strcmp(argv[1],"-poly") == 0 )
	    poly();
	else if ( strcmp(argv[1],"-rotate") == 0 )
	    rotate();
	else if ( strcmp(argv[1],"-troll") == 0 )
	    desktopText();
	else if ( strcmp(argv[1],"-trollwidget") == 0 )
	    desktopWidget();
	else
	    validOptions = FALSE;
    }
    if ( argc == 3 ) {
	validOptions = TRUE;
	if ( strcmp(argv[1],"-shadetext") == 0 )
	    desktopText( argv[2] );
	else if ( strcmp(argv[1],"-shadewidget") == 0 )
	    desktopWidget( argv[2] );
	else
	    validOptions = FALSE;
    }
    if ( !validOptions ) {
	fprintf( stderr, "Usage:\n\tdesktop -poly"
			       "\n\tdesktop -rotate"
			       "\n\tdesktop -troll"
			       "\n\tdesktop -trollwidget"
			       "\n\tdesktop -shadetext <text>"
			       "\n\tdesktop -shadewidget <text>\n" );
	rotate();
    }
    return 0;
}
