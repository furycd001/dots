/****************************************************************************
** $Id: qt/examples/texture/gltexobj.h   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

/****************************************************************************
**
** This is a simple QGLWidget displaying an openGL wireframe box
**
****************************************************************************/

#ifndef GLTEXOBJ_H
#define GLTEXOBJ_H

#include <qgl.h>


class GLTexobj : public QGLWidget
{
    Q_OBJECT

public:

    GLTexobj( QWidget* parent, const char* name );
    ~GLTexobj();

public slots:

    void		setXRotation( int degrees );
    void		setYRotation( int degrees );
    void		setZRotation( int degrees );
    void		toggleAnimation();

protected:

    void		initializeGL();
    void		paintGL();
    void		resizeGL( int w, int h );

    virtual GLuint 	makeObject( const QImage& tex1, const QImage& tex2 );

private:
    bool animation;
    GLuint object;
    GLfloat xRot, yRot, zRot, scale;
    QTimer* timer;
};


#endif // GLTEXOBJ_H
