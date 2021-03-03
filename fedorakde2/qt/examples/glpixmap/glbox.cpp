/****************************************************************************
** $Id: qt/examples/glpixmap/glbox.cpp   2.3.2   edited 2001-01-26 $
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
** The OpenGL code is mostly borrowed from Brian Pauls "spin" example
** in the Mesa distribution
**
****************************************************************************/

#include <math.h>

#include "glbox.h"

#if defined(_CC_MSVC_)
#pragma warning(disable:4305) // init: truncation from const double to float
#endif

/*!
  Create a GLBox widget
*/

GLBox::GLBox( QWidget* parent, const char* name, const QGLWidget* shareWidget )
    : QGLWidget( parent, name, shareWidget )
{
    xRot = yRot = zRot = 0.0;		// default object rotation
    scale = 1.5;			// default object scale
}


/*!
  Create a GLBox widget
*/

GLBox::GLBox( const QGLFormat& format, QWidget* parent, const char* name, 
	      const QGLWidget* shareWidget )
    : QGLWidget( format, parent, name, shareWidget )
{
    xRot = yRot = zRot = 0.0;		// default object rotation
    scale = 1.5;			// default object scale
}


/*!
  Release allocated resources
*/

GLBox::~GLBox()
{
    glDeleteLists( object, 1 );
}


/*!
  Set up the OpenGL rendering state, and define display list
*/

void GLBox::initializeGL()
{
    qglClearColor( green );		// Let OpenGL clear to green
    object = makeObject();		// Make display list
    glEnable( GL_DEPTH_TEST );
}


/*!
  Set up the OpenGL view port, matrix mode, etc.
*/

void GLBox::resizeGL( int w, int h )
{
    glViewport( 0, 0, (GLint)w, (GLint)h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 200.0);
}


/*!
  Paint the box. The actual openGL commands for drawing the box are
  performed here.
*/

void GLBox::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatef( 0.0, 0.0, -3.0 );
    glScalef( scale, scale, scale );

    glRotatef( xRot, 1.0, 0.0, 0.0 ); 
    glRotatef( yRot, 0.0, 1.0, 0.0 ); 
    glRotatef( zRot, 0.0, 0.0, 1.0 );

    glCallList( object );
}






/*!
  Generate an OpenGL display list for the object to be shown, i.e. the box
*/

GLuint GLBox::makeObject()
{	
    GLuint list;

    list = glGenLists( 1 );

    glNewList( list, GL_COMPILE );

    GLint i, j, rings, sides;
    float theta1, phi1, theta2, phi2;
    float v0[03], v1[3], v2[3], v3[3];
    float t0[03], t1[3], t2[3], t3[3];
    float n0[3], n1[3], n2[3], n3[3];
    float innerRadius=0.4;
    float outerRadius=0.8;
    float scalFac;
    double pi = 3.14159265358979323846;

    rings = 8;
    sides = 10;
    scalFac=1/(outerRadius*2);

    for (i = 0; i < rings; i++) {
        theta1 = (float)i * 2.0 * pi / rings;
        theta2 = (float)(i + 1) * 2.0 * pi / rings;
        for (j = 0; j < sides; j++) {
            phi1 = (float)j * 2.0 * pi / sides;
            phi2 = (float)(j + 1) * 2.0 * pi / sides;

            v0[0] = cos(theta1) * (outerRadius + innerRadius * cos(phi1));
            v0[1] = -sin(theta1) * (outerRadius + innerRadius * cos(phi1));
            v0[2] = innerRadius * sin(phi1);

            v1[0] = cos(theta2) * (outerRadius + innerRadius * cos(phi1));
            v1[1] = -sin(theta2) * (outerRadius + innerRadius * cos(phi1));
            v1[2] = innerRadius * sin(phi1);
            v2[0] = cos(theta2) * (outerRadius + innerRadius * cos(phi2));
            v2[1] = -sin(theta2) * (outerRadius + innerRadius * cos(phi2));
            v2[2] = innerRadius * sin(phi2);

            v3[0] = cos(theta1) * (outerRadius + innerRadius * cos(phi2));
            v3[1] = -sin(theta1) * (outerRadius + innerRadius * cos(phi2));
            v3[2] = innerRadius * sin(phi2);

            n0[0] = cos(theta1) * (cos(phi1));
            n0[1] = -sin(theta1) * (cos(phi1));
            n0[2] = sin(phi1);

            n1[0] = cos(theta2) * (cos(phi1));
            n1[1] = -sin(theta2) * (cos(phi1));
            n1[2] = sin(phi1);

            n2[0] = cos(theta2) * (cos(phi2));
            n2[1] = -sin(theta2) * (cos(phi2));
            n2[2] = sin(phi2);

            n3[0] = cos(theta1) * (cos(phi2));
            n3[1] = -sin(theta1) * (cos(phi2));
            n3[2] = sin(phi2);

            t0[0] = v0[0]*scalFac + 0.5;
            t0[1] = v0[1]*scalFac + 0.5;
            t0[2] = v0[2]*scalFac + 0.5;

            t1[0] = v1[0]*scalFac + 0.5;
            t1[1] = v1[1]*scalFac + 0.5;
            t1[2] = v1[2]*scalFac + 0.5;

            t2[0] = v2[0]*scalFac + 0.5;
            t2[1] = v2[1]*scalFac + 0.5;
            t2[2] = v2[2]*scalFac + 0.5;

            t3[0] = v3[0]*scalFac + 0.5;
            t3[1] = v3[1]*scalFac + 0.5;
            t3[2] = v3[2]*scalFac + 0.5;

	    // Create blue-black checkered coloring
	    if ( (i+j) % 2 )
		qglColor( black );
	    else
		qglColor( QColor( "steelblue" ) );

            glBegin(GL_POLYGON);
                glNormal3fv(n3); glTexCoord3fv(t3); glVertex3fv(v3);
                glNormal3fv(n2); glTexCoord3fv(t2); glVertex3fv(v2);
                glNormal3fv(n1); glTexCoord3fv(t1); glVertex3fv(v1);
                glNormal3fv(n0); glTexCoord3fv(t0); glVertex3fv(v0);
            glEnd();
        }
    }
    glEndList();

    return list;
}


/*!
  Set the rotation angle of the object to \e degrees around the X axis.
*/

void GLBox::setXRotation( int degrees )
{
    xRot = (GLfloat)(degrees % 360);
    updateGL();
}


/*!
  Set the rotation angle of the object to \e degrees around the Y axis.
*/

void GLBox::setYRotation( int degrees )
{
    yRot = (GLfloat)(degrees % 360);
    updateGL();
}


/*!
  Set the rotation angle of the object to \e degrees around the Z axis.
*/

void GLBox::setZRotation( int degrees )
{
    zRot = (GLfloat)(degrees % 360);
    updateGL();
}




/*!
  Sets the rotation angles of this object to that of \a fromBox
*/

void GLBox::copyRotation( const GLBox& fromBox )
{
    xRot = fromBox.xRot;
    yRot = fromBox.yRot;
    zRot = fromBox.zRot;
}
