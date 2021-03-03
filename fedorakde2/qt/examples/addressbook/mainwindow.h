/****************************************************************************
** $Id: qt/examples/addressbook/mainwindow.h   2.3.2   edited 2001-01-26 $
**
** Copyright ( C ) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef AB_MAINWINDOW_H
#define AB_MAINWINDOW_H

#include <qmainwindow.h>
#include <qstring.h>

class QToolBar;
class QPopupMenu;
class ABCentralWidget;

class ABMainWindow: public QMainWindow
{
    Q_OBJECT

public:
    ABMainWindow();
    ~ABMainWindow();

protected slots:
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void filePrint();
    void closeWindow();

protected:
    void setupMenuBar();
    void setupFileTools();
    void setupStatusBar();
    void setupCentralWidget();

    QToolBar *fileTools;
    QString filename;
    ABCentralWidget *view;

};


#endif
