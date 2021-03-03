#include <stdlib.h>
#include <stdio.h>


#include <kapp.h>
#include <kdebug.h>
#include <kprocess.h>
#include <kglobal.h>
#include <kstddirs.h>


#include "extension.h"
#include <qwindowdefs.h>
#include <qglobal.h>
#include <locale.h>

#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <X11/XKBlib.h>
#include <X11/X.h>

XKBExtension::XKBExtension(Display *d)
{
  // determine display to use
  if (!d)
    d = qt_xdisplay();
  dpy = d;

  qt_input_encoding = XInternAtom( d, "_QT_INPUT_ENCODING", false);
  
  // verify the Xlib has matching XKB extension
  int major = XkbMajorVersion;
  int minor = XkbMinorVersion;
  if (!XkbLibraryVersion(&major, &minor))
    {
      kdDebug() << "Xlib XKB extension does not match" << endl;
      return;
    }
  kdDebug() << "Xlib XKB extension major=" << major << " minor=" << minor << endl;

  // verify the X server has matching XKB extension
  // if yes, the XKB extension is initialized
  int opcode_rtrn;
  int error_rtrn;
  if (!XkbQueryExtension(dpy, &opcode_rtrn, &xkb_opcode, &error_rtrn,
			 &major, &minor))
    {
      kdDebug() << "X server has not matching XKB extension" << endl;
      return;
    }

  kdDebug() << "X server XKB extension major=" << major << " minor=" << minor << endl;
}

XKBExtension::~XKBExtension()
{
    // clear the property. Makes Qt use the locale again for input mapping
    Atom type;
    int format;
    unsigned long  nitems, after = 1;
    unsigned char * data;
    XGetWindowProperty( dpy, qt_xrootwin(), qt_input_encoding, 0, 1024,
				true, XA_STRING, &type, &format, &nitems,
				&after,  &data );
    if( data )
	delete data;
}    
void XKBExtension::setLayout(QString rule, QString model, QString layout, const QString &encoding, unsigned int group)
{
  if (rule.isEmpty() || model.isEmpty() || layout.isEmpty())
    return;

  QString exe = KGlobal::dirs()->findExe("setxkbmap");
  if (exe.isEmpty())
    return;

  KProcess *p = new KProcess;
  *p << exe;
  *p << "-rules" << rule;
  *p << "-model" << model;
  *p << "-layout" << layout;
  
  p->start(KProcess::Block);

  XkbLockGroup( dpy, XkbUseCoreKbd, group );
//   XkbStateRec rec;
//   XkbGetState( dpy, XkbUseCoreKbd, &rec );
//   qDebug( "keyboard state is: group: %d, lock %d", (int)rec.group, (int)rec.locked_group);

#if QT_VERSION < 230
  const char * enc = encoding.latin1();
  XChangeProperty( dpy, qt_xrootwin(), qt_input_encoding, XA_STRING, 8, 
		   PropModeReplace, (unsigned char *)enc, strlen(enc) );
#endif
}

