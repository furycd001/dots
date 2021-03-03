/****************************************************************************
** $Id: qt/examples/themes/themes.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef THEMES_H
#define THEMES_H

#include <qmainwindow.h>
#include <qfont.h>

class QTabWidget;

class Themes: public QMainWindow
{
    Q_OBJECT

public:
    Themes( QWidget *parent = 0, const char *name = 0, WFlags f = WType_TopLevel );

protected:
    QTabWidget *tabwidget;

    int sWood, sMetal, sPlatinum, sWindows, sCDE, sMotif, sMotifPlus;

protected slots:
    void styleWood();
    void styleMetal();
    void stylePlatinum();
    void styleWindows();
    void styleCDE();
    void styleMotif();
    void styleMotifPlus();

    void about();
    void aboutQt();

private:
    void selectStyleMenu( int );

    QFont appFont;

};


#endif
