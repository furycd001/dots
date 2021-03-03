/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef HELP_H
#define HELP_H

#if defined(HAVE_KDE)
#include <kmainwindow.h>
class KToolBar;
#else
class QToolBar;
#include <qmainwindow.h>
#endif

#include <qmap.h>

class QTextBrowser;
class MainWindow;
class HelpDialog;
class QPopupMenu;

#if defined(HAVE_KDE)
#define QMainWindow KMainWindow
#endif

class Help : public QMainWindow
{
    Q_OBJECT

#undef QMainWindow
    
public:
    Help( const QString& home, MainWindow* parent = 0, const char *name=0 );
    ~Help();

    void setSource( const QString& );

    void setupBookmarkMenu();
    QTextBrowser *viewer() const { return browser; }

private slots:
    void textChanged();
    void filePrint();
    void goTopics();
    void goHome();
    void goQt();
    void showLink( const QString &link, const QString &title );
    void showBookmark( int id );

private:
    void setupFileActions();
    void setupGoActions();

private:
    QTextBrowser* browser;
#if defined(HAVE_KDE)
    KToolBar *toolbar;
#else
    QToolBar *toolbar;
#endif
    MainWindow *mainWindow;
    HelpDialog *helpDialog;
    QPopupMenu *bookmarkMenu;
    QMap<int, QString> bookmarks;

};

#endif
