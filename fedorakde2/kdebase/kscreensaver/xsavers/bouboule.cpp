/* bouboule --- glob of spheres twisting and changing size */

/*-
 * Copyright 1996 by Jeremie PETIT <petit@eurecom.fr>, <jpetit@essi.fr>
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation.
 *
 * This file is provided AS IS with no warranties of any kind.  The author
 * shall have no liability with respect to the infringement of copyrights,
 * trade secrets or any patents by this file or any part thereof.  In no
 * event will the author be liable for any lost revenue or profits or
 * other special, indirect and consequential damages.
 */

/* Ported to kscreensave:
   April 1998, Cedric Tefft <cedric@earthling.net>
   In case of problems contact me, not the original author
*/
// layout management added 1998/04/19 by Mario Weilguni <mweilguni@kde.org>

#include <qslider.h>
#include <kglobal.h>
#include <kconfig.h>
#include <krandomsequence.h>

#include "xlock.h"		/* in xlockmore distribution */


#define USEOLDXARCS  1		/* If 1, we use old xarcs list for erasing.
				   * else we just roughly erase the window.
				   * This mainly depends on the number of stars,
				   * because when they are many, it is faster to
				   * erase the whole window than to erase each star
				 */

#if HAVE_GETTIMEOFDAY
#define ADAPT_ERASE  1		/* If 1, then we try ADAPT_CHECKS black XFillArcs,
				   * and after, ADAPT_CHECKS XFillRectangle.
				   * We check which method seems better, knowing that
				   * XFillArcs is generally visually better. So we
				   * consider that XFillArcs is still better if its time
				   * is about XFillRectangle * ADAPT_ARC_PREFERED
				   * We need gettimeofday
				   * for this... Does it exist on other systems ? Do we
				   * have to use another function for others ?
				   * This value overrides USEOLDXARCS.
				 */

#ifdef USE_XVMSUTILS
#if 0
#include "../xvmsutils/unix_time.h"
#else
#include <X11/unix_time.h>
#endif
#endif
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#if HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#endif
#define ADAPT_CHECKS 50
#define ADAPT_ARC_PREFERED 150	/* Maybe the value that is the most important
				   * for adapting to a system */
#endif

#define dtor(x)    (((x) * M_PI) / 180.0)	/* Degrees to radians */

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define MINSTARS      1
//#define colorchange 50	/* How often we change colors (1 = always)
//				   * This value should be tuned accordingly to
//				   * the number of stars */
#define MAX_SIZEX_SIZEY 2.	/* This controls whether the sphere can be very
				   * very large and have a small height (or the
				   * opposite) or no. */

#define THETACANRAND  80	/* percentage of changes for the speed of
				   * change of the 3 theta values */
#define SIZECANRAND   80	/* percentage of changes for the speed of
				   * change of the sizex and sizey values */
#define POSCANRAND    80	/* percentage of changes for the speed of
				   * change of the x and y values */
int maxsize;
int bouboule_initialized=0;
int colorchange;
bool use3d;

#define PARAM_INSTALLED 0
#define PARAM_ZDIFF    1.5

#define MAXSPEED 100
#define MINSPEED 0
#define DEFSPEED 50
#define MINPOINTS 1
#define MAXPOINTS 500
#define DEFPOINTS 100
#define MINSIZE   1
#define MAXSIZE   75
#define DEFSIZE  25
#define MAXCCSPEED 100
#define MINCCSPEED 1
#define DEFCCSPEED 50

/*-
 * Note that these XXXCANRAND values can be 0, that is no rand acceleration
 * variation.
 */

#define VARRANDALPHA (rnd->getDouble())
#define VARRANDSTEP  (M_PI/(rnd->getDouble()*100.0+100.0))
#define VARRANDMIN   (-70.0)
#define VARRANDMAX   70.0

#define MINZVAL   5		/* stars can come this close */
#define SCREENZ  2000		/* this is where the screen is */
#define MAXZVAL 10000		/* stars can go this far away */

#define GETZDIFF(z) ((PARAM_ZDIFF)*20.0*(1.0-(SCREENZ)/((z)+1000)))
#define MAXDIFF  MAX(-GETZDIFF(MINZVAL),GETZDIFF(MAXZVAL))

/*-
 * These values are the variation parameters of the acceleration variation
 * of the SinVariables that are randomized.
 */

/******************************/
typedef struct SinVariableStruct
/******************************/
{
	double      alpha;	/* 
				 * Alpha is the current state of the sinvariable
				 * alpha should be initialized to a value between
				 * 0.0 and 2 * M_PI
				 */
	double      step;	/*
				 * Speed of evolution of alpha. It should be a reasonable
				 * fraction of 2 * M_PI. This value directly influence
				 * the variable speed of variation.
				 */
	double      minimum;	/* Minimum value for the variable */
	double      maximum;	/* Maximum value for the variable */
	double      value;	/* Current value */
	int         mayrand;	/* Flag for knowing whether some randomization can be
				 * applied to the variable */
	struct SinVariableStruct *varrand;	/* Evolving Variable: the variation of
						   * alpha */
} SinVariable;

/***********************/
typedef struct StarStruct
/***********************/
{
	double      x, y, z;	/* Position of the star */
	short       size;	/* Try to guess */
} Star;

/****************************/
typedef struct StarFieldStruct
/****************************/
{
	short       width, height;	/* width and height of the starfield window */
	short       max_star_size;	/* Maximum radius for stars. stars radius will
					 * vary from 1 to MAX_STAR_SIZE */
	SinVariable x;		/* Evolving variables:               */
	SinVariable y;		/* Center of the field on the screen */
	SinVariable z;
	SinVariable sizex;	/* Evolving variable: half width  of the field */
	SinVariable sizey;	/* Evolving variable: half height of the field */
	SinVariable thetax;	/* Evolving Variables:              */
	SinVariable thetay;	/* rotation angles of the starfield */
	SinVariable thetaz;	/* around x, y and z local axis     */
	Star       *star;	/* List of stars */
	XArc       *xarc;	/* Current List of arcs */
	XArc       *xarcleft;	/* additional list for the left arcs */
#if ((USEOLDXARCS == 1) || (ADAPT_ERASE == 1))
	XArc       *oldxarc;	/* Old list of arcs */
	XArc       *oldxarcleft;
#endif
	unsigned long color;	/* Current color of the starfield */
	int         colorp;	/* Pointer to color of the starfield */
	int         NbStars;	/* Number of stars */
	short       colorchange;	/* Counter for the color change */
#if (ADAPT_ERASE == 1)
	short       hasbeenchecked;
	long        rect_time;
	long        xarc_time;
#endif
} StarField;

static KRandomSequence *rnd = 0;

static StarField starfield;

/*********/
static void
sinvary(SinVariable * v)
/*********/

{
	v->value = v->minimum +
		(v->maximum - v->minimum) * (sin(v->alpha) + 1.0) / 2.0;

	if (v->mayrand == 0)
		v->alpha += v->step;
	else {
		int         vaval = rnd->getLong(100);

		if (vaval <= v->mayrand)
			sinvary(v->varrand);
		v->alpha += (100.0 + (v->varrand->value)) * v->step / 100.0;
	}

	if (v->alpha > 2 * M_PI)
		v->alpha -= 2 * M_PI;
}

/*************************************************/
static void
sininit(SinVariable * v,
	double alpha, double step, double minimum, double maximum,
	short int mayrand)
{
	v->alpha = alpha;
	v->step = step;
	v->minimum = minimum;
	v->maximum = maximum;
	v->mayrand = mayrand;
	if (mayrand != 0) {
		if (v->varrand == NULL)
			v->varrand = (SinVariable *) calloc(1, sizeof (SinVariable));
		sininit(v->varrand,
			VARRANDALPHA,
			VARRANDSTEP,
			VARRANDMIN,
			VARRANDMAX,
			0);
		sinvary(v->varrand);
	}
	/* We calculate the values at least once for initialization */
	sinvary(v);
}

static void
sinfree(SinVariable * point)
{
	SinVariable *temp, *next;

	next = point->varrand;
	while (next) {
		temp = next;
		next = temp->varrand;
		(void) free((void *) temp);
	}
	point->varrand=NULL;
	
}

/***************/
void
initbouboule(Window win)
/***************/

/*-
 *  The stars init part was first inspirated from the net3d game starfield
 * code.  But net3d starfield is not really 3d starfield, and I needed real 3d,
 * so only remains the net3d starfield initialization main idea, that is
 * the stars distribution on a sphere (theta and omega computing)
 */
{
 	int         size = maxsize;
	int         i;
	double      theta, omega;
        StarField *sp = &starfield;
        XWindowAttributes xwa;

	(void) XGetWindowAttributes(dsp, win, &xwa);

 	sp->width = xwa.width;
	sp->height = xwa.height;

	XSetForeground(dsp, Scr[screen].gc, BlackPixel(dsp, screen));
 	XFillRectangle(dsp, win, Scr[screen].gc, 0, 0, sp->width,sp->height);

	if (size < -MINSIZE)
		sp->max_star_size = rnd->getLong(-size - MINSIZE + 1) + MINSIZE;
	else if (size < MINSIZE)
		sp->max_star_size = MINSIZE;
	else
		sp->max_star_size = size;

	sp->NbStars = batchcount;
	if (sp->NbStars < -MINSTARS) {
		if (sp->star) {
			(void) free((void *) sp->star);
			sp->star = NULL;
		}
		if (sp->xarc) {
			(void) free((void *) sp->xarc);
			sp->xarc = NULL;
		}
		if (sp->xarcleft) {
			(void) free((void *) sp->xarcleft);
			sp->xarcleft = NULL;
		}
#if ((USEOLDXARCS == 1) || (ADAPT_ERASE == 1))
		if (sp->oldxarc) {
			(void) free((void *) sp->oldxarc);
			sp->oldxarc = NULL;
		}
		if (sp->oldxarcleft) {
			(void) free((void *) sp->oldxarcleft);
			sp->oldxarcleft = NULL;
		}
#endif
		sp->NbStars = rnd->getLong(-sp->NbStars - MINSTARS + 1) + MINSTARS;
	} else if (sp->NbStars < MINSTARS)
		sp->NbStars = MINSTARS;

	/* We get memory for lists of objects */
	if (sp->star == NULL)
		sp->star = (Star *) malloc(sp->NbStars * sizeof (Star));
	if (sp->xarc == NULL)
		sp->xarc = (XArc *) malloc(sp->NbStars * sizeof (XArc));
	if (use3d && sp->xarcleft == NULL)
		sp->xarcleft = (XArc *) malloc(sp->NbStars * sizeof (XArc));
#if ((USEOLDXARCS == 1) || (ADAPT_ERASE == 1))
	if (sp->oldxarc == NULL)
		sp->oldxarc = (XArc *) malloc(sp->NbStars * sizeof (XArc));
	if (use3d && sp->oldxarcleft == NULL)
		sp->oldxarcleft = (XArc *) malloc(sp->NbStars * sizeof (XArc));
#endif

	{
		/* We initialize evolving variables */
		sininit(&sp->x,
			rnd->getDouble() * M_PI, M_PI / (rnd->getDouble()*100.0 + 100.0),
			((double) sp->width) / 4.0,
			3.0 * ((double) sp->width) / 4.0,
			POSCANRAND);
		sininit(&sp->y,
			rnd->getDouble() * M_PI, M_PI / (rnd->getDouble()*100.0 + 100.0),
			((double) sp->height) / 4.0,
			3.0 * ((double) sp->height) / 4.0,
			POSCANRAND);

		/* for z, we have to ensure that the bouboule does not get behind */
		/* the eyes of the viewer.  His/Her eyes are at 0.  Because the */
		/* bouboule uses the x-radius for the z-radius, too, we have to */
		/* use the x-values. */
		sininit(&sp->z,
			rnd->getDouble() * M_PI, M_PI / (rnd->getDouble()*100.0 + 100.0),
			((double) sp->width / 2.0 + MINZVAL),
			((double) sp->width / 2.0 + MAXZVAL),
			POSCANRAND);


		sininit(&sp->sizex,
			rnd->getDouble() * M_PI, M_PI / (rnd->getDouble()*100.0 + 100.0),
			MIN(((double) sp->width) - sp->x.value,
			    sp->x.value) / 5.0,
			MIN(((double) sp->width) - sp->x.value,
			    sp->x.value),
			SIZECANRAND);

		sininit(&sp->sizey,
			rnd->getDouble() * M_PI, M_PI / (rnd->getDouble()*100.0 + 100.0),
			MAX(sp->sizex.value / MAX_SIZEX_SIZEY,
			    sp->sizey.maximum / 5.0),
			MIN(sp->sizex.value * MAX_SIZEX_SIZEY,
			    MIN(((double) sp->height) -
				sp->y.value,
				sp->y.value)),
			SIZECANRAND);

		sininit(&sp->thetax,
			rnd->getDouble() * M_PI, M_PI / (rnd->getDouble()*200.0 + 200.0),
			-M_PI, M_PI,
			THETACANRAND);
		sininit(&sp->thetay,
			rnd->getDouble() * M_PI, M_PI / (rnd->getDouble()*200.0 + 200.0),
			-M_PI, M_PI,
			THETACANRAND);
		sininit(&sp->thetaz,
			rnd->getDouble() * M_PI, M_PI / (rnd->getDouble()*400.0 + 400.0),
			-M_PI, M_PI,
			THETACANRAND);
	}
	for (i = 0; i < sp->NbStars; i++) {
		Star       *star;
		XArc       *arc = NULL, *arcleft = NULL;

#if ((USEOLDXARCS == 1) || (ADAPT_ERASE == 1))
		XArc       *oarc = NULL, *oarcleft = NULL;

#endif

		star = &(sp->star[i]);
		arc = &(sp->xarc[i]);
		if (use3d)
			arcleft = &(sp->xarcleft[i]);
#if ((USEOLDXARCS == 1) || (ADAPT_ERASE == 1))
		oarc = &(sp->oldxarc[i]);
		if (use3d)
			oarcleft = &(sp->oldxarcleft[i]);
#endif
		/* Elevation and bearing of the star */
		theta = dtor(rnd->getDouble()*180.0 - 90.0);
		omega = dtor(rnd->getDouble()*360.0 - 180.0);

		/* Stars coordinates in a 3D space */
		star->x = cos(theta) * sin(omega);
		star->y = sin(omega) * sin(theta);
		star->z = cos(omega);

		/* We set the stars size */
		star->size = rnd->getLong(2 * sp->max_star_size);
		if (star->size < sp->max_star_size)
			star->size = 0;
		else
			star->size -= sp->max_star_size;

		/* We set default values for the XArc lists elements, but offscreen */
		arc->x = xwa.width;
		arc->y = xwa.height;
		if (use3d) {
			arcleft->x = xwa.width;
			arcleft->y = xwa.height;
		}
#if ((USEOLDXARCS == 1) || (ADAPT_ERASE == 1))
		oarc->x = xwa.width;
		oarc->y = xwa.height;
		if (use3d) {
			oarcleft->x = xwa.width;
			oarcleft->y = xwa.height;
		}
#endif
		arc->width = 2 + star->size;
		arc->height = 2 + star->size;
		if (use3d) {
			arcleft->width = 2 + star->size;
			arcleft->height = 2 + star->size;
		}
#if ((USEOLDXARCS == 1) || (ADAPT_ERASE == 1))
		oarc->width = 2 + star->size;
		oarc->height = 2 + star->size;
		if (use3d) {
			oarcleft->width = 2 + star->size;
			oarcleft->height = 2 + star->size;
		}
#endif

		arc->angle1 = 0;
		arc->angle2 = 360 * 64;
		if (use3d) {
			arcleft->angle1 = 0;
			arcleft->angle2 = 360 * 64;
		}
#if ((USEOLDXARCS == 1) || (ADAPT_ERASE == 1))
		oarc->angle1 = 0;
		oarc->angle2 = 360 * 64;	/* ie. we draw whole disks:
						 * from 0 to 360 degrees */
		if (use3d) {
			oarcleft->angle1 = 0;
			oarcleft->angle2 = 360 * 64;
		}
#endif
	}

	if (!mono && (Scr[screen].npixels > 2))
		sp->colorp = rnd->getLong(Scr[screen].npixels);
	/* We set up the starfield color */
	if (!use3d && !mono && Scr[screen].npixels > 2)
		sp->color = Scr[screen].pixels[sp->colorp];
	else
		sp->color = WhitePixel(dsp,screen);

#if (ADAPT_ERASE == 1)
	/* We initialize the adaptation code for screen erasing */
	sp->hasbeenchecked = ADAPT_CHECKS * 2;
	sp->rect_time = 0;
	sp->xarc_time = 0;
#endif
	bouboule_initialized=1;

}

/****************/
void
drawbouboule(Window win)
/****************/

{

        StarField *sp = &starfield;
	int         i, diff = 0;
	double      CX, CY, CZ, SX, SY, SZ;
	Star       *star;
	XArc       *arc = NULL, *arcleft = NULL;

	if(!bouboule_initialized)
		return;

#if (ADAPT_ERASE == 1)
	struct timeval tv1;
	struct timeval tv2;

#endif

#if ((USEOLDXARCS == 0) || (ADAPT_ERASE == 1))
	short       x_1, y_1, x_2, y_2;

	/* bounding rectangle around the old starfield,
	 * for erasing with the smallest rectangle
	 * instead of filling the whole screen */
	int         maxdiff = 0;	/* maximal distance between left and right */

	/* star in 3d mode, otherwise 0 */
#endif

#if ((USEOLDXARCS == 0) || (ADAPT_ERASE == 1))
	if (use3d) {
		maxdiff = (int) MAXDIFF;
	}
	x_1 = (int) sp->x.value - (int) sp->sizex.value -
		sp->max_star_size - maxdiff;
	y_1 = (int) sp->y.value - (int) sp->sizey.value -
		sp->max_star_size;
	x_2 = 2 * ((int) sp->sizex.value + sp->max_star_size + maxdiff);
	y_2 = 2 * ((int) sp->sizey.value + sp->max_star_size);
#endif
	/* We make variables vary. */
	sinvary(&sp->thetax);
	sinvary(&sp->thetay);
	sinvary(&sp->thetaz);

	sinvary(&sp->x);
	sinvary(&sp->y);
	if (use3d)
		sinvary(&sp->z);

	/* A little trick to prevent the bouboule from being
	 * bigger than the screen */
	sp->sizex.maximum =
		MIN(((double) sp->width) - sp->x.value,
		    sp->x.value);
	sp->sizex.minimum = sp->sizex.maximum / 3.0;

	/* Another trick to make the ball not too flat */
	sp->sizey.minimum =
		MAX(sp->sizex.value / MAX_SIZEX_SIZEY,
		    sp->sizey.maximum / 3.0);
	sp->sizey.maximum =
		MIN(sp->sizex.value * MAX_SIZEX_SIZEY,
		    MIN(((double) sp->height) - sp->y.value,
			sp->y.value));

	sinvary(&sp->sizex);
	sinvary(&sp->sizey);

	/*
	 * We calculate the rotation matrix values. We just make the
	 * rotation on the fly, without using a matrix.
	 * Star positions are recorded as unit vectors pointing in various
	 * directions. We just make them all rotate.
	 */
	CX = cos(sp->thetax.value);
	SX = sin(sp->thetax.value);
	CY = cos(sp->thetay.value);
	SY = sin(sp->thetay.value);
	CZ = cos(sp->thetaz.value);
	SZ = sin(sp->thetaz.value);

	for (i = 0; i < sp->NbStars; i++) {
		star = &(sp->star[i]);
		arc = &(sp->xarc[i]);
		if (use3d) {
			arcleft = &(sp->xarcleft[i]);
			/* to help the eyes, the starfield is always as wide as */
			/* deep, so .sizex.value can be used. */
			diff = (int) GETZDIFF(sp->sizex.value *
				      ((SY * CX) * star->x + (SX) * star->y +
				       (CX * CY) * star->z) + sp->z.value);
		}
		arc->x = (short) ((sp->sizex.value *
				   ((CY * CZ - SX * SY * SZ) * star->x +
				    (-CX * SZ) * star->y +
				    (SY * CZ + SZ * SX * CY) * star->z) +
				   sp->x.value));
		arc->y = (short) ((sp->sizey.value *
				   ((CY * SZ + SX * SY * CZ) * star->x +
				    (CX * CZ) * star->y +
				    (SY * SZ - SX * CY * CZ) * star->z) +
				   sp->y.value));

		if (use3d) {
			arcleft->x = (short) ((sp->sizex.value *
					((CY * CZ - SX * SY * SZ) * star->x +
					 (-CX * SZ) * star->y +
					 (SY * CZ + SZ * SX * CY) * star->z) +
					       sp->x.value));
			arcleft->y = (short) ((sp->sizey.value *
					((CY * SZ + SX * SY * CZ) * star->x +
					 (CX * CZ) * star->y +
					 (SY * SZ - SX * CY * CZ) * star->z) +
					       sp->y.value));
			arc->x += diff;
			arcleft->x -= diff;
		}
		if (star->size != 0) {
			arc->x -= star->size;
			arc->y -= star->size;
			if (use3d) {
				arcleft->x -= star->size;
				arcleft->y -= star->size;
			}
		}
	}

	/* First, we erase the previous starfield */
	if (PARAM_INSTALLED && use3d)
		// Used to be GREEN -- picked 3 at random
		XSetForeground(dsp,Scr[screen].gc,Scr[screen].pixels[3]);
  	else 		
		XSetForeground(dsp,Scr[screen].gc,BlackPixel(dsp,screen));

#if (ADAPT_ERASE == 1)
	if (sp->hasbeenchecked == 0) {
		/* We just calculate which method is the faster and eventually free
		 * the oldxarc list */
		if (sp->xarc_time >
		    ADAPT_ARC_PREFERED * sp->rect_time) {
			sp->hasbeenchecked = -2;	/* XFillRectangle mode */
			(void) free((void *) sp->oldxarc);
			sp->oldxarc = NULL;
			if (use3d) {
				(void) free((void *) sp->oldxarcleft);
				sp->oldxarcleft = NULL;
			}
		} else {
			sp->hasbeenchecked = -1;	/* XFillArcs mode */
		}
	}
	if (sp->hasbeenchecked == -2) {
		/* Erasing is done with XFillRectangle */
		XFillRectangle(dsp, win, Scr[screen].gc,
			       x_1, y_1, x_2, y_2);
	} else if (sp->hasbeenchecked == -1) {
		/* Erasing is done with XFillArcs */
		XFillArcs(dsp, win, Scr[screen].gc,
			  sp->oldxarc, sp->NbStars);
		if (use3d)
			XFillArcs(dsp, win, Scr[screen].gc,
				  sp->oldxarcleft, sp->NbStars);
	} else {
		long        usec;

		if (sp->hasbeenchecked > ADAPT_CHECKS) {
			GETTIMEOFDAY(&tv1);
			XFillRectangle(dsp, win, Scr[screen].gc,
				       x_1, y_1, x_2, y_2);
			GETTIMEOFDAY(&tv2);
			usec = (tv2.tv_sec - tv1.tv_sec) * 1000000;
			if (usec + tv2.tv_usec - tv1.tv_usec > 0) {
				sp->rect_time += usec + tv2.tv_usec - tv1.tv_usec;
				sp->hasbeenchecked--;
			}
		} else {
			GETTIMEOFDAY(&tv1);
			XFillArcs(dsp, win, Scr[screen].gc,
				  sp->oldxarc, sp->NbStars);
			if (use3d)
				XFillArcs(dsp, win, Scr[screen].gc,
					  sp->oldxarcleft, sp->NbStars);
			GETTIMEOFDAY(&tv2);
			usec = (tv2.tv_sec - tv1.tv_sec) * 1000000;
			if (usec + tv2.tv_usec - tv1.tv_usec > 0) {
				sp->xarc_time += usec + tv2.tv_usec - tv1.tv_usec;
				sp->hasbeenchecked--;
			}
		}
	}
#else
#if (USEOLDXARCS == 1)
	XFillArcs(dsp, win, Scr[screen].gc,
		  sp->oldxarc, sp->NbStars);
	if (use3d)
		XFillArcs(dsp, win, Scr[screen].gc,
			  sp->oldxarcleft, sp->NbStars);
#else
	XFillRectangle(dsp, win, Scr[screen].gc,
		       x_1, y_1, x_2, y_2);
#endif
#endif

	/* Then we draw the new one */
	if (use3d) {
		if (PARAM_INSTALLED)
			XSetFunction(dsp, Scr[screen].gc, GXor);
		// Used to be RED -- picked 4 at random
		XSetForeground(dsp,Scr[screen].gc,Qt::red.pixel()); 
		XFillArcs(dsp, win,Scr[screen].gc, sp->xarc, sp->NbStars); 
		// Used to be BLUE -- picked 5 at random
//		XSetForeground(dsp,Scr[screen].gc,Scr[screen].pixels[5]); 
		XSetForeground(dsp,Scr[screen].gc,Qt::blue.pixel()); 
		XFillArcs(dsp,win, Scr[screen].gc,sp->xarcleft, sp->NbStars);

  		if (PARAM_INSTALLED) 
			XSetFunction(dsp,Scr[screen].gc, GXcopy);
	} else {
 		XSetForeground(dsp, Scr[screen].gc,sp->color);
 		XFillArcs(dsp, win, Scr[screen].gc, sp->xarc,sp->NbStars); 
	}

#if ((USEOLDXARCS == 1) || (ADAPT_ERASE == 1))
#if (ADAPT_ERASE == 1)
	if (sp->hasbeenchecked >= -1) {
		arc = sp->xarc;
		sp->xarc = sp->oldxarc;
		sp->oldxarc = arc;
		if (use3d) {
			arcleft = sp->xarcleft;
			sp->xarcleft = sp->oldxarcleft;
			sp->oldxarcleft = arcleft;
		}
	}
#else
	arc = sp->xarc;
	sp->xarc = sp->oldxarc;
	sp->oldxarc = arc;
	if (use3d) {
		arcleft = sp->xarcleft;
		sp->xarcleft = sp->oldxarcleft;
		sp->oldxarcleft = arcleft;
	}
#endif
#endif

	/* We set up the color for the next drawing */
	if (!use3d && !mono && Scr[screen].npixels > 2 &&
	    (++sp->colorchange >= colorchange)) {
		sp->colorchange = 0;
		if (++sp->colorp >= Scr[screen].npixels)
			sp->colorp = 0;
		sp->color = Scr[screen].pixels[sp->colorp];
	}
}

void
release_bouboule()
{
	if (!bouboule_initialized)
		return;
	else
		bouboule_initialized=0;

//	if (starfield != NULL) {
//		int         screen;

//		for (screen = 0; screen < MI_NUM_SCREENS(mi); screen++) {

//		for (screen = 0; screen < 1; screen++) {

			StarField  *sp = &starfield;
			if (sp->star) {
				(void) free((void *) sp->star);
				sp->star=NULL;
			}
			if (sp->xarc) {
				(void) free((void *) sp->xarc);
				sp->xarc=NULL;
			}

			if (sp->xarcleft) {
				(void) free((void *) sp->xarcleft);
				sp->xarcleft=NULL;
			}
#if ((USEOLDXARCS == 1) || (ADAPT_ERASE == 1))
			if (sp->oldxarc) {
				(void) free((void *) sp->oldxarc);
				sp->oldxarc=NULL;
			}
			if (sp->oldxarcleft) {
				(void) free((void *) sp->oldxarcleft);
				sp->oldxarcleft=NULL;
			}
#endif

			sinfree(&(sp->x));
			sinfree(&(sp->y));
			sinfree(&(sp->z));

			sinfree(&(sp->sizex));
			sinfree(&(sp->sizey));
			sinfree(&(sp->thetax));
			sinfree(&(sp->thetay));
			sinfree(&(sp->thetaz));
//		}
//		(void) free((void *) starfield);
//		starfield = NULL;
//	}

}

//---------------------------------------------------

#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qcolor.h>
#include <qlayout.h>

#include <kbuttonbox.h>
#include <klocale.h>
#include <kconfig.h>
#include <kmessagebox.h>

#include "bouboule.h"
#include "bouboule.moc"
#include "helpers.h"

#undef Below

static kBoubouleSaver *saver = NULL;

void startScreenSaver( Drawable d )
{

	if ( saver )
		return;
	saver = new kBoubouleSaver( d );

}

void stopScreenSaver()
{

	if ( saver )
		delete saver;
	saver = NULL;

}

int setupScreenSaver()
{
	kBoubouleSetup dlg;

	return dlg.exec();
}

//-----------------------------------------------------------------------------

kBoubouleSaver::kBoubouleSaver( Drawable drawable ) : kScreenSaver( drawable )
{
	rnd = new KRandomSequence();
	readSettings();

	colorContext = QColor::enterAllocContext();

	batchcount = numPoints;
	maxsize = pointSize;
	colorchange = MAXCCSPEED-colorCycleDelay;
	use3d = flag_3dmode;

    // Clear to background colour when exposed
    XSetWindowBackground(qt_xdisplay(), mDrawable,
                            BlackPixel(qt_xdisplay(), qt_xscreen()));

	initXLock( mGc );
	initbouboule( mDrawable );

	timer.start( speed );
	connect( &timer, SIGNAL( timeout() ), SLOT( slotTimeout() ) );
}

kBoubouleSaver::~kBoubouleSaver()
{
	timer.stop();
        release_bouboule();
	QColor::leaveAllocContext();
	QColor::destroyAllocContext( colorContext );
	delete rnd; rnd = 0;
}

void kBoubouleSaver::setSpeed( int spd )
{
	timer.stop();

        release_bouboule();
	speed = MAXSPEED - spd;

        initbouboule( mDrawable );

	timer.start( speed );

}

void kBoubouleSaver::setPoints( int p )
{
	numPoints = p;
	timer.stop();
        release_bouboule();
        batchcount = numPoints;
	initbouboule( mDrawable );
	timer.start( speed );
}

void kBoubouleSaver::setSize( int p )
{
	pointSize = p;
	timer.stop();
        release_bouboule();
	maxsize = pointSize;
	initbouboule( mDrawable );
	timer.start( speed );
}

void kBoubouleSaver::setColorCycle( int spd )
{
	timer.stop();

        release_bouboule();
	colorCycleDelay = spd;
	colorchange = MAXCCSPEED - colorCycleDelay;

        initbouboule( mDrawable );

	timer.start( speed );

}

void kBoubouleSaver::set3DMode( bool mode3d )
{
	timer.stop();

        release_bouboule();
	flag_3dmode = use3d = mode3d;
	
        initbouboule( mDrawable );

	timer.start( speed );

}

void kBoubouleSaver::readSettings()
{
    KConfig *config = klock_config();
	config->setGroup( "Settings" );

	speed = MAXSPEED - config->readNumEntry( "Speed", MAXSPEED - DEFSPEED );
	numPoints = config->readNumEntry( "NumPoints", DEFPOINTS );
	pointSize = config->readNumEntry( "PointSize", DEFSIZE );
	colorCycleDelay = config->readNumEntry( "ColorCycleDelay", DEFCCSPEED );
	flag_3dmode = config->readBoolEntry( "3DMode", false );

	delete config;
}

void kBoubouleSaver::slotTimeout()
{
	drawbouboule( mDrawable );
}

//-----------------------------------------------------------------------------

kBoubouleSetup::kBoubouleSetup( QWidget *parent, const char *name )
	: QDialog( parent, name, TRUE )
{
	readSettings();

	setCaption( i18n("Setup Bouboule Screen Saver") );
//	setGeometry (x,0,300,250);

	QLabel *label;
	QPushButton *button;
	QSlider *slider;

	QVBoxLayout *tl = new QVBoxLayout(this, 10, 10);
	QHBoxLayout *tl1 = new QHBoxLayout;
	tl->addLayout(tl1);
	QVBoxLayout *tl11 = new QVBoxLayout(5);
	tl1->addLayout(tl11);		

	label = new QLabel( i18n("Speed:"), this );
	min_size(label);
	tl11->addWidget(label);

	slider = new QSlider(MINSPEED, MAXSPEED, 10, speed, QSlider::Horizontal,
                        this );
	slider->setMinimumSize( 135, 20 );
    slider->setTickmarks(QSlider::Below);
    slider->setTickInterval(10);
	connect( slider, SIGNAL( valueChanged( int ) ), 
		 SLOT( slotSpeed( int ) ) );
	tl11->addWidget(slider);
	tl11->addSpacing(5);

	label = new QLabel( i18n("Number of points:"), this );
	min_size(label);
	tl11->addWidget(label);

	slider = new QSlider(MINPOINTS, MAXPOINTS, 50, numPoints,
                        QSlider::Horizontal, this );
	slider->setMinimumSize( 135, 20 );
    slider->setTickmarks(QSlider::Below);
    slider->setTickInterval(50);
	connect( slider, SIGNAL( valueChanged( int ) ), 
		 SLOT( slotPoints( int ) ) );
	tl11->addWidget(slider);
	tl11->addSpacing(5);

	label = new QLabel( i18n("Point size:"), this );
	min_size(label);
	tl11->addWidget(label);

	slider = new QSlider(MINSIZE, MAXSIZE, 5, pointSize,
                        QSlider::Horizontal, this );
	slider->setMinimumSize( 135, 20 );
    slider->setTickmarks(QSlider::Below);
    slider->setTickInterval(5);
	connect( slider, SIGNAL( valueChanged( int ) ), 
		 SLOT( slotSize( int ) ) );
	tl11->addWidget(slider);
	tl11->addSpacing(5);

	freqlabel = new QLabel( i18n("Color-change frequency:"), this );
	min_size(freqlabel);
	tl11->addWidget(freqlabel);
	freqlabel->setEnabled( !flag_3dmode );

	freqslider = new QSlider(MINCCSPEED, MAXCCSPEED, 10, colorCycleDelay,
                            QSlider::Horizontal, this );
	freqslider->setMinimumSize( 135, 20 );
    freqslider->setTickmarks(QSlider::Below);
    freqslider->setTickInterval(10);
	connect( freqslider, SIGNAL( valueChanged( int ) ), 
		 SLOT( slotColorCycle( int ) ) );
	tl11->addWidget(freqslider);
	tl11->addSpacing(5);
	freqslider->setEnabled( !flag_3dmode );

	QCheckBox *cb = new QCheckBox( i18n("3D mode"), this );
	min_size(cb);
	cb->setChecked( flag_3dmode );
	connect( cb, SIGNAL( toggled( bool ) ), SLOT( slot3DMode( bool ) ) );
	tl11->addWidget(cb);
	tl11->addStretch(1);

	preview = new QWidget( this );
	preview->setFixedSize( 220, 170 );
	preview->setBackgroundColor( black );
	preview->show();    // otherwise saver does not get correct size
	saver = new kBoubouleSaver( preview->winId() );
	tl1->addWidget(preview);

	KButtonBox *bbox = new KButtonBox(this);	
	button = bbox->addButton( i18n("About"));
	connect( button, SIGNAL( clicked() ), SLOT(slotAbout() ) );
	bbox->addStretch(1);

	button = bbox->addButton( i18n("OK"));	
	connect( button, SIGNAL( clicked() ), SLOT( slotOkPressed() ) );

	button = bbox->addButton(i18n("Cancel"));
	connect( button, SIGNAL( clicked() ), SLOT( reject() ) );
	bbox->layout();
	tl->addWidget(bbox);

	tl->freeze();
}

void kBoubouleSetup::readSettings()
{
    KConfig *config = klock_config();
	config->setGroup( "Settings" );

	speed = config->readNumEntry( "Speed", DEFSPEED );
	if ( speed > MAXSPEED )
		speed = MAXSPEED;
	else if ( speed < MINSPEED )
		speed = MINSPEED;

	numPoints = config->readNumEntry( "NumPoints", DEFPOINTS );
	pointSize = config->readNumEntry( "PointSize", DEFSIZE );
	colorCycleDelay = config->readNumEntry( "ColorCycleDelay", DEFSIZE );
	flag_3dmode = config->readBoolEntry( "3DMode", false );

	delete config;
}

void kBoubouleSetup::slotSpeed( int num )
{
	speed = num;

	if ( saver )
		saver->setSpeed( speed );
}

void kBoubouleSetup::slotPoints( int num )
{
	numPoints = num;

	if ( saver )
		saver->setPoints( numPoints );
}

void kBoubouleSetup::slotSize( int num )
{
	pointSize = num;

	if ( saver )
		saver->setSize( pointSize );
}

void kBoubouleSetup::slotColorCycle( int num )
{
	colorCycleDelay = num;

	if ( saver )
		saver->setColorCycle( colorCycleDelay );
}

void kBoubouleSetup::slot3DMode( bool mode )
{
	flag_3dmode = mode;

	if ( saver )
		saver->set3DMode( flag_3dmode );
	freqslider->setEnabled( !mode );
	freqlabel->setEnabled( !mode );
}

void kBoubouleSetup::slotOkPressed()
{
    KConfig *config = klock_config();
    config->setGroup( "Settings" );

    QString sspeed;
    sspeed.setNum( speed );
    config->writeEntry( "Speed", sspeed );

    QString spoints;
    spoints.setNum( numPoints );
    config->writeEntry( "NumPoints", spoints );

    QString ssize;
    ssize.setNum( pointSize );
    config->writeEntry( "PointSize", ssize );

    QString scolorcycle;
    scolorcycle.setNum( colorCycleDelay );
    config->writeEntry( "ColorCycleDelay", scolorcycle );

    config->writeEntry( "3DMode", flag_3dmode );

    config->sync();
    delete config;
    accept();
}

void kBoubouleSetup::slotAbout()
{
	KMessageBox::about(this,
			     i18n("Bouboule v0.1 -- a glob of spheres twisting and changing size\n\nCopyright (c) 1996 by Jeremie PETIT\n\nPorted to kscreensave by Cedric Tefft"));
}


