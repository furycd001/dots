/****************************************************************************
** $Id: qt/examples/splitter/splitter.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qapplication.h>
#include <qlabel.h>
#include <qsplitter.h>
#include <qmultilineedit.h>

#include <qpainter.h>


class Test : public QWidget {
public:
    Test(QWidget* parent=0, const char* name=0, int f=0);
    void paintEvent(QPaintEvent* e);
private:
};



Test::Test(QWidget* parent, const char* name, int f) :
    QWidget(parent, name, f)
{

}

void Test::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
    p.setClipRect(e->rect());
    const int d = 1000; //large number
    int x1 = 0;
    int x2 = width()-1;
    int y1 = 0;
    int y2 = height()-1;

    int x = (x1+x2)/2;
    p.drawLine( x, y1, x+d, y1+d   );
    p.drawLine( x, y1, x-d, y1+d   );
    p.drawLine( x, y2, x+d, y2-d   );
    p.drawLine( x, y2, x-d, y2-d   );

    int y = (y1+y2)/2;
    p.drawLine( x1, y, x1+d, y+d   );
    p.drawLine( x1, y, x1+d, y-d   );
    p.drawLine( x2, y, x2-d, y+d   );
    p.drawLine( x2, y, x2-d, y-d   );
}


int main( int argc, char ** argv )
{
    QApplication a( argc, argv );

    QSplitter *s1 = new QSplitter( QSplitter::Vertical, 0 , "main" );

    QSplitter *s2 = new QSplitter( QSplitter::Horizontal, s1, "top" );

    Test *t1 = new Test( s2 );
    t1->setBackgroundColor( Qt::blue.light( 180 ) );
    t1->setMinimumSize( 50, 0 );

    Test *t2 = new Test( s2 );
    t2->setBackgroundColor( Qt::green.light( 180 ) );
    s2->setResizeMode( t2, QSplitter::KeepSize );
    s2->moveToFirst( t2 );

    QSplitter *s3 = new QSplitter( QSplitter::Horizontal,  s1, "bottom" );

    Test *t3 = new Test( s3 );
    t3->setBackgroundColor( Qt::red );
    Test *t4 = new Test( s3 );
    t4->setBackgroundColor( Qt::white );

    Test *t5 = new Test( s3 );
    t5->setMaximumHeight( 250 );
    t5->setMinimumSize( 80, 50 );
    t5->setBackgroundColor( Qt::yellow );

#ifdef _WS_QWS_
    // Qt/Embedded XOR drawing not yet implemented.
    s1->setOpaqueResize( TRUE );
#endif
    s2->setOpaqueResize( TRUE );
    s3->setOpaqueResize( TRUE );

    a.setMainWidget( s1 );
    s1->setCaption("Qt Example - Splitters");
    s1->show();
    int result = a.exec();
    delete s1;
    return result;
}
