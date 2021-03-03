/****************************************************************************
** $Id: qt/examples/overlay_x11/gearwidget.h   2.3.2   edited 2001-01-26 $
**
** Definition of a simple Qt OpenGL widget
**
** Copyright (C) 1999 by Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef GEAR_H
#define GEAR_H

#include <qgl.h>

class GearWidget : public QGLWidget
{
public:
    GearWidget( QWidget *parent=0, const char *name=0 );

protected:
    void initializeGL();
    void resizeGL( int, int );
    void paintGL();
};


#endif
