/****************************************************************************
** $Id: qt/examples/box/glbox.cpp   2.3.2   edited 2001-01-26 $
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
** The OpenGL code is mostly borrowed from Brian Pauls "spin" example
** in the Mesa distribution
**
****************************************************************************/

#include "glbox.h"

#if defined(_CC_MSVC_)
#pragma warning(disable:4305) // init: truncation from const double to float
#endif

/*!
  Create a GLBox widget
*/

GLBox::GLBox( QWidget* parent, const char* name )
    : QGLWidget( parent, name )
{
    xRot = yRot = zRot = 0.0;		// default object rotation
    scale = 1.25;			// default object scale
    object = 0;
}


/*!
  Release allocated resources
*/

GLBox::~GLBox()
{
    glDeleteLists( object, 1 );
}


/*!
  Paint the box. The actual openGL commands for drawing the box are
  performed here.
*/

void GLBox::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT );

    glLoadIdentity();
    glTranslatef( 0.0, 0.0, -10.0 );
    glScalef( scale, scale, scale );

    glRotatef( xRot, 1.0, 0.0, 0.0 ); 
    glRotatef( yRot, 0.0, 1.0, 0.0 ); 
    glRotatef( zRot, 0.0, 0.0, 1.0 );

    glCallList( object );
}


/*!
  Set up the OpenGL rendering state, and define display list
*/

void GLBox::initializeGL()
{
    qglClearColor( black ); 		// Let OpenGL clear to black
    object = makeObject();		// Generate an OpenGL display list
    glShadeModel( GL_FLAT );
}



/*!
  Set up the OpenGL view port, matrix mode, etc.
*/

void GLBox::resizeGL( int w, int h )
{
    glViewport( 0, 0, (GLint)w, (GLint)h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glFrustum( -1.0, 1.0, -1.0, 1.0, 5.0, 15.0 );
    glMatrixMode( GL_MODELVIEW );
}


/*!
  Generate an OpenGL display list for the object to be shown, i.e. the box
*/

GLuint GLBox::makeObject()
{	
    GLuint list;

    list = glGenLists( 1 );

    glNewList( list, GL_COMPILE );

    qglColor( white );		      // Shorthand for glColor3f or glIndex

    glLineWidth( 2.0 );

    glBegin( GL_LINE_LOOP );
    glVertex3f(  1.0,  0.5, -0.4 );
    glVertex3f(  1.0, -0.5, -0.4 );
    glVertex3f( -1.0, -0.5, -0.4 );
    glVertex3f( -1.0,  0.5, -0.4 );
    glEnd();

    glBegin( GL_LINE_LOOP );
    glVertex3f(  1.0,  0.5, 0.4 );
    glVertex3f(  1.0, -0.5, 0.4 );
    glVertex3f( -1.0, -0.5, 0.4 );
    glVertex3f( -1.0,  0.5, 0.4 );
    glEnd();

    glBegin( GL_LINES );
    glVertex3f(  1.0,  0.5, -0.4 );   glVertex3f(  1.0,  0.5, 0.4 );
    glVertex3f(  1.0, -0.5, -0.4 );   glVertex3f(  1.0, -0.5, 0.4 );
    glVertex3f( -1.0, -0.5, -0.4 );   glVertex3f( -1.0, -0.5, 0.4 );
    glVertex3f( -1.0,  0.5, -0.4 );   glVertex3f( -1.0,  0.5, 0.4 );
    glEnd();

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
