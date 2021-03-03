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

#ifndef __rmbmenu_h__
#define __rmbmenu_h__

#include <qpopupmenu.h>
#include "taskmanager.h"

class OpMenu : public QPopupMenu
{
    Q_OBJECT

public:
    OpMenu( Task* t, TaskManager* manager, QWidget *parent = 0, const char *name = 0 );
    virtual ~OpMenu();

    enum WindowOperation {
        MaximizeOp = 100,
        IconifyOp,
        CloseOp,
        ToCurrentOp,
        RestoreOp,
        StayOnTopOp,
        ShadeOp
    };

protected slots:
    void init();
    void initDeskPopup();
    void op( int );
    void sendToDesktop( int );


private:
    Task 		*task;
    TaskManager 	*taskManager;
    QPopupMenu 		*deskpopup;
};

class RMBMenu : public QPopupMenu
{
    Q_OBJECT

public:
    RMBMenu( QList<Task>* list, TaskManager* manager, QWidget *parent = 0, const char *name = 0 );
    virtual ~RMBMenu();

    void init();

protected slots:
    void slotExec( int );

private:
    QMap<int, Task*> 	 map;
    QList<Task> 	*tasks;
    TaskManager 	*taskManager;
};

#endif
