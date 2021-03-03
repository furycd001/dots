/****************************************************************************
** $Id: qt/examples/popup/popup.cpp   2.3.2   edited 2001-06-12 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "popup.h"
#include <qapplication.h>
#include <qkeycode.h>
#include <qlayout.h>

FancyPopup::FancyPopup( QWidget* parent, const char*  name ):
    QLabel( parent, name, WType_Popup ){
        setFrameStyle( WinPanel|Raised );
        setAlignment( AlignCenter );
        resize(150,100);
        moves = 0;
        setMouseTracking( TRUE );
}

void FancyPopup::mouseMoveEvent( QMouseEvent * e){
    moves++;
    QString s;
    s.sprintf("%d/%d", e->pos().x(), e->pos().y());
    if (e->state() & QMouseEvent::LeftButton)
        s += " (down)";
    setText(s);
}

void FancyPopup::mouseReleaseEvent( QMouseEvent * e){
    if  (rect().contains( e->pos() ) || moves > 5)
        close();
}

void FancyPopup::closeEvent( QCloseEvent *e ){
    e->accept();
    moves = 0;
    if (!popupParent)
        return;

    // remember that we (as a popup) might recieve the mouse release
    // event instead of the popupParent. This is due to the fact that
    // the popupParent popped us up in its mousePressEvent handler. To
    // avoid the button remaining in pressed state we simply send a
    // faked mouse button release event to it.
    QMouseEvent me( QEvent::MouseButtonRelease, QPoint(0,0), QPoint(0,0), QMouseEvent::LeftButton, QMouseEvent::NoButton);
    QApplication::sendEvent( popupParent, &me );
}

void FancyPopup::popup( QWidget* parent) {
    popupParent = parent;
    setText("Move the mouse!");
    if (popupParent)
        move( popupParent->mapToGlobal( popupParent->rect().bottomLeft() ) );
    show();
}






Frame::Frame(QWidget* parent, const char* name): QFrame(parent, name){
    button1 = new QPushButton("Simple Popup", this);
    connect ( button1, SIGNAL( clicked() ), SLOT( button1Clicked() ) );
    button2 = new QPushButton("Fancy Popup", this);
    connect ( button2, SIGNAL( pressed() ), SLOT( button2Pressed() ) );

    QBoxLayout * l = new QHBoxLayout( this );
    button1->setMaximumSize(button1->sizeHint());
    button2->setMaximumSize(button2->sizeHint());
    l->addWidget( button1 );
    l->addWidget( button2 );
    l->activate();

//     button1->setGeometry(20,20,100,30);
//     button2->setGeometry(140,20,100,30);
    resize(270, 70);

    //create a very simple popup: it is just composed with other
    //widget and will be shown after clicking on button1

    popup1 = new QFrame( this ,0, WType_Popup);
    popup1->setFrameStyle( WinPanel|Raised );
    popup1->resize(150,100);
    QLineEdit *tmpE = new QLineEdit( popup1 );
    connect( tmpE, SIGNAL( returnPressed() ), popup1, SLOT( hide() ) );
    tmpE->setGeometry(10,10, 130, 30);
    tmpE->setFocus();
    QPushButton *tmpB = new QPushButton("Click me!", popup1);
    connect( tmpB, SIGNAL( clicked() ), popup1, SLOT( close() ) );
    tmpB->setGeometry(10, 50, 130, 30);

    // the fancier version uses its own class. It will be shown when
    // pressing button2, so they behavior is more like a modern menu
    // or toolbar.

    popup2 = new FancyPopup( this );

    // you might also add new widgets to the popup, just like you do
    // it with any other widget.  The next four lines (if not
    // commented out) will for instance add a line edit widget.

//     tmpE = new QLineEdit( popup2 );
//     tmpE->setFocus();
//     connect( tmpE, SIGNAL( returnPressed() ), popup2, SLOT( close() ) );
//     tmpE->setGeometry(10, 10, 130, 30);
}


void Frame::button1Clicked(){
    popup1->move( mapToGlobal( button1->geometry().bottomLeft() ) );
    popup1->show();
}

void Frame::button2Pressed(){
    popup2->popup(button2);
}


int main( int argc, char **argv )
{
    QApplication a(argc,argv);

    Frame frame;
    frame.setCaption("Qt Example - Custom Popups");
    a.setMainWidget(&frame);
    frame.show();
    return a.exec();
}
