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

#ifndef IMPORT_H
#define IMPORT_H

#include <qstring.h>
#include <qobject.h>

class QProcess;
class MainWindow;

class Import : public QObject
{
    Q_OBJECT
    
public:
    Import( const QString &fn, MainWindow *mw  );
    
    static QString filters();
    static int numFilters();
    static QString wildcard( int i );
    static QString command( int i );
    static void addImportEntry( const QString &wilrdcard, const QString &command );
    
private slots:
    void gotData( const QString &data );
    void bye();
    
private:
    QProcess *process;
    QString filename;
    MainWindow *mainWindow;
    QString incomingData;

    bool haveLoaded;    
};

#endif
