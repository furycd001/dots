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

#ifndef __taskcontainer_h__
#define __taskcontainer_h__

#include <qlist.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qtimer.h>
#include <qtoolbutton.h>

#include "taskmanager.h"

class LMBMenu;
class RMBMenu;

class TaskContainer : public QToolButton
{
    Q_OBJECT

public:
    TaskContainer( Task*, TaskManager*, bool showAll, bool sort, bool icon, QWidget *parent = 0, const char *name = 0 );
    TaskContainer( Startup*, TaskManager*, bool showAll, bool sort, bool icon, QWidget *parent = 0, const char *name = 0 );
    virtual ~TaskContainer();

    void setArrowType( Qt::ArrowType at );
    void setShowAll( bool );
    void setSortByDesktop( bool );
    void setShowIcon( bool );

    void init();

    void add( Task* );
    void add( Startup* );

    void remove( Task* );
    void remove( Startup* );

    bool contains( Task* );
    bool contains( Startup* );
    bool contains( WId );

    bool isEmpty();
    bool onCurrentDesktop();

    QString id();
    int desktop();
    QString name();

    virtual QSizePolicy sizePolicy () const;

    void publishIconGeometry( QPoint );
    void desktopChanged( int );
    void windowDesktopChanged( WId );

protected slots:
    void animationTimerFired();
    void toggled();
    void popupLMB();
    void popupRMB();
    void dragSwitch();
    void update();

protected:
    void drawButton( QPainter* );
    void mousePressEvent( QMouseEvent* );
    void mouseReleaseEvent( QMouseEvent* );
    void dragEnterEvent( QDragEnterEvent* );
    void dragLeaveEvent( QDragLeaveEvent* );

    void updateFilteredTaskList();

private:
    QString 			sid;
    QTimer           	 	animationTimer;
    QTimer 			dragSwitchTimer;
    int               	  	currentFrame;
    QList<Task> 		tasks;
    QList<Task> 		ftasks;
    QList<Startup> 		startups;
    QList<QPixmap> 		frames;
    ArrowType 			arrowType;
    TaskManager 		*taskManager;
    LMBMenu 			*lmbMenu;
    RMBMenu 			*rmbMenu;
    bool 			showAll;
    bool 			sortByDesktop;
    bool			showIcon;
    static QImage 		blendGradient;
};

#endif
