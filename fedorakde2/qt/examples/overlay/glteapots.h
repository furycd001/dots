/****************************************************************************
** $Id: qt/examples/overlay/glteapots.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

/****************************************************************************
**
** This is a QGLWidget displaying a group of teapots and a rubber-band
** in an overlay plane
**
****************************************************************************/

#ifndef GLBOX_H
#define GLBOX_H

#include <qgl.h>


class GLTeapots : public QGLWidget
{
    Q_OBJECT

public:

    GLTeapots( QWidget* parent, const char* name );
    ~GLTeapots();

protected:

    void		initializeGL();
    void		paintGL();
    void		resizeGL( int w, int h );

    void		initializeOverlayGL();
    void		paintOverlayGL();
    void		resizeOverlayGL( int w, int h );

    void		mousePressEvent( QMouseEvent* e );
    void		mouseMoveEvent( QMouseEvent* e );
    void		mouseReleaseEvent( QMouseEvent* e );

    void		renderTeapot( GLfloat x, GLfloat y, GLfloat ambr,
				      GLfloat ambg, GLfloat ambb, GLfloat difr,
				      GLfloat difg, GLfloat difb, 
				      GLfloat specr, GLfloat specg, 
				      GLfloat specb, GLfloat shine );

    void		teapot();

private:
    GLuint teapotList;
    QPoint rubberP1;
    QPoint rubberP2;
    bool rubberOn;
};


#endif // GLBOX_H
