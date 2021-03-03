/****************************************************************************
** $Id: qt/extensions/nsplugin/examples/grapher/grapher.h   2.3.2   edited 2001-07-22 $
**
** Grapher example for netscape plugin.
**
** Created : 970601
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
#ifndef GRAPHER_H
#define GRAPHER_H

// Include Qt Netscape Plugin classes.
#include "qnp.h"

// Include other Qt classes.
#include <qpainter.h>
#include <qtextstream.h>
#include <qbuffer.h>
#include <qpixmap.h>
#include <qmenubar.h>
#include <qpushbutton.h>
#include <qlist.h>
#include <qmessagebox.h>

#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>

// Include some C library functions.
#include <math.h>
#include <stdlib.h>

#ifndef M_PI // Some math.h don't include this.
#define M_PI 3.14159265358979323846264338327950288
#endif


// GraphModel is a simple abstract class that describes
// a table of numeric and text data.
//

class GraphModel {
public:
    enum ColType { Numeric, Label };

    union Datum {
	double dbl;
	QString* str;
    };

    virtual QList<Datum>& graphData()=0;
    virtual ColType colType(int col) const=0;
    virtual int nCols() const=0;
};


//
// Graph is a widget subclass that displays a GraphModel.
// Since the widget is a QNPWidget, it can be used as a plugin window,
// returned by Grapher::newWindow() below.
//

#ifdef WIN32
#include <windows.h>
#endif


class Graph : public QNPWidget {
    Q_OBJECT
public:
    // Constructs a Graph to display a GraphModel
    //
    Graph(GraphModel&);
    ~Graph();

    // Two styles are available - Pie and Bar graph
    //
    enum Style { Pie, Bar };
    static const char* styleName[];
    void setStyle(Style);
    void setStyle(const char*);

    // Timer event processing rotates the pie graph
    //
    void timerEvent(QTimerEvent*);

    // These functions are provided by QNPWidget - we override
    // them to hide and show the plugin menubar.
    //
    void enterInstance();
    void leaveInstance();

    // Paint the graph...
    //
    void paintEvent(QPaintEvent*);
    //
    // ... as either a "Loading" message, a Bar graph, a Pie graph,
    // or an error message.
    //
    void paintWait(QPaintEvent*);
    void paintBar(QPaintEvent*);
    void paintPie(QPaintEvent*);
    void paintError(const char*);
    //bool winEvent(MSG *msg);

signals:
    // Signals emitted when the Help menus are selected.
    void aboutPlugin();
    void aboutData();

private:
    GraphModel& model;
    QMenuBar *menubar;
    Style style;
    QPopupMenu* stylemenu;
    int pieRotationTimer;
    int pieRotation;
    QPixmap pm;
    //QFrame *f;

private slots:
    void setStyleFromMenu(int id);
};

//
// Grapher is a subclass of QNPInstance, and so it can be returned
// by GrapherPlugin::newInstance().  A QNPInstance represents the
// plugin, distinctly from the plugin window.
//
// Grapher is also a GraphModel, because it loads graph data from
// the net.  When Grapher creates a window in newWindow(), it creates
// a Graph widget to display the GraphModel that is the Grapher itself.
//

class Grapher : public QNPInstance, GraphModel {
    Q_OBJECT
public:
    // Create a Grapher - all Grapher plugins are created
    // by one GrapherPlugin object.
    //
    Grapher();
    ~Grapher();

    // We override this QNPInstance function to create our
    // own subclass of QNPWidget, a Graph widget.
    //
    QNPWidget* newWindow();

    // We override this QNPInstance function to process the
    // incoming graph data.
    //
    int write(QNPStream* /*str*/, int /*offset*/, int len, void* buffer);

private:
    // Grapher is a GraphModel, so it implements the pure virtual
    // functions of that class.
    //
    QList<Datum>& graphData();
    ColType colType(int col) const;
    int nCols() const;

    void consumeLine();
    QList<Datum> data;
    QBuffer line;
    bool firstline;
    int ncols;
    ColType *coltype;

private slots:
    // Slots that are connected to the Graph menu items.
    //
    void aboutPlugin();
    void aboutData();
};

#endif