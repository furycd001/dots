/*
 *
 *            kStart OpenGL screensave for KDE
 *
 * $Id: space.cpp,v 1.2 2001/05/27 00:27:57 mhunter Exp $
 *
 *            Copyright (C) 1998 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
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
 * Based on star.c:
 *
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
 */

#define LONG64
//#define QT_CLEAN_NAMESPACE

#include <qslider.h>
#include <qlayout.h>
#include <kglobal.h>
#include <kconfig.h>
#include <krandomsequence.h>
#include <kdebug.h>
#include "xlock.h"
#include "helpers.h"
#ifdef HAVE_CONFIG_H
#include "../../config.h"
#endif

#ifdef HAVE_GL

#include <klocale.h>

#undef index
#include "space.h"
#include <math.h>
#include <X11/Intrinsic.h>
#ifdef HAVE_GL_XMESA_H
#include <GL/xmesa.h>
#endif
#include <GL/gl.h>
#include <GL/glx.h>
#ifdef HAVE_GL_GLUT_H
// We don't need GLUT, but some BROKEN GLU implemenations, such as the one
// used in SuSE Linux 6.3, do. :(
#include <GL/glut.h>
#endif
#include <GL/glu.h>

#ifndef PI
#ifdef M_PI
#define PI M_PI
#else
#define PI 3.141592657
#endif
#endif

enum {
    NORMAL = 0,
    WEIRD = 1
} flag = NORMAL;

enum {
    STREAK = 0,
    CIRCLE = 1
};

#define MAXSTARS 400
#define MAXPOS 10000
#define MAXWARP1 10
#define MAXANGLES 6000


typedef struct _starRec {
    GLint type;
    float x[2], y[2], z[2];
    float offsetX, offsetY, offsetR, rotation;
} starRec;


GLenum doubleBuffer, directRender;
GLint windW, windH;

GLint starCount = MAXSTARS / 2;
float speed = 1.0;
float warpinterval = 30000.0;
GLint nitro = 0;
starRec stars[MAXSTARS];
float sinTable[MAXANGLES];

static GLXContext  glx_context;
static KRandomSequence *rnd = 0;

float Sin(float angle)
{

    return (sinTable[(GLint)angle]);
}

float Cos(float angle)
{

    return (sinTable[((GLint)angle+(MAXANGLES/4))%MAXANGLES]);
}

void NewStar(GLint n, GLint d)
{

    if (rnd->getLong(4) == 0) {
	stars[n].type = CIRCLE;
    } else {
	stars[n].type = STREAK;
    }
    stars[n].x[0] = rnd->getDouble() * MAXPOS - MAXPOS / 2;
    stars[n].y[0] = rnd->getDouble() * MAXPOS - MAXPOS / 2;
    stars[n].z[0] = rnd->getDouble() * MAXPOS + d;
    if (rnd->getLong(4) == 0 && flag == WEIRD) {
	stars[n].offsetX = rnd->getDouble()* 100 - 100 / 2;
	stars[n].offsetY = rnd->getDouble()* 100 - 100 / 2;
	stars[n].offsetR = rnd->getDouble()* 25 - 25 / 2;
    } else {
	stars[n].offsetX = 0.0;
	stars[n].offsetY = 0.0;
	stars[n].offsetR = 0.0;
    }
}

void RotatePoint(float *x, float *y, float rotation)
{
    float tmpX, tmpY;

    tmpX = *x * Cos(rotation) - *y * Sin(rotation);
    tmpY = *y * Cos(rotation) + *x * Sin(rotation);
    *x = tmpX;
    *y = tmpY;
}

void MoveStars(void)
{
    float offset;
    GLint n;

    offset = speed * 60.0;

    for (n = 0; n < starCount; n++) {
	stars[n].x[1] = stars[n].x[0];
	stars[n].y[1] = stars[n].y[0];
	stars[n].z[1] = stars[n].z[0];
	stars[n].x[0] += stars[n].offsetX;
	stars[n].y[0] += stars[n].offsetY;
	stars[n].z[0] -= offset;
        stars[n].rotation += stars[n].offsetR;
        if (stars[n].rotation > MAXANGLES) {
            stars[n].rotation = 0.0;
	}
    }
}

GLenum StarPoint(GLint n)
{
    float x0, y0, x1, y1, width;
    GLint i;

    x0 = stars[n].x[0] * windW / stars[n].z[0];
    y0 = stars[n].y[0] * windH / stars[n].z[0];
    RotatePoint(&x0, &y0, stars[n].rotation);
    x0 += windW / 2.0;
    y0 += windH / 2.0;

    if (x0 >= 0.0 && x0 < windW && y0 >= 0.0 && y0 < windH) {
	if (stars[n].type == STREAK) {
	    x1 = stars[n].x[1] * windW / stars[n].z[1];
	    y1 = stars[n].y[1] * windH / stars[n].z[1];
	    RotatePoint(&x1, &y1, stars[n].rotation);
	    x1 += windW / 2.0;
	    y1 += windH / 2.0;

	    glLineWidth(MAXPOS/100.0/stars[n].z[0]+1.0);
	    glColor3f(1.0, (MAXWARP1-speed)/MAXWARP1, (MAXWARP1-speed)/MAXWARP1);
	    if (fabs(x0-x1) < 1.0 && fabs(y0-y1) < 1.0) {
		glBegin(GL_POINTS);
		    glVertex2f(x0, y0);
		glEnd();
	    } else {
		glBegin(GL_LINES);
		    glVertex2f(x0, y0);
		    glVertex2f(x1, y1);
		glEnd();
	    }
	} else {
	    width = MAXPOS / 10.0 / stars[n].z[0] + 1.0;
	    glColor3f(1.0, 0.0, 0.0);
	    glBegin(GL_POLYGON);
		for (i = 0; i < 8; i++) {
		    float x = x0 + width * Cos((float)i*MAXANGLES/8.0);
		    float y = y0 + width * Sin((float)i*MAXANGLES/8.0);
		    glVertex2f(x, y);
		};
	    glEnd();
	}
	return GL_TRUE;
    } else {
	return GL_FALSE;
    }
}

void ShowStars(void)
{
    GLint n;

    glClear(GL_COLOR_BUFFER_BIT);

    for (n = 0; n < starCount; n++) {
	if (stars[n].z[0] > speed || (stars[n].z[0] > 0.0 && speed < MAXWARP1)) {
	    if (StarPoint(n) == GL_FALSE) {
		NewStar(n, MAXPOS);
	    }
	} else {
	    NewStar(n, MAXPOS);
	}
    }
}

static void Init(void)
{
    float angle;
    GLint n;

    for (n = 0; n < MAXSTARS; n++) {
	NewStar(n, 100);
    }

    angle = 0.0;
    for (n = 0; n < MAXANGLES ; n++) {
	sinTable[n] = sin(angle);
        angle += PI / (MAXANGLES / 2.0);
    }

    glClearColor(0.0, 0.0, 0.0, 0.0);

    glDisable(GL_DITHER);
}

void reshape(int width, int height)
{

    windW = (GLint)width;
    windH = (GLint)height;

    glViewport(0, 0, windW, windH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-0.5, windW+0.5, -0.5, windH+0.5);
    glMatrixMode(GL_MODELVIEW);

}

void Idle(void)
{

    MoveStars();
    ShowStars();
    if (nitro > 0) {
	speed = (float)(nitro / 10) + 1.0;
	if (speed > MAXWARP1) {
	    speed = MAXWARP1;
	}
	if (++nitro > MAXWARP1*10) {
	    nitro = -nitro;
	}
    } else if (nitro < 0) {
	nitro++;
	speed = (float)(-nitro / 10) + 1.0;
	if (speed > MAXWARP1) {
	    speed = MAXWARP1;
	}
    }

    glFlush();

    /*    if (doubleBuffer) {
	tkSwapBuffers();
    }*/
}


void
drawSpace(Window /*window*/)
{

  /*
    Display    *display = dsp;
    //glXMakeCurrent(display, window, mp->glx_context);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glFlush();
    glXSwapBuffers(display, window);
  */

  Idle();

}


void release_Space(){

  glXDestroyContext(dsp, glx_context);

}

static XVisualInfo *glVis[MAXSCREENS];

int
getVisual(XVisualInfo * wantVis, int visual_count) {

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
        if (!glVis[screen]) {
                return (0);
        }

        /* check if GL can render into root window. */
        for(i=0;i<visual_count;i++)
                if ( (glVis[screen]->visual == (wantVis+i)->visual) )
                        return (1); // success

        // The visual we received did not match one we asked for
        return (0);
}


void
initSpace(Window window)
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
	    kdError() << i18n("GL can not render with root visual\n") << endl;
	    return;
	  }
	}

	/* PURIFY 3.0a on SunOS4 reports a 104 byte memory leak on the next line each
	 * time that morph3d mode is run in random mode. */

	glx_context = glXCreateContext(display, wantVis, 0, True);

	XFree((char *) wantVis);


	glXMakeCurrent(display, window, glx_context);
	glDrawBuffer(GL_FRONT);

	if (mono) {
	  glIndexi(WhitePixel(display, screen));
	  glClearIndex(BlackPixel(display, screen));
	}

	reshape(xwa.width, xwa.height);
	Init();
}




#endif

#define MINSPEED 1
#define MAXSPEED 100
#define DEFSPEED 50
#define MINWARP 1
#define MAXWARP 30
#define DEFWARP 2
#define WARPFACTOR 100
//-----------------------------------------------------------------------------

#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qcolor.h>

#include <kconfig.h>
#include <kmessagebox.h>

#include "space.moc"

#undef Below

static kSpaceSaver *saver = NULL;

void startScreenSaver( Drawable d )
{
	if ( saver )
		return;
	saver = new kSpaceSaver( d );
}

void stopScreenSaver()
{
	if ( saver )
		delete saver;
	saver = NULL;
}

int setupScreenSaver()
{
	kSpaceSetup dlg;

	return dlg.exec();
}

//-----------------------------------------------------------------------------

kSpaceSaver::kSpaceSaver( Drawable drawable ) : kScreenSaver( drawable )
{
	rnd = new KRandomSequence();
	readSettings();
	counter = (int)warpinterval *WARPFACTOR;

	colorContext = QColor::enterAllocContext();

	initXLock( mGc );
	initSpace( mDrawable );

	timer.start( speed );
	connect( &timer, SIGNAL( timeout() ), SLOT( slotTimeout() ) );
}

kSpaceSaver::~kSpaceSaver()
{
	timer.stop();
	release_Space();
	QColor::leaveAllocContext();
	QColor::destroyAllocContext( colorContext );
	delete rnd; rnd = 0;
}

void kSpaceSaver::setSpeed( int spd )
{
	timer.stop();
	speed = MAXSPEED - spd ;
	//	printf("speed %d\n",speed);
	timer.stop();
	timer.start( speed  );
}

void kSpaceSaver::setWarp( int  w )
{
        warpinterval = w;
	counter = (int)warpinterval;
	initSpace( mDrawable );
}

void kSpaceSaver::readSettings()
{
	KConfig *config = klock_config();
	config->setGroup( "Settings" );

	QString str;

	str = config->readEntry( "Speed" );
	if ( !str.isNull() )
		speed = MAXSPEED - str.toInt();
	else
		speed = DEFSPEED;

	warpinterval = config->readNumEntry( "WarpInterval", 15 );
	delete config;
}

void kSpaceSaver::slotTimeout()
{
  //printf("%d %d \n",(int)warpinterval, MAXWARP);
  if(warpinterval != MAXWARP){
     if(nitro == 0)
     counter -= speed +1;

     if(counter <= 0){
       nitro = 1;
       counter = (int) warpinterval *WARPFACTOR;
     }
  }
  else
    nitro = 0;

  drawSpace( mDrawable );
}

//-----------------------------------------------------------------------------

kSpaceSetup::kSpaceSetup( QWidget *parent, const char *name )
	: KDialogBase( parent, name, true, i18n("Setup Space Screen Saver"),
		       Ok|Cancel|User1, Ok, false, i18n("About") )
{
    readSettings();

    QWidget *page = new QWidget( this );
    setMainWidget( page );
    QHBoxLayout *hb = new QHBoxLayout( page, spacingHint() );
    QVBoxLayout *vb = new QVBoxLayout( hb, spacingHint() );

    QLabel *label;
    QSlider *slider;

    label = new QLabel( i18n("Speed:"), page );
    vb->addWidget( label );

    slider = new QSlider(MINSPEED, MAXSPEED, 10, speed, QSlider::Horizontal,
		    page );
    vb->addWidget( slider );
    slider->setTickmarks(QSlider::Below);
    slider->setTickInterval(10);
    connect( slider, SIGNAL( valueChanged( int ) ), SLOT( slotSpeed( int ) ) );

    vb->addSpacing( 10 );
    label = new QLabel( i18n("Warp Interval:"), page );
    vb->addWidget( label );

    slider = new QSlider(MINWARP, MAXWARP, 3, warpinterval,
		    QSlider::Horizontal, page );
    vb->addWidget( slider );
    slider->setTickmarks(QSlider::Below);
    slider->setTickInterval(3);
    connect( slider, SIGNAL( valueChanged( int ) ), SLOT( slotWarp( int ) ) );

    vb->addStrut( 150 );
    vb->addStretch( 1 );

    preview = new QWidget( page );
    hb->addWidget( preview );
    preview->setFixedSize( 220, 170 );
    preview->setBackgroundColor( black );
    preview->show();    // otherwise saver does not get correct size
    saver = new kSpaceSaver( preview->winId() );

    connect( this, SIGNAL( user1Clicked() ), SLOT( slotAbout() ) );
}

void kSpaceSetup::readSettings()
{
	KConfig *config = klock_config();
	config->setGroup( "Settings" );

	speed = config->readNumEntry( "Speed", speed );

	if ( speed > MAXSPEED )
		speed = MAXSPEED;
	else if ( speed < MINSPEED )
		speed = MINSPEED;

	warpinterval = config->readNumEntry( "WarpInterval", 15 );

	delete config;
}

void kSpaceSetup::slotSpeed( int num )
{
	speed = num ;

	if ( saver )
		saver->setSpeed( speed );
}

void kSpaceSetup::slotWarp( int num )
{
        warpinterval = num;
	if ( saver )
		saver->setWarp( warpinterval );
}

void kSpaceSetup::slotOk()
{
	KConfig *config = klock_config();
	config->setGroup( "Settings" );

	QString sspeed;
	sspeed.setNum( speed );
	config->writeEntry( "Speed", sspeed );

	QString interval;
	interval.setNum( (int)warpinterval );
	config->writeEntry( "WarpInterval", interval );

	config->sync();
	delete config;
	accept();
}

void kSpaceSetup::slotAbout()
{
	KMessageBox::about(this,
		i18n("KSpace\nCopyright (c) 1998\n"
				   "Bernd Johannes Wuebben <wuebben@kde.org>"));
}


/*
static GLenum Args(int argc, char **argv)
{
    GLint i;

    doubleBuffer = GL_FALSE;
    directRender = GL_TRUE;

    for (i = 1; i < argc; i++) {
	if (strcmp(argv[i], "-sb") == 0) {
	    doubleBuffer = GL_FALSE;
	} else if (strcmp(argv[i], "-db") == 0) {
	    doubleBuffer = GL_TRUE;
	} else if (strcmp(argv[i], "-dr") == 0) {
	    directRender = GL_TRUE;
	} else if (strcmp(argv[i], "-ir") == 0) {
	    directRender = GL_FALSE;
	}
    }
    return GL_TRUE;
}



void main(int argc, char **argv)
{
    GLenum type;

    if (Args(argc, argv) == GL_FALSE) {
	tkQuit();
    }

    windW = 300;
    windH = 300;
    tkInitPosition(0, 0, 300, 300);

    type = TK_RGB;
    type |= (doubleBuffer) ? TK_DOUBLE : TK_SINGLE;
    type |= (directRender) ? TK_DIRECT : TK_INDIRECT;
    tkInitDisplayMode(type);

    if (tkInitWindow("Stars") == GL_FALSE) {
	tkQuit();
    }

    Init();

    tkExposeFunc(Reshape);
    tkReshapeFunc(Reshape);
    tkKeyDownFunc(Key);
    tkIdleFunc(Idle);
    tkExec();
}

*/
