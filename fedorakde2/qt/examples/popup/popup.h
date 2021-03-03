/****************************************************************************
** $Id: qt/examples/popup/popup.h   2.3.2   edited 2001-01-26 $
**
** Definition of something or other
**
** Created : 979899
**
** Copyright (C) 1997 by Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef POPUP_H
#define POPUP_H
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>

class FancyPopup : public QLabel
{
    Q_OBJECT
public:
    FancyPopup( QWidget* parent = 0, const char*  name=0);
    
    void popup( QWidget* parent = 0);
protected:
    virtual void mouseMoveEvent( QMouseEvent * );
    virtual void mouseReleaseEvent( QMouseEvent * );
    virtual void closeEvent( QCloseEvent * );
    
private:
    QWidget* popupParent;
    int moves;
};


 class Frame : public QFrame
 {
     Q_OBJECT
 public:
     Frame( QWidget *parent=0, const char*  name=0);

 protected:

 private slots:
     void button1Clicked();
     void button2Pressed();
     
 private:
     QPushButton *button1;
     QPushButton *button2;
     
     QFrame* popup1;
     FancyPopup* popup2;
 };

#endif
