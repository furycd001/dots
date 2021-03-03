/****************************************************************************
** $Id: qt/examples/progress/progress.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qprogressdialog.h>
#include <qapplication.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qpainter.h>
#include <stdlib.h>

class AnimatedThingy : public QLabel {
public:
    AnimatedThingy( QWidget* parent, const QString& s ) :
	QLabel(parent),
	label(s),
	step(0)
    {
	label+="\n... and wasting CPU\nwith this animation!\n";

	for (int i=0; i<nqix; i++)
	    ox[0][i] = oy[0][i] = ox[1][i] = oy[1][i] = 0;
	x0 = y0 = x1 = y1 = 0;
	dx0 = rand()%8+2;
	dy0 = rand()%8+2;
	dx1 = rand()%8+2;
	dy1 = rand()%8+2;
    }

    void show()
    {
	if (!isVisible()) startTimer(100);
	QWidget::show();
    }

    void hide()
    {
	QWidget::hide();
	killTimers();
    }

    QSize sizeHint() const
    {
	return QSize(120,100);
    }

protected:
    void timerEvent(QTimerEvent*)
    {
	QPainter p(this);
	QPen pn=p.pen();
	pn.setWidth(2);
	pn.setColor(backgroundColor());
	p.setPen(pn);

	step = (step + 1) % nqix;

	p.drawLine(ox[0][step], oy[0][step], ox[1][step], oy[1][step]);

	inc(x0, dx0, width());
	inc(y0, dy0, height());
	inc(x1, dx1, width());
	inc(y1, dy1, height());
	ox[0][step] = x0;
	oy[0][step] = y0;
	ox[1][step] = x1;
	oy[1][step] = y1;

	QColor c;
	c.setHsv( (step*255)/nqix, 255, 255 ); // rainbow effect
	pn.setColor(c);
	p.setPen(pn);
	p.drawLine(ox[0][step], oy[0][step], ox[1][step], oy[1][step]);
	p.setPen(colorGroup().text());
	p.drawText(rect(), AlignCenter, label);
    }

    void paintEvent(QPaintEvent* event)
    {
	QPainter p(this);
	QPen pn=p.pen();
	pn.setWidth(2);
	p.setPen(pn);
	p.setClipRect(event->rect());
	for (int i=0; i<nqix; i++) {
	    QColor c;
	    c.setHsv( (i*255)/nqix, 255, 255 ); // rainbow effect
	    pn.setColor(c);
	    p.setPen(pn);
	    p.drawLine(ox[0][i], oy[0][i], ox[1][i], oy[1][i]);
	}
	p.setPen(colorGroup().text());
	p.drawText(rect(), AlignCenter, label);
    }

private:
    void inc(int& x, int& dx, int b)
    {
	x+=dx;
	if (x<0) { x=0; dx=rand()%8+2; }
	else if (x>=b) { x=b-1; dx=-(rand()%8+2); }
    }

    enum {nqix=10};
    int ox[2][nqix];
    int oy[2][nqix];
    int x0,y0,x1,y1;
    int dx0,dy0,dx1,dy1;
    QString label;
    int step;
};


class CPUWaster : public QWidget
{
    Q_OBJECT

    enum { first_draw_item = 1000, last_draw_item = 1006 };

    int drawItemRects(int id)
    {
	int n = id - first_draw_item;
	int r = 100;
	while (n--) r*=(n%3 ? 5 : 4);
	return r;
    }
    QString drawItemText(int id)
    {
	QString str;
	str.sprintf("%d Rectangles", drawItemRects(id));
	return str;
    }

public:
    CPUWaster() :
	pb(0)
    {
	menubar = new QMenuBar( this, "menu" );
	CHECK_PTR( menubar );

	QPopupMenu* file = new QPopupMenu();
	CHECK_PTR( file );
	menubar->insertItem( "&File", file );
	for (int i=first_draw_item; i<=last_draw_item; i++)
	    file->insertItem( drawItemText(i), i );
	connect( menubar, SIGNAL(activated(int)), this, SLOT(doMenuItem(int)) );
	file->insertSeparator();
	file->insertItem( "Quit", qApp,  SLOT(quit()) );

	options = new QPopupMenu();
	CHECK_PTR( options );
	menubar->insertItem( "&Options", options );
	td_id = options->insertItem( "Timer driven", this, SLOT(timerDriven()) );
	ld_id = options->insertItem( "Loop driven", this, SLOT(loopDriven()) );
	options->insertSeparator();
	dl_id = options->insertItem( "Default label", this, SLOT(defaultLabel()) );
	cl_id = options->insertItem( "Custom label", this, SLOT(customLabel()) );
	options->insertSeparator();
	md_id = options->insertItem( "No minimum duration", this, SLOT(toggleMinimumDuration()) );
	options->setCheckable( TRUE );
	loopDriven();
	defaultLabel();

	setFixedSize( 400, 300 );

	setBackgroundColor( black );
    }

public slots:
    void doMenuItem(int id)
    {
	if (id >= first_draw_item && id <= last_draw_item)
	    draw(drawItemRects(id));
    }

    void stopDrawing() { got_stop = TRUE; }

    void timerDriven()
    {
	timer_driven = TRUE;
	options->setItemChecked( td_id, TRUE );
	options->setItemChecked( ld_id, FALSE );
    }

    void loopDriven()
    {
	timer_driven = FALSE;
	options->setItemChecked( ld_id, TRUE );
	options->setItemChecked( td_id, FALSE );
    }

    void defaultLabel()
    {
	default_label = TRUE;
	options->setItemChecked( dl_id, TRUE );
	options->setItemChecked( cl_id, FALSE );
    }

    void customLabel()
    {
	default_label = FALSE;
	options->setItemChecked( dl_id, FALSE );
	options->setItemChecked( cl_id, TRUE );
    }

    void toggleMinimumDuration()
    {
	options->setItemChecked( md_id, 
	   !options->isItemChecked( md_id ) );
    }

private:
    void timerEvent( QTimerEvent* )
    {
	pb->setProgress( pb->totalSteps() - rects );
	rects--;

	{
	    QPainter p(this);

	    int ww = width();
	    int wh = height();

	    if ( ww > 8 && wh > 8 ) {
		QColor c(rand()%255, rand()%255, rand()%255);
		int x = rand() % (ww-8);
		int y = rand() % (wh-8);
		int w = rand() % (ww-x);
		int h = rand() % (wh-y);
		p.fillRect( x, y, w, h, c );
	    }
	}

	if (!rects || got_stop) {
	    pb->setProgress( pb->totalSteps() );
	    QPainter p(this);
	    p.fillRect(0, 0, width(), height(), backgroundColor());
	    enableDrawingItems(TRUE);
	    killTimers();
	    delete pb;
	    pb = 0;
	}
    }

    QProgressDialog* newProgressDialog( const char* label, int steps, bool modal )
    {
	QProgressDialog *d = new QProgressDialog(label, "Cancel", steps, this,
						 "progress", modal);
        if ( options->isItemChecked( md_id ) )
	    d->setMinimumDuration(0);
	if ( !default_label )
	    d->setLabel( new AnimatedThingy(d,label) );
	return d;
    }

    void enableDrawingItems(bool yes)
    {
	for (int i=first_draw_item; i<=last_draw_item; i++) {
	    menubar->setItemEnabled(i, yes);
	}
    }
		    
    void draw(int n)
    {
	if ( timer_driven ) {
	    if ( pb ) {
		qWarning("This cannot happen!");
		return;
	    }
	    rects = n;
	    pb = newProgressDialog("Drawing rectangles.\n"
				   "Using timer event.", n, FALSE);
	    pb->setCaption("Please Wait");
	    connect(pb, SIGNAL(cancelled()), this, SLOT(stopDrawing()));
	    enableDrawingItems(FALSE);
	    startTimer(0);
	    got_stop = FALSE;
	} else {
	    QProgressDialog* lpb = newProgressDialog(
			"Drawing rectangles.\nUsing loop.", n, TRUE);
	    lpb->setCaption("Please Wait");

	    QPainter p(this);
	    for (int i=0; i<n; i++) {
		lpb->setProgress(i);
		if ( lpb->wasCancelled() )
		    break;

		QColor c(rand()%255, rand()%255, rand()%255);
		int x = rand()%(width()-8);
		int y = rand()%(height()-8);
		int w = rand()%(width()-x);
		int h = rand()%(height()-y);
		p.fillRect(x,y,w,h,c);
	    }

	    p.fillRect(0, 0, width(), height(), backgroundColor());

	    delete lpb;
	}
    }

    QMenuBar* menubar;
    QProgressDialog* pb;
    QPopupMenu* options;
    int td_id, ld_id;
    int dl_id, cl_id;
    int md_id;
    int rects;
    bool timer_driven;
    bool default_label;
    bool got_stop;
};

int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    int wincount = argc > 1 ? atoi(argv[1]) : 1;

    for ( int i=0; i<wincount; i++ ) {
	CPUWaster* cpuw = new CPUWaster;
	if ( i == 0 ) a.setMainWidget(cpuw);
	cpuw->setCaption("Qt Example - Progress");
	cpuw->show();
    }
    return a.exec();
}

#include "progress.moc"
