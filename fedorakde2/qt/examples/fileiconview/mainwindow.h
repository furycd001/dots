/****************************************************************************
** $Id: qt/examples/fileiconview/mainwindow.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef MAINWIN_H
#define MAINWIN_H

#include <qmainwindow.h>

class QtFileIconView;
class DirectoryView;
class QProgressBar;
class QLabel;
class QComboBox;
class QToolButton;

class FileMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    FileMainWindow();

    QtFileIconView *fileView() { return fileview; }
    DirectoryView *dirList() { return dirlist; }

    void show();

protected:
    void setup();
    void setPathCombo();

    QtFileIconView *fileview;
    DirectoryView *dirlist;
    QProgressBar *progress;
    QLabel *label;
    QComboBox *pathCombo;
    QToolButton *upButton, *mkdirButton;

protected slots:
    void directoryChanged( const QString & );
    void slotStartReadDir( int dirs );
    void slotReadNextDir();
    void slotReadDirDone();
    void cdUp();
    void newFolder();
    void changePath( const QString &path );
    void enableUp();
    void disableUp();
    void enableMkdir();
    void disableMkdir();

};

#endif
