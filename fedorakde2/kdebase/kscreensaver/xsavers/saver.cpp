#include <kapp.h>
#include <kglobal.h>
#include <kprocess.h>
#include <klocale.h>
#include <kstddirs.h>
#include <X11/Xlib.h>

#include "saver.h"
#include "saver.moc"

//-----------------------------------------------------------------------------
kScreenSaver::kScreenSaver(Drawable drawable) : QObject()
{
	Window root;
	int ai;
	unsigned int au;

	mDrawable = drawable;
	mGc = XCreateGC(qt_xdisplay(), mDrawable, 0, 0);
	XGetGeometry(qt_xdisplay(), mDrawable, &root, &ai, &ai,
		&mWidth, &mHeight, &au, &au); 
}

kScreenSaver::~kScreenSaver()
{
	XFreeGC(qt_xdisplay(), mGc);
}

//-----------------------------------------------------------------------------


