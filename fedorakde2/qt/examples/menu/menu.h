/****************************************************************************
** $Id: qt/examples/menu/menu.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef MENU_H
#define MENU_H

#include <qwidget.h>
#include <qmenubar.h>
#include <qlabel.h>


class MenuExample : public QWidget
{
    Q_OBJECT
public:
    MenuExample( QWidget *parent=0, const char *name=0 );

public slots:
    void open();
    void news();
    void save();
    void closeDoc();
    void undo();
    void redo();
    void normal();
    void bold();
    void underline();
    void about();
    void aboutQt();
    void printer();
    void file();
    void fax();
    void printerSetup();

protected:
    void    resizeEvent( QResizeEvent * );

signals:
    void    explain( const QString& );

private:
    QMenuBar *menu;
    QLabel   *label;
    bool isBold;
    bool isUnderline;
    int boldID, underlineID;
};


#endif // MENU_H
