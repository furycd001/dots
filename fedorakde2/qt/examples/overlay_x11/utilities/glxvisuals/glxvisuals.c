
/* Copyright (c) Mark J. Kilgard, 1996. */

/* This program is freely distributable without licensing fees 
   and is provided without guarantee or warrantee expressed or 
   implied. This program is -not- in the public domain. */

#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <GL/glx.h>

static char *ClassOf(int c);
static char *Format(int n, int w);

void
main(int argc, char *argv[])
{
  Display *dpy;
  XVisualInfo match, *visualList, *vi, *visualToTry;
  int errorBase, eventBase, major, minor, found;
  int glxCapable, bufferSize, level, renderType, doubleBuffer, stereo,
    auxBuffers, redSize, greenSize, blueSize, alphaSize, depthSize,
    stencilSize, acRedSize, acGreenSize, acBlueSize, acAlphaSize;

  dpy = XOpenDisplay(NULL);
  if (!dpy) {
    fprintf(stderr, "Could not connect to %s.\n", XDisplayName(NULL));
    exit(1);
  }
  if (glXQueryExtension(dpy, &errorBase, &eventBase) == False) {
    fprintf(stderr, "OpenGL not supported by X server.\n");
    exit(1);
  }

  glXQueryVersion(dpy, &major, &minor);
  printf("display: %s\n", XDisplayName(NULL));
  printf("using GLX version: %d.%d\n\n", major, minor);

  match.screen = DefaultScreen(dpy);
  visualList = XGetVisualInfo(dpy, VisualScreenMask, &match, &found);

  printf("   visual     bf lv rg d st  r  g  b a   ax dp st accum buffs\n");
  printf(" id dep cl    sz l  ci b ro sz sz sz sz  bf th cl  r  g  b  a\n");
  printf("-------------------------------------------------------------\n");

  visualToTry = NULL;
  for(vi = visualList; found > 0; found--, vi++) {
    glXGetConfig(dpy, vi, GLX_USE_GL, &glxCapable);
    if (glxCapable) {
      printf("0x%x %2d %s", vi->visualid, vi->depth, ClassOf(vi->class));
      glXGetConfig(dpy, vi, GLX_BUFFER_SIZE, &bufferSize);
      glXGetConfig(dpy, vi, GLX_LEVEL, &level);
      glXGetConfig(dpy, vi, GLX_RGBA, &renderType);
      glXGetConfig(dpy, vi, GLX_DOUBLEBUFFER, &doubleBuffer);
      glXGetConfig(dpy, vi, GLX_STEREO, &stereo);
      glXGetConfig(dpy, vi, GLX_AUX_BUFFERS, &auxBuffers);
      glXGetConfig(dpy, vi, GLX_RED_SIZE, &redSize);
      glXGetConfig(dpy, vi, GLX_GREEN_SIZE, &greenSize);
      glXGetConfig(dpy, vi, GLX_BLUE_SIZE, &blueSize);
      glXGetConfig(dpy, vi, GLX_ALPHA_SIZE, &alphaSize);
      glXGetConfig(dpy, vi, GLX_DEPTH_SIZE, &depthSize);
      glXGetConfig(dpy, vi, GLX_STENCIL_SIZE, &stencilSize);
      glXGetConfig(dpy, vi, GLX_ACCUM_RED_SIZE, &acRedSize);
      glXGetConfig(dpy, vi, GLX_ACCUM_GREEN_SIZE, &acGreenSize);
      glXGetConfig(dpy, vi, GLX_ACCUM_BLUE_SIZE, &acBlueSize);
      glXGetConfig(dpy, vi, GLX_ACCUM_ALPHA_SIZE, &acAlphaSize);
      printf("    %2s %2s %1s  %1s  %1s ",
        Format(bufferSize, 2), Format(level, 2),
        renderType ? "r" : "c",
	doubleBuffer ? "y" : ".", 
	stereo ? "y" : ".");
      printf("%2s %2s %2s %2s ",
        Format(redSize, 2), Format(greenSize, 2),
	Format(blueSize, 2), Format(alphaSize, 2));
      printf("%2s %2s %2s %2s %2s %2s %2s",
        Format(auxBuffers, 2), Format(depthSize, 2), Format(stencilSize, 2),
        Format(acRedSize, 2), Format(acGreenSize, 2),
        Format(acBlueSize, 2), Format(acAlphaSize, 2));
      printf("\n");
      visualToTry = vi;
    }
  }

  if (visualToTry) {
    GLXContext context;
    Window window;
    Colormap colormap;
    XSetWindowAttributes swa;

    context = glXCreateContext(dpy, visualToTry, 0, GL_TRUE);
    colormap = XCreateColormap(dpy,
      RootWindow(dpy, visualToTry->screen),
      visualToTry->visual, AllocNone);
    swa.colormap = colormap;
    swa.border_pixel = 0;
    window = XCreateWindow(dpy, RootWindow(dpy, visualToTry->screen), 0, 0, 100, 100,
      0, visualToTry->depth, InputOutput, visualToTry->visual,
      CWBorderPixel | CWColormap, &swa);
    glXMakeCurrent(dpy, window, context);
    printf("\n");
    printf("OpenGL vendor string: %s\n", glGetString(GL_VENDOR));
    printf("OpenGL renderer string: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version string: %s\n", glGetString(GL_VERSION));
    if (glXIsDirect(dpy, context))
      printf("direct rendering: supported\n");
    printf( "GL extensions: '%s'\n\n", glGetString(GL_EXTENSIONS) );
#if defined(GLX_VERSION_1_1)
    printf( "GLX extensions: '%s'\n\n", glXQueryExtensionsString( dpy, visualToTry->screen ) );
#endif

  } else
    printf("No GLX-capable visuals!\n");
  XFree(visualList);
}

static char *
ClassOf(int c)
{
  switch (c) {
  case StaticGray:   return "sg";
  case GrayScale:    return "gs";
  case StaticColor:  return "sc";
  case PseudoColor:  return "pc";
  case TrueColor:    return "tc";
  case DirectColor:  return "dc";
  default:           return "??";
  }
}

static char *
Format(int n, int w)
{
  static char buffer[256];
  static int bufptr;
  char *buf;

  if (bufptr >= sizeof(buffer) - w)
    bufptr = 0;
  buf = buffer + bufptr;
  if (n == 0)
    sprintf(buf, "%*s", w, ".");
  else
    sprintf(buf, "%*d", w, n);
  bufptr += w + 1;
  return buf;
}
