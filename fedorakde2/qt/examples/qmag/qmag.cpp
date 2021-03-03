/****************************************************************************
** $Id: qt/examples/qmag/qmag.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qcombobox.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qlabel.h>
#include <qfiledialog.h>
#include <qregexp.h>

#include <qapplication.h>
#include <qpainter.h>
#include <qwmatrix.h>


class MagWidget : public QWidget
{
    Q_OBJECT
public:
    MagWidget( QWidget *parent=0, const char *name=0 );

public slots:
    void	setZoom( int );
    void	setRefresh( int );
    void	save();
    void	multiSave();

protected:
    void	paintEvent( QPaintEvent * );
    void	mousePressEvent( QMouseEvent * );
    void	mouseReleaseEvent( QMouseEvent * );
    void	mouseMoveEvent( QMouseEvent * );
    void	focusOutEvent( QFocusEvent * );
    void	timerEvent( QTimerEvent * );
    void	resizeEvent( QResizeEvent * );

private:
    void	grabAround(QPoint pos);
    void	grab();

    QComboBox   *zoom;
    QComboBox   *refresh;
    QPushButton *saveButton;
    QPushButton *multiSaveButton;
    QPushButton *quitButton;
    QPixmap	pm;		// pixmap, magnified
    QPixmap	p;		// pixmap
    QImage	image;		// image of pixmap (for RGB)
    QLabel      *rgb;
    int		yoffset;	// pixels in addition to the actual picture
    int		z;		// magnification factor
    int		r;		// autorefresh rate (index into refreshrates)
    bool	grabbing;	// TRUE if qmag is currently grabbing
    int		grabx, graby;
    QString	multifn;	// filename for multisave
};


#ifdef COMPLEX_GUI
static const char *zoomfactors[] = {
    "100%", "200%", "300%", "400%", "500%",
    "600%", "700%", "800%", "1600%", 0 };

static const char *refreshrates[] = {
    "No autorefresh", "50 per second", "4 per second", "3 per second", "2 per second",
    "Every second", "Every two seconds", "Every three seconds",
    "Every five seconds", "Every ten seconds", 0 };
#endif

static const int timer[] = {
    0, 20, 250, 333, 500, 1000, 2000, 3000, 5000, 10000 };


MagWidget::MagWidget( QWidget *parent, const char *name )
    : QWidget( parent, name)
{
    z = 1;			// default zoom (100%)
    r = 0;			// default refresh (none)

#ifdef COMPLEX_GUI
    int w=0, x=0, n;

    zoom = new QComboBox( FALSE, this );
    CHECK_PTR(zoom);
    zoom->insertStrList( zoomfactors, 9 );
    connect( zoom, SIGNAL(activated(int)), SLOT(setZoom(int)) );

    refresh = new QComboBox( FALSE, this );
    CHECK_PTR(refresh);
    refresh->insertStrList( refreshrates, 9 );
    connect( refresh, SIGNAL(activated(int)), SLOT(setRefresh(int)) );

    for( n=0; n<9; n++) {
	int w2 = zoom->fontMetrics().width( zoomfactors[n] );
	w = QMAX(w2, w);
    }
    zoom->setGeometry( 2, 2, w+30, 20 );

    x = w+34;
    w = 0;
    for( n=0; n<9; n++) {
	int w2 = refresh->fontMetrics().width( refreshrates[n] );
	w = QMAX(w2, w);
    }
    refresh->setGeometry( x, 2, w+30, 20 );

    saveButton = new QPushButton( this );
    CHECK_PTR(saveButton);
    connect( saveButton, SIGNAL(clicked()), this, SLOT(save()) );
    saveButton->setText( "Save" );
    saveButton->setGeometry( x+w+30+2, 2,
			     10+saveButton->fontMetrics().width("Save"), 20 );

    multiSaveButton = new QPushButton( this );
    multiSaveButton->setToggleButton(TRUE);
    CHECK_PTR(multiSaveButton);
    connect( multiSaveButton, SIGNAL(clicked()), this, SLOT(multiSave()) );
    multiSaveButton->setText( "MultiSave" );
    multiSaveButton->setGeometry( saveButton->geometry().right() + 2, 2,
			     10+multiSaveButton->fontMetrics().width("MultiSave"), 20 );

    quitButton = new QPushButton( this );
    CHECK_PTR(quitButton);
    connect( quitButton, SIGNAL(clicked()), qApp, SLOT(quit()) );
    quitButton->setText( "Quit" );
    quitButton->setGeometry( multiSaveButton->geometry().right() + 2, 2,
			     10+quitButton->fontMetrics().width("Quit"), 20 );
#else
    zoom = 0;
    multiSaveButton = 0;
#endif

    setRefresh(1);
    setZoom(5);

    rgb = new QLabel( this );
    CHECK_PTR( rgb );
    rgb->setText( "" );
    rgb->setAlignment( AlignVCenter );
    rgb->resize( width(), rgb->fontMetrics().height() + 4 );

#ifdef COMPLEX_GUI
    yoffset = zoom->height()	// top buttons
	+ 4			// space around top buttons
	+ rgb->height();	// color-value text height
    setMinimumSize( quitButton->pos().x(), yoffset+20 );
    resize( quitButton->geometry().topRight().x() + 2, yoffset+60 );
#else
    yoffset = 0;
    resize(350,350);
#endif

    grabx = graby = -1;
    grabbing = FALSE;

    setMouseTracking( TRUE );	// and do let me know what pixel I'm at, eh?

    grabAround( QPoint(grabx=qApp->desktop()->width()/2, graby=qApp->desktop()->height()/2) );
}


void MagWidget::setZoom( int index )
{
    if (index == 8)
	z = 16;
    else
	z = index+1;
    grab();
}


void MagWidget::setRefresh( int index )
{
    r = index;
    killTimers();
    if (index && !grabbing)
	startTimer( timer[r] );
}


void MagWidget::save()
{
    if ( !p.isNull() ) {
	killTimers();
	QString fn = QFileDialog::getSaveFileName();
	if ( !fn.isEmpty() )
	    p.save( fn, "BMP" );
	if ( r )
	    startTimer( timer[r] );
    }
}

void MagWidget::multiSave()
{
    if ( !p.isNull() ) {
	multifn = ""; // stops saving
	multifn = QFileDialog::getSaveFileName();
	if ( multifn.isEmpty() )
	    multiSaveButton->setOn(FALSE);
	if ( !r )
	    p.save( multifn, "BMP" );
    } else {
	multiSaveButton->setOn(FALSE);
    }
}


void MagWidget::grab()
{
    if ( !isVisible() ) 
	return;			// don't eat resources when iconified

    if ( grabx < 0 || graby < 0 )
	return;			// don't grab until the user has said to

    int x,y, w,h;

    w = (width()+z-1)/z;
    h = (height()+z-1-yoffset)/z;
    if ( w<1 || h<1 )
	return;			// don't ask too much from the window system :)

    x = grabx-w/2;		// find a suitable position to grab from
    y = graby-h/2;
    if ( x + w > QApplication::desktop()->width() )
	x = QApplication::desktop()->width()-w;
    else if ( x < 0 )
	x = 0;
    if ( y + h > QApplication::desktop()->height() ) 
	y = QApplication::desktop()->height()-h;
    else if ( y < 0 )
	y = 0;

    p = QPixmap::grabWindow( QApplication::desktop()->winId(),  x, y, w, h );
    image = p.convertToImage();
    QWMatrix m;			// after getting it, scale it
    m.scale( (double)z, (double)z );
    pm = p.xForm( m );

    if ( !multiSaveButton || !multiSaveButton->isOn() )
	repaint( FALSE );		// and finally repaint, flicker-free
}


void MagWidget::paintEvent( QPaintEvent * )
{
    if ( !pm.isNull() ) {
	QPainter paint( this );
	paint.drawPixmap( 0, zoom ? zoom->height()+4 : 0, pm, 
			      0,0, width(), height()-yoffset );
    }
}


void MagWidget::mousePressEvent( QMouseEvent *e )
{
    if ( !grabbing ) {		// prepare to grab...
	grabbing = TRUE;
	killTimers();
	grabMouse( crossCursor );
	grabx = -1;
	graby = -1;
    } else {			// REALLY prepare to grab
	grabx = mapToGlobal(e->pos()).x();
	graby = mapToGlobal(e->pos()).y();
    }
}



void MagWidget::mouseReleaseEvent( QMouseEvent * e )
{
    if ( grabbing && grabx >= 0 && graby >= 0 ) {
	grabbing = FALSE;
	grabAround(e->pos());
	releaseMouse();
    }
}

void MagWidget::grabAround(QPoint pos)
{
    int rx, ry;
    rx = mapToGlobal(pos).x();
    ry = mapToGlobal(pos).y();
    int w = QABS(rx-grabx);
    int h = QABS(ry-graby);
    if ( w > 10 && h > 10 ) {
	int pz;
	pz = 1;
	while ( w*pz*h*pz < width()*(height()-yoffset) &&
		w*pz < QApplication::desktop()->width() &&
		h*pz < QApplication::desktop()->height() )
	    pz++;
	if ( (w*pz*h*pz - width()*(height()-yoffset)) > 
	     (width()*(height()-yoffset) - w*(pz-1)*h*(pz-1)) )
	    pz--;
	if ( pz < 1 )
	    pz = 1;
	if ( pz > 8 )
	    pz = 8;
	if ( zoom )
	    zoom->setCurrentItem( pz-1 );

	z = pz;
	grabx = QMIN(rx, grabx) + w/2;
	graby = QMIN(ry, graby) + h/2;
	resize( w*z, h*z+yoffset );
    }
    grab();
    if ( r )
	startTimer( timer[r] );
}


void MagWidget::mouseMoveEvent( QMouseEvent *e )
{
    if ( grabbing || pm.isNull() ||
	 e->pos().y() > height() - (zoom ? zoom->fontMetrics().height() - 4 : 0) ||
	 e->pos().y() < (zoom ? zoom->height()+4 : 4) ) {
	rgb->setText( "" );
    } else {
	int x,y;
	x = e->pos().x() / z;
	y = (e->pos().y() - ( zoom ? zoom->height() : 0 ) - 4) / z;
	QString pixelinfo;
	if ( image.valid(x,y) )
	{
	    QRgb px = image.pixel(x,y);
	    pixelinfo.sprintf(" %3d,%3d,%3d  #%02x%02x%02x",
		qRed(px), qGreen(px), qBlue(px),
		qRed(px), qGreen(px), qBlue(px));
	}
	QString label;
	label.sprintf( "x=%d, y=%d %s", 
	    x+grabx, y+graby, (const char*)pixelinfo );
	rgb->setText( label );
    }
}
	

void MagWidget::focusOutEvent( QFocusEvent * )
{
    rgb->setText( "" );
}


void MagWidget::timerEvent( QTimerEvent * )
{
    grab();
/*
    if ( multiSaveButton->isOn() && !multifn.isEmpty() ) {
	QRegExp num("[0-9][0-9]*");
	int start;
	int len;
	if ((start=num.match(multifn,0,&len))>=0)
	    multifn.replace(num,
		QString().setNum(multifn.mid(start,len).toInt()+1)
	    );
	p.save( multifn, "BMP" );
    }
*/
}


void MagWidget::resizeEvent( QResizeEvent * )
{
    rgb->setGeometry( 0, height() - rgb->height(), width(), rgb->height() );
    grab();
}


#include "qmag.moc"


int main( int argc, char **argv )
{
    QApplication a( argc, argv );
    MagWidget m;
    a.setMainWidget( &m );
    m.show();
    return a.exec();
}
