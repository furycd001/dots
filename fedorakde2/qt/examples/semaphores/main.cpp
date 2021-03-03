/****************************************************************************
** $Id: qt/examples/semaphores/main.cpp   2.3.2   edited 2001-07-19 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
#include <qapplication.h>
#include <qwidget.h>
#include <qpushbutton.h>
#include <qmultilineedit.h>
#include <qthread.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qlabel.h>

#if defined(QT_NO_THREAD)
#  error Thread support not enabled.
#endif

// Use pointers to create semaphores after QApplication object!
QSemaphore* yellowSem, *greenSem;


class YellowThread : public QThread
{
public:
    YellowThread(QWidget *o)
	: receiver(o)
    { ; }

    void run();


private:
    QWidget *receiver;
};


void YellowThread::run()
{
    for (int i = 0; i < 20; i++) {
	(*yellowSem)++;

	QCustomEvent *event = new QCustomEvent(12345);
     	event->setData(new QString("Yellow!"));
	QThread::postEvent(receiver, event);
	msleep(200);

	(*greenSem)--;
    }

    (*yellowSem)++;

    QCustomEvent *event = new QCustomEvent(12346);
    event->setData(new QString("Yellow!"));
    QThread::postEvent(receiver, event);

    (*greenSem)--;
}


class GreenThread: public QThread
{
public:
   GreenThread(QWidget *o)
	: receiver(o)
    { ; }

    void run();


private:
    QWidget *receiver;
};


void GreenThread::run()
{
    for (int i = 0; i < 20; i++) {
	(*greenSem)++;

	QCustomEvent *event = new QCustomEvent(12345);
     	event->setData(new QString("Green!"));
	QThread::postEvent(receiver, event);
	msleep(200);

	(*yellowSem)--;
    }

    (*greenSem)++;

    QCustomEvent *event = new QCustomEvent(12346);
    event->setData(new QString("Green!"));
    QThread::postEvent(receiver, event);
    msleep(10);

    (*yellowSem)--;
}



class SemaphoreExample : public QWidget
{
    Q_OBJECT
public:
    SemaphoreExample();

    void customEvent(QCustomEvent *);


public slots:
    void startExample();


protected:


private:
    QMultiLineEdit *mlineedit;
    QPushButton *button;
    QLabel *label;

    YellowThread yellowThread;
    GreenThread greenThread;
};


SemaphoreExample::SemaphoreExample()
    : QWidget(), yellowThread(this), greenThread(this)
{
    button = new QPushButton("&Ignition!", this);
    connect(button, SIGNAL(clicked()), SLOT(startExample()));

    mlineedit = new QMultiLineEdit(this);
    label = new QLabel(this);

    QVBoxLayout *vbox = new QVBoxLayout(this, 5);
    vbox->addWidget(button);
    vbox->addWidget(mlineedit);
    vbox->addWidget(label);
}


void SemaphoreExample::startExample()
{
    if (yellowThread.running() || greenThread.running()) {
	QMessageBox::information(this, "Sorry",
				 "The threads have not completed yet, and must finish before "
				 "they can be started again.");

	return;
    }

    mlineedit->clear();

    while (yellowSem->available() < yellowSem->total()) (*yellowSem)--;
    (*yellowSem)++;

    yellowThread.start();
    greenThread.start();
}


void SemaphoreExample::customEvent(QCustomEvent *event) {
    switch (event->type()) {
    case 12345:
	{
	    QString *s = (QString *) event->data();

	    mlineedit->append(*s);

	    if (*s == "Green!")
		label->setBackgroundColor(green);
	    else
		label->setBackgroundColor(yellow);
	    label->setText(*s);

	    delete s;

	    break;
	}

    case 12346:
	{
	    QString *s = (QString *) event->data();

	    QMessageBox::information(this, (*s) + " - Finished",
				     "The thread creating the \"" + *s +
				     "\" events has finished.");

	    break;
	}

    default:
	{
	    qWarning("Unknown custom event type: %d", event->type());
	}
    }
}


int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    yellowSem = new QSemaphore(1);
    greenSem = new QSemaphore(1);

    SemaphoreExample se;
    app.setMainWidget(&se);
    se.show();

    int r = app.exec();

    delete yellowSem;
    delete greenSem;

    return r;
}


#include "main.moc"
