/****************************************************************************
** $Id: qt/examples/glpixmap/glbox.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

/****************************************************************************
**
** This is a simple QGLWidget displaying a box
**
****************************************************************************/

#ifndef GLBOX_H
#define GLBOX_H

#include <qgl.h>


class GLBox : public QGLWidget
{
    Q_OBJECT

public:

    GLBox( QWidget* parent, const char* name, const QGLWidget* shareWidget=0 );
    GLBox( const QGLFormat& format, QWidget* parent, const char* name, 
	   const QGLWidget* shareWidget=0 );
    ~GLBox();

    void		copyRotation( const GLBox& fromBox );

public slots:

    void		setXRotation( int degrees );
    void		setYRotation( int degrees );
    void		setZRotation( int degrees );

protected:

    void		initializeGL();
    void		paintGL();
    void		resizeGL( int w, int h );

    virtual GLuint 	makeObject();

private:

    GLuint object;

    GLfloat	xRot, yRot, zRot, scale;

};


#endif // GLBOX_H
