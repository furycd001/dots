/* ----------------------------------------------------------------------
 *
 *                kPipes - OpenGL screensave for KDE
 *
 *                  Copyright (C) 1998 Lars Doelle
 *                      lars.doelle@on-line.de
 *
 * For implementations notes, see below
 *
 * -----------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Some overall stuff taken from space.c, which is
 * Copyright (c) 1998 Bernd Johannes Wuebben.
 *
 * The initGL routine might originate from Silicon Graphics
 * Copyright (c) 1991, 1992, 1993 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the name of
 * Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Silicon Graphics.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF
 * ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 * ------------------------------------------------------------------------ --
 */

//TODO: Overall, the screensaver is in a good shape.
// - should reinit when a repaintEvent occures
// - antialising would certainly improve drawing
// - Some additional parameters could be added to the setup
// - check FIXMEs below

/* Outline of the algorithm ----------------------------------------------- --

   The overall geometry is organized by a cube consisting of (2*CUBESIZE+1)^3
   cells. The even number for it's length in each direction guaranties the
   existencen of a unique center cell, which we assign the cell coordinate
   (0,0,0).

   The center of each of the cells are the places where growing pipes may
   eventually change their direction. The transition to the coordinate system
   of the actual pipes is done by further subdividing the cells into SUBCELLS^3
   subcells, where the length of each of the subcells is identified with
   the RADIUS of the pipes.

   The coordinate system for the pipes is defined using these subcells lengths
   as units, where the origin is the center of the center cell. Thus the
   pipe coordinate system is SUBCELLS times finer than the cell coordinate
   system in each direction.

   Thus the size of the cube is SUBCELLS*(2*CUBESIZE+1)*RADIUS.
FIXME: this should be used to adjust the OpenGL projection. The total size of
       the box should then be normalized to 1.

   The pipe drawing is accomplished by two primitives:

   1) The origin of the pipe is drawn once (paintGL). It is a simple sphere
      (start, makeSphere(1)). It must be located in the center of a cell.

   2) The pipe is extended by a composition of a sphere and a cylinder.
      (arrow, makeArrow()). The cylinder has the same RADIUS and height, and
      is placed so, that one edge of the cylinder is an equator of the sphere.
      This construction is directed, and must be rotated before it is put in
      place. Since the height of the cylinder is RADIUS, one extention has
      exactly the length of one subcell, so it is placed on subcell appart
      from the original head of the pipe.

   While a pipe may change it's direction when it's head is in the center of
   a cell it has to move straight while inbetween. This takes SUBCELLS steps.
   The variable `steps' counts the number of extentions and triggers a
   direction choose (chooseDir) whenever it becomes divideable by SUBCELLS.
FIXME: this could be clearer done by a calculation on the position.

   The choose basically consists of examining the surrounding within the
   cube for being occupied and choosing randomly among them. If no such
   cell exists, the trapped pipe stops running.

   As soon as less then half of the original pipes are left, the whole
   procedure starts over.


   Color selections is done by means of the HSV model. We choose two different
   colors and interpolate between them to get colors for the remaining pipes.
   We choose both hues randomly for a great varity of colors, but guarantee,
   that they differ by at least 50 degrees.

   Using the HSV model is very importent to get a proper lightning. We've
   tried RGB, too, but this does not give us any control over the intensity
   of the light.

-- ------------------------------------------------------------------------ */
#include "config.h"

#include <qslider.h>
#include <qlayout.h>
#include <kglobal.h>
#include <kconfig.h>
#include <krandomsequence.h>
#include <kdebug.h>

#include "xlock.h"
#ifdef HAVE_CONFIG_H
#include "../../config.h"
#endif

#include <klocale.h>
#include <kconfig.h>

#include "helpers.h"

#undef index
#include "pipes.h"
#include <X11/Intrinsic.h>

#ifdef HAVE_GL

#ifdef HAVE_GL_XMESA_H
#include <GL/xmesa.h>
#endif
#include <GL/gl.h>
#ifdef HAVE_GL_GLUT_H
// We don't need GLUT, but some BROKEN GLU implemenations, such as the one
// used in SuSE Linux 6.3, do. :(
#include <GL/glut.h>
#endif
#include <GL/glu.h>
#include <GL/glx.h>

// GL related helper routines /////////////////////////////////////////////////

GLenum doubleBuffer, directRender;
GLint windW, windH;

static GLXContext  glx_context;
static KRandomSequence *rnd = 0;

void reshape(int w, int h)
{
    glViewport( 0, 0, (GLint)w, (GLint)h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glFrustum( -w/(float)h, w/(float)h, -1.0, 1.0, 5.0, 15.0 );
    glMatrixMode( GL_MODELVIEW );
}

void doneGL()
{
  glXDestroyContext(dsp, glx_context);
}

static XVisualInfo *glVis[MAXSCREENS];

int getVisual(XVisualInfo * wantVis, int visual_count) {

        Display    *display = dsp;
        static int  first;
	int i;


        if (first) {
                for (screen = 0; screen < MAXSCREENS; screen++)
                        glVis[screen] = NULL;
        }


        if (!glVis[screen]) {
                if (mono) {
                        /* Monochrome display - use color index mode */
      //    int         attribList[] = {GLX_DOUBLEBUFFER, None};
                        int         attribList[] = {None};

                        glVis[screen] = glXChooseVisual(display, screen, attribList);
                } else {
                        int         attribList[] =
        //{GLX_RGBA, GLX_DOUBLEBUFFER, GLX_DEPTH_SIZE, 1, None};
                        {GLX_RGBA,  GLX_DEPTH_SIZE, 1, None};

                        glVis[screen] = glXChooseVisual(display, screen, attribList);
                }
        }

        // Make sure we have a visual
        if (!glVis[screen])
	  return (0);

        /* check if GL can render into root window. */
        for(i=0;i<visual_count;i++)
	  if ( (glVis[screen]->visual == (wantVis+i)->visual) )
	    return (1); // success

        // The visual we received did not match one we asked for
        return (0);
}

void initGL(Window window)
{
  Display    *display = dsp;
  XWindowAttributes xwa;


  (void) XGetWindowAttributes(dsp, window, &xwa);
  int         n;
  XVisualInfo *wantVis, vTemplate;
  int  VisualClassWanted=-1;

  vTemplate.screen = screen;
  vTemplate.depth = xwa.depth;

  if (VisualClassWanted == -1) {
    vTemplate.c_class = DefaultVisual(display, screen)->c_class;
  } else {
    vTemplate.c_class = VisualClassWanted;
  }

  wantVis = XGetVisualInfo(display,
         VisualScreenMask | VisualDepthMask | VisualClassMask,
         &vTemplate, &n);

  if (VisualClassWanted != -1 && n == 0) {
    /* Wanted visual not found so use default */

    vTemplate.c_class = DefaultVisual(display, screen)->c_class;

    wantVis = XGetVisualInfo(display,
           VisualScreenMask | VisualDepthMask | VisualClassMask,
           &vTemplate, &n);
  }
  /* if User asked for color, try that first, then try mono */
  /* if User asked for mono.  Might fail on 16/24 bit displays,
     so fall back on color, but keep the mono "look & feel". */

  if (!getVisual(wantVis, n)) {
    if (!getVisual(wantVis, n)) {
      kdError() << i18n("GL can not render with root visual\n");
      return;
    }
  }

  /* PURIFY 3.0a on SunOS4 reports a 104 byte memory leak on the next line each
   * time that morph3d mode is run in random mode. */

  glx_context = glXCreateContext(display, wantVis, 0, True);

  XFree((char *) wantVis);


  glXMakeCurrent(display, window, glx_context);

  // Make sure we draw to the correct buffer.
  glDrawBuffer(GL_FRONT);

  if (mono) {
    glIndexi(WhitePixel(display, screen));
    glClearIndex(BlackPixel(display, screen));
  }

  reshape(xwa.width, xwa.height);
}

#endif

// K Screen Saver Interface ///////////////////////////////////////////////////

#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qcolor.h>

#include <kmessagebox.h>

#include "pipes.moc"

#undef Below

static kPipesSaver *saver = NULL;

void startScreenSaver( Drawable d )
{
  if ( saver )
    return;
  saver = new kPipesSaver( d );
}

void stopScreenSaver()
{
  if ( saver )
    delete saver;
  saver = NULL;
}

int setupScreenSaver()
{
  kPipesSetup dlg;

  return dlg.exec();
}

// ///////////////////////////////////////////////////////////////////////////
// GLPipes - quick'n'dirty (tm) remake of a popular screen saver
// Copyright (c) 1998 by Lars Doelle <lars.doelle@on-line.de>
// GPL Version 2 applies.

#include "stdio.h"
#include "stdlib.h"
#include <qtimer.h>

#include <time.h>

#define HERE fprintf(stderr,"%s(%d): here.\n",__FILE__,__LINE__)

#ifdef HAVE_GL

static GLfloat base[2][4] =
{
  {0.3, 0.3, 0.3, 0.0 }, // red
  {0.1, 0.1, 0.8, 0.0 }  // brown
};

static GLfloat color[MAXPIPES][4] =
{
  {0.8, 0.8, 0.0, 0.0 }, // red
  {0.3, 0.3, 0.1, 0.0 }, // blue
  {0.2, 0.1, 0.4, 0.0 }, // green
  {0.0, 0.8, 0.7, 0.0 }, // lightblue
  {0.5, 0.7, 0.1, 0.0 }, // yellow
  {0.6, 0.2, 0.2, 0.0 }  // brown
};

void setColors(int n)
{ int i;
  int hue0 = rnd->getLong(360);
  int hue1 = (hue0 + rnd->getLong(260) + 50)%360;
  QColor co[2];
  co[0].setHsv(hue0,200,180);
  co[1].setHsv(hue1,200,150);
  for (i = 0; i < 2; i++)
  {
    base[i][0] = co[i].red  ()/255.0;
    base[i][1] = co[i].green()/255.0;
    base[i][2] = co[i].blue ()/255.0;
    base[i][3] = 0;
  }
  for (i = 0; i < n; i++)
  {
    color[i][0] = i*(base[1][0]-base[0][0])/(n-1) + base[0][0];
    color[i][1] = i*(base[1][1]-base[0][1])/(n-1) + base[0][1];
    color[i][2] = i*(base[1][2]-base[0][2])/(n-1) + base[0][2];
    color[i][3] = i*(base[1][3]-base[0][3])/(n-1) + base[0][3];
  }
}

// This is used both for movement and rotating the head (arrow) in
// the proper direction.

struct Direction
{
  int x, y, z;
  int deg90;
  int rx, ry;
};

static Direction direct[6] =
{//  x   y   z  deg  rx  ry
  { -1,  0,  0,  +1,  0, +1 }, // dec X
  {  1,  0,  0,  -1,  0, +1 }, // inc X
  {  0, -1,  0,  -1, +1,  0 }, // dec Y
  {  0,  1,  0,  +1, +1,  0 }, // inc Y
  {  0,  0, -1,   0,  0,  0 }, // dec Z
  {  0,  0,  1,  +2, +1,  0 }  // inc Z
};

Pipe::Pipe(kPipesSaver* b, int c)
{
  box = b;
  col = c;
}

bool Pipe::chooseDir(bool mayExtend)
{ int x0 = x/SUBCELLS, y0 = y/SUBCELLS, z0 = z/SUBCELLS;
  int i, n=0; int possible[6];
  prev_dir = dir;
  for (i = 0; i<6; i++)
  {
    int x1 = x0 + direct[i].x;
    int y1 = y0 + direct[i].y;
    int z1 = z0 + direct[i].z;
    if (!box->goodCubeLocation(x1,y1,z1)) continue;
    if (mayExtend && i == dir && rnd->getLong(CUBESIZE) != 0) goto extend;
    possible[n++] = i;
  }
  if (n==0) return FALSE; // out of choices, start over
  dir = possible[rnd->getLong(n)];
extend:
  int x1 = x0 + direct[dir].x;
  int y1 = y0 + direct[dir].y;
  int z1 = z0 + direct[dir].z;
  box->occupyCubeLocation(x1,y1,z1);
  return TRUE; // ok, found place to go
}

void Pipe::choosePos()
{
  running = TRUE;
  for (;;)
  {
    int x1 = rnd->getLong(2*CUBESIZE+1)-CUBESIZE;
    int y1 = rnd->getLong(2*CUBESIZE+1)-CUBESIZE;
    int z1 = rnd->getLong(2*CUBESIZE+1)-CUBESIZE;
    if (!box->goodCubeLocation(x1,y1,z1)) continue; // occupied
    box->occupyCubeLocation(x1,y1,z1);
    x = x1*SUBCELLS; y = y1*SUBCELLS; z = z1*SUBCELLS;
    break;
  }
}

void kPipesSaver::reinit()
{ int i;
  xRot = rnd->getDouble()*360.0;
  yRot = rnd->getDouble()*360.0;
  zRot = rnd->getDouble()*360.0; // default object rotation
  steps = 0;
  running = pipes;
  initial = TRUE;
  clearCube();
  setColors(pipes);
  for (i = 0; i < pipes; i++) pipe[i]->choosePos();
  // Though we start out with a random direction
  // we can guarantee a correct direction setting
  // by the 'steps = 0' assignment above. This
  // will force 'chooseDir' in 'tick', which
  // makes 'dir' to be set correctly. The 'initial'
  // setting finally makes 'prev_dir==dir' so that
  // we get a small sphere at the start position.
}

// Handling locations in the cube //////////////////////////////////////////////

/*!
    Clear the cube
*/

void kPipesSaver::clearCube()
{ int x,y,z;
  for (x=0;x<2*CUBESIZE+1;x++)
  for (y=0;y<2*CUBESIZE+1;y++)
  for (z=0;z<2*CUBESIZE+1;z++)
  cube[x][y][z] = FALSE;
}

/*!
    Whether the location is in the cube and not occupied yet.
*/

bool kPipesSaver::goodCubeLocation(int x, int y, int z)
{
//printf("Try [%d,%d,%d]\n",x,y,z);
  return -CUBESIZE <= x && x <= CUBESIZE &&
         -CUBESIZE <= y && y <= CUBESIZE &&
         -CUBESIZE <= z && z <= CUBESIZE &&
         !cube[x+CUBESIZE][y+CUBESIZE][z+CUBESIZE];
}

/*!
    Occupy the location
*/

void kPipesSaver::occupyCubeLocation(int x, int y, int z)
{
//printf("occupy [%d,%d,%d]\n",x,y,z);
  cube[x+CUBESIZE][y+CUBESIZE][z+CUBESIZE] = TRUE;
}

// The Pipebox ////////////////////////////////////////////////////////////////

/*!
  Paint the box. This is done only initially. The animation is done in
  the paintStep routine.
*/

void kPipesSaver::paintGL()
{ int i;
  steps = 0;

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // clear

  glLoadIdentity();

  glTranslatef( 0.0, 0.0, -10.0 );
  glScalef( scale, scale, scale );

  glRotatef( xRot, 1.0, 0.0, 0.0 );
  glRotatef( yRot, 0.0, 1.0, 0.0 );
  glRotatef( zRot, 0.0, 0.0, 1.0 );

//glCallList( coord ); // show coordinate system

  for (i = 0; i < pipes; i++)
  {
    glTranslatef( RADIUS*pipe[i]->x, RADIUS*pipe[i]->y, RADIUS*pipe[i]->z );
    glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color[pipe[i]->col] );
    glCallList( start );
    glTranslatef( -RADIUS*pipe[i]->x, -RADIUS*pipe[i]->y, -RADIUS*pipe[i]->z );
  }
  glFlush();
}

void kPipesSaver::makeStep()
{ int i;
  for (i = 0; i < pipes; i++)
  if (pipe[i]->running)
  { Pipe* p = pipe[i];
    Direction* d = &direct[p->dir];
    p->x += d->x; p->y += d->y; p->z += d->z;
  }
}

void kPipesSaver::paintStep()
// paint the next arrow step
{ int i;
  for (i = 0; i < pipes; i++)
  { Pipe* p = pipe[i]; Direction* d = &direct[p->dir];

    glLoadIdentity();
    glTranslatef( 0.0, 0.0, -10.0 );


    glScalef( scale, scale, scale );
    glRotatef( xRot, 1.0, 0.0, 0.0 );
    glRotatef( yRot, 0.0, 1.0, 0.0 );
    glRotatef( zRot, 0.0, 0.0, 1.0 );

    glTranslatef( RADIUS*p->x, RADIUS*p->y, RADIUS*p->z );
    glRotatef( 90.0*d->deg90, 1.0*d->rx, 1.0*d->ry, 0.0 );
    glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color[pipe[i]->col] );
    if (p->running)
    {
      glCallList( arrow );
      if (p->dir != p->prev_dir)
        glCallList( rnd->getLong(3)==0?sphere1:sphere0 );
      p->prev_dir = p->dir;
    }
    else
    {
      glCallList( start ); //FIXME: stopped pipe is still drawing spheres
    }
  }
}

//

void kPipesSaver::tick()
{
  if (steps % SUBCELLS == 0)
  {
    for (int i = 0; i < pipes; i++)
      if (pipe[i]->running)
      if (!pipe[i]->chooseDir(TRUE))
      {
        pipe[i]->running = FALSE;
        running -= 1;
      }
  }

  if (running < (pipes+1)/2)
  {
    reinit();
    return; // start over
  }

  if (initial)
  {
    paintGL();
    for (int i = 0; i < pipes; i++)
      pipe[i]->prev_dir = pipe[i]->dir;
    initial = FALSE;
  }

  steps = steps+1;
  makeStep();
  paintStep();
  glFlush();
}

// Creation of the basic display elements ////////////////////////////////////

/*!
  Generate an OpenGL display list for the coordinate system.
  This is only for debugging purposes. The positive ends of
  the axis are twice as long as the negative ones.
*/

GLuint kPipesSaver::makeCoord()
{ GLuint list = glGenLists(1);
  glNewList(list, GL_COMPILE);
    glLineWidth( 2.0 );
    glBegin( GL_LINES );
      glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color[0] );
      glVertex3f(  2.0,  0.0,  0.0 );   glVertex3f(-1.0,  0.0,  0.0 );
      glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color[1] );
      glVertex3f(  0.0,  2.0,  0.0 );   glVertex3f( 0.0, -1.0,  0.0 );
      glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color[2] );
      glVertex3f(  0.0,  0.0,  2.0 );   glVertex3f( 0.0,  0.0, -1.0 );
    glEnd();
  glEndList();
  return list;
}

/*!
    The pipes are extended by a combination of a sphere and a cylinder.
    The cylinder's height and radius is the same as the radius of the
    sphere and one of it's ends is located at the aequator of the sphere.
    Later, we'll make the non-cylinder end of this combination to be
    the extendion of the pipe.
*/

GLuint kPipesSaver::makeArrow()
{
  GLUquadricObj *quadric = gluNewQuadric();  /* Initialize the Sphere */
  gluQuadricNormals(quadric, (GLenum) GLU_SMOOTH); /* we want normals */
  gluQuadricTexture(quadric, GL_FALSE); /* we want texture */

  GLuint list = glGenLists(1); /* create the call list */
  glNewList(list, GL_COMPILE);
    gluCylinder(quadric, RADIUS, RADIUS, RADIUS, DETAIL, DETAIL);
//  gluSphere(quadric, RADIUS, DETAIL, DETAIL);             /* draw the sphere of specified radius */
  glEndList();               // finish up the list
  gluDeleteQuadric(quadric); // free up the quadric
  return list;
}

GLuint kPipesSaver::makeSphere(float f, float trans)
{
  GLUquadricObj *quadric = gluNewQuadric();  /* Initialize the Sphere */
  gluQuadricNormals(quadric, (GLenum) GLU_SMOOTH); /* we want normals */
  gluQuadricTexture(quadric, GL_FALSE); /* we want texture */

  GLuint list = glGenLists(1); /* create the call list */
  glNewList(list, GL_COMPILE);
    glTranslatef( 0, 0, trans );
    gluSphere(quadric, f*RADIUS, DETAIL, DETAIL);          /* draw the sphere of specified radius */
  glEndList();               // finish up the list
  gluDeleteQuadric(quadric); // free up the quadric
  return list;
}

// K Specific Interface ///////////////////////////////////////////////////////

kPipesSaver::kPipesSaver( Drawable drawable ) : kScreenSaver( drawable )
{
  rnd = new KRandomSequence();
  initXLock( mGc );

  xRot = rnd->getDouble()*360.0;
  yRot = rnd->getDouble()*360.0;
  zRot = rnd->getDouble()*360.0;      // default object rotation
  scale = 1.0;                        // default object scale
  steps = 0;
  initial = TRUE;

  // Clear to background colour when exposed
  XSetWindowBackground(qt_xdisplay(), mDrawable,
                          BlackPixel(qt_xdisplay(), qt_xscreen()));

  initGL( mDrawable );

  static GLfloat pos[4] = {-5.0, -5.0, 10.0, 1.0 };
  glLightfv( GL_LIGHT0, GL_POSITION, pos );
  glEnable( GL_CULL_FACE );
  glEnable( GL_LIGHTING );
  glEnable( GL_LIGHT0 );
  glEnable( GL_AUTO_NORMAL );
  glEnable( GL_DEPTH_TEST );

  coord   = makeCoord();                // Generate an OpenGL display list
  arrow   = makeArrow();                // Generate an OpenGL display list
  start   = makeSphere(1.0, 0);         // Generate an OpenGL display list
  sphere0 = makeSphere(1.0,RADIUS);     // Generate an OpenGL display list
  sphere1 = makeSphere(1.3,RADIUS);     // Generate an OpenGL display list
  glShadeModel( GL_SMOOTH );
//glClearColor( 0.0, 0.0, 0.0, 0.0 ); // Let OpenGL clear to black

  int i; for (i = 0; i<MAXPIPES; i++) pipe[i] = new Pipe(this, i%MAXPIPES);

  readSettings();
  reinit();

  connect( &timer, SIGNAL( timeout() ), SLOT( tick() ) );
  timer.start( SPEED );
}

kPipesSaver::~kPipesSaver()
{
  timer.stop();
//FIXME: delete display lists
  doneGL();
  int i; for (i = 0; i<MAXPIPES; i++) delete pipe[i];
  delete rnd; rnd = 0;
}

// configuration support

void kPipesSaver::setPipes( int n )
{
  timer.stop();
  pipes = n;
  reinit();
  timer.start( SPEED  );
}

void kPipesSaver::readSettings()
{
  KConfig *config = klock_config();
  config->setGroup( "Settings" );

  pipes = config->readNumEntry( "Pipes", DEFPIPES );
  if (pipes < 2) pipes = 2;
  if (pipes > MAXPIPES) pipes = MAXPIPES;
  delete config;
}

// Setup //////////////////////////////////////////////////////////////////////

kPipesSetup::kPipesSetup( QWidget *parent, const char *name )
  : KDialogBase( parent, name, true, i18n("Setup Pipes Screen Saver"), Ok|Cancel|User1,
		Ok, false, i18n("About") )
{
  readSettings();

  QWidget *page = new QWidget(this);
  setMainWidget( page );
  QHBoxLayout *hb = new QHBoxLayout( page );
  QVBoxLayout *vb = new QVBoxLayout( hb );

  QLabel *label;
  QSlider *slider;

  label = new QLabel( i18n("Number of Pipes"), page );
  vb->addWidget( label );

  slider = new QSlider(2, MAXPIPES, 1, pipes, QSlider::Horizontal, page );
  vb->addWidget( slider );
  slider->setTickmarks(QSlider::Below);
  slider->setTickInterval(1);
  connect( slider, SIGNAL( valueChanged( int ) ), SLOT( slotPipes( int ) ) );

  vb->addStrut( 150 );
  vb->addStretch( 1 );

  preview = new QWidget( page );
  hb->addWidget( preview );
  preview->setFixedSize( 220, 170 );
  preview->setBackgroundColor( black );
  preview->show();    // otherwise saver does not get correct size
  saver = new kPipesSaver( preview->winId() );

  connect( this, SIGNAL( user1Clicked() ), SLOT( slotAbout() ) );
}

void kPipesSetup::readSettings()
{
  KConfig *config = klock_config();
  config->setGroup( "Settings" );

  pipes = config->readNumEntry( "Pipes", pipes );
  delete config;
}

void kPipesSetup::slotPipes( int num )
{
  pipes = num ;

  if ( saver )
    saver->setPipes( pipes );
}

void kPipesSetup::slotOk()
{
  KConfig *config = klock_config();
  config->setGroup( "Settings" );

  QString spipes;
  spipes.setNum( pipes );
  config->writeEntry( "Pipes", spipes );

  config->sync();
  delete config;
  accept();
}

void kPipesSetup::slotAbout()
{
  KMessageBox::about(this,
	i18n("KPipes\nCopyright (c) 1998-2000\n"
	   "Lars Doelle <lars.doelle@on-line.de>"));
}

#endif

