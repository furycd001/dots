/*****************************************************************

Copyright (c) 2001 Matthias Elter <elter@kde.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#ifndef __taskbar_h__
#define __taskbar_h__

#include <qlist.h>
#include <taskmanager.h>

#include "fittslawscrollview.h"
#include "taskcontainer.h"

class QGridLayout;

class TaskBar : public FittsLawScrollView
{
    Q_OBJECT

public:
    TaskBar( bool enableFrame, QWidget *parent = 0, const char *name = 0 );
    ~TaskBar();

    void setOrientation( Orientation );
    void setArrowType( Qt::ArrowType at );
    Orientation orientation() { return orient; }
    TaskManager* taskManager();

    void configure();
    int containerCount();

public slots:
    void scrollLeft();
    void scrollRight();

signals:
    void needScrollButtons( bool );
    void containerCountChanged();

protected slots:
    void add( Task* );
    void add( Startup* );
    void remove( Task* );
    void remove( Startup* );

    void desktopChanged( int );
    void windowDesktopChanged( WId );

    void publishIconGeometry();

protected:
    void viewportMousePressEvent( QMouseEvent* );
    void viewportMouseReleaseEvent( QMouseEvent* );
    void viewportMouseDoubleClickEvent( QMouseEvent* );
    void viewportMouseMoveEvent( QMouseEvent* );
    void propagateMouseEvent( QMouseEvent* );
    void resizeEvent( QResizeEvent* );
    void reLayout();
    bool idMatch( const QString& id1, const QString& id2 );

private:
    Orientation 		orient;
    static 			TaskManager* manager;
    bool 			blocklayout;
    bool			showAllWindows;
    bool			groupTasks;
    bool 			sortByDesktop;
    bool			showIcon;
    ArrowType 			arrowType;
    QList<TaskContainer> 	containers;
};

#endif
