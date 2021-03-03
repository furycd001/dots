
/* Copyright (c) Mark J. Kilgard, 1996. */

/* This program is freely distributable without licensing fees 
   and is provided without guarantee or warrantee expressed or 
   implied. This program is -not- in the public domain. */

/* compile: cc -o sovinfo sovinfo.c sovLayerUtil.c -lX11 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sovLayerUtil.h"

int
main(int argc, char *argv[])
{
  Display *dpy;
  char *display_name, *arg, *class;
  sovVisualInfo template, *lvinfo;
  int nVisuals, i, overlaysOnly = 0;

  display_name = NULL;
  for (i = 1; i < argc; i++) {
    arg = argv[i];
    if (!strcmp(arg, "-display")) {
      if (++i >= argc) {
        fprintf(stderr, "sovinfo: missing argument to -display\n");
        exit(1);
      }
      display_name = argv[i];
    } else if (!strcmp(arg, "-overlays_only")) {
      overlaysOnly = 1;
    } else {
      fprintf(stderr,
        "usage: sovinfo [-display dpy] [-overlays_only]\n");
      exit(1);
    }
  }
  dpy = XOpenDisplay(display_name);
  if (dpy == NULL) {
    fprintf(stderr, "sovinfo: cannot open display %s\n",
      XDisplayName(NULL));
    exit(1);
  }
  lvinfo = sovGetVisualInfo(dpy, 0L, &template, &nVisuals);
  for (i = 0; i < nVisuals; i++) {
    if (!overlaysOnly || lvinfo[i].layer > 0) {
      printf("  Visual ID: 0x%x\n", lvinfo[i].vinfo.visualid);
      printf("    screen: %d\n", lvinfo[i].vinfo.screen);
      printf("    depth: %d\n", lvinfo[i].vinfo.depth);
      switch (lvinfo[i].vinfo.class) {
      case StaticGray:
        class = "StaticGray";
        break;
      case GrayScale:
        class = "GrayScale";
        break;
      case StaticColor:
        class = "StaticColor";
        break;
      case PseudoColor:
        class = "PseudoColor";
        break;
      case TrueColor:
        class = "TrueColor";
        break;
      case DirectColor:
        class = "DirectColor";
        break;
      default:
        class = "Unknown";
        break;
      }
      printf("    class: %s\n", class);
      switch (lvinfo[i].type) {
      case None:
        printf("    transparent type: None\n");
        break;
      case TransparentPixel:
        printf("    transparent type: TransparentPixel\n");
        printf("    pixel value: %d\n", lvinfo[i].value);
        break;
      case TransparentMask:
        printf("    transparent type: TransparentMask\n");
        printf("    transparency mask: %0x%x\n", lvinfo[i].value);
        break;
      default:
        printf("    transparent type: Unknown or invalid\n");
        break;
      }
      printf("    layer: %d\n", lvinfo[i].layer);
    }
  }
  return 0;
}
