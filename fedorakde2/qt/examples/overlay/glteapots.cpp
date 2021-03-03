/****************************************************************************
** $Id: qt/examples/overlay/glteapots.cpp   2.3.2   edited 2001-01-26 $
**
** Implementation of GLTeapots
** This is a QGLWidget displaying a group of teapots and a rubber-band
** in an overlay plane
**
** The OpenGL code in this example is mostly borrowed from the "teapots"
** example program in the "OpenGL Programming Guide", by Jackie Neider,
** Tom Davis, and Mason Woo, Addison Wesley 1993.
** It can be obtained from ftp.sgi.com and contains the following copyright
** notice:

 * (c) Copyright 1993, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED 
 * Permission to use, copy, modify, and distribute this software for 
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that 
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission. 
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * US Government Users Restricted Rights 
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(TM) is a trademark of Silicon Graphics, Inc.

****************************************************************************/

#include "glteapots.h"
#include <qapplication.h>

#if defined(_CC_MSVC_)
#pragma warning(disable:4305) // init: truncation from const double to float
#endif

/*!
  Create a GLTeapots widget.

  Specifies the following frame buffer requirements to the QGLWidget
  constructor:
  <ul>
  <li> HasOverlay - we want an overlay context to draw the rubber-band in
  <li> SingleBuffer - makes the drawing process of the teapots visible
  </ul>
*/


GLTeapots::GLTeapots( QWidget* parent, const char* name )
    : QGLWidget( QGLFormat( QGL::SingleBuffer | QGL::HasOverlay ),
		 parent, name )
{
    teapotList = 0;
    rubberOn = FALSE;
}

/*!
  Release allocated resources
*/

GLTeapots::~GLTeapots()
{
    glDeleteLists( teapotList, 1 );
}


/*!
  Do the main plane painting: a set of teapots.
*/

void GLTeapots::paintGL()
{
    QApplication::setOverrideCursor( waitCursor ); // Since it takes some time

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderTeapot( 2.0, 17.0, 0.0215, 0.1745, 0.0215, 
		  0.07568, 0.61424, 0.07568,
		  0.633, 0.727811, 0.633, 0.6 );
    renderTeapot( 2.0, 14.0, 0.135, 0.2225, 0.1575,
		  0.54, 0.89, 0.63, 0.316228,
		  0.316228, 0.316228, 0.1 );
    renderTeapot( 2.0, 11.0, 0.05375, 0.05, 0.06625,
		  0.18275, 0.17, 0.22525,
		  0.332741, 0.328634, 0.346435, 0.3 );
    renderTeapot( 2.0, 8.0, 0.25, 0.20725, 0.20725,
		  1, 0.829, 0.829, 0.296648,
		  0.296648, 0.296648, 0.088 );
    renderTeapot( 2.0, 5.0, 0.1745, 0.01175, 0.01175,
		  0.61424, 0.04136, 0.04136,
		  0.727811, 0.626959, 0.626959, 0.6);
    renderTeapot( 2.0, 2.0, 0.1, 0.18725, 0.1745,
		  0.396, 0.74151, 0.69102,
		  0.297254, 0.30829, 0.306678, 0.1 );

    renderTeapot( 6.0, 17.0, 0.329412, 0.223529, 0.027451,
		  0.780392, 0.568627, 0.113725,
		  0.992157, 0.941176, 0.807843, 0.21794872 );
    renderTeapot( 6.0, 14.0, 0.2125, 0.1275, 0.054,
		  0.714, 0.4284, 0.18144,
		  0.393548, 0.271906, 0.166721, 0.2 );
    renderTeapot( 6.0, 11.0, 0.25, 0.25, 0.25, 
		  0.4, 0.4, 0.4,
		  0.774597, 0.774597, 0.774597, 0.6 );
    renderTeapot( 6.0, 8.0, 0.19125, 0.0735, 0.0225,
		  0.7038, 0.27048, 0.0828,
		  0.256777, 0.137622, 0.086014, 0.1 );
    renderTeapot( 6.0, 5.0, 0.24725, 0.1995, 0.0745,
		  0.75164, 0.60648, 0.22648,
		  0.628281, 0.555802, 0.366065, 0.4 );
    renderTeapot( 6.0, 2.0, 0.19225, 0.19225, 0.19225,
		  0.50754, 0.50754, 0.50754,
		  0.508273, 0.508273, 0.508273, 0.4 );

    renderTeapot( 10.0, 17.0, 0.0, 0.0, 0.0,
		  0.01, 0.01, 0.01,
		  0.50, 0.50, 0.50, .25 );
    renderTeapot( 10.0, 14.0, 0.0, 0.1, 0.06,
		  0.0, 0.50980392, 0.50980392,
		  0.50196078, 0.50196078, 0.50196078, .25 );
    renderTeapot( 10.0, 11.0, 0.0, 0.0, 0.0, 
		  0.1, 0.35, 0.1,
		  0.45, 0.55, 0.45, .25 );
    renderTeapot( 10.0, 8.0, 0.0, 0.0, 0.0,
		  0.5, 0.0, 0.0,
		  0.7, 0.6, 0.6, .25 );
    renderTeapot( 10.0, 5.0, 0.0, 0.0, 0.0,
		  0.55, 0.55, 0.55,
		  0.70, 0.70, 0.70, .25 );
    renderTeapot( 10.0, 2.0, 0.0, 0.0, 0.0,
		  0.5, 0.5, 0.0,
		  0.60, 0.60, 0.50, .25 );

    renderTeapot( 14.0, 17.0, 0.02, 0.02, 0.02,
		  0.01, 0.01, 0.01,
		  0.4, 0.4, 0.4, .078125 );
    renderTeapot( 14.0, 14.0, 0.0, 0.05, 0.05,
		  0.4, 0.5, 0.5,
		  0.04, 0.7, 0.7, .078125 );
    renderTeapot( 14.0, 11.0, 0.0, 0.05, 0.0,
		  0.4, 0.5, 0.4,
		  0.04, 0.7, 0.04, .078125 );
    renderTeapot( 14.0, 8.0, 0.05, 0.0, 0.0,
		  0.5, 0.4, 0.4,
		  0.7, 0.04, 0.04, .078125 );
    renderTeapot( 14.0, 5.0, 0.05, 0.05, 0.05,
		  0.5, 0.5, 0.5,
		  0.7, 0.7, 0.7, .078125 );
    renderTeapot( 14.0, 2.0, 0.05, 0.05, 0.0,
		  0.5, 0.5, 0.4, 
		  0.7, 0.7, 0.04, .078125 );

    // May add a glFinish() here to make sure the GL rendering has finished
    // before the mouse cursor is reset from "waiting" to normal.
    QApplication::restoreOverrideCursor();
}


/*!
  Set up the OpenGL rendering state of the main plane
*/

void GLTeapots::initializeGL()
{
    GLfloat ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat position[] = { 0.0, 3.0, 3.0, 0.0 };
    
    GLfloat lmodel_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat local_view[] = { 0.0 };

    glLightfv( GL_LIGHT0, GL_AMBIENT, ambient );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
    glLightfv( GL_LIGHT0, GL_POSITION, position );
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, lmodel_ambient );
    glLightModelfv( GL_LIGHT_MODEL_LOCAL_VIEWER, local_view );

    glFrontFace( GL_CW );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glEnable( GL_AUTO_NORMAL );
    glEnable( GL_NORMALIZE );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );
}



/*!
  Set up the main plane's view port, matrix mode, etc. suitably for
  viewing the teapots.
*/

void GLTeapots::resizeGL( int w, int h )
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if ( w <= h )
	glOrtho( 0.0, 20.0, 0.0, 20.0*(GLfloat)h/(GLfloat)w, -10.0, 10.0 );
    else
	glOrtho( 0.0, 20.0*(GLfloat)w/(GLfloat)h, 0.0, 20.0, -10.0, 10.0 );
    glMatrixMode(GL_MODELVIEW);
}


/*!
  Set up the OpenGL rendering state of the overlay plane
*/

void GLTeapots::initializeOverlayGL()
{
    glLineWidth( 2 );
    glLineStipple( 3, 0xAAAA );
    glEnable( GL_LINE_STIPPLE );
}


/*!
  Do the overlay plane painting: a rubber-band (if the user has
  dragged out one).

*/

void GLTeapots::paintOverlayGL()
{
    glClear( GL_COLOR_BUFFER_BIT );
    if ( rubberOn ) {
	qglColor( QColor( 255, 255, 0 ) );
	glBegin( GL_LINE_LOOP );
	glVertex2i( rubberP1.x(), rubberP1.y() );
	glVertex2i( rubberP2.x(), rubberP1.y() );
	glVertex2i( rubberP2.x(), rubberP2.y() );
	glVertex2i( rubberP1.x(), rubberP2.y() );
	glEnd();
    }
}


/*!
  Set up the overlay plane's view port, matrix mode, etc. suitably for
  viewing the rubber-band.
*/

void GLTeapots::resizeOverlayGL( int w, int h )
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D( 0, w, h, 0 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


/*!
  User presses a mouse button: start pulling out a rubber-band rectangle
*/

// We do a simple rubber-band drawing here; erasing the whole overlay
// plane for each repaint. For less flickering of the rubber-band
// itself, we could remember the coordinates of the last rectangle
// drawn, and then erase only that by painting it again with
// transparent color.

void GLTeapots::mousePressEvent( QMouseEvent* e )
{
    rubberP1 = e->pos();
    rubberP2 = rubberP1;
    rubberOn = TRUE;
}

/*!
  User drags the mouse: Update the rubber-band coordinates and repaint
  the overlay.
*/

void GLTeapots::mouseMoveEvent( QMouseEvent* e )
{
    if ( rubberOn ) {
	rubberP2 = e->pos();
	updateOverlayGL();
    }
}

/*!
  User releases the mouse button: remove the rubber-band.
*/

void GLTeapots::mouseReleaseEvent( QMouseEvent* )
{
    if ( rubberOn ) {
	rubberOn = FALSE;
	updateOverlayGL();
    }
}


/*!
  Renders a teapot with the given material properties
*/

void GLTeapots::renderTeapot( GLfloat x, GLfloat y, GLfloat ambr,
			  GLfloat ambg, GLfloat ambb, GLfloat difr,
			  GLfloat difg, GLfloat difb, 
			  GLfloat specr, GLfloat specg, 
			  GLfloat specb, GLfloat shine )
{
    float mat[4];

    glPushMatrix();
    glTranslatef (x, y, 0.0);
    mat[0] = ambr; mat[1] = ambg; mat[2] = ambb; mat[3] = 1.0;	
    glMaterialfv (GL_FRONT, GL_AMBIENT, mat);
    mat[0] = difr; mat[1] = difg; mat[2] = difb;	
    glMaterialfv (GL_FRONT, GL_DIFFUSE, mat);
    mat[0] = specr; mat[1] = specg; mat[2] = specb;
    glMaterialfv (GL_FRONT, GL_SPECULAR, mat);
    glMaterialf (GL_FRONT, GL_SHININESS, shine*128.0);
    teapot();
    glPopMatrix();
}


/*!
  Draw a classic OpenGL teapot
*/

void GLTeapots::teapot()
{
    static long patchdata[][16] = {
	{102,103,104,105,4,5,6,7,8,9,10,11,12,13,14,15},
	{12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27},
	{24,25,26,27,29,30,31,32,33,34,35,36,37,38,39,40},
	{96,96,96,96,97,98,99,100,101,101,101,101,0,1,2,3,},
	{0,1,2,3,106,107,108,109,110,111,112,113,114,115,116,117},
	{118,118,118,118,124,122,119,121,123,126,125,120,40,39,38,37},
	{41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56},
	{53,54,55,56,57,58,59,60,61,62,63,64,28,65,66,67},
	{68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83},
	{80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95}
    };

    static float cpdata[][3] = {
	{0.2,0,2.7},{0.2,-0.112,2.7},{0.112,-0.2,2.7},{0,-0.2,2.7},
	{1.3375,0,2.53125},{1.3375,-0.749,2.53125},{0.749,-1.3375,2.53125},
	{0,-1.3375,2.53125},{1.4375,0,2.53125},{1.4375,-0.805,2.53125},
	{0.805,-1.4375,2.53125},{0,-1.4375,2.53125},{1.5,0,2.4},
	{1.5,-0.84,2.4},{0.84,-1.5,2.4},{0,-1.5,2.4},{1.75,0,1.875},
	{1.75,-0.98,1.875},{0.98,-1.75,1.875},{0,-1.75,1.875},{2,0,1.35},
	{2,-1.12,1.35},{1.12,-2,1.35},{0,-2,1.35},{2,0,0.9},{2,-1.12,0.9},
	{1.12,-2,0.9},{0,-2,0.9},{-2,0,0.9},{2,0,0.45},{2,-1.12,0.45},
	{1.12,-2,0.45},{0,-2,0.45},{1.5,0,0.225},{1.5,-0.84,0.225},
	{0.84,-1.5,0.225},{0,-1.5,0.225},{1.5,0,0.15},{1.5,-0.84,0.15},
	{0.84,-1.5,0.15},{0,-1.5,0.15},{-1.6,0,2.025},{-1.6,-0.3,2.025},
	{-1.5,-0.3,2.25},{-1.5,0,2.25},{-2.3,0,2.025},{-2.3,-0.3,2.025},
	{-2.5,-0.3,2.25},{-2.5,0,2.25},{-2.7,0,2.025},{-2.7,-0.3,2.025},
	{-3,-0.3,2.25},{-3,0,2.25},{-2.7,0,1.8},{-2.7,-0.3,1.8},{-3,-0.3,1.8},
	{-3,0,1.8},{-2.7,0,1.575},{-2.7,-0.3,1.575},{-3,-0.3,1.35},{-3,0,1.35},
	{-2.5,0,1.125},{-2.5,-0.3,1.125},{-2.65,-0.3,0.9375},{-2.65,0,0.9375},
	{-2,-0.3,0.9},{-1.9,-0.3,0.6},{-1.9,0,0.6},{1.7,0,1.425},
	{1.7,-0.66,1.425},{1.7,-0.66,0.6},{1.7,0,0.6},{2.6,0,1.425},
	{2.6,-0.66,1.425},{3.1,-0.66,0.825},{3.1,0,0.825},{2.3,0,2.1},
	{2.3,-0.25,2.1},{2.4,-0.25,2.025},{2.4,0,2.025},{2.7,0,2.4},
	{2.7,-0.25,2.4},{3.3,-0.25,2.4},{3.3,0,2.4},{2.8,0,2.475},
	{2.8,-0.25,2.475},{3.525,-0.25,2.49375},{3.525,0,2.49375},
	{2.9,0,2.475},{2.9,-0.15,2.475},{3.45,-0.15,2.5125},{3.45,0,2.5125},
	{2.8,0,2.4},{2.8,-0.15,2.4},{3.2,-0.15,2.4},{3.2,0,2.4},{0,0,3.15},
	{0.8,0,3.15},{0.8,-0.45,3.15},{0.45,-0.8,3.15},{0,-0.8,3.15},
	{0,0,2.85},{1.4,0,2.4},{1.4,-0.784,2.4},{0.784,-1.4,2.4},{0,-1.4,2.4},
	{0.4,0,2.55},{0.4,-0.224,2.55},{0.224,-0.4,2.55},{0,-0.4,2.55},
	{1.3,0,2.55},{1.3,-0.728,2.55},{0.728,-1.3,2.55},{0,-1.3,2.55},
	{1.3,0,2.4},{1.3,-0.728,2.4},{0.728,-1.3,2.4},{0,-1.3,2.4},{0,0,0},
	{1.425,-0.798,0},{1.5,0,0.075},{1.425,0,0},{0.798,-1.425,0},
	{0,-1.5,0.075},{0,-1.425,0},{1.5,-0.84,0.075},{0.84,-1.5,0.075}
    };

    static float tex[2][2][2] = {{{0, 0},{1, 0}},{{0, 1},{1, 1}}};

    if ( !glIsList( teapotList ) ) {
	float p[4][4][3], q[4][4][3], r[4][4][3], s[4][4][3];
	long grid = 14;
	
	teapotList = glGenLists( 1 );
	glNewList( teapotList, GL_COMPILE );
	glPushMatrix();
	glRotatef( 270.0, 1.0, 0.0, 0.0 );
	glScalef( 0.5, 0.5, 0.5 );
	glTranslatef( 0.0, 0.0, -1.5 );
	for ( long i = 0; i < 10; i++ ) {
	    for ( long j = 0; j < 4; j++ ) {
		for ( long k = 0; k < 4; k++ ) {
		    for ( long l = 0; l < 3; l++ ) {
			p[j][k][l] = cpdata[patchdata[i][j*4+k]][l];
			q[j][k][l] = cpdata[patchdata[i][j*4+(3-k)]][l];
			if ( l == 1 ) 
			    q[j][k][l] *= -1.0;
			if ( i < 6 ) {
			    r[j][k][l] = cpdata[patchdata[i][j*4+(3-k)]][l];
			    if ( l == 0 )
				r[j][k][l] *= -1.0;
			    s[j][k][l] = cpdata[patchdata[i][j*4+k]][l];
			    if ( l == 0 )
				s[j][k][l] *= -1.0;
			    if ( l == 1 )
				s[j][k][l] *= -1.0;
			}
		    }
		}
	    }
	    glMap2f( GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2, 
		    &tex[0][0][0] );
	    glMap2f( GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &p[0][0][0] );
	    glEnable( GL_MAP2_VERTEX_3);
	    glEnable(GL_MAP2_TEXTURE_COORD_2 );
	    glMapGrid2f( grid, 0.0, 1.0, grid, 0.0, 1.0 );
	    glEvalMesh2( GL_LINE, 0, grid, 0, grid );
	    glMap2f( GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &q[0][0][0] );
	    glEvalMesh2( GL_LINE, 0, grid, 0, grid );
	    if ( i < 6 ) {
		glMap2f( GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4,
			 &r[0][0][0]);
		glEvalMesh2( GL_LINE, 0, grid, 0, grid );
		glMap2f( GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4,
			 &s[0][0][0] );
		glEvalMesh2( GL_LINE, 0, grid, 0, grid );
	    }
	}
	glDisable(GL_MAP2_VERTEX_3);
	glDisable(GL_MAP2_TEXTURE_COORD_2);
	glPopMatrix();
	glEndList();
    }

    glCallList( teapotList );
}
