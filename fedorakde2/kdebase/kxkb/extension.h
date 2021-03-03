#ifndef __EXTENSION_H__
#define __EXTENSION_H__


#include <X11/Xlib.h>
#define explicit int_explicit        // avoid compiler name clash in XKBlib.h
#include <X11/XKBlib.h> 


class XKBExtension
{

public:

  XKBExtension(Display *display=0);
    ~XKBExtension();
    
  void setLayout(QString rule, QString model, QString layout, const QString &encoding, unsigned int group);

private:

  Display *dpy;
    Atom qt_input_encoding;
    int xkb_opcode;
  
};


#endif
