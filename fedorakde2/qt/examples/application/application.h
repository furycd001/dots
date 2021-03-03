/****************************************************************************
** $Id: qt/examples/application/application.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef APPLICATION_H
#define APPLICATION_H

#include <qmainwindow.h>

class QMultiLineEdit;
class QToolBar;
class QPopupMenu;

class ApplicationWindow: public QMainWindow
{
    Q_OBJECT
public:
    ApplicationWindow();
    ~ApplicationWindow();
    
protected:
    void closeEvent( QCloseEvent* );

private slots:
    void newDoc();
    void load();
    void load( const char *fileName );
    void save();
    void saveAs();
    void print();

    void about();
    void aboutQt();

private:
    QPrinter *printer;
    QMultiLineEdit *e;
    QToolBar *fileTools;
    QString filename;
};


#endif
