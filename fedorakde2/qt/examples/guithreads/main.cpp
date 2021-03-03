/****************************************************************************
** $Id: qt/examples/guithreads/main.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qapplication.h>
#include <qhbox.h>
#include <qthread.h>
#include <qlabel.h>


class WaitThread: public QThread
{
public:
    WaitThread(QThread *thr1, QThread *thr2, QLabel *lbl);

    void run();


private:
    QThread *thread1, *thread2;
    QLabel *label;
};


WaitThread::WaitThread(QThread *thr1, QThread *thr2, QLabel *lbl)
    : thread1(thr1), thread2(thr2), label(lbl)
{
}


void WaitThread::run()
{
    thread1->wait();
    thread2->wait();

    qApp->lock();
    label->setText("<h1>Done!</h1>");
    qApp->unlock();
}


class GUIThread : public QThread
{
public:
    GUIThread( QLabel*, const QString& );

protected:
    void run();

private:
    QLabel* label;
    QString text;
};

static QMutex* mutex;

GUIThread::GUIThread( QLabel* l, const QString& t )
: label( l ), text( t )
{
}

void GUIThread::run()
{
    for ( int i = 0; i < 5; i++ ) {
	mutex->lock();
	qApp->lock();
	label->setText(text);
	qApp->unlock();
	sleep( 1 );
	mutex->unlock();
    }
}

// The program starts here

int main( int argc, char** argv )
{
    QApplication app( argc, argv );
    mutex = new QMutex;

    QHBox box( 0, 0, TRUE );
    QLabel label( &box );
    label.setAlignment(Qt::AlignCenter);
    label.setMinimumSize(400, 300);

    GUIThread first( &label, "<b>Ping</b>" );
    GUIThread second( &label, "<i>Pong</i>" );
    WaitThread third(&first, &second, &label);

    app.setMainWidget( &box );
    box.setCaption("Qt Example - GUI Threads");
    box.show();

    first.start();
    second.start();
    third.start();

    int r = app.exec();

    delete mutex;
    return r;
}

