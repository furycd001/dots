/****************************************************************************
** $Id: qt/extensions/xt/src/qxt.h   2.3.2   edited 2001-01-26 $
**
** Definition of Qt extension classes for Xt/Motif support.
**
** Created : 980107
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QXT_H
#define QXT_H

#include <qapplication.h>
#include <qwidget.h>
#include <X11/Intrinsic.h>

class QXtApplication : public QApplication {
    Q_OBJECT
    void init();
public:
    QXtApplication(int& argc, char** argv,
	const char* appclass=0,
	XrmOptionDescRec *options=0, int num_options=0,
	const char** resources=0);
    QXtApplication(Display*);
    QXtApplication(Display *, int, char **);
    ~QXtApplication();
};

class QXtWidget : public QWidget {
    Q_OBJECT
    Widget xtw;
    Widget xtparent;
    bool   need_reroot;
    void init(const char* name, WidgetClass widget_class,
		    Widget parent, QWidget* qparent,
		    ArgList args, Cardinal num_args,
		    bool managed);
    friend void qwidget_realize( Widget widget, XtValueMask* mask,
				 XSetWindowAttributes* attributes );

public:
    QXtWidget(const char* name, Widget parent, bool managed=FALSE);
    QXtWidget(const char* name, WidgetClass widget_class,
	      QWidget *parent=0, ArgList args=0, Cardinal num_args=0,
	      bool managed=FALSE);
    ~QXtWidget();

    Widget xtWidget() const { return xtw; }
    bool isActiveWindow() const;
    void setActiveWindow();

protected:
    void moveEvent( QMoveEvent* );
    void resizeEvent( QResizeEvent* );
    bool x11Event( XEvent * );
};

#endif
