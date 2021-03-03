/*-
 * morph3d.c - Shows 3D morphing objects (XLock Version)
 *
 * See xlock.c for copying information.
 *
 * The original code for this mode was written by Marcelo Fernandes Vianna
 * (me...) and was inspired on a WindowsNT(R)'s screen saver. It was written
 * from scratch and it was not based on any other source code.
 *
 * Porting it to xlock (the final objective of this code since the moment I
 * decided to create it) was possible by comparing the original Mesa's gear
 * demo with it's ported version to xlock, so thanks for Danny Sung (look at
 * gear.c) for his indirect help.
 *
 * Thanks goes also to Brian Paul for making it possible and inexpensive
 * to use OpenGL at home.
 *
 * If you are interested in the original version of this program (not a xlock
 * mode, please refer to the Mesa package (ftp iris.ssec.wisc.edu on /pub/Mesa)
 *
 * Since I'm not a native english speaker, my apologies for any gramatical
 * mistake.
 *
 * My e-mail addresses are
 * vianna@cat.cbpf.br
 *         and
 * marcelo@venus.rdc.puc-rio.br
 *
 * Marcelo F. Vianna (Feb-13-1997)
 */

/* Ported to kscreensave:
   July 1997, Emanuel Pirker <epirker@edu.uni-klu.ac.at>
   Last revised: 12-Jul-97
   Please contact me in case of problems, not the original author!
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
#include "../../config.h"

#ifdef HAVE_GL

#include <klocale.h>

#include <math.h>
#include <X11/Intrinsic.h>

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





//ModeSpecOpt morph3d_opts = {0, NULL, 0, NULL, NULL};

#define Scale4Window               0.3
#define Scale4Iconic               1.0

#define glNormal3fMul(X1,Y1,Z1,X2,Y2,Z2) glNormal3f((Y1)*(Z2)-(Z1)*(Y2),(Z1)*(X2)-(X1)*(Z2),(X1)*(Y2)-(Y1)*(X2))
#define sqr(A)                     ((A)*(A))

/* Increasing this values produces better image quality, the price is speed. */
#define tetradivisions             23
#define cubedivisions              20
#define octadivisions              21
#define dodecadivisions            10
#define icodivisions               15

#define tetraangle                 109.47122063449069174
#define cubeangle                  90.000000000000000000
#define octaangle                  109.47122063449069174
#define dodecaangle                63.434948822922009981
#define icoangle                   41.810314895778596167

#ifndef Pi
#define Pi                         M_PI
#endif
#define SQRT2                      1.4142135623730951455
#define SQRT3                      1.7320508075688771932
#define SQRT5                      2.2360679774997898051
#define SQRT6                      2.4494897427831778813
#define SQRT15                     3.8729833462074170214
#define cossec36_2                 0.8506508083520399322
#define cos72                      0.3090169943749474241
#define sin72                      0.9510565162951535721
#define cos36                      0.8090169943749474241
#define sin36                      0.5877852522924731292

/*************************************************************************/

typedef struct {
	GLint       WindH, WindW;
	GLfloat     step;
	GLfloat     seno;
	int         object;
	int         edgedivisions;
        void        (*draw_object) ();
	float       Magnitude;
	float      *MaterialColor[20];
	GLXContext  glx_context;
} morph3dstruct;

static float front_shininess[] = {60.0};
static float front_specular[] = {0.7, 0.7, 0.7, 1.0};
static float ambient[] = {0.0, 0.0, 0.0, 1.0};
static float diffuse[] = {1.0, 1.0, 1.0, 1.0};
static float position0[] = {1.0, 1.0, 1.0, 0.0};
static float position1[] = {-1.0, -1.0, 1.0, 0.0};
static float lmodel_ambient[] = {0.5, 0.5, 0.5, 1.0};
static float lmodel_twoside[] = {GL_TRUE};

static float MaterialRed[] = {0.7, 0.0, 0.0, 1.0};
static float MaterialGreen[] = {0.1, 0.5, 0.2, 1.0};
static float MaterialBlue[] = {0.0, 0.0, 0.7, 1.0};
static float MaterialCyan[] = {0.2, 0.5, 0.7, 1.0};
static float MaterialYellow[] = {0.7, 0.7, 0.0, 1.0};
static float MaterialMagenta[] = {0.6, 0.2, 0.5, 1.0};
static float MaterialWhite[] = {0.7, 0.7, 0.7, 1.0};
static float MaterialGray[] = {0.2, 0.2, 0.2, 1.0};

static morph3dstruct *morph3d = NULL;

#define TRIANGLE(Edge, Amp, Divisions, Z) \
{                                         \
  GLfloat   Xf,Yf,Xa,Yb,Xf2,Yf2;          \
  GLfloat   Factor,Factor1,Factor2;       \
  GLfloat   VertX,VertY,VertZ,NeiAX,NeiAY,NeiAZ,NeiBX,NeiBY,NeiBZ; \
  GLfloat   Ax,Ay,Bx;                     \
  int       Ri,Ti;                        \
  GLfloat   Vr=(Edge)*SQRT3/3;            \
  GLfloat   AmpVr2=(Amp)/sqr(Vr);         \
  GLfloat   Zf=(Edge)*(Z);                \
                                          \
  Ax=(Edge)*(+0.5/(Divisions)), Ay=(Edge)*(-SQRT3/(2*Divisions));   \
  Bx=(Edge)*(-0.5/(Divisions));           \
                                          \
  for (Ri=1; Ri<=(Divisions); Ri++) {     \
    glBegin(GL_TRIANGLE_STRIP);           \
    for (Ti=0; Ti<Ri; Ti++) {             \
      Xf=(float)(Ri-Ti)*Ax + (float)Ti*Bx;      \
      Yf=Vr+(float)(Ri-Ti)*Ay + (float)Ti*Ay;   \
      Xa=Xf+0.001; Yb=Yf+0.001;                 \
      Factor=1-(((Xf2=sqr(Xf))+(Yf2=sqr(Yf)))*AmpVr2);  \
      Factor1=1-((sqr(Xa)+Yf2)*AmpVr2);                 \
      Factor2=1-((Xf2+sqr(Yb))*AmpVr2);                 \
      VertX=Factor*Xf;        VertY=Factor*Yf;        VertZ=Factor*Zf;      \
      NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Yf-VertY; NeiAZ=Factor1*Zf-VertZ; \
      NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ; \
      glNormal3fMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ);          \
      glVertex3f(VertX, VertY, VertZ);           \
                                                 \
      Xf=(float)(Ri-Ti-1)*Ax + (float)Ti*Bx;     \
      Yf=Vr+(float)(Ri-Ti-1)*Ay + (float)Ti*Ay;  \
      Xa=Xf+0.001; Yb=Yf+0.001;                  \
      Factor=1-(((Xf2=sqr(Xf))+(Yf2=sqr(Yf)))*AmpVr2); \
      Factor1=1-((sqr(Xa)+Yf2)*AmpVr2);          \
      Factor2=1-((Xf2+sqr(Yb))*AmpVr2);          \
      VertX=Factor*Xf;        VertY=Factor*Yf;        VertZ=Factor*Zf;        \
      NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Yf-VertY; NeiAZ=Factor1*Zf-VertZ; \
      NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ; \
      glNormal3fMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ);          \
      glVertex3f(VertX, VertY, VertZ);           \
                                                 \
    }                                            \
    Xf=(float)Ri*Bx;                             \
    Yf=Vr+(float)Ri*Ay;                          \
    Xa=Xf+0.001; Yb=Yf+0.001;                    \
    Factor=1-(((Xf2=sqr(Xf))+(Yf2=sqr(Yf)))*AmpVr2); \
    Factor1=1-((sqr(Xa)+Yf2)*AmpVr2);                \
    Factor2=1-((Xf2+sqr(Yb))*AmpVr2);                \
    VertX=Factor*Xf;        VertY=Factor*Yf;        VertZ=Factor*Zf;         \
    NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Yf-VertY; NeiAZ=Factor1*Zf-VertZ;  \
    NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ;  \
    glNormal3fMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ);           \
    glVertex3f(VertX, VertY, VertZ);                 \
    glEnd();                                      \
  }                                               \
}

#define SQUARE(Edge, Amp, Divisions, Z)           \
{                                                 \
  int       Xi,Yi;                                \
  GLfloat   Xf,Yf,Y,Xf2,Yf2,Y2,Xa,Yb;             \
  GLfloat   Factor,Factor1,Factor2;               \
  GLfloat   VertX,VertY,VertZ,NeiAX,NeiAY,NeiAZ,NeiBX,NeiBY,NeiBZ; \
  GLfloat   Zf=(Edge)*(Z);                        \
  GLfloat   AmpVr2=(Amp)/sqr((Edge)*SQRT2/2);     \
                                                  \
  for (Yi=0; Yi<(Divisions); Yi++) {              \
    Yf=-((Edge)/2.0) + ((float)Yi)/(Divisions)*(Edge);   \
    Yf2=sqr(Yf);                                  \
    Y=Yf+1.0/(Divisions)*(Edge);                  \
    Y2=sqr(Y);                                    \
    glBegin(GL_QUAD_STRIP);                       \
    for (Xi=0; Xi<=(Divisions); Xi++) {           \
      Xf=-((Edge)/2.0) + ((float)Xi)/(Divisions)*(Edge); \
      Xf2=sqr(Xf);                                \
                                                  \
      Xa=Xf+0.001; Yb=Y+0.001;                    \
      Factor=1-((Xf2+Y2)*AmpVr2);                 \
      Factor1=1-((sqr(Xa)+Y2)*AmpVr2);            \
      Factor2=1-((Xf2+sqr(Yb))*AmpVr2);           \
      VertX=Factor*Xf;        VertY=Factor*Y;         VertZ=Factor*Zf;   \
      NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Y-VertY;  NeiAZ=Factor1*Zf-VertZ; \
      NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ; \
      glNormal3fMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ);          \
      glVertex3f(VertX, VertY, VertZ);                                        \
                                                                              \
      Xa=Xf+0.001; Yb=Yf+0.001;                                               \
      Factor=1-((Xf2+Yf2)*AmpVr2);                                            \
      Factor1=1-((sqr(Xa)+Yf2)*AmpVr2);                                       \
      Factor2=1-((Xf2+sqr(Yb))*AmpVr2);                                       \
      VertX=Factor*Xf;        VertY=Factor*Yf;        VertZ=Factor*Zf;        \
      NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Yf-VertY; NeiAZ=Factor1*Zf-VertZ; \
      NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ; \
      glNormal3fMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ);          \
      glVertex3f(VertX, VertY, VertZ);                                        \
    }                                                                         \
    glEnd();                                                                  \
  }                                                                           \
}

#define PENTAGON(Edge, Amp, Divisions, Z)                                    \
{                                                                            \
  int       Ri,Ti,Fi;                                                        \
  GLfloat   Xf,Yf,Xa,Yb,Xf2,Yf2;                                             \
  GLfloat   x[6],y[6];                                                       \
  GLfloat   Factor,Factor1,Factor2;                                          \
  GLfloat   VertX,VertY,VertZ,NeiAX,NeiAY,NeiAZ,NeiBX,NeiBY,NeiBZ;           \
  GLfloat   Zf=(Edge)*(Z);                                                   \
  GLfloat   AmpVr2=(Amp)/sqr((Edge)*cossec36_2);                             \
                                                                             \
  for(Fi=0;Fi<6;Fi++) {                                                      \
    x[Fi]=-cos( Fi*2*Pi/5 + Pi/10 )/(Divisions)*cossec36_2*(Edge);           \
    y[Fi]=sin( Fi*2*Pi/5 + Pi/10 )/(Divisions)*cossec36_2*(Edge);            \
  }                                                                          \
                                                                             \
  for (Ri=1; Ri<=(Divisions); Ri++) {                                        \
    for (Fi=0; Fi<5; Fi++) {                                                 \
      glBegin(GL_TRIANGLE_STRIP);                                            \
      for (Ti=0; Ti<Ri; Ti++) {                                              \
        Xf=(float)(Ri-Ti)*x[Fi] + (float)Ti*x[Fi+1];                         \
        Yf=(float)(Ri-Ti)*y[Fi] + (float)Ti*y[Fi+1];                         \
        Xa=Xf+0.001; Yb=Yf+0.001;                                            \
	Factor=1-(((Xf2=sqr(Xf))+(Yf2=sqr(Yf)))*AmpVr2);                     \
	Factor1=1-((sqr(Xa)+Yf2)*AmpVr2);                                    \
	Factor2=1-((Xf2+sqr(Yb))*AmpVr2);                                    \
        VertX=Factor*Xf;        VertY=Factor*Yf;        VertZ=Factor*Zf;     \
        NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Yf-VertY; NeiAZ=Factor1*Zf-VertZ;  \
        NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ;  \
        glNormal3fMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ);       \
	glVertex3f(VertX, VertY, VertZ);                                     \
                                                                             \
        Xf=(float)(Ri-Ti-1)*x[Fi] + (float)Ti*x[Fi+1];                       \
        Yf=(float)(Ri-Ti-1)*y[Fi] + (float)Ti*y[Fi+1];                       \
        Xa=Xf+0.001; Yb=Yf+0.001;                                            \
	Factor=1-(((Xf2=sqr(Xf))+(Yf2=sqr(Yf)))*AmpVr2);                     \
	Factor1=1-((sqr(Xa)+Yf2)*AmpVr2);                                    \
	Factor2=1-((Xf2+sqr(Yb))*AmpVr2);                                    \
        VertX=Factor*Xf;        VertY=Factor*Yf;        VertZ=Factor*Zf;     \
        NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Yf-VertY; NeiAZ=Factor1*Zf-VertZ; \
        NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ; \
        glNormal3fMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ);       \
	glVertex3f(VertX, VertY, VertZ);                                     \
                                                                             \
      }                                                                      \
      Xf=(float)Ri*x[Fi+1];                                                  \
      Yf=(float)Ri*y[Fi+1];                                                  \
      Xa=Xf+0.001; Yb=Yf+0.001;                                              \
      Factor=1-(((Xf2=sqr(Xf))+(Yf2=sqr(Yf)))*AmpVr2);                       \
      Factor1=1-((sqr(Xa)+Yf2)*AmpVr2);                                      \
      Factor2=1-((Xf2+sqr(Yb))*AmpVr2);                                      \
      VertX=Factor*Xf;        VertY=Factor*Yf;        VertZ=Factor*Zf;       \
      NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Yf-VertY; NeiAZ=Factor1*Zf-VertZ;\
      NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ;\
      glNormal3fMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ);         \
      glVertex3f(VertX, VertY, VertZ);                                       \
      glEnd();                                                               \
    }                                                                        \
  }                                                                          \
}

static void
draw_tetra()
{
	GLuint      list;

	morph3dstruct *mp = &morph3d[screen];

	list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	TRIANGLE(2, mp->seno, mp->edgedivisions, 0.5 / SQRT6);
	glEndList();

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[0]);
	glCallList(list);
	glPushMatrix();
	glRotatef(180, 0, 0, 1);
	glRotatef(-tetraangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[1]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + tetraangle, 0.5, SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[2]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + tetraangle, 0.5, -SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[3]);
	glCallList(list);

	glDeleteLists(list, 1);
}

static void
draw_cube()
{
	GLuint      list;

	morph3dstruct *mp = &morph3d[screen];

	list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	SQUARE(2, mp->seno, mp->edgedivisions, 0.5)
		glEndList();

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[0]);
	glCallList(list);
	glRotatef(cubeangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[1]);
	glCallList(list);
	glRotatef(cubeangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[2]);
	glCallList(list);
	glRotatef(cubeangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[3]);
	glCallList(list);
	glRotatef(cubeangle, 0, 1, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[4]);
	glCallList(list);
	glRotatef(2 * cubeangle, 0, 1, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[5]);
	glCallList(list);

	glDeleteLists(list, 1);
}

static void
draw_octa()
{
	GLuint      list;

	morph3dstruct *mp = &morph3d[screen];

	list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	TRIANGLE(2, mp->seno, mp->edgedivisions, 1 / SQRT6);
	glEndList();

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[0]);
	glCallList(list);
	glPushMatrix();
	glRotatef(180, 0, 0, 1);
	glRotatef(-180 + octaangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[1]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-octaangle, 0.5, SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[2]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-octaangle, 0.5, -SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[3]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[4]);
	glCallList(list);
	glPushMatrix();
	glRotatef(180, 0, 0, 1);
	glRotatef(-180 + octaangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[5]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-octaangle, 0.5, SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[6]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-octaangle, 0.5, -SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[7]);
	glCallList(list);

	glDeleteLists(list, 1);
}

static void
draw_dodeca()
{
	GLuint      list;

	morph3dstruct *mp = &morph3d[screen];

#define TAU ((SQRT5+1)/2)

	list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	PENTAGON(1, mp->seno, mp->edgedivisions, sqr(TAU) * sqrt((TAU + 2) / 5) / 2);
	glEndList();

	glPushMatrix();
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[0]);
	glCallList(list);
	glRotatef(180, 0, 0, 1);
	glPushMatrix();
	glRotatef(-dodecaangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[1]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(-dodecaangle, cos72, sin72, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[2]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(-dodecaangle, cos72, -sin72, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[3]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(dodecaangle, cos36, -sin36, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[4]);
	glCallList(list);
	glPopMatrix();
	glRotatef(dodecaangle, cos36, sin36, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[5]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[6]);
	glCallList(list);
	glRotatef(180, 0, 0, 1);
	glPushMatrix();
	glRotatef(-dodecaangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[7]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(-dodecaangle, cos72, sin72, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[8]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(-dodecaangle, cos72, -sin72, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[9]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(dodecaangle, cos36, -sin36, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[10]);
	glCallList(list);
	glPopMatrix();
	glRotatef(dodecaangle, cos36, sin36, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[11]);
	glCallList(list);

	glDeleteLists(list, 1);
}

static void
draw_icosa()
{
	GLuint      list;

	morph3dstruct *mp = &morph3d[screen];

	list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	TRIANGLE(1.5, mp->seno, mp->edgedivisions, (3 * SQRT3 + SQRT15) / 12);
	glEndList();

	glPushMatrix();

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[0]);
	glCallList(list);
	glPushMatrix();
	glRotatef(180, 0, 0, 1);
	glRotatef(-icoangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[1]);
	glCallList(list);
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[2]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, -SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[3]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[4]);
	glCallList(list);
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[5]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 0, 0, 1);
	glRotatef(-icoangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[6]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, -SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[7]);
	glCallList(list);
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, -SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[8]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 0, 0, 1);
	glRotatef(-icoangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[9]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[10]);
	glCallList(list);
	glPushMatrix();
	glRotatef(180, 0, 0, 1);
	glRotatef(-icoangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[11]);
	glCallList(list);
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[12]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, -SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[13]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[14]);
	glCallList(list);
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[15]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 0, 0, 1);
	glRotatef(-icoangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[16]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, -SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[17]);
	glCallList(list);
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, -SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[18]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 0, 0, 1);
	glRotatef(-icoangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[19]);
	glCallList(list);

	glDeleteLists(list, 1);
}

void
drawmorph3d(Window window)
{
	morph3dstruct *mp = &morph3d[screen];

	Display    *display = dsp;

	glXMakeCurrent(display, window, mp->glx_context);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	glTranslatef(0.0, 0.0, -10.0);

	glScalef(Scale4Window * mp->WindH / mp->WindW, Scale4Window, Scale4Window);
	glTranslatef(2.5 * mp->WindW / mp->WindH * sin(mp->step * 1.11), 2.5 *
		     cos(mp->step * 1.25 * 1.11), 0);

	glRotatef(mp->step * 100, 1, 0, 0);
	glRotatef(mp->step * 95, 0, 1, 0);
	glRotatef(mp->step * 90, 0, 0, 1);

	mp->seno = (sin(mp->step) + 1.0 / 3.0) * (4.0 / 5.0) * mp->Magnitude;
	mp->draw_object();

	glPopMatrix();

	glFlush();

	glXSwapBuffers(display, window);

	mp->step += 0.05;
}

static void
reshape(int width, int height)
{
	morph3dstruct *mp = &morph3d[screen];

	glViewport(0, 0, mp->WindW = (GLint) width, mp->WindH = (GLint) height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 5.0, 15.0);
	glMatrixMode(GL_MODELVIEW);
}

static void
pinit()
{
	morph3dstruct *mp = &morph3d[screen];

	glClearDepth(1.0);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glColor3f(1.0, 1.0, 1.0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position0);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position1);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, front_shininess);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, front_specular);

	switch (mp->object) {
		case 2:
			mp->draw_object = draw_cube;
			mp->MaterialColor[0] = MaterialRed;
			mp->MaterialColor[1] = MaterialGreen;
			mp->MaterialColor[2] = MaterialCyan;
			mp->MaterialColor[3] = MaterialMagenta;
			mp->MaterialColor[4] = MaterialYellow;
			mp->MaterialColor[5] = MaterialBlue;
			mp->edgedivisions = cubedivisions;
			mp->Magnitude = 2.0;
			break;
		case 3:
			mp->draw_object = draw_octa;
			mp->MaterialColor[0] = MaterialRed;
			mp->MaterialColor[1] = MaterialGreen;
			mp->MaterialColor[2] = MaterialBlue;
			mp->MaterialColor[3] = MaterialWhite;
			mp->MaterialColor[4] = MaterialCyan;
			mp->MaterialColor[5] = MaterialMagenta;
			mp->MaterialColor[6] = MaterialGray;
			mp->MaterialColor[7] = MaterialYellow;
			mp->edgedivisions = octadivisions;
			mp->Magnitude = 2.5;
			break;
		case 4:
			mp->draw_object = draw_dodeca;
			mp->MaterialColor[0] = MaterialRed;
			mp->MaterialColor[1] = MaterialGreen;
			mp->MaterialColor[2] = MaterialCyan;
			mp->MaterialColor[3] = MaterialBlue;
			mp->MaterialColor[4] = MaterialMagenta;
			mp->MaterialColor[5] = MaterialYellow;
			mp->MaterialColor[6] = MaterialGreen;
			mp->MaterialColor[7] = MaterialCyan;
			mp->MaterialColor[8] = MaterialRed;
			mp->MaterialColor[9] = MaterialMagenta;
			mp->MaterialColor[10] = MaterialBlue;
			mp->MaterialColor[11] = MaterialYellow;
			mp->edgedivisions = dodecadivisions;
			mp->Magnitude = 2.0;
			break;
		case 5:
			mp->draw_object = draw_icosa;
			mp->MaterialColor[0] = MaterialRed;
			mp->MaterialColor[1] = MaterialGreen;
			mp->MaterialColor[2] = MaterialBlue;
			mp->MaterialColor[3] = MaterialCyan;
			mp->MaterialColor[4] = MaterialYellow;
			mp->MaterialColor[5] = MaterialMagenta;
			mp->MaterialColor[6] = MaterialRed;
			mp->MaterialColor[7] = MaterialGreen;
			mp->MaterialColor[8] = MaterialBlue;
			mp->MaterialColor[9] = MaterialWhite;
			mp->MaterialColor[10] = MaterialCyan;
			mp->MaterialColor[11] = MaterialYellow;
			mp->MaterialColor[12] = MaterialMagenta;
			mp->MaterialColor[13] = MaterialRed;
			mp->MaterialColor[14] = MaterialGreen;
			mp->MaterialColor[15] = MaterialBlue;
			mp->MaterialColor[16] = MaterialCyan;
			mp->MaterialColor[17] = MaterialYellow;
			mp->MaterialColor[18] = MaterialMagenta;
			mp->MaterialColor[19] = MaterialGray;
			mp->edgedivisions = icodivisions;
			mp->Magnitude = 2.5;
			break;
		default:
			mp->draw_object = draw_tetra;
			mp->MaterialColor[0] = MaterialRed;
			mp->MaterialColor[1] = MaterialGreen;
			mp->MaterialColor[2] = MaterialBlue;
			mp->MaterialColor[3] = MaterialWhite;
			mp->edgedivisions = tetradivisions;
			mp->Magnitude = 2.5;
			break;
	}
	if (mono) {
		int         loop;

		for (loop = 0; loop < 20; loop++)
			mp->MaterialColor[loop] = MaterialGray;
	}
}

static XVisualInfo *glVis[MAXSCREENS];

int
getVisual(XVisualInfo * wantVis, int visual_count)
{
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
                        int         attribList[] = {GLX_DOUBLEBUFFER, None};

                        glVis[screen] = glXChooseVisual(display, screen, attribList);
                } else {
                        int         attribList[] =
                        {GLX_RGBA, GLX_DOUBLEBUFFER, GLX_DEPTH_SIZE, 1, None};

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
initmorph3d(Window window)
{
	Display    *display = dsp;
	morph3dstruct *mp;
	XWindowAttributes xwa;
	KRandomSequence rnd;

	(void) XGetWindowAttributes(dsp, window, &xwa);

	if (morph3d == NULL) {
		if ((morph3d = (morph3dstruct *) calloc(ScreenCount(dsp),
					    sizeof (morph3dstruct))) == NULL)
			return;
	}
	mp = &morph3d[screen];
	mp->step = rnd.getLong(90);

	if (mp->glx_context) {
		glXDestroyContext(display, mp->glx_context);
		mp->glx_context = NULL;
	} {
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
		mp->glx_context = glXCreateContext(display, wantVis, 0, True);
		XFree((char *) wantVis);
	}

	glXMakeCurrent(display, window, mp->glx_context);

	if (mono) {
		glIndexi(WhitePixel(display, screen));
		glClearIndex(BlackPixel(display, screen));
	}

	reshape(xwa.width, xwa.height);
	mp->object = batchcount;
	if (mp->object <= 0 || mp->object > 5)
		mp->object = rnd.getLong(5) + 1;
	pinit();
}

void
change_morph3d()
{
	morph3dstruct *mp = &morph3d[screen];

	mp->object = (mp->object) % 5 + 1;
	pinit();
}

void
release_morph3d()
{
	if (morph3d != NULL) {
		int         screen;

		for (screen = 0; screen < 1; screen++) {
			morph3dstruct *mp = &morph3d[screen];

			if (mp->glx_context)
				glXDestroyContext(dsp, mp->glx_context);
#if 0 /* This is wrong for multiscreens anyway */
#ifdef GLX_MESA_release_buffers
			glXReleaseBuffersMESA(dsp, win);
#endif
#endif
		}
		(void) free((void *) morph3d);
		morph3d = NULL;
	}
}

#endif

#define MINSPEED 0
#define MAXSPEED 100
#define DEFSPEED 100
#define MINBATCH 1
#define MAXBATCH 5
#define DEFBATCH 2

//-----------------------------------------------------------------------------

#undef index

#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qcolor.h>

#include <kconfig.h>
#include <kmessagebox.h>

#include "morph3d.h"
#include "morph3d.moc"

#undef Below    // X sux

static kMorph3dSaver *saver = NULL;

void startScreenSaver( Drawable d )
{
	if ( saver )
		return;
	saver = new kMorph3dSaver( d );
}

void stopScreenSaver()
{
	if ( saver )
		delete saver;
	saver = NULL;
}

int setupScreenSaver()
{
	kMorph3dSetup dlg;

	return dlg.exec();
}

//-----------------------------------------------------------------------------

kMorph3dSaver::kMorph3dSaver( Drawable drawable ) : kScreenSaver( drawable )
{
	readSettings();

    // Clear to background colour when exposed
    XSetWindowBackground(qt_xdisplay(), mDrawable,
                            BlackPixel(qt_xdisplay(), qt_xscreen()));

	colorContext = QColor::enterAllocContext();

	batchcount = maxLevels;

	initXLock( mGc );
	initmorph3d( mDrawable );

	timer.start( speed );
	connect( &timer, SIGNAL( timeout() ), SLOT( slotTimeout() ) );
}

kMorph3dSaver::~kMorph3dSaver()
{
	timer.stop();
	release_morph3d();
	QColor::leaveAllocContext();
	QColor::destroyAllocContext( colorContext );
}

void kMorph3dSaver::setSpeed( int spd )
{
	timer.stop();
	speed = MAXSPEED - spd;
	timer.start( speed );
}

void kMorph3dSaver::setLevels( int l )
{
	batchcount = maxLevels = l;
	initmorph3d( mDrawable );
}

void kMorph3dSaver::readSettings()
{
	KConfig *config = klock_config();
	config->setGroup( "Settings" );

	QString str;

	str = config->readEntry( "Speed" );
	if ( !str.isNull() )
		speed = MAXSPEED - str.toInt();
	else
		speed = DEFSPEED;

	maxLevels = config->readNumEntry( "MaxLevels", DEFBATCH );
	// CC: fixed MaxLevels <-> ObjectType inconsistency

	delete config;
}

void kMorph3dSaver::slotTimeout()
{
	drawmorph3d( mDrawable );
}

//-----------------------------------------------------------------------------

kMorph3dSetup::kMorph3dSetup( QWidget *parent, const char *name )
	: KDialogBase( parent, name, true, i18n("Setup Morph3D Screen Saver"),
			Ok|Cancel|User1, Ok, false, i18n("About") )
{
    readSettings();

    showButton( User1, true );

    QWidget *page = new QWidget( this );
    setMainWidget( page );
    QHBoxLayout *hl = new QHBoxLayout( page, spacingHint() );

    QVBoxLayout *vb = new QVBoxLayout( hl, spacingHint() );

    QLabel *label;
    QSlider *slider;

    label = new QLabel( i18n("Speed:"), page );
    vb->addWidget( label );

    slider = new QSlider(MINSPEED, MAXSPEED, 10, speed, QSlider::Horizontal,
                        page);
    vb->addWidget( slider );
    slider->setTickmarks(QSlider::Below);
    slider->setTickInterval(10);
    connect( slider, SIGNAL( valueChanged( int ) ), SLOT( slotSpeed( int ) ) );

    label = new QLabel( i18n("Object Type:"), page );
    vb->addWidget( label );

    slider = new QSlider(MINBATCH, MAXBATCH, 1, maxLevels,
			QSlider::Horizontal, page);
    vb->addWidget( slider );
    slider->setTickmarks(QSlider::Below);
    slider->setTickInterval(1);
    connect( slider, SIGNAL( valueChanged( int ) ), SLOT( slotLevels( int ) ) );

    vb->addStrut( 150 );
    vb->addStretch( 1 );

    preview = new QWidget( page );
    hl->addWidget( preview );
    preview->setFixedSize( 220, 170 );
    preview->setBackgroundColor( black );
    preview->show();    // otherwise saver does not get correct size
    saver = new kMorph3dSaver( preview->winId() );

    connect( this, SIGNAL( user1Clicked() ), SLOT( slotAbout() ) );
}

void kMorph3dSetup::readSettings()
{
	KConfig *config = klock_config();
	config->setGroup( "Settings" );

	speed = config->readNumEntry( "Speed", speed );
	if ( speed > MAXSPEED )
		speed = MAXSPEED;
	else if ( speed < MINSPEED )
		speed = MINSPEED;

	maxLevels = config->readNumEntry( "MaxLevels", DEFBATCH );
	// CC: fixed MaxLevels <-> ObjectType inconsistency

	delete config;
}

void kMorph3dSetup::slotSpeed( int num )
{
	speed = num;

	if ( saver )
		saver->setSpeed( speed );
}

void kMorph3dSetup::slotLevels( int num )
{
	maxLevels = num;

	if ( saver )
		saver->setLevels( maxLevels );
}

void kMorph3dSetup::slotOk()
{
	KConfig *config = klock_config();
	config->setGroup( "Settings" );

	QString sspeed;
	sspeed.setNum( speed );
	config->writeEntry( "Speed", sspeed );

	QString slevels;
	slevels.setNum( maxLevels );
	config->writeEntry( "MaxLevels", slevels );

	config->sync();
	delete config;
	accept();
}

void kMorph3dSetup::slotAbout()
{
	KMessageBox::about(this,
			     i18n("Morph3D\n\nCopyright (c) 1997 by Marcelo F. Vianna\n\nPorted to kscreensave by Emanuel Pirker."));
}


